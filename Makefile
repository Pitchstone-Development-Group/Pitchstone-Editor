ifdef OS
RM = del /Q
MKDIR = mkdir
MV = move
D = \ 
S = $(strip $(D))
OUT = bin/pitchstone.exe
else
ifeq ($(shell uname), Linux)
RM = rm -f
MKDIR = mkdir -p
MV = mv
S = /
OUT = bin/pitchstone
endif
endif

C_FLAGS = -O3 -ftree-vectorize -ftree-vectorizer-verbose=5 -mavx -mfma -mfpmath=sse -ffast-math -mno-vzeroupper -std=c++17 -Iinclude/
W_FLAGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused
L_FLAGS = -Iinclude/ -IC:/VulkanSDK/1.3.211.0/Include -Llibs/ -LC:/VulkanSDK/1.3.211.0/Lib -lglfw3dll -lvulkan-1

all: cleantop root vulkan build

tmps:
	$(MKDIR) tmp$(S)
	$(MKDIR) tmp$(S)vulkan

root: src/*.cpp
	g++ src/*.cpp  -c $(C_FLAGS) $(W_FLAGS) > error.txt 2>&1
	$(MV) *.o tmp$(S)

vulkan: src/vulkan/*.cpp
	g++ src/vulkan/*.cpp -c $(C_FLAGS) $(W_FLAGS) > error.txt 2>&1
	$(MV) *.o tmp$(S)vulkan

build:
	g++ tmp/*.o tmp/vulkan/*.o -o $(OUT) $(C_FLAGS) $(L_FLAGS) > error.txt 2>&1

cleantop:
	$(RM) *.o

clean:
	$(RM) *.o
	$(RM) tmp$(S)*.o tmp$(S)imgui$(S)*.o tmp$(S)vulkan$(S)*.o