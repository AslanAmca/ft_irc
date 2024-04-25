/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Whois.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/14 05:57:07 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:29:46 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

void Command::whois(Server *server, Client *client, std::string parameter)
{
	std::string command = "WHOIS";

	if (!client->getRegistered())
	{
		client->sendMessageToSelf(ERR_NOTREGISTERED(client->getIdentity()));
		return;
	}

	std::istringstream stream(parameter);
	std::string firstSection;
	std::string secondSection;
	std::string temp;

	stream >> firstSection >> secondSection >> temp;

	if (firstSection.empty())
	{
		client->sendMessageToSelf(ERR_NONICKNAMEGIVEN(client->getIdentity()));
		return;
	}

	std::string serverSection = "";
	std::string nicknameSection = firstSection;

	if (!secondSection.empty())
	{
		if (!temp.empty())
		{
			client->sendMessageToSelf(ERR_FORMAT(command));
			return;
		}

		serverSection = firstSection;
		nicknameSection = secondSection;
	}

	if (!serverSection.empty() && (serverSection != "ft_irc" && serverSection != nicknameSection))
	{
		client->sendMessageToSelf(ERR_NOSUCHSERVER(client->getIdentity(), serverSection));
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
		if (Utils::isNickMask(nicknameList[i]))
		{
			client->sendMessageToSelf(ERR_NICKMASK(command));
			return;
		}

		if (!Utils::validName(nicknameList[i], true))
		{
			client->sendMessageToSelf(ERR_BADNICK(command));
			return;
		}

		Client *clientToWhois = server->getClientByNickname(nicknameList[i]);

		if (clientToWhois == NULL)
		{
			client->sendMessageToSelf(ERR_NOSUCHNICK(client->getIdentity(), nicknameList[i]));
		}
		else
		{
			client->sendMessageToSelf(RPL_WHOISUSER(client->getIdentity(), clientToWhois->getNickname(), clientToWhois->getUsername(), clientToWhois->getHostname(), clientToWhois->getRealname()));
			client->sendMessageToSelf(RPL_WHOISSERVER(client->getIdentity(), clientToWhois->getNickname()));

			std::string channelNames = clientToWhois->getVisibleChannelForOtherClient(client);
			if (!channelNames.empty())
			{
				client->sendMessageToSelf(RPL_WHOISCHANNELS(client->getIdentity(), clientToWhois->getNickname(), clientToWhois->getVisibleChannelForOtherClient(client)));
			}

			client->sendMessageToSelf(RPL_ENDOFWHOIS(client->getIdentity(), clientToWhois->getNickname()));
		}
	}
}