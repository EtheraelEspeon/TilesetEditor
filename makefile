rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d)) # terrifying expression that recursively iterates through files

ifeq ($(OS), Windows_NT)
APP_NAME=TilesetEditor.exe
RM_DIR=rd /s /q
rm_file=del $(subst /,\, $(1))
WINDOWS_MACROS=WINDOWS=
WINDOWS_LINKER_FLAGS=-subsystem,console
SHARED_LIBRARY_EXT=dll
make_directory = mkdir $(subst /,\, $(1))
else
APP_NAME=TilesetEditor.o
RM_DIR=rm -rf
rm_file=rm -f $(1)
PLATFORM_FLAGS=
SHARED_LIBRARY_EXT=so
make_directory = mkdir $(1)
endif

LIB_FILES="raylib"
CPP_VER=c++20
LIB_SEARCH_PATH=./

SRC_DIR=./src
OBJ_DIR=./obj
TMP_DIR=./temp
RES_DIR=./res
RELEASE_DIR=./release

# find all the files we care about

SRC_FILES = $(call rwildcard,.,*.cpp) # grab all the .cpp files in the src directory
OBJ_FILES = $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRC_FILES)) # substitute src/*.cpp with obj/*.o in the found source files

# declare all the targets

.PHONY: test build run clean release update_tpp

test:
	echo src files: $(SRC_FILES)
	echo obj files: $(OBJ_FILES)

# debug build
build: $(OBJ_FILES)
	-$(call rm_file,$(APP_NAME))
	g++ -Wl,$(WINDOWS_LINKER_FLAGS) -l$(LIB_FILES) -L$(LIB_SEARCH_PATH) -std=$(CPP_VER) -o $(APP_NAME) $(OBJ_FILES) -g

# Define a target for each object file in the obj dir that depends on its corresponding src file
# $@ is the name of the target, $< is the name of the first prereq,
# prepending - makes the target continue if the cmd fails, prepended @ makes it silent
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	-@$(call make_directory,$(dir $@))
	g++ -c -o $@ $< -std=$(CPP_VER) -D"$(WINDOWS_MACROS)" -g

# exists because the modificaton date of .tpp files
# isnt a dependency of the *.o target
update_tpp:
	$(call rm_file,"$(patsubst $(SRC_DIR)%.tpp,$(OBJ_DIR)%.o,$(call rwildcard,.,*.tpp))")

run: build
	$(APP_NAME)

clean:
	-$(RM_DIR) "$(TMP_DIR)"
	-$(RM_DIR) "$(OBJ_DIR)"

release: clean # xcopy copies a directory from a source to a destination on windows, probably doesn't work on Linux
	-$(RM_DIR) "$(RELEASE_DIR)"                                        
	$(call make_directory,$(RELEASE_DIR))                                  
	$(call make_directory,$(RELEASE_DIR)/$(TMP_DIR))                       
	xcopy "$(RES_DIR)" "release/res\\" /E /exclude:extensions_excluded_in_release.txt   
	copy "$(LIB_SEARCH_PATH)raylib.$(SHARED_LIBRARY_EXT)" "$(RELEASE_DIR)" 
	g++ -Wl,$(WINDOWS_LINKER_FLAGS) -l$(LIB_FILES) -D$(WINDOWS_MACROS) -L$(LIB_SEARCH_PATH) -std=$(CPP_VER) -o $(RELEASE_DIR)/$(APP_NAME) $(SRC_FILES) -O3
# delete the old release directory
# make a new one
# add a temp folder
# copy the res folder into the release dir, ignoring any .ase files
# copy the required shared libs into the root of the release dir
# build a release version