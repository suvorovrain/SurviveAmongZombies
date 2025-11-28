CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O3 -MMD -MP
LDFLAGS = -lSDL2 -lm

SRC_DIR = src
DEMO_DIR = ../../src
INCLUDE_DIR = include
BUILD_DIR = build
TEST_DIR = tests
DEPS_DIR = ../../deps

# Sources for engine (src/)
LIB_SOURCES = $(shell find $(SRC_DIR) -type f -name '*.c')
LIB_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/lib/%.o,$(LIB_SOURCES))

# Demo
DEMO_SOURCES = $(shell find $(DEMO_DIR) -type f -name '*.c')
DEMO_OBJECTS = $(patsubst $(DEMO_DIR)/%.c,$(BUILD_DIR)/demo/%.o,$(DEMO_SOURCES))

# Deps
DEPS_SOURCES = $(wildcard $(DEPS_DIR)/*.c)
DEPS_OBJECTS = $(patsubst $(DEPS_DIR)/%.c,$(BUILD_DIR)/deps/%.o,$(DEPS_SOURCES))

DEPS = $(LIB_OBJECTS:.o=.d) $(DEMO_OBJECTS:.o=.d)

TARGET = $(BUILD_DIR)/demo_game

TEST_SOURCES = $(shell find $(TEST_DIR) -type f -name '*.c' ! -name 'test_framework.c')
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/tests/%.o,$(TEST_SOURCES))
TEST_FRAMEWORK_OBJ = $(BUILD_DIR)/tests/test_framework.o
TEST_TARGET = $(BUILD_DIR)/test_runner
SRC_OBJECTS_FOR_TESTS = $(LIB_OBJECTS)

.PHONY: all clean run test fmt check-fmt

all: $(TARGET)

$(BUILD_DIR) $(BUILD_DIR)/tests:
	@mkdir -p $@

# Engine
$(BUILD_DIR)/lib/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -I$(DEPS_DIR) -c $< -o $@

# Demo (NOW TAKES SOURCES FROM ../../src)
$(BUILD_DIR)/demo/%.o: $(DEMO_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -I$(DEPS_DIR) -c $< -o $@

# Deps (NOW TAKES SOURCES FROM ../../deps)
$(BUILD_DIR)/deps/%.o: $(DEPS_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) -std=c99 -O2 -w -I$(DEPS_DIR) -c $< -o $@

# Link game
$(TARGET): $(LIB_OBJECTS) $(DEMO_OBJECTS) $(DEPS_OBJECTS)
	$(CC) $(LIB_OBJECTS) $(DEMO_OBJECTS) $(DEPS_OBJECTS) -o $@ $(LDFLAGS)

# Tests
$(BUILD_DIR)/tests/test_framework.o: $(TEST_DIR)/test_framework.c | $(BUILD_DIR)/tests
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -I$(DEPS_DIR) -c $< -o $@

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)/tests
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -I$(DEPS_DIR) -I$(TEST_DIR) -c $< -o $@

$(TEST_TARGET): $(TEST_FRAMEWORK_OBJ) $(TEST_OBJECTS) $(SRC_OBJECTS_FOR_TESTS) $(DEPS_OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	@./$(TEST_TARGET)

# clang-format
FMT_SOURCES := $(shell find $(SRC_DIR) $(DEMO_DIR) $(INCLUDE_DIR) -path "$(INCLUDE_DIR)/external" -prune -o -name '*.c' -o -name '*.h' -print)

fmt:
	clang-format -i $(FMT_SOURCES)

check-fmt:
	@clang-format --dry-run --Werror $(FMT_SOURCES)

-include $(DEPS)
