<?php
define('DEBUG', FALSE);
//define('DEBUG', TRUE);
define('MODIFY_LAST_MATCH', TRUE);

require_once "XML/Util.php";

/**
 * File storage support class.
 * Store metadata tree in relational database.<br>
 *
 * @author Tomas Hlava <th@red2head.com>
 * @author Paul Baranowski <paul@paulbaranowski.org>
 * @version $Revision$
 * @package Campcaster
 * @subpackage StorageServer
 * @copyright 2006 MDLF, Inc.
 * @license http://www.gnu.org/licenses/gpl.txt
 * @link http://www.campware.org
 * @see StoredFile
 * @see XmlParser
 * @see DataEngine
 */
class MetaData {

	public $gunid;
	public $resDir;
	public $fname;
	public $exists;

    /**
     * @param Greenbox $gb
     * 		A reference to GreenBox object
     * @param string $gunid
     * 		global unique id
     * @param string $resDir
     * 		resource directory
     */
    public function __construct(&$gb, $gunid, $resDir)
    {
        $this->gunid = $gunid;
        $this->resDir = $resDir;
        $this->fname = $this->makeFname();
        $this->exists = null;
    }


    /**
     * Parse and store metadata from XML file or XML string
     *
     * @param string $mdata
     * 		Local path to metadata XML file or XML string
     * @param string $loc
     * 		location: 'file'|'string'
     * @param string $format
     * 		Metadata format for validation
     *      ('audioclip' | 'playlist' | 'webstream' | NULL)
     *      (NULL = no validation)
     * @return true|PEAR_Error
     */
    public function insert($mdata, $loc='file', $format=NULL)
    {
        if ($this->exists()) {
        	return FALSE;
        }
        $tree =& $this->parse($mdata, $loc);
        if (PEAR::isError($tree)) {
        	return $tree;
        }
        $this->format = $format;
        $res = $this->validate($tree);
        if (PEAR::isError($res)) {
        	return $res;
        }
        $res = $this->storeDoc($tree);
        if (PEAR::isError($res)) {
        	return $res;
        }
        $this->exists = TRUE;
        $r = $this->regenerateXmlFile();
        if (PEAR::isError($r)) {
        	return $r;
        }
        return TRUE;
    }


    /**
     * Call delete and insert
     *
     * @param string $mdata
     * 		local path to metadata XML file or XML string
     * @param string $loc
     * 		'file'|'string'
     * @param string $format
     * 		metadata format for validation
     *      ('audioclip' | 'playlist' | 'webstream' | NULL)
     *      (NULL = no validation)
     * @return true or PEAR::error
     */
    public function replace($mdata, $loc='file', $format=NULL)
    {
        if ($this->exists()) {
            $res = $this->delete();
            if (PEAR::isError($res)) {
            	return $res;
            }
        }
        return $this->insert($mdata, $loc, $format);
    }


    /**
     * Return true if metadata exists
     *
     * @return boolean
     */
    public function exists()
    {
    	if (is_null($this->exists)) {
	        $this->exists = $this->dbCheck($this->gunid)
            				&& is_file($this->fname)
            				&& is_readable($this->fname);
    	}
        return $this->exists;
    }


    /**
     * Delete all file's metadata
     *
     * @return TRUE|PEAR_Error
     */
    public function delete()
    {
        global $CC_CONFIG, $CC_DBC;
        if (file_exists($this->fname)) {
        	@unlink($this->fname);
        }
        $res = $CC_DBC->query("
            DELETE FROM ".$CC_CONFIG['mdataTable']."
            WHERE gunid=x'{$this->gunid}'::bigint
        ");
        if (PEAR::isError($res)) {
        	return $res;
        }
        $this->exists = FALSE;
        return TRUE;
    }


    /**
     * Return metadata XML string
     *
     * @return string
     */
    public function getMetadata()
    {
        if (file_exists($this->fname)) {
            $res = file_get_contents($this->fname);
            return $res;
        } else {
        	return file_get_contents(dirname(__FILE__).'/emptyMdata.xml');
        }
    }


    /**
     * Return all metadata for this GUNID.
     *
     * @return array
     */
    public function getAllMetadata()
    {
        global $CC_CONFIG, $CC_DBC;
        $sql = "SELECT predns, predicate, object
            	FROM ".$CC_CONFIG['mdataTable']."
            	WHERE gunid=x'{$this->gunid}'::bigint";
        $rows = $CC_DBC->getAll($sql);
        $values = array();
        foreach ($rows as $row) {
        	$values[$row["predns"].":".$row["predicate"]] = $row["object"];
        }
		return $values;
    }


    /**
     * Get metadata element value and record id
     *
     * @param string $category
     * 		metadata element name
     * @param int $parid
     * 		metadata record id of parent element
     * @return array
     * 		int 'mid': record id
     * 		string 'value': metadata value}
     */
    public function getMetadataEl($category, $parid=NULL)
    {
        global $CC_CONFIG, $CC_DBC;
        // handle predicate namespace shortcut
        $a = XML_Util::splitQualifiedName($category);
        if (PEAR::isError($a)) {
        	return $a;
        }
        $catNs = $a['namespace'];
        $cat = $a['localPart'];
        $cond = "gunid=x'{$this->gunid}'::bigint AND predicate='$cat'";
        if (!is_null($catNs)) {
        	$cond .= " AND predns='$catNs'";
        }
        if (!is_null($parid)) {
        	$cond .= " AND subjns='_I' AND subject='$parid'";
        }
        $sql = "SELECT id as mid, object as value
            	FROM ".$CC_CONFIG['mdataTable']."
            	WHERE $cond
            	ORDER BY id";
        $all = $CC_DBC->getAll($sql);
        foreach ($all as $i => $v) {
            if (is_null($all[$i]['value'])) {
                $all[$i]['value'] = '';
            }
        }
        if (PEAR::isError($all)) {
        	return $all;
        }
        return $all;
    }


    /**
     * Set metadata value / delete metadata record
     *
     * @param int $mid
     * 		metadata record id
     * @param string $value
     * 		new metadata value (NULL for delete)
     * @return boolean
     */
    public function setMetadataEl($mid, $value=NULL)
    {
        global $CC_CONFIG, $CC_DBC;
        $info = $CC_DBC->getRow("
            SELECT parmd.predns as parns, parmd.predicate as parname,
                md.predxml, md.predns as chns, md.predicate as chname
            FROM ".$CC_CONFIG['mdataTable']." parmd
            INNER JOIN ".$CC_CONFIG['mdataTable']." md
                ON parmd.id=md.subject AND md.subjns='_I'
            WHERE md.id=$mid
        ");
        if (PEAR::isError($info)) {
        	return $info;
        }
        if (is_null($info)) {
            return PEAR::raiseError(
                "MetaData::setMetadataEl: parent container not found"
            );
        }
        extract($info);
        $parname = ($parns ? "$parns:" : '').$parname;
        $category = ($chns ? "$chns:" : '').$chname;
        $r = $this->validateOneValue($parname, $category, $predxml, $value);
        if (PEAR::isError($r)) {
        	return $r;
        }
        if (!is_null($value)) {
        	$escapedValue = pg_escape_string($value);
            $sql = "
                UPDATE ".$CC_CONFIG['mdataTable']."
                SET object='$escapedValue', objns='_L'
                WHERE id={$mid}
            ";
            $res = $CC_DBC->query($sql);
        } else {
            $res = $this->deleteRecord($mid);
        }
        if (PEAR::isError($res)) {
        	return $res;
        }
        return TRUE;
    }


    /**
     * Insert new metadata element
     *
     * @param int $parid
     * 		metadata record id of parent element
     * @param string $category
     * 		metadata element name
     * @param string value
     * 		new metadata value (NULL for delete)
     * @param string $predxml
     * 		'T' | 'A' | 'N' (tag, attr., namespace)
     * @return int
     * 		new metadata record id
     */
    public function insertMetadataEl($parid, $category, $value=NULL, $predxml='T')
    {
        global $CC_CONFIG, $CC_DBC;
        //$category = strtolower($category);
        $parent = $CC_DBC->getRow("
            SELECT predns, predicate, predxml FROM ".$CC_CONFIG['mdataTable']."
            WHERE gunid=x'{$this->gunid}'::bigint AND id=$parid
        ");
        if (PEAR::isError($parent)) {
        	return $parent;
        }
        if (is_null($parent)) {
            return PEAR::raiseError(
                "MetaData::insertMetadataEl: container not found"
            );
        }
        $parNs = ($parent['predns'] ? "{$parent['predns']}:" : '');
        $parName = $parNs.$parent['predicate'];
        $r = $this->validateOneValue($parName, $category, $predxml, $value);
        if (PEAR::isError($r)) {
        	return $r;
        }
        $a = XML_Util::splitQualifiedName($category);
        if (PEAR::isError($a)) {
        	return $a;
        }
        $catNs = $a['namespace'];
        $cat = $a['localPart'];
        $objns = (is_null($value) ? '_blank' : '_L' );
        $nid= $this->storeRecord('_I', $parid, $catNs, $cat, $predxml,
            $objns, $value);
        if (PEAR::isError($nid)) {
        	return $nid;
        }
        return $nid;
    }


    /**
     * Get metadata element value
     *
     * @param string $category
     * 		metadata element name
     * @param string $lang
     * 		optional xml:lang value for select language version
     * @param string $deflang
     * 		optional xml:lang for default language
     * @param string $objns
     * 		object namespace prefix - for internal use only
     * @return array
     * 		array of matching records as hash with fields:
     *   <ul>
     *      <li>mid int, local metadata record id</li>
     *      <li>value string, element value</li>
     *      <li>attrs hasharray of element's attributes indexed by
     *          qualified name (e.g. xml:lang)</li>
     *   </ul>
     * @see BasicStor::bsGetMetadataValue
     */
    private function getMetadataValueWithAttrs($category, $lang=NULL, $deflang=NULL, $objns='_L')
    {
        $all = $this->getMetadataEl($category);
        $res = array();
        $exact = NULL;
        $def = NULL;
        $plain = NULL;
        // add attributes to result
        foreach ($all as $i => $rec) {
            $pom = $this->getSubrows($rec['mid']);
            if (PEAR::isError($pom)) {
            	return $pom;
            }
            $all[$i]['attrs'] = $pom['attrs'];
            $atlang = (isset($pom['attrs']['xml:lang']) ?
                $pom['attrs']['xml:lang'] : NULL);
            // select only matching lang (en is default)
            if (is_null($lang)) {
                $res[] = $all[$i];
            } else {
                switch (strtolower($atlang)) {
                    case '':
                        $plain = array($all[$i]);
                    	break;
                    case strtolower($lang):
                        $exact = array($all[$i]);
                    	break;
                    case strtolower($deflang):
                        $def = array($all[$i]);
                    	break;
                }
            }
        }
        if ($exact) {
        	return $exact;
        }
        if ($def) {
        	return $def;
        }
        if ($plain) {
        	return $plain;
        }
        return $res;
    }


    /**
     * Get metadata element value.
     *
     * @param string $category
     * 		metadata element name
     * @return string
     * 		If the value doesnt exist, return the empty string.
     */
    public function getMetadataValue($category)
    {
        $element = $this->getMetadataEl($category);
        $value = (isset($element[0]['value']) ? $element[0]['value'] : '');
        return $value;
    }


    /**
     * Set metadata element value
     *
     * @param string $category
     * 		metadata element name (e.g. dc:title)
     * @param string $value
     * 		value to store, if NULL then delete record
     * @param string $lang
     * 		optional xml:lang value for select language version
     * @param int $mid
     * 		metadata record id (OPTIONAL on unique elements)
     * @param string $container
     * 		container element name for insert
     * @return boolean
     */
    public function setMetadataValue($category, $value, $lang=NULL, $mid=NULL,
        $container='metadata')
    {
        // resolve actual element:
        $rows = $this->getMetadataValueWithAttrs($category, $lang);
        $aktual = NULL;
        if (count($rows) > 1) {
            if (is_null($mid)) {
                if (MODIFY_LAST_MATCH) {
                	$aktual = array_pop($rows);
                } else {
                    return PEAR::raiseError(
                        "MetaData::setMetadataValue:".
                        " nonunique category, mid required"
                    );
                }
            } else {
                foreach ($rows as $i => $row) {
                    if ($mid == intval($row['mid'])) {
                    	$aktual = $row;
                    }
                }
            }
        } else {
        	$aktual = (isset($rows[0])? $rows[0] : NULL);
        }
        if (!is_null($aktual)) {
            $res = $this->setMetadataEl($aktual['mid'], $value);
            if (PEAR::isError($res)) {
            	return $res;
            }
            if (!is_null($lang)
            	&& isset($aktual['attrs']['xml:lang'])
            	&& $aktual['attrs']['xml:lang'] != $lang)  {
                $lg = $this->getMetadataEl('xml:lang', $aktual['mid']);
                if (PEAR::isError($lg)) {
                	return $lg;
                }
                if (isset($lg['mid'])) {
                    $res = $this->setMetadataEl($lg['mid'], $lang);
                    if (PEAR::isError($res)) {
                    	return $res;
                    }
                } else {
                    $res = $this->insertMetadataEl(
                        $aktual['mid'], 'xml:lang', $lang, 'A');
                    if (PEAR::isError($res)) {
                    	return $res;
                    }
                }
            }
        } else {
            // resolve container:
            $contArr = $this->getMetadataValueWithAttrs($container, NULL, NULL, '_blank');
            if (PEAR::isError($contArr)) {
            	return $contArr;
            }
            $parid = $contArr[0]['mid'];
            if (is_null($parid)) {
                return PEAR::raiseError(
                    "MetaData::setMetadataValue: container ($container) not found"
                );
            }
            $nid = $this->insertMetadataEl($parid, $category, $value);
            if (PEAR::isError($nid)) {
            	return $nid;
            }
            if (!is_null($lang)) {
                $res = $this->insertMetadataEl($nid, 'xml:lang', $lang, 'A');
                if (PEAR::isError($res) && $res->getCode()!=VAL_UNKNOWNA) {
                	return $res;
                }
            }
        }
        return TRUE;
    }


    /**
     * Regenerate XML metadata file after metadata value change
     *
     * @return boolean
     */
    public function regenerateXmlFile()
    {
        $atime = date('c');
        // php4 fix:
        if ($atime=='c') {
          $tz = preg_replace("|00$|",":00",trim(`date +%z`));
          $atime = trim(`date +%Y-%m-%dT%H:%M:%S`).$tz;
        }
        $r = $this->setMetadataValue('ls:mtime', $atime);
        if (PEAR::isError($r)) {
            return $r;
        }
        $fn = $this->fname;
        $xml = $this->genXMLDoc();
        if (PEAR::isError($xml)) {
        	return $xml;
        }
        if (!$fh = fopen($fn, 'w')) {
            return PEAR::raiseError(
                "MetaData::regenerateXmlFile: cannot open for write ($fn)"
            );
        }
        if (fwrite($fh, $xml) === FALSE) {
            return PEAR::raiseError(
                "MetaData::regenerateXmlFile: write error ($fn)"
            );
        }
        fclose($fh);
        @chmod($fn, 0664);
        return TRUE;
    }


    /**
     * Contruct filepath of metadata file
     *
     * @return string
     */
    private function makeFname()
    {
        return "{$this->resDir}/{$this->gunid}.xml";
    }


    /**
     * Return filename
     *
     * @return string
     */
    public function getFname()
    {
        return $this->fname;
    }


    /**
     * Set the metadata format to the object instance
     * @return void
     */
    public function setFormat($format=NULL)
    {
        $this->format = $format;
    }


    /**
     * Check if there are any file's metadata in database
     *
     * @param string $gunid
     * 		global unique id
     * @return boolean
     */
    private function dbCheck($gunid)
    {
        global $CC_CONFIG, $CC_DBC;
        $cnt = $CC_DBC->getOne("
            SELECT count(*)as cnt
            FROM ".$CC_CONFIG['mdataTable']."
            WHERE gunid=x'$gunid'::bigint
        ");
        if (PEAR::isError($cnt)) {
        	return $cnt;
        }
        return (intval($cnt) > 0);
    }


    /* ============================================= parse and store metadata */
    /**
     * Parse XML metadata
     *
     * @param string $mdata
     * 		local path to metadata XML file or XML string
     * @param string $loc
     * 		location: 'file'|'string'
     * @return array|PEAR_Error
     * 		reference, parse result tree
     */
    public function &parse($mdata='', $loc='file')
    {
        require_once("XmlParser.php");
        return XmlParser::parse($mdata, $loc);
    }


    /**
     * Validate parsed metadata
     *
     * @param array $tree
     * 		parsed tree
     * @return true|PEAR_Error
     */
    private function validate(&$tree)
    {
        global $CC_CONFIG;
        if ($CC_CONFIG['validate'] && !is_null($this->format)) {
            require_once("Validator.php");
            $val = new Validator($this->format, $this->gunid);
            if (PEAR::isError($val)) {
            	return $val;
            }
            $res = $val->validate($tree);
            if (PEAR::isError($res)) {
            	return $res;
            }
        }
        return TRUE;
    }


    /**
     * Validate one metadata value (on insert/update)
     *
     * @param string $parName
     * 		parent element name
     * @param string $category
     * 		qualif. category name
     * @param string $predxml
     * 		'A' | 'T' (attr or tag)
     * @param string $value
     * 		validated element value
     * @return true|PEAR_Error
     */
    private function validateOneValue($parName, $category, $predxml, $value)
    {
        global $CC_CONFIG;
        if ($CC_CONFIG['validate'] && !is_null($this->format)) {
            require_once("Validator.php");
            $val = new Validator($this->format, $this->gunid);
            if (PEAR::isError($val)) {
            	return $val;
            }
            $r = $val->validateOneValue($parName, $category, $predxml, $value);
            if (PEAR::isError($r)) {
            	return $r;
            }
        }
        return TRUE;
    }


    /**
     * Insert parsed metadata into database
     *
     * @param array $tree
     * 		parsed tree
     * @return true|PEAR_Error
     */
    private function storeDoc(&$tree)
    {
        global $CC_CONFIG, $CC_DBC;
        $CC_DBC->query("BEGIN");
        $res = $this->storeNode($tree);
        if (PEAR::isError($res)) {
            $CC_DBC->query("ROLLBACK");
            return $res;
        }
        $res = $CC_DBC->query("COMMIT");
        if (PEAR::isError($res)) {
        	$CC_DBC->query("ROLLBACK");
        	return $res;
        }
        return TRUE;
    }


    /**
     * Process one node of metadata XML for insert or update.
     *
     * @param object $node
     * 		node in tree returned by XmlParser
     * @param int $parid
     *		parent id
     * @param array $nSpaces
     *		array of name spaces definitions
     * @return int
     * 		local metadata record id
     */
    private function storeNode(&$node, $parid=NULL, $nSpaces=array())
    {
        //echo $node->node_name().", ".$node->node_type().", ".$node->prefix().", $parid.\n";
        $nSpaces = array_merge($nSpaces, $node->nSpaces);
        // null parid = root node of metadata tree
        $subjns  = (is_null($parid)? '_G'         : '_I');
        $subject = (is_null($parid)? $this->gunid : $parid);
        $object  = $node->content;
        if (is_null($object) || $object == '') {
            $objns  = '_blank';
            $object = NULL;
        } else {
        	$objns = '_L';
        }
        $id = $this->storeRecord($subjns, $subject,
            $node->ns, $node->name, 'T', $objns, $object);
        // process attributes
        foreach ($node->attrs as $atn => $ato) {
            $this->storeRecord('_I', $id,
                $ato->ns, $ato->name, 'A', '_L', $ato->val);
        }
        // process child nodes
        foreach ($node->children as $ch) {
            $this->storeNode($ch, $id, $nSpaces);
        }
        // process namespace definitions
        foreach ($node->nSpaces as $ns => $uri) {
            $this->storeRecord('_I', $id,
                'xmlns', $ns, 'N', '_L', $uri);
        }
        return $id;
    }


    /**
     * Update object namespace and value of one metadata record
     * identified by metadata record id
     *
     * @param int $mdid
     * 		metadata record id
     * @param string $object
     * 		object value, e.g. title string
     * @param string $objns
     * 		object namespace prefix, have to be defined in file's metadata
     * @return true|PEAR_Error
     */
//    function updateRecord($mdid, $object, $objns='_L')
//    {
//    	$object_sql = is_null($object) ? "NULL" : "'".pg_escape_string($object)."'";
//    	$objns_sql = is_null($objns) ? "NULL" : "'".pg_escape_string($objns)."'";
//        $res = $CC_DBC->query("UPDATE {$this->mdataTable}
//            SET objns = $objns_sql, object = $object_sql
//            WHERE gunid = x'{$this->gunid}'::bigint AND id='$mdid'
//        ");
//        if (PEAR::isError($res)) {
//        	return $res;
//        }
//        return TRUE;
//    }


    /**
     * Insert or update of one metadata record completely
     *
     * @param string $subjns
     * 		subject namespace prefix, have to be defined
     *      in file's metadata (or reserved prefix)
     * @param string $subject
     * 		subject value, e.g. gunid
     * @param string $predns
     * 		predicate namespace prefix, have to be defined
     *      in file's metadata (or reserved prefix)
     * @param string $predicate
     * 		predicate value, e.g. name of DC element
     * @param string $predxml
     * 		'T'|'A'|'N' - XML tag, attribute or NS def.
     * @param string $objns
     * 		object namespace prefix, have to be defined
     *      in file's metadata (or reserved prefix)
     * @param string $object
     * 		object value, e.g. title of song
     * @return int
     * 		new metadata record id
     */
    private function storeRecord($subjns, $subject, $predns, $predicate, $predxml='T',
        $objns=NULL, $object=NULL)
    {
        global $CC_CONFIG, $CC_DBC;
        //echo "$subjns, $subject, $predns, $predicate, $predxml, $objns, $object\n";
        //$predns = strtolower($predns);
        //$predicate = strtolower($predicate);
        $subjns_sql = is_null($subjns) ? "NULL" : "'".pg_escape_string($subjns)."'";
		$subject_sql = is_null($subject) ? "NULL" : "'".pg_escape_string($subject)."'";
		$predns_sql = is_null($predns) ? "NULL" : "'".pg_escape_string($predns)."'";
		$predicate_sql = is_null($predicate) ? "NULL" : "'".pg_escape_string($predicate)."'";
		$objns_sql = is_null($objns) ? "NULL" : "'".pg_escape_string($objns)."'";
		$object_sql = is_null($object) ? "NULL" : "'".pg_escape_string($object)."'";
        $id = $CC_DBC->nextId($CC_CONFIG['mdataTable']."_id_seq");
        if (PEAR::isError($id)) {
        	return $id;
        }
        $res = $CC_DBC->query("
            INSERT INTO ".$CC_CONFIG['mdataTable']."
                (id , gunid, subjns, subject,
                    predns, predicate, predxml,
                    objns, object
                )
            VALUES
                ($id, x'{$this->gunid}'::bigint, $subjns_sql, $subject_sql,
                    $predns_sql, $predicate_sql, '$predxml',
                    $objns_sql, $object_sql
                )
        ");
        if (PEAR::isError($res)) {
        	return $res;
        }
        return $id;
    }


    /**
     * Delete metadata record recursively
     *
     * @param int $mid
     * 		local metadata record id
     * @return boolean
     */
    private function deleteRecord($mid)
    {
        global $CC_CONFIG, $CC_DBC;
        $sql = "SELECT id FROM ".$CC_CONFIG['mdataTable']."
            	WHERE subjns='_I' AND subject='{$mid}' AND
                gunid=x'{$this->gunid}'::bigint";
        $rh = $CC_DBC->query($sql);
        if (PEAR::isError($rh)) {
        	return $rh;
        }
        while ($row = $rh->fetchRow()) {
            $r = $this->deleteRecord($row['id']);
            if (PEAR::isError($r)) {
            	return $r;
            }
        }
        $rh->free();
        $sql = "DELETE FROM ".$CC_CONFIG['mdataTable']."
            	WHERE id={$mid} AND
                gunid=x'{$this->gunid}'::bigint";
        $res = $CC_DBC->query($sql);
        if (PEAR::isError($res)) {
        	return $res;
        }
        return TRUE;
    }


    /* =========================================== XML reconstruction from db */
    /**
     * Generate PHP array from metadata database
     *
     * @return array
     * 		array with metadata tree
     */
    public function genPhpArray()
    {
        global $CC_CONFIG, $CC_DBC;
        $res = array();
        $row = $CC_DBC->getRow("
            SELECT * FROM ".$CC_CONFIG['mdataTable']."
            WHERE gunid=x'{$this->gunid}'::bigint
                AND subjns='_G' AND subject='{$this->gunid}'
        ");
        if (PEAR::isError($row)) {
        	return $row;
        }
        if (!is_null($row)) {
            $node = $this->genXMLNode($row, FALSE);
            if (PEAR::isError($node)) {
            	return $node;
            }
            $res = $node;
        }
        return $res;
    }


    /**
     * Generate XML document from metadata database
     *
     * @return string
     * 		string with XML document
     */
    public function genXMLDoc()
    {
        global $CC_CONFIG, $CC_DBC;
        require_once("XML/Util.php");
        $res = XML_Util::getXMLDeclaration("1.0", "UTF-8")."\n";
        $row = $CC_DBC->getRow("
            SELECT * FROM ".$CC_CONFIG['mdataTable']."
            WHERE gunid=x'{$this->gunid}'::bigint
                AND subjns='_G' AND subject='{$this->gunid}'
        ");
        if (PEAR::isError($row)) {
        	return $row;
        }
        if (is_null($row)) {
            $node = XML_Util::createTagFromArray(array(
                'localpart'=>'none'
            ));
        } else {
            $node = $this->genXMLNode($row);
            if (PEAR::isError($node)) {
            	return $node;
            }
        }
        $res .= $node;
        require_once("XML/Beautifier.php");
        $fmt = new XML_Beautifier();
        return $res;
    }


    /**
     * Generate XML element from database
     *
     * @param array $row
     * 		hash with metadata record fields
     * @param boolean $genXML
     * 		if TRUE generate XML else PHP array
     * @return string
     * 		XML serialization of node
     */
    private function genXMLNode($row, $genXML=TRUE)
    {
        if (DEBUG) {
        	echo "genXMLNode:\n";
        }
        if (DEBUG) {
        	var_dump($row);
        }
        extract($row);
        $arr = $this->getSubrows($id, $genXML);
        if (PEAR::isError($arr)) {
        	return $arr;
        }
        if (DEBUG) {
        	var_dump($arr);
        }
        extract($arr);  // attr, children, nSpaces
        if ($genXML) {
            $node = XML_Util::createTagFromArray(array(
                'namespace' => $predns,
                'localPart' => $predicate,
                'attributes'=> $attrs,
                'content'   => (is_null($object) ? $children : htmlspecialchars($object)),
            ), FALSE);
//                'content'   => (is_null($object) ? $children : htmlentities($object, ENT_COMPAT, 'UTF-8')),
        } else {
            $node = array_merge(
                array(
                    'elementname'    => ($predns ? "$predns:" : '').$predicate,
                    'content' => $object,
                    'children'=> $children,
                ), $arr
            );
        }
        return $node;
    }


    /**
     * Return values of attributes, child nodes and namespaces for
     * one metadata record
     *
     * @param int $parid
     * 		local id of parent metadata record
     * @param boolean $genXML
     * 		if TRUE generate XML else PHP array for children
     * @return array
     * 		hash with three fields:
     *      - attr hash, attributes
     *      - children array, child nodes
     *      - nSpaces hash, namespace definitions
     */
    private function getSubrows($parid, $genXML=TRUE)
    {
        global $CC_CONFIG, $CC_DBC;
        if (DEBUG) {
        	echo" getSubrows:\n";
        }
        $qh = $CC_DBC->query($q = "
            SELECT
                id, predxml, predns, predicate, objns, object
            FROM ".$CC_CONFIG['mdataTable']."
            WHERE
                subjns='_I' AND subject='$parid' AND
                gunid=x'{$this->gunid}'::bigint
            ORDER BY id
        ");
        if (PEAR::isError($qh)) {
        	return $qh;
        }
        $attrs = array();
        $children = array();
        $nSpaces = array();
        if (DEBUG) {
        	echo "  #=".$qh->numRows()."\n$q\n";
        }
        while ($row = $qh->fetchRow()) {
            if (DEBUG) {
            	var_dump($row);
            }
            extract($row);
            switch ($predxml) {
	            case "N":
	                $nSpaces["$predicate"] = $object;
	//                $nSpaces["$predicate"] = htmlentities($object, ENT_COMPAT, 'UTF-8');
	            case "A":
	                $sep=':';
	                if($predns=='' || $predicate=='') $sep='';
	                $attrs["{$predns}{$sep}{$predicate}"] = $object;
	//                $attrs["{$predns}{$sep}{$predicate}"] = htmlentities($object, ENT_COMPAT, 'UTF-8');
	                break;
	            case "T":
	                $children[] = $this->genXMLNode($row, $genXML);
	                break;
	            default:
	                return PEAR::raiseError(
	                    "MetaData::getSubrows: unknown predxml ($predxml)");
            } // switch
        }
        $qh->free();
        if ($genXML) {
        	$children   = join(" ", $children);
        }
        return compact('attrs', 'children', 'nSpaces');
    }


    /* ========================================================= test methods */
    /**
     * Test method
     *
     * @return true|PEAR_Error
     */
    public function test()
    {
        $res = $this->replace(getcwd().'/mdata2.xml');
        if (PEAR::isError($res)) {
        	return $res;
        }
        $res = $this->getMetadata();
        if (PEAR::isError($res)) {
        	return $res;
        }
        return TRUE;
    }

} // class MetaData
?>