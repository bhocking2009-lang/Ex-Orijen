#!/usr/bin/env sh
set -eu
SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
OUT_DIR=${OUT_DIR:-build/v1_0_verify}
export OUT_DIR
"$SCRIPT_DIR/verify_v0_1.sh"
