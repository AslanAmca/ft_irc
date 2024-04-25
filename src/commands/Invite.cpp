/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/24 21:23:50 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:18:11 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

void Command::invite(Server *server, Client *client, std::string parameter)
{
	std::string command = "INVITE";

	if (!client->getRegistered())
	{;
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	std::istringstream stream(parameter);
	std::string nicknameToInvite;
	std::string channelName;
	std::string temp;

	stream >> nicknameToInvite >> channelName >> temp;

	if (nicknameToInvite.empty() || channelName.empty())
	{
		client->sendMessageToSelf(ERR_NEEDMOREPARAMS(client->getIdentity(), command));
		return;
	}

	if (!temp.empty())
	{
		client->sendMessageToSelf(ERR_TOOMANYPARAMS(command));
		return;
	}

	Channel *channel = server->getChannel(channelName);
	Client *clientToInvite = server->getClientByNickname(nicknameToInvite);

	if (channel == NULL || !channel->isClientJoined(client))
	{
		client->sendMessageToSelf(ERR_NOTONCHANNEL(client->getIdentity(), channelName));
	}
	else if (channel->isInviteOnly() && !channel->isClientModerator(client))
	{
		client->sendMessageToSelf(ERR_CHANOPRIVSNEEDED(client->getIdentity(), channelName));
	}
	else if (clientToInvite == NULL)
	{
		client->sendMessageToSelf(ERR_NOSUCHNICK(client->getIdentity(), nicknameToInvite));
	}
	else if (channel->isClientJoined(clientToInvite))
	{
		client->sendMessageToSelf(ERR_USERONCHANNEL(client->getIdentity(), nicknameToInvite, channelName));
	}
	else if (channel->isClientBanned(clientToInvite))
	{
		client->sendMessageToSelf(ERR_BANNEDFROMCHAN(client->getIdentity(), channelName));
	}
	else
	{
		channel->addInvite(clientToInvite);

		clientToInvite->sendMessageToSelf(RPL_INVITE(client->getIdentity(), nicknameToInvite, channelName));

		client->sendMessageToSelf(RPL_INVITING(client->getIdentity(), nicknameToInvite, channelName));
	}
}
