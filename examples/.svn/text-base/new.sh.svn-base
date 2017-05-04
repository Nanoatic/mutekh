#!/bin/sh

name="$1"

if [ "x$name" = x ] ; then
    echo "usage: $0 new_program_name"
    exit
fi

if [ -e "$name" ] ; then
    echo "error: '$name' directory already exists !"
    exit
fi

mkdir "$name"
(
    cd "$name"
    tar --strip 1 -xf ../template.tar
    for i in config_* ; do 
	sed -i "s/template/$name/g" "$i"
    done
)

echo "Done, ready to compile with CONF=examples/$name/config_{soclib,emu,x86}"

