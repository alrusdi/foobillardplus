#!/bin/sh
aclocal --force  
autoconf -f   
autoheader -f   
automake -a -c -f
