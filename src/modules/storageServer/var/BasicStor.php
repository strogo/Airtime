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
 
 
    Author   : $Author$
    Version  : $Revision$
    Location : $URL$

------------------------------------------------------------------------------*/
define('GBERR_DENY', 40);
define('GBERR_FILEIO', 41);
define('GBERR_FILENEX', 42);
define('GBERR_FOBJNEX', 43);
define('GBERR_WRTYPE', 44);
define('GBERR_NONE', 45);
define('GBERR_AOBJNEX', 46);
define('GBERR_NOTF', 47);
define('GBERR_SESS', 48);
define('GBERR_PREF', 49);
define('GBERR_TOKEN', 50);
define('GBERR_PUT', 51);
define('GBERR_LOCK', 52);

define('GBERR_NOTIMPL', 69);

require_once dirname(__FILE__)."/../../alib/var/alib.php";
require_once "StoredFile.php";
require_once "Transport.php";

/**
 *  BasicStor class
 *
 *  Core of LiveSupport file storage module
 *
 *  @author  $Author$
 *  @version $Revision$
 *  @see Alib
 */
class BasicStor extends Alib{
    var $filesTable;
    var $mdataTable;
    var $accessTable;
    var $storageDir;
    var $bufferDir;
    var $accessDir;
    var $rootId;
    var $storId;
    var $doDebug = true;

    /**
     *  Constructor
     *
     *  @param dbc PEAR::db abstract class reference
     *  @param config config array from conf.php
     *  @return class instance
     */
    function BasicStor(&$dbc, $config)
    {
        parent::Alib($dbc, $config);
        $this->config = $config;
        $this->filesTable = $config['tblNamePrefix'].'files';
        $this->mdataTable = $config['tblNamePrefix'].'mdata';
        $this->accessTable= $config['tblNamePrefix'].'access';
        $this->storageDir = $config['storageDir'];
        $this->bufferDir  = $config['bufferDir'];
        $this->transDir  = $config['transDir'];
        $this->accessDir  = $config['accessDir'];
        $this->dbc->setErrorHandling(PEAR_ERROR_RETURN);
        $this->rootId = $this->getRootNode();
        $this->storId = $this->wd =
            $this->getObjId('StorageRoot', $this->rootId);
        $this->dbc->setErrorHandling();
    }

    /**
     *  Create new folder
     *
     *  @param parid int, parent id
     *  @param folderName string, name for new folder
     *  @return id of new folder
     *  @exception PEAR::error
     */
    function bsCreateFolder($parid, $folderName)
    {
        return $this->addObj($folderName , 'Folder', $parid);
    }

    /**
     *  Store new file in the storage
     *
     *  @param parid int, parent id
     *  @param fileName string, name for new file
     *  @param mediaFileLP string, local path of media file
     *  @param mdataFileLP string, local path of metadata file
     *  @param gunid string, global unique id OPTIONAL
     *  @param ftype string, internal file type
     *  @param mdataLoc string 'file'|'string' (optional)
     *  @return int
     *  @exception PEAR::error
     */
    function bsPutFile($parid, $fileName, $mediaFileLP, $mdataFileLP,
        $gunid=NULL, $ftype='unKnown', $mdataLoc='file')
    {
        $name   = addslashes("$fileName");
        $id = $this->addObj($name , $ftype, $parid);
        if($this->dbc->isError($id)) return $id;
        $ac =  StoredFile::insert(
            $this, $id, $name, $mediaFileLP, $mdataFileLP, $mdataLoc,
            $gunid, $ftype
        );
        if($this->dbc->isError($ac)){
            $res = $this->removeObj($id);
            return $ac;
        }
        if($ftype == 'playlist') $ac->setMime('application/smil');
        return $id;
    }

    /**
     *  Rename file
     *
     *  @param id int, virt.file's local id
     *  @param newName string
     *  @return boolean or PEAR::error
     */
    function bsRenameFile($id, $newName)
    {
        $parid = $this->getParent($id);
        switch($this->getObjType($id)){
            case"audioclip":
            case"playlist":
            case"webstream":
                $ac = StoredFile::recall($this, $id);
                if($this->dbc->isError($ac)){
                    // catch nonerror exception:
                    //if($ac->getCode() != GBERR_FOBJNEX)
                    return $ac;
                }
                $res = $ac->rename($newName);
                if($this->dbc->isError($res)) return $res;
                break;
            case"File":
            default:
        }
        return $this->renameObj($id, $newName);
    }

    /**
     *  Move file
     *
     *  @param id int, virt.file's local id
     *  @param did int, destination folder local id
     *  @return boolean or PEAR::error
     */
    function bsMoveFile($id, $did)
    {
        $parid = $this->getParent($id);
        if($this->getObjType($did) !== 'Folder')
            return PEAR::raiseError(
                "BasicStor::moveFile: destination is not folder ($did)",
                GBERR_WRTYPE
            );
        switch($this->getObjType($id)){
            case"audioclip":
            case"playlist":
            case"webstream":
            case"File":
            case"Folder":
                return $this->moveObj($id, $did);
                break;
            default:
                return PEAR::raiseError(
                    "BasicStor::moveFile: unsupported object to move, sorry.",
                    GBERR_WRTYPE
                );
        }
    }

    /**
     *  Copy file
     *
     *  @param id int, virt.file's local id
     *  @param did int, destination folder local id
     *  @return boolean or PEAR::error
     */
    function bsCopyFile($id, $did)
    {
        $parid = $this->getParent($id);
        if($this->getObjType($did) !== 'Folder'){
            return PEAR::raiseError(
                'BasicStor::bsCopyFile: destination is not folder',
                GBERR_WRTYPE
            );
        }
        switch($this->getObjType($id)){
            case"audioclip":
            case"playlist":
            case"webstream":
            case"File":
            case"Folder":
                return $this->copyObj($id, $did);
                break;
            default:
                return PEAR::raiseError(
                    "BasicStor::moveFile: unsupported object to copy, sorry.",
                    GBERR_WRTYPE
                );
        }
    }

    /**
     *  Delete file
     *
     *  @param id int, virt.file's local id
     *  @param forced boolean, unconditional delete
     *  @return true or PEAR::error
     */
    function bsDeleteFile($id, $forced=FALSE)
    {
        // full delete:
        if(!$this->config['useTrash'] || $forced){
            $res = $this->removeObj($id, $forced);
            return $res;
        }
        // move to trash:
        $did = $this->getObjId($this->config['TrashName'], $this->storId);
        if($this->dbc->isError($did)) return $did;
        switch($this->getObjType($id)){
            case"audioclip":
            case"playlist":
            case"webstream":
                $ac = StoredFile::recall($this, $id);
                if($this->dbc->isError($ac)) return $ac;
                if(is_null($did)){
                    return PEAR::raiseError("BasicStor::bsDeleteFile: ".
                        "trash not found", GBERR_NOTF);
                }
                $res = $ac->setState('deleted');
                if($this->dbc->isError($res)) return $res;
                break;
            default:
        }
        $res = $this->bsMoveFile($id, $did);
        return $res;
    }

    /* ----------------------------------------------------- put, access etc. */
    /**
     *  Check validity of asscess/put token
     *
     *  @param token string, access/put token
     *  @param type string 'put'|'access'|'download'
     *  @return boolean
     */
    function bsCheckToken($token, $type='put')
    {
        $cnt = $this->dbc->getOne("
            SELECT count(token) FROM {$this->accessTable}
            WHERE token=x'{$token}'::bigint AND type='$type'
        ");
        if($this->dbc->isError($cnt)){ return FALSE; }
        return ($cnt == 1);
    }
    
    /**
     *  Get gunid from token
     *
     *  @param token string, access/put token
     *  @param type string 'put'|'access'|'download'
     *  @return string
     */
    function _gunidFromToken($token, $type='put')
    {
        $acc = $this->dbc->getRow("
            SELECT to_hex(gunid)as gunid, ext FROM {$this->accessTable}
            WHERE token=x'{$token}'::bigint AND type='$type'
        ");
        if($this->dbc->isError($acc)){ return $acc; }
        $gunid = StoredFile::_normalizeGunid($acc['gunid']);
        if($this->dbc->isError($gunid)){ return $gunid; }
        return $gunid;
    }
    
    /**
     *  Create and return access link to real file
     *
     *  @param realFname string, local filepath to accessed file
     *      (NULL for only increase access counter, no symlink)
     *  @param ext string, useful filename extension for accessed file
     *  @param gunid int, global unique id
     *  @param type string 'access'|'download'
     *  @param parent int parent token
     *  @return array with: seekable filehandle, access token
     */
    function bsAccess($realFname, $ext, $gunid, $type='access', $parent='0')
    {
        $token  = StoredFile::_createGunid();
        if(!is_null($realFname)){
            $linkFname = "{$this->accessDir}/$token.$ext";
            if(!file_exists($realFname)){
                return PEAR::raiseError(
                    "BasicStor::bsAccess: real file not found ($realFname)",
                    GBERR_FILEIO);
            }
            if(! @symlink($realFname, $linkFname)){
                return PEAR::raiseError(
                    "BasicStor::bsAccess: symlink create failed ($linkFname)",
                    GBERR_FILEIO);
            }
        }else $linkFname=NULL;
        $this->dbc->query("BEGIN");
        $res = $this->dbc->query("
            INSERT INTO {$this->accessTable}
                (gunid, token, ext, type, parent, ts)
            VALUES
                (x'{$gunid}'::bigint, x'$token'::bigint,
                '$ext', '$type', x'{$parent}'::bigint, now())
        ");
        if($this->dbc->isError($res)){
            $this->dbc->query("ROLLBACK"); return $res; }
        $res = $this->dbc->query("
            UPDATE {$this->filesTable}
            SET currentlyAccessing=currentlyAccessing+1
            WHERE gunid=x'{$gunid}'::bigint
        ");
        if($this->dbc->isError($res)){
            $this->dbc->query("ROLLBACK"); return $res; }
        $res = $this->dbc->query("COMMIT");
        if($this->dbc->isError($res)){ return $res; }
        return array('fname'=>$linkFname, 'token'=>$token);
    }

    /**
     *  Release access link to real file
     *
     *  @param token string, access token
     *  @param type string 'access'|'download'
     *  @return string, global unique ID
     */
    function bsRelease($token, $type='access')
    {
        if(!$this->bsCheckToken($token, $type)){
            return PEAR::raiseError(
             "BasicStor::bsRelease: invalid token ($token)"
            );
        }
        $acc = $this->dbc->getRow("
            SELECT to_hex(gunid)as gunid, ext FROM {$this->accessTable}
            WHERE token=x'{$token}'::bigint AND type='$type'
        ");
        if($this->dbc->isError($acc)){ return $acc; }
        $ext = $acc['ext'];
        $gunid = StoredFile::_normalizeGunid($acc['gunid']);
        $linkFname = "{$this->accessDir}/$token.$ext";
        if(file_exists($linkFname)) if(! @unlink($linkFname)){
            return PEAR::raiseError(
                "BasicStor::bsRelease: unlink failed ($linkFname)",
                GBERR_FILEIO);
        }
        $this->dbc->query("BEGIN");
        $res = $this->dbc->query("
            UPDATE {$this->filesTable}
            SET currentlyAccessing=currentlyAccessing-1
            WHERE gunid=x'{$gunid}'::bigint AND currentlyAccessing>0
        ");
        if($this->dbc->isError($res)){
            $this->dbc->query("ROLLBACK"); return $res; }
        $res = $this->dbc->query("
            DELETE FROM {$this->accessTable} WHERE token=x'$token'::bigint
        ");
        if($this->dbc->isError($res)){
            $this->dbc->query("ROLLBACK"); return $res; }
        $res = $this->dbc->query("COMMIT");
        if($this->dbc->isError($res)){ return $res; }
        return $gunid;
    }

    /**
     *  Create and return downloadable URL for file
     *
     *  @param id int, virt.file's local id
     *  @param part string, 'media'|'metadata'
     *  @param parent int parent token
     *  @return array with strings:
     *      downloadable URL, download token, chsum, size, filename
     */
    function bsOpenDownload($id, $part='media', $parent='0')
    {
        $ac = StoredFile::recall($this, $id);
        if($this->dbc->isError($ac)) return $ac;
        $gunid      = $ac->gunid;
        switch($part){
            case"media":
                $realfile  = $ac->_getRealRADFname();
                $ext       = $ac->_getExt();
                $filename  = $ac->_getFileName();
                break;
            case"metadata":
                $realfile  = $ac->_getRealMDFname();
                $ext       = "xml";
                $filename  = $ac->_getFileName();
                break;
            default:
                return PEAR::raiseError(
                 "BasicStor::bsOpenDownload: unknown part ($part)"
                );
        }
        $acc = $this->bsAccess($realfile, $ext, $gunid, 'download', $parent);
        if($this->dbc->isError($acc)){ return $acc; }
        $url = $this->getUrlPart()."access/".basename($acc['fname']);
        $chsum = md5_file($realfile);
        $size = filesize($realfile);
        return array(
            'url'=>$url, 'token'=>$acc['token'],
            'chsum'=>$chsum, 'size'=>$size,
            'filename'=>$filename
        );
    }

    /**
     *  Discard downloadable URL
     *
     *  @param token string, download token
     *  @param part string, 'media'|'metadata'
     *  @return string, gunid
     */
    function bsCloseDownload($token, $part='media')
    {
        if(!$this->bsCheckToken($token, 'download')){
            return PEAR::raiseError(
             "BasicStor::bsCloseDownload: invalid token ($token)"
            );
        }
        return $this->bsRelease($token, 'download');
    }

    /**
     *  Create writable URL for HTTP PUT method file insert
     *
     *  @param chsum string, md5sum of the file having been put
     *  @param gunid string, global unique id
     *  @return array with: writable URL, PUT token
     */
    function bsOpenPut($chsum, $gunid)
    {
        $ext    = '';
        $token  = StoredFile::_createGunid();
        $res    = $this->dbc->query("
            DELETE FROM {$this->accessTable} WHERE token=x'$token'::bigint
        ");
        if($this->dbc->isError($res)){ return $res; }
        $res    = $this->dbc->query("
            INSERT INTO {$this->accessTable}
                (gunid, token, ext, chsum, type, ts)
            VALUES
                (x'{$gunid}'::bigint, x'$token'::bigint,
                    '$ext', '$chsum', 'put', now())
        ");
        if($this->dbc->isError($res)){ return $res; }
        $fname = "{$this->accessDir}/$token";
        touch($fname);      // is it needed?
        $url = $this->getUrlPart()."xmlrpc/put.php?token=$token";
        return array('url'=>$url, 'token'=>$token);
    }

    /**
     *  Get file from writable URL and return local filename.
     *  Caller should move or unlink this file.
     *
     *  @param token string, PUT token
     *  @return string, local path of the file having been put
     */
    function bsClosePut($token)
    {
        $token = StoredFile::_normalizeGunid($token);
        if(!$this->bsCheckToken($token, 'put')){
            return PEAR::raiseError(
             "BasicStor::bsClosePut: invalid token ($token)",
             GBERR_TOKEN
            );
        }
        $chsum = $this->dbc->getOne("
            SELECT chsum FROM {$this->accessTable}
            WHERE token=x'{$token}'::bigint
        ");
        $res = $this->dbc->query("
            DELETE FROM {$this->accessTable} WHERE token=x'$token'::bigint
        ");
        if($this->dbc->isError($res)){ return $res; }
        $fname = "{$this->accessDir}/$token";
        $md5sum = md5_file($fname);
        if($chsum != $md5sum){
            if(file_exists($fname)) @unlink($fname);
            return PEAR::raiseError(
             "BasicStor::bsClosePut: md5sum does not match (token=$token)",
             GBERR_PUT
            );
        }
        return $fname;
    }

    /**
     *  Check uploaded file
     *
     *  @param token string, put token
     *  @return hash, (
     *      status: boolean,
     *      size: int - filesize
     *      expectedsum: string - expected checksum
     *      realsum: string - checksum of uploaded file
     *   )
     */
    function bsCheckPut($token)
    {
        if(!$this->bsCheckToken($token, 'put')){
            return PEAR::raiseError(
             "BasicStor::bsClosePut: invalid token ($token)"
            );
        }
        $chsum = $this->dbc->getOne("
            SELECT chsum FROM {$this->accessTable}
            WHERE token=x'{$token}'::bigint
        ");
        if($this->dbc->isError($chsum)){ return $chsum; }
        $fname = "{$this->accessDir}/$token";
        $md5sum = md5_file($fname);
        $size = filesize($fname);
        $status = ($chsum == $md5sum);
        return array(
            'status'=>$status, 'size'=>$size,
            'expectedsum'=>$chsum,
            'realsum'=>$md5sum,
        );
    }

    /**
     *  Return starting part of storageServer URL
     *
     *  @return string, url
     */
    function getUrlPart()
    {
        $host  = $this->config['storageUrlHost'];
        $port  = $this->config['storageUrlPort'];
        $path  = $this->config['storageUrlPath'];
        return "http://$host:$port$path/";
    }
    
    /* ---------------------------------------------- replicas, versions etc. */
    /**
     *  Create replica.<br>
     *  <b>TODO: NOT FINISHED</b>
     *
     *  @param id int, virt.file's local id
     *  @param did int, destination folder local id
     *  @param replicaName string, name of new replica
     *  @return int, local id of new object
     */
    function bsCreateReplica($id, $did, $replicaName)
    {
        return PEAR::raiseError(
            'BasicStor::bsCreateReplica: not implemented', GBERR_NOTIMPL
        );
        // ---
        if($this->getObjType($did) !== 'Folder')
            return PEAR::raiseError(
                'BasicStor::bsCreateReplica: dest is not folder', GBERR_WRTYPE
            );
        if($replicaName=='') $replicaName = $this->getObjName($id);
        while(($exid = $this->getObjId($replicaName, $did))<>'')
            { $replicaName.='_R'; }
        $rid = $this->addObj($replicaName , 'Replica', $did, 0, $id);
        if($this->dbc->isError($rid)) return $rid;
#        $this->addMdata($this->_pathFromId($rid), 'isReplOf', $id);
        return $rid;
    }

    /**
     *  Create version.<br>
     *  <b>TODO: NOT FINISHED</b>
     *
     *  @param id int, virt.file's local id
     *  @param did int, destination folder local id
     *  @param versionLabel string, name of new version
     *  @return int, local id of new object
     */
    function bsCreateVersion($id, $did, $versionLabel)
    {
        return PEAR::raiseError(
            'BasicStor::bsCreateVersion: not implemented', GBERR_NOTIMPL
        );
    }

    /* ------------------------------------------------------------- metadata */

    /**
     *  Replace metadata with new XML file or string
     *
     *  @param id int, virt.file's local id
     *  @param mdata string, local path of metadata XML file
     *  @param mdataLoc string 'file'|'string'
     *  @return boolean or PEAR::error
     */
    function bsReplaceMetadata($id, $mdata, $mdataLoc='file')
    {
        $ac = StoredFile::recall($this, $id);
        if($this->dbc->isError($ac)) return $ac;
        return $ac->replaceMetaData($mdata, $mdataLoc);
    }
    
    /**
     *  Get metadata as XML string
     *
     *  @param id int, virt.file's local id
     *  @return string or PEAR::error
     */
    function bsGetMetadata($id)
    {
        $ac = StoredFile::recall($this, $id);
        if($this->dbc->isError($ac)) return $ac;
        return $ac->getMetaData();
    }

    /**
     *  Get metadata element value
     *
     *  @param id int, virt.file's local id
     *  @param category string, metadata element name
     *  @param lang string, optional xml:lang value for select language version
     *  @param deflang string, optional xml:lang for default language
     *  @return array of matching records (as hash {id, value, attrs})
     *  @see Metadata::getMetadataValue
     */
    function bsGetMetadataValue($id, $category, $lang=NULL, $deflang=NULL)
    {   
        $ac = StoredFile::recall($this, $id);
        if($this->dbc->isError($ac)) return $ac;
        return $ac->md->getMetadataValue($category, $lang, $deflang);
    }
    
    /**
     *  Set metadata element value
     *
     *  @param id int, virt.file's local id
     *  @param category string, metadata element identification (e.g. dc:title)
     *  @param value string/NULL value to store, if NULL then delete record
     *  @param lang string, optional xml:lang value for select language version
     *  @param mid int, metadata record id (OPTIONAL on unique elements)
     *  @param container string, container element name for insert
     *  @param regen boolean, optional flag, if true, regenerate XML file
     *  @return boolean
     */
    function bsSetMetadataValue($id, $category, $value,
        $lang=NULL, $mid=NULL, $container='metadata', $regen=TRUE)
    {
        $ac = StoredFile::recall($this, $id);
        if($this->dbc->isError($ac)) return $ac;
        /* disabled - html ui change only nonimportant categories
        if($ac->isEdited()){
            return PEAR::raiseError(
                'BasicStor::bsSetMetadataValue: is edited', GBERR_LOCK
            );
        }
        */
        if($category == 'dcterms:extent'){
            $value = $this->normalizeExtent($value);
        }
        $res = $ac->md->setMetadataValue(
            $category, $value, $lang, $mid, $container);
        if($this->dbc->isError($res)) return $res;
        if($regen){
            $r = $ac->md->regenerateXmlFile();
            if($this->dbc->isError($r)) return $r;
        }
        return $res;
    }

    /**
     *  Normalize time value to hh:mm:ss:dddddd format
     *
     *  @param v mixed, value to normalize
     *  @return string
     */
    function normalizeExtent($v)
    {
        if(!preg_match("|^\d{2}:\d{2}:\d{2}.\d{6}$|", $v)){
            require_once"Playlist.php";
            $s = Playlist::_plTimeToSecs($v);
            $t = Playlist::_secsToPlTime($s);
            return $t;
        }
        return $v;
    }
    
    /**
     *  Set metadata values in 'batch' mode
     *
     *  @param id int, virt.file's local id
     *  @param values hasharray, array of key/value pairs
     *      (e.g. 'dc:title'=>'New title')
     *  @param lang string, optional xml:lang value for select language version
     *  @param container string, container element name for insert
     *  @param regen boolean, optional flag, if true, regenerate XML file
     *  @return boolean
     */
    function bsSetMetadataBatch(
        $id, $values, $lang=NULL, $container='metadata', $regen=TRUE)
    {
        if(!is_array($values)) $values = array($values);
        foreach($values as $category=>$oneValue){
            $res = $this->bsSetMetadataValue($id, $category, $oneValue,
                $lang, NULL, $container, FALSE);
            if($this->dbc->isError($res)) return $res;
        }
        $res = $this->bsSetMetadataValue($id, 'ls:mtime', date('U'),
            $lang, NULL, $container, $regen);
        if($this->dbc->isError($res)) return $res;
        return TRUE;
    }

    /**
     *  Search in local metadata database.
     *
     *  @param criteria hash, with following structure:<br>
     *   <ul>
     *     <li>filetype - string, type of searched files,
     *       meaningful values: 'audioclip', 'webstream', 'playlist', 'all'</li>
     *     <li>operator - string, type of conditions join
     *       (any condition matches / all conditions match), 
     *       meaningful values: 'and', 'or', ''
     *       (may be empty or ommited only with less then 2 items in
     *       &quot;conditions&quot; field)
     *     </li>
     *     <li>orderby : string - metadata category for sorting (optional)</li>
     *     <li>desc : boolean - flag for descending order (optional)</li>
     *     <li>conditions - array of hashes with structure:
     *       <ul>
     *           <li>cat - string, metadata category name</li>
     *           <li>op - string, operator - meaningful values:
     *               'full', 'partial', 'prefix', '=', '&lt;',
     *               '&lt;=', '&gt;', '&gt;='</li>
     *           <li>val - string, search value</li>
     *       </ul>
     *     </li>
     *   </ul>
     *  @param limit int, limit for result arrays (0 means unlimited)
     *  @param offset int, starting point (0 means without offset)
     *  @return hash, fields:
     *       results : array with gunid strings
     *       cnt : integer - number of matching gunids 
     *  of files have been found
     *  @see DataEngine
     */
    function bsLocalSearch($criteria, $limit=0, $offset=0)
    {
        require_once "DataEngine.php";
        $de =& new DataEngine($this);
        return $de->localSearch($criteria, $limit, $offset);
    }

    /**
     *  Return values of specified metadata category
     *
     *  @param category string, metadata category name
     *          with or without namespace prefix (dc:title, author)
     *  @param limit int, limit for result arrays (0 means unlimited)
     *  @param offset int, starting point (0 means without offset)
     *  @param criteria hash, see bsLocalSearch method
     *  @return hash, fields:
     *       results : array with gunid strings
     *       cnt : integer - number of matching values
     *  @see DataEngine
     */
    function bsBrowseCategory($category, $limit=0, $offset=0, $criteria=NULL)
    {
        require_once "DataEngine.php";
        $de =& new DataEngine($this);
        return $de->browseCategory($category, $limit, $offset, $criteria);
    }

    /* --------------------------------------------------------- info methods */

    /**
     *  List files in folder
     *
     *  @param id int, local id of folder
     *  @return array
     */
    function bsListFolder($id)
    {
        if($this->getObjType($id) !== 'Folder')
            return PEAR::raiseError(
                'BasicStor::bsListFolder: not a folder', GBERR_NOTF
            );
        $listArr = $this->getDir($id, 'id, name, type, param as target', 'name');
        if($this->dbc->isError($listArr)) return $listArr;
        foreach($listArr as $i=>$v){
            if($v['type'] == 'Folder')  break;
            $gunid = $this->_gunidFromId($v['id']);
            if($this->dbc->isError($gunid)) return $gunid;
            if(is_null($gunid)){ unset($listArr[$i]); break; }
            $listArr[$i]['type'] = $r = $this->_getType($gunid);
            if($this->dbc->isError($r)) return $r;
            $listArr[$i]['gunid'] = $gunid;
            if(StoredFIle::_getState($gunid) == 'incomplete')
                unset($listArr[$i]);
        }
        return $listArr;
    }
    
    /**
     *  Analyze media file for internal metadata information
     *
     *  @param id int, virt.file's local id
     *  @return array
     */
    function bsAnalyzeFile($id)
    {
        $ac = StoredFile::recall($this, $id);
        if($this->dbc->isError($ac)) return $ac;
        $ia = $ac->analyzeMediaFile();
        return $ia;
    }

    /**
     *  List files in folder
     *
     *  @param id int, local id of object
     *  @param relPath string, relative path
     *  @return array
     */
    function getObjIdFromRelPath($id, $relPath='.')
    {
        $relPath = trim(urldecode($relPath));
        //if($this->getObjType($id) !== 'Folder')
        $nid = $this->getParent($id);
        if($this->dbc->isError($nid)) return $nid;
        if(is_null($nid)){
            return PEAR::raiseError("null parent for id=$id"); }
        //else $nid = $id;
        if(substr($relPath, 0, 1)=='/'){ $nid=$this->storId; }
        $a = split('/', $relPath);
        foreach($a as $i=>$pathItem){
            switch($pathItem){
                case".":
                    break;
                case"..":
                    if($nid != $this->storId){
                        $nid = $this->getParent($nid);
                        if($this->dbc->isError($nid)) return $nid;
                        if(is_null($nid)){
                             return PEAR::raiseError(
                                "null parent for $nid");
                        }
                    }
                    break;
                case"":
                    break;
                default:
                    $nid = $this->getObjId($pathItem, $nid);
                    if($this->dbc->isError($nid)) return $nid;
                    if(is_null($nid)){
                         return PEAR::raiseError(
                            "Object $pathItem not found (from id=$id)");
                    }
            }
        }
        return $nid;
    }
    
    /**
     *  Check if file exists in the storage
     *
     *  @param id int, local id
     *  @param ftype string, internal file type
     *  @return boolean
     */
    function bsExistsFile($id, $ftype=NULL)
    {
        $ac = StoredFile::recall($this, $id);
        if($this->dbc->isError($ac)){
            // catch some exceptions
            switch($ac->getCode()){
                case GBERR_FILENEX:
                case GBERR_FOBJNEX:
                    return FALSE;
                    break;
                default: return $ac;
            }
        }
        $realFtype = $this->_getType($ac->gunid);
        if(!is_null($ftype) && (
            ($realFtype != $ftype)
            // webstreams are subset of audioclips
            && !($realFtype == 'webstream' && $ftype == 'audioclip')
        )) return FALSE;
        return TRUE;
    }

    /* ---------------------------------------------------- redefined methods */
    /**
     *  Get object type by id.
     *  (RootNode, Folder, File, )
     *
     *  @param oid int, local object id
     *  @return string/err
     */
    function getObjType($oid)
    {
        $type = parent::getObjType($oid);
        if($type == 'File'){
            $gunid = $this->_gunidFromId($oid);
            if($this->dbc->isError($gunid)) return $gunid;
            $ftype = $this->_getType($gunid);
            if($this->dbc->isError($ftype)) return $ftype;
            if(!is_null($ftype)) $type=$ftype;
        }
        return $type;
    }
    
    /**
     *  Add new user with home folder
     *
     *  @param login string
     *  @param pass string OPT
     *  @param realname string OPT
     *  @return int/err
     */
    function addSubj($login, $pass=NULL, $realname='')
    {
        $uid = parent::addSubj($login, $pass, $realname);
        if($this->dbc->isError($uid)) return $uid;
        if($this->isGroup($uid) === FALSE){
            $fid = $this->bsCreateFolder($this->storId, $login);
            if($this->dbc->isError($fid)) return $fid;
            $res = parent::addPerm($uid, '_all', $fid, 'A');
            if($this->dbc->isError($res)) return $res;
            if(!$this->config['isArchive']){
                $res =$this->addSubj2Gr($login, $this->config['StationPrefsGr']);
                if($this->dbc->isError($res)) return $res;
                $res =$this->addSubj2Gr($login, $this->config['AllGr']);
                if($this->dbc->isError($res)) return $res;
                $pfid = $this->bsCreateFolder($fid, 'public');
                if($this->dbc->isError($pfid)) return $pfid;
                $res = parent::addPerm($uid, '_all', $pfid, 'A');
                if($this->dbc->isError($res)) return $res;
                $allGrId =  $this->getSubjId($this->config['AllGr']);
                if($this->dbc->isError($allGrId)) return $allGrId;
                $res = parent::addPerm($allGrId, 'read', $pfid, 'A');
                if($this->dbc->isError($res)) return $res;
            }
        }
        return $uid;
    }
    /**
     *  Remove user by login and remove also his home folder
     *
     *  @param login string
     *  @return boolean/err
     */
    function removeSubj($login)
    {
        if(FALSE !== array_search($login, $this->config['sysSubjs'])){
            return $this->dbc->raiseError(
                "BasicStor::removeSubj: cannot remove system user/group");
        }
        $res = parent::removeSubj($login);
        if($this->dbc->isError($res)) return $res;
        $id = $this->getObjId($login, $this->storId);
        if($this->dbc->isError($id)) return $id;
        if(!is_null($id)){
            // remove home folder:
            $res = $this->bsDeleteFile($id);
            if($this->dbc->isError($res)) return $res;
        }
        return TRUE;
    }  
    
    /* ================================================== "protected" methods */
    /**
     *  Check authorization - auxiliary method
     *
     *  @param acts array of actions
     *  @param pars array of parameters - e.g. ids
     *  @param sessid string, session id
     *  @return true or PEAR::error
     */
    function _authorize($acts, $pars, $sessid='')
    {
        $userid = $this->getSessUserId($sessid);
        if($this->dbc->isError($userid)) return $userid;
        if(is_null($userid)){
            return PEAR::raiseError(
                "BasicStor::_authorize: invalid session", GBERR_DENY);
        }
        if(!is_array($pars)) $pars = array($pars);
        if(!is_array($acts)) $acts = array($acts);
        $perm = true;
        foreach($acts as $i=>$action){
            $res = $this->checkPerm($userid, $action, $pars[$i]);
            if($this->dbc->isError($res)) return $res;
            $perm = $perm && $res;
        }
        if($perm) return TRUE;
        $adesc = "[".join(',',$acts)."]";
        return PEAR::raiseError(
            "BasicStor::$adesc: access denied", GBERR_DENY);
    }

    /**
     *  Return users's home folder local ID
     *
     *  @param sessid string, session ID
     *  @return local folder id
     */
    function _getHomeDirId($sessid)
    {
        $login = $this->getSessLogin($sessid);
        if($this->dbc->isError($login)) return $login;
        $parid = $this->getObjId($login, $this->storId);
        if($this->dbc->isError($parid)) return $parid;
        if(is_null($parid)){
            return PEAR::raiseError("BasicStor::_getHomeDirId: ".
                "homedir not found", GBERR_NOTF);
        }
        return $parid;
    }
    
    /**
     *  Get local id from global id
     *
     *  @param gunid string global id
     *  @return int local id
     */
    function _idFromGunid($gunid)
    {
        return $this->dbc->getOne(
            "SELECT id FROM {$this->filesTable} WHERE gunid=x'$gunid'::bigint"
        );
    }

    /**
     *  Get global id from local id
     *
     *  @param id int local id
     *  @return string global id
     */
    function _gunidFromId($id)
    {
        if(!is_numeric($id)) return NULL;
        $gunid = $this->dbc->getOne("
            SELECT to_hex(gunid)as gunid FROM {$this->filesTable}
            WHERE id='$id'
        ");
        if($this->dbc->isError($gunid)) return $gunid;
        if(is_null($gunid)) return NULL;
        return StoredFile::_normalizeGunid($gunid);
    }

    /**
     *  Get storage-internal file type
     *
     *  @param gunid string, global unique id of file
     *  @return string, see install()
     */
    function _getType($gunid)
    {
        $ftype = $this->dbc->getOne("
            SELECT ftype FROM {$this->filesTable}
            WHERE gunid=x'$gunid'::bigint
        ");
        return $ftype;
    }

    /**
     *  Check gunid format
     *
     *  @param gunid string, global unique ID
     *  @return boolean
     */
    function _checkGunid($gunid)
    {
        $res = preg_match("|^([0-9a-fA-F]{16})?$|", $gunid);
        return $res;
    }

    /**
     *  Returns if gunid is free
     *
     */
    function _gunidIsFree($gunid)
    {
        $cnt = $this->dbc->getOne("
            SELECT count(*) FROM {$this->filesTable}
            WHERE gunid=x'{$this->gunid}'::bigint
        ");
        if($this->dbc->isError($cnt)) return $cnt;
        if($cnt > 0) return FALSE;
        return TRUE;
    }    

    /**
     *  Set playlist edit flag
     *
     *  @param playlistId string, playlist global unique ID
     *  @param val boolean, set/clear of edit flag
     *  @param sessid string, session id
     *  @return boolean, previous state
     */
    function _setEditFlag($playlistId, $val=TRUE, $sessid='')
    {
        if($sessid !== ''){
            $userid = $this->getSessUserId($sessid);
            if($this->dbc->isError($userid)) return $userid;
        }else $userid=NULL;
        $ac = StoredFile::recallByGunid($this, $playlistId);
        if($this->dbc->isError($ac)) return $ac;
        $state = $ac->_getState();
        if($val){ $r = $ac->setState('edited', $userid); }
        else{ $r = $ac->setState('ready', 'NULL'); }
        if($this->dbc->isError($r)) return $r;
        return ($state == 'edited');
    }

    /**
     *  Check if playlist is marked as edited
     *
     *  @param playlistId string, playlist global unique ID
     *  @return FALSE | int - id of user editing it
     */
    function _isEdited($playlistId)
    {
        $ac = StoredFile::recallByGunid($this, $playlistId);
        if($this->dbc->isError($ac)) return $ac;
        if(!$ac->isEdited($playlistId)) return FALSE;
        return $ac->isEditedBy($playlistId);
    }

    /* ---------------------------------------- redefined "protected" methods */
    /**
     *  Copy virtual file.<br>
     *  Redefined from parent class.
     *
     *  @return int, new object local id
     */
    function copyObj($id, $newParid, $after=NULL)
    {
        $parid = $this->getParent($id);
        $nid = parent::copyObj($id, $newParid, $after);
        if($this->dbc->isError($nid)) return $nid;
        switch($this->getObjType($id)){
            case"audioclip":
            case"playlist":
            case"webstream":
                $ac = StoredFile::recall($this, $id);
                if($this->dbc->isError($ac)){ return $ac; }
                $ac2 = StoredFile::copyOf($ac, $nid);
                $ac2->rename($this->getObjName($nid));
                break;
            case"File":
            default:
        }
        return $nid;
    }

    /**
     *  Move virtual file.<br>
     *  Redefined from parent class.
     *
     *  @return boolean
     */
    function moveObj($id, $newParid, $after=NULL)
    {
        $parid = $this->getParent($id);
        switch($this->getObjType($id)){
            case"audioclip":
            case"playlist":
            case"webstream":
                $ac = StoredFile::recall($this, $id);
                if($this->dbc->isError($ac)){ return $ac; }
                if($ac->isEdited())
                    return PEAR::raiseError(
                        'BasicStor::moveObj: is edited');
                if($ac->isAccessed())
                    return PEAR::raiseError(
                        'BasicStor::moveObj: is accessed');
                break;
            default:
        }
        $nid = parent::moveObj($id, $newParid, $after);
        if($this->dbc->isError($nid)) return $nid;
        return TRUE;
    }
    /**
     *  Optionaly remove virtual file with the same name and add new one.<br>
     *  Redefined from parent class.
     *
     *  @return id
     */
    function addObj($name, $type, $parid=1, $aftid=NULL, $param='')
    {
        $exid = $this->getObjId($name, $parid);
        if($this->dbc->isError($exid)) return $exid;
        //if(!is_null($exid)){ $this->removeObj($exid); }
        $name2 = $name;
        for( ;
            $xid = $this->getObjId($name2, $parid),
                !is_null($xid) && !$this->dbc->isError($xid);
            $name2 .= "_"
        );
        if(!is_null($exid)){ $r = $this->renameObj($exid, $name2); }
        return parent::addObj($name, $type, $parid, $aftid, $param);
    }

    /**
     *  Remove virtual file.<br>
     *  Redefined from parent class.
     *
     *  @param id int, local id of removed object
     *  @param forced boolean, unconditional delete
     *  @return true or PEAR::error
     */
    function removeObj($id, $forced=FALSE)
    {
        switch($ot = $this->getObjType($id)){
            case"audioclip":
            case"playlist":
            case"webstream":
                $ac = StoredFile::recall($this, $id);
                if($this->dbc->isError($ac)) return $ac;
                if($ac->isEdited() && !$forced){
                    return PEAR::raiseError(
                        'BasicStor.php: removeObj: is edited'
                    );
                }
                if($ac->isAccessed() && !$forced){
                    return PEAR::raiseError(
                        'BasicStor.php: removeObj: is accessed'
                    );
                }
                $ac->delete();
                break;
            case"File":
            case"Folder":
            case"Replica":
                break;
            default:
                return PEAR::raiseError(
                    "BasicStor::bsDeleteFile: unknown obj type ($ot)"
                );
        }
        $res = parent::removeObj($id);
        if($this->dbc->isError($res)) return $res;
        return TRUE;
    }

    /* =============================================== test and debug methods */
    /**
     *  Reset storageServer for debugging.
     *
     *  @param loadSampleData boolean - flag for allow sample data loading
     *  @param filesOnly boolean - flag for operate only on files in storage
     */
    function resetStorage($loadSampleData=TRUE, $filesOnly=FALSE)
    {
        if($filesOnly) $this->deleteFiles();
        else $this->deleteData();
        if(!$this->config['isArchive']){
            $tr =& new Transport($this->dbc, $this, $this->config);
            $tr->resetData();
        }
        $res = array(
            'audioclips'=>array(), 'playlists'=>array(), 'webstreams'=>array()
        );
        if(!$loadSampleData) return $res;
        $rootHD = $this->getObjId('root', $this->storId);
        $samples = dirname(__FILE__)."/tests/sampleData.php";
        if(file_exists($samples)){
            include $samples;
        }else $sampleData = array();
        foreach($sampleData as $k=>$it){
            $type = $it['type'];
            $xml = $it['xml'];
            if(isset($it['gunid'])) $gunid = $it['gunid'];
            else $gunid = '';
            switch($type){
                case"audioclip":
                    $media = $it['media'];
                    $fname = basename($media);
                    break;
                case"playlist":
                case"webstream":
                    $media = '';
                    $fname = basename($xml);
                    break;
            }
            $r = $this->bsPutFile(
                $rootHD, $fname,
                $media, $xml, $gunid, $type
            );
            if(PEAR::isError($r)){ return $r; }
            $res["{$type}s"][] = $this->_gunidFromId($r);
        }
        return $res;
    }

    /**
     *  dump
     *
     */
    function dump($id='', $indch='    ', $ind='', $format='{name}')
    {
        if($id=='') $id = $this->storId;
        return parent::dump($id, $indch, $ind, $format);
    }

    /**
     *
     *
     */
    function dumpDir($id='', $format='$o["name"]')
    {
        if($id=='') $id = $this->storId;
        $arr = $this->getDir($id, 'id,name');
//        if($this->doDebug){ $this->debug($arr); exit; }
        $arr = array_map(create_function('$o', 'return "'.$format.'";'), $arr);
        return join('', $arr);
    }

    /**
     *
     *
     */
    function debug($va)
    {
        echo"<pre>\n"; print_r($va); #exit;
    }

    /**
     *  deleteFiles
     *
     *  @return void
     */
    function deleteFiles()
    {
        $ids = $this->dbc->getAll("SELECT id FROM {$this->filesTable}");
        if(is_array($ids)) foreach($ids as $i=>$item){
            $this->bsDeleteFile($item['id'], TRUE);
        }
    }
    /**
     *  deleteData
     *
     *  @return void
     */
    function deleteData()
    {
        $this->deleteFiles();
        parent::deleteData();
        $this->initData();
    }
    /**
     *  Create BasicStor object with temporarily changed configuration
     *  to prevent data changes in tests
     *
     */
    function createTestSpace(&$dbc, $config){
        $configBckp = $config;
        $config['tblNamePrefix'] .= '_test_';
        mkdir($config['storageDir'].'/tmp');
        $config['storageDir']    .=  '/tmp/stor';
        $config['bufferDir']      =  $config['storageDir'].'/buffer';
        $config['transDir']      .=  '/tmp/trans';
        $config['accessDir']     .=  '/tmp/access';
        mkdir($config['storageDir']);
        mkdir($config['bufferDir']);
        $bs =& new BasicStor($dbc, $config);
        $bs->configBckp = $configBckp;
        $r = $bs->install();
        if(PEAR::isError($r)){ return $r; }
        return $bs;
    }
    /**
     *  Clean up test space
     *
     */
    function releaseTestSpace(){
        $r = $this->uninstall();
        if(PEAR::isError($r)){ return $r; }
        #rmdir($this->config['bufferDir']);
        rmdir($this->config['storageDir']);
        $this->config = $this->configBckp;
        rmdir($this->config['storageDir'].'/tmp');
    }
    /**
     *  testData
     *
     */
    function testData($d='')
    {
        $exdir = dirname(__FILE__).'/tests';
        $o[] = $this->addSubj('test1', 'a');
        $o[] = $this->addSubj('test2', 'a');
        $o[] = $this->addSubj('test3', 'a');
        $o[] = $this->addSubj('test4', 'a');

        $o[] = $t1hd = $this->getObjId('test1', $this->storId);
        $o[] = $t1d1 = $this->bsCreateFolder($t1hd, 'test1_folder1');
        $o[] = $this->bsCreateFolder($t1hd, 'test1_folder2');
        $o[] = $this->bsCreateFolder($t1d1, 'test1_folder1_1');
        $o[] = $t1d12 = $this->bsCreateFolder($t1d1, 'test1_folder1_2');

        $o[] = $t2hd = $this->getObjId('test2', $this->storId);
        $o[] = $this->bsCreateFolder($t2hd, 'test2_folder1');

        $o[] = $this->bsPutFile($t1hd, 'file1.mp3', "$exdir/ex1.mp3", '', NULL, 'audioclip');
        $o[] = $this->bsPutFile($t1d12, 'file2.wav', "$exdir/ex2.wav", '', NULL, 'audioclip');
/*
*/
        $this->tdata['storage'] = $o;
    }

    /**
     *  test
     *
     */
    function test()
    {
        $this->test_log = '';
//        if($this->dbc->isError($p = parent::test())) return $p;
        $this->deleteData();
        $this->testData();
        if($this->config['useTrash']){
            $trash = "\n        {$this->config['TrashName']}";
        }else{
            $trash = "";
        }
        if(!$this->config['isArchive']){
            $this->test_correct = "    StorageRoot
        root
        test1
            file1.mp3
            public
            test1_folder1
                test1_folder1_1
                test1_folder1_2
                    file2.wav
            test1_folder2
        test2
            public
            test2_folder1
        test3
            public
        test4
            public{$trash}
";
        }else{
            $this->test_correct = "    StorageRoot
        root
        test1
            file1.mp3
            test1_folder1
                test1_folder1_1
                test1_folder1_2
                    file2.wav
            test1_folder2
        test2
            test2_folder1
        test3
        test4{$trash}
";
        }
        $r = $this->dumpTree($this->storId, '    ', '    ', '{name}');
        if($this->dbc->isError($r)) return $r;
        $this->test_dump = $r;
        if($this->test_dump==$this->test_correct)
            { $this->test_log.="# BasicStor::test: OK\n"; return true; }
        else return PEAR::raiseError(
            "BasicStor::test:\ncorrect:\n.{$this->test_correct}.\n".
            "dump:\n.{$this->test_dump}.\n", 1, PEAR_ERROR_RETURN);
    }

    /**
     *  initData - initialize
     *
     */
    function initData()
    {
        $this->rootId = $this->getRootNode();
        $this->storId = $this->wd =
            $this->addObj('StorageRoot', 'Folder', $this->rootId);
        $rootUid = parent::addSubj('root', $this->config['tmpRootPass']);
        $res = parent::addPerm($rootUid, '_all', $this->rootId, 'A');
        if($this->dbc->isError($res)) return $res;
        $res = parent::addPerm($rootUid, 'subjects', $this->rootId, 'A');
        if($this->dbc->isError($res)) return $res;
        $fid = $this->bsCreateFolder($this->storId, 'root');
        if($this->dbc->isError($fid)) return $fid;
        if($this->config['useTrash']){
            $tfid = $this->bsCreateFolder(
                $this->storId, $this->config["TrashName"]);
            if($this->dbc->isError($tfid)) return $tfid;
        }
        $allid = parent::addSubj($this->config['AllGr']);
        if($this->dbc->isError($allid)) return $allid;
        $r = $this->addSubj2Gr('root', $this->config['AllGr']);
        $r = $res = parent::addPerm($allid, 'read', $this->rootId, 'A');
        $admid = parent::addSubj($this->config['AdminsGr']);
        if($this->dbc->isError($admid)) return $admid;
        $r = $this->addSubj2Gr('root', $this->config['AdminsGr']);
        if($this->dbc->isError($r)) return $r;
        $res = parent::addPerm($admid, '_all', $this->rootId, 'A');
        if($this->dbc->isError($res)) return $res;
        if(!$this->config['isArchive']){
            $stPrefGr = parent::addSubj($this->config['StationPrefsGr']);
            if($this->dbc->isError($stPrefGr)) return $stPrefGr;
            $this->addSubj2Gr('root', $this->config['StationPrefsGr']);
        }
    }

    /**
     *  install - create tables
     *
     *  file states:
     *  <ul>
     *      <li>empty</li>
     *      <li>incomplete</li>
     *      <li>ready</li>
     *      <li>edited</li>
     *      <li>deleted</li>
     *  </ul>
     *  file types:
     *  <ul>
     *      <li>audioclip</li>
     *      <li>playlist</li>
     *      <li>webstream</li>
     *  </ul>
     *  access types:
     *  <ul>
     *      <li>access</li>
     *      <li>download</li>
     *  </ul>
     */
    function install()
    {
        parent::install();
        #echo "{$this->filesTable}\n";
        $r = $this->dbc->query("CREATE TABLE {$this->filesTable} (
            id int not null,
            gunid bigint not null,                      -- global unique ID
            name varchar(255) not null default'',       -- human file id ;)
            mime varchar(255) not null default'',       -- mime type
            ftype varchar(128) not null default'',      -- file type
            state varchar(128) not null default'empty', -- file state
            currentlyAccessing int not null default 0,  -- access counter
            editedBy int REFERENCES {$this->subjTable}  -- who edits it
        )");
        if($this->dbc->isError($r)) return $r;
        $this->dbc->query("CREATE UNIQUE INDEX {$this->filesTable}_id_idx
            ON {$this->filesTable} (id)");
        $this->dbc->query("CREATE UNIQUE INDEX {$this->filesTable}_gunid_idx
            ON {$this->filesTable} (gunid)");
        $this->dbc->query("CREATE INDEX {$this->filesTable}_name_idx
            ON {$this->filesTable} (name)");

        #echo "{$this->mdataTable}\n";
        $this->dbc->createSequence("{$this->mdataTable}_id_seq");
        $r = $this->dbc->query("CREATE TABLE {$this->mdataTable} (
            id int not null,
            gunid bigint,
            subjns varchar(255),             -- subject namespace shortcut/uri
            subject varchar(255) not null default '',
            predns varchar(255),             -- predicate namespace shortcut/uri
            predicate varchar(255) not null,
            predxml char(1) not null default 'T', -- Tag or Attribute
            objns varchar(255),              -- object namespace shortcut/uri
            object text
        )");
        if($this->dbc->isError($r)) return $r;
        $this->dbc->query("CREATE UNIQUE INDEX {$this->mdataTable}_id_idx
            ON {$this->mdataTable} (id)");
        $this->dbc->query("CREATE INDEX {$this->mdataTable}_gunid_idx
            ON {$this->mdataTable} (gunid)");
        $this->dbc->query("CREATE INDEX {$this->mdataTable}_subj_idx
            ON {$this->mdataTable} (subjns, subject)");
        $this->dbc->query("CREATE INDEX {$this->mdataTable}_pred_idx
            ON {$this->mdataTable} (predns, predicate)");

        #echo "{$this->accessTable}\n";
        $r = $this->dbc->query("CREATE TABLE {$this->accessTable} (
            gunid bigint,                               -- global unique id
            token bigint,                               -- access token
            chsum char(32) not null default'',          -- md5 checksum
            ext varchar(128) not null default'',        -- extension
            type varchar(20) not null default'',        -- access type
            parent bigint,                              -- parent token
            ts timestamp
        )");
        if($this->dbc->isError($r)) return $r;
        $this->dbc->query("CREATE INDEX {$this->accessTable}_token_idx
            ON {$this->accessTable} (token)");
        $this->dbc->query("CREATE INDEX {$this->accessTable}_gunid_idx
            ON {$this->accessTable} (gunid)");
        $this->dbc->query("CREATE INDEX {$this->accessTable}_parent_idx
            ON {$this->accessTable} (parent)");
        if(!file_exists($this->storageDir)){
            mkdir($this->storageDir, 02775);
        }
        if(!file_exists($this->bufferDir)){
            mkdir($this->bufferDir, 02775);
        }
        $this->initData();
    }
    /**
     *  id  subjns  subject predns  predicate   objns   object
     *  y1  literal xmbf    NULL    namespace   literal http://www.sotf.org/xbmf
     *  x1  gunid   <gunid> xbmf    contributor NULL    NULL
     *  x2  mdid    x1      xbmf    role        literal Editor
     *
     *  predefined shortcuts:
     *      _L              = literal
     *      _G              = gunid (global id of media file)
     *      _I              = mdid (local id of metadata record)
     *      _nssshortcut    = namespace shortcut definition
     *      _blank          = blank node
     */

    /**
     *  uninstall
     *
     *  @return void
     */
    function uninstall()
    {
        $this->dbc->query("DROP TABLE {$this->mdataTable}");
        $this->dbc->dropSequence("{$this->mdataTable}_id_seq");
        $this->dbc->query("DROP TABLE {$this->filesTable}");
        $this->dbc->query("DROP TABLE {$this->accessTable}");
        $d = @dir($this->storageDir);
        while (is_object($d) && (false !== ($entry = $d->read()))){
            if(filetype("{$this->storageDir}/$entry")=='dir'){
                if($entry!='CVS' && $entry!='tmp' && strlen($entry)==3)
                {
                    $dd = dir("{$this->storageDir}/$entry");
                    while (false !== ($ee = $dd->read())){
                        if(substr($ee, 0, 1)!=='.')
                            unlink("{$this->storageDir}/$entry/$ee");
                    }
                    $dd->close();
                    rmdir("{$this->storageDir}/$entry");
                }
            }
        }
        if(is_object($d)) $d->close();
        if(file_exists($this->bufferDir)){
            $d = dir($this->bufferDir);
            while (false !== ($entry = $d->read())) if(substr($entry,0,1)!='.')
                { unlink("{$this->bufferDir}/$entry"); }
            $d->close();
            @rmdir($this->bufferDir);
        }
        parent::uninstall();
    }

    /**
     *  Aux logging for debug
     *
     *  @param msg string - log message
     */
    function debugLog($msg)
    {
        $fp=fopen("{$this->storageDir}/log", "a") or die("Can't write to log\n");
        fputs($fp, date("H:i:s").">$msg<\n");
        fclose($fp);
    }

}
?>