<?php
header("Content-type: text/html; charset=utf-8");
session_start();

## LS classes/functions #############################################
require_once(dirname(__FILE__).'/ui_conf.php');
require_once(dirname(__FILE__).'/ui_handler.class.php');

## initialize objects ###############################################
$uiHandler = new uiHandler($CC_CONFIG);
$uiHandler->init();
$uiBase =& $uiHandler;

//include("../templates/loader/index.tpl");
ob_start();
?>