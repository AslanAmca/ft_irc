/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 06:32:23 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/23 19:05:58 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

void Command::nick(Server *server, Client *client, std::string parameter)
{
	std::string command = "NICK";

	if (!client->getPasswordEntered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	std::istringstream stream(parameter);
	std::string parameterNickname;
	std::string temp;

	stream >> parameterNickname >> temp;

	if (parameterNickname.empty())
	{
		client->sendMessageToSelf(ERR_NONICKNAMEGIVEN(client->getIdentity()));
		return;
	}

	if (!temp.empty())
	{
		client->sendMessageToSelf(ERR_TOOMANYPARAMS(command));
		return;
	}

	if (!Utils::validName(parameterNickname, true))
	{
		client->sendMessageToSelf(ERR_ERRONEUSNICKNAME(client->getIdentity(), parameterNickname));
		return;
	}

	if (server->getClientByNickname(parameterNickname) != NULL || Utils::toUpperString(parameterNickname) == "BOT")
	{
		client->sendMessageToSelf(ERR_NICKNAMEINUSE(client->getIdentity(), parameterNickname));
		return;
	}

	std::string oldNickName = client->getNickname();
	client->getNickname() = parameterNickname;

	if (!client->getNickEntered())
	{
		client->sendMessageToSelf(RPL_NICK());
	}
	else
	{
		client->sendMessageToSelf(RPL_NICKCHANGE(oldNickName, parameterNickname));

		std::vector<Channel *> channels = client->getChannels();
		for (size_t i = 0; i < channels.size(); i++)
		{
			channels[i]->broadcastMessage(RPL_NICKCHANGE(oldNickName, parameterNickname), client);
		}
	}

	client->getNickEntered() = true;

	if (client->getUserEntered() && !client->getRegistered())
		client->getRegistered() = true;
}
