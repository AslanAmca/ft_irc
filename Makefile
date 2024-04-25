# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aaslan <aaslan@student.42kocaeli.com.tr    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/15 16:28:53 by aaslan            #+#    #+#              #
#    Updated: 2024/04/24 16:27:13 by aaslan           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 
SRC			= src/main.cpp src/Server.cpp src/ServerHelper.cpp src/Client.cpp src/Channel.cpp src/Utils.cpp \
				src/commands/Cap.cpp src/commands/Pass.cpp src/commands/Nick.cpp src/commands/User.cpp src/commands/Quit.cpp \
				src/commands/Join.cpp src/commands/Part.cpp src/commands/Mode.cpp src/commands/Topic.cpp \
				src/commands/Names.cpp src/commands/List.cpp src/commands/Invite.cpp src/commands/Kick.cpp \
				src/commands/Privmsg.cpp src/commands/Notice.cpp src/commands/Whois.cpp
OBJ			= $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re