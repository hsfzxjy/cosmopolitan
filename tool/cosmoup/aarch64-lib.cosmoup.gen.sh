#!/bin/bash

VARIANT="$1"

cat <<EOF
link . aarch64.lds aarch64-linux-cosmo/lib/${VARIANT}/%
link . crt.o aarch64-linux-cosmo/lib/${VARIANT}/%
link . crtfastmath.o aarch64-linux-cosmo/lib/${VARIANT}/%
link . libcxx.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . libcosmo.a aarch64-linux-cosmo/lib/${VARIANT}/%
EOF

if [ ! -z "$VARIANT" ]; then
  exit
fi

cat <<EOF
link . libc.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . libcrypt.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . libdl.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . libgcc_s.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . libgomp.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . libm.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . libpthread.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . libresolv.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . librt.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . libstdc++.a aarch64-linux-cosmo/lib/${VARIANT}/%
link . libunwind.a aarch64-linux-cosmo/lib/${VARIANT}/%
EOF
