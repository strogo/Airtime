<?php /* Smarty version 2.6.6, created on 2007-12-21 23:30:06
         compiled from playlist/editor.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('function', 'getHour', 'playlist/editor.tpl', 6, false),array('function', 'getMinute', 'playlist/editor.tpl', 6, false),array('function', 'getSecond', 'playlist/editor.tpl', 6, false),array('function', 'cycle', 'playlist/editor.tpl', 34, false),array('function', 'niceTime', 'playlist/editor.tpl', 38, false),array('modifier', 'string_format', 'playlist/editor.tpl', 32, false),array('modifier', 'capitalize', 'playlist/editor.tpl', 42, false),)), $this); ?>
<!-- start playlist editor -->
    <div class="container_elements" style="width: 607px;">

                <div style="width: 574px;">
                    <div style="float: left;"><h1>##Playlist Editor## </h1></div>
                    <div style="float: right;"><h1><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=PL.editMetaData" style="color: #666666"><?php echo $this->_tpl_vars['PL']->title; ?>
 &nbsp; <?php echo S_getHour(array('time' => $this->_tpl_vars['PL']->duration), $this);?>
##h##&nbsp;<?php echo S_getMinute(array('time' => $this->_tpl_vars['PL']->duration), $this);?>
##m##&nbsp;<?php echo S_getSecond(array('time' => $this->_tpl_vars['PL']->duration), $this);?>
##s##</a></h1></div>
                </div>

                <div class="head" style="width: 574px;">
                    <div class="left">&nbsp;</div>
                    <div class="right">&nbsp;</div>
                    <div class="clearer">&nbsp;</div>
                </div>
                <div class="container_table" style="width: 594px;">
                    <table style="width: 574px;">
                        <form name="PL">
                    <!-- start repeat after 14 columns -->
                        <tr class="blue_head">
                            <td style="width: 30px"><input type="checkbox" name="all" onClick="collector_switchAll('PL')"></td>
                            <td style="width: 200px">##Title##</td>
                            <td>                     ##Duration##</td>
                            <td style="width: 200px">##Artist##</td>
                            <td style="width: 30px;">##Type##</td>
                            <td style="width: 30px; border: 0">##Move##</td>
                        </tr>
                    <!-- end repeat after 14 columns -->
                    <!-- start item -->
                    <?php if (count($_from = (array)$this->_tpl_vars['PL']->getFlat($this->_tpl_vars['PL']->activeId))):
    foreach ($_from as $this->_tpl_vars['pos'] => $this->_tpl_vars['i']):
?>
                        <!-- fade information -->
                        <tr onClick="return contextmenu('<?php echo $this->_tpl_vars['i']['attrs']['id']; ?>
', <?php if ($this->_tpl_vars['i']['firstInList'] == 1): ?>'PL.changeFadeIn'<?php else: ?>'PL.changeTransition'<?php endif; ?>)" style="background-color: #bbb">
                            <td></td>
                            <td colspan="5" style="border: 0; cursor: pointer">##Fade## <?php echo ((is_array($_tmp=$this->_tpl_vars['i']['fadein_ms'])) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
 ms</td>
                        </tr>
                        <tr class="<?php echo smarty_function_cycle(array('values' => 'blue1, blue2'), $this);?>
">
                            <td><input type="checkbox" class="checkbox" name="<?php echo $this->_tpl_vars['i']['attrs']['id']; ?>
"/></td>
                            <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "playlist/actionhandler.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>><?php echo $this->_tpl_vars['i']['title']; ?>
</td>
                            <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "playlist/actionhandler.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?> style="text-align: right">
                                <?php $this->assign('_duration', $this->_tpl_vars['i']['playlength']);  echo S_niceTime(array('in' => $this->_tpl_vars['_duration']), $this);?>

                            </td>
                            <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "playlist/actionhandler.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>><?php echo $this->_tpl_vars['i']['creator']; ?>
</td>
                            <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "playlist/actionhandler.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>>
                                <img src="img/<?php echo $this->_tpl_vars['i']['type']; ?>
.png" border="0" alt="<?php echo ((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('capitalize', true, $_tmp) : smarty_modifier_capitalize($_tmp)); ?>
"  />
                            </td>
                            <td style="border: 0">
                                <a href="#" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=PL.moveItem&id=<?php echo $this->_tpl_vars['i']['attrs']['id']; ?>
&pos=<?php echo $this->_tpl_vars['pos']-1; ?>
')"><img src="img/bt_top_xsm.png"    alt="##move up##" vspace=1 hspace=1/></a>
                                <a href="#" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=PL.moveItem&id=<?php echo $this->_tpl_vars['i']['attrs']['id']; ?>
&pos=<?php echo $this->_tpl_vars['pos']+1; ?>
')"><img src="img/bt_bottom_xsm.png" alt="##move down##" vspace=1 hspace=1/></a>
                            </td>
                        </tr>
                    <?php endforeach; unset($_from); endif; ?>
                        <?php if (isset ( $this->_tpl_vars['pos'] )): ?>
                        <!-- fade information -->
                        <tr onClick="return contextmenu('<?php echo $this->_tpl_vars['i']['attrs']['id']; ?>
', 'PL.changeFadeOut')" style="background-color: #bbb">
                            <td></td>
                            <td colspan="5" style="border: 0; cursor: pointer">##Fade## <?php echo ((is_array($_tmp=$this->_tpl_vars['i']['fadeout_ms'])) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
 ms</td>
                        </tr>
                        <?php else: ?>
                            <tr class="<?php echo smarty_function_cycle(array('values' => 'blue1, blue2'), $this);?>
">
                                <td style="border: 0" colspan="6" align="center">##No Entry##</td>
                            </tr>
                        <?php endif; ?>
                    <!-- end item -->
                        </form>
                    </table>
                </div>
                <div class="footer" style="width: 569px;">
                    <input type="button" class="button_large" onClick="collector_submit('PL', '0&popup[]=PL.changeAllTransitions', '<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
', 'chgAllTrans', 400, 150)" value="##Change Fades##" />
                    <input type="button" class="button_large" onClick="collector_submit('PL', 'PL.removeItem')"   value="##Remove Selected##" />
                    <input type="button" class="button_large" onClick="collector_clearAll('PL', 'PL.removeItem')" value="##Clear Playlist##" />
                </div>
                <div class="container_button">
                    <input type="button" class="button_large" value="##Save Playlist##"    onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=PL.save')">
                    <input type="button" class="button_large" value="##Revert to Saved##"  onClick="popup('<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?popup[]=PL.confirmRevert',  'PL.revertChanges',  400, 50)">
                    <input type="button" class="button_large" value="##Delete Playlist##"  onClick="popup('<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?popup[]=PL.confirmDelete',  'PL.deleteActive',   400, 50)">
                </div>
                <div class="container_button">
                    <input type="button" class="button_large" value="##Reorder Playlist##" onClick="popup('<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?popup[]=PL.arrangeItems',   'PL.arrangeItems',   533, 600)">
                    <input type="button" class="button_large" value="##Close Playlist##"   onClick="popup('<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?popup[]=PL.confirmRelease', 'PL.confirmRelease', 400, 50)">
                    <input type="button" class="button_large" value="##Description##"      onClick="location.href='<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=PL.editMetaData'">
                </div>
            </div>

<script type="text/javascript">
        document.forms['PL'].elements['all'].checked = false;
        collector_switchAll('PL');
</script>

<?php $this->assign('_duration', null); ?>

<!-- end playlist editor -->