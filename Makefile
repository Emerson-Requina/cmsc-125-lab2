CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude -g

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/schedsim

# Find all C source files recursively
SRCS = $(shell find $(SRC_DIR) -name "*.c")

# Convert .c file paths to .o file paths
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking objects..."
	$(CC) $(CFLAGS) -o $@ $^

# FIX: Separated the directory creation from the implicit rule 
# to avoid the "mixed implicit and normal rules" warning.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@echo "Compiling $< ..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning project..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)

rebuild: clean all

# FIX: Added $(ARGS) so you can pass flags from the terminal
run: $(TARGET)
	./$(TARGET) $(ARGS)

.PHONY: all clean rebuild run