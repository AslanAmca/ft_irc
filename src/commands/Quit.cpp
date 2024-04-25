/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 22:38:35 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 09:00:15 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

void Command::quit(Server *server, Client *client, std::string parameter)
{
	std::string command = "QUIT";
	std::string quitMessage = "No reason given";

	if (!client->getRegistered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	if (!parameter.empty())
	{
		std::istringstream stream(parameter);
		std::string messageSection;
		stream >> messageSection;

		if (messageSection[0] != ':')
		{
			client->sendMessageToSelf(ERR_FORMAT(command));
			return;
		}

		quitMessage = parameter.substr(parameter.find(':') + 1);
	}

	if (client->isOnAnyChannel())
	{
		std::vector<Channel *> clientChannels = client->getChannels();
		for (size_t i = 0; i < clientChannels.size(); i++)
		{
			Channel *channel = clientChannels[i];
			channel->removeClient(client, true);

			if (channel->getClientCount() == 0)
			{
				server->deleteChannel(channel);
			}
			else
			{
				channel->broadcastMessage(RPL_QUIT(client->getIdentity(), quitMessage));

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

	client->sendMessageToSelf(RPL_QUIT(client->getIdentity(), quitMessage));

	server->deleteClient(client);
}
