<?php

define('ACTION_BASE', '/actions' ) ;

/**
 * HTML User Interface module
 * @package Campcaster
 * @subpackage htmlUI
 * @version $Revision$
 */
class uiHandler extends uiBase {
	public $uiBase;
	public $redirUrl;

    /**
     * Initialize a new Browser Class
     * Call uiBase constructor
     *
     * @param array $config
     * 		configurartion data
     */
    public function __construct($config)
    {
        parent::__construct($config);
    } // constructor


    // --- authentication ---
    /**
     * Login to the storageServer.
     * It set sessid to the cookie with name defined in ../conf.php
     */
    function login($formdata, $mask)
    {
        #$this->_cleanArray($_SESSION);

        if (!$this->_validateForm($formdata, $mask)) {
            $_SESSION['retransferFormData']['login'] = $formdata['login'];
            $_SESSION['retransferFormData']['langid'] = $formdata['langid'];
            $_SESSION['retransferFormData']['pass'] = "\n";
            $this->redirUrl = UI_BROWSER.'?popup[]=login';
            return FALSE;
        }

        $sessid = $this->gb->login($formdata['login'], $formdata['pass']);

        if (!$sessid || PEAR::isError($sessid)){
            $this->_retMsg('Login failed.');
            $_SESSION['retransferFormData']['login'] = $formdata['login'];
            $_SESSION['retransferFormData']['langid'] = $formdata['langid'];
            $_SESSION['retransferFormData']['pass'] = "\n";
            $this->redirUrl = UI_BROWSER.'?popup[]=login';
            return FALSE;
        }

        #setcookie($this->config['authCookieName'], $sessid);
        echo "<meta http-equiv='set-cookie' content='".$this->config['authCookieName']."=".$sessid.";'>";
        ob_flush();

        $id = $this->gb->getObjId($formdata['login'], $this->gb->storId);

        if (PEAR::isError($id)) {
            $this->_retMsg('Access to home directory failed.');
            $_SESSION['retransferFormData']['login'] = $formdata['login'];
            $this->redirUrl = UI_BROWSER.'?popup[]=login';
            return FALSE;
        }

        $this->sessid = $sessid;
        $this->langid = $formdata['langid'];
        $this->redirUrl = UI_BROWSER.'?popup[]=_2SCHEDULER&popup[]=_close';

        return TRUE;
     } // fn login


    /**
     * Logut from storageServer, takes sessid from cookie
     *
     * @param boolean $trigger_login
     * 		trigger login popup after logout
     */
    function logout($trigger_login = FALSE)
    {
        $this->gb->logout($this->sessid);
        #setcookie($this->config['authCookieName'], '');
        echo "<meta http-equiv='set-cookie' content='".$this->config['authCookieName']."=;'>";
        ob_clean();
        session_destroy();

        if ($trigger_login) {
             $this->redirUrl = UI_BROWSER.'?popup[]=_clear_parent&popup[]=login';
        } else {
        	$this->redirUrl = UI_BROWSER.'?popup[]=_clear_parent&popup[]=_close';
        }
    } // fn logout


    // --- files ---
    /**
     * Provides file upload and store it to the storage
     *
     * @param array $formdata
     * 		submitted text and file
     */
    function uploadFile($formdata, $mask, $replace=NULL)
    {
        if ($this->test4audioType($formdata['mediafile']['name']) === FALSE) {
            if (UI_ERROR) {
            	$this->_retMsg('"$1" uses an unsupported file type.', $formdata['mediafile']['name']);
            }
            $this->redirUrl = UI_BROWSER."?act=addFileData&folderId=".$formdata['folderId'];
            return FALSE;
        }

        $id  = $formdata['id'];
        $folderId = $formdata['folderId'];

        if ($this->gb->getFileType($folderId) != 'Folder') {
            $this->_retMsg('The target is not a folder.');
            $this->redirUrl = UI_BROWSER."?act=fileList";
            return FALSE;
        }

        if (!$this->_validateForm($formdata, $mask)) {
            $this->redirUrl = UI_BROWSER."?act=editFile&id=".$id;
            return FALSE;
        }

        $metadata = camp_get_audio_metadata($formdata['mediafile']['tmp_name']);
        if (PEAR::isError($metadata)) {
            $this->_retMsg($metadata->getMessage());
            $this->redirUrl = UI_BROWSER."?act=addFileData&folderId=".$formdata['folderId'];
            return FALSE;
        }

        $tmpgunid = md5(microtime().$_SERVER['SERVER_ADDR'].rand()."org.mdlf.campcaster");
        $ntmp = $this->gb->bufferDir.'/'.$tmpgunid;
        move_uploaded_file($formdata['mediafile']['tmp_name'], $ntmp);
        chmod($ntmp, 0664);

//        echo "buffer dir: ".$this->gb->bufferDir."<BR>";
//        echo "$ntmp <br>";
//        print_r($formdata);
//        exit;
        $r = $this->gb->putFile($folderId, $formdata['mediafile']['name'], $ntmp, NULL, $this->sessid, $replace);
        @unlink($ntmp);

        if (PEAR::isError($r)) {
            $this->_retMsg($r->getMessage());
            $this->redirUrl = UI_BROWSER."?act=editFile&id=".$id;
            return FALSE;
        }

        $this->setMetadataValue($r, UI_MDATA_KEY_TITLE, $formdata['mediafile']['name']);
        $this->transMData($r);

		// set records in default language too
        if (UI_UPLOAD_LANGID !== UI_DEFAULT_LANGID) {
            $this->setMetadataValue($r, UI_MDATA_KEY_TITLE, $formdata['mediafile']['name'], UI_UPLOAD_LANGID);
            $this->transMData($r, UI_UPLOAD_LANGID);
        }

        $this->redirUrl = UI_BROWSER."?act=addFileMData&id=$r";
        if (UI_VERBOSE) {
        	$this->_retMsg('Data of audiclip saved.');
        }
        return $r;
    } // fn uploadFile


    function test4audioType($filename)
    {
        foreach ($this->config['file_types'] as $t) {
            if (preg_match('/'.str_replace('/', '\/', $t).'$/i', $filename)) {
                return TRUE;
            }
        }
        return FALSE;
    } // fn test4AudioType


    /**
     * @todo Rename this function.
     * @param unknown_type $id
     * @param unknown_type $langid
     * @return void
     */
    function transMData($id, $langid=UI_DEFAULT_LANGID)
    {
        include(dirname(__FILE__).'/formmask/metadata.inc.php');

        $ia = $this->gb->analyzeFile($id, $this->sessid);
        if (PEAR::isError($ia)) {
            $this->_retMsg($ia->getMessage());
            return;
        }
        $this->setMetadataValue($id, UI_MDATA_KEY_DURATION, $this->gb->_secsToPlTime($ia['playtime_seconds']));
        $this->setMetadataValue($id, UI_MDATA_KEY_FORMAT, UI_MDATA_VALUE_FORMAT_FILE);

        // some data from raw audio
        if (isset($ia['audio']['channels'])) {
        	$this->setMetadataValue($id, UI_MDATA_KEY_CHANNELS, $ia['audio']['channels']);
        }
        if (isset($ia['audio']['sample_rate'])) {
        	$this->setMetadataValue($id, UI_MDATA_KEY_SAMPLERATE, $ia['audio']['sample_rate']);
        }
        if (isset($ia['audio']['bitrate'])) {
        	$this->setMetadataValue($id, UI_MDATA_KEY_BITRATE, $ia['audio']['bitrate']);
        }
        if (isset($ia['audio']['codec'])) {
        	$this->setMetadataValue($id, UI_MDATA_KEY_ENCODER, $ia['audio']['codec']);
        }

        // from id3 Tags
        // loop main, music, talk
        foreach ($mask['pages'] as $key => $val) {
        	// loop through elements
            foreach ($mask['pages'][$key] as $k => $v) {
            	if (isset($v['element']) && isset($ia[$v['element']])) {
	                $this->setMetadataValue($id, $v['element'], $ia[$v['element']], $langid);
            	}
            }
        }
    }


    /**
     * Provides file upload and store it to the storage
     *
     * @param array $formdata, submitted text and file
     * @param unknown $mask
     */
    function addWebstream($formdata, $mask)
    {
        $id  = $formdata['id'];
        $folderId = $formdata['folderId'];

        if ($this->gb->getFileType($folderId) != 'Folder') {
            $this->_retMsg ('The target is not a folder.');
            $this->redirUrl = UI_BROWSER."?act=fileList";
            return FALSE;
        }
        if (!$this->_validateForm($formdata, $mask)) {
            $this->redirUrl = UI_BROWSER."?act=editWebstream&id=".$id;
            return FALSE;
        }

        $r = $this->gb->storeWebstream($folderId, $formdata['title'], NULL, $this->sessid, NULL, $formdata['url']);

        if (PEAR::isError($r)) {
            $this->_retMsg($r->getMessage());
            $this->redirUrl = UI_BROWSER."?act=editWebstream&id=".$id;
            return FALSE;
        }

        $extent = sprintf('%02d', $formdata['length']['H']).':'.sprintf('%02d', $formdata['length']['i']).':'.sprintf('%02d', $formdata['length']['s']).'.000000';

        $this->setMetadataValue($r, UI_MDATA_KEY_TITLE,    $formdata['title']);
        $this->setMetadataValue($r, UI_MDATA_KEY_DURATION, $extent);
        $this->setMetadataValue($r, UI_MDATA_KEY_FORMAT, UI_MDATA_VALUE_FORMAT_STREAM);

        $this->redirUrl = UI_BROWSER."?act=addWebstreamMData&id=$r";
        if (UI_VERBOSE) {
        	$this->_retMsg('Stream data saved.');
        }
        return $r;
    } // fn addWebstream


    function editWebstream($formdata, $mask)
    {
        $id  = $formdata['id'];
        if (!$this->_validateForm($formdata, $mask)) {
            $this->redirUrl = UI_BROWSER."?act=editWebstream&id=".$id;
            return FALSE;
        }
        $extent = sprintf('%02d', $formdata['length']['H']).':'.sprintf('%02d', $formdata['length']['i']).':'.sprintf('%02d', $formdata['length']['s']).'.000000';

        $this->setMetadataValue($id, UI_MDATA_KEY_TITLE, $formdata['title']);
        $this->setMetadataValue($id, UI_MDATA_KEY_URL, $formdata['url']);
        $this->setMetadataValue($id, UI_MDATA_KEY_DURATION, $extent);

        $this->redirUrl = UI_BROWSER.'?act=editItem&id='.$formdata['id'];
        if (UI_VERBOSE) {
        	$this->_retMsg('Stream data saved.');
        }
        return TRUE;
    } // fn editWebstream


    /**
     * @todo Rename this function to "editMetadata".
     * @param unknown_type $formdata
     */
    function editMetaData($formdata)
    {
        include(dirname(__FILE__).'/formmask/metadata.inc.php');
        $id             = $formdata['id'];
        $curr_langid    = $formdata['curr_langid'];
        $this->redirUrl = UI_BROWSER."?act=editItem&id=$id&curr_langid=".$formdata['target_langid'];

        foreach ($mask['pages'] as $key => $val) {
            foreach ($mask['pages'][$key] as $k => $v) {
                $formdata[$key.'___'.uiBase::formElementEncode($v['element'])] ? $mData[uiBase::formElementDecode($v['element'])] = $formdata[$key.'___'.uiBase::formElementEncode($v['element'])] : NULL;
            }
        }

        if (!count($mData)) {
        	return;
        }

        foreach ($mData as $key => $val) {
            $r = $this->setMetadataValue($id, $key, $val, $curr_langid);
            if (PEAR::isError($r)) {
                $this->_retMsg('Unable to set "$1" to value "$2".', $key, $val);
            }
        }

        if (UI_VERBOSE) {
        	$this->_retMsg('Metadata saved.');
        }
    } // fn editMetadata


    /**
     * Create new folder in the storage
     *
     * @param string $name
     * 		name for the new folder
     * @param int $id
     * 		local id to create folder in
     */
    function newFolder($name, $id)
    {
        $r = $this->gb->createFolder($id, $name, $this->sessid);
        if (PEAR::isError($r)) {
            $this->_retMsg($r->getMessage());
        }
        $this->redirUrl = UI_BROWSER.'?act=fileList&id='.$this->id;
    } // fn newFolder


    /**
     * Change the name of file or folder
     *
     * @param string $newname
     * 		new name for the file or folder
     * @param int $id
     * 		destination folder id
     */
    function rename($newname, $id)
    {
        $r = $this->gb->renameFile($id, $newname, $this->sessid);
        if (PEAR::isError($r)) {
        	$this->_retMsg($r->getMessage());
        }
        $this->redirUrl = UI_BROWSER."?act=fileList&id=".$this->pid;
    } // fn rename


    /**
     * Move file to another folder
     *
     * @todo format of destination path should be properly defined
     *
     * @param string $newPath
     * 		destination relative path
     * @param int $id
     * 		destination folder id
     */
    function move($newPath, $id)
    {
        $newPath = urldecode($newPath);
        $did = $this->gb->getObjIdFromRelPath($id, $newPath);
        $r = $this->gb->moveFile($id, $did, $this->sessid);
        if (PEAR::isError($r)) {
            $this->_retMsg($r->getMessage());
            $this->redirUrl  = UI_BROWSER."?act=fileList&id=".$this->pid;
        } else {
        	$this->redirUrl = UI_BROWSER."?act=fileList&id=".$did;
        }
    } // fn move


    /**
     *  Copy file to another folder
     *
     *  @todo format of destinantion path should be properly defined
     *
     *  @param string $newPath
     * 		destination relative path
     *  @param int $id
     * 		destination folder id
     */
    function copy($newPath, $id)
    {
        $newPath = urldecode($newPath);
        $did = $this->gb->getObjIdFromRelPath($id, $newPath);
        $r = $this->gb->copyFile($id, $did, $this->sessid);
        if (PEAR::isError($r)) {
            $this->_retMsg($r->getMessage());
            $this->redirUrl  = UI_BROWSER."?act=fileList&id=".$this->pid;
        } else {
        	$this->redirUrl = UI_BROWSER."?act=fileList&id=".$did;
        }
    } // fn copy


    /**
     * Delete a stored file.
     *
     * @param mixed $id
     * 		either an int or an array of ints which are
     * 		IDs of files or folders to delete.
     * @param boolean $delOverride
     * 		this parameter is not used
     * @return boolean
     */
    function delete($id, $delOverride=FALSE)
    {
        $this->redirUrl = UI_BROWSER."?popup[]=_reload_parent&popup[]=_close";

        /* no folder support yet
        if (!($delOverride==$id) && (count($this->gb->getObjType($id)=='Folder'?
                      $this->gb->listFolder($id, $this->sessid):NULL))) {
            $this->_retMsg("Folder is not empty. You can override this protection by clicking DEL again");
            $this->redirUrl = UI_BROWSER."?act=fileList&id=".$this->pid."&delOverride=$id";
            return FALSE;
        }
        */

        if (is_array($id)) {
        	$ids = $id;
        } else {
        	$ids[] = $id;
        }

        foreach ($ids as $id) {
            if ($this->gb->getFileType($id) == 'playlist') {
                $r = $this->gb->deletePlaylist($id, $this->sessid);
            } else {
                $r = $this->gb->deleteFile($id, $this->sessid);
            }

            if (PEAR::isError($r)) {
                $this->_retMsg($r->getMessage());
                return FALSE;
            }
        }

        return TRUE;
    } // fn delete


    /**
     * Call access method and show access path.
     * Example only - not really useable.
     * @todo resource should be released by release method call
     *
     * @param int $id
     * 		local id of accessed file
     */
    function getFile($id)
    {
        $r = $this->gb->access($id, $this->sessid);
        if (PEAR::isError($r)) {
        	$this->_retMsg($r->getMessage());
        } else {
        	echo $r;
        }
    } // fn getFile


    /**
     * Show file's metadata as XML
     *
     * @param int $id
     * 		local id of stored file
     */
    function getMdata($id)
    {
        header("Content-type: text/xml");
        $r = $this->gb->getMdata($id, $this->sessid);
        print_r($r);
    }


    // --- perms ---
    /**
     * Add new permission record
     *
     * @param int $subj
     * 		local user/group id
     * @param string $permAction
     * 		type of action from set predefined in conf.php
     * @param int $id
     * 		local id of file/object
     * @param char $allowDeny
     * 		'A' or 'D'
     * @return boolean
     */
    function addPerm($subj, $permAction, $id, $allowDeny)
    {
        if (PEAR::isError(
            $this->gb->addPerm(
                $subj, $permAction, $id, $allowDeny, $this->sessid
            )
        )) {
            $this->_retMsg('Access denied.');
            return FALSE;
        }
        $this->redirUrl = UI_BROWSER.'?act=permissions&id='.$id;
        return TRUE;
    } // fn addPerm


    /**
     * Remove permission record
     *
     * @param int $permid
     * 		local id of permission record
     * @param int $oid
     * 		local id of object to handle
     */
    function removePerm($permid, $oid)
    {
        if (PEAR::isError($this->gb->removePerm($permid, NULL, NULL, $this->sessid))) {
            $this->_retMsg('Access denied.');
            return FALSE;
        }
        $this->redirUrl = UI_BROWSER.'?act=permissions&id='.$oid;
        return TRUE;
    } // fn removePerm


    /**
     * @param unknown_type $formdata
     * @param array $mask
     * @return boolean
     */
    function _validateForm($formdata, $mask)
    {
        $form = new HTML_QuickForm('validation', UI_STANDARD_FORM_METHOD, UI_HANDLER);
        uiBase::parseArrayToForm($form, $mask, 'server');
        if (!$form->validate()) {
            $_SESSION['retransferFormData'] = $_REQUEST;
            return FALSE;
        }
        // test for uploaded files bacause HTMLQuickForm::validate() ignores them
        if (is_array($form->_submitFiles)) {
            $was_error = FALSE;
            foreach ($form->_submitFiles as $key => $val) {
                if ($val['error']) {

                    switch ($val['error']) {
                        case 1:
                            $was_error = TRUE;
                            $this->_retMsg('The uploaded filer is bigger than allowed in system settings. See "Help", chapter "Troubleshooting" for more information.');
                            break;
                        case 2:
                            $was_error = TRUE;
                            $this->_retMsg('The uploaded filer is bigger than allowed in system settings. See "Help", chapter "Troubleshooting" for more information.');
                            break;
                        case 3:
                            $was_error = TRUE;
                            $this->_retMsg('Upload of file "$1" was incomplete.', $mask[$key]['label']);
                            break;
                        case 4:
                            if ($mask[$key]['required']) {
                                $was_error = TRUE;
                                $this->_retMsg('File "$1" has not been uploaded.', $mask[$key]['label']);
                            }
                            break;
                    }
                }
            }
            if ($was_error) {
                $_SESSION['retransferFormData'] = array_merge($_REQUEST, $_FILES);
                #$this->_retMsg('Invalid or incomplete form data.');
                return FALSE;
            }
        }
        /*
        foreach($mask as $k) {
            if ($k['type']=='file' && $k['required']==TRUE) {
                if ($_FILES[$k['element']]['error']) {
                    $_SESSION['retransferFormData'] = array_merge($_REQUEST, $_FILES);
                    return FALSE;
                }
            }
        }  */
        return TRUE;
    } // fn _validateForm


    /**
     * @param unknown_type $formdata
     * @param unknown_type $mask
     * @return boolean
     */
    function changeStationPrefs($formdata, $mask)
    {
        $this->redirUrl = UI_BROWSER;

        if ($this->_validateForm($formdata, $mask) == FALSE) {
            $this->_retMsg('Error while saving settings.');
            return FALSE;
        }
        foreach ($mask as $key => $val) {
            if (isset($val['isPref']) && $val['isPref']) {
                if (strlen($formdata[$val['element']])) {
                    if (PEAR::isError($this->gb->saveGroupPref($this->sessid, 'StationPrefs', $val['element'], $formdata[$val['element']])))
                        $this->_retMsg('Error while saving settings.');
                } else {
                    $this->gb->delGroupPref($this->sessid,  'StationPrefs', $val['element']);
                }
            }
            if (isset($val['type']) && ($val['type'] == 'file') && $formdata[$val['element']]['name']) {
                $stationLogoPath = $this->gb->loadGroupPref($this->sessid, 'StationPrefs', 'stationLogoPath');
                $filePath = $formdata[$val['element']]['tmp_name'];
                if (function_exists("getimagesize")) {
                    $size = @getimagesize($filePath);
                    if ($size === FALSE) {
                        $this->_retMsg('Error while uploading logo: the file uploaded is not an image.');
                        return FALSE;
                    }
                    if ( ($size[0] > 128) || ($size[1] > 128) ) {
                        $this->_retMsg('Error uploading logo: the logo can be no larger than 128x128.');
                        return FALSE;
                    }
                }
                $success = @move_uploaded_file($filePath, $stationLogoPath);
                if (!$success) {
                    $this->_retMsg('Error while uploading logo: could not move the file to the destination directory.');
                    return FALSE;
                }
            }
        }
        $this->loadStationPrefs($mask, TRUE);
        if (UI_VERBOSE) {
            $this->_retMsg('Settings saved.');
        }

        return TRUE;
    } // fn changeStationPrefs

} // class uiHandler
?>
