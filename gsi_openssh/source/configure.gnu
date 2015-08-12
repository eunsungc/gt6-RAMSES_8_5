#! /bin/sh

globus_common_path="$(pkg-config globus-common --variable=path)"
PATH="${PATH}${globus_common_path:+:$globus_common_path}"
srcdir="$(dirname $0)"

exec "${srcdir}/configure" ${1:+"$@"} --with-gsi=yes --sysconfdir="\${prefix}/etc/gsissh" --with-privsep-path="\${prefix}/var/empty"
