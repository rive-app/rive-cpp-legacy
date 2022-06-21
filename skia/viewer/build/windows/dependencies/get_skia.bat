@echo off
if not exist ".\skia" (
    git clone https://github.com/google/skia skia
) else (
    pushd skia
    git fetch
    git pull
    popd
)
pushd .\skia
git checkout chrome/m99
python3 tools/git-sync-deps
popd