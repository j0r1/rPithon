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

rm rPithon_*.tar.gz
( cd $TMPDIR/pkg/ ; R CMD build . && mv rPithon_*.tar.gz $CURDIR )
tar xfz rPithon_*.tar.gz rPithon/inst/doc/rPithon.html
mv rPithon/inst/doc/rPithon.html www/index.html
rm -rf rPithon
rm -rf $TMPDIR

