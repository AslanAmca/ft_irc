/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/24 13:13:39 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:21:40 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

static void handleOnlyNames(Server *server, Client *client)
{
	std::vector<Channel *> channels = server->getChannels();
	std::vector<Channel *> invisibleChannels;

	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i]->isClientJoined(client) || channels[i]->isPublic())
		{
			client->sendMessageToSelf(RPL_NAMREPLY(client->getIdentity(), channels[i]->getSymbol(), channels[i]->getName(), channels[i]->getVisibleNicknamesForClient(client)));
			client->sendMessageToSelf(RPL_ENDOFNAMES(client->getIdentity(), channels[i]->getName()));
		}
		else
		{
			invisibleChannels.push_back(channels[i]);
		}
	}

	if (invisibleChannels.size() == 0 && server->getNicknamesNotOnChannel().empty())
		return;

	std::string channelSymbol = "*";
	std::string channelName = "*";
	std::string otherNicknames = "";

	for (size_t i = 0; i < invisibleChannels.size(); i++)
	{
		std::string names = invisibleChannels[i]->getVisibleNicknamesForClient(client);

		if (!names.empty())
			otherNicknames += invisibleChannels[i]->getVisibleNicknamesForClient(client) + " ";
	}
	otherNicknames += server->getNicknamesNotOnChannel();

	client->sendMessageToSelf(RPL_NAMREPLY(client->getIdentity(), channelSymbol, channelName, otherNicknames));
	client->sendMessageToSelf(RPL_ENDOFNAMES(client->getIdentity(), channelName));
}

void Command::names(Server *server, Client *client, std::string parameter)
{
	std::string command = "NAMES";

	if (!client->getRegistered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	if (parameter.empty())
	{
		handleOnlyNames(server, client);
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

	for (size_t i = 0; i < channelNameList.size(); i++)
	{
		Channel *channel = server->getChannel(channelNameList[i]);

		if (channel != NULL && (channel->isClientJoined(client) || channel->isPublic()))
		{
			client->sendMessageToSelf(RPL_NAMREPLY(client->getIdentity(), channel->getSymbol(), channelNameList[i], channel->getVisibleNicknamesForClient(client)));
		}

		client->sendMessageToSelf(RPL_ENDOFNAMES(client->getIdentity(), channelNameList[i]));
	}
}