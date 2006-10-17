<?php
/**
 * Tracklist support for StorageServer
 * @see http://dev.tilos.hu/projects/ikra/ticket/144
 */
class Tracklist extends StoredFile{
	/**
     *  Create instance of Tracklist object and recall existing file
     *  by gunid.<br/>
     *
     *  @param gb reference to GreenBox object
     *  @param gunid string, global unique id
     *  @param className string, optional classname to recall
     *  @return instance of Tracklist object
     */
    function &recallByGunid(&$gb, $gunid, $className='Tracklist')
    {
        return parent::recallByGunid($gb, $gunid, $className);
    }

    /**
     *  Create instance of Tracklist object and recall existing file
     *  by access token.<br/>
     *
     *  @param gb reference to GreenBox object
     *  @param token string, access token
     *  @param className string, optional classname to recall
     *  @return instance of Tracklist object
     */
    function &recallByToken(&$gb, $token, $className='Tracklist')
    {
        return parent::recallByToken($gb, $token, $className);
    }

    /**
     *  Create instance of Tarcklist object and insert new file
     *
     *  @param gb reference to GreenBox object
     *  @param oid int, local object id in the tree
     *  @param fname string, name of new file
     *  @param mediaFileLP string, ignored
     *  @param metadata string, local path to tracklist XML file or XML string
     *  @param mdataLoc string 'file'|'string' (optional)
     *  @param plid global unique id (optional) - for insert file with gunid
     *  @param ftype string, ignored
     *  @return instance of Tracklist object
     */
    function &insert(&$gb, $oid, $fname,
        $mediaFileLP='', $metadata='', $mdataLoc='file',
        $plid=NULL, $ftype=NULL)
    {
        return parent::insert($gb, $oid, $fname,
            '', $metadata, $mdataLoc, $plid, 'tracklist', 'Tracklist');
    }
    
    /**
     *  Create instance of Tracklist object and insert empty file
     *  
     *  @param gb reference to GreenBox object
     *  @param plid global unique id
     *  @param fname string, name of new file
     *  @param parid int, local object id of parent folder
     *  @return instance of Tracklist object
     */
    function &create(&$gb, $plid, $fname=NULL, $parid=NULL)
    {
        $tmpFname = uniqid('');
        $oid = $this->addObj($tmpFname , 'tracklist', $parid);
        if(PEAR::isError($oid)) return $oid;
        $pl =&  Tracklist::insert($this, $oid, '', '',
            dirname(__FILE__).'/emptyTracklist.xml',
            'file', $plid
        );
        if(PEAR::isError($pl)){
            $res = $this->removeObj($oid);
            return $pl;
        }
        $fname = ($fname == '' || is_null($fname) ? "newFile.xml" : $fname );
        $res = $this->bsRenameFile($oid, $fname);
        if(PEAR::isError($res)) return $res;
        $res = $pl->setState('ready');
        if(PEAR::isError($res)) return $res;
        $res = $pl->setMime('application/smil');
        if(PEAR::isError($res)) return $res;
        $res = $pl->setAuxMetadata();
        if(PEAR::isError($res)) return $res;
        return $pl;
    }

    /**
     *  Lock tracklist for edit
     *
     *  @param gb reference to GreenBox object
     *  @param subjid int, local subject (user) id
     *  @param val boolean, if false do unlock
     *  @return boolean, previous state or error object
     */
    function lock(&$gb, $subjid=NULL, $val=TRUE)
    {
        if($val && $gb->_isEdited($this->gunid) !== FALSE){
            return PEAR::raiseError(
                'Tracklist::lock: tracklist already locked'
            );
        }
        $r = $gb->_setEditFlag($this->gunid, $val, NULL, $subjid);
        return $r;
    }
    
    /**
     *  Unlock tracklist (+recalculate and pregenerate XML)
     *
     *  @param gb reference to GreenBox object
     *  @return boolean, previous state or error object
     */
    function unLock(&$gb)
    {
        $r = $this->recalculateTimes();
        if(PEAR::isError($r)) return $r;
        $r = $this->md->regenerateXmlFile();
        if(PEAR::isError($r)) return $r;
        $r = $this->lock($gb, $this->gunid, NULL, FALSE);
        return $r;
    }

    
    /**
     *  Inserting of new TracklistItem
     *
     *  @param parid int - parent record id
     *  @param offset string - relative offset in extent format
     *  @param acGunid string - audioClip gunid
     *  @param acLen string - audioClip length in extent format
     *  @param acTit string - audioClip title
     *  @param fadeIn string - fadeIn value in ss.ssssss or extent format
     *  @param fadeOut string - fadeOut value in ss.ssssss or extent format
     *  @param plElGunid string - optional playlist element gunid
     *  @param elType string - optional 'audioClip' | 'playlist'
     *  @return array with fields:
     *  <ul>
     *   <li>plElId int - record id of playlistElement</li>
     *   <li>plElGunid string - gl.unique id of playlistElement</li>
     *   <li>fadeInId int - record id</li>
     *   <li>fadeOutId int - record id</li>
     *  </ul>
     * 
     */
    function insertTracklist($parid, $offset, $acGunid, $acLen, $acTit,
        $fadeIn=NULL, $fadeOut=NULL, $plElGunid=NULL, $elType='audioClip')
    {
        // insert playlistElement
        $r = $this->md->insertMetadataEl($parid, 'playlistElement');
        if(PEAR::isError($r)){ return $r; }
        $plElId = $r;
        // create and insert gunid (id attribute)
        if(is_null($plElGunid)) $plElGunid = StoredFile::_createGunid();
        $r = $this->md->insertMetadataEl($plElId, 'id', $plElGunid, 'A');
        if(PEAR::isError($r)){ return $r; }
        // insert relativeOffset
        $r = $this->md->insertMetadataEl(
            $plElId, 'relativeOffset', $offset, 'A');
        if(PEAR::isError($r)){ return $r; }
        // insert audioClip (or playlist) element into playlistElement
        $r = $this->md->insertMetadataEl($plElId, $elType);
        if(PEAR::isError($r)){ return $r; }
        $acId = $r;
        $r = $this->md->insertMetadataEl($acId, 'id', $acGunid, 'A');
        if(PEAR::isError($r)){ return $r; }
        $r = $this->md->insertMetadataEl($acId, 'playlength', $acLen, 'A');
        if(PEAR::isError($r)){ return $r; }
        $r = $this->md->insertMetadataEl($acId, 'title', $acTit, 'A');
        if(PEAR::isError($r)){ return $r; }
        $fadeInId=NULL;
        $fadeOutId=NULL;
        if(!is_null($fadeIn) || !is_null($fadeOut)){
            // insert fadeInfo element into playlistElement
            $r = $this->md->insertMetadataEl($plElId, 'fadeInfo');
            if(PEAR::isError($r)){ return $r; }
            $fiId = $r;
            $fiGunid = StoredFile::_createGunid();
            $r = $this->md->insertMetadataEl($fiId, 'id', $fiGunid, 'A');
            if(PEAR::isError($r)){ return $r; }
            $r = $this->md->insertMetadataEl($fiId, 'fadeIn', $fadeIn, 'A');
            if(PEAR::isError($r)){ return $r; }
            $fadeInId = $r;
            $r = $this->md->insertMetadataEl($fiId, 'fadeOut', $fadeOut, 'A');
            if(PEAR::isError($r)){ return $r; }
            $fadeOutId = $r;
        }
        return compact('plElId', 'plElGunid', 'fadeInId', 'fadeOutId');
    }
}
	
class TracklistItem {
		
}
?>