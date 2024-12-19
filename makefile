TARGET = main

CC = gcc
CFLAGS = -Wall -Wextra -g

SRC_DIR = src
SRC_MAIN = src/main

OBJ_DIR = obj

SRC_FILES = $(wildcard $(SRC_MAIN).c $(SRC_DIR)/**/*.c)

OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(OBJ_DIR) $(TARGET)

