#!/bin/bash
EXEC_PATH=$(dirname "$0")
export LD_LIBRARY_PATH="${EXEC_PATH}/;${EXEC_PATH}/asi-plugins"
eval "${EXEC_PATH}/asiExeServer \"$@\""