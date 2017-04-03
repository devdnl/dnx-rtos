#!/usr/bin/env bash

# Script create release package (script should be started from root dir).

get_version() {
    cat src/system/include/libc/dnx/os.h \
    | grep get_OS_version\(void\) -m 1 -A 3 \
    | grep return \
    | sed -e 's/.*return\s"//' -e 's/";//' -e 's/\s/_/'
}

VERSION=$(get_version)

git clean -xfd
tar --exclude=.git -zcvf ${VERSION}.tar.gz .
zip ${VERSION}.zip . -r -9 --exclude /.git* --exclude ${VERSION}.tar.gz
echo "Done"
