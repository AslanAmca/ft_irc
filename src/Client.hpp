/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 18:33:43 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/24 15:08:30 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctime>

// Forward Declaration
class Channel;

class Client
{
private:
	int _fd;
	std::string _hostname;
	std::string _text;
	std::string _nickname;
	std::string _username;
	std::string _realname;

	bool _isPasswordEntered;
	bool _isUserEntered;
	bool _isNickEntered;
	bool _isRegistered;

	bool _isInvisible;

	std::time_t _connectTime;

	std::vector<Channel *> _channels;

public:
	Client(int clientFd, std::string hostname);
	~Client();

	int getFd() const;
	std::string &getText();
	std::string &getNickname();
	std::string &getUsername();
	std::string &getRealname();
	std::string getHostname();
	std::string getIdentity();
	std::time_t getConnectTime();

	bool &getPasswordEntered();
	bool &getUserEntered();
	bool &getNickEntered();
	bool &getRegistered();

	bool isInvisible();
	void setInvisible(bool invisible);
	std::string getModes();

	void sendMessageToSelf(std::string message);

	bool isOnAnyChannel() const;
	bool isOnChannel(std::string channelName) const;
	bool isChannelLimitFull() const;
	std::vector<Channel *> getChannels();
	void joinChannel(Channel *channel);
	void leaveChannel(Channel *channel);
	std::string getVisibleChannelForOtherClient(Client *otherClient);
};

#endif