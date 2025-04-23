# ----- Secure Messenger Makefile -----
# This Makefile will build all .c files in the src/ directory
# Outputs to build/secmsg.x
# ----------------------------------------

# Compiler and flags
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -I./src

# Directories for source code and the output binary
SRC_DIR = src
BUILD_DIR = build

# Name of output binary
TARGET = $(BUILD_DIR)/secmsg.x

# External libraries that nee to be linked
LIBS = -lssl -lcrypto

# Sources files to compile, all .c files in the src directory
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Object files generated from src
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default if user types make
all: $(BUILD_DIR) $(TARGET)

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link all .o files
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

# Compile each .c file into an object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up, delete compiled output
clean:
	rm -rf $(BUILD_DIR)

run: all	
	 ./$(TARGET)

.PHONY: all clean run
