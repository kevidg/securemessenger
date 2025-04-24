# 🔐 Secure Messaging Application

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

A terminal-based secure messaging application built with C, featuring **AES encryption**, **Diffie-Hellman key exchange**, and optional **file saving** of conversations. Developed as a final project for CS 455 - Spring 2025.

## 📌 Features

- 🔒 End-to-end encrypted messaging (AES)
- 🔁 Secure key exchange (Diffie-Hellman)
- 🖥️ Terminal-based user interface
- 💾 Optional message saving to a file
- ⚠️ Robust error handling

## 🚀 How It Works

1. Launch one terminal as server <-s> use -n to set the user name
2. Launch another terminal as client <-c> use -n to set the user name 
   - A secure key is exchanged using Diffie-Hellman.
   - Messages are encrypted and transmitted via AES.
3. On exit, </quit> users may choose to save the chat log to a file.

## 🛠️ Requirements

- GCC or compatible C compiler
- POSIX-compliant OS (Linux/macOS)
- Networking via sockets
- Standard C libraries
- OpenSSL <libssl-dev>

## 🧪 Build Instructions

```bash
<securemessenger>\$make
Or from the /src directory use gcc *.c -o secmsg.x -lssl -lcrypto
