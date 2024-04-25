/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 21:32:53 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:08:59 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"
#include <cstdlib>

static void handleChannelMode(Server *server, Client *client, std::string parameter)
{
	std::string command = "MODE";
	std::istringstream stream(parameter);
	std::string channelName;
	std::string mode;
	std::string param;
	std::vector<std::string> params;

	stream >> channelName >> mode;

	while (stream >> param)
		params.push_back(param);

	if (!Utils::validChannelName(channelName))
	{
		client->sendMessageToSelf(ERR_BADCHANMASK(client->getIdentity(), channelName));
		return;
	}

	Channel *channel = server->getChannel(channelName);

	if (channel == NULL || (!channel->isPublic() && !channel->isClientJoined(client)))
	{
		client->sendMessageToSelf(ERR_NOSUCHCHANNEL(client->getIdentity(), channelName));
		return;
	}

	if (!channel->isClientJoined(client))
	{
		client->sendMessageToSelf(ERR_NOTONCHANNEL(client->getIdentity(), channelName));
		return;
	}

	if (!channel->isClientModerator(client))
	{
		client->sendMessageToSelf(ERR_CHANOPRIVSNEEDED(client->getIdentity(), channelName));
		return;
	}

	if (mode == "+b" && params.size() == 0)
	{
		std::string nicknames = channel->getBanList().empty() ? "No one has been banned" : channel->getBanList();
		client->sendMessageToSelf(RPL_BANLIST(client->getIdentity(), channelName, nicknames));
		client->sendMessageToSelf(RPL_ENDOFBANLIST(client->getIdentity(), channelName));
		return;
	}

	size_t paramModeCount = 0;
	for (size_t i = 1; i < mode.size(); i++)
	{
		if (mode[i] == 'o' || mode[i] == 'v' || mode[i] == 'b')
		{
			paramModeCount++;
		}
		else if ((mode[i] == 'l' || mode[i] == 'k') && mode[0] == '+')
		{
			paramModeCount++;
		}
	}

	if (params.size() != paramModeCount)
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	std::vector< std::pair<char, std::string> > modeAndParam;
	size_t paramIndex = 0;
	for (size_t i = 1; i < mode.size(); i++)
	{
		if (mode[i] == 'o' || mode[i] == 'v' || mode[i] == 'b')
		{
			modeAndParam.push_back(std::make_pair(mode[i], params[paramIndex]));
			paramIndex++;
		}
		else if ((mode[i] == 'l' || mode[i] == 'k') && mode[0] == '+')
		{
			modeAndParam.push_back(std::make_pair(mode[i], params[paramIndex]));
			paramIndex++;
		}
		else
		{
			modeAndParam.push_back(std::make_pair(mode[i], ""));
		}
	}

	for (size_t i = 0; i < modeAndParam.size(); i++)
	{
		char flag = modeAndParam[i].first;
		std::string flagParam = modeAndParam[i].second;

		if (flag == 'p')
		{
			if (mode[0] == '+' && !channel->isPrivate())
			{
				channel->setIsPrivate(true);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+p"));
			}
			else if (mode[0] == '-' && channel->isPrivate())
			{
				channel->setIsPrivate(false);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-p"));
			}
		}
		else if (flag == 's')
		{
			if (mode[0] == '+' && !channel->isSecret())
			{
				channel->setIsSecret(true);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+s"));
			}
			else if (mode[0] == '-' && channel->isSecret())
			{
				channel->setIsSecret(false);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-s"));
			}
		}
		else if (flag == 'i')
		{
			if (mode[0] == '+' && !channel->isInviteOnly())
			{
				channel->setIsInviteOnly(true);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+i"));
			}
			else if (mode[0] == '-' && channel->isInviteOnly())
			{
				channel->setIsInviteOnly(false);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-i"));
			}
		}
		else if (flag == 't')
		{
			if (mode[0] == '+' && !channel->isTopicModeratorOnly())
			{
				channel->setIsTopicModeratorOnly(true);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+t"));
			}
			else if (mode[0] == '-' && channel->isTopicModeratorOnly())
			{
				channel->setIsTopicModeratorOnly(false);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-t"));
			}
		}
		else if (flag == 'n')
		{
			if (mode[0] == '+' && !channel->isNoMessageOutside())
			{
				channel->setIsNoMessageOutside(true);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+n"));
			}
			else if (mode[0] == '-' && channel->isNoMessageOutside())
			{
				channel->setIsNoMessageOutside(false);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-n"));
			}
		}
		else if (flag == 'm')
		{
			if (mode[0] == '+' && !channel->isModerated())
			{
				channel->setIsModerated(true);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+m"));
			}
			else if (mode[0] == '-' && channel->isModerated())
			{
				channel->setIsModerated(false);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-m"));
			}
		}
		else if (flag == 'l')
		{
			if (mode[0] == '+')
			{
				if (flagParam.find_first_not_of("0123456789") != std::string::npos || flagParam[0] == '0')
				{
					std::string message = flagParam + " :Bad Limit";
					client->sendMessageToSelf(message);
					continue;
				}

				channel->setUserLimit(std::atoi(flagParam.c_str()));
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+l " + flagParam));
			}
			else if (mode[0] == '-' && channel->getLimit() > 0)
			{
				channel->setUserLimit(0);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-l"));
			}
		}
		else if (flag == 'k')
		{
			if (mode[0] == '+')
			{
				if (!channel->getKey().empty())
				{
					client->sendMessageToSelf(ERR_KEYSET(client->getIdentity(), channelName));
					continue;
				}

				if (flagParam.find_first_of("#&\a,:ığüşöç ") != std::string::npos)
				{
					client->sendMessageToSelf(ERR_BADCHANKEY(channelName, flagParam));
					continue;
				}

				channel->setKey(flagParam);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+k " + flagParam));
			}
			else if (mode[0] == '-' && !channel->getKey().empty())
			{
				channel->setKey("");
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-k"));
			}
		}
		else if (flag == 'o')
		{
			Client *user = server->getClientByNickname(flagParam);

			if (user == NULL)
			{
				client->sendMessageToSelf(ERR_NOSUCHNICK(client->getIdentity(), flagParam));
				continue;
			}

			if (!channel->isClientJoined(user))
			{
				client->sendMessageToSelf(ERR_USERNOTINCHANNEL(client->getIdentity(), flagParam, channelName));
				continue;
			}

			if (mode[0] == '+' && !channel->isClientModerator(user))
			{
				if (channel->isClientBanned(user))
				{
					std::string message = user->getNickname() + " :Has already been banned";
					client->sendMessageToSelf(message);
					continue;
				}

				channel->addModerator(user);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+o " + flagParam));
			}
			else if (mode[0] == '-' && channel->isClientModerator(user))
			{
				channel->removeModerator(user);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-o " + flagParam));
			}
		}
		else if (flag == 'v')
		{
			Client *user = server->getClientByNickname(flagParam);

			if (user == NULL)
			{
				client->sendMessageToSelf(ERR_NOSUCHNICK(client->getIdentity(), flagParam));
				continue;
			}

			if (!channel->isClientJoined(user))
			{
				client->sendMessageToSelf(ERR_USERNOTINCHANNEL(client->getIdentity(), flagParam, channelName));
				continue;
			}

			if (mode[0] == '+' && !channel->isClientCanSpeak(user))
			{
				if (channel->isClientModerator(user))
					continue;

				if (channel->isClientBanned(user))
				{
					std::string message = user->getNickname() + " :Is banned";
					client->sendMessageToSelf(message);
					continue;
				}

				channel->addSpeakingClient(user);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+v " + flagParam));
			}
			else if (mode[0] == '-' && channel->isClientCanSpeak(user))
			{
				channel->removeSpeakingClient(user);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-v " + flagParam));
			}
		}
		else if (flag == 'b')
		{
			if (Utils::isMask(flagParam))
			{
				client->sendMessageToSelf(ERR_MASK(command));
				continue;
			}

			if (Utils::isNickMask(flagParam))
			{
				client->sendMessageToSelf(ERR_NICKMASK(command));
				continue;
			}

			Client *clientToBan = server->getClientByNickname(flagParam);

			if (clientToBan == NULL)
			{
				client->sendMessageToSelf(ERR_NOSUCHNICK(client->getIdentity(), flagParam));
				continue;
			}

			if (mode[0] == '+' && !channel->isClientBanned(clientToBan))
			{
				channel->addBanned(clientToBan);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "+b " + flagParam));

				if (channel->isClientModerator(clientToBan))
				{
					channel->removeModerator(clientToBan);
					channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-o " + flagParam));
				}
				
				if (channel->isClientCanSpeak(clientToBan))
				{
					channel->removeSpeakingClient(clientToBan);
					channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-v " + flagParam));
				}
			}
			else if (mode[0] == '-' && channel->isClientBanned(clientToBan))
			{
				channel->removeBanned(clientToBan);
				channel->broadcastMessage(RPL_MODE_CHANNEL(client->getIdentity(), channelName, "-b " + flagParam));
			}
		}
		else
		{
			client->sendMessageToSelf(ERR_UNKNOWNMODE(client->getIdentity(), flag, channelName));
		}
	}
}

static void handleUserMode(Server *server, Client *client, std::string nickname, std::string mode)
{
	Client *targetClient = server->getClientByNickname(nickname);

	if (targetClient == NULL)
	{
		client->sendMessageToSelf(ERR_NOSUCHNICK(client->getIdentity(), nickname));
		return;
	}
	if (targetClient->getNickname() != client->getNickname())
	{
		client->sendMessageToSelf(ERR_USERSDONTMATCH_CHANGE(client->getIdentity()));
		return;
	}

	char modeSign = mode[0];
	std::string modeChars = mode.substr(1);

	for (size_t i = 0; i < modeChars.size(); i++)
	{
		if (modeChars[i] == 'i')
		{
			if (modeSign == '+' && !targetClient->isInvisible())
			{
				targetClient->setInvisible(true);
				client->sendMessageToSelf(RPL_MODE_USER(client->getIdentity(), client->getNickname(), "+i"));
			}
			else if (modeSign == '-' && targetClient->isInvisible())
			{
				targetClient->setInvisible(false);
				client->sendMessageToSelf(RPL_MODE_USER(client->getIdentity(), client->getNickname(), "-i"));
			}
		}
		else
		{
			client->sendMessageToSelf(ERR_UMODEUNKNOWNFLAG(client->getIdentity()));
		}
	}
}

static void handleNoMode(Server *server, Client *client, std::string target)
{
	if (target[0] == '#' || target[0] == '&')
	{
		if (!Utils::validChannelName(target))
		{
			client->sendMessageToSelf(ERR_BADCHANMASK(client->getIdentity(), target));
			return;
		}

		Channel *channel = server->getChannel(target);

		if (channel == NULL || (!channel->isPublic() && !channel->isClientJoined(client)))
		{
			client->sendMessageToSelf(ERR_NOSUCHCHANNEL(client->getIdentity(), target));
		}
		else
		{
			std::string modes = channel->getModes() == "+" ? "No mode set yet" : channel->getModes();
			client->sendMessageToSelf(RPL_CHANNELMODEIS(client->getIdentity(), target, modes));
		}
	}
	else
	{
		Client *targetClient = server->getClientByNickname(target);

		if (targetClient == NULL)
		{
			client->sendMessageToSelf(ERR_NOSUCHNICK(client->getIdentity(), target));
		}
		else if (targetClient->getNickname() != client->getNickname())
		{
			client->sendMessageToSelf(ERR_USERSDONTMATCH_VIEW(client->getIdentity()));
		}
		else
		{
			std::string modes = targetClient->getModes().empty() ? "No mode set yet" : targetClient->getModes();
			client->sendMessageToSelf(RPL_UMODEIS(client->getIdentity(), modes));
		}
	}
}

void Command::mode(Server *server, Client *client, std::string parameter)
{
	std::string command = "MODE";

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
	std::string target;
	std::string mode;
	std::string param1;

	stream >> target >> mode >> param1;

	if (target.empty())
	{
		client->sendMessageToSelf(ERR_NEEDMOREPARAMS(client->getIdentity(), command));
		return;
	}

	// Target'ın mode'larını görmek istiyor, değiştirmek istemiyor.
	if (mode.empty())
	{
		handleNoMode(server, client, target);
		return;
	}

	// Mode girilmişse demek ki mode değiştirilmek isteniyor.
	// Mode kısmı + veya - ile başlamalı, diğer karakterlerin içinde + veya - olmamalı.
	if ((mode[0] != '+' && mode[0] != '-') || mode.find_first_of("+-", 1) != std::string::npos || Utils::hasDuplicateChar(mode))
	{
		client->sendMessageToSelf(ERR_FORMAT(command));
		return;
	}

	// Mode var ve format olarak doğru, geri kalan kısmı target'ın türüne göre yorumlamamız gerekiyor.
	if (target[0] == '#' || target[0] == '&')
	{
		handleChannelMode(server, client, parameter);
	}
	else
	{
		if (!param1.empty())
		{
			client->sendMessageToSelf(ERR_TOOMANYPARAMS(command));
			return;
		}

		handleUserMode(server, client, target, mode);
	}
}
