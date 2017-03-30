#!/usr/bin/env bash

cd $(dirname $0)

if [ -d "../.git/hooks" ] && [ ! -f ../.git/hooks/commit-msg ]; then

    echo "Adding git commit hook..."

cat << 'EOF' > ../.git/hooks/commit-msg
#!/bin/sh
#
# Script adds branch name at the beginning of the commit message

git_branch()
{
    local b="$(git symbolic-ref HEAD 2> /dev/null)";
    if [ -n "$b" ]; then
        printf "%s:" "${b##refs/heads/}";
    fi
}

msg=$(cat $1)
echo "$(git_branch) $msg" > $1
exit 0
EOF

    chmod +x ../.git/hooks/commit-msg

fi
