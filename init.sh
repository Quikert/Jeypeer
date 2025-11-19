#!/usr/bin/env bash
set -e

ROOT="$(cd "$(dirname "$0")" && pwd)"
LOGS="./logs"
C_DIR="./c"

mkdir -p "$LOGS"
: > "$LOGS/blocks.log"
: > "$LOGS/process.log"
: > "$LOGS/tokens.log"

echo "[Build] Compiling"
cc -std=c11 -O2 -Wall -Wextra \
  "$C_DIR/blocks.c" "$C_DIR/chains.c" "$C_DIR/peers.c" "$C_DIR/tokens.c" "$C_DIR/p2p.c" "$C_DIR/shellhash.c" "$C_DIR/process.c" \
  -o "$ROOT/jeypeer"

echo "[Init] Creatind 'Genesis block'"
"$ROOT/jeypeer" init

echo "[Done] Usage ./jeypeer help"
