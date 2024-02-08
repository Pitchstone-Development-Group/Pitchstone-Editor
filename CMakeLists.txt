cmake_minimum_required(VERSION 3.21)

project("Pitchstone Editor" LANGUAGES C CXX)

IF (WIN32)
	cmake_policy(SET CMP0091 NEW)
ENDIF()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_SIZEOF_VOID_P 8)

find_package(Vulkan REQUIRED)

add_executable(Pitchstone)

if (CMAKE_BUILD_TYPE)
	string(TOLOWER ${CMAKE_BUILD_TYPE} PITCHSTONE_BUILD)
ELSE()
	set(PITCHSTONE_BUILD debug)
ENDIF()

IF (WIN32)
	set(CMAKE_CXX_FLAGS "/EHsc /arch:AVX2 /W4 /WX")
ELSE()
	set(CMAKE_CXX_FLAGS "-mavx -mavx2 -mfma -Wall -Wextra -Wpedantic -Werror")
ENDIF()


IF (${PITCHSTONE_BUILD} STREQUAL release)
	set(PITCHSTONE_BUILD Release)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS}")
ELSE()
	set(PITCHSTONE_BUILD Debug)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS}")
	# Explicitly saying debug mode can cause both release and debug libs to be linked
	IF (WIN32)
		target_link_options(Pitchstone PRIVATE "/NODEFAULTLIB:msvcrt.lib")
	ENDIF()
ENDIF()

IF (WIN32)
	set_target_properties(Pitchstone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)
ELSE()
	set_target_properties(Pitchstone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin/${PITCHSTONE_BUILD})
ENDIF()

IF (WIN32)
	cmake_path(GET Vulkan_LIBRARIES PARENT_PATH PITCHSTONE_VULKAN_LIBS)
	target_link_directories(Pitchstone PRIVATE ${PITCHSTONE_VULKAN_LIBS})
ENDIF()

target_include_directories(Pitchstone PRIVATE Vulkan::Headers)
target_include_directories(Pitchstone PRIVATE include)

target_link_libraries(Pitchstone PRIVATE Vulkan::Vulkan)
target_link_libraries(Pitchstone PRIVATE SDL2main SDL2)

# Mainly for copying the DLLs and their PDBs in debug
IF (WIN32)
	cmake_path(GET Vulkan_INCLUDE_DIRS PARENT_PATH VULKAN_SDK_PATHS)
	file(COPY ${VULKAN_SDK_PATHS}/Bin/SDL2.dll DESTINATION ../bin/${PITCHSTONE_BUILD})
	IF (PITCHSTONE_BUILD STREQUAL Debug)
		file(COPY ${VULKAN_SDK_PATHS}/Bin/SDL2.pdb DESTINATION ../bin/${PITCHSTONE_BUILD})
		file(COPY ${VULKAN_SDK_PATHS}/Bin/SDL2main.pdb DESTINATION ../bin/${PITCHSTONE_BUILD})
	ENDIF()
ENDIF()

file(COPY resources/ DESTINATION ../bin/${PITCHSTONE_BUILD})

target_sources(Pitchstone PRIVATE include/imgui/imgui_demo.cpp)
target_sources(Pitchstone PRIVATE include/imgui/imgui_draw.cpp)
target_sources(Pitchstone PRIVATE include/imgui/imgui_impl_sdl2.cpp)
target_sources(Pitchstone PRIVATE include/imgui/imgui_impl_vulkan.cpp)
target_sources(Pitchstone PRIVATE include/imgui/imgui_tables.cpp)
target_sources(Pitchstone PRIVATE include/imgui/imgui_widgets.cpp)
target_sources(Pitchstone PRIVATE include/imgui/imgui.cpp)

add_subdirectory(src)