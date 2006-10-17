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
require_once "BasicStor.php";

/**
 *  GreenBox class
 *
 *  LiveSupport file storage module
 *
 *  @author  $Author$
 *  @version $Revision$
 *  @see BasicStor
 */
class GreenBox extends BasicStor{

    /* ====================================================== storage methods */

    /**
     *  Create new folder
     *
     *  @param parid int, parent id
     *  @param folderName string, name for new folder
     *  @param sessid string, session id
     *  @return id of new folder
     *  @exception PEAR::error
     */
    function createFolder($parid, $folderName, $sessid='')
    {
        if(($res = $this->_authorize('write', $parid, $sessid)) !== TRUE)
            return $res;
        return $this->bsCreateFolder($parid, $folderName);
    }

    /**
     *  Store new file in the storage
     *
     *  @param parid int, parent id
     *  @param fileName string, name for new file
     *  @param mediaFileLP string, local path of media file
     *  @param mdataFileLP string, local path of metadata file
     *  @param sessid string, session id
     *  @param gunid string, global unique id OPTIONAL
     *  @param ftype string, internal file type
     *  @return int
     *  @exception PEAR::error
     */
    function putFile($parid, $fileName,
         $mediaFileLP, $mdataFileLP, $sessid='',
         $gunid=NULL, $ftype='audioclip')
    {
        if(($res = $this->_authorize('write', $parid, $sessid)) !== TRUE)
            return $res;
        $id = $this->bsPutFile(
            $parid, $fileName, $mediaFileLP, $mdataFileLP, $gunid, $ftype
        );
        return $id;
    }

    /**
     *  Store new webstream
     *
     *  @param parid int, parent id
     *  @param fileName string, name for new file
     *  @param mdataFileLP string, local path of metadata file
     *  @param sessid string, session id
     *  @param gunid string, global unique id OPTIONAL
     *  @param url string, wewbstream url
     *  @return int
     *  @exception PEAR::error
     */
    function storeWebstream($parid, $fileName, $mdataFileLP, $sessid='',
         $gunid=NULL, $url)
    {
        if(($res = $this->_authorize('write', $parid, $sessid)) !== TRUE)
            return $res;
        if(!file_exists($mdataFileLP)){
            $mdataFileLP = dirname(__FILE__).'/emptyWebstream.xml';
        }
        $oid = $this->bsPutFile(
            $parid, $fileName, '', $mdataFileLP, $gunid, 'webstream'
        );
        if(PEAR::isError($oid)) return $oid;
        $r = $this-> bsSetMetadataValue(
            $oid, 'ls:url', $url, NULL, NULL, 'metadata');
        if(PEAR::isError($r)) return $r;
        return $oid;
    }


    /**
     *  Access stored file - increase access counter
     *
     *  @param id int, virt.file's local id
     *  @param sessid string, session id
     *  @return string access token
     */
    function accessFile($id, $sessid='')
    {
        if(($res = $this->_authorize('read', $id, $sessid)) !== TRUE)
            return $res;
        $gunid = $this->_gunidFromId($id);
        $r = $this->bsAccess(NULL, '', $gunid, 'access');
        if(PEAR::isError($r)){ return $r; }
        $token = $r['token'];
        return $token;
    }

    /**
     *  Release stored file - decrease access counter
     *
     *  @param token string, access token
     *  @param sessid string, session id
     *  @return boolean
     */
    function releaseFile($token, $sessid='')
    {
        $r = $this->bsRelease($token, 'access');
        if(PEAR::isError($r)){ return $r; }
        return FALSE;
    }

    /**
     *  Analyze media file for internal metadata information
     *
     *  @param id int, virt.file's local id
     *  @param sessid string, session id
     *  @return array
     */
    function analyzeFile($id, $sessid='')
    {
        if(($res = $this->_authorize('read', $id, $sessid)) !== TRUE)
            return $res;
        return $this->bsAnalyzeFile($id);
    }

    /**
     *  Rename file
     *
     *  @param id int, virt.file's local id
     *  @param newName string
     *  @param sessid string, session id
     *  @return boolean or PEAR::error
     */
    function renameFile($id, $newName, $sessid='')
    {
        $parid = $this->getParent($id);
        if(($res = $this->_authorize('write', $parid, $sessid)) !== TRUE)
            return $res;
        return $this->bsRenameFile($id, $newName);
    }

    /**
     *  Move file
     *
     *  @param id int, virt.file's local id
     *  @param did int, destination folder local id
     *  @param sessid string, session id
     *  @return boolean or PEAR::error
     */
    function moveFile($id, $did, $sessid='')
    {
        if(($res = $this->_authorize(
            array('read', 'write'), array($id, $did), $sessid
        )) !== TRUE) return $res;
        return $this->bsMoveFile($id, $did);
    }

    /**
     *  Copy file
     *
     *  @param id int, virt.file's local id
     *  @param did int, destination folder local id
     *  @param sessid string, session id
     *  @return boolean or PEAR::error
     */
    function copyFile($id, $did, $sessid='')
    {
        if(($res = $this->_authorize(
            array('read', 'write'), array($id, $did), $sessid
        )) !== TRUE) return $res;
        return $this->bsCopyFile($id, $did);
    }

    /**
     *  Replace file. Doesn't change filetype!
     *
     *  @param id int, virt.file's local id
     *  @param mediaFileLP string, local path of media file
     *  @param mdataFileLP string, local path of metadata file
     *  @param sessid string, session id
     *  @return true or PEAR::error
     */
    function replaceFile($id, $mediaFileLP, $mdataFileLP, $sessid='')
    {
        if(($res = $this->_authorize('write', $id, $sessid)) !== TRUE)
            return $res;
        return $this->bsReplaceFile($id, $mediaFileLP, $mdataFileLP);
    }

    /**
     *  Delete file
     *
     *  @param id int, virt.file's local id
     *  @param sessid int
     *  @param forced boolean, if true don't use trash
     *  @return true or PEAR::error
     */
    function deleteFile($id, $sessid='', $forced=FALSE)
    {
        $parid = $this->getParent($id);
        if(($res = $this->_authorize('write', $parid, $sessid)) !== TRUE)
            return $res;
        return $this->bsDeleteFile($id, $forced);
    }

    /* ------------------------------------------------------------- metadata */

    /**
     *  Replace metadata with new XML file or string
     *
     *  @param id int, virt.file's local id
     *  @param mdata string, XML string or local path of metadata XML file
     *  @param mdataLoc string, metadata location: 'file'|'string'
     *  @param sessid string, session id
     *  @return boolean or PEAR::error
     */
    function replaceMetadata($id, $mdata, $mdataLoc='file', $sessid='')
    {
        if(($res = $this->_authorize('write', $id, $sessid)) !== TRUE)
            return $res;
        return $this->bsReplaceMetadata($id, $mdata, $mdataLoc);
    }

    /**
     *  Get metadata XML tree as string
     *
     *  @param id int, virt.file's local id
     *  @param sessid string, session id
     *  @return string or PEAR::error
     */
    function getMdata($id, $sessid='')
    {
        if(($res = $this->_authorize('read', $id, $sessid)) !== TRUE)
            return $res;
        return $this->bsGetMetadata($id);
    }

    /**
     *  Return metadata as hierarchical PHP hash-array
     *
     *  If xml:lang attribute is specified in metadata category,
     *  array of metadata values indexed by xml:lang values
     *  is presented instead of one plain metadata value.
     *
     *  @param id int, local object id
     *  @param sessid string, session ID
     *  @return array
     */
    function getMdataArray($id, $sessid)
    {
        if(($res = $this->_authorize('read', $id, $sessid)) !== TRUE)
            return $res;
        $ac = StoredFile::recall($this, $id);
        if(PEAR::isError($ac)){ return $ac; }
        $arr = $ac->md->genPhpArray();
        $md = FALSE;
        foreach($arr['children'] as $i=>$a){
            if($a['elementname'] == 'metadata'){
                $md = $arr['children'][$i];
                break;
            }
        }
        if($md === FALSE){
            return PEAR::raiseError(
                "GreenBox::getMdataArray: no metadata container found"
            );
        }
        $res = array();
        foreach($md['children'] as $i=>$el){
            $lang = $el['attrs']['xml:lang'];
            $category = $el['elementname'];
            if($lang == ""){
                $res[$category] = $el['content'];
            }else{
                $res[$category][$lang] = $el['content'];
            }
        }
        return $res;
    }

    /**
     *  Get metadata element value
     *
     *  @param id int, virt.file's local id
     *  @param category string, metadata element name
     *  @param sessid string, session id
     *  @param lang string, optional xml:lang value for select language version
     *  @param deflang string, optional xml:lang for default language
     *  @return array of matching records as hash with fields:
     *   <ul>
     *      <li>mid int, local metadata record id</li>
     *      <li>value string, element value</li>
     *      <li>attrs hasharray of element's attributes indexed by
     *          qualified name (e.g. xml:lang)</li>
     *   </ul>
     */
    function getMdataValue($id, $category, $sessid='',
        $lang=NULL, $deflang=NULL)
    {
        if(($res = $this->_authorize('read', $id, $sessid)) !== TRUE)
            return $res;
        return $this->bsGetMetadataValue($id, $category, $lang, $deflang);
    }

    /**
     *  Set metadata element value
     *
     *  @param id int, virt.file's local id
     *  @param category string, metadata element identification (e.g. dc:title)
     *  @param sessid string, session id
     *  @param value string/NULL value to store, if NULL then delete record
     *  @param lang string, optional xml:lang value for select language version
     *  @param mid int, metadata record id (OPTIONAL on unique elements)
     *  @return boolean
     */
    function setMdataValue($id, $category, $sessid, $value, $lang=NULL, $mid=NULL)
    {
        if(($res = $this->_authorize('write', $id, $sessid)) !== TRUE)
            return $res;
        if ($this->getFileType($id) == 'audioclip') {
	        require_once 'AnonymAccess.php';
	        if ($category == NICE_NAME_CATEGORY) {
	            $aa = new AnonymAccess($this,$sessid);
	            $aa->changeIt($id, $value, $lang);
	            /* $aa->debug(
	                array(
	                    'id'=>$id,
	                    'category'=>$category,
	                    'sessid'=>$sessid,
	                    'value'=>$value,
	                    'lang'=>$lang,
	                    'mid'=>$mid
	                ),'new'
	            ); */
	        }
        }
        return $this->bsSetMetadataValue($id, $category, $value, $lang, $mid);
    }
    
    function getAnonymFileName($gunid,$sessid,$lang=NULL) {
        require_once 'AnonymAccess.php';
        $aa = new AnonymAccess($this,$sessid);
        return $aa->getM3UByGunid($gunid,$lang);
    }

    function getAnonymURL($gunid,$sessid,$lang=NULL) {
        require_once 'AnonymAccess.php';
        $aa = new AnonymAccess($this,$sessid);
        return $this->config['anonymAccessURL'].'/'.$aa->getM3UByGunid($gunid,$lang);
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
     *     <li>limit : int - limit for result arrays (0 means unlimited)</li>
     *     <li>offset : int - starting point (0 means without offset)</li>
     *     <li>orderby : string - metadata category for sorting (optional)
     *         default sorting by dc:title (+ primary sorting by filetype -
     *         audioclips, playlists, webstreams ...)
     *     </li>
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
     *  @param sessid string, session id
     *  @return array of hashes, fields:
     *   <ul>
     *       <li>cnt : integer - number of matching gunids 
     *              of files have been found</li>
     *       <li>results : array of hashes:
     *          <ul>
     *           <li>gunid: string</li>
     *           <li>type: string - audioclip | playlist | webstream</li>
     *           <li>title: string - dc:title from metadata</li>
     *           <li>creator: string - dc:creator from metadata</li>
     *           <li>length: string - dcterms:extent in extent format</li>
     *          </ul>
     *      </li>
     *   </ul>
     *  @see BasicStor::bsLocalSearch
     */
    function localSearch($criteria, $sessid='')
    {
        $limit  = intval(isset($criteria['limit']) ? $criteria['limit'] : 0);
        $offset = intval(isset($criteria['offset']) ? $criteria['offset'] : 0);
        return $this->bsLocalSearch($criteria, $limit, $offset);
    }

    /**
     *  Return values of specified metadata category
     *
     *  @param category string, metadata category name
     *          with or without namespace prefix (dc:title, author)
     *  @param criteria hash, see localSearch method
     *  @param sessid string
     *  @return hash, fields:
     *       results : array with found values
     *       cnt : integer - number of matching values
     *  @see BasicStor::bsBrowseCategory
     */
    function browseCategory($category, $criteria, $sessid='')
    {
        $limit  = intval(isset($criteria['limit']) ? $criteria['limit'] : 0);
        $offset = intval(isset($criteria['offset']) ? $criteria['offset'] : 0);
        $res = $this->bsBrowseCategory($category, $limit, $offset, $criteria);
        return $res;
    }
    
    /*===================================================== tracklist methods */
    /**
     *  Create a new empty tracklist.
     *
     *  @param parid int, parent id
     *  @param fname string, human readable menmonic file name
     *  @param sessid string, session ID
     *  @return int, local id of created tracklist
     */
    function createTracklist($parid, $fname, $sessid='')
    {
        $gunid  = StoredFile::_createGunid();
        require_once"LocStor.php";
        $lc =& new LocStor($this->dbc, $this->config);
        echo '<XMP STYLE="background:yellow;">before</XMP>';
        $gunid2 = $lc->createTracklist($sessid, $gunid, $fname);
        echo '<XMP STYLE="background:yellow;">after</XMP>';
        if(PEAR::isError($gunid2)) return $gunid2;
        // get local id:
        $id = $this->_idFromGunid($gunid2);
        if(PEAR::isError($id)) return $id;
        // get home dir id:
        $hdid = $this->_getHomeDirIdFromSess($sessid);
        if(PEAR::isError($hdid)) return $hdid;
        // optionally move it to the destination folder:
        if($parid != $hdid && !is_null($parid)){
            $r = $this->bsMoveFile($id, $parid);
            if(PEAR::isError($r)){ return $r; }
        }
        return $id;
    }
        
    /**
     *  Check whether a Tracklist metafile with the given tracklist ID
     *  is available for editing, i.e., exists and is not marked as
     *  beeing edited.
     *
     *  @param id int, local id
     *  @param sessid string, session ID
     *  @return TRUE | int - id of user editing it
     */
    function tracklistIsAvailable($id, $sessid)
    {
        $gunid = $this->_gunidFromId($id);
        require_once"LocStor.php";
        $lc =& new LocStor($this->dbc, $this->config);
        return $lc->tracklistIsAvailable($sessid, $gunid, TRUE);
    }
    

    /**
     *  Mark tracklist as edited and return edit token
     *
     *  @param id int, local object id
     *  @param sessid string, session ID
     *  @return string, tracklist access token
     */
    function lockTracklistForEdit($id, $sessid)
    {
        $gunid = $this->_gunidFromId($id);
        require_once"LocStor.php";
        $lc =& new LocStor($this->dbc, $this->config);
        $res = $lc->editTracklist($sessid, $gunid);
        if(PEAR::isError($res)) return $res;
        return $res['token'];
    }

    /*====================================================== playlist methods */
    /**
     *  Create a new empty playlist.
     *
     *  @param parid int, parent id
     *  @param fname string, human readable menmonic file name
     *  @param sessid string, session ID
     *  @return int, local id of created playlist
     */
    function createPlaylist($parid, $fname, $sessid='')
    {
        $gunid  = StoredFile::_createGunid();
        require_once"LocStor.php";
        $lc =& new LocStor($this->dbc, $this->config);
        $gunid2 = $lc->createPlaylist($sessid, $gunid, $fname);
        if(PEAR::isError($gunid2)) return $gunid2;
        // get local id:
        $id = $this->_idFromGunid($gunid2);
        if(PEAR::isError($id)) return $id;
        // get home dir id:
        $hdid = $this->_getHomeDirIdFromSess($sessid);
        if(PEAR::isError($hdid)) return $hdid;
        // optionally move it to the destination folder:
        if($parid != $hdid && !is_null($parid)){
            $r = $this->bsMoveFile($id, $parid);
            if(PEAR::isError($r)){ return $r; }
        }
        return $id;
    }

    /**
     *  Return playlist as XML string
     *
     *  @param id int, local object id
     *  @param sessid string, session ID
     *  @return string, XML
     */
    function getPlaylistXml($id, $sessid)
    {
        return $this->getMdata($id, $sessid);
    }

    /**
     *  Return playlist as hierarchical PHP hash-array
     *
     *  @param id int, local object id
     *  @param sessid string, session ID
     *  @return array
     */
    function getPlaylistArray($id, $sessid)
    {
        $gunid = $this->_gunidFromId($id);
        $pl = StoredFile::recall($this, $id);
        if(PEAR::isError($pl)){ return $pl; }
        $gunid = $pl->gunid;
        return $pl->md->genPhpArray();
    }

    /**
     *  Mark playlist as edited and return edit token
     *
     *  @param id int, local object id
     *  @param sessid string, session ID
     *  @return string, playlist access token
     */
    function lockPlaylistForEdit($id, $sessid)
    {
        $gunid = $this->_gunidFromId($id);
        require_once"LocStor.php";
        $lc =& new LocStor($this->dbc, $this->config);
        $res = $lc->editPlaylist($sessid, $gunid);
        if(PEAR::isError($res)) return $res;
        return $res['token'];
    }

    /**
     *  Release token, regenerate XML from DB and clear edit flag.
     *
     *  @param token string, playlist access token
     *  @param sessid string, session ID
     *  @return string gunid
     */
    function releaseLockedPlaylist($token, $sessid)
    {
        $gunid = $this->bsCloseDownload($token, 'metadata');
        if(PEAR::isError($gunid)) return $gunid;
        $ac = StoredFile::recallByGunid($this, $gunid);
        if(PEAR::isError($ac)){ return $ac; }
        $r = $ac->md->regenerateXmlFile();
        if(PEAR::isError($r)) return $r;
        $this->_setEditFlag($gunid, FALSE, $sessid);
        return $gunid;
    }

    /**
     *  Add audioclip specified by local id to the playlist
     *
     *  @param token string, playlist access token
     *  @param acId string, local ID of added file
     *  @param sessid string, session ID
     *  @param fadeIn string, optional, in time format hh:mm:ss.ssssss
     *  @param fadeOut string, dtto
     *  @param length string - optional length in extent format -
     *          for webstream (or for overrule length of audioclip)
     *  @param pause string, optional - pause between half-faded points
     *          in time format hh:mm:ss.ssssss
     *  @return string, generated playlistElement gunid
     */
    function addAudioClipToPlaylist($token, $acId, $sessid,
        $fadeIn=NULL, $fadeOut=NULL, $length=NULL, $pause=NULL)
    {
        require_once"Playlist.php";
        $pl = Playlist::recallByToken($this, $token);
        if(PEAR::isError($pl)) return $pl;
        $acGunid = $this->_gunidFromId($acId);
        if($pl->_cyclicRecursion($acGunid)){
            return PEAR::raiseError(
                "GreenBox::addAudioClipToPlaylist: cyclic-recursion detected".
                " ($type)"
            );
        }
//        $res = $pl->addAudioClip($acId, $fadeIn, $fadeOut, NULL, $pause);
        $res = $pl->addAudioClip($acId, $fadeIn, $fadeOut, NULL, $length);
        if(PEAR::isError($res)) return $res;
        // recalculate offsets and total length:
//        $r = $pl->recalculateTimes($pause);
        $r = $pl->recalculateTimes();
        if(PEAR::isError($r)){ return $r; }
        return $res;
    }

    /**
     *  Remove audioclip from playlist
     *
     *  @param token string, playlist access token
     *  @param plElGunid string, global id of deleted playlistElement
     *  @param sessid string, session ID
     *  @return boolean
     */
    function delAudioClipFromPlaylist($token, $plElGunid, $sessid)
    {
        require_once"Playlist.php";
        $pl = Playlist::recallByToken($this, $token);
        if(PEAR::isError($pl)) return $pl;
        $res = $pl->delAudioClip($plElGunid);
        if(PEAR::isError($res)) return $res;
        // recalculate offsets and total length:
        $r = $pl->recalculateTimes();
        if(PEAR::isError($r)){ return $r; }
        return $res;
    }

    /**
     *  Change fadeInfo values
     *
     *  @param token string, playlist access token
     *  @param plElGunid string, global id of deleted playlistElement
     *  @param fadeIn string, optional, in time format hh:mm:ss.ssssss
     *  @param fadeOut string, dtto
     *  @param sessid string, session ID
     *  @return boolean
     */
    function changeFadeInfo($token, $plElGunid, $fadeIn, $fadeOut, $sessid)
    {
        require_once"Playlist.php";
        $pl = Playlist::recallByToken($this, $token);
        if(PEAR::isError($pl)) return $pl;
        $res = $pl->changeFadeInfo($plElGunid, $fadeIn, $fadeOut);
        if(PEAR::isError($res)) return $res;
        // recalculate offsets and total length:
        $r = $pl->recalculateTimes();
        if(PEAR::isError($r)){ return $r; }
        return TRUE;
    }

    /**
     *  Move audioClip to the new position in the playlist.
     *
     *  This method may change id attributes of playlistElements and/or
     *  fadeInfo.
     *
     *  @param token string, playlist access token
     *  @param plElGunid string, global id of deleted playlistElement
     *  @param newPos int - new position in playlist
     *  @param sessid string, session ID
     *  @return boolean
     */
    function moveAudioClipInPlaylist($token, $plElGunid, $newPos, $sessid)
    {
        require_once"Playlist.php";
        $pl = Playlist::recallByToken($this, $token);
        if(PEAR::isError($pl)) return $pl;
        $res = $pl->moveAudioClip($plElGunid, $newPos);
        if(PEAR::isError($res)) return $res;
        // recalculate offsets and total length:
        $r = $pl->recalculateTimes();
        if(PEAR::isError($r)){ return $r; }
        return TRUE;
    }

    /**
     *  RollBack playlist changes to the locked state
     *
     *  @param token string, playlist access token
     *  @param sessid string, session ID
     *  @return string gunid of playlist
     */
    function revertEditedPlaylist($token, $sessid='')
    {
        require_once"LocStor.php";
        $lc =& new LocStor($this->dbc, $this->config);
        return $lc->revertEditedPlaylist($token, $sessid);
    }

    /**
     *  Delete a Playlist metafile.
     *
     *  @param id int, local id
     *  @param sessid string, session ID
     *  @return boolean
     */
    function deletePlaylist($id, $sessid)
    {
        $gunid = $this->_gunidFromId($id);
        require_once"LocStor.php";
        $lc =& new LocStor($this->dbc, $this->config);
        return $lc->deletePlaylist($sessid, $gunid);
    }

    /**
     *  Find info about clip at specified offset in playlist.
     *
     *  @param sessid string, session id
     *  @param plid string, playlist global unique id
     *  @param offset string, current playtime (hh:mm:ss.ssssss)
     *  @param distance int, 0=current clip; 1=next clip ...
     *  @param lang string, optional xml:lang value for select language version
     *  @param deflang string, optional xml:lang for default language
     *  @return array of matching clip info:
     *   <ul>
     *      <li>gunid string, global unique id of clip</li>
     *      <li>elapsed string, already played time of clip</li>
     *      <li>remaining string, remaining time of clip</li>
     *      <li>duration string, total playlength of clip </li>
     *   </ul>
     */
    function displayPlaylistClipAtOffset($sessid, $plid, $offset, $distance=0,
        $lang=NULL, $deflang=NULL)
    {
        require_once"Playlist.php";
        $pl = Playlist::recallByGunid($this, $plid);
        if(PEAR::isError($pl)) return $pl;
        $res = $pl->displayPlaylistClipAtOffset($offset, $distance);
        if(PEAR::isError($res)) return $res;
        $res['title'] = NULL;
        $id = $this->_idFromGunid($res['gunid']);
        if(PEAR::isError($id)) return $id;
        if(!is_null($id)){
            $md = $this->bsGetMetadataValue($id, "dc:title", $lang, $deflang);
            if(PEAR::isError($md)) return $md;
            if(isset($md[0]['value'])){ $res['title'] = $md[0]['value']; }
        }
        return $res;
    }
    
    /**
     *  Create a tarfile with playlist export - playlist and all matching
     *  sub-playlists and media files (if desired)
     *
     *  @param sessid - string, session ID
     *  @param plids - array of strings, playlist global unique IDs
     *          (one gunid is accepted too)
     *  @param type - string, playlist format, values: lspl | smil | m3u
     *  @param standalone - boolean, if only playlist should be exported or
     *          with all related files
     *  @return hasharray with  fields:
     *      fname string: readable fname,
     *      token string: access token
     */
    function exportPlaylistOpen($sessid, $plids, $type='lspl', $standalone=FALSE)
    {
        return $this->bsExportPlaylistOpen($plids, $type, $standalone);
    }
    
    /**
     *  Close playlist export previously opened by the exportPlaylistOpen method
     *
     *  @param token - string, access token obtained from exportPlaylistOpen
     *            method call
     *  @return boolean true or error object
     */
    function exportPlaylistClose($token)
    {
        return $this->bsExportPlaylistClose($token);
    }
    
    /**
     *  Open writable handle for import playlist in LS Archive format
     *
     *  @param sessid string, session id
     *  @param chsum string, md5 checksum of imported file (optional)
     *  @return hasharray with:
     *      fname string: writable local filename
     *      token string: put token
     */
    function importPlaylistOpen($sessid, $chsum='')
    {
        $userid = $r =$this->getSessUserId($sessid);
        if($this->dbc->isError($r)) return $r;
        $r = $this->bsOpenPut($chsum, NULL, $userid);
        if(PEAR::isError($r)) return $r;
        return $r;
    }
    
    /**
     *  Close import-handle and import playlist
     *
     *  @param token string, import token obtained by importPlaylistOpen method
     *  @return int, result file local id (or error object)
     */
    function importPlaylistClose($token)
    {
        $arr = $r = $this->bsClosePut($token);
        if(PEAR::isError($r)) return $r;
        $fname = $arr['fname'];
        $owner = $arr['owner'];
        $parid = $r= $this->_getHomeDirId($owner);
        if(PEAR::isError($r)) {
            if(file_exists($fname)) @unlink($fname);
            return $r;
        }
        $res = $r = $this->bsImportPlaylist($parid, $fname, $owner);
        if(file_exists($fname)) @unlink($fname);
        if(PEAR::isError($r)) return $r;
        return $res;
    }
    
    /**
     *  Check whether a Playlist metafile with the given playlist ID exists.
     *
     *  @param id int, local id
     *  @param sessid string, session ID
     *  @return boolean
     */
    function existsPlaylist($id, $sessid)
    {
        $gunid = $this->_gunidFromId($id);
        require_once"LocStor.php";
        $lc =& new LocStor($this->dbc, $this->config);
        return $lc->existsPlaylist($sessid, $gunid);
    }

    /**
     *  Check whether a Playlist metafile with the given playlist ID
     *  is available for editing, i.e., exists and is not marked as
     *  beeing edited.
     *
     *  @param id int, local id
     *  @param sessid string, session ID
     *  @return TRUE | int - id of user editing it
     */
    function playlistIsAvailable($id, $sessid)
    {
        $gunid = $this->_gunidFromId($id);
        require_once"LocStor.php";
        $lc =& new LocStor($this->dbc, $this->config);
        return $lc->playlistIsAvailable($sessid, $gunid, TRUE);
    }

    /* ---------------------------------------------- time conversion methods */
    /**
     *  Convert playlist time value to float seconds
     *
     *  @param plt string, playlist time value (HH:mm:ss.dddddd)
     *  @return int, seconds
     */
    function _plTimeToSecs($plt)
    {
        require_once"Playlist.php";
        return Playlist::_plTimeToSecs($plt);
    }

    /**
     *  Convert float seconds value to playlist time format
     *
     *  @param s0 int, seconds
     *  @return string, time in playlist time format (HH:mm:ss.dddddd)
     */
    function _secsToPlTime($s0)
    {
        require_once"Playlist.php";
        return Playlist::_secsToPlTime($s0);
    }
    
    /* ------------------------------------------------------- render methods */
    /**
     *  Render playlist to ogg file (open handle)
     *
     *  @param sessid  :  string  -  session id
     *  @param plid : string  -  playlist gunid
     *  @return token : string - render token
     */
    function renderPlaylistToFileOpen($sessid, $plid)
    {
        require_once "Renderer.php";
        $r = Renderer::rnRender2FileOpen($this, $plid);
        if(PEAR::isError($r)) return $r;
        return $r;
    }

    /**
     *  Render playlist to ogg file (check results)
     *
     *  @param token  :  string  -  render token
     *  @return hasharray:
     *      status : string - susccess | working | fault
     *      tmpfile : string - filepath to result temporary file
     */
    function renderPlaylistToFileCheck($token)
    {
        require_once "Renderer.php";
        $r = Renderer::rnRender2FileCheck($this, $token);
        if(PEAR::isError($r)) return $r;
        return array('status'=>$r['status'], 'tmpfile'=>$r['tmpfile']);
    }

    /**
     *  Render playlist to ogg file (list results)
     *
     *  @param status  :  string  -  success | working | fault 
     *      if this parameter is not set, then return with all unclosed
     *  @return array of hasharray:
     *      status : string - susccess | working | fault
     *      tmpfile : string - filepath to result temporary file
     */
    function renderPlaylistToFileList($status='')
    {
        require_once "Renderer.php";
        return Renderer::rnRender2FileList($this, $status);
    }

    /**
     *  Render playlist to ogg file (close handle)
     *
     *  @param token  :  string  -  render token
     *  @return status : boolean
     */
    function renderPlaylistToFileClose($token)
    {
        require_once "Renderer.php";
        $r = Renderer::rnRender2FileClose($this, $token);
        if(PEAR::isError($r)) return $r;
        return array(TRUE);
    }


    /**
     *  Render playlist to storage media clip (open handle)
     *
     *  @param sessid  :  string  -  session id
     *  @param plid : string  -  playlist gunid
     *  @return token : string - render token
     */
    function renderPlaylistToStorageOpen($sessid, $plid)
    {
        require_once "Renderer.php";
        $owner = $this->getSessUserId($sessid);
        if($this->dbc->isError($owner)) return $owner;
        $r = Renderer::rnRender2FileOpen($this, $plid, $owner);
        if(PEAR::isError($r)) return $r;
        return $r;
    }

    /**
     *  Render playlist to storage media clip (check results)
     *
     *  @param token  :  string  -  render token
     *  @return hasharray:
     *      status : string - susccess | working | fault
     *      gunid : string - gunid of result file
     */
    function renderPlaylistToStorageCheck($token)
    {
        require_once "Renderer.php";
        $r = Renderer::rnRender2StorageCheck($this, $token);
        if(PEAR::isError($r)) return $r;
        return $r;
    }


    /**
     *  Render playlist to RSS file (open handle)
     *
     *  @param sessid  :  string  -  session id
     *  @param plid : string  -  playlist gunid
     *  @return token : string - render token
     */
    function renderPlaylistToRSSOpen($sessid, $plid)
    {
        $token = '123456789abcdeff';
        $fakeFile = "{$this->accessDir}/$token.rss";
        file_put_contents($fakeFile, "fake rendered file");
        return array('token'=>$token);
    }

    /**
     *  Render playlist to RSS file (check results)
     *
     *  @param token  :  string  -  render token
     *  @return hasharray:
     *      status : string - susccess | working | fault
     *      tmpfile : string - filepath to result temporary file
     */
    function renderPlaylistToRSSCheck($token)
    {
        $fakeFile = "{$this->accessDir}/$token.rss";
        if($token != '123456789abcdeff' || !file_exists($fakeFile)){
            return PEAR::raiseError(
                "LocStor::renderPlaylistToRSSCheck: invalid token ($token)"
            );
        }
        return array(
            'status'=> 'success',
            'tmpfile'   => $fakeFile,
        );
    }

    /**
     *  Render playlist to RSS file (list results)
     *
     *  @param status  :  string  -  success | working | fault
     *  @return array of hasharray:
     *      status : string - susccess | working | fault
     *      tmpfile : string - filepath to result temporary file
     */
    function renderPlaylistToRSSList($status='')
    {
      $dummytokens = array ('123456789abcdeff');
      foreach ($dummytokens as $token) {
          $r[] = renderPlaylistToRSSCheck($token);
      }
      return $r;
    }

    /**
     *  Render playlist to RSS file (close handle)
     *
     *  @param token  :  string  -  render token
     *  @return status : boolean
     */
    function renderPlaylistToRSSClose($token)
    {
        if($token != '123456789abcdeff'){
            return PEAR::raiseError(
                "GreenBox::renderPlaylistToRSSClose: invalid token"
            );
        }
        $fakeFile = "{$this->accessDir}/$token.rss";
        unlink($fakeFile);
        return TRUE;
    }


    /*================================================= storage admin methods */
    /* ------------------------------------------------------- backup methods */
    /**
     *  Create backup of storage (open handle)
     *
     *  @param sessid   :  string  -  session id
     *  @param criteria : struct - see search criteria
     *  @return hasharray:
     *           token  : string - backup token
     */
    function createBackupOpen($sessid, $criteria='')
    {
        require_once "Backup.php";
        $bu = $r = new Backup($this);
        if (PEAR::isError($r)) return $r;
        return $bu->openBackup($sessid,$criteria);
    }

    /**
     *  Create backup of storage (check results)
     *
     *  @param token  :  string  -  backup token
     *  @return hasharray with field: 
     *      status : string - susccess | working | fault
     *      faultString: string - description of fault
     *      token  : stirng - backup token
     *      url    : string - access url
     */
    function createBackupCheck($token)
    {
        require_once "Backup.php";
        $bu = $r = new Backup($this);
        if (PEAR::isError($r)) return $r;
        return $bu->checkBackup($token);
    }

    /**
     *  Create backup of storage (list results)
     *
     *  @param sessid : string - session id
     *  @param stat : status (optional)
     *      if this parameter is not set, then return with all unclosed backups
     *  @return array of hasharray with field: 
     *      status : string - susccess | working | fault
     *      token  : stirng - backup token
     *      url    : string - access url
     */
    function createBackupList($sessid, $stat='')
    {
        require_once "Backup.php";
        $bu = $r = new Backup($this);
        if (PEAR::isError($r)) return $r;
        return $bu->listBackups($stat);
    }

    /**
     *  Create backup of storage (close handle)
     *
     *  @param token   :  string  -  backup token
     *  @return status :  boolean
     */
    function createBackupClose($token)
    {
        require_once "Backup.php";
        $bu = $r = new Backup($this);
        if (PEAR::isError($r)) return $r;
        return $bu->closeBackup($token);
    }
    
    /* ===================================================== restore funcitons*/
    /**
     *  Restore a backup file
     *
     *  @param  sessid   :  string - session id
     *  @param  filename :  string - backup file path
     *  @return token    :  string - restore token
     */
    function backupRestoreOpen($sessid, $filename)
    {
        require_once 'Restore.php';
        $rs = new Restore($this);
        if (PEAR::isError($rs)) return $rs;
        return $rs->openRestore($sessid,$filename);
    }

    /**
     *  Check status of backup restore
     *
     *  @param token   :  string    -  restore token
     *  @return status :  hasharray - fields:
     * 							token:  string - restore token
     *                          status: string - working | fault | success
     *                          faultString: string - description of fault 
     */
    function backupRestoreCheck($token)
    {
        require_once 'Restore.php';
        $rs = new Restore($this);
        if (PEAR::isError($rs)) return $rs;
        return $rs->checkRestore($token);
    }
    
    /**
     *  Close a restore procedure
     *
     *  @param token   :  string    -  restore token
     *  @return status :  bool      -  is success
     */
    function backupRestoreClose($token) {
    	require_once 'Restore.php';
    	$rs = new Restore($this);
    	if (PEAR::isError($rs)) return $rs;
    	return $rs->closeRestore($token);
    }
    /* ============================================== methods for preferences */

    /**
     *  Read preference record by session id
     *
     *  @param sessid string, session id
     *  @param key string, preference key
     *  @return string, preference value
     */
    function loadPref($sessid, $key)
    {
        require_once 'Prefs.php';
        $pr =& new Prefs($this);
        $res = $pr->loadPref($sessid, $key);
        return $res;
    }

    /**
     *  Save preference record by session id
     *
     *  @param sessid string, session id
     *  @param key string, preference key
     *  @param value string, preference value
     *  @return boolean
     */
    function savePref($sessid, $key, $value)
    {
        require_once 'Prefs.php';
        $pr =& new Prefs($this);
        $res = $pr->savePref($sessid, $key, $value);
        return $res;
    }

    /**
     *  Delete preference record by session id
     *
     *  @param sessid string, session id
     *  @param key string, preference key
     *  @return boolean
     */
    function delPref($sessid, $key)
    {
        require_once 'Prefs.php';
        $pr =& new Prefs($this);
        $res = $pr->delPref($sessid, $key);
        return $res;
    }

    /**
     *  Read group preference record
     *
     *  @param sessid string, session id
     *  @param group string, group name
     *  @param key string, preference key
     *  @return string, preference value
     */
    function loadGroupPref($sessid, $group, $key)
    {
        require_once 'Prefs.php';
        $pr =& new Prefs($this);
        $res = $pr->loadGroupPref($sessid, $group, $key);
        return $res;
    }

    /**
     *  Save group preference record
     *
     *  @param sessid string, session id
     *  @param group string, group name
     *  @param key string, preference key
     *  @param value string, preference value
     *  @return boolean
     */
    function saveGroupPref($sessid, $group, $key, $value)
    {
        require_once 'Prefs.php';
        $pr =& new Prefs($this);
        $res = $pr->saveGroupPref($sessid, $group, $key, $value);
        return $res;
    }

    /**
     *  Delete group preference record
     *
     *  @param sessid string, session id
     *  @param group string, group name
     *  @param key string, preference key
     *  @return boolean
     */
    function delGroupPref($sessid, $group, $key)
    {
        require_once 'Prefs.php';
        $pr =& new Prefs($this);
        $res = $pr->delGroupPref($sessid, $group, $key);
        return $res;
    }

    /* =================================================== networking methods */
    /* ------------------------------------------------------- common methods */
    /**
     *  Common "check" method for transports
     *
     *  @param trtok: string - transport token
     *  @return struct/hasharray with fields:
     *      trtype: string - audioclip | playlist | search | file
     *      state: string - transport state
     *      direction: string - up | down
     *      expectedsize: int - file size in bytes
     *      realsize: int - currently transported bytes
     *      expectedchsum: string - orginal file checksum
     *      realchsum: string - transported file checksum
     *      ... ?
     */
    function getTransportInfo($trtok)
    {
        require_once"Transport.php";
        $tr =& new Transport($this);
        return $tr->getTransportInfo($trtok);
    }
    
    /**
     *  Turn transports on/off, optionaly return current state.
     *
     *  @param sessid string: session id
     *  @param onOff: boolean optional (if not used, current state is returned)
     *  @return boolean - previous state
     */
    function turnOnOffTransports($sessid, $onOff=NULL)
    {
        require_once"Transport.php";
        $tr =& new Transport($this);
        return $tr->turnOnOffTransports($sessid, $onOff);
    }
    
    /**
     *  Pause, resume or cancel transport
     *
     *  @param trtok: string - transport token
     *  @param action: string - pause | resume | cancel
     *  @return string - resulting transport state
     */
    function doTransportAction($trtok, $action)
    {
        require_once"Transport.php";
        $tr =& new Transport($this);
        $res = $tr->doTransportAction($trtok, $action);
        return $res;
    }
    
    /* ------------------------ methods for ls-archive-format file transports */
    /**
     *  Open async file transfer from local storageServer to network hub,
     *  file should be ls-archive-format file.
     *
     *  @param filePath string - local path to uploaded file
     *  @return string - transport token
     */
    function uploadFile2Hub($filePath)
    {
        require_once"Transport.php";
        $tr =& new Transport($this);
        return $tr->uploadFile2Hub($filePath);
    }
    
    /**
     *  Get list of prepared transfers initiated by hub
     *
     *  @return array of structs/hasharrays with fields:
     *      trtok: string transport token
     *      ... ?
     */
    function getHubInitiatedTransfers()
    {
        require_once"Transport.php";
        $tr =& new Transport($this);
        return $tr->getHubInitiatedTransfers();
    }
    
    /**
     *  Start of download initiated by hub
     *
     *  @param trtok: string - transport token obtained from
     *          the getHubInitiatedTransfers method
     *  @return string - transport token
     */
    function startHubInitiatedTransfer($trtok)
    {
        require_once"Transport.php";
        $tr =& new Transport($this);
        return $tr->startHubInitiatedTransfer($trtok);
    }
    
    /* ------------- special methods for audioClip/webstream object transport */

    /**
     *  Start upload of audioClip/webstream/playlist from local storageServer
     *  to hub
     *
     *  @param gunid: string - global unique id of object being transported
     *  @param withContent: boolean - if true, transport playlist content too
     *  @return string - transport token
     */
    function upload2Hub($gunid, $withContent=FALSE)
    {
        require_once"Transport.php";
        $tr =& new Transport($this);
        return $tr->upload2Hub($gunid, $withContent);
    }
    
    /**
     *  Start download of audioClip/webstream/playlist from hub to local
     *  storageServer
     *
     *  @param sessid string: session id
     *  @param gunid: string - global unique id of playlist being transported
     *  @param withContent: boolean - if true, transport playlist content too
     *  @return string - transport token
     */
    function downloadFromHub($sessid, $gunid, $withContent=TRUE){
        $uid = $r = $this->getSessUserId($sessid);
        if($this->dbc->isError($r)) return $r;
        require_once"Transport.php";
        $tr =& new Transport($this);
        return $tr->downloadFromHub($uid, $gunid, $withContent);
    }


    /* ------------------------------------------------ global-search methods */
    /**
     *  Start search job on network hub
     *
     *  @param criteria: LS criteria format (see localSearch)
     *  @return string - transport token
     */
    function globalSearch($criteria)
    {
        require_once"Transport.php";
        $tr =& new Transport($this);
        return $tr->globalSearch($criteria);
    }
    
    /**
     *  Get results from search job on network hub
     *
     *  @param trtok: string - transport token
     *  @return : LS search result format (see localSearch)
     */
    function getSearchResults($trtok)
    {
        require_once"Transport.php";
        $tr =& new Transport($this);
        return $tr->getSearchResults($trtok);
    }
    
    /* ========================================================= info methods */
    /**
     *  List files in folder
     *
     *  @param id int, local id of folder
     *  @param sessid string, session id
     *  @return array
     */
    function listFolder($id, $sessid='')
    {
        if(($res = $this->_authorize('read', $id, $sessid)) !== TRUE)
            return $res;
        $listArr = $this->bsListFolder($id);
        return $listArr;
    }

    /**
     *  Get type of stored file (by local id)
     *
     *  @param id int, local id
     *  @return string/err
     */
    function getFileType($id)
    {
        // $id = $this->_idFromGunid($gunid);
        $type = $this->getObjType($id);
        return $type;
    }

    /**
     *  Check if file gunid exists in the storage and
     *  user have permission to read it
     *
     *  @param sessid string, session id
     *  @param gunid string
     *  @param ftype string, internal file type, optional
     *  @return string/err
     */
    function existsFile($sessid, $gunid, $ftype=NULL)
    {
        $id = $this->_idFromGunid($gunid);
        $ex = $this->bsExistsFile($id, $ftype);
        if(($res = $this->_authorize('read', $id, $sessid)) !== TRUE)
            return $res;
        return $ex;
    }

    /* ==================================================== redefined methods */
    /**
     *  Get file's path in virtual filesystem
     *
     *  @param id int
     *  @return array
     */
    function getPath($id)
    {
        $pa =  parent::getPath($id, 'id, name, type');
        array_shift($pa);
        return $pa;
    }

    /**
     *   Get user id from session id
     *
     *	 This redefinition only simulate old (bad) behaviour - returns NULL
     *	 for wrong sessid (code ALIBERR_NOTEXISTS).
     *   HtmlUI depends on it.
     *
     *   @param sessid string
     *   @return int/error
     */
    function getSessUserId($sessid)
    {
        $r = parent::getSessUserId($sessid);
        if(PEAR::isError($r)){
            if($r->getCode()==ALIBERR_NOTEXISTS) return NULL;
            else return $r;
        }
        return $r;
    }

    /**
     *   Change user password.
     *
     *   ('superuser mode'= superuser is changing some password without
     *    knowledge of the old password)
     *
     *   @param login string
     *   @param oldpass string, old password
     *      (should be null or empty for 'superuser mode')
     *   @param pass string, optional
     *   @param sessid string, session id, required for 'superuser mode'
     *   @return boolean/err
     */
    function passwd($login, $oldpass=null, $pass='', $sessid='')
    {
        if(is_null($oldpass) || $oldpass == ''){
            if(($res = $this->_authorize('subjects', $this->rootId, $sessid))
                !== TRUE
            ){ sleep(2); return $res; }
            else $oldpass=null;
        }else{
            if(FALSE === $this->authenticate($login, $oldpass)){
                sleep(2);
                return PEAR::raiseError(
                    "GreenBox::passwd: access denied (oldpass)", GBERR_DENY);
            }
        }
        $res = parent::passwd($login, $oldpass, $pass);
        if(PEAR::isError($res)) return $res;
        return TRUE;
    }

    /**
     *   Insert permission record
     *
     *   @param sid int - local user/group id
     *   @param action string
     *   @param oid int - local object id
     *   @param type char - 'A'|'D' (allow/deny)
     *   @param sessid string, session id
     *   @return int - local permission id
     */
    function addPerm($sid, $action, $oid, $type='A', $sessid='')
    {
        $parid = $this->getParent($oid);
        if(($res = $this->_authorize('editPerms', $parid, $sessid)) !== TRUE){
            return $res;
        }
        return parent::addPerm($sid, $action, $oid, $type);
    }
    
    /**
     *   Remove permission record
     *
     *   @param permid int OPT - local permission id
     *   @param subj int OPT - local user/group id
     *   @param obj int OPT - local object id
     *   @param sessid string, session id
     *   @return boolean/error
     */
    function removePerm($permid=NULL, $subj=NULL, $obj=NULL, $sessid='')
    {
        if(!is_null($permid)){
            $oid = $this->_getPermOid($permid);
            if(PEAR::isError($oid)) return $oid;
            if(!is_null($oid)){
                $parid = $this->getParent($oid);
                if(($res = $this->_authorize('editPerms', $parid, $sessid)) !== TRUE)
                    return $res;
            }
        }
        $res = parent::removePerm($permid, $subj, $obj);
        return $res;
    }
    
}
?>