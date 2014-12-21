#!/bin/bash

VERSION=`cat DESCRIPTION |grep Version|cut -f 2 -d " "`
LIBNAME=rPithon

CURDIR=`pwd`
TMPDIR=`tempfile`
rm -r $TMPDIR
if ! mkdir $TMPDIR ; then
	echo "Couldn't create temporary directory"
	exit -1
fi

if ! hg archive $TMPDIR/${LIBNAME}-${VERSION} ; then
	echo "Couldn't export repository"
	exit -1
fi

cd $TMPDIR

rm -f `find ${LIBNAME}-${VERSION} -name ".hg*"`
rm ${LIBNAME}-${VERSION}/builddist.sh
(cd ${LIBNAME}-${VERSION}/inst/doc ; pdflatex rPithon.tex ; pdflatex rPithon.tex ; pdflatex rPithon.tex )
	
if ! tar cfz ${LIBNAME}-${VERSION}.tar.gz ${LIBNAME}-${VERSION}/ ; then
	echo "Couldn't create archive"
	exit -1
fi

mv $TMPDIR $CURDIR/

