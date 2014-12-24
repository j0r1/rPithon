#!/bin/bash

CURDIR=`pwd`
TMPDIR=`tempfile`
rm -r $TMPDIR
if ! mkdir $TMPDIR ; then
	echo "Couldn't create temporary directory"
	exit -1
fi

if ! hg archive $TMPDIR/ ; then
	echo "Couldn't export repository"
	exit -1
fi

( cd $TMPDIR/pkg/ ; R CMD build . && mv rPithon_*.tar.gz $CURDIR )
rm -rf $TMPDIR

