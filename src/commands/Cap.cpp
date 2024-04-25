/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 04:46:22 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/10 04:57:09 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "../Server.hpp"

void Command::cap(Client *client, std::string parameter)
{
	std::string command = "CAP";

	if (parameter == "LS 302" || parameter == "ls 302")
	{
		client->sendMessageToSelf(RPL_CAPLS());
		return;
	}

	client->sendMessageToSelf(ERR_CAPLS());
}