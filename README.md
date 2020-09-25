# UVG | Redes - Proyecto 2

## XMPP chat client.
- Made by: Sebastian Arriola (11463)

## Project Description
- The project is a XMPP Chat Client which connects to a server provided by the university course.
- The Chat Client is used through the command line interface by writing commands (see the Commands section).

## Implemented Functionalities
- :heavy_check_mark: Register account on the server.
- :heavy_check_mark: Delete account from the server.
- :heavy_check_mark: Login / logout from the server.
- :heavy_check_mark: Show all users registered on the server (Shows only the first 32 results).
- :heavy_check_mark: Show your contacts (roster).
- :heavy_check_mark: Show user / contact details (vCard).
- :heavy_check_mark: 1 to 1 communication with any user or contact (private messaging).
- :heavy_check_mark: Participate in group chats (group messaging).
- :heavy_check_mark: Define your presence message and status.
- :heavy_check_mark: Send / receive notifications (when a contact changes its presence and when a message is received).
- :heavy_check_mark: Send / receive files.

## Installation
### With Docker (Recommended)
- Install docker on your machine (Docker version used was 19.03.12)
- Clone the project directory into your machine.
- Inside the project directory, run `docker build --tag xmpp_chat .`, which creates the project image with all the project dependencies.
- To use the XMPP Chat, run `docker run -it xmpp_chat`, which runs the project container.
- When you are finished using the XMPP Chat and if you want to erase everything used by Docker, run `docker rmi xmpp_chat ubuntu:bionic`

### With Ubuntu
- The library used to interact with the XMPP protocol was libstrophe. Libstrophe uses the expat XML Parser. The terminal client was built using the ncurses library.
- You have to install all the dependencies on your host machine. Run `sudo apt update -y && sudo apt install -y build-essential pkg-config libssl-dev libexpat-dev libstrophe-dev libncurses5-dev libncursesw5-dev`
- Clone the project directory into your machine.
- Inside the project directory, run `make`
- To run the XMPP Chat Client, run `./ncurses_client`

## Commands
The syntax of the commands is: `/command <argument> [options]` where `<argument>` denotes an argument that **MUST** be specified and `[options]` are **OPTIONAL**. **IMPORTANT NOTE: all Jabber IDS MUST be specified without the host part (see examples of each command) EXCEPT when using the `/file` command.**
* `/help`
  - usage: `/help <command>`
  - description: shows usage details of the specified command.
  - example: `/help users`
* `/users`
  - usage: `/users`
  - description: shows up to 32 of the users that are registered on the server.
  - example: `/users`
* `/roster`
  - usage: `/roster [add <jid>]`
  - description: when no options are specified, like `/roster`, it shows the user's contacts (its roster). When used with options, like `/roster add <jid>`, it sends a friend request (subscription) to the user identified by `jid`. The Jabber ID **must** be specified without the host part (see example).
  - example: `/roster add sebdev`
* `/active`
  - usage: `/active`
  - description: shows all presence messages that have been received. This shows all the presence changes that the user's contacts have made. In essence, it shows which users are active on the server.
  - example: `/active`
* `/presence`
  - usage: `/presence <show> [status]`
  - description: changes the user's current status and presence message. `<show>` **MUST** be one of the following: `available, away, xa, dnd, invisible` `[status]` is optional and it is a string of characters.
  - example: `/presence away sleeping...`
* `/priv`
  - usage: `/priv <jid>`
  - description: starts a private chat with the user identified by `jid`. The Jabber ID **MUST** be specified without the host part (see example). To send a message, simply type in your message and press ENTER/RETURN.
  - example: `/priv sebdev`
* `/group`
  - usage: `/group <room_jid> <nick>`
  - description: joins the group chat identified by `room_jid`. If the group does not exist and the server is configured correctly, the group is created. The group Jabber ID **MUST** be specified without the host part (see example). `nick` is the nickname that the user chooses to use inside the group.
  - example: `/group linux_lovers tux`
* `/vcard`
  - usage: `/vcard <jid>`
  - description: shows the vCard of the user identified by `jid`. The Jabber ID **MUST** be specified without the host part (see example).
  - example: `/vcard sebdev_gajim` (use the jid `sebdev_gajim` to see a correctly working vCard).
* `/file`
  - usage: `/file <filename> <jid>`
  - description: sends the file identified by `filename` to the user identified by `jid`. The Jabber ID **MUST** be **FULLY** specified. It is recommended to first run the `/active` command to get a user's full Jabber ID. **NOTE: for simplicity, only files that are in the current directory can be sent.**
  - example: `/file test.png sebdev@redes2020.xyz/31gr16q3bc`. (`test.png` is a file included in this repo for testing purposes).
* `/menu`
  - usage: `/menu`
  - description: when on a private or group chat, this command exits the chat, otherwise it clears the screen.
  - example: `/menu`
* `/delete`
  - usage: `/delete`
  - description: deletes your account from the server.
  - example `/delete`
* `/quit`
  - usage: `/quit`
  - description: exit the program :cry:
  - example: `/quit`

## Useful Links
- [Libstrophe](http://strophe.im/libstrophe/)
- [Ncurses](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)
- [XMPP Protocol](https://xmpp.org)
