![Build Status](https://github.com/rive-app/rive-cpp/actions/workflows/tests.yml/badge.svg) 
![Discord badge](https://img.shields.io/discord/532365473602600965)
![Twitter handle](https://img.shields.io/twitter/follow/rive_app.svg?style=social&label=Follow)


# rive-cpp
C++ runtime for [Rive](https://rive.app). Provides these runtime features:
- Loading Artboards and their contents from **.riv** files.
- Querying LinearAnimations and StateMachines from Artboards.
- Making changes to Artboard hierarchy (fundamentally same guts used by LinearAnimations and StateMachines) and effienclty solving those changes via Artboard::advance.
- Abstract Renderer for submitting high level vector path commands with retained path objects to optimize and minimize path re-computation (ultimately up to the concrete rendering implementation).
- Example concrete renderer written in C++ with [Skia](https://skia.org/). Skia renderer code is in [skia/renderer/src/skia_renderer.cpp](skia/renderer/src/skia_renderer.cpp).

## Build System
We use [premake5](https://premake.github.io/). The Rive dev team primarily works on MacOS. There is some work done by the community to also support Windows and Linux. PRs welcomed for specific platforms you with to support! We encourage you to use premake as it's highly extensible and configurable for a variety of platforms.

## Build
In the ```rive-cpp``` directory, run ```build.sh``` to debug build and ```build.sh release``` for a release build.

If you've put the `premake5` executable in the `rive-cpp/build` folder, you can run it with `PATH=.:$PATH ./build.sh`

## Testing
Uses the [Catch2](https://github.com/catchorg/Catch2) testing framework.

```
cd dev
./test.sh
```

In the ```dev``` directory, run ```test.sh``` to compile and execute the tests.

(if you've installed `premake5` in `rive-cpp/build`, you can run it with `PATH=../../build:$PATH ./test.sh`)

The tests live in ```rive/test```. To add new tests, create a new ```xxx_test.cpp``` file here. The test harness will automatically pick up the new file.

There's a VSCode command provided to ```run tests``` from the Tasks: Run Task command palette. 

## Code Formatting
rive-cpp uses clang-format, you can install it with brew on MacOS: ```brew install clang-format```.

## Memory Checks
Note that if you're on MacOS you'll want to install valgrind, which is somewhat complicated these days. This is the easiest solution (please PR a better one when it becomes available).
```
brew tap LouisBrunner/valgrind
brew install --HEAD LouisBrunner/valgrind/valgrind
```
You can now run the all the tests through valgrind by running ```test.sh memory```.

## Disassembly Explorer
If you want to examine the generated assembly code per cpp file, install [Disassembly Explorer](https://marketplace.visualstudio.com/items?itemName=dseight.disasexpl) in VSCode.

A ```disassemble``` task is provided to compile and preview the generated assembly. You can reach it via the Tasks: Run Task command palette or you can bind it to a shortcut by editing your VSCode keybindings.json:
```
[
    {
        "key": "cmd+d",
        "command": "workbench.action.tasks.runTask",
        "args": "disassemble"
    }
]
```
