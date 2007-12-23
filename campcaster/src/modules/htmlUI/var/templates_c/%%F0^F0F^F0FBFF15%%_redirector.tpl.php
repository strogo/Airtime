<?php /* Smarty version 2.6.6, created on 2007-12-21 23:29:48
         compiled from popup/_redirector.tpl */ ?>
<script type="text/javascript">

    if (opener.location.href.indexOf("<?php echo $this->_tpl_vars['target']; ?>
") !== -1)
        opener.location.reload();
    else
        opener.location.href = "<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=<?php echo $this->_tpl_vars['target']; ?>
";

</script>