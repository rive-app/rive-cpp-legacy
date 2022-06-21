@echo off
if not exist ".\imgui" (
    git clone https://github.com/ocornut/imgui imgui
) else (
    pushd imgui
    git fetch
    git pull
    popd
)
pushd .\imgui
git checkout master
popd
