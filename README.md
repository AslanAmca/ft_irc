# ft_irc
This project is about creating our own IRC server. We used an actual IRC client to connect to our server and test it.
**IRC** (Internet Relay Chat) is a protocol for real-time text messaging between internet-connected computers created in **1988**. It is mainly used for group discussion in chat rooms called “**channels**” although it supports private messages between two users, data transfer, and various client-side commands.

## Mandatory part
  - We have to develop an IRC server in C++ 98.
  - The server must be capable of handling multiple clients at the same time and never hang.
  - Only 1 poll() (or equivalent) can be used for handling all these operations (read, write, but also listen, and so forth).
  - An IRC client must be able to connect to your server without encountering any error.
  - Communication between client and server has to be done via TCP/IP (v4).
  - Using the IRC client with our server must be similar to using it with any official IRC server. However, we only have to implement the following features:
    -  We must be able to authenticate, set a nickname, a username, join a channel, send and receive private messages using the IRC client.
    -  All the messages sent from one client to a channel have to be forwarded to every other client that joined the channel.
    -  We must have operators and regular users.
    -  Then, we have to implement the commands that are specific to operators.
## Bonus part
Here are the extra features we added to our IRC server so it looks even more like and actual IRC server:
  - Handle file transfer.
  - Basic Bot

# Our IRC Commands
| Command | Description |
| :-----------: | :----------- |
| CAP LS 302 | The `CAP LS 302` is the command written for hexchat that lists the commands that can be used on the server. |
| Pass | The `PASS` command is used to set a ‘connection password’. If set, the password must be set before any attempt to register the connection is made. |
| Nick | The `NICK` command is used to give the client a nickname or change the previous one. |
| User | The `USER` command is used at the beginning of a connection to specify the username and realname of a new user. |
| Quit | The `QUIT` command is used to terminate a client’s connection to the server. The server closing the connection to the client. |
| Join | The `JOIN` command indicates that the client wants to join the given channel(s), each channel using the given key for it. |
| Part | The `PART` command removes the client from the given channel(s). |
| Mode | The `MODE` command is used to set or remove options (or modes) from a given target. Our user modes : i. Our channels modes: o,p,s,i,t,n,m,l,b,v,k |
| Topic | The `TOPIC` command is used to change or view the topic of the given channel. |
| Names | The `NAMES` command is used to view the nicknames joined to a channel and their channel membership prefixes. |
| List | The `LIST` command is used to get a list of channels along with some information about each channel. |
| Invite | The `INVITE` command is used to invite a user to a channel. |
| Kick | The `KICK` command can be used to request the forced removal of a user from a channel. |
| Privmsg | The `PRIVMSG` command is used to send private messages between users, as well as to send messages to channels. |
| Notice | The `NOTICE` command is used to send notices between users, as well as to send notices to channels. The difference between `NOTICE` and `PRIVMSG` is that automatic replies to a NOTICE message are never sent. |
| Whois | The `WHOIS` command is used to retrieve a user's information. You can access this information by specifying the user's nickname or IP address. |

# Start IRC Server
To compile the program, use:
  - `make`
To start the Server, use:
  - `./ircserv <port> <password>`
    - **port**: The port number on which your IRC server will be listening to for incoming IRC connections.
    - **password**: The connection password. It will be needed by any IRC client that tries to connect to your server.
To connect to the server, you can use:
  - `nc <IP ADDRESS> <PORT>`
    - **IP ADDRESS**: Host IP address.
    - **PORT**: The PORT that the server listening on.
  - You can also use an IRC Client like HexChat

# Bot
There is a simple bot on the server. You can talk to the bot as "PRIVMSG Bot :message", you can see the commands by typing help.
 - PRIVMSG Bot :help