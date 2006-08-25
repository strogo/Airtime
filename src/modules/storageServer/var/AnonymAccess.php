<?php

require_once 'StoredFile.php';

define('NICE_NAME_CATEGORY','dc:title');

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


    var $validcharacters_regexp = '/[^a-zA-Z0-1_]/';
    var $replace_to = '_';
    
    
    var $value;
    var $oldvalue;
    var $old_linkname;
    var $new_linkname;
    var $old_link;
    var $new_link;
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
    	global $config;
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
        $this->debug($ac,'ac');    
        $this->extension = substr($ac->name,strrpos($ac->name,'.'));
        $this->mediafile = $ac->md->resDir.'/'.$ac->md->gunid;
        $this->debug($this->mediafile,'mediafile');
        
        $this->old_link = $config['AnonymAccessDir'].'/'.$this->old_linkname.$this->extension;
        $this->new_link = $config['AnonymAccessDir'].'/'.$this->new_linkname.$this->extension;
        
        $this->debug();
        
        if ($this->oldvalue!=$value) {
            if (is_link($this->old_link)) {
            	return $this->renameNiceURL();
            } else {
                return $this->createNiceURL();
            }
        } else {
            if (!$this->old_link) {
                return $this->createNiceURL();
            } 
        }
        return true;
    }
    
    function createNiceURL() {
        return symlink($this->mediafile,$this->new_link);
    }
    
    function renameNiceURL() {
        return rename($this->old_link,$this->new_link);
    }
    
    function deleteNiceURL() {
    	return unlink($this->old_link);
    }
    
    function createM3U() {
        global $config;
        return file_put_contents(
            $config['AnonymAccessDir'].'/'.$this->new_linkname.'.m3u',
            $this->new_link
        )==strlen($this->new_link);
    }

    function renameM3U() {
        global $config;
        return rename(
            $config['AnonymAccessDir'].'/'.$this->old_linkname.'.m3u',
            $config['AnonymAccessDir'].'/'.$this->new_linkname.'.m3u'
        );
    }

    function deleteM3U() {
        global $config;
        return unlink($config['AnonymAccessDir'].'/'.$this->old_linkname.'.m3u');
    }
    
    function &getFileInfo() {
        return StoredFile::recallByGunid($this->gb,$this->gb->_gunidFromId($this->id));
    }
    
    function debug($arr=null,$title='title') {
        if (is_null($arr)) {
            $tmpgb = $this->gb;
            $this->gb = null;
            file_put_contents('/tmp/aa.log','anonym:'.print_r($this,true),FILE_APPEND);
            $this->gb = $tmpgb;
        } else {
            file_put_contents('/tmp/aa.log',$title.':'.print_r($arr,true),FILE_APPEND);
        }
    }
}
?>
