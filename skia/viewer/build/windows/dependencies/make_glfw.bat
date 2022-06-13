@echo off
if not exist ".\glfw" (
    git clone https://github.com/glfw/glfw glfw
) else (
    pushd glfw
    git fetch
    git pull
    popd
)
pushd .\glfw
git checkout master
popd

mkdir glfw_build
cd glfw_build
cmake ../glfw -DBUILD_SHARED_LIBS=OFF
rem Force compile with /MT, GLFW defaults to /MDd but the rest of our libs expect /MT
set _CL_=/MT
call "%MSBuild%" GLFW.sln 

