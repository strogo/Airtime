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
#   Author   : $Author: sebastian $
#   Version  : $Revision: 1.9.2.1 $
#   Location : $Source: /home/paul/cvs2svn-livesupport/newcvsrepo/livesupport/bin/Attic/postInstallScheduler.sh,v $
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#  This script makes post-installation steps for the LiveSupport scheduler.
#
#  Invoke as:
#  ./bin/postInstallScheduler.sh
#
#  To get usage help, try the -h option
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Determine directories, files
#-------------------------------------------------------------------------------
reldir=`dirname $0`/..
basedir=`cd $reldir; pwd;`
bindir=$basedir/bin
etcdir=$basedir/etc
docdir=$basedir/doc
tmpdir=$basedir/tmp
usrdir=$basedir/usr


#-------------------------------------------------------------------------------
#  Print the usage information for this script.
#-------------------------------------------------------------------------------
printUsage()
{
    echo "LiveSupport scheduler post-install script.";
    echo "parameters";
    echo "";
    echo "  -d, --directory     The installation directory, required.";
    echo "  -D, --database      The name of the LiveSupport database.";
    echo "                      [default: LiveSupport]";
    echo "  -g, --apache-group  The group the apache daemon runs as.";
    echo "                      [default: apache]";
    echo "  -H, --host          The fully qualified host name of the system";
    echo "                      [default: guess].";
    echo "  -p, --port          The port of the apache web server [default: 80]"
    echo "  -P, --scheduler-port    The port of the scheduler daemon to install"
    echo "                          [default: 3344]";
    echo "  -r, --www-root      The root directory for web documents served";
    echo "                      by apache [default: /var/www]";
    echo "  -s, --dbserver      The name of the database server host.";
    echo "                      [default: localhost]";
    echo "  -u, --dbuser        The name of the database user to access the"
    echo "                      database. [default: livesupport]";
    echo "  -w, --dbpassword    The database user password.";
    echo "                      [default: livesupport]";
    echo "  -o, --output-device The audio device of broadcast";
    echo "                      [default: plughw:0,0]";
    echo "  -h, --help          Print this message and exit.";
    echo "";
}


#-------------------------------------------------------------------------------
#  Process command line parameters
#-------------------------------------------------------------------------------
CMD=${0##*/}

opts=$(getopt -o d:D:g:H:hp:P:r:s:u:w:o: -l apache-group:,database:,dbserver:,dbuser:,dbpassword:,directory:,host:,help,port:,scheduler-port:,www-root:,output-device -n $CMD -- "$@") || exit 1
eval set -- "$opts"
while true; do
    case "$1" in
        -d|--directory)
            installdir=$2;
            shift; shift;;
        -D|--database)
            database=$2;
            shift; shift;;
        -g|--apache-group)
            apache_group=$2;
            shift; shift;;
        -H|--host)
            hostname=$2;
            shift; shift;;
        -h|--help)
            printUsage;
            exit 0;;
        -p|--port)
            http_port=$2;
            shift; shift;;
        -P|--scheduler-port)
            scheduler_port=$2;
            shift; shift;;
        -r|--www-root)
            www_root=$2;
            shift; shift;;
        -s|--dbserver)
            dbserver=$2;
            shift; shift;;
        -u|--dbuser)
            dbuser=$2;
            shift; shift;;
        -w|--dbpassword)
            dbpassword=$2;
            shift; shift;;
        -o|--output-device)
            output_alsa_device=$2;
            shift; shift;;
        --)
            shift;
            break;;
        *)
            echo "Unrecognized option $1.";
            printUsage;
            exit 1;
    esac
done

if [ "x$installdir" == "x" ]; then
    echo "Required parameter install directory not specified.";
    printUsage;
    exit 1;
fi

if [ "x$hostname" == "x" ]; then
    hostname=`hostname -f`;
fi

if [ "x$http_port" == "x" ]; then
    http_port=80;
fi

if [ "x$scheduler_port" == "x" ]; then
    scheduler_port=3344;
fi

if [ "x$dbserver" == "x" ]; then
    dbserver=localhost;
fi

if [ "x$database" == "x" ]; then
    database=LiveSupport;
fi

if [ "x$dbuser" == "x" ]; then
    dbuser=livesupport;
fi

if [ "x$dbpassword" == "x" ]; then
    dbpassword=livesupport;
fi

if [ "x$apache_group" == "x" ]; then
    apache_group=apache;
fi

if [ "x$www_root" == "x" ]; then
    www_root=/var/www
fi

if [ "x$output_alsa_device" == "x" ]; then
    output_alsa_device="plughw:0,0";
fi

echo "Making post-install steps for the LiveSupport scheduler.";
echo "";
echo "Using the following installation parameters:";
echo "";
echo "  installation directory: $installdir";
echo "  host name:              $hostname";
echo "  web server port:        $http_port";
echo "  scheduler port:         $scheduler_port";
echo "  database server:        $dbserver";
echo "  database:               $database";
echo "  database user:          $dbuser";
echo "  database user password: $dbpassword";
echo "  apache daemon group:    $apache_group";
echo "  apache document root:   $www_root";
echo "  broadcast device:       $output_alsa_device";
echo ""

#-------------------------------------------------------------------------------
#  The details of installation
#-------------------------------------------------------------------------------
ls_php_host=$hostname
ls_php_port=$http_port
ls_php_urlPrefix=livesupport

ls_alib_xmlRpcPrefix="xmlrpc/xrLocStor.php"
ls_storage_xmlRpcPrefix="xmlrpc/xrLocStor.php"

ls_dbserver=$dbserver
ls_dbuser=$dbuser
ls_dbpassword=$dbpassword
ls_database=$database

ls_scheduler_host=$hostname
ls_scheduler_port=$scheduler_port
ls_scheduler_urlPrefix=
ls_scheduler_xmlRpcPrefix=RC2
ls_output_alsa_device=$output_alsa_device


postgres_user=postgres

install_bin=$installdir/bin
install_etc=$installdir/etc
install_lib=$installdir/lib
install_tmp=$installdir/tmp
install_usr=$installdir/usr
install_var=$installdir/var


# replace / characters with a \/ sequence, for sed below
# the sed statement is really "s/\//\\\\\//g", but needs escaping because of
# bash, hence the extra '\' characters
installdir_s=`echo $installdir | sed -e "s/\//\\\\\\\\\//g"`
ls_storage_xmlRpcPrefix_s=`echo $ls_storage_xmlRpcPrefix | \
                                sed -e "s/\//\\\\\\\\\//g"`
ls_alib_xmlRpcPrefix_s=`echo $ls_alib_xmlRpcPrefix | sed -e "s/\//\\\\\\\\\//g"`
ls_php_urlPrefix_s=`echo $ls_php_urlPrefix | sed -e "s/\//\\\\\\\\\//g"`
ls_scheduler_urlPrefix_s=`echo $ls_scheduler_urlPrefix | \
                                sed -e "s/\//\\\\\\\\\//g"`
ls_scheduler_xmlRpcPrefix_s=`echo $ls_scheduler_xmlRpcPrefix | \
                                sed -e "s/\//\\\\\\\\\//g"`
ls_output_alsa_device_s=`echo $ls_output_alsa_device | sed -e "s/\//\\\\\\\\\//g"`

replace_sed_string="s/ls_install_dir/$installdir_s/; \
              s/ls_dbuser/$ls_dbuser/; \
              s/ls_dbpassword/$ls_dbpassword/; \
              s/ls_dbserver/$ls_dbserver/; \
              s/ls_database/$ls_database/; \
              s/ls_storageUrlPath/\/$ls_php_urlPrefix_s\/storageServer\/var/; \
              s/ls_php_urlPrefix/$ls_php_urlPrefix_s/; \
              s/ls_storage_xmlRpcPrefix/$ls_storage_xmlRpcPrefix_s/; \
              s/ls_alib_xmlRpcPrefix/$ls_alib_xmlRpcPrefix_s/; \
              s/ls_php_host/$ls_php_host/; \
              s/ls_php_port/$ls_php_port/; \
              s/ls_output_alsa_device/$ls_output_alsa_device_s/; \
              s/ls_archiveUrlPath/\/$ls_php_urlPrefix_s\/archiveServer\/var/; \
              s/ls_scheduler_urlPrefix/$ls_scheduler_urlPrefix_s/; \
              s/ls_scheduler_xmlRpcPrefix/$ls_scheduler_xmlRpcPrefix_s/; \
              s/ls_scheduler_host/$ls_scheduler_host/; \
              s/ls_scheduler_port/$ls_scheduler_port/;"



#-------------------------------------------------------------------------------
#  Function to check for the existence of an executable on the PATH
#
#  @param $1 the name of the exectuable
#  @return 0 if the executable exists on the PATH, non-0 otherwise
#-------------------------------------------------------------------------------
check_exe() {
    if [ -x "`which $1 2> /dev/null`" ]; then
        echo "Executable $1 found...";
        return 0;
    else
        echo "Executable $1 not found...";
        return 1;
    fi
}


#-------------------------------------------------------------------------------
#  Check to see if this script is being run as root
#-------------------------------------------------------------------------------
if [ `whoami` != "root" ]; then
    echo "Please run this script as root.";
    exit ;
fi


#-------------------------------------------------------------------------------
#  Check for required tools
#-------------------------------------------------------------------------------
echo "Checking for required tools..."

check_exe "sed" || exit 1;
check_exe "psql" || exit 1;
check_exe "php" || exit 1;
check_exe "pear" || exit 1;
check_exe "odbcinst" || exit 1;


#-------------------------------------------------------------------------------
#   Check for the apache group to be a real group
#-------------------------------------------------------------------------------
group_tmp_file=/tmp/ls_group_check.$$
touch $group_tmp_file
test_result=`chgrp $apache_group $group_tmp_file 2> /dev/null`
if [ $? != 0 ]; then
    rm -f $group_tmp_file;
    echo "Unable to use apache deamon group $apache_group.";
    echo "Please check if $apache_group is a correct user group.";
    exit 1;
fi
rm -f $group_tmp_file;


#-------------------------------------------------------------------------------
#  Create the necessary database user and database itself
#-------------------------------------------------------------------------------
echo "Creating database and database user...";

# FIXME: the below might not work for remote databases

if [ "x$ls_dbserver" == "xlocalhost" ]; then
    su - $postgres_user -c "echo \"CREATE USER $ls_dbuser \
                                   ENCRYPTED PASSWORD '$ls_dbpassword' \
                                   CREATEDB NOCREATEUSER;\" \
                            | psql template1" \
        || echo "Couldn't create database user $ls_dbuser.";

    su - $postgres_user -c "echo \"CREATE DATABASE \\\"$ls_database\\\" \
                                    OWNER $ls_dbuser ENCODING 'utf-8';\" \
                            | psql template1" \
        || echo "Couldn't create database $ls_database.";
else
    echo "Unable to automatically create database user and table for";
    echo "remote database $ls_dbserver.";
    echo "Make sure to create database user $ls_dbuser with password";
    echo "$ls_dbpassword on database server at $ls_dbserver.";
    echo "Also create a database called $ld_database, owned by this user.";
    echo "";
    echo "The easiest way to achieve this is by issuing the following SQL";
    echo "commands to PostgreSQL:";
    echo "CREATE USER $ls_dbuser";
    echo "    ENCRYPTED PASSWORD '$ls_dbpassword'";
    echo "    CREATEDB NOCREATEUSER;";
    echo "CREATE DATABASE \"$ls_database\"";
    echo "    OWNER $ls_dbuser ENCODING 'utf-8';";
fi


# TODO: check for the success of these operations somehow


#-------------------------------------------------------------------------------
#  Create the ODBC data source and driver
#-------------------------------------------------------------------------------
echo "Creating ODBC data source and driver...";

odbcinst_template=$install_etc/odbcinst_template
odbc_template=$install_etc/odbc_template
odbc_template_tmp=/tmp/odbc_template.$$

# check for an existing PostgreSQL ODBC driver, and only install if necessary
odbcinst_res=`odbcinst -q -d | grep "\[PostgreSQL\]"`
if [ "x$odbcinst_res" == "x" ]; then
    echo "Registering ODBC PostgreSQL driver...";
    odbcinst -i -d -v -f $odbcinst_template || exit 1;
fi

echo "Registering LiveSupport ODBC data source...";
cat $odbc_template | sed -e "$replace_sed_string" > $odbc_template_tmp
odbcinst -i -s -l -f $odbc_template_tmp || exit 1;
rm -f $odbc_template_tmp


#-------------------------------------------------------------------------------
#   Install PEAR packages (locally in the LiveSupport)
#-------------------------------------------------------------------------------
$install_usr/lib/pear/bin/install.sh -d $installdir || exit 1;


#-------------------------------------------------------------------------------
#  Customize the configuration files with the appropriate values
#-------------------------------------------------------------------------------
echo "Customizing configuration files..."

cat $install_var/storageServer/var/conf.php.template \
    | sed -e "$replace_sed_string" \
    > $install_var/storageServer/var/conf.php

cat $install_var/archiveServer/var/conf.php.template \
    | sed -e "$replace_sed_string" \
    > $install_var/archiveServer/var/conf.php

cat $install_usr/var/conf.php.template \
    | sed -e "$replace_sed_string" \
    > $install_usr/var/conf.php

cat $install_etc/scheduler.xml.template \
    | sed -e "$replace_sed_string" \
    > $install_etc/scheduler.xml


#-------------------------------------------------------------------------------
#  Setup working directories
#-------------------------------------------------------------------------------
echo "Setting up working directories..."

mkdir -p $install_var/storageServer/var/stor/buffer
mkdir -p $install_var/storageServer/var/access
mkdir -p $install_var/storageServer/var/trans

mkdir -p $install_var/archiveServer/var/stor/buffer
mkdir -p $install_var/archiveServer/var/access
mkdir -p $install_var/archiveServer/var/trans

#-------------------------------------------------------------------------------
#  Setup directory permissions
#-------------------------------------------------------------------------------
echo "Setting up directory permissions..."

chgrp $apache_group $install_var/archiveServer/var/stor
chgrp $apache_group $install_var/archiveServer/var/access
chgrp $apache_group $install_var/archiveServer/var/trans
chgrp $apache_group $install_var/archiveServer/var/stor/buffer

chmod g+sw $install_var/archiveServer/var/stor
chmod g+sw $install_var/archiveServer/var/access
chmod g+sw $install_var/archiveServer/var/trans
chmod g+sw $install_var/archiveServer/var/stor/buffer

chgrp $apache_group $install_var/storageServer/var/stor
chgrp $apache_group $install_var/storageServer/var/access
chgrp $apache_group $install_var/storageServer/var/trans
chgrp $apache_group $install_var/storageServer/var/stor/buffer

chmod g+sw $install_var/storageServer/var/stor
chmod g+sw $install_var/storageServer/var/access
chmod g+sw $install_var/storageServer/var/trans
chmod g+sw $install_var/storageServer/var/stor/buffer


#-------------------------------------------------------------------------------
#  Configuring Apache
#-------------------------------------------------------------------------------
echo "Configuring apache ..."
CONFFILE=90_php_livesupport.conf
AP_DDIR_FOUND=no
for APACHE_DDIR in \
    /etc/apache/conf.d /etc/apache2/conf/modules.d /etc/httpd/conf.d
do
    echo -n "$APACHE_DDIR "
    if [ -d $APACHE_DDIR ]; then
        echo "Y"
        AP_DDIR_FOUND=yes
        cp $basedir/etc/apache/$CONFFILE $APACHE_DDIR
    else
        echo "N"
    fi
done
if [ "$AP_DDIR_FOUND" != "yes" ]; then
    echo "###############################"
    echo " Could not configure Apache"
    echo "  include following file into apache config manually:"
    echo "  $basedir/etc/apache/$CONFFILE"
    echo "###############################"
fi
echo "done"

echo "Restarting apache...";
AP_SCR_FOUND=no
for APACHE_SCRIPT in apache apache2 httpd ; do
    echo -n "$APACHE_SCRIPT "
    if [ -x /etc/init.d/$APACHE_SCRIPT ]; then
        echo "Y"
        AP_SCR_FOUND=yes
        /etc/init.d/$APACHE_SCRIPT restart
    else
        echo "N"
    fi
done
if [ "$AP_SCR_FOUND" != "yes" ]; then
    echo "###############################"
    echo " Could not reload Apache"
    echo "  please reload apache manually"
    echo "###############################"
fi
echo "done"


#-------------------------------------------------------------------------------
#  Create symlinks
#-------------------------------------------------------------------------------
echo "Creating symlinks...";

# create symlink for the PHP pages in apache's document root
rm -f $www_root/livesupport
ln -s $install_var $www_root/livesupport


#-------------------------------------------------------------------------------
#  Initialize the database
#-------------------------------------------------------------------------------
echo "Initializing database...";

# create PHP-related database tables
cd $install_var/storageServer/var/install
php -q install.php || exit 1;
cd -

# create scheduler-related database tables
cd $installdir
$bindir/scheduler.sh install || exit 1;
cd -


#-------------------------------------------------------------------------------
#  Create the gstreamer registry
#-------------------------------------------------------------------------------
echo "Creating gstreamer registry...";

gstreamer_dir=`find $install_lib -type d -name "gstreamer-*"`
export LD_LIBRARY_PATH=$install_lib
export GST_REGISTRY=$install_etc/gst-registry.xml
export GST_PLUGIN_PATH=$gstreamer_dir
$install_bin/gst-register > /dev/null 2>&1


#-------------------------------------------------------------------------------
#  Say goodbye
#-------------------------------------------------------------------------------
echo "Done."

