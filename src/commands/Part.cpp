/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/24 15:46:02 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:28:09 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

static void handleClientLeave(Server *server, Client *client, std::string channelName, std::string message)
{
	std::string command = "PART";

	if (!Utils::validChannelName(channelName))
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	Channel *channel = server->getChannel(channelName);

	if (channel == NULL || channel->isSecretForClient(client))
	{
		client->sendMessageToSelf(ERR_NOSUCHCHANNEL(client->getIdentity(), channelName));
	}
	else if (!channel->isClientJoined(client))
	{
		client->sendMessageToSelf(ERR_NOTONCHANNEL(client->getIdentity(), channelName));
	}
	else
	{
		client->leaveChannel(channel);

		if (message.empty())
		{
			client->sendMessageToSelf(RPL_PART(client->getIdentity(), channelName));
		}
		else
		{
			client->sendMessageToSelf(RPL_PART_MESSAGE(client->getIdentity(), channelName, message));
		}

		channel->removeClient(client, false);

		if (channel->getClientCount() == 0)
		{
			server->deleteChannel(channel);
			return;
		}

		if (message.empty())
		{
			channel->broadcastMessage(RPL_PART(client->getIdentity(), channelName));
		}
		else
		{
			channel->broadcastMessage(RPL_PART_MESSAGE(client->getIdentity(), channelName, message));
		}

		if (channel->getModeratorCount() == 0)
		{
			Client *oldestClient = channel->getOldestClient();
			channel->addModerator(oldestClient);

			std::string source = "ft_irc";
			channel->broadcastMessage(RPL_MODE_CHANNEL_USER(source, channel->getName(), "+o", oldestClient->getNickname()));
		}
	}
}

void Command::part(Server *server, Client *client, std::string parameter)
{
	std::string command = "PART";

	if (!client->getRegistered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	std::istringstream stream(parameter);
	std::string channelSection;
	std::string messageSection;
	std::string message = "";

	stream >> channelSection >> messageSection;

	if (channelSection.empty())
	{
		client->sendMessageToSelf(ERR_NEEDMOREPARAMS(client->getIdentity(), command));
		return;
	}

	if (!messageSection.empty())
	{
		if (messageSection[0] != ':')
		{
			client->sendMessageToSelf(ERR_FORMAT(command));
			return;
		}

		message = parameter.substr(parameter.find(':') + 1);
	}

	stream.clear();
	stream.str(channelSection);

	if (channelSection[0] == ',' || channelSection[channelSection.size() - 1] == ',')
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	std::vector<std::string> channelNameList;
	std::string channelName;
	while (std::getline(stream, channelName, ','))
	{
		channelNameList.push_back(channelName);
	}

	for (size_t i = 0; i < channelNameList.size(); i++)
	{
		handleClientLeave(server, client, channelNameList[i], message);
	}
}
