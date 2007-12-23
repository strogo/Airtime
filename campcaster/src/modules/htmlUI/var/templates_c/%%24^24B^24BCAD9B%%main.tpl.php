<?php /* Smarty version 2.6.6, created on 2007-12-21 23:13:49
         compiled from scratchpad/main.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('function', 'cycle', 'scratchpad/main.tpl', 24, false),array('function', 'niceTime', 'scratchpad/main.tpl', 41, false),array('modifier', 'lower', 'scratchpad/main.tpl', 27, false),array('modifier', 'truncate', 'scratchpad/main.tpl', 34, false),array('modifier', 'capitalize', 'scratchpad/main.tpl', 45, false),)), $this); ?>
<?php $this->assign('_PL_activeId', $this->_tpl_vars['PL']->getActiveId());  $this->assign('SCRATCHPAD', $this->_tpl_vars['SCRATCHPAD']->get()); ?>

<!-- start scratch pad -->
<form name="SP">
    <div class="container_elements">
        <h1>##ScratchPad##</h1>
        <div class="head" style="width:255px; height: 21px;">&nbsp;</div>
        <div class="container_table" style="width:275px;">
            <table style="width:255px;">

            <!-- start table header -->
                <tr class="blue_head">
                    <td style="width: 1px"><input type="checkbox" name="all" onClick="collector_switchAll('SP')"></td>
                    <td style="width: *"><a href="#" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=SP.reorder&by=title', 'order');" id="blue_head">##Title##</a></td>
                    <td style="width: 1px"><a href="#" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=SP.reorder&by=title', 'order');" id="blue_head">##Duration##</td>
                    <td style="width: 1px; border: 0; text-align: center"><a href="#" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=SP.reorder&by=title', 'order');" id="blue_head">##Type##</td>
                </tr>
            <!-- end table header -->

    <?php if (count ( $this->_tpl_vars['SCRATCHPAD'] ) >= 1): ?>
        <?php if (count($_from = (array)$this->_tpl_vars['SCRATCHPAD'])):
    foreach ($_from as $this->_tpl_vars['i']):
?>
                <!-- start item -->
                <tr class="<?php echo smarty_function_cycle(array('values' => 'blue1, blue2'), $this);?>
">
                    <td><input type="checkbox" class="checkbox" name="<?php echo $this->_tpl_vars['i']['id']; ?>
"/></td>
                    <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "scratchpad/actionhandler.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?> style="cursor: pointer">
                        <?php if (((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('lower', true, $_tmp) : smarty_modifier_lower($_tmp)) == 'playlist'): ?>

                            <?php if ($this->_tpl_vars['_PL_activeId'] == $this->_tpl_vars['i']['id']): ?>
                                <div style="font-weight: bold; cursor: pointer">
                            <?php else: ?>
                                <div style="cursor: pointer">
                            <?php endif; ?>
                                <?php echo ((is_array($_tmp=$this->_tpl_vars['i']['title'])) ? $this->_run_mod_handler('truncate', true, $_tmp, 14, "...", true) : smarty_modifier_truncate($_tmp, 14, "...", true)); ?>

                                </div>
                        <?php else: ?>
                            <?php echo ((is_array($_tmp=$this->_tpl_vars['i']['title'])) ? $this->_run_mod_handler('truncate', true, $_tmp, 14, "...", true) : smarty_modifier_truncate($_tmp, 14, "...", true)); ?>

                        <?php endif; ?>                      </td>
                    <?php $this->assign('_duration', $this->_tpl_vars['i']['duration']); ?>
                    <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "scratchpad/actionhandler.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?> style="text-align: right; cursor: pointer"><?php echo S_niceTime(array('in' => $this->_tpl_vars['_duration']), $this);?>
</td>
                    <td <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "scratchpad/actionhandler.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?> style="border: 0; text-align: center; cursor: pointer">
                        <?php if ($this->_tpl_vars['PL']->isAvailable($this->_tpl_vars['i']['id']) == false): ?>
                            <div align="left"><img src="img/ico_lock.png">
                            <img src="img/<?php echo $this->_tpl_vars['i']['type']; ?>
.png" border="0" alt="<?php echo ((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('capitalize', true, $_tmp) : smarty_modifier_capitalize($_tmp)); ?>
"  /></div>
                        <?php else: ?>
                        <img src="img/<?php echo $this->_tpl_vars['i']['type']; ?>
.png" border="0" alt="<?php echo ((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('capitalize', true, $_tmp) : smarty_modifier_capitalize($_tmp)); ?>
"  /> <?php endif; ?>
                    </td>
                </tr>
                <!-- end item -->
        <?php endforeach; unset($_from); endif; ?>
    <?php else: ?>
        <tr class="blue1">
            <td style="border: 0" colspan="4" align="center">##empty##</td>
        </tr>
    <?php endif; ?>
            </table>
        </div>

        <div class="footer" style="width:250px;">
            <select name="SP_multiaction" onChange="collector_submit('SP', this.value)">
                <option>##Multiple Action:##</option>
                <option value="SP.removeItem">##Remove files##</option>
                <?php if ($this->_tpl_vars['_PL_activeId']): ?>
                    <option value="PL.addItem">##Add files to open Playlist##</option>
                <?php else: ?>
                    <option value="PL.create">##New playlist using this files##</option>
                <?php endif; ?>
            </select>
            <script type="text/javascript">
                // due to browser remembers filled form fields, here this is unwanted behavior
                document.forms['SP'].elements['SP_multiaction'].options[0].selected = true;
                document.forms['SP'].elements['all'].checked = false;
                collector_switchAll('SP');
            </script>
            <a href="#" onClick="collector_clearAll('SP', 'SP.removeItem')" id="blue_head">##Clear##</a>
        </div>

    </div>
</form>
<!-- end scratch pad -->

<?php $this->assign('_PL_activeId', null);  $this->assign('_duration', null); ?>