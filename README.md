# Pitchstone-Editor
A free, multi-platform editor using Vulkan as its foundation

## Libraries Used

This software uses the Vulkan API, hosted outside the repo via the Vulkan SDK

SDL2 is under the [zlib license](https://www.libsdl.org/license.php), hosted outside the repo via the Vulkan SDK

## Building Instructions

### Windows

1) Install the Vulkan SDK, Which can be found here: https://vulkan.lunarg.com/sdk/home#windows (NOTE: Be sure to include glm and SDL2 as additional packages to install)

2) Install Visual Studio Community, and select at least the "Desktop Development with C++", containing MSBuild

3) Run `"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe` and then append the parent directory of the result to the PATH environment variable

4) Set your current directory to the root of the repository

5) Run `cmake -S . -B build`

6) Run `MSBuild.exe '.\build\Pitchstone Editor.sln' --config Debug`, for release just change the `Debug` to `Release`

The resulting binary should either be in `bin/Debug` or `bin/Release`

### Linux

Instructions to be added later