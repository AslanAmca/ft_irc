/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 00:57:49 by aaslan            #+#    #+#             */
/*   Updated: 2024/04/17 21:04:23 by aaslan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <vector>
#include <set>

class Utils
{
public:
	static void trimStart(std::string &str, const std::string &charsToTrim = " ");
	static void trimEnd(std::string &str, const std::string &charsToTrim = " ");
	static void trimBoth(std::string &str, const std::string &charsToTrim = " ");
	static std::string toUpperString(std::string str);
	static std::string intToString(int value);
	static bool isMask(std::string receiver);
	static bool isNickMask(std::string receiver);
	static bool validChannelName(std::string &channelName);
	static bool validName(std::string name, bool isNickname);
	static std::string getDuplicateValue(const std::vector<std::string> &list);
	static bool hasDuplicateChar(std::string value);
};

#endif