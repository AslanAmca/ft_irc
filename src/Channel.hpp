/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 21:45:50 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:10:33 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>

// Forward Declaration
class Client;

class Channel
{
private:
	std::string _name;
	std::string _key;
	std::string _topic;
	size_t _userLimit;			// MODE +l
	bool _isPrivate;			// MODE +p
	bool _isSecret;				// MODE +s
	bool _isInviteOnly;			// MODE +i
	bool _isTopicModeratorOnly; // MODE +t
	bool _isNoMessageOutside;	// MODE +n -> dışarıdan mesaj gönderilemez.
	bool _isModerated;			// MODE +m -> Muted gibi düşünebiliriz, moderatorler ve +v ile yetki verilenler konuşabilir.

	// listeler
	std::vector<Client *> _clients;
	std::vector<Client *> _moderatorClients;
	std::vector<Client *> _speakingClients;
	std::vector<Client *> _bannedClients;
	std::vector<Client *> _invitedClients;

public:
	Channel(std::string name, std::string key);
	~Channel();

	std::string getName() const;
	std::string getKey() const;
	std::string getTopic() const;
	std::string getVisibleNicknamesForClient(Client *client);
	std::string getSymbol();
	std::string getModes();
	size_t getLimit() const;
	int getClientCount() const;
	int getModeratorCount() const;

	void setIsPrivate(bool isPrivate);
	void setIsSecret(bool isSecret);
	void setIsInviteOnly(bool isInviteOnly);
	void setIsTopicModeratorOnly(bool isTopicModeratorOnly);
	void setIsNoMessageOutside(bool isNoMessageOutside);
	void setIsModerated(bool isModerated);
	void setUserLimit(size_t limit);
	void setTopic(std::string topic);
	void setKey(std::string key);

	bool isSecretForClient(Client *client);
	bool isPublic() const;
	bool isPrivate() const;
	bool isSecret() const;
	bool isInviteOnly() const;
	bool isTopicModeratorOnly() const;
	bool isNoMessageOutside() const;
	bool isModerated() const;
	bool isFull() const;
	bool isWrongKey(std::string key);

	bool isClientJoined(Client *client);
	bool isClientModerator(Client *client);
	bool isClientBanned(Client *client);
	bool isClientInvited(Client *client);
	bool isClientCanSpeak(Client *client);

	void broadcastMessage(std::string message);
	void broadcastMessage(std::string message, Client *exceptClient);

	Client *getOldestClient();
	std::string getVisibleClientCountForClient(Client *client);
	void addClient(Client *client);
	void removeClient(Client *client, bool exit);

	void addModerator(Client *client);
	void removeModerator(Client *client);

	void addInvite(Client *client);
	void removeInvite(Client *client);

	void addSpeakingClient(Client *client);
	void removeSpeakingClient(Client *client);

	void addBanned(Client *client);
	void removeBanned(Client *client);
	std::string getBanList();
};

#endif