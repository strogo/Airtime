<?php /* Smarty version 2.6.6, created on 2007-12-21 23:14:05
         compiled from subjects/addSubjForm.tpl */ ?>
<?php if ($this->_tpl_vars['act'] == "SUBJECTS.addUser"): ?>
    <?php $this->assign('dynform', $this->_tpl_vars['SUBJECTS']->getAddSubjForm('addUser'));  endif; ?>

<?php if ($this->_tpl_vars['act'] == "SUBJECTS.addGroup"): ?>
    <?php $this->assign('dynform', $this->_tpl_vars['SUBJECTS']->getAddSubjForm('addGroup'));  endif; ?>

<div class="container_elements" style="width: 607px;">
    <?php if ($this->_tpl_vars['act'] == 'SUBJECTS.addUser'): ?>
        <h1>##Add User##</h1>
    <?php else: ?>
        <h1>##Add Group##</h1>
    <?php endif; ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "sub/dynForm_plain.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
</div>

