/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Notice.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 21:29:17 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/24 16:28:44 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

static void handleOneChannel(Server *server, Client *client, std::string channelName, std::string text)
{
	if (!Utils::validChannelName(channelName))
		return;

	if (text.empty())
		return;

	Channel *channel = server->getChannel(channelName);

	if (channel == NULL || channel->isSecretForClient(client))
		return;
	if (!channel->isPublic() && !channel->isClientJoined(client))
		return;
	if (channel->isNoMessageOutside() && !channel->isClientJoined(client))
		return;
	if (channel->isModerated() && (!channel->isClientModerator(client) && !channel->isClientCanSpeak(client)))
		return;
	if (channel->isClientBanned(client))
		return;

	channel->broadcastMessage(RPL_NOTICE(client->getIdentity(), channelName, text), client);
}

static void handleChannels(Server *server, Client *client, std::string channelSection, std::string text)
{
	if (channelSection[0] == ',' || channelSection[channelSection.size() - 1] == ',')
		return;

	std::istringstream stream(channelSection);
	std::vector<std::string> channelList;
	std::string channel;
	while (std::getline(stream, channel, ','))
	{
		channelList.push_back(channel);
	}

	std::string duplicateTarget = Utils::getDuplicateValue(channelList);
	if (!duplicateTarget.empty())
		return;

	for (size_t i = 0; i < channelList.size(); i++)
	{
		handleOneChannel(server, client, channelList[i], text);
	}
}

static void handleOneUser(Server *server, Client *client, std::string nickname, std::string text)
{
	if (!Utils::validName(nickname, true))
		return;

	if (text.empty())
		return;

	if (Utils::toUpperString(nickname) == "BOT")
		return;

	Client *user = server->getClientByNickname(nickname);

	if (user == NULL)
		return;

	user->sendMessageToSelf(RPL_NOTICE(client->getNickname(), nickname, text));
}

static void handleUsers(Server *server, Client *client, std::string nicknameSection, std::string text)
{
	if (nicknameSection[0] == ',' || nicknameSection[nicknameSection.size() - 1] == ',')
		return;

	std::istringstream stream(nicknameSection);
	std::vector<std::string> nicknameList;
	std::string nickname;
	while (std::getline(stream, nickname, ','))
	{
		nicknameList.push_back(nickname);
	}

	std::string duplicateTarget = Utils::getDuplicateValue(nicknameList);
	if (!duplicateTarget.empty())
		return;

	for (size_t i = 0; i < nicknameList.size(); i++)
	{
		handleOneUser(server, client, nicknameList[i], text);
	}
}

void Command::notice(Server *server, Client *client, std::string parameter)
{
	if (!client->getRegistered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	std::istringstream stream(parameter);
	std::string receiverSection;
	std::string textSection;

	stream >> receiverSection >> textSection;

	if (receiverSection.empty() || textSection.empty())
		return;

	if (textSection[0] != ':')
		return;

	std::string text = parameter.substr(parameter.find(':') + 1);

	if (Utils::isMask(receiverSection) || Utils::isNickMask(receiverSection))
		return;

	if (receiverSection[0] == '#' || receiverSection[0] == '&')
	{
		handleChannels(server, client, receiverSection, text);
	}
	else
	{
		handleUsers(server, client, receiverSection, text);
	}
}