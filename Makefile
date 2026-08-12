SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include

CC := clang
# _CRT_SECURE_NO_WARNINGS: MSVC deprecates standard C (fopen, getenv, strcpy)
# in favour of its own _s variants, which are not standard. We keep the standard.
CFLAGS := -Os -MD -Wall -Wextra -Wpedantic -D_CRT_SECURE_NO_WARNINGS -I$(INCLUDE_DIR)
LDFLAGS := -Os

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

TARGETS := $(patsubst $(BUILD_DIR)/%.o, $(BUILD_DIR)/%.exe, $(OBJS))

.PHONY: all clean

all: $(TARGETS)

$(BUILD_DIR)/%.exe: $(BUILD_DIR)/%.o
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

-include $(OBJS:.o=.d)
