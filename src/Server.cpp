/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 15:56:15 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/24 14:38:38 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "commands/Command.hpp"

Server::Server(int port, std::string password)
	: _port(port), _password(password), _startTime(std::time(NULL)),
	  _serverFd(0), _multiplexingFd(0)
{
}

Server::~Server()
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		close(_clients[i]->getFd());
		delete _clients[i];
	}
	_clients.clear();

	for (size_t i = 0; i < _channels.size(); i++)
		delete _channels[i];
	_channels.clear();

	if (_multiplexingFd > 0)
		close(_multiplexingFd);

	if (_serverFd > 0)
		close(_serverFd);
}

void Server::run()
{
	createServerSocket();
	bindAndListen();
	createMultiplexing();
	eventLoop();
}

void Server::createServerSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_serverFd == -1)
		throw std::runtime_error("Failed to create server socket.");

	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking mode for server socket.");

	int enableReuse = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) == -1)
		throw std::runtime_error("Failed to set SO_REUSEADDR.");
}

void Server::bindAndListen()
{
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(_port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(_serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		throw std::runtime_error("Failed to bind server socket.");

	if (listen(_serverFd, LISTEN_BACKLOG) == -1)
		throw std::runtime_error("Failed to listen on server socket.");
}

void Server::createMultiplexing()
{
#ifdef __linux__
	_multiplexingFd = epoll_create1(0);
	if (_multiplexingFd == -1)
		throw std::runtime_error("Failed to create epoll instance.");

	struct epoll_event serverEvent;
	serverEvent.data.fd = _serverFd;
	serverEvent.events = EPOLLIN;

	if (epoll_ctl(_multiplexingFd, EPOLL_CTL_ADD, _serverFd, &serverEvent) == -1)
		throw std::runtime_error("Failed to add server socket to epoll instance.");
#endif

#ifdef __APPLE__
	_multiplexingFd = kqueue();
	if (_multiplexingFd == -1)
		throw std::runtime_error("Failed to create kqueue instance.");

	struct kevent serverEvent;
	EV_SET(&serverEvent, _serverFd, EVFILT_READ, EV_ADD, 0, 0, NULL);

	if (kevent(_multiplexingFd, &serverEvent, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("Failed to add server socket to kqueue instance.");
#endif
}

void Server::eventLoop()
{
#ifdef __linux__
	while (true)
	{
		struct epoll_event events[MAXEVENTS];
		int eventCount = epoll_wait(_multiplexingFd, events, MAXEVENTS, -1);
		if (eventCount == -1)
			throw std::runtime_error("Failed to wait for events on epoll instance.");

		for (int i = 0; i < eventCount; i++)
		{
			if (events[i].data.fd == _serverFd)
				handleClientConnection();
			else
				handleClientMessage(events[i].data.fd);
		}
	}
#endif

#ifdef __APPLE__
	while (true)
	{
		struct kevent events[MAXEVENTS];
		int eventCount = kevent(_multiplexingFd, NULL, 0, events, MAXEVENTS, NULL);
		if (eventCount == -1)
			throw std::runtime_error("Failed to wait for events on kqueue instance.");
		for (int i = 0; i < eventCount; i++)
		{
			if (static_cast<int>(events[i].ident) == _serverFd)
				handleClientConnection();
			else
				handleClientMessage(events[i].ident);
		}
	}
#endif
}

void Server::handleClientConnection()
{
	struct sockaddr_in clientAddress;
	int addrlen = sizeof(clientAddress);

	int clientFd = accept(_serverFd, (struct sockaddr *)&clientAddress, (socklen_t *)&addrlen);
	if (clientFd == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == ECONNABORTED || errno == EINTR)
			return;

		throw std::runtime_error("Failed to accept new connection.");
	}

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set non-blocking mode for client socket.");

#ifdef __linux__
	struct epoll_event clientEvent;
	clientEvent.data.fd = clientFd;
	clientEvent.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(_multiplexingFd, EPOLL_CTL_ADD, clientFd, &clientEvent) == -1)
		throw std::runtime_error("Failed to add client socket to epoll instance.");
#endif

#ifdef __APPLE__
	struct kevent clientEvent;
	EV_SET(&clientEvent, clientFd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
	if (kevent(_multiplexingFd, &clientEvent, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("Failed to add client socket to kqueue instance.");
#endif

	Client *newClient = new Client(clientFd, inet_ntoa(clientAddress.sin_addr));
	_clients.push_back(newClient);
	newClient->sendMessageToSelf(RPL_WELCOME());
}

void Server::handleClientDisconnected(int clientFd)
{
	Client *client = getClientByFd(clientFd);
	if (client == NULL)
		return;

	if (client->isOnAnyChannel())
	{
		std::string message = "Disconnected from the server";
		std::vector<Channel *> clientChannels = client->getChannels();

		for (size_t i = 0; i < clientChannels.size(); i++)
		{
			Channel *channel = clientChannels[i];
			channel->removeClient(client, true);

			if (channel->getClientCount() == 0)
			{
				deleteChannel(channel);
			}
			else
			{
				channel->broadcastMessage(RPL_QUIT(client->getIdentity(), message));

				if (channel->getModeratorCount() == 0)
				{
					Client *oldestClient = channel->getOldestClient();
					channel->addModerator(oldestClient);

					std::string source = "ft_irc";
					channel->broadcastMessage(RPL_MODE_CHANNEL_USER(source, channel->getName(), "+o", oldestClient->getNickname()));
				}
			}
		}
	}

	deleteClient(client);
}

void Server::handleLongMessage(Client *client)
{
	client->sendMessageToSelf(ERR_INPUTTOOLONG(client->getIdentity()));

	client->getText() = "";

	char buffer[4096];
	int cleaned_byte = 0;
	while ((cleaned_byte = recv(client->getFd(), buffer, sizeof(buffer), 0)) > 0)
	{
	}

	if (cleaned_byte == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK ||
			errno == ECONNREFUSED || errno == ENOTCONN || errno == EINTR)
			return;

		throw std::runtime_error("Failed to clear socket buffer.");
	}
}

void Server::handleClientMessage(int clientFd)
{
	char buffer[4096];
	memset(&buffer, 0, sizeof(buffer));
	int received_byte = recv(clientFd, buffer, sizeof(buffer), 0);

	if (received_byte == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK ||
			errno == ECONNREFUSED || errno == ENOTCONN || errno == EINTR)
			return;

		throw std::runtime_error("Failed to accept new connection.");
	}
	else if (received_byte == 0)
	{
		handleClientDisconnected(clientFd);
	}
	else
	{
		Client *client = getClientByFd(clientFd);
		if (client == NULL)
			return;

		client->getText().append(buffer);

		if (client->getText().size() > 512)
		{
			handleLongMessage(client);
			return;
		}

		if (client->getText().find('\n') == std::string::npos)
			return;

		std::size_t newlinePos;
		while ((newlinePos = client->getText().find('\n')) != std::string::npos)
		{
			std::string message = client->getText().substr(0, newlinePos);

			client->getText().erase(0, newlinePos + 1);

			if (message[message.size() - 1] == '\r')
				message.erase(message.size() - 1);

			Utils::trimBoth(message);

			if (message.empty())
				continue;

			if (message.find_first_of("\t\v\r\f") != std::string::npos)
			{
				client->sendMessageToSelf(ERR_SPACEONLY());
				continue;
			}

			handleCommand(message, client);

			if (getClientByFd(clientFd) == NULL)
				break;
		}
	}
}

void Server::handleCommand(std::string message, Client *client)
{
	std::string command = message;
	std::string parameter = "";
	size_t spacePostion = message.find(' ');
	if (spacePostion != std::string::npos)
	{
		command = message.substr(0, spacePostion);
		parameter = message.substr(spacePostion + 1);
	}
	command = Utils::toUpperString(command);

	if (command == "CAP" || command == "/CAP")
		Command::cap(client, parameter);
	else if (command == "PASS" || command == "/PASS")
		Command::pass(client, parameter, _password);
	else if (command == "NICK" || command == "/NICK")
		Command::nick(this, client, parameter);
	else if (command == "USER" || command == "/USER")
		Command::user(this, client, parameter);
	else if (command == "QUIT" || command == "/QUIT")
		Command::quit(this, client, parameter);
	else if (command == "JOIN" || command == "/JOIN")
		Command::join(this, client, parameter);
	else if (command == "PART" || command == "/PART")
		Command::part(this, client, parameter);
	else if (command == "MODE" || command == "/MODE")
		Command::mode(this, client, parameter);
	else if (command == "TOPIC" || command == "/TOPIC")
		Command::topic(this, client, parameter);
	else if (command == "NAMES" || command == "/NAMES")
		Command::names(this, client, parameter);
	else if (command == "LIST" || command == "/LIST")
		Command::list(this, client, parameter);
	else if (command == "INVITE" || command == "/INVITE")
		Command::invite(this, client, parameter);
	else if (command == "KICK" || command == "/KICK")
		Command::kick(this, client, parameter);
	else if (command == "PRIVMSG" || command == "/PRIVMSG")
		Command::privmsg(this, client, parameter);
	else if (command == "NOTICE" || command == "/NOTICE")
		Command::notice(this, client, parameter);
	else if (command == "WHOIS" || command == "/WHOIS")
		Command::whois(this, client, parameter);
	else
		client->sendMessageToSelf(ERR_UNKNOWNCOMMAND(client->getIdentity(), command));
}