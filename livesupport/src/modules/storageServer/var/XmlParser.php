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
require_once "XML/Util.php";

/* ================================================================== Element */
/**
 *  Object representation of one XML element
 *
 *  @see MetaData
 */
class XmlElement {
    /**
     *  Namespace prefix
     */
    var $ns;
    /**
     *  Element name
     */
    var $name;
    /**
     *  Attributes
     */
    var $attrs      = array();
    /**
     *  Namespace definitions
     */
    var $nSpaces    = array();
    /**
     *  Child nodes
     */
    var $children   = array();
    /**
     *  Text content of element
     */
    var $content    = '';

    /**
     *  Constructor
     *
     *  @param fullname string, fully qualified name of element
     *  @param attrs hash of attributes
     *  @param nSpaces hash of namespace definitions
     *  @param children hash of child nodes
     *  @return this
     */
    function XmlElement($fullname, $attrs, $nSpaces=array(), $children=array())
    {
        $a = XML_Util::splitQualifiedName($fullname);
        $this->ns   = $a['namespace'];
        $this->name = $a['localPart'];
        $this->attrs    = $attrs;
        $this->nSpaces  = $nSpaces;
        $this->children = $children;
    }
}

/* ================================================================ Attribute */
/**
 *  Object representation of one XML attribute
 *
 *  @see MetaData
 */
class XmlAttrib {
    /**
     *  Namespace prefix
     */
    var $ns;
    /**
     *  Attribute name
     */
    var $name;
    /**
     *  Attribute value
     */
    var $val;
    /**
     *  Constructor
     *
     *  @param atns string, namespace prefix
     *  @param atnm string, attribute name
     *  @param atv string, attribute value
     *  @return this
     */
    function XmlAttrib($atns, $atnm, $atv)
    {
        $this->ns   = $atns;
        $this->name = $atnm;
        $this->val  = $atv;
    }
}

/* =================================================================== Parser */
/**
 *  XML parser object encapsulation
 *
 *  @see MetaData
 */
class XmlParser {
    /**
     *  Tree of nodes
     */
    var $tree  = NULL;
    /**
     *  Parse stack
     */
    var $stack = array();
    /**
     *  Error structure
     */
    var $err = array(FALSE, '');
    /**
     *  Constructor
     *
     *  @param data string, XML string to be parsed
     *  @return this
     */
    function XmlParser($data){
        $xml_parser = xml_parser_create('UTF-8');
        xml_parser_set_option($xml_parser, XML_OPTION_CASE_FOLDING, FALSE);
        xml_set_object($xml_parser, $this);
        xml_set_element_handler($xml_parser, "startTag", "endTag");
        xml_set_character_data_handler($xml_parser, 'characterData');
        $res = xml_parse($xml_parser, $data, TRUE);
        if(!$res) {
            $this->err = array(TRUE,
                sprintf("XML error: %s at line %d\n",
                    xml_error_string(xml_get_error_code($xml_parser)),
                    xml_get_current_line_number($xml_parser)
                )
            );
//            var_dump($data);
        }
        xml_parser_free($xml_parser);
    }

    /**
     *  Start tag handler
     *
     *  @param parser resource, reference to parser resource
     *  @param fullname string element name
     *  @param attrs array of attributes
     *  @return none
     */
    function startTag($parser, $fullname, $attrs) {
        $nSpaces = array();
        foreach($attrs as $atn=>$atv){
            $a    = XML_Util::splitQualifiedName($atn);
            $atns = $a['namespace'];
            $atnm = $a['localPart'];
            unset($attrs[$atn]);
            if($atns == 'xmlns') $nSpaces[$atnm] = $atv;
            else if ($atns == NULL && $atnm == 'xmlns'){
                $nSpaces[''] = $atv;
            }else{
                $attrs[$atn] = new XmlAttrib($atns, $atnm, $atv);
            }
        }
        $el = new XmlElement($fullname, $attrs, $nSpaces);
        array_push($this->stack, $el);
    }

    /**
     *  End tag handler
     *
     *  @param parser resource, reference to parser resource
     *  @param fullname string element name
     *  @return none
     */
    function endTag($parser, $fullname) {
        $cnt = count($this->stack);
        if($cnt>1){
            $this->stack[$cnt-2]->children[] = $this->stack[$cnt-1];
            $lastEl = array_pop($this->stack);
        }else{
            $this->tree = $this->stack[0];
        }
    }

    /**
     *  Character data handler
     *
     *  @param parser resource, reference to parser resource
     *  @param data string
     *  @return none
     */
    function characterData($parser, $data) {
        $cnt = count($this->stack);
        if(trim($data)!=''){ $this->stack[$cnt-1]->content .= $data; }
    }

    /**
     *  Default handler
     *
     *  @param parser resource, reference to parser resource
     *  @param data string
     *  @return none
     */
    function defaultHandler($parser, $data)
    {
        $cnt = count($this->stack);
        //if(substr($data, 0, 1) == "&" && substr($data, -1, 1) == ";"){
        //    $this->stack[$cnt-1]->content .= trim($data);
        //}else{
            $this->stack[$cnt-1]->content .= "*** $data ***";
        //}
    }

    /**
     *  Return result tree
     *
     *  @return hash, tree structure
     */
    function getTree(){
        return $this->tree;
    }

    /**
     *  Return error string
     *
     *  @return boolean if error occured
     */
    function isError(){
        return $this->err[0];
    }

    /**
     *  Return error string
     *
     *  @return string, error message
     */
    function getError(){
        return $this->err[1];
    }

    /* ----------------------------------- auxiliary methos for serialization */
    /**
     *  Serialize metadata of one file
     *
     *  @return string, serialized XML
     */
    function serialize(){
        $res  = '<?xml version="1.0" encoding="utf-8"?>';
        $res .= $this->serializeEl($this->tree);
        $res .= "\n";
        return $res;
    }

    /**
     *  Serialize one metadata element
     *
     *  @param el object, element object
     *  @param lvl int, level for indentation
     *  @return string, serialized XML
     */
    function serializeEl($el, $lvl=0){
        $ind    = str_repeat(" ", $lvl);
        $elNs   = $el->ns;
        $elName = $el->name;
        $attrs  = XML_Util::attributesToString($el->attrs);
        $fullName   = ($elNs=='' ? '' : "$elNs:")."$elName";
        $res  = "\n{$ind}<{$fullName}{$attrs}>";
        $haveCh = (count($el->children)>0);
        foreach($el->children as $ch){
            $res .= $this->serializeEl($ch, $lvl+1);
        }
        $res .= XML_Util::replaceEntities("{$el->content}");
        if($haveCh) $res .= "\n{$ind}";
        $res .= "</{$fullName}>";
        return $res;
    }

    /* -------------------------------------------------------- debug methods */
    /**
     *  Debug dump of tree
     *
     *  @return hash, tree structure
     */
    function dump(){
        var_dump($this->tree);
    }

}
?>