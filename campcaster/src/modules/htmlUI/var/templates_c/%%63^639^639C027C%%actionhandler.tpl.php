<?php /* Smarty version 2.6.6, created on 2007-12-21 23:13:50
         compiled from scratchpad/actionhandler.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'lower', 'scratchpad/actionhandler.tpl', 4, false),)), $this); ?>
onClick="return contextmenu('<?php echo $this->_tpl_vars['i']['id']; ?>
'
    , 'SP.removeItem'
    
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
            <?php if ($this->_tpl_vars['i']['duration'] == '00:00:00.000000'): ?>
                , 'PL.addStream'
            <?php else: ?>
                , 'PL.addItem'
                <?php endif; ?>    
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
            <?php elseif ($this->_tpl_vars['PL']->isAvailable($this->_tpl_vars['i']['id']) == true): ?>
                , 'SCHEDULER.addPL'
                , 'PL.addItem'
                , 'PL.activate'
                , 'PL.delete'
            <?php endif; ?>
        <?php elseif ($this->_tpl_vars['PL']->isAvailable($this->_tpl_vars['i']['id']) == true): ?>
            , 'SCHEDULER.addPL'
            , 'PL.activate'
            , 'PL.create'
            , 'delete'
            , 'PL.export'
        <?php endif; ?>
    <?php endif; ?>
    
    , 'TR.upload2Hub'
)"