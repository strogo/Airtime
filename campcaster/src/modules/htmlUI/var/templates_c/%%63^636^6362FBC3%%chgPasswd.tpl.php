<?php /* Smarty version 2.6.6, created on 2007-12-21 23:16:00
         compiled from subjects/chgPasswd.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('function', 'tra', 'subjects/chgPasswd.tpl', 5, false),)), $this); ?>
<?php $this->assign('subjectName', $this->_tpl_vars['SUBJECTS']->getSubjectName($this->_tpl_vars['_REQUEST']['id']));  $this->assign('dynform', $this->_tpl_vars['SUBJECTS']->getChgPasswdForm($this->_tpl_vars['subjectName'])); ?>

<div class="container_elements" style="width: 607px;">
     <h1><?php echo S_tra(array('str' => 'Change password for: $1','1' => $this->_tpl_vars['subjectName']), $this);?>
</h1>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "sub/dynForm_plain.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
</div>