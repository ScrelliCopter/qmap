#!/bin/sh
set -e

EMSDK_PATH="$HOME/Git/emsdk"
BUILDDIR=embuild

mkdir -p "$BUILDDIR"
cp resources/* "$BUILDDIR/"
for i in palette.lmp colormap.lmp maps/e1m1.bsp; do cp "$i" "$BUILDDIR/"; done
. "$EMSDK_PATH/emsdk_env.sh"
emcmake cmake -B "$BUILDDIR" -DCMAKE_BUILD_TYPE=Release
emmake make -C "$BUILDDIR"
