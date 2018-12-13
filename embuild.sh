#!/usr/bin/env bash
mkdir -p embuild
cp resources/* embuild/
cd embuild
source ~/Git/emsdk/emsdk_env.sh
emcmake cmake -DCMAKE_BUILD_TYPE=Release ..
emmake make
