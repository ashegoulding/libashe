#!/bin/bash
aclocal && libtoolize --copy && automake --add-missing --copy && autoconf
if [ $? -eq 0 ]; then
	exit 0
fi

REQ_PACKAGES="autoconf automake libtool"
echo -e "[en] Somethin went wrong.
\tThis project needs following packages to compile: $REQ_PACKAGES"\
>& 2

exit 1

