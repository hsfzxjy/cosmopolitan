#!/bin/bash

VARIANT="$1"

cat <<EOF
link . ape-no-modify-self.o x86_64-linux-cosmo/lib/${VARIANT}/%
link . ape.lds x86_64-linux-cosmo/lib/${VARIANT}/%
link . ape.o x86_64-linux-cosmo/lib/${VARIANT}/%
link . crt.o x86_64-linux-cosmo/lib/${VARIANT}/%
link . crtfastmath.o x86_64-linux-cosmo/lib/${VARIANT}/%
link . libcxx.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . libcosmo.a x86_64-linux-cosmo/lib/${VARIANT}/%
EOF

if [ ! -z "$VARIANT" ]; then
  exit
fi

cat <<EOF
link . libc.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . libcrypt.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . libdl.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . libgcc_s.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . libgomp.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . libm.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . libpthread.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . libresolv.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . librt.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . libstdc++.a x86_64-linux-cosmo/lib/${VARIANT}/%
link . libunwind.a x86_64-linux-cosmo/lib/${VARIANT}/%
EOF
