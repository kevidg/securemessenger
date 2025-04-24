# 🔐 Secure Messaging Application

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

A terminal-based secure messaging application built with C, featuring **AES encryption**, **Diffie-Hellman key exchange**, and optional **file saving** of conversations. Developed as a final project for CS 455 - Spring 2025.

## 📌 Features

- 🔒 End-to-end encrypted messaging (AES)
- 🔁 Secure key exchange (Diffie-Hellman)
- 🖥️ Terminal-based user interface
- 💾 Optional message saving to a file
- ⚠️ Robust error handling
- 🧠 Dynamic memory management for buffers

## 🚀 How It Works

1. Launch the app with the recipient's IP address as a command line argument.
2. If the recipient is online and running the app:
   - A secure key is exchanged using Diffie-Hellman.
   - Messages are encrypted and transmitted via AES.
3. On exit, users may choose to save the chat log to a file.

## 🛠️ Requirements

- GCC or compatible C compiler
- POSIX-compliant OS (Linux/macOS)
- Networking via sockets
- Standard C libraries

## 🧪 Build Instructions

```bash
<securemessenger>\$make
