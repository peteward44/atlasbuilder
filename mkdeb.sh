#!/bin/bash

checkinstall -D \
  --fstrans=yes \
  --pkgsource="https://github.com/peteward44/atlasbuilder/" \
  --pkglicense="MIT" \
  --deldesc=no \
  --nodoc \
  --maintainer="Pete Ward \\<peteward44@gmail.com\\>" \
  --pkgarch=$(dpkg \
  --print-architecture) \
  --pkgversion="1.0.0" \
  --pkgrelease="SNAPSHOT" \
  --pkgname=atlasbuilder \
  --requires="libc6 \(\>= 2.4\),libgcc1 \(\>= 1:4.1.1\),libstdc++6 \(\>= 4.1.1\),libvips42 \(\>= 8.2.2\),libboost-program-options1.58.0,libboost-system1.58.0,libboost-filesystem1.58.0"

