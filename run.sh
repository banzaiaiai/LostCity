#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"

[ -d build ] || cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"

GALLIUM_DRIVER=llvmpipe EGL_LOG_LEVEL=fatal ./build/bin/app "$@"