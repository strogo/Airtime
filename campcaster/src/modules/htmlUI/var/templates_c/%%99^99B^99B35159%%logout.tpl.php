<?php /* Smarty version 2.6.6, created on 2007-12-21 23:29:05
         compiled from popup/logout.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('function', 'tra', 'popup/logout.tpl', 6, false),)), $this); ?>
<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "popup/header.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>

<center>
##Are you sure you want to logout?##
<br><br>
<input type="button" class="button" value="<?php echo S_tra(array('0' => 'Cancel'), $this);?>
" onclick="javascript: window.close()">
<input type="button" class="button" value="<?php echo S_tra(array('0' => 'OK'), $this);?>
" onclick="javascript: location.href='<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=<?php echo $this->_tpl_vars['logouttype']; ?>
'">&nbsp;
</center>


</body>
</html>