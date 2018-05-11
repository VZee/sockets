README for CHATCLIENT.CPP and CHATSERVE.PY
Author: Louisa Katlubeck

******************************************
INTRODUCTION
Chatserve.py and Chatclient.cpp work together to create a simple chat program.
Chatserve.py functions as the server, and Chatclient.cpp is the client. The 
programs are written so that they may be run from the same system, in two different
command windows. Chatserve.py must be started first, then Chatclient.cpp can be run.
The programs were run on flip1.engr.oregonstate.edu during testing.
******************************************

******************************************
CHATSERVE
To run chatserve, open a command window, and enter 

chmod +x chatserve

Once that command is run, enter

./chatserve ####

where the #### is the desired port number.
The program will let you know that it is listening, at which point it is ready for a connection
from the client.

To close the connection, press CTRL-C.
To close and then open a new connection enter \quit.
******************************************

******************************************
CHATCLIENT.CPP
Once chatserve is successfully listening, open another command window, and type

g++ -o chatclient chatclient.cpp

Once that line is executed, run the chat client by typing 
./chatclient #### ####

where the first ### is the host (eg 127.0.0.1) and the second #### is the same port used for the server.
Chatclient will then prompt you for your handle, and provide space to enter a message, and then it will open a connection to the server.

To close and then open a new connection enter \quit.

Note that the client must initiate contact with the server, and be the first to enter a message.
******************************************

******************************************
ON CLOSING THE CONNECTION
Either the client or the server may use \quit to close the connection on the client, 
while the server will keep running. The client will prompt the user to enter their handle.
Then the server will wait for the client to re establish the connection. Once the server enters CTRL-C, 
the entire connection will be terminated.
******************************************
