<?php

require_once 'StoredFile.php';

define('NICE_NAME_CATEGORY','dc:title');
define('CREATE_M3U',true);

/**
 * Anonym Access Class
 * 
 * Share the binary content of StorageServer without access check.
 * 
 * Create a link to the media file, which link has the title as filename (nice
 * URL).
 * All nice URL available as .m3u playlist too.
 * 
 */

class AnonymAccess {
    var $gb;
    var $sessid;


    var $validcharacters_regexp = '/[^a-zA-Z0-9_]/';
    var $replace_to = '_';
    
    
    var $value;
    var $oldvalue;
    var $old_linkname;
    var $new_linkname;
    var $old_link;
    var $new_link;
    var $old_m3u;
    var $new_m3u;
    var $id;
    var $extension;
    var $mediafile;
    
	function AnonymAccess(&$gb, $sessid) {
		$this->gb =& $gb;
        $this->sessid = $sessid;
        //$this->getFileInfo();
        //$this->debug();
    }
    
    function changeIt($id, $value, $lang) {
        $this->value = $value;
        $this->id = $id;
    	$old = $this->gb->getMdataValue($id, NICE_NAME_CATEGORY, $this->sessid, $lang);
        //$this->debug($old,'old');
        $this->oldvalue=$old[0]['value'];
        $this->old_linkname = preg_replace(
            $this->validcharacters_regexp,
            $this->replace_to,
            $this->oldvalue
        );
        $this->new_linkname = preg_replace(
            $this->validcharacters_regexp,
            $this->replace_to,
            $this->value
        ); 
        $ac=$this->getFileInfo();
        //$this->debug($ac,'ac');    
        $this->extension = substr($ac->name,strrpos($ac->name,'.'));
        $this->mediafile = $ac->md->resDir.'/'.$ac->md->gunid;
        $this->debug($this->mediafile,'mediafile');
        
        $this->old_link = $this->gb->config['anonymAccessDir'].'/'.$this->old_linkname.$this->extension;
        $this->new_link = $this->gb->config['anonymAccessDir'].'/'.$this->new_linkname.$this->extension;
        $this->old_m3u  = $this->gb->config['anonymAccessDir'].'/'.$this->old_linkname.'.m3u';
        $this->new_m3u  = $this->gb->config['anonymAccessDir'].'/'.$this->new_linkname.'.m3u';
        
        $this->debug();
        
        if ($this->oldvalue!=$value) {
            if (is_link($this->old_link)) {
            	return $this->renameNiceURL();
            } else {
                return $this->createNiceURL();
            }
        } else {
            if (!is_link($this->old_link)) {
                return $this->createNiceURL();
            } 
        }
        return true;
    }
    
    function createNiceURL() {
        $this->debug($this->mediafile.' -> '.$this->new_link,'createNiceURL');
        if (CREATE_M3U) {
            $this->createM3U();
        }
        if (is_file($this->mediafile) && !is_file($this->new_link)) {
            return symlink($this->mediafile,$this->new_link);            
        } else {
            return false;   
        }
        
    }
    
    function renameNiceURL() {
        $this->debug($this->old_link.' -> '.$this->new_link,'renameNiceURL');
        if (CREATE_M3U) {
            $this->renameM3U();
        }
        return rename($this->old_link,$this->new_link);
    }
    
    function deleteNiceURL() {
        if (CREATE_M3U) {
            $this->deleteM3U();
        }
    	return unlink($this->old_link);
    }
    
    function createM3U() {
        $this->debug($this->new_m3u.' -> '.$this->new_link,'createM3U');
        return file_put_contents(
            $this->new_m3u,
            $this->new_link
        )==strlen($this->new_link);
    }

    function renameM3U() {
        $this->debug($this->old_m3u.' -> '.$this->new_m3u,'renameM3U');
        if (is_file($this->old_m3u)) {
            return rename($this->old_m3u,$this->new_m3u);
        } else {
            return $this->createM3U();
        }
        
    }

    function deleteM3U() {
        return unlink($this->old_m3u);
    }
    
    function &getFileInfo() {
        return StoredFile::recallByGunid($this->gb,$this->gb->_gunidFromId($this->id));
    }
    
    function debug($arr=null,$title='title') {
        if (is_null($arr)) {
            //$tmpgb = $this->gb;
            //$this->gb = null;
            file_put_contents('/tmp/aa.log','anonym:'.print_r($this,true),FILE_APPEND);
            //$this->gb = $tmpgb;
        } else {
            file_put_contents('/tmp/aa.log',$title.':'.print_r($arr,true),FILE_APPEND);
        }
    }
}
?>
