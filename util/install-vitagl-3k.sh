#!/usr/bin/env bash
set -o nounset
: ${VITASDK:?'$VITASDK is not set'}
trap "rm -rf ${tmpdir:=$(mktemp -d)}" EXIT
echo $tmpdir
cd $tmpdir

git clone https://github.com/Rinnegatamante/vitaShaRK
pushd vitaShaRK
sed -i'' 's@^//#define DISABLE_SHACCCG_EXTENSIONS@#define DISABLE_SHACCCG_EXTENSIONS@' \
    -- source/vitashark.c
grep '#define DISABLE_SHACCCG_EXTENSIONS' source/vitashark.c
make && make install
popd
echo 'Cleaning up vitaShaRK'
rm -rf vitaShaRK

git clone https://github.com/Rinnegatamante/vitaGL
pushd vitaGL
make clean
export HAVE_VITA3K_SUPPORT=1
# export HAVE_GLSL_SUPPORT=1
make && make install
popd
echo 'Cleaning up vitaGL'
rm -rf vitaGL

echo "vitaGL with Vita3K support installed!"
echo "Cleaning up $tmpdir"
rm -rf "$tmpdir"
echo "Done!"