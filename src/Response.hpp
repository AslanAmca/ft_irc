/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 06:12:36 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:34:20 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

// Made-up answers that are not in the RFC
#define RPL_CAPLS() "/CAP LS 302\n/PASS\n/NICK\n/USER\n/QUIT\n/JOIN\n/PART\n/MODE\n/TOPIC\n/NAMES\n/LIST\n/INVITE\n/KICK\n/PRIVMSG\n/NOTICE"
#define RPL_WELCOME() "Welcome to ft_irc server"
#define RPL_PASS() "Password accepted"
#define RPL_USER() "Username and realname accepted. Hostname and servername are ignored for security reasons"
#define ERR_CAPLS() "Only CAP LS 302 command is supported"
#define ERR_TOOMANYPARAMS(command) command + " :Too many parameters"
#define ERR_SPACEONLY() "Message parts are separated only by space(s)(0x20) character"
#define ERR_FORMAT(command) command + " :Wrong format"
#define ERR_USERNAMEINUSE(username) username + " :Username is already in use"
#define ERR_ERRONEUSUSERNAME(username) username + " :Erroneus username"
#define ERR_MASK(command) command + " :Hostmask and servermask extensions is only available to server operators"
#define ERR_NICKMASK(command) command + " :Nickmask is not supported, only nickname should be used"
#define ERR_BADCHANKEY(channel, key) channel + " " + key + " :Bad Channel Key"
#define ERR_BADNICK(nickname) nickname + " :Bad Nickname"
#define ERR_EMPTY(param) param + " :Cannot be empty"

// COMMON
#define ERR_NOTREGISTERED(clientIdentity) ":ft_irc 451 " + clientIdentity + " :You have not registered"
#define ERR_ALREADYREGISTRED(clientIdentity) ":ft_irc 462 " + clientIdentity + " :You may not reregister"
#define ERR_NEEDMOREPARAMS(clientIdentity, command) ":ft_irc 461 " + clientIdentity + " " + command + " :Not enough parameters"
#define ERR_INPUTTOOLONG(clientIdentity) ":ft_irc 417 " + clientIdentity + " :Input line was too long"
#define ERR_UNKNOWNCOMMAND(clientIdentity, command) ":ft_irc 421 " + clientIdentity + " " + command + " :Unknown command"

// PASS
#define ERR_PASSWDMISMATCH(clientIdentity) ":ft_irc 464 " + clientIdentity + " :Password incorrect"

// NICK
#define ERR_NONICKNAMEGIVEN(clientIdentity) ":ft_irc 431 " + clientIdentity + " :No nickname given"
#define ERR_ERRONEUSNICKNAME(clientIdentity, parameterNickname) ":ft_irc 432 " + clientIdentity + " " + parameterNickname + " :Erroneus nickname"
#define ERR_NICKNAMEINUSE(clientIdentity, parameterNickname) ":ft_irc 433 " + clientIdentity + " " + parameterNickname + " :Nickname is already in use"
#define RPL_NICKCHANGE(oldNick, newNick) ":" + oldNick + " NICK " + newNick
#define RPL_NICK() "Nickname accepted"

// QUIT
#define RPL_QUIT(clientIdentity, message) ":" + clientIdentity + " QUIT :" + message

// JOIN
#define RPL_JOIN(clientIdentity, channel) ":" + clientIdentity + " JOIN " + channel

// PART
#define RPL_PART(clientIdentity, channel) ":" + clientIdentity + " PART " + channel
#define RPL_PART_MESSAGE(clientIdentity, channel, message) ":" + clientIdentity + " PART " + channel + " :" + message

// CHANNEL
#define ERR_NOSUCHCHANNEL(clientIdentity, channel) ":ft_irc 403 " + clientIdentity + " " + channel + " :No such channel"
#define ERR_BADCHANMASK(clientIdentity, channel) ":ft_irc 476 " + clientIdentity + " " + channel + " :Bad Channel Mask"
#define ERR_TOOMANYCHANNELS(clientIdentity, channel) ":ft_irc 405 " + clientIdentity + " " + channel + " :You have joined too many channels"
#define ERR_NOTONCHANNEL(clientIdentity, channel) ":ft_irc 442 " + clientIdentity + " " + channel + " :You’re not on that channel"
#define ERR_USERONCHANNEL(clientIdentity, parameterNickname, channel) ":ft_irc 443 " + clientIdentity + " " + parameterNickname + " " + channel + " :is already on channel"
#define ERR_CHANNELISFULL(clientIdentity, channel) ":ft_irc 471 " + clientIdentity + " " + channel + " :Cannot join channel (+l)"
#define ERR_INVITEONLYCHAN(clientIdentity, channel) ":ft_irc 473 " + clientIdentity + " " + channel + " :Cannot join channel (+i)"
#define ERR_BANNEDFROMCHAN(clientIdentity, channel) ":ft_irc 474 " + clientIdentity + " " + channel + " :Cannot join channel (+b)"
#define ERR_BADCHANNELKEY(clientIdentity, channel) ":ft_irc 475 " + clientIdentity + " " + channel + " :Cannot join channel (+k)"
#define ERR_CHANOPRIVSNEEDED(clientIdentity, channel) ":ft_irc 482 " + clientIdentity + " " + channel + " :You’re not channel operator"

// TOPIC
#define RPL_TOPIC(clientIdentity, channel, topic) ":ft_irc 332 " + clientIdentity + " " + channel + " :" + topic
#define RPL_NOTOPIC(clientIdentity, channel) ":ft_irc 331 " + clientIdentity + " " + channel + " :No topic is set"
#define RPL_TOPIC_CHANNEL(clientIdentity, channel, topic) ":" + clientIdentity + " TOPIC " + channel + " :" + topic

// INVITE
#define ERR_NOSUCHNICK(clientIdentity, parameterNickname) ":ft_irc 401 " + clientIdentity + " " + parameterNickname + " :No such nick/channel"
#define RPL_INVITING(clientIdentity, parameterNickname, channel) ":ft_irc 341 " + clientIdentity + " " + parameterNickname + " " + channel
#define RPL_INVITE(clientIdentity, parameterNickname, channel) ":" + clientIdentity + " INVITE " + parameterNickname + " " + channel

// NAMES
#define RPL_NAMREPLY(clientIdentity, channelSymbol, channel, nicknameList) ":ft_irc 353 " + clientIdentity + " " + channelSymbol + " " + channel + " :" + nicknameList
#define RPL_ENDOFNAMES(clientIdentity, channel) ":ft_irc 366 " + clientIdentity + " " + channel + " :End of /NAMES list"

// LIST
#define RPL_LISTSTART(clientIdentity) ":ft_irc 321 " + clientIdentity + " Channel :Users Name"
#define RPL_LIST(clientIdentity, channel, visibleClientCount, topic) ":ft_irc 322 " + clientIdentity + " " + channel + " " + visibleClientCount + " :" + topic
#define RPL_LISTEND(clientIdentity) ":ft_irc 323 " + clientIdentity + " :End of /LIST"

// KICK
#define ERR_USERNOTINCHANNEL(clientIdentity, nickname, channel) ":ft_irc 441 " + clientIdentity + " " + nickname + " " + channel + " :They aren’t on that channel"
#define ERR_CANTKICKYOURSELF(channel) "You can't kick yourself of the " + channel
#define RPL_KICK(clientIdentity, channel, nickname, comment) ":" + clientIdentity + " KICK " + channel + " " + nickname + " :" + comment

// PRIVMSG && NOTICE
#define ERR_NORECIPIENT(clientIdentity, command) ":ft_irc 411 " + clientIdentity + " :No recipient given (" + command + ")"
#define ERR_NOTEXTTOSEND(clientIdentity) ":ft_irc 412 " + clientIdentity + " :No text to send"
#define ERR_TOOMANYTARGETS(clientIdentity, target) ":ft_irc 407 " + clientIdentity + " " + target + " :Duplicate recipients. No message delivered"
#define ERR_CANNOTSENDTOCHAN(clientIdentity, channel) ":ft_irc 404 " + clientIdentity + " " + channel + " :Cannot send to channel"
#define RPL_PRIVMSG(clientIdentity, receiver, message) ":" + clientIdentity + " PRIVMSG " + receiver + " :" + message
#define RPL_NOTICE(clientIdentity, receiver, message) ":" + clientIdentity + " NOTICE " + receiver + " :" + message

// WHOIS
#define ERR_NOSUCHSERVER(clientIdentity, servername) ":ft_irc 402 " + clientIdentity + " " + servername + " :No such server"
#define RPL_WHOISUSER(clientIdentity, nickname, username, hostname, realname) ":ft_irc 311 " + clientIdentity + " " + nickname + " " + username + " " + hostname + " * :" + realname
#define RPL_WHOISSERVER(clientIdentity, nickname) ":ft_irc 312 " + clientIdentity + " " + nickname + " ft_irc :IRC project for Ecole 42"
#define RPL_WHOISCHANNELS(clientIdentity, nickname, channelList) ":ft_irc 319 " + clientIdentity + " " + nickname + " :" + channelList
#define RPL_ENDOFWHOIS(clientIdentity, nickname) ":ft_irc 318 " + clientIdentity + " " + nickname + " :End of /WHOIS list"

// MODE -> USER
#define ERR_USERSDONTMATCH_VIEW(clientIdentity) ":ft_irc 502 " + clientIdentity + " :Can't view modes for other users"
#define ERR_USERSDONTMATCH_CHANGE(clientIdentity) ":ft_irc 502 " + clientIdentity + " :Cant change mode for other users"
#define ERR_UMODEUNKNOWNFLAG(clientIdentity) ":ft_irc 501 " + clientIdentity + " :Unknown MODE flag"
#define RPL_UMODEIS(clientIdentity, userModes) ":ft_irc 221 " + clientIdentity + " " + userModes
#define RPL_MODE_USER(clientIdentity, client, mode) ":" + clientIdentity + " MODE " + client + " :" + mode

// MODE -> CHANNEL
#define RPL_CHANNELMODEIS(clientIdentity, channel, mode) ":ft_irc 324 " + clientIdentity + " " + channel + " " + mode
#define RPL_MODE_CHANNEL(clientIdentity, channel, mode) ":" + clientIdentity + " MODE " + channel + " " + mode
#define RPL_MODE_CHANNEL_USER(source, channel, mode, nickname) ":" + source + " MODE " + channel + " " + mode + " " + nickname
#define RPL_BANLIST(clientIdentity, channel, nicknameList) ":ft_irc 367 " + clientIdentity + " " + channel + " " + nicknameList
#define RPL_ENDOFBANLIST(clientIdentity, channel) ":ft_irc 368 " + clientIdentity + " " + channel + " :End of channel ban list"
#define ERR_KEYSET(clientIdentity, channel) ":ft_irc 467 " + clientIdentity + " " + channel + " :Channel key already set"
#define ERR_UNKNOWNMODE(clientIdentity, modeChar, channel) ":ft_irc 472 " + clientIdentity + " " + modeChar + " :is unknown mode char to me for " + channel

#endif