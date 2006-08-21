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

include_once "XML/RPC.php";
include_once dirname(__FILE__)."/../conf.php";

$pars = $argv;
array_shift($pars);
$verbose = FALSE;
#$verbose = TRUE;
if($pars[0] == '-v'){ $verbose = TRUE; array_shift($pars); }
if($pars[0] == '-s'){
    array_shift($pars);
    $serverPath = array_shift($pars);
}else{
    $serverPath =
      "http://{$config['storageUrlHost']}:{$config['storageUrlPort']}".
      "{$config['storageUrlPath']}/{$config['storageXMLRPC']}";
}

#$serverPath = "http://localhost:80/livesupportStorageServerCVS/xmlrpc/xrLocStor.php";

$url = parse_url($serverPath);
$client = new XML_RPC_Client($url['path'], $url['host']);
$method = array_shift($pars);

if($verbose){
    $client->debug = 1;
    echo "serverPath: $serverPath\n";
    echo "host: {$url['host']}, path: {$url['path']}\n";
    echo "method: $method\n";
    echo "pars:\n";
    var_dump($pars);
}

$infos = array(
    "listMethods"       => array('m'=>"system.listMethods", 'p'=>NULL),
    "methodHelp"        => array('m'=>"system.methodHelp", 'p'=>0),
    "methodSignature"   => array('m'=>"system.methodSignature", 'p'=>0),
    "test"              =>
        array('m'=>"locstor.test", 'p'=>array('sessid', 'teststring')),
    "getVersion"      => array('m'=>"locstor.getVersion",
        'p'=>array(), 'r'=>'version'),
    "authenticate"      => array('m'=>"locstor.authenticate",
        'p'=>array('login', 'pass'), 'r'=>'authenticate'),
    "login"             => array('m'=>"locstor.login",
        'p'=>array('login', 'pass'), 'r'=>'sessid'),
    "logout"            => array('m'=>"locstor.logout",
        'p'=>array('sessid'), 'r'=>'status'),

    "storeAudioClipOpen"        => array('m'=>"locstor.storeAudioClipOpen",
        'p'=>array('sessid', 'gunid', 'metadata', 'fname', 'chsum'),
        'r'=>array('url', 'token')
    ),
    "storeAudioClipClose"       => array('m'=>"locstor.storeAudioClipClose",
        'p'=>array('sessid', 'token'), 'r'=>'gunid'),
    "accessRawAudioData"        => array('m'=>"locstor.accessRawAudioData",
        'p'=>array('sessid', 'gunid'), 'r'=>array('url', 'token')),
    "releaseRawAudioData"       => array('m'=>"locstor.releaseRawAudioData",
        'p'=>array('token'), 'r'=>'status'),
    "downloadRawAudioDataOpen"  =>
        array('m'=>"locstor.downloadRawAudioDataOpen",
            'p'=>array('sessid', 'gunid'), 'r'=>array('url', 'token')),
    "downloadRawAudioDataClose" =>
        array('m'=>"locstor.downloadRawAudioDataClose",
            'p'=>array('sessid', 'token'), 'r'=>'gunid'),
    "downloadMetadataOpen"      => array('m'=>"locstor.downloadMetadataOpen",
        'p'=>array('sessid', 'gunid'), 'r'=>array('url', 'token')),
    "downloadMetadataClose"     => array('m'=>"locstor.downloadMetadataClose",
        'p'=>array('sessid', 'token'), 'r'=>'gunid'),

    "deleteAudioClip"   =>
        array('m'=>"locstor.deleteAudioClip",
            'p'=>array('sessid', 'gunid', 'forced'), 'r'=>'status'),
    "existsAudioClip"   => array('m'=>"locstor.existsAudioClip",
        'p'=>array('sessid', 'gunid'), 'r'=>'exists'),
    "getAudioClip"  => array('m'=>"locstor.getAudioClip",
        'p'=>array('sessid', 'gunid'), 'r'=>'metadata'),
    "updateAudioClipMetadata"   => array('m'=>"locstor.updateAudioClipMetadata",
        'p'=>array('sessid', 'gunid', 'metadata'), 'r'=>'status'),
    "searchMetadata"    => array('m'=>"locstor.searchMetadata", 'p'=>NULL),
    "browseCategory"    => array('m'=>"locstor.browseCategory", 'p'=>NULL),
    "resetStorage"  => array('m'=>"locstor.resetStorage",
        'p'=>array()),
#        'p'=>array('loadSampleData', 'invalidateSessionIds')),
    "storeWebstream"    => array('m'=>"locstor.storeWebstream",
        'p'=>array('sessid', 'gunid', 'metadata', 'fname', 'url'),
        'r'=>array('gunid')
    ),

    "createPlaylist"    => array('m'=>"locstor.createPlaylist",
        'p'=>array('sessid', 'plid', 'fname'), 'r'=>'plid'),
    "editPlaylist"      => array('m'=>"locstor.editPlaylist",
        'p'=>array('sessid', 'plid'), 'r'=>array('url', 'token')),
    "savePlaylist"      => array('m'=>"locstor.savePlaylist",
        'p'=>array('sessid', 'token', 'newPlaylist'), 'r'=>'plid'),
    "revertEditedPlaylist"      => array('m'=>"locstor.revertEditedPlaylist",
        'p'=>array('sessid', 'token'), 'r'=>'plid'),
    "deletePlaylist"    => array('m'=>"locstor.deletePlaylist",
        'p'=>array('sessid', 'plid', 'forced'), 'r'=>'status'),
    "accessPlaylist"    => array('m'=>"locstor.accessPlaylist",
        'p'=>array('sessid', 'plid'), 'r'=>array('url', 'token')),
    "releasePlaylist"   => array('m'=>"locstor.releasePlaylist",
        'p'=>array('token'), 'r'=>'plid'),
    "existsPlaylist"    => array('m'=>"locstor.existsPlaylist",
        'p'=>array('sessid', 'plid'), 'r'=>'exists'),
    "playlistIsAvailable"   => array('m'=>"locstor.playlistIsAvailable",
        'p'=>array('sessid', 'plid'), 'r'=>array('available', 'ownerid', 'ownerlogin')),

    "exportPlaylistOpen"   => array('m'=>"locstor.exportPlaylistOpen",
        'p'=>array('sessid', 'plids', 'type', 'standalone'),
        'r'=>array('url', 'token')),
    "exportPlaylistClose"   => array('m'=>"locstor.exportPlaylistClose",
        'p'=>array('token'), 'r'=>array('status')),
    "importPlaylistOpen"   => array('m'=>"locstor.importPlaylistOpen",
        'p'=>array('sessid', 'chsum'), 'r'=>array('url', 'token')),
    "importPlaylistClose"   => array('m'=>"locstor.importPlaylistClose",
        'p'=>array('token'), 'r'=>array('gunid')),

    "renderPlaylistToFileOpen"   => array('m'=>"locstor.renderPlaylistToFileOpen",
        'p'=>array('sessid', 'plid'),
        'r'=>array('token')),
    "renderPlaylistToFileCheck"   => array('m'=>"locstor.renderPlaylistToFileCheck",
        'p'=>array('token'), 'r'=>array('status', 'url')),
    "renderPlaylistToFileClose"   => array('m'=>"locstor.renderPlaylistToFileClose",
        'p'=>array('token'), 'r'=>array('status')),
    "renderPlaylistToStorageOpen"   => array('m'=>"locstor.renderPlaylistToStorageOpen",
        'p'=>array('sessid', 'plid'),
        'r'=>array('token')),
    "renderPlaylistToStorageCheck"   => array('m'=>"locstor.renderPlaylistToStorageCheck",
        'p'=>array('token'), 'r'=>array('status', 'gunid')),
    "renderPlaylistToRSSOpen"   => array('m'=>"locstor.renderPlaylistToRSSOpen",
        'p'=>array('sessid', 'plid'),
        'r'=>array('token')),
    "renderPlaylistToRSSCheck"   => array('m'=>"locstor.renderPlaylistToRSSCheck",
        'p'=>array('token'), 'r'=>array('status', 'url')),
    "renderPlaylistToRSSClose"   => array('m'=>"locstor.renderPlaylistToRSSClose",
        'p'=>array('token'), 'r'=>array('status')),

    "createBackupOpen"    => array('m'=>"locstor.createBackupOpen",
        'p'=>array('sessid', 'criteria'), 'r'=>array('token')),
    "createBackupCheck"   => array('m'=>"locstor.createBackupCheck",
        'p'=>array('token'), 'r'=>array('status', 'url', 'metafile')),
    "createBackupClose"   => array('m'=>"locstor.createBackupClose",
        'p'=>array('token'), 'r'=>array('status')),

    "loadPref"      => array('m'=>"locstor.loadPref",
        'p'=>array('sessid', 'key'), 'r'=>'value'),
    "savePref"      => array('m'=>"locstor.savePref",
        'p'=>array('sessid', 'key', 'value'), 'r'=>'status'),
    "delPref"       => array('m'=>"locstor.delPref",
        'p'=>array('sessid', 'key'), 'r'=>'status'),
    "loadGroupPref"      => array('m'=>"locstor.loadGroupPref",
        'p'=>array('sessid', 'group', 'key'), 'r'=>'value'),
    "saveGroupPref"      => array('m'=>"locstor.saveGroupPref",
        'p'=>array('sessid', 'group', 'key', 'value'), 'r'=>'status'),

    "getTransportInfo"       => array('m'=>"locstor.getTransportInfo",
        'p'=>array('trtok'), 
        'r'=>array('state', 'realsize', 'expectedsize', 'realchsum', 'expectedchsum')),
    "turnOnOffTransports" => array('m'=>"locstor.turnOnOffTransports",
        'p'=>array('sessid', 'onOff'), 'r'=>array('state')),
    "doTransportAction" => array('m'=>"locstor.doTransportAction",
        'p'=>array('sessid', 'trtok', 'action'), 'r'=>array('state')),
    "uploadFile2Hub" => array('m'=>"locstor.uploadFile2Hub",
        'p'=>array('sessid', 'filePath'), 'r'=>array('trtok')),
    "getHubInitiatedTransfers" => array('m'=>"locstor.getHubInitiatedTransfers",
        'p'=>array('sessid'), 'r'=>array()),
    "startHubInitiatedTransfer" => array('m'=>"locstor.startHubInitiatedTransfer",
        'p'=>array('trtok'), 'r'=>array()),
    "upload2Hub" => array('m'=>"locstor.upload2Hub",
        'p'=>array('sessid', 'gunid'), 'r'=>array('trtok')),
    "downloadFromHub" => array('m'=>"locstor.downloadFromHub",
        'p'=>array('sessid', 'gunid'), 'r'=>array('trtok')),
    "globalSearch" => array('m'=>"locstor.globalSearch",
        'p'=>array('sessid', 'criteria'), 'r'=>array('trtok')),
    "getSearchResults" => array('m'=>"locstor.getSearchResults",
        'p'=>array('trtok')),

/*
    "uploadToArchive"       => array('m'=>"locstor.uploadToArchive",
        'p'=>array('sessid', 'gunid'), 'r'=>'trtok'),
    "downloadFromArchive"       => array('m'=>"locstor.downloadFromArchive",
        'p'=>array('sessid', 'gunid'), 'r'=>'trtok'),
*/

    "openPut"       => array('m'=>"locstor.openPut", 'p'=>array()),
    "closePut"      => array('m'=>"locstor.closePut", 'p'=>array()),
);


switch($method){
case"searchMetadata":
case"globalSearch":
case"createBackupOpen":
    $parr = array(
        'sessid'=>$pars[0],
        'criteria'=>array(
            'filetype'=>'audioclip',
            'operator'=>'and',
            'limit'=> 0,
            'offset'=> 0,
            'conditions'=>array(
                array('cat'=>$pars[1], 'op'=>'partial', 'val'=>$pars[2])
            )
        ),
    );
    break;
case"browseCategory":
    $parr = array(
        'sessid'=>$pars[0],
        'category'=>$pars[1],
        'criteria'=>array(
            'filetype'=>'audioclip',
            'operator'=>'and',
            'limit'=> 0,
            'offset'=> 0,
            'conditions'=>array(
                array('cat'=>$pars[2], 'op'=>'partial', 'val'=>$pars[3])
            )
        ),
    );
    break;
case"resetStorage":
    $parr = array(
        'loadSampleData'=>(boolean)$pars[0],
        'invalidateSessionIds'=>(boolean)$pars[1],
    );
    break;
default:
    $pinfo = $infos[$method]['p'];
    if(is_null($pinfo)){
        $parr = NULL;
    }elseif(!is_array($pinfo)){
        $parr = $pars[0];
        #echo "pinfo not null and not array.\n"; exit;
    }elseif(count($pinfo) == 0){
        $parr = (object)array();
    }else{
        $parr = array(); $i=0;
        foreach($pinfo as $it){
            if(isset($pars[$i])) $parr[$it] = $pars[$i];
            $i++;
        }
    }
} // switch

$fullmethod = $infos[$method]['m'];
$msg = new XML_RPC_Message($fullmethod, array(XML_RPC_encode($parr)));

if($verbose){
    echo "parr:\n";
    var_dump($parr);
    echo "message:\n";
    echo $msg->serialize()."\n";
}

#$client->setDebug(1);
$res = $client->send($msg);
if($res->faultCode() > 0) {
    echo "xr_cli_test.php: ".$res->faultString()." ".$res->faultCode()."\n";
#    echo var_export($res);
    exit(1);
}

if($verbose){
    echo "result:\n";
    echo $res->serialize();
}

$resp = XML_RPC_decode($res->value());
if(isset($infos[$method]['r'])){
    $pom = $infos[$method]['r'];
    if(is_array($pom)){
        foreach($pom as $k=>$it) $pom[$k] = $resp[$it];
        echo join(' ', $pom)."\n";
    }else switch($pom){
        case"status":
        case"exists":
            echo ($resp[$pom]=='1' ? "TRUE" : "FALSE" )."\n";
            break;
        default:
            echo "{$resp[$pom]}\n";
    }
}else{
    switch($method){
    case"searchMetadata":
    case"getSearchResults":
        $acCnt = 0; $acGunids = array();
        $plCnt = 0; $plGunids = array();
        foreach($resp['results'] as $k=>$v){
            if($v['type']=='audioclip'){ $acCnt++;
                $acGunids[] = $v['gunid'];
            }
            if($v['type']=='playlist'){ $plCnt++;
                $plGunids[] = $v['gunid'];
            }
        }
        echo
            "AC({$acCnt}): ".
                join(", ", $acGunids).
            " | PL({$plCnt}): ".
                join(", ", $plGunids).
            "\n";
        break;
    case"browseCategory":
        echo
            "RES({$resp['cnt']}): ".
                join(", ", $resp['results']).
            "\n";
        break;
    default:
        print_r($resp);
    }
}

?>
