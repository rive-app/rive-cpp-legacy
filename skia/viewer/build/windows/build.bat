@echo off
 set "DEPENDENCIES_DIR=%cd%\dependencies"
set "BAT_DIR=%cd%"

if not exist "%DEPENDENCIES_DIR%\bin\premake5.exe" (
    pushd "%DEPENDENCIES_DIR%"
    call .\get_premake5.bat || goto :error
    popd
)
if not exist "%DEPENDENCIES_DIR%\skia" goto :get_skia
if "%FORCE_BUILD%"=="true" goto :get_skia
goto :got_skia

:get_skia
pushd "%DEPENDENCIES_DIR%"
call .\get_skia.bat || goto :error
popd

:got_skia

if not exist "%DEPENDENCIES_DIR%\skia\out\static\skia.lib" goto :build_skia
if "%FORCE_BUILD%"=="true" goto :build_skia
goto :built_skia

:build_skia
pushd "%DEPENDENCIES_DIR%"
call .\make_skia.bat || goto :error
popd

:built_skia

if not exist "%DEPENDENCIES_DIR%\skia" (
    pushd "%DEPENDENCIES_DIR%"
    call .\get_skia.bat || goto :error
    popd
)

if not exist "%DEPENDENCIES_DIR%\gl3w" (
    pushd "%DEPENDENCIES_DIR%"
    call .\make_gl3w.bat || goto :error
    popd
)

if not exist "%DEPENDENCIES_DIR%\imgui" (
    pushd "%DEPENDENCIES_DIR%"
    call .\get_imgui.bat || goto :error
    popd
)

export "MSBuild=%ProgramFiles%\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe"

if not exist "%DEPENDENCIES_DIR%\glfw" (
    pushd "%DEPENDENCIES_DIR%"
    call .\make_glfw.bat || goto :error
    popd
)

set "PREMAKE=%DEPENDENCIES_DIR%\bin\premake5.exe"
%PREMAKE% --file=premake5_win.lua vs2022

call "%MSBuild%" rive.sln 

if not exist "%BAT_DIR%\..\..\..\package\Runtime\Libraries\Windows" mkdir %BAT_DIR%\..\..\..\package\Runtime\Libraries\Windows
copy .\bin\debug\rive.dll %BAT_DIR%\..\..\..\package\Runtime\Libraries\Windows\rive.dll

goto :end

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%

:end