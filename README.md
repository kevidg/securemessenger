# 🔐 Secure Messaging Application

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

A terminal-based secure messaging application built with C, featuring **AES encryption**, **Diffie-Hellman key exchange**, and optional **file saving** of conversations. Developed as a final project for CS 455 - Spring 2025.

From a terninal run ./secmsg.x with -s to start the server, use -n to set the username. The default username will be the system login name. In a second terminal run ./secmsg.x -c to connect to the server. Use -n to set the client username. 
Ex:
Terminal 1:
   name@linux /home$./secmsg.x -s -n servername

Terminal 2:
   name@linux /home$./secmsg.x -c -n clientname

Type /quit in either terminal to end the session

If using the Makefile check the /build directory for the executable secmsg.x 

Or from the /src directory use gcc *.c -o secmsg.x -lssl -lcrypto
