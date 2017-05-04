#! /bin/bash

##
## Script made to boot from an iso cdrom file
##

if [ "x$1" = "x" ] ; then
	echo "usage: $0 kernel-ibmpc-x86.out [qemu options]"
	exit 0
else
	KERNEL=$1
fi

if [ "x$QEMU" = "x" ] ; then
	QEMU=$(type -p qemu 2>/dev/null)
fi

D=$(dirname $0)

TAR=tar
CP=cp
RM=rm
MKISOFS=mkisofs

ISO_NAME=mutekh.iso

$TAR xjf ${D}/iso.tar.bz2
$CP $KERNEL tmp_iso/boot/kernel-ibmpc-x86.out
$MKISOFS -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o $ISO_NAME tmp_iso/
$RM -r tmp_iso/

if [ -e "$QEMU" ] ; then
	$QEMU -boot d -cdrom $ISO_NAME "$@"
fi

