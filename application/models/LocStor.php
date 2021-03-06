<?php
if (isset($WHITE_SCREEN_OF_DEATH) && $WHITE_SCREEN_OF_DEATH) {
    echo __FILE__.':line '.__LINE__.": LocStor loading<br>";
}
require_once("BasicStor.php");
if (isset($WHITE_SCREEN_OF_DEATH) && $WHITE_SCREEN_OF_DEATH) {
    echo __FILE__.':line '.__LINE__.": Loaded BasicStor<br>";
}
require_once("Transport.php");
if (isset($WHITE_SCREEN_OF_DEATH) && $WHITE_SCREEN_OF_DEATH) {
    echo __FILE__.':line '.__LINE__.": Loaded Transport<br>";
}

/**
 * LocStor class
 *
 * Local storage interface
 *
 * @package Airtime
 * @subpackage StorageServer
 * @copyright 2010 Sourcefabric O.P.S.
 * @license http://www.gnu.org/licenses/gpl.txt
 */
class LocStor extends BasicStor {

    /* ---------------------------------------------------------------- store */

    /**
     * Store or replace existing audio clip.
     *
     * Sending a file to the storage server is a 3 step process:
     * 1) Call storeAudioClipOpen
     * 2) Upload the file to the URL specified
     * 3) Call storeAudioClipClose
     *
     * @param string $sessid
     * 		session id
     * @param string $gunid
     * 		global unique id
     * @param string $metadata
     * 		metadata XML string
     * @param string $fname
     * 		human readable menmonic file name
     *      with extension corresponding to filetype
     * @param string $chsum
     * 		md5 checksum of media file
     * @param string $ftype
     * 		audioclip | playlist | webstream
     * @return array
     * 		{url:writable URL for HTTP PUT, token:access token}
     */
    protected function storeAudioClipOpen($sessid, $gunid, $metadata,
    $fname, $chsum, $ftype='audioclip')
    {
        // Check the gunid format
        if (!BasicStor::CheckGunid($gunid)) {
            return PEAR::raiseError(
                "LocStor::storeAudioClipOpen: Wrong gunid ($gunid)"
            );
        }

        // Check if we already have this file.
        $duplicate = StoredFile::RecallByMd5($chsum);
        if (!empty($chsum) && $duplicate) {
            return PEAR::raiseError(
                "LocStor::storeAudioClipOpen: Duplicate file"
                ." - Matched MD5 ($chsum) against '".$duplicate->getName()."'",
                888);
        }

        // Check if specified gunid exists.
        $storedFile =& StoredFile::RecallByGunid($gunid);
        if (!is_null($storedFile) && !PEAR::isError($storedFile)) {
            // gunid exists - do replace
            $oid = $storedFile->getId();
            if (($res = BasicStor::Authorize('write', $oid, $sessid)) !== TRUE) {
                return $res;
            }
            if ($storedFile->isAccessed()) {
                return PEAR::raiseError(
                    'LocStor::storeAudioClipOpen: is accessed'
                    );
            }
            $res = $storedFile->replace($oid, $storedFile->getName(), '', $metadata, 'string');
            if (PEAR::isError($res)) {
                return $res;
            }
        } else {
            // gunid doesn't exist - do insert:
            $tmpFname = uniqid();
            if (($res = BasicStor::Authorize('write', null, $sessid)) !== TRUE) {
                return $res;
            }
            $values = array(
                "metadata" => $metadata,
                "gunid" => $gunid,
                "filetype" => $ftype);
            $storedFile =& StoredFile::Insert($values);
            if (PEAR::isError($storedFile)) {
                return $storedFile;
            }
            if (PEAR::isError($res)) {
                return $res;
            }
        }
        $res = $storedFile->setState('incomplete');
        if (PEAR::isError($res)) {
            return $res;
        }
        if ($fname == '') {
            $fname = "newFile";
        }
        $storedFile->setName($fname);
        return $this->bsOpenPut($chsum, $storedFile->gunid);
    }


    /**
     * Store or replace existing audio clip
     *
     * @param string $sessid
     * @param string $token
     * @return string gunid|PEAR_Error
     */
    protected function storeAudioClipClose($sessid, $token)
    {
        $storedFile =& StoredFile::RecallByToken($token);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        $arr = $this->bsClosePut($token);
        if (PEAR::isError($arr)) {
            $storedFile->delete();
            return $arr;
        }
        $fname = $arr['fname'];
        $res = $storedFile->setRawMediaData($fname);
        if (PEAR::isError($res)) {
            return $res;
        }
        if (file_exists($fname)) {
            @unlink($fname);
        }
        $res = $storedFile->setState('ready');
        if (PEAR::isError($res)) {
            return $res;
        }
        return $storedFile->gunid;
    }


    /**
     * Check uploaded file
     *
     * @param string $token
     * 		"put" token
     * @return array
     * 		hash, (status: boolean, size: int - filesize)
     */
    protected function uploadCheck($token)
    {
        return $this->bsCheckPut($token);
    }


    /**
     * Store webstream
     *
     * @param string $sessid
     * 		session id
     * @param string $gunid
     * 		global unique id
     * @param string $metadata
     * 		metadata XML string
     * @param string $fname
     * 		human readable menmonic file name with extension corresponding to filetype
     * @param string $url
     * 		webstream url
     * @return string
     * 		gunid
     */
    protected function storeWebstream($sessid, $gunid, $metadata, $fname, $url)
    {
        $a = $this->storeAudioClipOpen(
        $sessid, $gunid, $metadata, $fname, md5(''), 'webstream');
        if (PEAR::isError($a)) {
            return $a;
        }
        $gunid = $this->storeAudioClipClose($sessid, $a['token']);
        if (PEAR::isError($gunid)) {
            return $gunid;
        }
        $storedFile =& StoredFile::RecallByGunid($gunid);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        $r = $storedFile->setMetadataValue('ls:url', $url);
        if (PEAR::isError($r)) {
            return $r;
        }
        return $gunid;
    }


    /* --------------------------------------------------------------- access */
    /**
     * Make access to audio clip
     *
     * @param string $sessid
     * @param string $gunid
     * @param int $parent
     * 		parent token
     * @return array
     * 		with: seekable filehandle, access token
     */
    public function accessRawAudioData($sessid, $gunid, $parent='0')
    {
        $storedFile =& StoredFile::RecallByGunid($gunid);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        if (($res = BasicStor::Authorize('read', $storedFile->getId(), $sessid)) !== TRUE) {
            return $res;
        }
        return $storedFile->accessRawMediaData($parent);
    }


    /**
     * Release access to audio clip
     *
     * @param string $sessid
     * @param string $token
     * 		access token
     * @return boolean|PEAR_Error
     */
    public function releaseRawAudioData($sessid, $token)
    {
        $storedFile =& StoredFile::RecallByToken($token);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        return $storedFile->releaseRawMediaData($token);
    }


    /* ------------------------------------------------------------- download */
    /**
     * Create and return downloadable URL for audio file
     *
     * @param string $sessid
     * 		session id
     * @param string $gunid
     * 		global unique id
     * @return array
     * 		array with strings:
     *      downloadable URL, download token, chsum, size, filename
     */
    protected function downloadRawAudioDataOpen($sessid, $gunid)
    {
        $ex = $this->existsAudioClip($sessid, $gunid);
        if (PEAR::isError($ex)) {
            return $ex;
        }
        $media = StoredFile::RecallByGunid($gunid);
        $id = $media->getId();
        if (is_null($id) || !$ex) {
            return PEAR::raiseError(
                "LocStor::downloadRawAudioDataOpen: gunid not found ($gunid)",
            GBERR_NOTF
            );
        }
        if (($res = BasicStor::Authorize('read', $id, $sessid)) !== TRUE) {
            return $res;
        }
        return $this->bsOpenDownload($id);
    }


    /**
     * Discard downloadable URL for audio file
     *
     * @param string $token
     * 		download token
     * @return string
     * 		gunid
     */
    protected function downloadRawAudioDataClose($token)
    {
        return $this->bsCloseDownload($token);
    }


    /**
     * Create and return downloadable URL for metadata
     *
     * @param string $sessid
     * 		session id
     * @param string $gunid
     * 		global unique id
     * @return array
     * 		array with strings:
     *      downloadable URL, download token, chsum, filename
     */
    protected function downloadMetadataOpen($sessid, $gunid)
    {
        //        $res = $this->existsAudioClip($sessid, $gunid);
        //        if(PEAR::isError($res)) return $res;
        $media = StoredFile::RecallByGunid($gunid);
        $id = $media->getGunid();
        if (is_null($id)) {
            return PEAR::raiseError(
             "LocStor::downloadMetadataOpen: gunid not found ($gunid)"
            );
        }
        if (($res = BasicStor::Authorize('read', $id, $sessid)) !== TRUE) {
            return $res;
        }
        $res = $this->bsOpenDownload($id, 'metadata');
        #unset($res['filename']);
        return $res;
    }


    /**
     * Discard downloadable URL for metadata
     *
     * @param string $token
     * 		download token
     * @return string
     * 		gunid
     */
    protected function downloadMetadataClose($token)
    {
        return $this->bsCloseDownload($token, 'metadata');
    }


    /**
     * Return metadata as XML
     *
     * @param string $sessid
     * @param string $gunid
     * @return string|PEAR_Error
     */
    protected function getAudioClip($sessid, $gunid)
    {
        $storedFile =& StoredFile::RecallByGunid($gunid);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        if (($res = BasicStor::Authorize('read', $storedFile->getId(), $sessid)) !== TRUE) {
            return $res;
        }
        $md = $storedFile->getMetadata();
        if (PEAR::isError($md)) {
            return $md;
        }
        return $md;
    }


    /* ------------------------------------------------------- search, browse */

    /**
     * Search in metadata database
     *
     * @param string $sessid
     * @param array $criteria
     * 	with following structure:<br>
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
     *          or array of strings for multicolumn orderby
     *          [default: dc:creator, dc:source, dc:title]
     *     </li>
     *     <li>desc : boolean - flag for descending order (optional)
     *          or array of boolean for multicolumn orderby
     *          (it corresponds to elements of orderby field)
     *          [default: all ascending]
     *     </li>
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
     *           <li>source: string - dc:source from metadata</li>
     *           <li>length: string - dcterms:extent in extent format</li>
     *          </ul>
     *      </li>
     *   </ul>
     *  @see BasicStor::localSearch
     */
    public function searchMetadata($sessid, $criteria)
    {
        if (($res = BasicStor::Authorize('read', $this->storId, $sessid)) !== TRUE) {
            return $res;
        }
        $criteria['resultMode'] = 'xmlrpc';
        $res = $this->localSearch($criteria, $sessid);
        return $res;
    }


    /**
     * @param array $criteria
     * @param mixed $sessid
     *      This variable isnt used.
     * @return unknown
     */
    public function localSearch($criteria, $sessid='')
    {
        $limit = intval(isset($criteria['limit']) ? $criteria['limit'] : 0);
        $offset = intval(isset($criteria['offset']) ? $criteria['offset'] : 0);
        $res = $this->bsLocalSearch($criteria, $limit, $offset);
        return $res;
    }


    /**
     * Return values of specified metadata category
     *
     * @param string $category
     * 		metadata category name
     *      with or without namespace prefix (dc:title, author)
     * @param hash $criteria
     * 		see searchMetadata method
     * @param string $sessid
     * @return array
     * 		hash, fields:
     *       results : array with found values
     *       cnt : integer - number of matching values
     *  @see BasicStor::bsBrowseCategory
     */
    protected function browseCategory($category, $criteria=NULL, $sessid='')
    {
        $limit = intval(isset($criteria['limit']) ? $criteria['limit'] : 0);
        $offset = intval(isset($criteria['offset']) ? $criteria['offset'] : 0);
        $res = $this->bsBrowseCategory($category, $limit, $offset, $criteria);
        return $res;
    }


    /* ----------------------------------------------------------------- etc. */
    /**
     * Check if audio clip exists
     *
     * @param string $sessid
     * @param string $gunid
     * @return boolean
     */
    protected function existsAudioClip($sessid, $gunid)
    {
        $ex = $this->existsFile($sessid, $gunid, 'audioclip');
        // webstreams are subset of audioclips - moved to BasicStor
        // if($ex === FALSE ){
        //    $ex = $this->existsFile($sessid, $gunid, 'webstream');
        // }
        if ($ex === FALSE ) {
            return FALSE;
        }
        if (PEAR::isError($ex)) {
            return $ex;
        }
        $storedFile =& StoredFile::RecallByGunid($gunid);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        return $storedFile->exists();
    }


    /**
     * Check if file exists in the storage
     *
     * @param string $sessid
     * @param string $gunid
     * @param string $ftype
     * 		internal file type
     * @return boolean
     */
    protected function existsFile($sessid, $gunid, $ftype=NULL)
    {
        if (($res = BasicStor::Authorize('read', $id, $sessid)) !== TRUE) {
            return $res;
        }
        $f = StoredFile::RecallByGunid($gunid);
        if (PEAR::isError($f)) {
            return FALSE;
        }
        return $f->existsFile();
    }


    /**
     * Delete existing audio clip
     *
     * @param string $sessid
     * @param string $gunid
     * @param boolean $forced
     * 		if true, don't use trash
     * @return boolean|PEAR_Error
     */
    protected function deleteAudioClip($sessid, $gunid, $forced=FALSE)
    {
        $storedFile =& StoredFile::RecallByGunid($gunid);
        if (is_null($storedFile)) {
            return TRUE;
        }
        if (PEAR::isError($storedFile)) {
            if ($storedFile->getCode()==GBERR_FOBJNEX && $forced) {
                return TRUE;
            }
            return $storedFile;
        }
        if (($res = BasicStor::Authorize('write', $storedFile->getId(), $sessid)) !== TRUE) {
            return $res;
        }
        $res = $storedFile->delete();
        if (PEAR::isError($res)) {
            return $res;
        }
        return TRUE;
    }


    /**
     * Update existing audio clip metadata
     *
     * @param string $sessid
     * @param string $gunid
     * @param string $metadata
     * 		metadata XML string
     * @return boolean|PEAR_Error
     */
    protected function updateAudioClipMetadata($sessid, $gunid, $metadata)
    {
        $storedFile =& StoredFile::RecallByGunid($gunid);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        if (($res = BasicStor::Authorize('write', $storedFile->getId(), $sessid)) !== TRUE) {
            return $res;
        }
        return $storedFile->setMetadata($metadata, 'string');
    }


    /*====================================================== playlist methods */
    /**
     * Create a new empty playlist.
     *
     * @param string $sessid
     * 		session ID
     * @param string $playlistId
     * 		playlist global unique ID
     * @param string $fname
     * 		human readable mnemonic file name
     * @return string
     * 		playlist global unique ID
     */
    public function createPlaylist($sessid, $playlistId, $fname)
    {
        $ex = $this->existsPlaylist($sessid, $playlistId);
        if (PEAR::isError($ex)) {
            return $ex;
        }
        if ($ex) {
            return PEAR::raiseError(
                'LocStor::createPlaylist: already exists'
                );
        }
        $tmpFname = uniqid('');
        if (($res = BasicStor::Authorize('write', null, $sessid)) !== TRUE) {
            return $res;
        }
        $values = array(
            "metadata" => dirname(__FILE__).'/emptyPlaylist.xml',
            "gunid" => $playlistId,
            "filetype" => "playlist");
        // This is all wrong now.
        $storedFile = StoredFile::Insert($values);
        if ($fname == '') {
            $fname = "newFile.xml";
        }
        $storedFile->setName($fname);
        $storedFile->setState('ready');
        $storedFile->setMime('application/smil');
        return $storedFile->gunid;
    }


    /**
     * Open a Playlist metafile for editing.
     * Open readable URL and mark file as beeing edited.
     *
     * @param string $sessid
     * 		session ID
     * @param string $playlistId
     * 		playlist global unique ID
     * @return struct
     *      {url:readable URL for HTTP GET, token:access token, chsum:checksum}
     */
    public function editPlaylist($sessid, $playlistId)
    {
        $ex = $this->existsPlaylist($sessid, $playlistId);
        if (PEAR::isError($ex)) {
            return $ex;
        }
        if (!$ex) {
            return PEAR::raiseError(
                'LocStor::editPlaylist: playlist not exists'
                );
        }
        if ($this->isEdited($playlistId) !== FALSE) {
            return PEAR::raiseError(
                'LocStor::editPlaylist: playlist already edited'
                );
        }
        $storedFile =& StoredFile::RecallByGunid($playlistId);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        $id = $storedFile->getId();
        if (($res = BasicStor::Authorize('write', $id, $sessid)) !== TRUE) {
            return $res;
        }
        $res = $this->bsOpenDownload($id, 'metadata');
        if (PEAR::isError($res)) {
            return $res;
        }
        $r = $this->setEditFlag($playlistId, TRUE, $sessid);
        if (PEAR::isError($r)) {
            return $r;
        }
        unset($res['filename']);
        return $res;
    }


    /**
     * Store a new Playlist metafile in place of the old one.
     *
     * @param string $sessid
     * 		session ID
     * @param string $playlistToken
     * 		playlist access token
     * @param string $newPlaylist
     * 		new playlist as XML string
     * @return string
     * 		playlistId
     */
    protected function savePlaylist($sessid, $playlistToken, $newPlaylist)
    {
        $playlistId = $this->bsCloseDownload($playlistToken, 'metadata');
        if (PEAR::isError($playlistId)) {
            return $playlistId;
        }
        $storedFile =& StoredFile::RecallByGunid($playlistId);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        $res = $storedFile->setMetadata($newPlaylist, 'string', 'playlist');
        if (PEAR::isError($res)) {
            return $res;
        }
        $r = $this->setEditFlag($playlistId, FALSE, $sessid);
        if (PEAR::isError($r)) {
            return $r;
        }
        return $playlistId;
    }


    /**
     * RollBack playlist changes to the locked state
     *
     * @param string $playlistToken
     * 		playlist access token
     * @param string $sessid
     * 		session ID
     * @return string
     * 		gunid of playlist
     */
    public function revertEditedPlaylist($playlistToken, $sessid='')
    {
        $gunid = $this->bsCloseDownload($playlistToken, 'metadata');
        if (PEAR::isError($gunid)) {
            return $gunid;
        }
        $storedFile =& StoredFile::RecallByGunid($gunid);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        $id = $storedFile->getId();
        $mdata = $storedFile->getMetadata();
        if (PEAR::isError($mdata)) {
            return $mdata;
        }
        $res = $storedFile->setMetadata($mdata, 'string');
        if (PEAR::isError($res)) {
            return $res;
        }
        $this->setEditFlag($gunid, FALSE, $sessid);
        return $gunid;
    }


    /**
     * Delete a Playlist metafile.
     *
     * @param string $sessid
     * 		session ID
     * @param string $playlistId
     * 		playlist global unique ID
     * @param boolean $forced
     * 		if true don't use trash
     * @return boolean
     */
    public function deletePlaylist($sessid, $playlistId, $forced=FALSE)
    {
        $ex = $this->existsPlaylist($sessid, $playlistId);
        if (PEAR::isError($ex)) {
            return $ex;
        }
        if (!$ex) {
            if ($forced) {
                return TRUE;
            }
            return PEAR::raiseError(
                'LocStor::deletePlaylist: playlist not exists',
            GBERR_FILENEX
            );
        }
        $storedFile =& StoredFile::RecallByGunid($playlistId);
        if (is_null($storedFile) || PEAR::isError($storedFile)) {
            return $storedFile;
        }
        if (($res = BasicStor::Authorize('write', $storedFile->getId(), $sessid)) !== TRUE) {
            return $res;
        }
        $res = $storedFile->delete();
        if (PEAR::isError($res)) {
            return $res;
        }
        return TRUE;
    }


    /**
     * Access (read) a Playlist metafile.
     *
     * @param string $sessid
     * 		session ID
     * @param string $playlistId
     * 		playlist global unique ID
     * @param boolean $recursive
     * 		flag for recursive access content inside playlist
     * @param int $parent
     * 		parent token
     * @return struct {
     *      url: readable URL for HTTP GET,
     *      token: access token,
     *      chsum: checksum,
     *      content: array of structs - recursive access (optional)
     *      filename: string mnemonic filename
     *  }
     */
    public function accessPlaylist($sessid, $playlistId, $recursive=FALSE, $parent='0')
    //    {
    //        if ($recursive) {
    //            require_once("AccessRecur.php");
    //            $r = AccessRecur::accessPlaylist($this, $sessid, $playlistId);
    //            if (PEAR::isError($r)) {
    //                return $r;
    //            }
    //            return $r;
    //        }
    //        $ex = $this->existsPlaylist($sessid, $playlistId);
    //        if (PEAR::isError($ex)) {
    //            return $ex;
    //        }
    //        if (!$ex) {
    //            return PEAR::raiseError(
    //                "LocStor::accessPlaylist: playlist not found ($playlistId)",
    //                GBERR_NOTF
    //            );
    //        }
    //        $id = BasicStor::IdFromGunid($playlistId);
    //        if (($res = BasicStor::Authorize('read', $id, $sessid)) !== TRUE) {
    //            return $res;
    //        }
    //        $res = $this->bsOpenDownload($id, 'metadata', $parent);
    //        #unset($res['filename']);
    //        return $res;
    }


    /**
     * Release the resources obtained earlier by accessPlaylist().
     *
     * @param string $sessid
     * 		session ID
     * @param string $playlistToken
     * 		playlist access token
     * @param boolean $recursive
     * 		flag for recursive access content inside playlist
     * @return string
     * 		playlist ID
     */
    public function releasePlaylist($sessid, $playlistToken, $recursive=FALSE)
    {
        if ($recursive) {
            require_once"AccessRecur.php";
            $r = AccessRecur::releasePlaylist($this, $sessid, $playlistToken);
            if (PEAR::isError($r)) {
                return $r;
            }
            return $r;
        }
        return $this->bsCloseDownload($playlistToken, 'metadata');
    }


    /**
     * Create a tarfile with playlist export - playlist and all matching
     * sub-playlists and media files (if desired)
     *
     * @param string $sessid
     * 		session ID
     * @param array $plids
     * 		array of strings, playlist global unique IDs (one gunid is accepted too)
     * @param string $type
     * 		playlist format, values: lspl | smil | m3u
     * @param boolean $standalone
     * 		if only playlist should be exported or with all related files
     * @return hasharray with  fields:
     *      url string: readable url,
     *      token string: access token
     *      chsum string: md5 checksum,
     */
    protected function exportPlaylistOpen($sessid, $plids, $type='lspl', $standalone=FALSE)
    {
        $res = $this->bsExportPlaylistOpen($plids, $type, !$standalone);
        if (PEAR::isError($res)) {
            return $res;
        }
        $url = BasicStor::GetUrlPart()."access/".basename($res['fname']);
        $chsum = md5_file($res['fname']);
        $size = filesize($res['fname']);
        return array(
            'url' => $url,
            'token' => $res['token'],
            'chsum' => $chsum,
        );
    }


    /**
     * Close playlist export previously opened by the exportPlaylistOpen method
     *
     * @param string $token
     * 		access token obtained from exportPlaylistOpen method call
     * @return boolean|PEAR_Error
     */
    protected function exportPlaylistClose($token)
    {
        return $this->bsExportPlaylistClose($token);
    }


    /**
     * Open writable handle for import playlist in LS Archive format
     *
     * @param string $sessid
     * 		session id
     * @param string $chsum
     * 		md5 checksum of imported file
     * @return hasharray with:
     *      url string: writable URL
     *      token string: PUT token
     */
    protected function importPlaylistOpen($sessid, $chsum)
    {
        $userid = Alib::GetSessUserId($sessid);
        if (PEAR::isError($userid)) {
            return $userid;
        }
        $r = $this->bsOpenPut($chsum, NULL, $userid);
        if (PEAR::isError($r)) {
            return $r;
        }
        return $r;
    }


    /**
     * Close import-handle and import playlist
     *
     * @param string $token
     * 		import token obtained by importPlaylistOpen method
     * @return string
     * 		result file global id (or error object)
     */
    protected function importPlaylistClose($token)
    {
        $arr = $this->bsClosePut($token);
        if (PEAR::isError($arr)) {
            return $arr;
        }
        $fname = $arr['fname'];
        $owner = $arr['owner'];
        $res = $this->bsImportPlaylist($fname);
        if (file_exists($fname)) {
            @unlink($fname);
        }
        if (PEAR::isError($res)) {
            return $res;
        }
        $media = StoredFile::Recall($id);
        return $media->getGunId();
    }


    /**
     * Check whether a Playlist metafile with the given playlist ID exists.
     *
     * @param string $sessid
     * 		session ID
     * @param string $playlistId
     * 		playlist global unique ID
     * @return boolean
     */
    public function existsPlaylist($sessid, $playlistId)
    {
        return $this->existsFile($sessid, $playlistId, 'playlist');
    }


    /**
     * Check whether a Playlist metafile with the given playlist ID
     * is available for editing, i.e., exists and is not marked as
     * being edited.
     *
     * @param string $sessid
     * 		session ID
     * @param string $playlistId
     * 		playlist global unique ID
     * @param boolean $getUid
     * 		flag for returning editedby uid
     * @return boolean
     */
    public function playlistIsAvailable($sessid, $playlistId, $getUid=FALSE)
    {
        $ex = $this->existsPlaylist($sessid, $playlistId);
        if (PEAR::isError($ex)) {
            return $ex;
        }
        if (!$ex) {
            return PEAR::raiseError(
                'LocStor::playlistIsAvailable: playlist not exists'
                );
        }
        $ie = $this->isEdited($playlistId);
        if ($ie === FALSE) {
            return TRUE;
        }
        if ($getUid) {
            return $ie;
        }
        return FALSE;
    }


    /* ------------------------------------------------------- render methods */
    /**
     * Render playlist to ogg file (open handle)
     *
     * @param string $sessid
     * 		session id
     * @param string $plid
     * 		 playlist gunid
     * @return hasharray
     *      token: string - render token
     */
    protected function renderPlaylistToFileOpen($sessid, $plid)
    {
        require_once("Renderer.php");
        $r = Renderer::rnRender2FileOpen($this, $plid);
        if (PEAR::isError($r)) {
            return $r;
        }
        return $r;
    }


    /**
     * Render playlist to ogg file (check results)
     *
     * @param string $token
     * 		render token
     * @return hasharray:
     *      status : string - success | working | fault
     *      url : string - readable url
     */
    protected function renderPlaylistToFileCheck($token)
    {
        require_once("Renderer.php");
        $r = Renderer::rnRender2FileCheck($this, $token);
        if (PEAR::isError($r)) {
            return $r;
        }
        return array('status'=>$r['status'], 'url'=>$r['url']);
    }


    /**
     * Render playlist to ogg file (close handle)
     *
     * @param string $token
     * 		render token
     * @return boolean status
     */
    protected function renderPlaylistToFileClose($token)
    {
        require_once("Renderer.php");
        $r = Renderer::rnRender2FileClose($this, $token);
        if (PEAR::isError($r)) {
            return $r;
        }
        return array(TRUE);
    }


    /**
     * Render playlist to storage media clip (open handle)
     *
     * @param string $sessid
     * 		session id
     * @param string $plid
     * 		playlist gunid
     * @return string
     * 		render token
     */
    protected function renderPlaylistToStorageOpen($sessid, $plid)
    {
        require_once("Renderer.php");
        $owner = Alib::GetSessUserId($sessid);
        if (PEAR::isError($owner)) {
            return $owner;
        }
        $r = Renderer::rnRender2FileOpen($this, $plid, $owner);
        if (PEAR::isError($r)) {
            return $r;
        }
        return $r;
    }


    /**
     * Render playlist to storage media clip (check results)
     *
     * @param string $token
     * 		render token
     * @return hasharray:
     *      status : string - success | working | fault
     *      gunid  : string - gunid of result file
     */
    protected function renderPlaylistToStorageCheck($token)
    {
        require_once("Renderer.php");
        $r = Renderer::rnRender2StorageCheck($this, $token);
        if (PEAR::isError($r)) {
            return $r;
        }
        return $r;
    }


    /**
     * Render playlist to RSS file (open handle)
     *
     * @param string $sessid
     * 		session id
     * @param string $plid
     * 		 playlist gunid
     * @return string
     * 		render token
     */
    protected function renderPlaylistToRSSOpen($sessid, $plid)
    {
        global $CC_CONFIG;
        $token = '123456789abcdeff';
        $fakeFile = $CC_CONFIG['accessDir']."/$token.rss";
        file_put_contents($fakeFile, "fake rendered file");
        return array('token'=>$token);
    }


    /**
     * Render playlist to RSS file (check results)
     *
     * @param string $token
     * 		render token
     * @return hasharray :
     *      status : string - success | working | fault
     *      url    : string - readable url
     */
    protected function renderPlaylistToRSSCheck($token)
    {
        $fakeFile = $CC_CONFIG['accessDir']."/$token.rss";
        if ($token != '123456789abcdeff' || !file_exists($fakeFile)) {
            return PEAR::raiseError(
                "LocStor::renderPlaylistToRSSCheck: invalid token ($token)"
            );
        }
        $fakeFUrl = BasicStor::GetUrlPart()."access/$token.rss";
        return array(
            'status'=> 'success',
            'url'   => $fakeFUrl,
        );
    }


    /**
     * Render playlist to RSS file (close handle)
     *
     * @param string $token
     * 		render token
     * @return boolean
     * 		status
     */
    protected function renderPlaylistToRSSClose($token)
    {
        if ($token != '123456789abcdeff') {
            return PEAR::raiseError(
                "LocStor::renderPlaylistToRSSClose: invalid token"
                );
        }
        $fakeFile = $CC_CONFIG['accessDir']."/$token.rss";
        unlink($fakeFile);
        return TRUE;
    }


    /*================================================= storage admin methods */

    /* ------------------------------------------------------- backup methods */

    /**
     * Create backup of storage (open handle)
     *
     * @param string $sessid
     * 		session id
     * @param array $criteria
     * 		see search criteria
     * @return array
     *           token  : string - backup token
     */
    protected function createBackupOpen($sessid, $criteria='')
    {
        require_once("Backup.php");
        $bu = new Backup($this);
        if (PEAR::isError($bu)) {
            return $bu;
        }
        $r = $bu->openBackup($sessid,$criteria);
        if ($r === FALSE) {
            return PEAR::raiseError(
                "LocStor::createBackupOpen: false returned from Backup"
                );
        }
        return $r;
    }


    /**
     * Create backup of storage (check results)
     *
     * @param string $token
     * 		backup token
     * @return hasharray
     * 		with field:
     *      status : string - susccess | working | fault
     *      faultString: string - description of fault
     *      token  : stirng - backup token
     *      url    : string - access url
     */
    protected function createBackupCheck($token)
    {
        require_once("Backup.php");
        $bu = new Backup($this);
        if (PEAR::isError($bu)) {
            return $bu;
        }
        return $bu->checkBackup($token);
    }


    /**
     * Create backup of storage (list results)
     *
     * @param string $sessid
     * 		session id
     * @param status $stat
     *      if this parameter is not set, then return with all unclosed backups
     * @return array
     * 		array of hasharray with field:
     *      status : string - susccess | working | fault
     *      token  : stirng - backup token
     *      url    : string - access url
     */
    protected function createBackupList($sessid, $stat='')
    {
        require_once("Backup.php");
        $bu = new Backup($this);
        if (PEAR::isError($bu)) {
            return $bu;
        }
        return $bu->listBackups($stat);
    }


    /**
     * Create backup of storage (close handle)
     *
     * @param string $token
     * 		backup token
     * @return boolean
     * 		status
     */
    protected function createBackupClose($token)
    {
        require_once("Backup.php");
        $bu = new Backup($this);
        if (PEAR::isError($bu)) {
            return $bu;
        }
        return $bu->closeBackup($token);
    }


    /* ------------------------------------------------------ restore methods */

    /**
     * Restore a backup file (open handle)
     *
     * @param string $sessid
     * 		session id
     * @param string $chsum
     * 		md5 checksum of imported file
     * @return array
     * 		array with:
     *      url string: writable URL
     *      fname string: writable local filename
     *      token string: PUT token
     */
    protected function restoreBackupOpen($sessid, $chsum)
    {
        $userid = Alib::getSessUserId($sessid);
        if (PEAR::isError($userid)) {
            return $userid;
        }
        $r = $this->bsOpenPut($chsum, NULL, $userid);
        if (PEAR::isError($r)) {
            return $r;
        }
        return $r;
    }


    /**
     * Restore a backup file (close put handle)
     *
     * @param string $sessid
     * 		session id
     * @param string $token
     * 		"put" token
     * @return string $token
     * 		restore token
     */
    protected function restoreBackupClosePut($sessid, $token) {
        $arr = $this->bsClosePut($token);
        if (PEAR::isError($arr)) {
            return $arr;
        }
        $fname = $arr['fname'];
        require_once('Restore.php');
        $rs = new Restore($this);
        if (PEAR::isError($rs)) {
            return $rs;
        }
        return $rs->openRestore($sessid, $fname);
    }


    /**
     * Restore a backup file (check state)
     *
     * @param string $token
     * 		restore token
     * @return array
     * 		status - fields:
     * 		token:  string - restore token
     *      status: string - working | fault | success
     *      faultString: string - description of fault
     */
    protected function restoreBackupCheck($token)
    {
        require_once('Restore.php');
        $rs = new Restore($this);
        if (PEAR::isError($rs)) {
            return $rs;
        }
        return $rs->checkRestore($token);
    }


    /**
     * Restore a backup file (close handle)
     *
     * @param string $token
     * 		restore token
     * @return array
     * 		status - fields:
     * 		token:  string - restore token
     *      status: string - working | fault | success
     */
    protected function restoreBackupClose($token) {
        require_once('Restore.php');
        $rs = new Restore($this);
        if (PEAR::isError($rs)) {
            return $rs;
        }
        return $rs->closeRestore($token);
    }


    /*===================================================== auxiliary methods */
    /**
     * Dummy method - only returns Airtime version
     *
     * @return string
     */
    public static function getVersion()
    {
        return CAMPCASTER_VERSION;
    }

    /**
     * Open upload transport (from station to hub)
     *
     * @param string $sessid
     * 		session id
     * @param string $chsum
     * 		checksum
     * @return array
     * 		hasharray with:
     *      url string: writable URL
     *      token string: PUT token
     */
    function uploadOpen($sessid, $chsum)
    {
        $owner = Alib::GetSessUserId($sessid);
        if (PEAR::isError($owner)) {
            return $owner;
        }
        $res = $this->bsOpenPut($chsum, NULL, $owner);
        if (PEAR::isError($res)) {
            return $res;
        }
        return array('url'=>$res['url'], 'token'=>$res['token']);
    }


    /**
     * Close upload transport
     *
     * @param string $token
     * 		transport token
     * @param string $trtype
     * 		transport type
     * @param array $pars
     * 		transport parameters
     * @return mixed
     */
    function uploadClose($token, $trtype, $pars=array())
    {
        $res = $this->bsClosePut($token);
        if (PEAR::isError($res)) {
            return $res;
        }
        extract($res);  // fname, owner
        switch ($trtype) {
            case "audioclip":
                $mdtoken = $pars['mdpdtoken'];
                $res = $this->bsClosePut($mdtoken);
                if (PEAR::isError($res)) {
                    return $res;
                }
                $mdfname = $res['fname'];
                if ($gunid == '') {
                    $gunid = NULL;
                }
                $values = array(
                    "filename" => $pars['name'],
                    "filepath" => $fname,
                    "metadata" => $mdfname,
                    "gunid" => $pars['gunid'],
                    "filetype" => "audioclip"
                    );
                    $storedFile = StoredFile::Insert($values);
                    if (PEAR::isError($storedFile)) {
                        return $storedFile;
                    }
                    $res = $storedFile->getId();
                    @unlink($fname);
                    @unlink($mdfname);
                    break;
            case "playlist":
                if ($gunid == '') {
                    $gunid = NULL;
                }
                $values = array(
                    "filename" => $pars['name'],
                    "metadata" => $fname,
                    "gunid" => $pars['gunid'],
                    "filetype" => "playlist"
                    );
                    $storedFile = StoredFile::Insert($values);
                    if (PEAR::isError($storedFile)) {
                        return $storedFile;
                    }
                    $res = $storedFile->getId();
                    @unlink($fname);
                    break;
            case "playlistPkg":
                $chsum = md5_file($fname);
                // importPlaylistOpen:
                $res = $this->bsOpenPut($chsum, NULL, $owner);
                if (PEAR::isError($res)) {
                    return $res;
                }
                $dest = $res['fname'];
                $token = $res['token'];
                copy($fname, $dest);
                $r = $this->importPlaylistClose($token);
                if (PEAR::isError($r)) {
                    return $r;
                }
                @unlink($fname);
                return $r;
                break;
            case "searchjob":
                $crits = file_get_contents($fname);
                $criteria = unserialize($crits);
                @unlink($fname);
                $results = $this->localSearch($criteria);
                if (PEAR::isError($results)) {
                    return $results;
                }
                $realfile = tempnam($this->accessDir, 'searchjob_');
                @chmod($realfile, 0660);
                $len = file_put_contents($realfile, serialize($results));
                $acc = BasicStor::bsAccess($realfile, '', NULL, 'download');
                if (PEAR::isError($acc)) {
                    return $acc;
                }
                $url = BasicStor::GetUrlPart()."access/".basename($acc['fname']);
                $chsum = md5_file($realfile);
                $size = filesize($realfile);
                $res = array(
                    'url'=>$url, 'token'=>$acc['token'],
                    'chsum'=>$chsum, 'size'=>$size,
                    'filename'=>$filename
                );
                return $res;
                break;
            case "metadata":
                break;
            default:
        }
        return $res;
    }


    /**
     * Open download transport
     *
     * @param string $sessid
     * 		session id
     * @param string $trtype
     * 		transport type
     * @param array $pars
     * 		transport parameters
     * @return hasharray with:
     *      url string: writable URL
     *      token string: PUT token
     */
    function downloadOpen($sessid, $trtype, $pars=array())
    {
        //        global $CC_CONFIG;
        //        switch ($trtype) {
        //            case "unknown":
        //            case "audioclip":
        //            case "metadata":
        //            case "playlist":
        //            case "playlistPkg":
        //                if (!isset($pars['gunid'])) {
        //                    return PEAR::raiseError("Archive::downloadOpen: gunid not set");
        //                }
        //                break;
        //        }
        //        $gunid = $pars['gunid'];
        //        // resolve trtype by object type:
        //        if ( ($trtype == 'unknown') || ($trtype == 'playlistPkg') ) {
        //            $media = StoredFile::RecallByGunid($gunid);
        //            $trtype2 = $media->getType();
        //            if (PEAR::isError($trtype2)) {
        //            	return $trtype2;
        //            }
        //            // required with content:
        //            $trtype = ( ($trtype2 == 'playlist') && ($trtype == 'playlistPkg') ?
        //                'playlistPkg' : $trtype2);
        //			//return PEAR::raiseError("Archive::downloadOpen: TT=$trtype TT2=$trtype2 G=$gunid");
        //        }
        //        switch ($trtype) {
        //            case "audioclip":
        //                $res = $this->downloadRawAudioDataOpen($sessid, $gunid);
        //                break;
        //            case "metadata":
        //                $res = $this->downloadMetadataOpen($sessid, $gunid);
        //                break;
        //            case "playlist":
        //                $res = $this->accessPlaylist($sessid, $gunid);
        //                break;
        //            case "playlistPkg":
        //                $res = $this->bsExportPlaylistOpen($gunid);
        //                if (PEAR::isError($res)) {
        //                	return $res;
        //                }
        //                $tmpn = tempnam($CC_CONFIG['transDir'], 'plExport_');
        //                $plfpath = "$tmpn.lspl";
        //                copy($res['fname'], $plfpath);
        //                $res = $this->bsExportPlaylistClose($res['token']);
        //                if (PEAR::isError($res)) {
        //                	return $res;
        //                }
        //                $fname = "transported_playlist.lspl";
        //                $id = BasicStor::IdFromGunid($gunid);
        //                $acc = BasicStor::bsAccess($plfpath, 'lspl', NULL, 'download');
        //                if (PEAR::isError($acc)) {
        //                	return $acc;
        //                }
        //                $url = BasicStor::GetUrlPart()."access/".basename($acc['fname']);
        //                $chsum = md5_file($plfpath);
        //                $size = filesize($plfpath);
        //                $res = array(
        //                    'url'=>$url, 'token'=>$acc['token'],
        //                    'chsum'=>$chsum, 'size'=>$size,
        //                    'filename'=>$fname
        //                );
        //                break;
        //            case "searchjob":
        //                $res = $pars;
        //                break;
        //            case "file":
        //                $res = array();
        //                break;
        //            default:
        //                return PEAR::raiseError("Archive::downloadOpen: NotImpl ($trtype)");
        //        }
        //        if (PEAR::isError($res)) {
        //        	return $res;
        //        }
        //        switch ($trtype) {
        //            case "audioclip":
        //            case "metadata":
        //            case "playlist":
        //            case "playlistPkg":
        //                $f = StoredFile::RecallByGunid($gunid);
        //                $title = $f->getTitle();
        //                break;
        //            case "searchjob":
        //            	$title = 'searchjob';
        //            	break;
        //            case "file":
        //            	$title = 'regular file';
        //            	break;
        //            default:
        //        }
        //        $res['title'] = $title;
        //        $res['trtype'] = $trtype;
        //        return $res;
        }


        /**
         * Close download transport
         *
         * @param string $token
         * 		transport token
         * @param string $trtype
         * 		transport type
         * @return array
         * 		hasharray with:
         *      url string: writable URL
         *      token string: PUT token
         */
        function downloadClose($token, $trtype)
        {
            switch ($trtype) {
                case "audioclip":
                    $res = $this->downloadRawAudioDataClose($token);
                    if (PEAR::isError($res)) {
                        return $res;
                    }
                    return $res;
                case "metadata":
                    $res = $this->downloadMetadataClose($token);
                    return $res;
                case "playlist":
                    $res = $this->releasePlaylist(NULL/*$sessid*/, $token);
                    return $res;
                case "playlistPkg":
                    $res = BasicStor::bsRelease($token, 'download');
                    if (PEAR::isError($res)) {
                        return $res;
                    }
                    $realFname = $r['realFname'];
                    @unlink($realFname);
                    if (preg_match("|(plExport_[^\.]+)\.lspl$|", $realFname, $va)) {
                        list(,$tmpn) = $va;
                        $tmpn = $CC_CONFIG['transDir']."/$tmpn";
                        if (file_exists($tmpn)) {
                            @unlink($tmpn);
                        }
                    }
                    return $res;
                case "searchjob":
                    $res = BasicStor::bsRelease($token, 'download');
                    return $res;
                case "file":
                    return array();
                default:
                    return PEAR::raiseError("Archive::downloadClose: NotImpl ($trtype)");
            }
        }


        /**
         * Prepare hub initiated transport
         *
         * @param string $target
         * 		hostname of transport target
         * @param string $trtype
         * 		transport type
         * @param string $direction
         * 		'up' | 'down'
         * @param array $pars
         * 		transport parameters
         * @return mixed
         */
        function prepareHubInitiatedTransfer(
        $target, $trtype='file', $direction='up',$pars=array())
        {
            $tr = new Transport($this);
            $trec = TransportRecord::create($tr, $trtype, $direction,
            array_merge($pars, array('target'=>$target)));
            if (PEAR::isError($trec)) {
                return $trec;
            }
            return TRUE;
        }


        /**
         * List hub initiated transports
         *
         * @param string $target
         * 		hostname of transport target
         * @param string $direction
         * 		'up' | 'down'
         * @param string $trtok
         * 		transport token
         * @return mixed
         */
        function listHubInitiatedTransfers(
        $target=NULL, $direction=NULL, $trtok=NULL)
        {
            $tr = new Transport($this);
            $res = $tr->getTransports($direction, $target, $trtok);
            return $res;
        }


        /**
         * Set state of hub initiated transport
         *
         * @param string $target
         * 		hostname of transport target
         * @param string $trtok
         * 		transport token
         * @param string $state
         * 		transport state
         * @return TransportRecord|PEAR_Error
         */
        function setHubInitiatedTransfer($target, $trtok, $state)
        {
            $tr = new Transport($this);
            $trec = TransportRecord::recall($tr, $trtok);
            if (PEAR::isError($trec)) {
                return $trec;
            }
            $r = $trec->setState($state);
            if (PEAR::isError($r)) {
                return $r;
            }
            return $trec;
        }

        /* ==================================================== auxiliary methods */

} // class LocStor
?>
