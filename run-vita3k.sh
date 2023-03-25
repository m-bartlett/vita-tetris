#!/usr/bin/env bash

## systemctl restart --user xdg-desktop-portal

set -o errexit

SOURCE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

VITA_VPK_ID='VITAGL005'

VITA3K_DIR="$HOME/Projects/vita/3K"
VITA3K_SDK_DIR="$VITA3K_DIR/sdk"
source "$VITA3K_SDK_DIR/vita-env"
VPK_INSTALL_DIR="$VITA3K_DIR/config/ux0/app/$VITA_VPK_ID"

make
cp -v "$SOURCE_DIR/eboot.bin" "$VPK_INSTALL_DIR/"
cp -v "$SOURCE_DIR/param.sfo" "$VPK_INSTALL_DIR/sce_sys/"
cd "$VITA3K_DIR"
./Vita3K -B Vulkan --installed-path $VITA_VPK_ID