/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 18:34:46 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/24 15:08:39 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Channel.hpp"

Client::Client(int clientFd, std::string hostname)
	: _fd(clientFd), _hostname(hostname),
	  _text(""), _nickname("*"), _username("*"), _realname("*"),
	  _isPasswordEntered(false), _isUserEntered(false), _isNickEntered(false), _isRegistered(false),
	  _isInvisible(false), _connectTime(std::time(NULL))
{
}

Client::~Client()
{
}

int Client::getFd() const
{
	return _fd;
}

std::string &Client::getText()
{
	return _text;
}

bool &Client::getPasswordEntered()
{
	return _isPasswordEntered;
}

bool &Client::getUserEntered()
{
	return _isUserEntered;
}

bool &Client::getNickEntered()
{
	return _isNickEntered;
}

bool &Client::getRegistered()
{
	return _isRegistered;
}

bool Client::isInvisible()
{
	return _isInvisible;
}

void Client::setInvisible(bool invisible)
{
	_isInvisible = invisible;
}

std::string Client::getModes()
{
	std::string mode = "";

	if (_isInvisible)
	{
		mode += "+i";
	}

	return mode;
}

std::string &Client::getNickname()
{
	return _nickname;
}

std::string &Client::getUsername()
{
	return _username;
}

std::string &Client::getRealname()
{
	return _realname;
}

std::string Client::getHostname()
{
	return _hostname;
}

std::string Client::getIdentity()
{
	return _nickname + "!" + _username + "@" + _hostname;
}

std::time_t Client::getConnectTime()
{
	return _connectTime;
}

bool Client::isOnAnyChannel() const
{
	return !_channels.empty();
}

bool Client::isOnChannel(std::string channelName) const
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getName() == channelName)
			return true;
	}
	return false;
}

bool Client::isChannelLimitFull() const
{
	return _channels.size() >= 10;
}

std::vector<Channel *> Client::getChannels()
{
	return _channels;
}

void Client::sendMessageToSelf(std::string message)
{
	message = message + "\r\n";
	send(_fd, message.c_str(), message.size(), 0);
}

void Client::joinChannel(Channel *channel)
{
	_channels.push_back(channel);
}

void Client::leaveChannel(Channel *channel)
{
	std::vector<Channel *>::iterator it = std::find(_channels.begin(), _channels.end(), channel);
	if (it != _channels.end())
	{
		_channels.erase(it);
	}
}

std::string Client::getVisibleChannelForOtherClient(Client *otherClient)
{
	std::string result = "";

	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (!_channels[i]->isPublic() && !_channels[i]->isClientJoined(otherClient))
			continue;

		if (_channels[i]->isClientModerator(this))
		{
			result += "@";
		}
		else if (_channels[i]->isClientCanSpeak(this))
		{
			result += "+";
		}

		result += _channels[i]->getName() + " ";
	}

	if (!result.empty())
		result.erase(result.size() - 1);

	return result;
}
