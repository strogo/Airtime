<?php /* Smarty version 2.6.6, created on 2007-12-21 23:22:38
         compiled from file/edit.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'capitalize', 'file/edit.tpl', 13, false),)), $this); ?>
<div class="content">
    <?php if ($this->_tpl_vars['_REQUEST']['act'] == addFileMData || $this->_tpl_vars['_REQUEST']['act'] == addWebstreamMData || $this->_tpl_vars['_REQUEST']['act'] == editItem): ?>
        <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "file/tabswitch.js.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
    <?php endif; ?>

    <div class="container_elements" style="width: 607px;">
        <h1>
        <?php if ($this->_tpl_vars['_REQUEST']['act'] == addFileData || $this->_tpl_vars['_REQUEST']['act'] == addWebstreamData || $this->_tpl_vars['_REQUEST']['act'] == addWebstreamMData): ?>
            ##New##
        <?php else: ?>
            ##Edit##
        <?php endif; ?>
        <?php echo ((is_array($_tmp=$this->_tpl_vars['editItem']['type'])) ? $this->_run_mod_handler('capitalize', true, $_tmp) : smarty_modifier_capitalize($_tmp)); ?>

        </h1>

    <?php if ($this->_tpl_vars['editItem']['type'] == 'audioclip' || $this->_tpl_vars['editItem']['type'] == 'file'): ?>
        <div id="div_Data">
        <?php if ($this->_tpl_vars['_REQUEST']['act'] == 'addFileData'): ?>
                <?php $this->assign('dynform',  $this->_reg_objects['UIBROWSER'][0]->fileForm(array('id' => $this->_tpl_vars['editItem']['id'],'folderId' => $this->_tpl_vars['editItem']['folderId']), $this));?>
                <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "sub/dynForm_plain.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
                <?php $this->assign('_uploadform', null); ?>
        <?php endif; ?>
        </div>

        <div id="div_MData">
            <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "file/metadataform.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
        </div>
    <?php endif; ?>

    <?php if ($this->_tpl_vars['editItem']['type'] == 'webstream'): ?>
        <div id="div_Data">
            <?php $this->assign('dynform',  $this->_reg_objects['UIBROWSER'][0]->webstreamForm(array('id' => $this->_tpl_vars['editItem']['id'],'folderId' => $this->_tpl_vars['editItem']['folderId']), $this));?>
            <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "sub/dynForm_plain.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
            <?php $this->assign('_uploadform', null); ?>
        </div>

        <div id="div_MData">
            <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "file/metadataform.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
        </div>
    <?php endif; ?>

    <?php if ($this->_tpl_vars['editItem']['type'] == 'playlist'): ?>
        <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "file/metadataform.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
    <?php endif; ?>

    </div>
</div>

