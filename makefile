rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

ifeq ($(OS),Windows_NT)
EXE_NAME=TilesetEditor.exe
RM=del
PLATFORM_FLAGS=-Wl,-subsystem,console "-DWINDOWS="
else
EXE_NAME=TilesetEditor.o
PLATFORM_FLAGS=
endif

SOURCES=$(call rwildcard,.,*.cpp)

CFLAGS=-L. -std=c++20

CFLAGS_DEBUG="-DDEBUG=" -g

CFLAGS_RELEASE=-O3

BUILD_COMMAND = g++ -o $(EXE_NAME) $(SOURCES) $(PLATFORM_FLAGS) $(CFLAGS) -l "raylib"

all: 
	$(BUILD_COMMAND) $(CFLAGS_DEBUG)

release:
	$(BUILD_COMMAND) $(CFLAGS_RELEASE)

clean:
	$(RM) $(EXE_NAME)
	$(RM) "temp/"

run: all
	./$(EXE_NAME)

debug: all
	gdb $(EXE_NAME)