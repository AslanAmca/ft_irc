/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   List.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 20:14:54 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/17 05:31:27 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

static void handleOneChannel(Server *server, Client *client, std::string channelName)
{
	if (!Utils::validChannelName(channelName))
	{
		client->sendMessageToSelf(ERR_BADCHANMASK(client->getIdentity(), channelName));
		return;
	}

	Channel *channel = server->getChannel(channelName);

	if (channel == NULL)
		return;

	if (channel->isClientJoined(client) || channel->isPublic())
	{
		client->sendMessageToSelf(RPL_LIST(client->getIdentity(), channel->getName(), channel->getVisibleClientCountForClient(client), channel->getTopic()));
	}
	else if (channel->isPrivate())
	{
		client->sendMessageToSelf(RPL_LIST(client->getIdentity(), "Prv", channel->getVisibleClientCountForClient(client), ""));
	}
}

void Command::list(Server *server, Client *client, std::string parameter)
{
	std::string command = "LIST";

	if (!client->getRegistered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	if (parameter.empty())
	{
		std::vector<Channel *> channels = server->getChannels();

		client->sendMessageToSelf(RPL_LISTSTART(client->getIdentity()));

		for (size_t i = 0; i < channels.size(); i++)
		{
			handleOneChannel(server, client, channels[i]->getName());
		}

		client->sendMessageToSelf(RPL_LISTEND(client->getIdentity()));
		return;
	}

	std::istringstream stream(parameter);
	std::string channelSection;
	std::string temp;

	stream >> channelSection >> temp;

	if (!temp.empty())
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
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

	client->sendMessageToSelf(RPL_LISTSTART(client->getIdentity()));

	for (size_t i = 0; i < channelNameList.size(); i++)
	{
		handleOneChannel(server, client, channelNameList[i]);
	}

	client->sendMessageToSelf(RPL_LISTEND(client->getIdentity()));
}
