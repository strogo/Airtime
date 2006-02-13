#!/bin/sh
#-------------------------------------------------------------------------------
#   Copyright (c) 2004 Media Development Loan Fund
#
#   This file is part of the LiveSupport project.
#   http://livesupport.campware.org/
#   To report bugs, send an e-mail to bugs@campware.org
#
#   LiveSupport is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   LiveSupport is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with LiveSupport; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#
#   Author   : $Author$
#   Version  : $Revision$
#   Location : $URL$
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  System V runlevel style invoke script for the LiveSupport Scheduler
#  This script is only used in the LiveSupport development environment
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

if [ -x $tmpdir/gLiveSupport ]; then
    gLiveSupport_exe=$tmpdir/gLiveSupport
else
    echo "Can't find scheduler executable.";
fi

if [ -f ~/.livesupport/gLiveSupport.xml ]; then
    config_file=~/.livesupport/gLiveSupport.xml
elif [ -f $etcdir/gLiveSupport.xml ]; then
    config_file=$etcdir/gLiveSupport.xml
else
    echo "Can't find configuration file.";
fi

mode=$1

echo "using configuration file:  $config_file";

$gLiveSupport_exe -c $config_file
