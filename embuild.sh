#!/usr/bin/env bash
set -e

#mkdir -p embuild
#cp resources/* embuild/
#pushd embuild
source "$HOME/repos/emsdk/emsdk_env.sh"
#emcmake cmake -DCMAKE_BUILD_TYPE=Release ..
#emmake make
#popd

emmake make "$@" \
	TARGET="qmap-embuild.html" \
	LDFLAGS="--preload-file ./resources/palette.lmp@ --preload-file ./resources/colormap.lmp@ --preload-file ./resources/e1m1.bsp@" \
	-j"$(nproc)"
