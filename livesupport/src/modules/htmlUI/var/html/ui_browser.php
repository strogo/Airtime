<?php
require_once dirname(__FILE__).'/../ui_browser_init.php';

if (is_array($_REQUEST['popup'])){
    foreach ($_REQUEST['popup'] as $val) {
        switch ($val) {
            case "jscom":
                 $Smarty->display('jscom.tpl');
            break;

            case "_reload_parent":
                $Smarty->display('popup/_reload_parent.tpl');
            break;

            case "_close":
                $Smarty->display('popup/_close.tpl');
            break;

            case "_clear_parent":
                $Smarty->display('popup/_clear_parent.tpl');
            break;

            case "_2PL.simpleManagement":
                $Smarty->assign('target', 'PL.simpleManagement');
                $Smarty->display('popup/_redirector.tpl');
            break;

            case "_2PL.editMetaData":
                $Smarty->assign('target', 'PL.editMetaData');
                $Smarty->display('popup/_redirector.tpl');
            break;

            case "_2changeStationPrefs":
                $Smarty->assign('target', 'changeStationPrefs');
                $Smarty->display('popup/_redirector.tpl');
            break;

            case "_2SCHEDULER":
                $Smarty->assign('target', 'SCHEDULER');
                $Smarty->display('popup/_redirector.tpl');
            break;

            case "login":
                $Smarty->assign('dynform', $uiBrowser->login($ui_fmask));
                $Smarty->display('popup/login.tpl');
            break;

            case "logout":
                $Smarty->assign('logouttype', 'logout');
                $Smarty->display('popup/logout.tpl');
            break;

            case "signover_1":
                $Smarty->assign('logouttype', 'signover');
                $Smarty->display('popup/logout.tpl');
            break;

            case "signover_2":
                $Smarty->assign('loginform', $uiBrowser->loginform($Smarty, $ui_fmask));
                $Smarty->display('popup/login.tpl');
            break;

            case "deleteItem": 
                if (is_array($_REQUEST['id'])) { 
                    foreach ($_REQUEST['id'] as $i) {
                        $idstr .= '&id[]='.$i;
                    }
                    $Smarty->assign('filecount', count($_REQUEST['id']));
                    $Smarty->assign('idstr', $idstr);                
                } else {
                    $Smarty->assign('filename', $uiBrowser->_getMDataValue($_REQUEST['id'], UI_MDATA_KEY_TITLE));
                }
                $Smarty->display('popup/deleteItem.tpl');
            break;

            case "PL.changeTransition";
                $Smarty->assign('dynform', $uiBrowser->PLAYLIST->changeTransitionForm($_REQUEST['id'], $_REQUEST['type'], $ui_fmask['PL.changeTransition']));
                $Smarty->display('popup/PLAYLIST.changeTransition.tpl');
            break;

            case "PL.changeAllTransitions";
                $Smarty->assign('dynform', $uiBrowser->PLAYLIST->changeAllTransitionsForm($ui_fmask['PL.changeTransition']));
                $Smarty->display('popup/PLAYLIST.changeAllTransitions.tpl');
            break;

            case "PL.confirmDelete":
                $Smarty->display('popup/PLAYLIST.confirmDelete.tpl');
            break;

            case "PL.confirmRevert":
                $Smarty->display('popup/PLAYLIST.confirmRevert.tpl');
            break;

            case "PL.confirmRelease":
                $Smarty->display('popup/PLAYLIST.confirmRelease.tpl');
            break;

            case "PL.arrangeItems":
                $Smarty->display('popup/PLAYLIST.arrangeItems.tpl');
            break;

            case "SCHEDULER.addItem":
                $Smarty->display('popup/SCHEDULER.addItem.tpl');
            break;

            case "SCHEDULER.removeItem":
                $Smarty->assign('playlistName', $uiBrowser->_getMDataValue($_REQUEST['playlistId'], UI_MDATA_KEY_TITLE));
                $Smarty->display('popup/SCHEDULER.removeItem.tpl');
            break;

            case "SUBJECTS.confirmRemoveSubj":
                $Smarty->display('popup/SUBJECTS.confirmRemoveSubj.tpl');
            break;

            case "testStream":
                $Smarty->assign('data', $uiBrowser->testStream($_REQUEST['url']));
                $Smarty->display('popup/testStream.tpl');
            break;
            
            case "listen2AudioClip":
                $Smarty->assign('data', $uiBrowser->listen2AudioClip($_REQUEST['id']));
                $Smarty->display('popup/listen2AudioClip.tpl');
            break;
            
            case "help":
                $Smarty->display('popup/help.tpl');
            break;
        }
    }
    die();
};

if ($uiBrowser->userid) {
    $Smarty->assign('showMenuTop',  TRUE);

    switch ($_REQUEST['act']){
    case "fileList":
        $Smarty->assign('structure', $uiBrowser->getStructure($uiBrowser->fid));
        $Smarty->assign('fileList', TRUE);

        if ($_REQUEST['tree']=='Y')
            $Smarty->assign('showTree', TRUE);
        else
            $Smarty->assign('showObjects', TRUE);

        $Smarty->assign('delOverride', $_REQUEST['delOverride']);
    break;

    case "permissions":
        $Smarty->assign('structure',   $uiBrowser->getStructure($uiBrowser->id));
        $Smarty->assign('permissions', $uiBrowser->permissions($uiBrowser->id));
        $Smarty->assign('fileList', TRUE);
    break;


    case "uploadFileM":
        $Smarty->assign('structure', $uiBrowser->getStructure($uiBrowser->id));
        $Smarty->assign('uploadform', $uiBrowser->uploadFileM($ui_fmask['uploadFileM'], $uiBrowser->id));
    break;

    case "addFileData":
    case "addFileMData":
        $Smarty->assign('structure', $uiBrowser->getStructure($uiBrowser->id));
        $Smarty->assign('editItem', array('type' => 'audioclip', 'id' => $_REQUEST['id'], 'folderId' => $uiBrowser->fid, 'curr_langid' => $_REQUEST['curr_langid']));
    break;

    case "addWebstreamData":
    case "addWebstreamMData":
        $Smarty->assign('structure', $uiBrowser->getStructure($uiBrowser->id));
        $Smarty->assign('editItem', array('type' => 'webstream', 'id' => $_REQUEST['id'], 'folderId' => $uiBrowser->fid, 'curr_langid' => $_REQUEST['curr_langid']));
    break;

    case "editItem":
        $Smarty->assign('structure', $uiBrowser->getStructure($uiBrowser->id));
        $Smarty->assign('editItem', array('type' => $uiBrowser->type, 'id' => $_REQUEST['id'], 'folderId' => $uiBrowser->fid, 'curr_langid' => $_REQUEST['curr_langid']));
    break;


    case "SEARCH":
        $Smarty->assign('searchForm', $uiBrowser->SEARCH->searchForm($uiBrowser->id, $ui_fmask));
        $Smarty->assign('showLibrary', TRUE);
    break;

    case "BROWSE":
        $Smarty->assign('browseForm', $uiBrowser->BROWSE->browseForm($uiBrowser->id, $ui_fmask));
        $Smarty->assign('showLibrary', TRUE);
    break;

    case "getFile":
        $Smarty->assign('fData', $uiBrowser->getFile($uiBrowser->id));
        $Smarty->assign('showFile', TRUE);
    break;

    case "getMData":
        $Smarty->assign('fMetaData', $uiBrowser->getMdata($uiBrowser->id));
        $Smarty->assign('showFile', TRUE);
    break;


    case "_analyzeFile":
        $Smarty->assign('_analyzeFile', $uiBrowser->_analyzeFile($uiBrowser->id, 'text'));
        $Smarty->assign('showFile', TRUE);
    break;

    case "changeStationPrefs":
        $Smarty->assign('dynform', $uiBrowser->changeStationPrefs($ui_fmask['stationPrefs']));
        $Smarty->assign('changeStationPrefs', TRUE);
    break;

    case "PL.simpleManagement":
        $Smarty->assign('PL_simpleManagement', TRUE);
    break;

    case "PL.editMetaData":
        $Smarty->assign('PL_editMetaData', TRUE);
        $Smarty->assign('_PL', array('curr_langid' => $_REQUEST['curr_langid']));
        $Smarty->assign('PL_simpleManagement', TRUE);
    break;

    case "SCHEDULER":
        $Smarty->assign('showScheduler', TRUE);
    break;


    case "SUBJECTS":
    case "SUBJECTS.manageGroupMember":
    case "SUBJECTS.addUser":
    case "SUBJECTS.addGroup":
    case "SUBJECTS.remSubj":
    case "SUBJECTS.chgPasswd":
        $Smarty->assign('showSubjects', TRUE);
        $Smarty->assign('act', $_REQUEST['act']);
    break;
    }

    if ($_REQUEST['act'] != 'SCHEDULER')
        $Smarty->assign('simpleSearchForm',   $uiBrowser->SEARCH->simpleSearchForm($ui_fmask['simplesearch']));
}

$Smarty->display('main.tpl');
?>