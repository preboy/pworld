CC              = g++
CFLAGS          = -g -O0 -Wall -std=c++14

SRC_DIR         = gameserver
BUILD_DIR       = build

MAKE_DIR        = test -d $(@D) || mkdir -p $(@D)

TARGET          = build/gameserver

FILE_SRC        := $(shell find $(SRC_DIR) -name *.cpp)
FILE_OBJ        := $(FILE_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
FILE_DEP        := $(FILE_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.d)

all: $(TARGET)

-include $(FILE_DEP)

$(TARGET): $(FILE_OBJ)
        @echo "Linking => $@"
        @$(CC) $(CFLAGS) -pthread -o $@ $^
        @echo 'Done!'

$(FILE_OBJ): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
        @echo "Compiling => $<"
        @$(MAKE_DIR)
        @$(CC) $(CFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -o "$@" "$<"

clean:
        @rm -rf $(BUILD_DIR)
        @rm -rf $(TARGET)

show:
        @echo $(FILE_SRC)
        @echo $(FILE_OBJ)
        @echo $(FILE_DEP)

.PHONY: all clean show

