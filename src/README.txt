#*******************************************#
# \       / ___      _   __    _   _   ___  #
#  \  A  /  |_  |   /   /  \  / \ / \  |_   #
#   \/ \/   |__ |__ \_  \__/ /   U   \ |__  #
#              _____    __                  #
#                |     /  \                 #
#    _           |  _  \__/   _   _         #
#   /  |    | ___  /   /_\   / \ / \        #
#   \_  \__/       \_ /   \ /   u   \       #
#                                           #
#*******************************************#
Created By: Burns Hudson, Karl Brewer, Kevin Smith

Table of Contents

    HowTo Compile
    HowTo Run
    HowTo Example Usage
    HowTo Edit Configuration Files


================================================================================
HowTo Compile
================================================================================
    make
      Or
    make all

To Compile Just Client:
    make cam-client

To Compile Just Server:
    make cam-server

To Clean:
    make clean

================================================================================
HowTo Run:
================================================================================
Server:
  ./cam-server

Get Function:
  ./cam-client -g <server> <username> <filename> <savelocation>

Put Function:
  ./cam-client -p <group>|<server> <usernames> <filename>

Stat Function:
  ./cam-client -s <group>|<server> <username> <stat_type>

================================================================================
HowTo Example Usage:
================================================================================
Server:
    ./cam-server

Get a file from the user LUKESKYWALKER
    ./cam-client -g LinuxVM1 LUKESKYWALKER deathstar.tar destroythis.tar

Put a file on a server using the CPSC360 group
    ./cam-client -p CPSC350 * networkingisfun.c homework.c

Stats from ALICE on LinuxVM1
    ./cam-client -s LinuxVM1 ALICE 0   /* List all users on LinuxVM1 */
    ./cam-client -s LinuxVM1 ALICE 1   /* List all proccess ALICE owns */
    ./cam-client -s LinuxVM1 ALICE 2   /* List the contents of a folder */
================================================================================
HowTo Edit Configuration Files:
================================================================================
  Server Configuration:
  camserver.rc

  Client Configuration:
  camclient.rc

Default Config Files:

camclient.rc :
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
client_stat_interval = 2
server_port = 8888

[group]
group_name = "cpsc360"
group_servers = {
	LinuxVM1
	LinuxVM3
}
group_users = {
	KEVIN4
	KARLB
	BURNSH
}

[group]
group_name = "cpsc854"
group_servers = {
	LinuxVM2
}
group_users = {
	ALICE
	BOB
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

camserver.rc :
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
server_port = 8888
server_ipv6 = "off"
server_tls = "off"
server_deamon = 0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
