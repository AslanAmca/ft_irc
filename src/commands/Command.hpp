/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 06:05:13 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/14 05:56:58 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "../Response.hpp"
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>

// Forward Declaration
class Server;
class Client;

class Command
{
public:
	// HexChat
	static void cap(Client *client, std::string parameter);

	// 4.1 Connection Registration
	static void pass(Client *client, std::string parameter, std::string pass);
	static void nick(Server *server, Client *client, std::string parameter);
	static void user(Server *server, Client *client, std::string parameter);
	static void quit(Server *server, Client *client, std::string parameter);

	// 4.2 Channel operations
	static void join(Server *server, Client *client, std::string parameter);
	static void part(Server *server, Client *client, std::string parameter);
	static void mode(Server *server, Client *client, std::string parameter);
	static void topic(Server *server, Client *client, std::string parameter);
	static void names(Server *server, Client *client, std::string parameter);
	static void list(Server *server, Client *client, std::string parameter);
	static void invite(Server *server, Client *client, std::string parameter);
	static void kick(Server *server, Client *client, std::string parameter);

	// 4.4 Sending messages
	static void privmsg(Server *server, Client *client, std::string parameter);
	static void notice(Server *server, Client *client, std::string parameter);

	// 4.5 User-based queries
	static void whois(Server *server, Client *client, std::string parameter);
};

#endif