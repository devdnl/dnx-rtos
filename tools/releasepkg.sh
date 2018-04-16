#!/usr/bin/env bash

# Script create release package (script should be started from root dir).

get_version() {
    cat src/system/include/libc/dnx/os.h \
    | grep get_OS_version\(void\) -m 1 -A 3 \
    | grep return \
    | sed -e 's/.*return\s"//' -e 's/";//' -e 's/\s/_/'
}

GITHASH=$(git rev-parse --short HEAD)
VERSION="dnx-RTOS-v$(get_version)-g${GITHASH}"

git clean -xfd
zip ${VERSION}.zip . -r -9 -x /.git* /*git
zip ${VERSION}.zip .gitignore
echo "Done"
