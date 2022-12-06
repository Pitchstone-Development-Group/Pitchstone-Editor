ifdef OS
RM = del /Q
MKDIR = mkdir
MV = move
D = \ 
S = $(strip $(D))
OUT = bin/pitchstone.exe
LIBS = -Llibs/ -Llibs/windows/ -L$(VULKAN_SDK)\Lib -lglfw3.dll -lavcodec.dll -lavformat.dll -lavutil.dll -lvulkan-1
INCLUDE = -Iinclude/ -Iinclude/windows/ -I$(VULKAN_SDK)\Include
else
ifeq ($(shell uname), Linux)
RM = rm -f
MKDIR = mkdir -p
MV = mv
S = /
OUT = bin/pitchstone
LIBS = -Llibs/ -lglfw -lavcodec -lavformat -lavutil -lvulkan
INCLUDE = -Iinclude/
endif
endif

C_FLAGS = -O3 -g -m64 -march=x86-64 -ftree-vectorize -ftree-vectorizer-verbose=5 -mavx -mfma -mfpmath=sse -ffast-math -mno-vzeroupper -std=c++20 $(INCLUDE)
W_FLAGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused -Wno-volatile -Wno-sign-conversion
L_FLAGS = $(INCLUDE) $(LIBS) 

all: cleantop root sys media vulkan build

tmps:
	$(MKDIR) tmp$(S)
	$(MKDIR) tmp$(S)sys
	$(MKDIR) tmp$(S)media
	$(MKDIR) tmp$(S)vulkan

root: src/*.cpp
	g++ src/*.cpp  -c $(C_FLAGS) $(W_FLAGS) > error.txt 2>&1
	$(MV) *.o tmp$(S)

sys: src/sys/*.cpp
	g++ src/sys/*.cpp  -c $(C_FLAGS) $(W_FLAGS) > error.txt 2>&1
	$(MV) *.o tmp$(S)sys

media: src/media/*.cpp
	g++ src/media/*.cpp  -c $(C_FLAGS) $(W_FLAGS) > error.txt 2>&1
	$(MV) *.o tmp$(S)media

vulkan: src/vulkan/*.cpp
	g++ src/vulkan/*.cpp -c $(C_FLAGS) $(W_FLAGS) > error.txt 2>&1
	$(MV) *.o tmp$(S)vulkan

build:
	g++ tmp/*.o tmp/sys/*.o tmp/vulkan/*.o -o $(OUT) $(C_FLAGS) $(L_FLAGS) > error.txt 2>&1

cleantop:
	$(RM) *.o

clean:
	$(RM) *.o
	$(RM) tmp$(S)*.o tmp$(S)sys$(S)*.o tmp$(S)vulkan$(S)*.o