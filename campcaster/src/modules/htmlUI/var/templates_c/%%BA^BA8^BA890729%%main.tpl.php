<?php /* Smarty version 2.6.6, created on 2007-12-21 23:13:55
         compiled from subjects/main.tpl */ ?>
<div class="content">

<?php if ($this->_tpl_vars['act'] == 'SUBJECTS'): ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "subjects/overview.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
  endif; ?>

<?php if ($this->_tpl_vars['act'] == "SUBJECTS.addUser" || $this->_tpl_vars['act'] == "SUBJECTS.addGroup"): ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "subjects/addSubjForm.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
  endif; ?>

<?php if ($this->_tpl_vars['act'] == "SUBJECTS.manageGroupMember"): ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "subjects/manageGroupMember.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
  endif; ?>

<?php if ($this->_tpl_vars['act'] == "SUBJECTS.chgPasswd"): ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "subjects/chgPasswd.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
  endif; ?>
</div>