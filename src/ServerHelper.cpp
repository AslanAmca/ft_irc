/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHelper.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 16:42:56 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/24 15:14:43 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "commands/Command.hpp"

Client *Server::getClientByFd(int clientFd)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->getFd() == clientFd)
			return _clients[i];
	}
	return NULL;
}

Client *Server::getClientByNickname(std::string nickname)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->getNickname() == nickname)
			return _clients[i];
	}
	return NULL;
}

Client *Server::getClientByUsername(std::string username)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->getUsername() == username)
			return _clients[i];
	}
	return NULL;
}

std::string Server::getNicknamesNotOnChannel()
{
	std::string result = "";

	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->getRegistered() && !_clients[i]->isOnAnyChannel() && !_clients[i]->isInvisible())
		{
			result += _clients[i]->getNickname() + " ";
		}
	}

	if (!result.empty())
		result.erase(result.size() - 1);

	return result;
}

void Server::deleteClient(Client *client)
{
	std::vector<Client *>::iterator it = std::find(_clients.begin(), _clients.end(), client);
	if (it != _clients.end())
	{
#ifdef __linux__
		epoll_ctl(_multiplexingFd, EPOLL_CTL_DEL, client->getFd(), NULL);
#endif

#ifdef __APPLE__
		struct kevent deleteEvent;
		EV_SET(&deleteEvent, client->getFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);

		kevent(_multiplexingFd, &deleteEvent, 1, NULL, 0, NULL);
#endif
		close(client->getFd());
		_clients.erase(it);
		delete client;
	}
}

std::vector<Channel *> Server::getChannels()
{
	return _channels;
}

Channel *Server::getChannel(std::string channelName)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (Utils::toUpperString(_channels[i]->getName()) == Utils::toUpperString(channelName))
			return _channels[i];
	}
	return NULL;
}

void Server::addChannel(Channel *channel)
{
	_channels.push_back(channel);
}

void Server::deleteChannel(Channel *channel)
{
	std::vector<Channel *>::iterator it = std::find(_channels.begin(), _channels.end(), channel);
	if (it != _channels.end())
	{
		_channels.erase(it);
		delete channel;
	}
}

void Server::handleBot(Client *client, std::string text)
{
	std::string name = "Bot";
	text = Utils::toUpperString(text);
	Utils::trimBoth(text);

	if (text == "HELP")
	{
		std::string commands = "<help> ping time server me count";
		client->sendMessageToSelf(RPL_PRIVMSG(name, client->getNickname(), commands));
	}
	else if (text == "PING")
	{
		client->sendMessageToSelf(RPL_PRIVMSG(name, client->getNickname(), "PONG"));
	}
	else if (text == "TIME")
	{
		std::time_t rawtime = std::time(NULL);
		std::tm *timeinfo = std::localtime(&rawtime);
		std::string timeString(std::asctime(timeinfo));
		client->sendMessageToSelf(RPL_PRIVMSG(name, client->getNickname(), timeString));
	}
	else if (text == "SERVER")
	{
		std::time_t now = std::time(NULL);
		double diff = difftime(now, _startTime);

		int hour = diff / 3600;
		int minute = (diff - hour * 3600) / 60;
		int second = diff - hour * 3600 - minute * 60;

		std::string message = "ft_irc has been up for: " +
							  Utils::intToString(hour) + " hour " +
							  Utils::intToString(minute) + " minute " +
							  Utils::intToString(second) + " second.";

		client->sendMessageToSelf(RPL_PRIVMSG(name, client->getNickname(), message));
	}
	else if (text == "ME")
	{
		std::time_t now = std::time(NULL);
		double diff = difftime(now, client->getConnectTime());

		int hour = diff / 3600;
		int minute = (diff - hour * 3600) / 60;
		int second = diff - hour * 3600 - minute * 60;

		std::string message = client->getIdentity() + " has been up for: ";
		message += Utils::intToString(hour) + " hour " +
				   Utils::intToString(minute) + " minute " +
				   Utils::intToString(second) + " second.";

		client->sendMessageToSelf(RPL_PRIVMSG(name, client->getNickname(), message));
	}
	else if (text == "COUNT")
	{
		int privateCount = 0, secretCount = 0;
		for (size_t i = 0; i < _channels.size(); i++)
		{
			if (_channels[i]->isPrivate())
				privateCount++;
			else if (_channels[i]->isSecret())
				secretCount++;
		}

		std::string message = "User Count: " + Utils::intToString(_clients.size()) + ". ";
		message += "Channel Count: " + Utils::intToString(_channels.size());
		message += " (+p:" + Utils::intToString(privateCount) + " +s:" + Utils::intToString(secretCount) + ").";

		client->sendMessageToSelf(RPL_PRIVMSG(name, client->getNickname(), message));
	}
	else
	{
		std::string message = "Unknown command. Send HELP message to see commands.";
		client->sendMessageToSelf(RPL_PRIVMSG(name, client->getNickname(), message));
	}
}
