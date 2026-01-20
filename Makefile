MINGW ?= D:/Development/MinGW
CC := $(MINGW)/bin/gcc.exe

SHELL := cmd.exe
.SHELLFLAGS := /C

TOOLCHAIN_FLAGS := -B$(MINGW)/bin/

PATH := $(MINGW)/bin;$(PATH)
export PATH

SRC_DIR := src
INC_DIR := $(SRC_DIR)/include
BUILD_DIR := build

TARGET := $(BUILD_DIR)/minecraft.exe

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

CFLAGS := -std=c11 -O2 -Wall -Wextra -I./$(INC_DIR) $(TOOLCHAIN_FLAGS)
LDFLAGS := -static -static-libgcc -lopengl32 -lgdi32 -luser32 -lkernel32 -lwinmm

.PHONY: all run clean

all: $(TARGET)

$(BUILD_DIR):
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

run: all
	$(TARGET)

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
