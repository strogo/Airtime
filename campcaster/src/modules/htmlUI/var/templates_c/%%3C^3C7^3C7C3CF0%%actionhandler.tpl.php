<?php /* Smarty version 2.6.6, created on 2007-12-21 23:20:13
         compiled from library/actionhandler.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'lower', 'library/actionhandler.tpl', 4, false),)), $this); ?>
onClick="return contextmenu('<?php echo $this->_tpl_vars['i']['id']; ?>
'
    , 'SP.addItem'
    
    <?php if (((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('lower', true, $_tmp) : smarty_modifier_lower($_tmp)) == 'audioclip'): ?>
        , 'listen', '<?php echo $this->_tpl_vars['i']['gunid']; ?>
'
        
        <?php if ($this->_tpl_vars['_PL_activeId']): ?>
            , 'PL.addItem'
        <?php else: ?>
            , 'PL.create'
        <?php endif; ?>
        
        , 'edit'
        , 'delete'
    <?php endif; ?>

    <?php if (((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('lower', true, $_tmp) : smarty_modifier_lower($_tmp)) == 'webstream'): ?>
        , 'listen', '<?php echo $this->_tpl_vars['i']['gunid']; ?>
'
        
        <?php if ($this->_tpl_vars['_PL_activeId']): ?>
            , 'PL.addStream'
        <?php else: ?>
            , 'PL.create'
        <?php endif; ?>
        
        , 'edit'
        , 'delete'
    <?php endif; ?>

    <?php if (((is_array($_tmp=$this->_tpl_vars['i']['type'])) ? $this->_run_mod_handler('lower', true, $_tmp) : smarty_modifier_lower($_tmp)) == 'playlist'): ?>
        <?php if ($this->_tpl_vars['_PL_activeId']): ?>
            <?php if ($this->_tpl_vars['_PL_activeId'] == $this->_tpl_vars['i']['id']): ?>
                , 'PL.release'
            <?php else: ?>
                , 'SCHEDULER.addPL'
                , 'PL.addItem'
                , 'delete'
            <?php endif; ?>
        <?php else: ?>
            , 'SCHEDULER.addPL'
            , 'PL.activate'
            , 'PL.create'
            , 'delete'
            , 'PL.export'
        <?php endif; ?>
    <?php endif; ?>

    , 'TR.upload2Hub'
)"
