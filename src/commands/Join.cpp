/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 22:27:09 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:20:52 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

static std::map<std::string, std::string> getChannelsWithKey(Client *client, std::string parameter)
{
	std::string command = "JOIN";
	std::map<std::string, std::string> result;
	std::istringstream stream(parameter);
	std::string channelSection;
	std::string keySection;
	std::string temp;

	stream >> channelSection >> keySection >> temp;

	if (channelSection.empty())
	{
		client->sendMessageToSelf(ERR_NEEDMOREPARAMS(client->getIdentity(), command));
		return result;
	}

	if (!temp.empty())
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return result;
	}

	stream.clear();
	stream.str(channelSection);

	if (channelSection[0] == ',' || channelSection[channelSection.size() - 1] == ',')
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return result;
	}

	std::vector<std::string> channelList;
	std::string channel;
	while (std::getline(stream, channel, ','))
	{
		channelList.push_back(channel);
	}

	std::vector<std::string> keyList;
	if (!keySection.empty())
	{
		stream.clear();
		stream.str(keySection);

		if (keySection[0] == ',' || keySection[keySection.size() - 1] == ',')
		{
			client->sendMessageToSelf(ERR_FORMAT(command));
			return result;
		}

		std::string key;
		while (std::getline(stream, key, ','))
		{
			keyList.push_back(key);
		}
	}

	if (keyList.size() > channelList.size())
	{
		client->sendMessageToSelf(ERR_TOOMANYPARAMS(command));
		return result;
	}

	for (size_t i = 0; i < keyList.size(); i++)
	{
		result.insert(std::make_pair(channelList[i], keyList[i]));
	}

	for (size_t i = keyList.size(); i < channelList.size(); i++)
	{
		result.insert(std::make_pair(channelList[i], ""));
	}

	return result;
}

void Command::join(Server *server, Client *client, std::string parameter)
{
	if (!client->getRegistered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	std::map<std::string, std::string> map = getChannelsWithKey(client, parameter);
	if (map.size() == 0)
		return;

	for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); it++)
	{
		std::string channelName = it->first;
		std::string channelKey = it->second;

		if (!Utils::validChannelName(channelName))
		{
			client->sendMessageToSelf(ERR_BADCHANMASK(client->getIdentity(), channelName));
			return;
		}

		if (!channelKey.empty() && channelKey.find_first_of("#&\a,:ığüşöç ") != std::string::npos)
		{
			client->sendMessageToSelf(ERR_BADCHANKEY(channelName, channelKey));
			return;
		}

		Channel *channel = server->getChannel(channelName);

		if (channel == NULL)
		{
			if (client->isChannelLimitFull())
			{
				client->sendMessageToSelf(ERR_TOOMANYCHANNELS(client->getIdentity(), channelName));
				return;
			}

			Channel *channel = new Channel(channelName, channelKey);
			channel->addClient(client);
			channel->addModerator(client);

			server->addChannel(channel);

			client->joinChannel(channel);

			client->sendMessageToSelf(RPL_JOIN(client->getIdentity(), channelName));
			client->sendMessageToSelf(RPL_TOPIC(client->getIdentity(), channelName, channel->getTopic()));
			client->sendMessageToSelf(RPL_NAMREPLY(client->getIdentity(), channel->getSymbol(), channelName, channel->getVisibleNicknamesForClient(client)));
			client->sendMessageToSelf(RPL_ENDOFNAMES(client->getIdentity(), channelName));
		}
		else if (channel->isClientJoined(client))
		{
			client->sendMessageToSelf(ERR_USERONCHANNEL(client->getIdentity(), client->getNickname(), channelName));
		}
		else if (channel->isClientBanned(client))
		{
			client->sendMessageToSelf(ERR_BANNEDFROMCHAN(client->getIdentity(), channelName));
		}
		else if (channel->isSecretForClient(client))
		{
			client->sendMessageToSelf(ERR_NOSUCHCHANNEL(client->getIdentity(), channelName));
		}
		else if (channel->isPrivate() && !channel->isClientInvited(client))
		{
			client->sendMessageToSelf(ERR_INVITEONLYCHAN(client->getIdentity(), channelName));
		}
		else if (channel->isInviteOnly() && !channel->isClientInvited(client))
		{
			client->sendMessageToSelf(ERR_INVITEONLYCHAN(client->getIdentity(), channelName));
		}
		else if (channel->isFull())
		{
			client->sendMessageToSelf(ERR_CHANNELISFULL(client->getIdentity(), channelName));
		}
		else if (channel->isWrongKey(channelKey))
		{
			client->sendMessageToSelf(ERR_BADCHANNELKEY(client->getIdentity(), channelName));
		}
		else if (client->isChannelLimitFull())
		{
			client->sendMessageToSelf(ERR_TOOMANYCHANNELS(client->getIdentity(), channelName));
		}
		else
		{
			channel->addClient(client);

			client->joinChannel(channel);
			client->sendMessageToSelf(RPL_JOIN(client->getIdentity(), channelName));

			if (!channel->getTopic().empty())
			{
				client->sendMessageToSelf(RPL_TOPIC(client->getIdentity(), channelName, channel->getTopic()));
			}

			client->sendMessageToSelf(RPL_NAMREPLY(client->getIdentity(), channel->getSymbol(), channelName, channel->getVisibleNicknamesForClient(client)));
			client->sendMessageToSelf(RPL_ENDOFNAMES(client->getIdentity(), channelName));

			channel->broadcastMessage(RPL_JOIN(client->getIdentity(), channelName), client);
		}
	}
}
