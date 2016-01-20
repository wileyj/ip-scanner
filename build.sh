#!/bin/sh
aclocal && automake --add-missing && autoconf
autoreconf
automake --add-missing
autoreconf
