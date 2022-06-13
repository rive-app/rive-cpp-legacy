@echo off
if not exist ".\gl3w" (
    git clone https://github.com/skaslev/gl3w gl3w
) else (
    cd gl3w
    git fetch
    git pull
)
pushd .\gl3w
if not exist ".\build" mkdir build
pushd build
cmake ../
set "MSBuild=%ProgramFiles%\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe"
call "%MSBuild%" gl3w.sln
popd
popd