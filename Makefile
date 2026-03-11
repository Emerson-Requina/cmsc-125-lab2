CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .

TARGET = $(BIN_DIR)/schedsim

# Explicitly list only the files needed for the scheduler
# Prevents it from accidentally including Lab 1 shell files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/process.c \
       $(SRC_DIR)/fcfs.c \
       $(SRC_DIR)/sjf.c \
       $(SRC_DIR)/stcf.c

# Convert .c paths to .o paths
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking objects into $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $< ..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning project..."
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean