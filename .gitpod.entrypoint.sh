#!/usr/bin/env bash
set -e

. $EMSDK_PATH/emsdk_env.sh 2>/dev/null

exec "$@"
