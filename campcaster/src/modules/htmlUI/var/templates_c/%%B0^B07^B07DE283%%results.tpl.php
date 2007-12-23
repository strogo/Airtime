<?php /* Smarty version 2.6.6, created on 2007-12-21 23:20:13
         compiled from library/results.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('function', 'cycle', 'library/results.tpl', 25, false),array('function', 'niceTime', 'library/results.tpl', 37, false),array('modifier', 'truncate', 'library/results.tpl', 29, false),array('modifier', 'lower', 'library/results.tpl', 39, false),array('modifier', 'capitalize', 'library/results.tpl', 39, false),)), $this); ?>
<?php $this->assign('_PL_activeId', $this->_tpl_vars['PL']->getActiveId());  if ($this->_tpl_vars['isHub']): ?>
	<?php $this->assign('action_handler', "library/hub_actionhandler.tpl");  else: ?>
	<?php $this->assign('action_handler', "library/actionhandler.tpl");  endif; ?>

<?php if ($this->_tpl_vars['_results']['cnt'] > 0): ?>
    <form name="SEARCHRESULTS">
    <div class="head" style="width:600px; height: 21px;">&nbsp;</div>
    <div class="container_table" style="width: 600px; height: auto;">

            <table style="width: 600px;">
                <tr class="blue_head">
                    <td style="width: 20px"><input type="checkbox" name="all" onClick="collector_switchAll('SEARCHRESULTS')"></td>
                    <td style="width: 200px;"><a href="#" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=<?php echo $this->_tpl_vars['_act_prefix']; ?>
.reorder&by=dc:title', 'order');" id="blue_head">##Title##</a></td>
                    <td style="width: 195px"><a href="#" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=<?php echo $this->_tpl_vars['_act_prefix']; ?>
.reorder&by=dc:creator', 'order');" id="blue_head">##Creator##</a></td>
                    <td style="width: 195px"><a href="#" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=<?php echo $this->_tpl_vars['_act_prefix']; ?>
.reorder&by=dc:source', 'order');" id="blue_head">##Album##</a></td>
                    <td style="width: 25px"><a href="#" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=<?php echo $this->_tpl_vars['_act_prefix']; ?>
.reorder&by=ls:track_num', 'order');" id="blue_head">##Track##</a></td>
                    <td><a href="#"  onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=<?php echo $this->_tpl_vars['_act_prefix']; ?>
.reorder&by=dcterms:extent', 'order');" id="blue_head">##Duration##</a></td>
                    <td style="width: 41px; border: 0; text-align: center">##Type##</td>
                </tr>
                <?php if (count($_from = (array)$this->_tpl_vars['_results']['items'])):
    foreach ($_from as $this->_tpl_vars['i']):
?>
            <!-- start item -->
                <tr class="background-color: <?php echo smarty_function_cycle(array('values' => 'blue1, blue2'), $this);?>
">
                    <td><input type="checkbox" class="checkbox" name="<?php echo $this->_tpl_vars['i']['id']; ?>
"/></td>
                    <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => $this->_tpl_vars['action_handler'], 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?> style="cursor: pointer">
                        <?php if ($this->_tpl_vars['PLAYLIST']['id'] == $this->_tpl_vars['i']['id']): ?>
                            <b><?php echo ((is_array($_tmp=$this->_tpl_vars['i']['title'])) ? $this->_run_mod_handler('truncate', true, $_tmp, 30, "...", true) : smarty_modifier_truncate($_tmp, 30, "...", true)); ?>
</b>
                        <?php else: ?>
                            <?php echo ((is_array($_tmp=$this->_tpl_vars['i']['title'])) ? $this->_run_mod_handler('truncate', true, $_tmp, 30, "...", true) : smarty_modifier_truncate($_tmp, 30, "...", true)); ?>

                        <?php endif; ?>
                    </td>
                    <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => $this->_tpl_vars['action_handler'], 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?> style="cursor: pointer"><?php echo $this->_tpl_vars['i']['creator']; ?>
</td>
                    <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => $this->_tpl_vars['action_handler'], 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?> style="cursor: pointer"><?php echo $this->_tpl_vars['i']['source']; ?>
</td>
                    <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => $this->_tpl_vars['action_handler'], 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?> style="cursor: pointer" align="center"><?php echo $this->_tpl_vars['i']['track_num']; ?>
</td>
                    <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => $this->_tpl_vars['action_handler'], 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?> style="text-align: right; cursor: pointer"><?php $this->assign('_duration', $this->_tpl_vars['i']['duration']);  echo S_niceTime(array('in' => $this->_tpl_vars['_duration']), $this);?>
</td>
                    <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => $this->_tpl_vars['action_handler'], 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?> style="border: 0; text-align: center; cursor: pointer">
                        <img src="img/<?php echo ((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('lower', true, $_tmp) : smarty_modifier_lower($_tmp)); ?>
.png" border="0" alt="<?php echo ((is_array($_tmp=((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('lower', true, $_tmp) : smarty_modifier_lower($_tmp)))) ? $this->_run_mod_handler('capitalize', true, $_tmp) : smarty_modifier_capitalize($_tmp)); ?>
"  />
                    </td>
                </tr>
            <!-- end item -->
                <?php endforeach; unset($_from); endif; ?>
            </table>

    </div>
    <div class="footer" style="width: 595px;">

         <div class="counter">
            
            <?php if (count($_from = (array)$this->_tpl_vars['_results']['pagination'])):
    foreach ($_from as $this->_tpl_vars['k'] => $this->_tpl_vars['p']):
?>
                <?php if ($this->_tpl_vars['k'] != $this->_tpl_vars['_results']['page']+1): ?>
                    <a href="#" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=<?php echo $this->_tpl_vars['_act_prefix']; ?>
.setOffset&page=<?php echo $this->_tpl_vars['k']; ?>
', 'pager')" id="blue_head" class="pagination_number"><span class="pagination_number"><?php echo $this->_tpl_vars['p']; ?>
</span></a>
                <?php else: ?>
                    <span class="pagination_number"><?php echo $this->_tpl_vars['p']; ?>
</span>
                <?php endif; ?>
            <?php endforeach; unset($_from); endif; ?>

                        &nbsp;&nbsp;
            ##Range##:&nbsp;<?php echo $this->_tpl_vars['_criteria']['offset']+1; ?>
-<?php if (( $this->_tpl_vars['_criteria']['offset']+$this->_tpl_vars['_criteria']['limit'] ) > $this->_tpl_vars['_results']['cnt']):  echo $this->_tpl_vars['_results']['cnt'];  else:  echo $this->_tpl_vars['_criteria']['offset']+$this->_tpl_vars['_criteria']['limit'];  endif; ?>&nbsp;
            ##Count##:&nbsp;<?php echo $this->_tpl_vars['_results']['cnt']; ?>
&nbsp;
            
         </div>

         <select name="SEARCHRESULTS_multiaction" onChange="collector_submit('SEARCHRESULTS', this.value)">
                <option>##Multiple Action:##</option>
                <option value="SP.addItem">##Add files to ScratchPad##</option>
                <?php if ($this->_tpl_vars['_PL_activeId']): ?>
                    <option value="PL.addItem">##Add files to open Playlist##</option>
                <?php else: ?>
                    <option value="PL.create">##New Playlist using this files##</option>
                <?php endif; ?>
                <option value="delete">##Delete files##</option>
         </select>
         <script type="text/javascript">
            // due to browser remembers filled form fields, here this is unwanted behavior
            document.forms['SEARCHRESULTS'].elements['SEARCHRESULTS_multiaction'].options[0].selected = true;
            document.forms['SEARCHRESULTS'].elements['all'].checked = false;
            collector_switchAll('SEARCHRESULTS')
         </script>

    </div>
   </form>

<?php else: ?>
    ##No match found.##
<?php endif; ?>

<?php $this->assign('_PL_activeId', null);  $this->assign('_duration', null); ?>