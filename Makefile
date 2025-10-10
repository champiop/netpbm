# ==== Configuration ====
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O0
LEX     := flex
AR      := ar
ARFLAGS := rcs

# ==== Directories ====
SRC_DIR 		 := src
BUILD_DIR 	 := build
BIN_DIR 		 := $(BUILD_DIR)/bin
INCLUDE_DIR  := include
TEST_DIR 		 := tests
TEST_BIN_DIR := $(TEST_DIR)/bin

# ==== Files ====
SRC			   := $(SRC_DIR)/netpbm.c
SRC_CLI		 := $(SRC_DIR)/netpbm_cli.c
SRC_LEX    := $(SRC_DIR)/netpbm.ll
LEX_C      := $(BUILD_DIR)/lex.yy.c
OBJ   		 := $(BUILD_DIR)/netpbm.o
OBJ_CLI		 := $(BUILD_DIR)/netpbm_cli.o
OBJ_LEX    := $(BUILD_DIR)/netpbm_lex.o
LIB_NAME   := $(BUILD_DIR)/libnetpbm.a
EXE_NAME   := $(BIN_DIR)/netpbm
TEST_SRC 	 := $(wildcard tests/test_*.c)
TEST_BIN   := $(patsubst tests/%.c,tests/bin/%,$(TEST_SRC))

# ==== Default target ====
all: $(EXE_NAME)

# ==== Rule: build executable ====
$(EXE_NAME): $(OBJ) $(OBJ_CLI) $(OBJ_LEX) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# ==== Rule: build static library ====
$(LIB_NAME): $(OBJ) $(OBJ_LEX) | $(BUILD_DIR)
	$(AR) $(ARFLAGS) $@ $^

# ==== Compile C sources ====
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/netpbm.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

# ==== Generate and compile lex source ====
$(OBJ_LEX): $(SRC_LEX) $(INCLUDE_DIR)/netpbm.h | $(BUILD_DIR)
	$(LEX) -o $(LEX_C) $(SRC_LEX)
	$(CC) $(CFLAGS) -Wno-implicit-function-declaration -Wno-unused-function -c $(LEX_C) -o $@ -I$(INCLUDE_DIR)

# ==== Compile tests sources ===
$(TEST_BIN_DIR)/%: $(TEST_DIR)/%.c $(LIB_NAME) | $(TEST_BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LIB_NAME) -I$(INCLUDE_DIR)

# ==== Directory creation ====
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TEST_BIN_DIR):
	mkdir -p $(TEST_BIN_DIR)

# ==== Clean ====
clean:
	rm -rf $(BUILD_DIR) $(TEST_BIN_DIR)

.PHONY: all clean lib tests
lib: $(LIB_NAME)

tests: $(TEST_BIN)
