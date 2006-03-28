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
#   Author   : $Author: fgerlits $
#   Version  : $Revision: 1842 $
#   Location : $URL: svn+ssh://maroy@code.campware.org/home/svn/repo/livesupport/trunk/livesupport/src/tools/gstreamer/gstreamer-0.8.10/bin/prepare.sh $
#-------------------------------------------------------------------------------                                                                                
#-------------------------------------------------------------------------------
# Run this script to prepare gstreamer to be configured and compiled.
# To read more about gstreamer, see http://gstreamer.freedesktop.org/
#-------------------------------------------------------------------------------
liboil=liboil-0.3.6
gstreamer=gstreamer-0.10.4
plugins_base=gst-plugins-base-0.10.5
plugins_good=gst-plugins-good-0.10.2
plugins_ugly=gst-plugins-ugly-0.10.2

reldir=`dirname $0`/..
basedir=`cd ${reldir}; pwd;`
bindir=${basedir}/bin
etcdir=${basedir}/etc
tmpdir=${basedir}/tmp

liboil_tar=${basedir}/src/${liboil}.tar.gz
gstreamer_tar=${basedir}/src/${gstreamer}.tar.bz2
plugins_base_tar=${basedir}/src/${plugins_base}.tar.bz2
plugins_good_tar=${basedir}/src/${plugins_good}.tar.bz2
plugins_ugly_tar=${basedir}/src/${plugins_ugly}.tar.bz2

mkdir -p ${tmpdir}

# copy over install-sh, as AC_CONFIG_SUBDIRS will be looking for it
cp -r $bindir/install-sh $tmpdir

cd ${tmpdir}
if [ ! -d $liboil ]; then
    tar xfz ${liboil_tar}
    cd ${liboil}
    # patch here
fi

cd ${tmpdir}
if [ ! -d $gstreamer ]; then
    tar xfj ${gstreamer_tar}
    cd ${gstreamer}
    # patch here
fi

cd ${tmpdir}
if [ ! -d $plugins_base ]; then
    tar xfj ${plugins_base_tar}
    cd ${plugins_base}
    # patch here
fi

cd ${tmpdir}
if [ ! -d $plugins_good ]; then
    tar xfj ${plugins_good_tar}
    cd ${plugins_good}
    # patch here
fi

cd ${tmpdir}
if [ ! -d $plugins_ugly ]; then
    tar xfj ${plugins_ugly_tar}
    cd ${plugins_ugly}
    # patch here
fi

