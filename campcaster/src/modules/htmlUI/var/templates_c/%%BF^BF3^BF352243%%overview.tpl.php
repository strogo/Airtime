<?php /* Smarty version 2.6.6, created on 2007-12-21 23:13:55
         compiled from subjects/overview.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'lower', 'subjects/overview.tpl', 14, false),array('modifier', 'escape', 'subjects/overview.tpl', 18, false),array('modifier', 'capitalize', 'subjects/overview.tpl', 28, false),array('function', 'cycle', 'subjects/overview.tpl', 15, false),)), $this); ?>
<div class="container_elements" style="width: 607px;">
    <h1>##User/Group Management##</h1>

    <div class="container_table" style="width: 594px;">
        <table style="width: 574px;">

            <tr class="blue_head">
                <td>##Login##</td>
                <td style="width: 40px; text-align: center">##Members##</td>
                <td style="width: 40px; border: 0; text-align: center">##Type##</td>
            </tr>

        <?php if (count($_from = (array)$this->_tpl_vars['SUBJECTS']->getSubjectsWCnt())):
    foreach ($_from as $this->_tpl_vars['i']):
?>            
            <?php if (((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('lower', true, $_tmp) : smarty_modifier_lower($_tmp)) == 'g'):  $this->assign('_type', 'group');  else:  $this->assign('_type', 'user');  endif; ?>
            <tr class="<?php echo smarty_function_cycle(array('values' => 'blue1, blue2'), $this);?>
"
                style="cursor: pointer"
                                <?php if ($this->_tpl_vars['i']['login'] != 'scheduler'): ?>onClick="return contextmenu('id=<?php echo $this->_tpl_vars['i']['id']; ?>
&login=<?php echo ((is_array($_tmp=$this->_tpl_vars['i']['login'])) ? $this->_run_mod_handler('escape', true, $_tmp, 'url') : smarty_modifier_escape($_tmp, 'url')); ?>
', <?php if (((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('lower', true, $_tmp) : smarty_modifier_lower($_tmp)) == 'g'): ?>'SUBJECTS.manageGroupMember', <?php else: ?>'SUBJECTS.chgPasswd', <?php endif; ?> 'SUBJECTS.removeSubj')"<?php endif; ?>
            >
                <td <?php if ($this->_tpl_vars['i']['login'] == 'scheduler'): ?>style="color: grey"<?php endif; ?>><?php echo $this->_tpl_vars['i']['login']; ?>
</td>
                <td style="width: 30px; text-align: center;">
                    <?php if (((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('lower', true, $_tmp) : smarty_modifier_lower($_tmp)) == 'g'): ?>
                        <?php echo $this->_tpl_vars['i']['cnt']; ?>

                    <?php else: ?>
                        -
                    <?php endif; ?>
                </td>
                <td style="border: 0; text-align: center;"><img src="img/<?php echo $this->_tpl_vars['_type']; ?>
.png" border="0" alt="<?php echo ((is_array($_tmp=$this->_tpl_vars['_type'])) ? $this->_run_mod_handler('capitalize', true, $_tmp) : smarty_modifier_capitalize($_tmp)); ?>
" /></td>

            </tr>
        <?php endforeach; unset($_from); endif; ?>

        </table>

        <br>
        <div class="container_button">
            <input type="button" class="button" value="##Add User##"  onclick="location.href='<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=SUBJECTS.addUser'">
            <input type="button" class="button" value="##Add Group##" onclick="location.href='<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=SUBJECTS.addGroup'">
        </div>

    </div>
</div>

<?php $this->assign('_type', null); ?>