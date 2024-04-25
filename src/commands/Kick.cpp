/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 00:23:50 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:21:22 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

static void handleKick(Server *server, Client *client, std::string channelName, std::string nickname, std::string comment)
{
	if (!Utils::validChannelName(channelName))
	{
		client->sendMessageToSelf(ERR_BADCHANMASK(client->getIdentity(), channelName));
		return;
	}

	if (!Utils::validName(nickname, true))
	{
		client->sendMessageToSelf(ERR_BADNICK(nickname));
		return;
	}

	Channel *channel = server->getChannel(channelName);
	Client *clientToKicked = server->getClientByNickname(nickname);

	if (channel == NULL || channel->isSecretForClient(client))
	{
		client->sendMessageToSelf(ERR_NOSUCHCHANNEL(client->getIdentity(), channelName));
	}
	else if (!channel->isClientJoined(client))
	{
		client->sendMessageToSelf(ERR_NOTONCHANNEL(client->getIdentity(), channelName));
	}
	else if (!channel->isClientModerator(client))
	{
		client->sendMessageToSelf(ERR_CHANOPRIVSNEEDED(client->getIdentity(), channelName));
	}
	else if (clientToKicked == NULL)
	{
		client->sendMessageToSelf(ERR_NOSUCHNICK(client->getIdentity(), nickname));
	}
	else if (!channel->isClientJoined(clientToKicked))
	{
		client->sendMessageToSelf(ERR_USERNOTINCHANNEL(client->getIdentity(), nickname, channelName));
	}
	else if (client->getNickname() == nickname)
	{
		client->sendMessageToSelf(ERR_CANTKICKYOURSELF(channelName));
	}
	else
	{
		clientToKicked->leaveChannel(channel);
		clientToKicked->sendMessageToSelf(RPL_KICK(client->getIdentity(), channelName, nickname, comment));

		channel->removeClient(clientToKicked, false);
		channel->broadcastMessage(RPL_KICK(client->getIdentity(), channelName, nickname, comment));
	}
}

void Command::kick(Server *server, Client *client, std::string parameter)
{
	std::string command = "KICK";
	std::string comment = client->getNickname();

	if (!client->getRegistered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	std::istringstream stream(parameter);
	std::string channelSection;
	std::string nicknameSection;
	std::string commentSection;

	stream >> channelSection >> nicknameSection >> commentSection;

	if (channelSection.empty() || nicknameSection.empty())
	{
		client->sendMessageToSelf(ERR_NEEDMOREPARAMS(client->getIdentity(), command));
		return;
	}

	if (!commentSection.empty())
	{
		if (commentSection[0] != ':')
		{
			client->sendMessageToSelf(ERR_FORMAT(command));
			return;
		}

		comment = parameter.substr(parameter.find(':') + 1);
	}

	if (channelSection.find(',') == std::string::npos)
	{
		if (nicknameSection[0] == ',' || nicknameSection[nicknameSection.size() - 1] == ',')
		{
			client->sendMessageToSelf(ERR_FORMAT(command));
			return;
		}

		stream.clear();
		stream.str(nicknameSection);

		std::vector<std::string> nicknameList;
		std::string nickname;
		while (std::getline(stream, nickname, ','))
		{
			nicknameList.push_back(nickname);
		}

		for (size_t i = 0; i < nicknameList.size(); i++)
		{
			handleKick(server, client, channelSection, nicknameList[i], comment);
		}
	}
	else
	{
		stream.clear();
		stream.str(channelSection);

		if (channelSection[0] == ',' || channelSection[channelSection.size() - 1] == ',')
		{
			client->sendMessageToSelf(ERR_FORMAT(command));
			return;
		}

		std::vector<std::string> channelList;
		std::string channel;
		while (std::getline(stream, channel, ','))
		{
			channelList.push_back(channel);
		}

		stream.clear();
		stream.str(nicknameSection);

		std::vector<std::string> nicknameList;
		std::string nickname;
		while (std::getline(stream, nickname, ','))
		{
			nicknameList.push_back(nickname);
		}

		if (channelList.size() != nicknameList.size())
		{
			client->sendMessageToSelf(ERR_TOOMANYPARAMS(command));
			return;
		}

		for (size_t i = 0; i < channelList.size(); i++)
		{
			handleKick(server, client, channelList[i], nicknameList[i], comment);
		}
	}
}
