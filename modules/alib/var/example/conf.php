<?php
/*------------------------------------------------------------------------------

    Copyright (c) 2004 Media Development Loan Fund
 
    This file is part of the LiveSupport project.
    http://livesupport.campware.org/
    To report bugs, send an e-mail to bugs@campware.org
 
    LiveSupport is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    LiveSupport is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with LiveSupport; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 
    Author   : $Author: tomas $
    Version  : $Revision: 1.2 $
    Location : $Source: /home/paul/cvs2svn-livesupport/newcvsrepo/livesupport/modules/alib/var/example/conf.php,v $

------------------------------------------------------------------------------*/

$config = array(
    'dsn'       => array(           // data source definition
        'username' => 'test',
        'password' => 'test',
        'hostspec' => 'localhost',
        'phptype'  => 'pgsql',
        'database' => 'LiveSupport-test'
    ),
    'tblNamePrefix'     => 'al_',
#    'tblNamePrefix'     => 'gb_',
    'RootNode'	=>'RootNode',
    'objtypes'  => array(
        'RootNode'      => array('Publication'),
        'Publication'   => array('Issue'),
        'Issue'         => array('Title', 'Section'),
        'Section'       => array('Title', 'Image', 'Par')
    ),
    'allowedActions'=> array(
        'RootNode'      => array(
            'addChilds', 'editPerms', 'read', 'edit', 'delete',
            'classes', 'subjects'),
        'Publication'   => array(
            'addChilds', 'editPerms', 'read', 'edit', 'delete'),
        'Issue'         => array(
            'addChilds', 'editPerms', 'read', 'edit', 'delete'),
        'Section'       => array(
            'addChilds', 'editPerms', 'read', 'edit', 'delete'),
        'Title'         => array('editPerms', 'read', 'edit', 'delete'),
        'Image'         => array('editPerms', 'read', 'edit', 'delete'),
        'Par'           => array('editPerms', 'read', 'edit', 'delete'),
        '_class'        => array(
            'addChilds', 'editPerms', 'read', 'edit', 'delete')
    ),
    'allActions'=> array(
        'editPerms', 'addChilds', 'read', 'edit', 'delete',
        'classes', 'subjects')
);
?>