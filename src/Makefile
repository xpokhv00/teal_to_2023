TARGET := compiler

BUILD_DIR := .
SOURCE_DIR := .
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))
ARCHIVE := ../xmacho12.zip

CC := gcc
CFLAGS := -std=c99 -pedantic -Wall -Wextra -g

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	#mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

zip:
	zip -j $(ARCHIVE) *.c *.h Makefile ../rozdeleni ../doc/dokumentace.pdf

.PHONY: clean

clean:
	rm $(OBJECTS) $(TARGET)
	rm $(ARCHIVE)
	#rm -rf $(BUILD_DIR)
