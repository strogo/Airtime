<?php
/*
 * Smarty plugin
 * -------------------------------------------------------------
 * File:     postfilter.localizer.php
 * Type:     postfilter
 * Name:     localizer
 * Version:  1.0
 * Date:     Feb, 2005
 * Purpose:  Replace string to translate with translations
 * Install:  Drop into the plugin directory, call
 *           $smarty->load_filter('post','localizer');
 *           from application.
 * Author:   Media Development Loan Fund
 * -------------------------------------------------------------
 */

 ## format: ##phrase to translate##

 function smarty_tra($matches)
 {
    foreach ($matches as $match) {
        #$key = substr($match, 2, strpos(substr($match, 2), '#'));
        return tra(preg_replace('/##/', '', $match));
    }
 }

 function smarty_outputfilter_localizer($compiled, &$smarty)
 {     
    $pattern = '/##.*##/U';
    return preg_replace_callback($pattern, 'smarty_tra', $compiled);
 }
?>
