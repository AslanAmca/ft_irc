/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/24 14:46:13 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 08:51:27 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

void Command::topic(Server *server, Client *client, std::string parameter)
{
	std::string command = "TOPIC";

	if (!client->getRegistered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	if (parameter.empty())
	{
		client->sendMessageToSelf(ERR_NEEDMOREPARAMS(client->getIdentity(), command));
		return;
	}

	std::istringstream stream(parameter);
	std::string channelName;
	std::string topicSection;

	stream >> channelName >> topicSection;

	if (!Utils::validChannelName(channelName))
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	if (topicSection.empty())
	{
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
			std::string topic = channel->getTopic();
			if (topic.empty())
				client->sendMessageToSelf(RPL_NOTOPIC(client->getIdentity(), channelName));
			else
				client->sendMessageToSelf(RPL_TOPIC(client->getIdentity(), channelName, topic));
		}
		return;
	}

	if (topicSection[0] != ':')
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	std::string topic = parameter.substr(parameter.find(':') + 1);

	Channel *channel = server->getChannel(channelName);

	if (channel == NULL || channel->isSecretForClient(client))
	{
		client->sendMessageToSelf(ERR_NOSUCHCHANNEL(client->getIdentity(), channelName));
	}
	else if (!channel->isClientJoined(client))
	{
		client->sendMessageToSelf(ERR_NOTONCHANNEL(client->getIdentity(), channelName));
	}
	else if (channel->isTopicModeratorOnly() && !channel->isClientModerator(client))
	{
		client->sendMessageToSelf(ERR_CHANOPRIVSNEEDED(client->getIdentity(), channelName));
	}
	else
	{
		channel->setTopic(topic);

		client->sendMessageToSelf(RPL_TOPIC(client->getIdentity(), channelName, topic));

		channel->broadcastMessage(RPL_TOPIC_CHANNEL(client->getIdentity(), channelName, topic));
	}
}
