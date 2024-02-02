# Pitchstone-Editor
A free, multi-platform editor using Vulkan as its foundation

## Libraries Used

This software uses the Vulkan API, hosted outside the repo via the Vulkan SDK

SDL2 is under the [zlib license](https://www.libsdl.org/license.php), hosted outside the repo via the Vulkan SDK

## Building Instructions

### Windows

NOTE: The only tested compiler is MVSC for now

1) Install the Vulkan SDK, Which can be found here: https://vulkan.lunarg.com/sdk/home#windows (NOTE: Be sure to include glm and SDL2 as additional packages to install)

2) Install Visual Studio Community, and select at least the "Desktop Development with C++", containing MSBuild

3) Download CMake through here: https://cmake.org/download/. It must be of version at least 3.21

4) Run `"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe` and then append the parent directory of the result to the PATH environment variable

5) Set your current directory to the root of the repository

6) Run `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`. For release just change the `Debug` to `Release`, or you can just run `cmake -S . -B build` for debug

7) Run `MSBuild.exe '.\build\Pitchstone Editor.sln' /property:Configuration=Debug`, for release just change the `Debug` to `Release`

The resulting binary and resources should either be in `bin/Debug` or `bin/Release`

### Linux

NOTE: The only tested compiler is GCC for now

1) Install the Vulkan SDK, Which can be found here: https://vulkan.lunarg.com/sdk/home#linux. There are tarballs and Ubuntu packages available

2) Install the SDL2 library and OpenGL Mathematics (glm) library, typical package names are `libsdl2-dev` and `libglm-dev`

3) Install cmake through your package manager or tarball, its version must be at least 3.21

4) Set your current directory to the root of the repository

5) Run `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`. For release just change the `Debug` to `Release`, or you can just run `cmake -S . -B build` for debug

6) Run `make -C build`

The resulting binary and resources should either be in `bin/Debug` or `bin/Release`