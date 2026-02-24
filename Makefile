SRC_DIR = src
OBJ_DIR = build
INCLUDE_DIR = include
ASSEMBLER_SRC = asm

ifeq ($(OS),Windows_NT)
    EXE_EXT = .exe
    RM = rm -rf
else
    EXE_EXT =
    RM = rm -rf
endif

ASSEMBLER = assembler$(EXE_EXT)

CC = g++
CFLAGS = -Wall -I$(INCLUDE_DIR)

SRCS = $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

TARGET = $(OBJ_DIR)/processor

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	@echo -e "Assembling...\n"
	$(ASSEMBLER_SRC)/$(ASSEMBLER) asm/assembly.asm asm/build/machine-code.bin
	@echo -e "\nRunning $(TARGET)..."
	@$(TARGET)