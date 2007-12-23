<?php /* Smarty version 2.6.6, created on 2007-12-21 23:29:49
         compiled from playlist/metadata.tpl */ ?>
<div class="container_elements" style="width: 607px;">
<h1>##Playlist Metadata##: <?php echo $this->_tpl_vars['PL']->title; ?>
</h1>
<?php $this->assign('_form', $this->_tpl_vars['PL']->metaDataForm($this->_tpl_vars['_PL']['curr_langid'])); ?>

<?php $this->assign('dynform', $this->_tpl_vars['_form']['langswitch']);  $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "sub/dynForm_plain.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>

<?php $this->assign('dynform', $this->_tpl_vars['_form']['main']);  $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "sub/dynForm_plain.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
</div>


<script type="text/javascript">
<?php echo '

var MData_confirmChangeVisited = false;

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

function MData_submit()
{
    if (validate_editMetaData(document.forms["editMetaData"])) {
        document.forms["editMetaData"].elements["target_langid"].value = document.forms["langswitch"].elements["target_langid"].value;
        document.forms["editMetaData"].submit();
    }
    return false;
}

function MData_switchLang()
{
    document.forms["editMetaData"].elements["target_langid"].value = document.forms["langswitch"].elements["target_langid"].value;
    document.forms["editMetaData"].submit();
}

function MData_cancel()
{
    '; ?>

    location.href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=PL.simpleManagement";
    <?php echo '
}

'; ?>

</script>