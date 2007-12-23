<?php /* Smarty version 2.6.6, created on 2007-12-21 23:22:38
         compiled from file/metadataform.tpl */ ?>
<?php if ($this->_tpl_vars['_REQUEST']['act'] == addFileMData || $this->_tpl_vars['_REQUEST']['act'] == addWebstreamMData || $this->_tpl_vars['_REQUEST']['act'] == editItem): ?>

    <?php $this->assign('_metadataform',  $this->_reg_objects['UIBROWSER'][0]->metaDataForm(array('id' => $this->_tpl_vars['editItem']['id'],'langid' => $this->_tpl_vars['editItem']['curr_langid']), $this));?>
    
    <?php $this->assign('dynform', $this->_tpl_vars['_metadataform']['langswitch']); ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "sub/dynForm_plain.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>

    <?php if (count($_from = (array)$this->_tpl_vars['_metadataform']['pages'])):
    foreach ($_from as $this->_tpl_vars['key'] => $this->_tpl_vars['dynform']):
?>
        <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "sub/dynForm_plain.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
    <?php endforeach; unset($_from); endif; ?>
    
    <script language="javascript">
    <?php echo '
    
    var MData_confirmChangeVisited = true;
    
    function MData_confirmChange(element)
    {
        if (MData_confirmChangeVisited) return true;
    
        if (confirm("##Are you sure you want to change existing metadata?##")) {
            MData_confirmChangeVisited = true;
            return true;
        } else {
            document.forms[\'langswitch\'].elements[\'target_langid\'].focus();
            return false;
        }
    }
    
    function MData_loadLang()
    {
        '; ?>

        location.href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=editItem&id=<?php echo $this->_tpl_vars['editItem']['id']; ?>
&MData_langId=" + document.forms['langswitch'].elements['MData_langid'].value;
        <?php echo '
    }
    
    function MData_submit()
    {
        if (validate_editMetaData(document.forms[\'editMetaData\'])) {
            document.forms[\'editMetaData\'].elements[\'target_langid\'].value = document.forms[\'langswitch\'].elements[\'target_langid\'].value;
            document.forms[\'editMetaData\'].submit();
        }
        showMain();
        return false;
    }
    
    function MData_switchLang()
    {
        document.forms[\'editMetaData\'].elements[\'target_langid\'].value = document.forms[\'langswitch\'].elements[\'target_langid\'].value;
        document.forms[\'editMetaData\'].submit();
    }
    
    function MData_cancel()
    {
        '; ?>

        location.href='<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
';
        <?php echo '
    }
    
    function spread(element, name)
    {
        if (document.forms[\'editMetaData\'].elements[\'Main___\' + name])     document.forms[\'editMetaData\'].elements[\'Main___\' + name].value  = element.value;
        if (document.forms[\'editMetaData\'].elements[\'Music___\' + name])    document.forms[\'editMetaData\'].elements[\'Music___\' + name].value = element.value;
        if (document.forms[\'editMetaData\'].elements[\'Voice___\' + name])     document.forms[\'editMetaData\'].elements[\'Voice___\' + name].value  = element.value;
    }
    '; ?>

    
    
    <?php if ($this->_tpl_vars['_REQUEST']['act'] == addFileData || $this->_tpl_vars['_REQUEST']['act'] == addWebstreamData): ?>
        document.getElementById('div_MData').style.display   = 'none';
    <?php else: ?>
        document.getElementById('div_Data').style.display  = 'none';
        showMain();
    <?php endif; ?>
    
    </script>
    
<?php endif; ?>