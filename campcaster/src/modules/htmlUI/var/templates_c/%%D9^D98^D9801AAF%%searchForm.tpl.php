<?php /* Smarty version 2.6.6, created on 2007-12-21 23:20:13
         compiled from library/searchForm.tpl */ ?>
<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "library/searchForm.js.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
    <?php if (count($_from = (array)$this->_tpl_vars['_form'])):
    foreach ($_from as $this->_tpl_vars['dynform']):
?>
        <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "sub/dynForm_plain.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
    <!-- end search -->
    <?php endforeach; unset($_from); endif; ?>