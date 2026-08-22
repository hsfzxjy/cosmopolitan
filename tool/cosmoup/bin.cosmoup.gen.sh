#!/bin/bash

BINPATH=$1

for arch in x86_64 aarch64; do
  for x in cc c++; do
    echo "alias bin/cosmocross bin/$arch-unknown-cosmo-$x"
  done
  for x in addr2line ar as c++filt install ld nm objcopy objdump readelf ranlib strip; do
    if [ -f "$BINPATH/$arch-unknown-cosmo-$x" ]; then
      continue
    fi
    echo "alias bin/$arch-linux-cosmo-$x bin/$arch-unknown-cosmo-$x"
  done
done

cat <<EOF
alias bin/cosmoar bin/unknown-unknown-cosmo-ar
alias bin/cosmocc bin/unknown-unknown-cosmo-c++
alias bin/cosmocc bin/unknown-unknown-cosmo-cc
alias bin/cosmoinstall bin/unknown-unknown-cosmo-install
alias bin/cosmoranlib bin/unknown-unknown-cosmo-ranlib
alias bin/cosmocc bin/cosmoc++
EOF

for x in $(ls $BINPATH); do
  # starts with ape-
  if [ x"$x" != x"${x#ape-}" ]; then
    echo "link . $x bin/%"
  # content startswith "#!/bin"
  elif [ -f "$BINPATH/$x" ] && head -n1 "$BINPATH/$x" | grep -q '^#!/bin'; then
    echo "shexec . $x bin/%"
  # end with .cosmoup
  elif [ x"$x" != x"${x%.cosmoup}" ]; then
    continue
  # is an executable
  else
    echo "exec . $x bin/%"
  fi
done
