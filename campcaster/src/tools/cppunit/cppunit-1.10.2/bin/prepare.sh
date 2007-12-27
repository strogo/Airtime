#!/bin/bash
#-------------------------------------------------------------------------------
#   Copyright (c) 2004 Media Development Loan Fund
#
#   This file is part of the Campcaster project.
#   http://campcaster.campware.org/
#   To report bugs, send an e-mail to bugs@campware.org
#
#   Campcaster is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   Campcaster is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with Campcaster; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#
#   Author   : $Author$
#   Version  : $Revision$
#   Location : $URL$
#-------------------------------------------------------------------------------                                                                                
#-------------------------------------------------------------------------------
# Run this script to prepare cppunit to be configured and compiled.
# For more information on CppUnit, see http://cppunit.sourceforge.net/
#-------------------------------------------------------------------------------
product=cppunit-1.10.2

reldir=`dirname $0`/..
basedir=`cd $reldir; pwd;`
bindir=$basedir/bin
etcdir=$basedir/etc
tmpdir=$basedir/tmp

tar=$basedir/src/$product.tar.gz

mkdir -p $tmpdir
cd $tmpdir

# copy over install-sh, as AC_CONFIG_SUBDIRS will be looking for it
cp -r $bindir/install-sh $tmpdir

# untar and patch the sources
if [ ! -d $product ]; then
    tar xfz $tar
    cd $product
    # see http://sourceforge.net/tracker/index.php?func=detail&aid=997006&group_id=11795&atid=311795
    patch -p1 < $etcdir/cppunit-1.10.2-nostandalone.patch
fi

