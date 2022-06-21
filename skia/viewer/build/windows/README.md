# Rive Windows Viewer

Main entry is [build.bat](build.bat).
Requirements:

- Git
  [Download and install](https://git-scm.com/download/win)
- CMake
  - [Download and install](https://cmake.org/download/).
- Python

  - Get [Python 3](https://www.python.org/downloads/). You may need to symlink python3 to python. Make sure to do the custom install and check "install for all users."
    ```
    mklink "C:\Program Files\Python310\python3.exe" "C:\Program Files\Python310\python.exe"
    ```

- Visual Studio 2022 Community Edition
  - Add clang tools
  - You may need to patch up the scripts if you have a different version.
- Install Google's [Depot Tools](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up) to build Skia.
