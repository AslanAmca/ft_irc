/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 01:04:47 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/21 00:35:05 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

void Utils::trimStart(std::string &str, const std::string &charsToTrim)
{
	if (str.empty())
		return;

	size_t start_pos = str.find_first_not_of(charsToTrim);
	if (start_pos != std::string::npos)
		str.erase(0, start_pos);
	else
		str.clear();
}

void Utils::trimEnd(std::string &str, const std::string &charsToTrim)
{
	if (str.empty())
		return;

	size_t end_pos = str.find_last_not_of(charsToTrim);
	if (end_pos != std::string::npos)
		str.erase(end_pos + 1);
	else
		str.clear();
}

void Utils::trimBoth(std::string &str, const std::string &charsToTrim)
{
	trimStart(str, charsToTrim);
	trimEnd(str, charsToTrim);
}

std::string Utils::toUpperString(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == '{')
			str[i] = '[';
		else if (str[i] == '}')
			str[i] = ']';
		else if (str[i] == '|')
			str[i] = '\\';
		else if (str[i] == '^')
			str[i] = '~';
		else
			str[i] = std::toupper(str[i]);
	}

	return str;
}

std::string Utils::intToString(int value)
{
	std::stringstream stream;
	stream << value;
	return stream.str();
}

bool Utils::isMask(std::string receiver)
{
	if (receiver[0] == '$')
		return true;

	if (receiver[0] != '#')
	{
		return false;
	}

	std::size_t dotPos = receiver.find('.');
	if (dotPos == std::string::npos)
	{
		return false;
	}

	std::string afterLastDot = receiver.substr(dotPos + 1);
	if (afterLastDot.find('*') != std::string::npos || afterLastDot.find('?') != std::string::npos)
	{
		return false;
	}

	return true;
}

bool Utils::isNickMask(std::string receiver)
{
	return receiver.find_first_of("*?!@") != std::string::npos;
}

bool Utils::validChannelName(std::string &channelName)
{
	if (channelName.empty())
		return false;

	if (channelName[0] == '&')
		channelName[0] = '#';

	if (channelName.size() < 2 || channelName.size() > 200 || channelName[0] != '#' || channelName.find_first_of("#&\a,:ığüşöç ", 1) != std::string::npos)
		return false;

	return true;
}

bool Utils::validName(std::string name, bool isNickname)
{
	if (name.empty())
		return false;

	if (isNickname && name.length() > 9)
		return false;

	if (!std::isalpha(name[0]))
		return false;

	const std::string allowedSpecialChars = "-[]\\`^{}";

	for (size_t i = 0; i < name.size(); ++i)
	{
		if (!std::isalnum(name[i]) && allowedSpecialChars.find(name[i]) == std::string::npos)
			return false;
	}

	return true;
}

std::string Utils::getDuplicateValue(const std::vector<std::string> &list)
{
	if (list.size() < 2)
		return "";

	std::set<std::string> seenTargets;

	for (std::vector<std::string>::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		if (!seenTargets.insert(*it).second)
		{
			return *it;
		}
	}

	return "";
}

bool Utils::hasDuplicateChar(std::string value)
{
	for (size_t i = 0; i < value.size(); ++i)
	{
		for (size_t j = i + 1; j < value.size(); ++j)
		{
			if (value[i] == value[j])
			{
				return true;
			}
		}
	}
	return false;
}
