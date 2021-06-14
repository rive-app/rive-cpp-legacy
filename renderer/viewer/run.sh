
OPTION=$1

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../dependencies/DiligentEngine_build/build/lib/DiligentCore/Debug/

if [ "$OPTION" = 'help' ]; then
    echo build.sh - build debug library
    echo build.sh clean - clean the build
    echo build.sh release - build release library
elif [ "$OPTION" = "release" ]; then
    ./build/bin/release/rive_diligent_viewer
else
    ./build/bin/debug/rive_diligent_viewer '-mode gl'
fi
