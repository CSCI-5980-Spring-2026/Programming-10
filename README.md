# Programming 10/11: Physics System Integration

In these live programming class, we will begin to integrate the [Bullet Physics SDK](https://github.com/bulletphysics/bullet3).  I will also demonstrate how to use agentic coding tools to create implementation plans for complex software architectures.

## Prerequisites

To work with this code, you will first need to install the following software (same or newer versions):

- [Visual Studio Code](https://code.visualstudio.com/)
- [CMake 4.2.1](https://cmake.org/download/)
- [LLVM 21.1.8](https://github.com/llvm/llvm-project/releases/tag/llvmorg-21.1.8)
- [Ninja 1.13.2](https://github.com/ninja-build/ninja/releases/tag/v1.13.2)

On Windows 11, the easiest way to install these is to use the winget package manager.

```
winget install code
winget install cmake
winget install llvm
winget install Ninja-build.Ninja
```

**Important:** You will need to add the LLVM binary path to your system path environment variable. By default, this is installed in `C:\Program Files\LLVM\bin`.

## Visual Studio Code Extensions

You will also need to install the following VS code extensions:

- [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
- [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb)

## MSVC Platform Libraries

Although we are using the LLVM compiler, our application will still need to link to the MSVC and Windows SDK libraries. Therefore, you will need to install [Microsoft Visual Studio Community](https://visualstudio.microsoft.com/downloads/?q=build+tools) even though we are not using it as an IDE (there does not appear to be any way around this on Windows). In the installer, make sure to check *Desktop development for C++*, and then it will install the required dependencies.

## Acknowledgments

Texture assets were obtained from [3dtextures.me](https://3dtextures.me/).

## License

Material for [CSCI 5980 Spring 2026](https://github.com/CSCI-5980-Spring-2026/Syllabus) by [Evan Suma Rosenberg](https://illusioneering.umn.edu/) is licensed under a [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-nc-sa/4.0/).
