#!/bin/bash
#-------------------------------------------------------------------------------#   Copyright (c) 2004 Media Development Loan Fund
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
#  Run the test suite for the scheduler.
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#  Determine directories, files
#-------------------------------------------------------------------------------
reldir=`dirname $0`/..
basedir=`cd $reldir; pwd;`
bindir=$basedir/bin
etcdir=$basedir/etc
libdir=$basedir/lib
tmpdir=$basedir/tmp

usrdir=`cd $basedir/../../../usr; pwd;`


#-------------------------------------------------------------------------------
#  Set up the environment
#-------------------------------------------------------------------------------
export LD_LIBRARY_PATH=$usrdir/lib:$LD_LIBRARY_PATH

if [ -x $tmpdir/testRunner ]; then
    testRunner=$tmpdir/testRunner
else
    echo "Can't find testRunner executable.";
fi

if [ -f ~/.campcaster/campcaster-scheduler.xml ]; then
    config_file=~/.campcaster/campcaster-scheduler.xml
elif [ -f $etcdir/campcaster-scheduler.xml ]; then
    config_file=$etcdir/campcaster-scheduler.xml
else
    echo "Can't find configuration file.";
fi


#-------------------------------------------------------------------------------
#  Run the tests
#-------------------------------------------------------------------------------
$testRunner -c $config_file ${*}

