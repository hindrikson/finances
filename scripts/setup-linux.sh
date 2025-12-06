#!/bin/bash

pushd ..
vendor/binaries/premake/linux/premake5 --verbose --cc=clang --file=Build.lua gmake2
popd
