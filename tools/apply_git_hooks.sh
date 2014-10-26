#!/usr/bin/env bash

cd $(dirname $0)

git_hook_path=../extra/git_hooks/commit-msg

cp -u $git_hook_path ../.git/hooks
