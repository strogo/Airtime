<?php
/**
 * @author Tomas Hlava <th@red2head.com>
 * @author Paul Baranowski <paul@paulbaranowski.org>
 * @version $Revision: 2458 $
 * @package Campcaster
 * @subpackage StorageServer
 * @copyright 2006 MDLF, Inc.
 * @license http://www.gnu.org/licenses/gpl.txt
 * @link http://www.campware.org
 */

// Do not allow remote execution.
$arr = array_diff_assoc($_SERVER, $_ENV);
if (isset($arr["DOCUMENT_ROOT"]) && ($arr["DOCUMENT_ROOT"] != "") ) {
    header("HTTP/1.1 400");
    header("Content-type: text/plain; charset=UTF-8");
    echo "400 Not executable\r\n";
    exit;
}

if (!function_exists('pg_connect')) {
    trigger_error("PostgreSQL PHP extension required and not found.", E_USER_ERROR);
    exit(2);
}

require_once('DB.php');


function camp_db_table_exists($p_name)
{
    global $CC_DBC;
    $sql = "SELECT * FROM ".$p_name;
    $result = $CC_DBC->GetOne($sql);
    if (PEAR::isError($result)) {
        return false;
    }
    return true;
}

$CC_DBC = DB::connect($CC_CONFIG['dsn'], TRUE);
if (PEAR::isError($CC_DBC)) {
    echo "Database connection problem.\n";
    echo "Check if database '".$CC_CONFIG['dsn']['database']."' exists".
        " with corresponding permissions.\n";
    echo "Database access is defined by 'dsn' values in conf.php.\n";
    exit(1);
}

$CC_DBC->setFetchMode(DB_FETCHMODE_ASSOC);

if (camp_db_table_exists($CC_CONFIG['transTable'])) {
    echo " * Removing database table ".$CC_CONFIG['transTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['transTable']);
    $CC_DBC->dropSequence($CC_CONFIG['transTable']."_id_seq");
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['transTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['mdataTable'])) {
    echo " * Removing database table ".$CC_CONFIG['mdataTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['mdataTable']);
    $CC_DBC->dropSequence($CC_CONFIG['mdataTable']."_id_seq");
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['mdataTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['filesTable'])) {
    echo " * Removing database table ".$CC_CONFIG['filesTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['filesTable']);
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['filesTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['accessTable'])) {
    echo " * Removing database table ".$CC_CONFIG['accessTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['accessTable']);
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['accessTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['permTable'])) {
    echo " * Removing database table ".$CC_CONFIG['permTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['permTable']);
    $CC_DBC->dropSequence($CC_CONFIG['permTable']."_id_seq");
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['permTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['sessTable'])) {
    echo " * Removing database table ".$CC_CONFIG['sessTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['sessTable']);
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['sessTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['subjTable'])) {
    echo " * Removing database table ".$CC_CONFIG['subjTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['subjTable']);
    $CC_DBC->dropSequence($CC_CONFIG['subjTable']."_id_seq");
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['subjTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['smembTable'])) {
    echo " * Removing database table ".$CC_CONFIG['smembTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['smembTable']);
    $CC_DBC->dropSequence($CC_CONFIG['smembTable']."_id_seq");
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['smembTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['classTable'])) {
    echo " * Removing database table ".$CC_CONFIG['classTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['classTable']);
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['classTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['cmembTable'])) {
    echo " * Removing database table ".$CC_CONFIG['cmembTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['cmembTable']);
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['cmembTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['structTable'])) {
    echo " * Removing database table ".$CC_CONFIG['structTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['structTable']);
    $CC_DBC->dropSequence($CC_CONFIG['structTable']."_id_seq");
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['structTable']."\n";
}

if (camp_db_table_exists($CC_CONFIG['treeTable'])) {
    echo " * Removing database table ".$CC_CONFIG['treeTable']."...";
    $CC_DBC->query("DROP TABLE ".$CC_CONFIG['treeTable']);
    $CC_DBC->dropSequence($CC_CONFIG['treeTable']."_id_seq");
    echo "done.\n";
} else {
    echo " * Skipping: database table ".$CC_CONFIG['treeTable']."\n";
}

?>