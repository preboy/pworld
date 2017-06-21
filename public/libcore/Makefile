CC       = g++
CFLAGS   = -g -O0 -Wall -std=c++14
INCLUDES = include ../common ../depends/lua/include ../depends/mysql/include

BUILD_DIR = build
SRC_DIR   = src

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

