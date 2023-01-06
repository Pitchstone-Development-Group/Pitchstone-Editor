# Pitchstone Editor

A free, multi-platform editor using Vulkan as its foundation

## Code Structure
|  Directory | Explanation |
|-------------|---------------|
| src/ | Source code for the Pitchstone Editor, with expection of src/imgui/ folder, licensed under the [GPL v3.0 License](https://github.com/Pitchstone-Development-Group/Pitchstone-Editor/blob/main/LICENSE) |
| include/ | Folder for libraries' header files (Both Linux and Windows) |
| include/windows/ | Folder for libraries' header files (Windows Only) |
| include/linux/ | Folder for libraries' header files (Linux Only) |
| libs/ | Static library binaries (Both Linux and Windows) |
| libs/windows/ | Static library binaries (Windows Only) |
| libs/linux/ | Static library binaries (Linux Only) |
| bin/ | Directory for the executable, dynamic libraries, and resources |

## Building

So far the current build uses Makefile to assemble the program, and will be moving to Cmake in the future.

### Windows
1) Download and install the latest version of the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows).
2) Download and install [MinGW-w64](https://github.com/brechtsanders/winlibs_mingw/releases/download/11.2.1-snapshot20211211-9.0.0-ucrt-r1/winlibs-x86_64-posix-seh-gcc-11.2.1-snapshot20211211-mingw-w64ucrt-9.0.0-r1.zip).
3) Go to the root directory of the repository, the same directory the Makefile should be at.
4) Run `mingw32-make setup` to assemble the temporary directories (***Run this before all other commands***).
5) Run `mingw32-make all` to build the entire application.

If you only modified source files of a single directory, then you may run `mingw32-make media build` for example.

### Linux
1) To install the needed dependencies, run:
* Debain/Ubuntu: `sudo apt-get install -y libvulkan-dev libglfw3-dev libavcodec-dev libavformat-dev libavutil-dev libswresample-dev`
2) To Install the Vulkan SDK, run:
* Ubuntu: [Getting Started with the Ubuntu Vulkan SDK](https://vulkan.lunarg.com/doc/view/latest/linux/getting_started_ubuntu.html)
* Tarball: [Getting Started with the Linux Tarball Vulkan SDK](https://vulkan.lunarg.com/doc/view/1.2.176.1/linux/getting_started.html)
3) Go to the root directory of the repository, the same directory the Makefile should be at.
4) Run `make setup` to assemble the temporary directories (***Run this before all other commands***).
5) Run `make all` to build the entire application.

If you only modified source files of a single directory, then you may run `make media build` for example.

## Libraries Used

GLFW is under the [zlib/libpng license](https://www.glfw.org/license)
* bin/glfw3.dll
* libs/windows/libglfw3.dll.a
* include/windows/GLFW/

FFPMEG and Libav are under the [GPL version 2 license](https://www.ffmpeg.org/legal.html) via [FFMPEG-Builds](https://github.com/BtbN/FFmpeg-Builds/releases)
* bin/avcodec-59.dll
* bin/avformat-59.dll
* bin/avutil-57.dll
* bin/swresample-4.dll
* libavcodec.dll.a
* libavformat.dll.a
* libavutil.dll.a
* include/windows/libavcodec/
* include/windows/libavformat/
* include/windows/libavutil/

GLM is under [The Happy Bunny License](https://github.com/g-truc/glm/blob/master/copying.txt)
* include/windows/glm/

Dear ImGui is under the [MIT License](https://github.com/ocornut/imgui/blob/master/LICENSE.txt)
* src/imgui