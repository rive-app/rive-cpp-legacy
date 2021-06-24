
OPTION=$1


if [ "$OPTION" = 'help' ]; then
    echo runs.sh - runs debug viewer
    echo run.sh release - run release viewer
    echo run.sh debug - run viewer with debugger
elif [ "$OPTION" = "release" ]; then
    ./build/bin/release/rive_low_level_viewer $2
elif [ "$OPTION" = "debug" ]; then
    lldb ./build/bin/debug/rive_low_level_viewer $2
else
    ./build/bin/debug/rive_low_level_viewer $1
fi
