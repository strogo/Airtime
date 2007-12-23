<?php
/**
 * @author $Author: paul $
 * @version $Revision: 2855 $
 */
header("Content-type: text/plain");
require_once('conf.php');
require_once("$STORAGE_SERVER_PATH/var/conf.php");

echo $CC_CONFIG['storageDir']."\n";
?>