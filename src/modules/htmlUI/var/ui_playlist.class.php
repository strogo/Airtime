<?php
class uiPlaylist
{
    function uiPlaylist(&$uiBase)
    {
        $this->Base         =& $uiBase;
        $this->activeId     =& $_SESSION[UI_PLAYLIST_SESSNAME]['activeId'];
        $this->changed      =& $_SESSION[UI_PLAYLIST_SESSNAME]['changed']; 
        $this->title        = $this->Base->_getMDataValue($this->activeId, UI_MDATA_KEY_TITLE);
        $this->duration     = $this->Base->_getMDataValue($this->activeId, UI_MDATA_KEY_DURATION);
        $this->token        =& $_SESSION[UI_PLAYLIST_SESSNAME]['token'];
        $this->reloadUrl    = UI_BROWSER.'?popup[]=_reload_parent&popup[]=_close';
        $this->redirectUrl  = UI_BROWSER.'?popup[]=_2PL.simpleManagement&popup[]=_close';
        $this->returnUrl    = UI_BROWSER.'?act=PL.simpleManagement';
    }

    function setReload()
    {
        $this->Base->redirUrl = $this->reloadUrl;
    }

    function setRedirect($target=FALSE)
    {
        if ($target!==FALSE)
            $this->Base->redirUrl = UI_BROWSER."?popup[]=$target&popup[]=_close";
        else
            $this->Base->redirUrl = $this->redirectUrl;
    }

    function setReturn()
    {
        $this->Base->redirUrl = $this->returnUrl;
    }

    function getPLArray($id)
    {
        return $this->Base->gb->getPlaylistArray($id, $this->Base->sessid);
    }

    function getActiveArr()
    {
        if (!$this->activeId) {
            return FALSE;
        }
        #echo '<pre><div align="left">'; print_r( $this->Base->gb->getPlaylistArray($this->activeId, $this->Base->sessid)); echo '</div></pre>';
        return $this->getPLArray($this->activeId);
    }

    function getActiveId()
    {
        if (!$this->activeId) {
            return FALSE;
        }
        return $this->activeId;
    }

    function activate($plid, $msg=TRUE)
    {
        # test if PL available
        # look PL
        # store access token to ls_pref abd session
        # load PL into session
        if ($this->token) {
            if (UI_WARNING) $this->Base->_retMsg('You already have an open playlist. Close it first.');
            return FALSE;
        }
        if(($userid = $this->Base->gb->playlistIsAvailable($plid, $this->Base->sessid)) !== TRUE) {
             if (UI_WARNING) $this->Base->_retMsg('Playlist has been locked by "$1".', $this->Base->gb->getSubjName($userid));
            return FALSE;
        }
        $token = $this->Base->gb->lockPlaylistForEdit($plid, $this->Base->sessid); 
        if (PEAR::isError($token)) {
            if (UI_VERBOSE === TRUE) print_r($token);
            $this->Base->_retMsg('Unable to open playlist "$1".', $this->Base->_getMDataValue($plid, UI_MDATA_KEY_TITLE));
            return FALSE;
        }
        $this->token = $token;
        $this->Base->gb->savePref($this->Base->sessid, UI_PL_ACCESSTOKEN_KEY, $plid.':'.$this->token);
        $this->activeId = $plid;
        if ($msg && UI_VERBOSE) $this->Base->_retMsg('Playlist "$1" opened.', $this->Base->_getMDataValue($plid, UI_MDATA_KEY_TITLE));

        return TRUE;
    }

    function release($msg=TRUE)
    {
        # get token from ls_pref
        # release PL
        # delete PL from session
        # remove token from ls_pref
        if(!$this->token) {
             if (UI_WARNING) $this->Base->_retMsg('There is no playlist available to unlock.');
            return FALSE;
        }
        $plgunid = $this->Base->gb->releaseLockedPlaylist($this->token, $this->Base->sessid);
        if (PEAR::isError($plgunid)) {
            if (UI_VERBOSE === TRUE) print_r($plgunid);
            if (UI_WARNING) $this->Base->_retMsg('Unable to release playlist.');
            return FALSE;
        }
        if ($msg && UI_VERBOSE) $this->Base->_retMsg('Playlist "$1" released.', $this->Base->_getMDataValue($this->Base->gb->_idFromGunid($plgunid), UI_MDATA_KEY_TITLE));
        $this->activeId = NULL;
        $this->token    = NULL;
        $this->Base->gb->delPref($this->Base->sessid, UI_PL_ACCESSTOKEN_KEY);
        
        $this->changed = FALSE;
        return TRUE;
    }


    function save()
    {
        $tmpid = $this->activeId;
        $this->release(FALSE);
        $this->activate($tmpid, FALSE);
        $this->changed = FALSE;
        if (UI_VERBOSE) $this->Base->_retMsg('Playlist "$1" saved.', $this->Base->_getMDataValue($tmpid, UI_MDATA_KEY_TITLE));

        return $this->activeId;
    }


    function revert()
    {        
        if(!$this->token) {
            if (UI_WARNING) $this->Base->_retMsg('No playlists have been locked by you.');
            return FALSE;
        }
        $plgunid = $this->Base->gb->revertEditedPlaylist($this->token, $this->Base->sessid);
        if (PEAR::isError($plgunid)) {
            if (UI_VERBOSE === TRUE) print_r($plgunid);
            if (UI_WARNING) $this->Base->_retMsg('Unable to revert to locked state.');
            return FALSE;
        }
        if (UI_VERBOSE) $this->Base->_retMsg('Playlist "$1" reverted.', $this->Base->_getMDataValue($this->Base->gb->_idFromGunid($plgunid), UI_MDATA_KEY_TITLE));
        $this->activeId = NULL;
        $this->token    = NULL;
        $this->Base->gb->delPref($this->Base->sessid, UI_PL_ACCESSTOKEN_KEY);

        if ($this->activate($this->Base->gb->_idFromGunid($plgunid), FALSE) !== TRUE)
            return FALSE;
            
        $this->changed  = FALSE;
        
        return $this->activeId;
    }

    function reportLookedPL($setMsg=FALSE)
    {
        if(is_string($this->Base->gb->loadPref($this->Base->sessid, UI_PL_ACCESSTOKEN_KEY))) {
            if ($setMsg == TRUE) $this->Base->_retMsg('Found locked playlist.');
            return TRUE;
        }
        return FALSE;
    }

    function loadLookedFromPref()
    {
        $this->changed  = TRUE;
        
        if(is_string($saved = $this->Base->gb->loadPref($this->Base->sessid, UI_PL_ACCESSTOKEN_KEY))) {
            list ($plid, $token) = explode (':', $saved);
            
            if (!$this->Base->gb->existsPlaylist($plid, $this->Base->sessid)) { 
                $this->Base->gb->delPref($this->Base->sessid, UI_PL_ACCESSTOKEN_KEY); 
                $this->Base->_retMsg('Playlist not found in database.');
                $this->Base->redirUrl = UI_BROWSER.'?popup[]=_2PL.simpleManagement&popup[]=_close';
                return FALSE;          
            }
            $this->activeId = $plid;
            $this->token    = $token;
            
            $this->Base->redirUrl = UI_BROWSER.'?popup[]=_2PL.simpleManagement&popup[]=_close';
            return TRUE;
        }
        return FALSE;
    }

    function addItem($elemIds, $duration=null)
    {
        $this->changed  = TRUE;
        $fadeIn         = null;
        $fadeOut        = null;
        $pause          = null;        
        
        if (!$elemIds) {
            if (UI_WARNING) $this->Base->_retMsg('No item(s) selected.');
            return FALSE;
        }
        if (!is_array($elemIds)) {
            $elemIds = array($elemIds);
        }
        if (isset($duration)) {
            $length = sprintf('%02d', $duration['H']).':'.sprintf('%02d', $duration['i']).':'.sprintf('%02d', $duration['s']).'.000000';
        }
        foreach ($elemIds as $elemId) { 
            $r = $this->Base->gb->addAudioClipToPlaylist($this->token, $elemId, $this->Base->sessid, $fadeIn, $fadeOut, $length, $pause);
            if (PEAR::isError($r)) {
                if (UI_VERBOSE === TRUE) print_r($r);
                $this->Base->_retMsg('Error while trying to add item to playlist.');
                return FALSE;
            }
        }        
        return TRUE;
    }

    function removeItem($elemIds)
    {
        $this->changed = TRUE;
        
        if (!$elemIds) {
            if (UI_WARNING) $this->Base->_retMsg('No item(s) selected.');
            return FALSE;
        }
        if (!is_array($elemIds))
            $elemIds = array($elemIds);

        foreach ($elemIds as $elemId) {
            if ($this->Base->gb->delAudioClipFromPlaylist($this->token, $elemId, $this->Base->sessid) !== TRUE) {
                $this->Base->_retMsg('Cannot remove item from playlist.');
                return FALSE;
            }
        }
        return TRUE;
    }

    function create($ids)
    {
        # create PL
        # activate
        # add clip if $id is set
        $this->changed = TRUE;
        
        if (is_array($this->activeId)) {
            $this->Base->_retMsg('A playlist is already opened.');
            return FALSE;
        }
        $datetime = strftime('%Y-%m-%d %H:%M:%S');
        $plid = $this->Base->gb->createPlaylist($this->Base->homeid, $datetime, $this->Base->sessid);
        if (!$plid) {
            $this->Base->_retMsg('Cannot create playlist.');
            return FALSE;
        }

        $this->Base->_setMDataValue($plid, UI_MDATA_KEY_CREATOR,     $this->Base->login);
        $this->Base->_setMDataValue($plid, UI_MDATA_KEY_DESCRIPTION, tra('created at $1', $datetime));

        if ($this->activate($plid)===FALSE) {
            return FALSE;
        }
        if ($ids) {
            if ($this->addItem($ids)!==TRUE) {
                return FALSE;
            }
        }
        #$this->redirUrl = UI_BRWOSER.'?popup=_2PL.simpleManagement';<br>

        return $plid;
    }


    function getFlat($id)
    {
        unset($this->flat);
        $this->_plwalk($this->getPLArray($id));
        
        if (is_Array($this->flat)) {
            reset($this->flat);
            $this->flat[key($this->flat)]['firstInList'] = true;
            end($this->flat);
            $this->flat[key($this->flat)]['lastInList'] = true; 
            reset($this->flat);

            return $this->flat;
        } else {
            return array();    
        }
    }


    function _plwalk($arr, $parent=0, $attrs=0)
    {
        foreach ($arr['children'] as $node=>$sub) {
            if ($sub['elementname']===UI_PL_ELEM_PLAYLIST) {
                $this->_plwalk($sub, $node, $sub['attrs']);
            }
            if ($sub['elementname']===UI_FILETYPE_AUDIOCLIP || $sub['elementname']===UI_FILETYPE_PLAYLIST) {
                #$this->flat["$parent.$node"] = $sub['attrs'];
                #$this->flat["$parent.$node"]['type'] = $sub['elementname'];
                $this->flat[$parent] = $this->Base->_getMetaInfo($this->Base->gb->_idFromGunid($sub['attrs']['id']));
                $this->flat[$parent]['attrs'] = $attrs;
                $this->flat[$parent]['playlength'] = $sub['attrs']['playlength'];
            }
            if ($sub['elementname']===UI_PL_ELEM_FADEINFO) {
                $this->flat[$parent][UI_PL_ELEM_FADEIN]  = GreenBox::_plTimeToSecs($sub['attrs'][UI_PL_ELEM_FADEIN]);
                $this->flat[$parent][UI_PL_ELEM_FADEOUT] = GreenBox::_plTimeToSecs($sub['attrs'][UI_PL_ELEM_FADEOUT]);
                $this->flat[$parent]['fadein_ms']  = $sub['attrs'][UI_PL_ELEM_FADEIN]  ? GreenBox::_plTimeToSecs($sub['attrs'][UI_PL_ELEM_FADEIN])  * 1000 : 0;
                $this->flat[$parent]['fadeout_ms'] = $sub['attrs'][UI_PL_ELEM_FADEOUT] ? GreenBox::_plTimeToSecs($sub['attrs'][UI_PL_ELEM_FADEOUT]) * 1000 : 0;
            }
        }
    }


    function changeTransition($id, $type, $duration)
    {
        $this->changed = TRUE;
        $pause = $pause;
        $xfade = GreenBox::_secsToPlTime($duration/1000);

        if ($id) {
            // just change fade between 2 clips
            $curr = $this->getCurrElement($id);
            $prev = $this->getPrevElement($id);
            $next = $this->getNextElement($id);

            switch ($type) {
                case "fadeX":
                    $item[$prev['attrs']['id']] =
                                  array(UI_PL_ELEM_FADEIN  => GreenBox::_secsToPlTime($prev[UI_PL_ELEM_FADEIN]),
                                        UI_PL_ELEM_FADEOUT => $xfade
                                  );
                    $item[$id]  = array(UI_PL_ELEM_FADEIN  => $xfade,
                                        UI_PL_ELEM_FADEOUT => GreenBox::_secsToPlTime($curr[UI_PL_ELEM_FADEOUT])
                                  );
                break;
                case "pause":
                    $item[$prev['attrs']['id']] =
                                  array(UI_PL_ELEM_FADEIN  => GreenBox::_secsToPlTime($prev[UI_PL_ELEM_FADEIN]),
                                        UI_PL_ELEM_FADEOUT => $pause
                                  );
                    $item[$id]  = array(UI_PL_ELEM_FADEIN  => $pause,
                                        UI_PL_ELEM_FADEOUT => GreenBox::_secsToPlTime($curr[UI_PL_ELEM_FADEOUT])
                                  );
                break;
                case "fadeIn":
                    $item[$id]  = array(UI_PL_ELEM_FADEIN  => $xfade,
                                        UI_PL_ELEM_FADEOUT => GreenBox::_secsToPlTime($curr[UI_PL_ELEM_FADEOUT])
                                  );
                break;
                case "fadeOut":
                    $item[$id] = array(UI_PL_ELEM_FADEIN  => GreenBox::_secsToPlTime($curr[UI_PL_ELEM_FADEIN]),
                                       UI_PL_ELEM_FADEOUT => $xfade
                                 );
                break;
            }
            #print_r($item);
            foreach ($item as $i=>$val) {
                $r = $this->Base->gb->changeFadeInfo($this->token, $i, $val[UI_PL_ELEM_FADEIN], $val[UI_PL_ELEM_FADEOUT], $this->Base->sessid);
                #print_r($r);
                if (PEAR::isError($r)) {
                    if (UI_VERBOSE === TRUE) print_r($r);
                    $this->Base->_retMsg('Changing fade information failed.');
                    return FALSE;
                }
            }
        } else {
            // change fade of all clips
            foreach ($this->getFlat($this->activeId) as $v) {
                $r = $this->Base->gb->changeFadeInfo($this->token, $v['attrs']['id'], $type==='pause'?$pause:$xfade, $type==='pause'?$pause:$xfade, $this->Base->sessid);
                #print_r($r);
                if (PEAR::isError($r)) {
                    if (UI_VERBOSE === TRUE) print_r($r);
                    $this->Base->_retMsg('Changing fade information failed.');
                    return FALSE;
                }
            }
        }
        return TRUE;
    }


    function moveItem($id, $pos)
    {
        $this->changed = TRUE;
        
        $r = $this->Base->gb->moveAudioClipInPlaylist($this->token, $id, $pos, $this->Base->sessid);
        if (PEAR::isError($r)) {
            if (UI_VERBOSE === TRUE) print_r($r);
            $this->Base->_retMsg('Cannot move item.');
            return FALSE;
        }
        return TRUE;
    }


    function reOrder($items)
    {
        $this->changed  = TRUE;
                
        asort($items);      # just to be sure items are in right order
        $pos = 0;
        foreach($items as $id=>$v) { 
            $pos++;
            $r = $this->Base->gb->moveAudioClipInPlaylist($this->token, $id, $pos, $this->Base->sessid);
            if (PEAR::isError($r)) {
                if (UI_VERBOSE === TRUE) print_r($r);
                $this->Base->_retMsg('Cannot move item.');
                return FALSE;
            }
        }
        return TRUE;
    }

    function getCurrElement($id)
    {
        $arr = $this->getFlat($this->activeId);
        while ($val = current($arr)) {
            if ($val['attrs']['id'] == $id) {
                return current($arr);
            }
            next($arr);
        }
    }


    function getPrevElement($id)
    {
        $arr = $this->getFlat($this->activeId);
        while ($val = current($arr)) {
            if ($val['attrs']['id'] == $id) {
                return prev($arr);
            }
            next($arr);
        }
    }


    function getNextElement($id)
    {
        $arr = $this->getFlat($this->activeId);
        while ($val = current($arr)) {
            if ($val['attrs']['id'] == $id) {
                return next($arr);
            }
            next($arr);
        }
    }


    function changeTransitionForm($id, $type, $mask)
    {
        $form = new HTML_QuickForm('PL_changeTransition', UI_STANDARD_FORM_METHOD, UI_HANDLER);
        $s = $this->getCurrElement($id);
        switch ($type) {
            case "fadeIn":
                $d = $this->getCurrElement($id);
                $duration = $d['fadein_ms'];
                $form->setConstants(array('headline' => '<b>'.$s['title'].'</b>'));
            break;
            case "transition":
                $d = $this->getPrevElement($id);
                $duration = $s['fadein_ms'];
                $form->setConstants(array('headline' => '<b>'.$d['title'].'</b> <-> <b>'.$s['title'].'</b>'));
            break;
            case "fadeOut":
                $d = $this->getCurrElement($id);
                $duration = $d['fadeout_ms'];
                $form->setConstants(array('headline' => '<b>'.$s['title'].'</b>'));
            break;
        }
        $form->setConstants(array('id'       => $id,
                                  'duration' => $duration)
        );
        $this->Base->_parseArr2Form($form, $mask[$type]);
        $this->Base->_parseArr2Form($form, $mask['all']);
        $renderer =& new HTML_QuickForm_Renderer_Array(true, true);
        $form->accept($renderer);
        return $renderer->toArray();
    }


    function changeAllTransitionsForm($mask)
    {
        $form = new HTML_QuickForm('PL_changeTransition', UI_STANDARD_FORM_METHOD, UI_HANDLER);
        $this->Base->_parseArr2Form($form, $mask['transition']);
        $this->Base->_parseArr2Form($form, $mask['all']);
        $renderer =& new HTML_QuickForm_Renderer_Array(true, true);
        $form->accept($renderer);
        return $renderer->toArray();
    }
    
    function setItemPlaylengthForm($id, $elemId, $mask)
    {
        if (isset($elemId)) { 
            $mask['act']['constant']        = 'PL.setItemPlaylength';
            $mask['elemId']['constant']     = $elemId;    
            $element                        = $this->getCurrElement($elemId);
            $mask['playlength']['default']  = substr($element['playlength'], 0, 8);
            $mask['duration']['constant']   = substr($element['duration'], 0, 8); 
        } else {
            $mask['act']['constant']        = 'PL.addItem';
            $mask['id']['constant']         = $id;
            $mask['playlength']['default']  = substr($this->Base->_getMDataValue($id, UI_MDATA_KEY_DURATION), 0, 8); 
            $mask['duration']['constant']   = $mask['playlength']['default'];
        }
        
        $form = new HTML_QuickForm('PL_setItemPlaylengthForm', UI_STANDARD_FORM_METHOD, UI_HANDLER);
        $this->Base->_parseArr2Form($form, $mask);
        $renderer =& new HTML_QuickForm_Renderer_Array(true, true);
        $form->accept($renderer);
        return $renderer->toArray();
    }


    function metaDataForm($langid)
    {
        include dirname(__FILE__).'/formmask/metadata.inc.php';
        $id     = $this->activeId;
        $langid = $langid ? $langid : UI_DEFAULT_LANGID;

        foreach ($mask['playlist'] as $k=>$v) {
            $mask['playlist'][$k]['element'] = $this->Base->_formElementEncode($v['element']);
            if ($getval = $this->Base->_getMDataValue($id, $v['element'], $langid)) {
                $mask['playlist'][$k]['default']                = $getval;
                $mask['playlist'][$k]['attributes']['onFocus']  = 'MData_confirmChange(this)';
            };
        }
        $form = new HTML_QuickForm('editMetaData', UI_STANDARD_FORM_METHOD, UI_HANDLER);
        $this->Base->_parseArr2Form($form, $mask['basics']);
        $this->Base->_parseArr2Form($form, $mask['playlist']);
        $this->Base->_parseArr2Form($form, $mask['buttons']);
        $form->setConstants(array('act'  => 'PL.editMetaData',
                                  'id'   => $id,
                                  'curr_langid' => $langid
                            )
        );
        $renderer =& new HTML_QuickForm_Renderer_Array(true, true);
        $form->accept($renderer);
        $output['main'] = $renderer->toArray();

        $form = new HTML_QuickForm('langswitch', UI_STANDARD_FORM_METHOD, UI_BROWSER);
        $this->Base->_parseArr2Form($form, $mask['langswitch']);
        $form->setConstants(array('target_langid'   => $langid));
        $renderer =& new HTML_QuickForm_Renderer_Array(true, true);
        $form->accept($renderer);
        $output['langswitch'] = $renderer->toArray();

        #print_r($output);
        return $output;
    }


    function editMetaData($formdata)
    {
        $this->changed  = TRUE;
                
        include dirname(__FILE__).'/formmask/metadata.inc.php';

        $id             = $this->activeId;
        $curr_langid    = $formdata['curr_langid'];

        ## if language switched stay on metadataform ##
        if ($curr_langid === $formdata['target_langid'])
            $this->Base->redirUrl = UI_BROWSER."?act=PL.simpleManagement";
        else
            $this->Base->redirUrl = UI_BROWSER."?act=PL.editMetaData&id=$id&curr_langid=".$formdata['target_langid'];

        foreach ($mask['playlist'] as $k=>$v) {
            $formdata[$this->Base->_formElementEncode($v['element'])] ? $mData[$this->Base->_formElementDecode($v['element'])] = $formdata[$this->Base->_formElementEncode($v['element'])] : NULL;
        }

        if (!count($mData)) return;

        foreach ($mData as $key=>$val) {
            $r = $this->Base->gb->setMDataValue($id, $key, $this->Base->sessid, $val, $curr_langid);
            if (PEAR::isError($r)) {
                if (UI_VERBOSE === TRUE) print_r($r);
                $this->Base->_retMsg('Unable to set "$1" to value "$2".', $key, $val);
            }
        }
        if (UI_VERBOSE) $this->Base->_retMsg('Metadata saved.');
    }


    function deleteActive()
    {
        $id = $this->activeId;
        $this->release(FALSE);
        if ($this->Base->delete($id))
            return $id;
        $this->Base->_retMsg('Cannot delete this playlist.');
        return FALSE;
    }


    function isAvailable($id)
    {
        if ($this->Base->gb->getFileType($id) !== UI_FILETYPE_PLAYLIST)
            return TRUE;

        if ($this->Base->gb->playlistIsAvailable($id, $this->Base->sessid) === TRUE)
            return TRUE;

        return FALSE;
    }

    function isUsedBy($id)
    {
        if ($this->Base->gb->getFileType($id) !== UI_FILETYPE_PLAYLIST)
            return FALSE;

        if (($userid = $this->Base->gb->playlistIsAvailable($id, $this->Base->sessid)) !== TRUE)
             return $this->Base->gb->getSubjName($userid);

        return FALSE;
    }
    
    function exportForm($id,$mask)
    {
        $mask['act']['constant']        = 'PL.export';
        $mask['id']['constant']         = $id;
        $form = new HTML_QuickForm('PL_exportForm', UI_STANDARD_FORM_METHOD, UI_HANDLER);
        $this->Base->_parseArr2Form($form, $mask);
        $renderer =& new HTML_QuickForm_Renderer_Array(true, true);
        $form->accept($renderer);
        return $renderer->toArray();
    }

    function importForm($id, $mask)
    {
        $form = new HTML_QuickForm('PL_importForm', UI_STANDARD_FORM_METHOD, UI_HANDLER);
        //print_r($mask);
        $this->Base->_parseArr2Form($form, $mask);
        $renderer =& new HTML_QuickForm_Renderer_Array(true, true);
        $form->accept($renderer);
        return $renderer->toArray();
    }
}
?>