/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 21:27:17 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/23 18:15:26 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

static void handleOneChannel(Server *server, Client *client, std::string channelName, std::string text)
{
	std::string command = "PRIVMSG";

	if (!Utils::validChannelName(channelName))
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	if (text.empty())
	{
		std::string param = "text";
		client->sendMessageToSelf(ERR_EMPTY(param));
		return;
	}

	Channel *channel = server->getChannel(channelName);

	if (channel == NULL || channel->isSecretForClient(client))
	{
		client->sendMessageToSelf(ERR_NOSUCHCHANNEL(client->getIdentity(), channelName));
	}
	else if (!channel->isPublic() && !channel->isClientJoined(client))
	{
		client->sendMessageToSelf(ERR_CANNOTSENDTOCHAN(client->getIdentity(), channelName));
	}
	else if (channel->isNoMessageOutside() && !channel->isClientJoined(client))
	{
		client->sendMessageToSelf(ERR_CANNOTSENDTOCHAN(client->getIdentity(), channelName));
	}
	else if (channel->isModerated() && (!channel->isClientModerator(client) && !channel->isClientCanSpeak(client)))
	{
		client->sendMessageToSelf(ERR_CANNOTSENDTOCHAN(client->getIdentity(), channelName));
	}
	else if (channel->isClientBanned(client))
	{
		client->sendMessageToSelf(ERR_CANNOTSENDTOCHAN(client->getIdentity(), channelName));
	}
	else
	{
		channel->broadcastMessage(RPL_PRIVMSG(client->getIdentity(), channelName, text), client);
	}
}

static void handleChannels(Server *server, Client *client, std::string channelSection, std::string text)
{
	std::string command = "PRIVMSG";

	if (channelSection[0] == ',' || channelSection[channelSection.size() - 1] == ',')
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	std::istringstream stream(channelSection);
	std::vector<std::string> channelList;
	std::string channel;
	while (std::getline(stream, channel, ','))
	{
		channelList.push_back(channel);
	}

	std::string duplicateTarget = Utils::getDuplicateValue(channelList);
	if (!duplicateTarget.empty())
	{
		client->sendMessageToSelf(ERR_TOOMANYTARGETS(client->getIdentity(), duplicateTarget));
		return;
	}

	for (size_t i = 0; i < channelList.size(); i++)
	{
		handleOneChannel(server, client, channelList[i], text);
	}
}

static void handleOneUser(Server *server, Client *client, std::string nickname, std::string text)
{
	std::string command = "PRIVMSG";

	if (!Utils::validName(nickname, true))
	{
		client->sendMessageToSelf(ERR_BADNICK(command));
		return;
	}

	if (text.empty())
	{
		std::string param = "text";
		client->sendMessageToSelf(ERR_EMPTY(param));
		return;
	}

	if (Utils::toUpperString(nickname) == "BOT")
	{
		server->handleBot(client, text);
		return;
	}

	Client *user = server->getClientByNickname(nickname);

	if (user == NULL)
	{
		client->sendMessageToSelf(ERR_NOSUCHNICK(client->getIdentity(), nickname));
	}
	else
	{
		user->sendMessageToSelf(RPL_PRIVMSG(client->getIdentity(), nickname, text));
	}
}

static void handleUsers(Server *server, Client *client, std::string nicknameSection, std::string text)
{
	std::string command = "PRIVMSG";

	if (nicknameSection[0] == ',' || nicknameSection[nicknameSection.size() - 1] == ',')
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	std::istringstream stream(nicknameSection);
	std::vector<std::string> nicknameList;
	std::string nickname;
	while (std::getline(stream, nickname, ','))
	{
		nicknameList.push_back(nickname);
	}

	std::string duplicateTarget = Utils::getDuplicateValue(nicknameList);
	if (!duplicateTarget.empty())
	{
		client->sendMessageToSelf(ERR_TOOMANYTARGETS(client->getIdentity(), duplicateTarget));
		return;
	}

	for (size_t i = 0; i < nicknameList.size(); i++)
	{
		handleOneUser(server, client, nicknameList[i], text);
	}
}

void Command::privmsg(Server *server, Client *client, std::string parameter)
{
	std::string command = "PRIVMSG";

	if (!client->getRegistered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	std::istringstream stream(parameter);
	std::string receiverSection;
	std::string textSection;

	stream >> receiverSection >> textSection;

	if (receiverSection.empty())
	{
		client->sendMessageToSelf(ERR_NORECIPIENT(client->getIdentity(), command));
		return;
	}

	if (textSection.empty())
	{
		client->sendMessageToSelf(ERR_NOTEXTTOSEND(client->getIdentity()));
		return;
	}

	if (textSection[0] != ':')
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	std::string text = parameter.substr(parameter.find(':') + 1);

	if (Utils::isMask(receiverSection))
	{
		client->sendMessageToSelf(ERR_MASK(command));
	}
	else if (Utils::isNickMask(receiverSection))
	{
		client->sendMessageToSelf(ERR_NICKMASK(command));
	}
	else if (receiverSection[0] == '#' || receiverSection[0] == '&')
	{
		handleChannels(server, client, receiverSection, text);
	}
	else
	{
		handleUsers(server, client, receiverSection, text);
	}
}