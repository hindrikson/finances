#!/bin/bash

pushd ..
vendor/binaries/premake/linux/premake5 --cc=clang --file=Build.lua gmake2
popd
