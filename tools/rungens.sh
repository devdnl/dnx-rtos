#!/usr/bin/env bash

for var in "$@"
do
    echo "Starting script: ${var}..."
    sh "$var" avs
done
