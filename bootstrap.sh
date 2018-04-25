#!/bin/bash
aclocal &&
    libtoolize --copy &&
    automake --add-missing --copy &&
    autoconf
