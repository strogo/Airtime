<?php /* Smarty version 2.6.6, created on 2007-12-21 23:20:22
         compiled from library/browseForm.tpl */ ?>
<?php $this->assign('_style_select', " style='width: 180px;'");  $this->assign('_style_textarea', " class='area_browse'"); ?>

                <table style="margin:0; padding:0">
                <tr>
                <td style="border:0">
                    <?php if (count($_from = (array)$this->_tpl_vars['browseForm']['col1'])):
    foreach ($_from as $this->_tpl_vars['dynform']):
?>
                        <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "library/dynForm_browse.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
                    <?php endforeach; unset($_from); endif; ?>	
                </td><td style="border:0">
                    <?php if (count($_from = (array)$this->_tpl_vars['browseForm']['col2'])):
    foreach ($_from as $this->_tpl_vars['dynform']):
?>
                        <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "library/dynForm_browse.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
                    <?php endforeach; unset($_from); endif; ?>
                </td><td style="border:0">
                    <?php if (count($_from = (array)$this->_tpl_vars['browseForm']['col3'])):
    foreach ($_from as $this->_tpl_vars['dynform']):
?>
                        <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "library/dynForm_browse.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
                    <?php endforeach; unset($_from); endif; ?>
                </td>
                </tr>
                </table>
                    <?php if (count($_from = (array)$this->_tpl_vars['browseForm']['global'])):
    foreach ($_from as $this->_tpl_vars['dynform']):
?>
                        <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "sub/dynForm_plain.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
                    <?php endforeach; unset($_from); endif; ?>		

		<!-- end browsing -->
<?php $this->assign('_style_select', "");  $this->assign('_style_textarea', ""); ?>