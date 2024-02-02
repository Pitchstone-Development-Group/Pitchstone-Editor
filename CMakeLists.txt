cmake_minimum_required(VERSION 3.24)

project("Pitchstone Editor" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)

add_executable(Pitchstone)

string(TOLOWER CMAKE_BUILD_TYPE PITCHSTONE_BUILD)

IF (PITCHSTONE_BUILD EQUAL "release")
	set(PITCHSTONE_BUILD Release)
ELSE()
	set(PITCHSTONE_BUILD Debug)
ENDIF()

IF (WIN32)
	set_target_properties(Pitchstone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)
ELSE()

ENDIF()

IF (WIN32)
	cmake_path(GET Vulkan_LIBRARIES PARENT_PATH PITCHSTONE_VULKAN_LIBS)
	target_link_directories(Pitchstone PRIVATE ${PITCHSTONE_VULKAN_LIBS})
ENDIF()

target_include_directories(Pitchstone PRIVATE Vulkan::Headers)

target_link_libraries(Pitchstone PRIVATE Vulkan::Vulkan)
target_link_libraries(Pitchstone PRIVATE SDL2)
target_link_libraries(Pitchstone PRIVATE SDL2main)

IF (WIN32)
	cmake_path(GET Vulkan_INCLUDE_DIRS PARENT_PATH VULKAN_SDK_PATHS)
	file(COPY ${VULKAN_SDK_PATHS}/Bin/SDL2.dll DESTINATION ../bin/${PITCHSTONE_BUILD})
ENDIF()

add_subdirectory(src)