CC		= g++
CFLAGS		= -Wall -std=c++11
INCLUDES        = -I../../public/common -I../../public/libcore/include -I../../public/depends/lua/include/ -I/usr/include/mysql
LIB_INCLUDES    = -L../../public/libcore/lib/   -L../../public/depends/lua/lib -L/usr/lib64/mysql
LIBS            = -lcore -pthread -llua -lmysqlclient -ldl


DEBUG ?= y
ifeq ($(DEBUG), y)
    CFLAGS += -g3 -O0 -D_DEBUG
else
    CFLAGS += -s  -O3
endif


SRC_DIR			= gameserver
BUILD_DIR		= build

MAKE_DIR		= test -d $(@D)	|| mkdir -p	$(@D)

TARGET			= ../bin/gameserver

FILE_SRC		:= $(shell find	$(SRC_DIR) -name *.cpp)
FILE_OBJ		:= $(FILE_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
FILE_DEP		:= $(FILE_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.d)

all: $(TARGET)

-include $(FILE_DEP)

$(TARGET): $(FILE_OBJ)
		@echo "Linking => $@"
		@$(MAKE_DIR)
		@$(CC) $(LIB_INCLUDES) $(CFLAGS) -o $@	$^ $(LIBS)
		@echo 'Done!'

$(FILE_OBJ): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
		@echo "Compiling =>	$<"
		@$(MAKE_DIR)
		@$(CC) $(INCLUDES) $(CFLAGS) -c	-MMD -MP -MF"$(@:%.o=%.d)" -o "$@" "$<"

clean:
		@rm	-rf	$(BUILD_DIR)
		@rm	-rf	$(TARGET)

show:
		@echo $(FILE_SRC)
		@echo $(FILE_OBJ)
		@echo $(FILE_DEP)

.PHONY:	all	clean show

