CC       = g++
CFLAGS   = -Wall -std=c++11
INCLUDES = -Iinclude -I../common -I../depends/lua/include -I/usr/include/mysql


DEBUG ?= y
ifeq ($(DEBUG), y)
    CFLAGS += -g3 -O0 -D_DEBUG
else
    CFLAGS += -s  -O3
endif


SRC_DIR   = src
BUILD_DIR = build

TARGET = lib/libcore.a

# -------------------------------------------------------- 

SRC := $(shell find $(SRC_DIR) -name *.cpp)

OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

DEP := $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.d)

DIR_GUARD = test -d $(@D) || mkdir -p $(@D)

# targets
all: $(TARGET)

# dependencies
-include $(DEP)

# build target
$(TARGET): $(OBJ)
	@echo ' '
	@echo 'Building library => $@'
	@$(DIR_GUARD)
	@ar -r  "$(TARGET)" $(OBJ)
	@echo 'Done!'
	@echo ' '

# build objects
$(OBJ): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling => $<"
	@$(DIR_GUARD)
	@$(CC) $(INCLUDES) $(CFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -o "$@" "$<"


# clean
clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all clean

