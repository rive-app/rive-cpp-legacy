
OPTION=$1

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../dependencies/DiligentCore-MacOS-x64-Release/lib/Release

if [ "$OPTION" = 'help' ]; then
    echo build.sh - build debug library
    echo build.sh clean - clean the build
    echo build.sh release - build release library
elif [ "$OPTION" = "release" ]; then
    ./build/bin/release/rive_diligent_viewer
else
    ./build/bin/debug/rive_diligent_viewer
fi
