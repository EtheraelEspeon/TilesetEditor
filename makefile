rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d)) # terrifying expression that recursively iterates through files

ifeq ($(OS), Windows_NT)
APP_NAME=TilesetEditor.exe
RM_COMMAND=rd /s /q
WINDOWS_MACROS="WINDOWS="
WINDOWS_LINKER_FLAGS=-subsystem,console
make_directory = mkdir $(subst /,\, $(1))
else
APP_NAME=TilesetEditor.o
RM_COMMAND=rm -rf
PLATFORM_FLAGS=
make_directory = mkdir $(1)
endif

LIB_FILES="raylib"
CPP_VER=c++20
LIB_SEARCH_PATH=./

SRC_DIR = ./src
OBJ_DIR = ./obj
TMP_DIR = ./temp

# find all the files we care about

SRC_FILES = $(call rwildcard,.,*.cpp) # grab all the .cpp files in the src directory
OBJ_FILES = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRC_FILES)) # substitute src/*.cpp with obj/*.o in the found source files

# declare all the targets

.PHONY: build run clean

test:
	echo src files: $(SRC_FILES)
	echo obj files: $(OBJ_FILES)

build: $(OBJ_FILES)
	-$(RM_COMMAND) $(APP_NAME)
	g++ -Wl,$(WINDOWS_LINKER_FLAGS) -l$(LIB_FILES) -D$(WINDOWS_MACROS) -L$(LIB_SEARCH_PATH) -std=$(CPP_VER) -o $(APP_NAME) $(OBJ_FILES)

# Define a target for each object file in the obj dir that depends on its corresponding src file
# $@ is the name of the target, $< is the name of the first prereq
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	-$(call make_directory,$(dir $@))
	g++ -c -o $@ $< -std=$(CPP_VER)

run: build
	$(APP_NAME)

clean:
	-$(RM_COMMAND) "$(TMP_DIR)"
	-$(RM_COMMAND) "$(OBJ_DIR)"