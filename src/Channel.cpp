/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 22:16:04 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:10:49 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"
#include "Utils.hpp"

Channel::Channel(std::string name, std::string key)
	: _name(name), _key(key),
	  _topic("Welcome to " + name), _userLimit(0),
	  _isPrivate(false), _isSecret(false),
	  _isInviteOnly(false), _isTopicModeratorOnly(false),
	  _isNoMessageOutside(false), _isModerated(false)
{
}

Channel::~Channel()
{
}

std::string Channel::getName() const
{
	return _name;
}

std::string Channel::getKey() const
{
	return _key;
}

std::string Channel::getTopic() const
{
	return _topic;
}

std::string Channel::getVisibleNicknamesForClient(Client *client)
{
	std::string result = "";

	for (size_t i = 0; i < _moderatorClients.size(); i++)
	{
		if (!isClientJoined(client) && _moderatorClients[i]->isInvisible())
			continue;

		result += "@" + _moderatorClients[i]->getNickname() + " ";
	}

	for (size_t i = 0; i < _speakingClients.size(); i++)
	{
		if (!isClientJoined(client) && _speakingClients[i]->isInvisible())
			continue;

		result += "+" + _speakingClients[i]->getNickname() + " ";
	}

	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (!isClientJoined(client) && _clients[i]->isInvisible())
			continue;

		if (isClientModerator(_clients[i]) || isClientCanSpeak(_clients[i]))
			continue;

		result += _clients[i]->getNickname() + " ";
	}

	if (!result.empty())
		result.erase(result.size() - 1);

	return result;
}

std::string Channel::getSymbol()
{
	if (_isSecret)
		return "@";
	else if (_isPrivate)
		return "*";
	else
		return "=";
}

std::string Channel::getModes()
{
	std::string mode = "+";

	if (_isPrivate)
	{
		mode += "p";
	}
	else if (_isSecret)
	{
		mode += "s";
	}

	if (_isInviteOnly)
	{
		mode += "i";
	}

	if (_isTopicModeratorOnly)
	{
		mode += "t";
	}

	if (_isNoMessageOutside)
	{
		mode += "n";
	}

	if (_isModerated)
	{
		mode += "m";
	}

	if (_userLimit > 0)
	{
		mode += "l " + Utils::intToString(_userLimit);
	}

	return mode;
}

size_t Channel::getLimit() const
{
	return _userLimit;
}

int Channel::getClientCount() const
{
	return _clients.size();
}

int Channel::getModeratorCount() const
{
	return _moderatorClients.size();
}

void Channel::setIsPrivate(bool isPrivate)
{
	_isPrivate = isPrivate;

	if (isPrivate)
		_isSecret = false;
}

void Channel::setIsSecret(bool isSecret)
{
	_isSecret = isSecret;

	if (isSecret)
		_isPrivate = false;
}

void Channel::setIsInviteOnly(bool isInviteOnly)
{
	_isInviteOnly = isInviteOnly;
}

void Channel::setIsTopicModeratorOnly(bool isTopicModeratorOnly)
{
	_isTopicModeratorOnly = isTopicModeratorOnly;
}

void Channel::setIsNoMessageOutside(bool isNoMessageOutside)
{
	_isNoMessageOutside = isNoMessageOutside;
}

void Channel::setIsModerated(bool isModerated)
{
	_isModerated = isModerated;
}

void Channel::setUserLimit(size_t limit)
{
	_userLimit = limit;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void Channel::setKey(std::string key)
{
	_key = key;
}

bool Channel::isTopicModeratorOnly() const
{
	return _isTopicModeratorOnly;
}

bool Channel::isNoMessageOutside() const
{
	return _isNoMessageOutside;
}

bool Channel::isModerated() const
{
	return _isModerated;
}

bool Channel::isFull() const
{
	// 0 is limitless
	if (_userLimit == 0)
		return false;

	return _clients.size() >= _userLimit;
}

bool Channel::isInviteOnly() const
{
	return _isInviteOnly;
}

bool Channel::isSecret() const
{
	return _isSecret;
}

bool Channel::isPrivate() const
{
	return _isPrivate;
}

bool Channel::isSecretForClient(Client *client)
{
	return isSecret() && (!isClientJoined(client) && !isClientInvited(client));
}

bool Channel::isPublic() const
{
	return !_isPrivate && !_isSecret;
}

bool Channel::isWrongKey(std::string key)
{
	if (_key.empty())
		return false;

	return _key != key;
}

bool Channel::isClientJoined(Client *client)
{
	return std::find(_clients.begin(), _clients.end(), client) != _clients.end();
}

bool Channel::isClientModerator(Client *client)
{
	return std::find(_moderatorClients.begin(), _moderatorClients.end(), client) != _moderatorClients.end();
}

bool Channel::isClientBanned(Client *client)
{
	return std::find(_bannedClients.begin(), _bannedClients.end(), client) != _bannedClients.end();
}

bool Channel::isClientInvited(Client *client)
{
	return std::find(_invitedClients.begin(), _invitedClients.end(), client) != _invitedClients.end();
}

bool Channel::isClientCanSpeak(Client *client)
{
	return std::find(_speakingClients.begin(), _speakingClients.end(), client) != _speakingClients.end();
}

void Channel::broadcastMessage(std::string message)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		_clients[i]->sendMessageToSelf(message);
	}
}

void Channel::broadcastMessage(std::string message, Client *exceptClient)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->getNickname() == exceptClient->getNickname())
			continue;

		_clients[i]->sendMessageToSelf(message);
	}
}

Client *Channel::getOldestClient()
{
	return _clients[0];
}

std::string Channel::getVisibleClientCountForClient(Client *client)
{
	if (isClientJoined(client))
	{
		return Utils::intToString(_clients.size());
	}

	int count = 0;
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (!_clients[i]->isInvisible())
		{
			count++;
		}
	}
	return Utils::intToString(count);
}

void Channel::addClient(Client *client)
{
	_clients.push_back(client);

	// Davetiye ile geldiyse davetliler listesinden kaldır
	removeInvite(client);
}

void Channel::removeClient(Client *client, bool exit)
{
	std::vector<Client *>::iterator it = std::find(_clients.begin(), _clients.end(), client);
	if (it != _clients.end())
	{
		_clients.erase(it);
	}

	// Client moderator ise oradan da çıkar.
	removeModerator(client);

	// Client konuşabilme yetkisi almışsa oradan da çıkar.
	removeSpeakingClient(client);

	// Kişi kanala girdiyse INVITE listesinde olamaz anacak özel bir durum var.
	// INVITE atıldıktan sonra kanala girmeden QUIT atarsa INVITE listesinde kalır.
	// QUIT'in özel durumu için en başta ne olur ne olmaz diye invite listesinde varsa silsin.
	removeInvite(client);

	// Kullanıcı banlandığında kanaldan atılacak. Dolayısıyla PART veya KICK çalışmaz, bu durumda ban listesinden silmeye gerek yok.
	// Kullanıcı server'dan çıkış yaparsa (QUIT veya CTRL + C), bu durumda client nesnesi silinecektir. Bu nedenle ban listesinden kaldırmak gerekir.
	// Eğer kaldırılmazsa, liste içerisinde NULL olur ve çeşitli hatalara neden olabilir.
	if (exit)
	{
		removeBanned(client);
	}
}

void Channel::addModerator(Client *client)
{
	_moderatorClients.push_back(client);
}

void Channel::removeModerator(Client *client)
{
	std::vector<Client *>::iterator it = std::find(_moderatorClients.begin(), _moderatorClients.end(), client);
	if (it != _moderatorClients.end())
	{
		_moderatorClients.erase(it);
	}
}

void Channel::addInvite(Client *client)
{
	std::vector<Client *>::iterator it = std::find(_invitedClients.begin(), _invitedClients.end(), client);
	if (it != _invitedClients.end())
		return;

	_invitedClients.push_back(client);
}

void Channel::removeInvite(Client *client)
{
	std::vector<Client *>::iterator it = std::find(_invitedClients.begin(), _invitedClients.end(), client);
	if (it != _invitedClients.end())
	{
		_invitedClients.erase(it);
	}
}

void Channel::addSpeakingClient(Client *client)
{
	_speakingClients.push_back(client);
}

void Channel::removeSpeakingClient(Client *client)
{
	std::vector<Client *>::iterator it = std::find(_speakingClients.begin(), _speakingClients.end(), client);
	if (it != _speakingClients.end())
	{
		_speakingClients.erase(it);
	}
}

void Channel::addBanned(Client *client)
{
	_bannedClients.push_back(client);
}

void Channel::removeBanned(Client *client)
{
	std::vector<Client *>::iterator it = std::find(_bannedClients.begin(), _bannedClients.end(), client);
	if (it != _bannedClients.end())
	{
		_bannedClients.erase(it);
	}
}

std::string Channel::getBanList()
{
	std::string result = "";

	for (size_t i = 0; i < _bannedClients.size(); i++)
	{
		result += _bannedClients[i]->getNickname() + " ";
	}

	if (!result.empty())
		result.erase(result.size() - 1);

	return result;
}
