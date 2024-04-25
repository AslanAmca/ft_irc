/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 15:47:36 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/24 14:37:05 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#define LISTEN_BACKLOG 128
#define MAXEVENTS 128

#include "Utils.hpp"
#include "Client.hpp"
#include "Channel.hpp"

#include <stdexcept>
#include <cstring>
#include <ctime>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// epoll api
#ifdef __linux__
#include <sys/epoll.h>
#endif

// kqueue
#ifdef __APPLE__
#include <sys/event.h>
#include <sys/time.h>
#endif

// Forward Declaration
class Command;

class Server
{
private:
	int _port;
	std::string _password;
	std::time_t _startTime;

	int _serverFd;
	int _multiplexingFd;

	std::vector<Client *> _clients;
	std::vector<Channel *> _channels;

public:
	Server(int port, std::string password);
	~Server();

	void run();
	void createServerSocket();
	void bindAndListen();
	void createMultiplexing();
	void eventLoop();

	void handleClientConnection();
	void handleClientDisconnected(int clientFd);
	void handleLongMessage(Client *client);
	void handleClientMessage(int clientFd);
	void handleCommand(std::string message, Client *client);

	Client *getClientByFd(int clientFd);
	Client *getClientByNickname(std::string nickname);
	Client *getClientByUsername(std::string username);
	std::string getNicknamesNotOnChannel();
	void deleteClient(Client *client);

	std::vector<Channel *> getChannels();
	Channel *getChannel(std::string channelName);
	void addChannel(Channel *channel);
	void deleteChannel(Channel *channel);

	void handleBot(Client *client, std::string text);
};

#endif