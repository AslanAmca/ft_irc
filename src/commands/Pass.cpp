/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 06:10:15 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/13 21:23:04 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

void Command::pass(Client *client, std::string parameter, std::string pass)
{
	std::string command = "PASS";

	if (client->getPasswordEntered())
	{
		client->sendMessageToSelf(ERR_ALREADYREGISTRED(client->getIdentity()));
		return;
	}

	if (parameter.empty())
	{
		client->sendMessageToSelf(ERR_NEEDMOREPARAMS(client->getIdentity(), command));
		return;
	}

	std::istringstream stream(parameter);
	std::string password;
	std::string temp;

	stream >> password >> temp;

	if (!temp.empty())
	{
		client->sendMessageToSelf(ERR_TOOMANYPARAMS(command));
		return;
	}

	if (password != pass)
	{
		client->sendMessageToSelf(ERR_PASSWDMISMATCH(client->getIdentity()));
		return;
	}

	client->getPasswordEntered() = true;
	client->sendMessageToSelf(RPL_PASS());
}