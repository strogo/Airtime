<?php /* Smarty version 2.6.6, created on 2007-12-21 23:16:24
         compiled from playlist/main.tpl */ ?>
<div class="content">

<?php if ($this->_tpl_vars['PL_editMetaData']): ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "playlist/metadata.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>

<?php elseif ($this->_tpl_vars['PL_import']): ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "playlist/import.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>

<?php elseif (is_array ( $this->_tpl_vars['PL']->getActiveArr() )): ?>              <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "playlist/editor.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>

<?php else: ?>                                              <div class="container_elements" style="width: 607px;">
    <h1>##Playlist Editor##</h1>
        <p>&nbsp;</p>
        <?php if ($this->_tpl_vars['PL']->reportLookedPL()): ?>
            <input type="button" value="##Open last Playlist##" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=PL.unlook')" class="button_wide">
        <?php else: ?>
            <input type="button" value="##New empty Playlist##" onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=PL.create')" class="button_wide">
        <?php endif; ?>
    </div>
<?php endif; ?>

</div>