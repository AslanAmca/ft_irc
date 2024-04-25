/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/15 16:49:06 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:31:03 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdlib>
#include <csignal>
#include <iostream>

Server *serverPtr = NULL;

void signalHandler(int signal)
{
	if (serverPtr != NULL)
		serverPtr->~Server();

	std::cerr << "\nSIGINT signal received with CTRL + C. Terminating the program." << std::endl;
	exit(signal);
}

int validatePort(char *port)
{
	std::string portString(port);

	if (portString.find_first_not_of("0123456789") != std::string::npos)
	{
		std::cerr << "Port can only be an unsigned number." << std::endl;
		return -1;
	}

	if (portString[0] == '0')
	{
		std::cerr << "Port number cannot start with 0." << std::endl;
		return -1;
	}

	int portInt = std::atoi(port);

	if (portInt <= 1024)
	{
		std::cerr << "Ports 0-1023 are reserved specifically for system services." << std::endl;
		return -1;
	}

	if (portInt > 65535)
	{
		std::cerr << "Port cannot be greater than 65535." << std::endl;
		return -1;
	}

	return portInt;
}

std::string validatePassword(char *pass)
{
	std::string password(pass);

	if (password.empty())
	{
		std::cerr << "Password cannot be empty." << std::endl;
		return "";
	}

	if (password.find_first_not_of(" \t\v\r\f") == password.npos)
	{
		std::cerr << "Password cannot consist of only spaces." << std::endl;
		return "";
	}

	return password;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "The arguments are wrong.\nPlease use ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	int port = validatePort(argv[1]);
	if (port == -1)
		return 1;

	std::string password = validatePassword(argv[2]);
	if (password.empty())
		return 1;

	signal(SIGINT, signalHandler);
	Server server(port, password);
	serverPtr = &server;

	try
	{
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}