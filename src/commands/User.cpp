/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 07:05:29 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 09:06:18 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"
#include <iostream>

void Command::user(Server *server, Client *client, std::string parameter)
{
	std::string command = "USER";

	if (!client->getPasswordEntered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	if (client->getUserEntered())
	{
		client->sendMessageToSelf(ERR_ALREADYREGISTRED(client->getIdentity()));
		return;
	}

	if (parameter.empty())
	{
		client->sendMessageToSelf(ERR_NEEDMOREPARAMS(client->getIdentity(), command));
		return;
	}

	std::istringstream stream(parameter);
	std::string section;
	std::vector<std::string> sections;

	while (stream >> section)
		sections.push_back(section);

	if (sections.size() < 4)
	{
		client->sendMessageToSelf(ERR_NEEDMOREPARAMS(client->getIdentity(), command));
		return;
	}

	for (size_t i = 0; i < 3; i++)
	{
		if (sections[i].find(':') != std::string::npos)
		{
			client->sendMessageToSelf(ERR_FORMAT(client->getIdentity()));
			return;
		}
	}

	if (sections[3][0] != ':')
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	std::string realname = parameter.substr(parameter.find(':') + 1);

	if (realname.empty())
	{
		std::string param = "realname";
		client->sendMessageToSelf(ERR_EMPTY(param));
		return;
	}

	std::string username = sections[0];

	if (!Utils::validName(username, false))
	{
		client->sendMessageToSelf(ERR_ERRONEUSUSERNAME(username));
		return;
	}

	if (server->getClientByUsername(username) != NULL)
	{
		client->sendMessageToSelf(ERR_USERNAMEINUSE(username));
		return;
	}

	client->getUsername() = username;
	client->getRealname() = realname;

	client->getUserEntered() = true;

	if (client->getNickEntered() && !client->getRegistered())
		client->getRegistered() = true;

	client->sendMessageToSelf(RPL_USER());
}
