<?php
/**
 * @author $Author: paul $
 * @version  : $Revision: 2335 $
 */

/*------------------------------------------------------------------------------
 *  This (web-callable) script returns group running httpd
 *----------------------------------------------------------------------------*/

 header("Content-type: text/plain");
 $egid = posix_getegid();
 $info = posix_getgrgid($egid);
 echo $info['name'];
?>