#!/bin/sh
set -e
if [[ -z "${DEPENDENCIES}" ]]; then
    echo "DEPENDENCIES env variable must be set. This script is usually called by other scripts."
    exit 1
fi

if [ $(uname) = Linux ]; then
    URL=https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-linux.tar.gz
else
    URL=https://github.com/premake/premake-core/releases/download/v5.0.0-beta1/premake-5.0.0-beta1-macosx.tar.gz
fi

mkdir -p $DEPENDENCIES/bin
echo Downloading Premake5
curl $URL -L -o $DEPENDENCIES//bin/premake_tmp.tar.gz
cd $DEPENDENCIES/bin
# Export premake5 into bin
tar -xvf premake_tmp.tar.gz 2>/dev/null
# Delete downloaded archive
rm premake_tmp.tar.gz
