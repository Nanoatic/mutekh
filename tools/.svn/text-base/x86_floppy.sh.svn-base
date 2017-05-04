if ! test -f x86_floppy.img ; then
    cp tools/x86_floppy.img.bz2 .
    bunzip2 x86_floppy.img.bz2
fi
mcopy -o -i x86_floppy.img kernel-ibmpc-x86.out ::
qemu -fda x86_floppy.img $*
