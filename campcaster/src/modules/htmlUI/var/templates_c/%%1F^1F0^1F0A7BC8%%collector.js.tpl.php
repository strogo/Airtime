<?php /* Smarty version 2.6.6, created on 2007-12-21 23:12:48
         compiled from script/collector.js.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('function', 'tra', 'script/collector.js.tpl', 45, false),)), $this); ?>
<?php echo '
<script type="text/javascript">

function collector_submit(formname, action, script, name, width, height)
{
    var href = \'\';
    var n;
    
    '; ?>

    if (!script) var script = '<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
';
    <?php echo '
    
    for (n = 0; n < (document.forms[formname].elements.length); n++) {
        if (document.forms[formname].elements[n].checked && document.forms[formname].elements[n].name != \'all\') {
            href = href + \'&id[]=\' + document.forms[formname].elements[n].name;
        }
    }

    if (href == \'\') return false;
    
    
    if (action == \'delete\') {
                '; ?>

                popup('<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?popup[]=deleteItem' + href, 'deleteItem', 400, 100);
                return;
                <?php echo '
    }
    if (name)   popup (script + \'?act=\' + action + href, popup, width, height);
    else        hpopup(script + \'?act=\' + action + href);
}

function collector_switchAll(formname)
{
    var n;

    for (n = 0; n < document.forms[formname].elements.length; n++) {
        if (document.forms[formname].elements[n].type == \'checkbox\') {
            document.forms[formname].elements[n].checked = document.forms[formname].elements[\'all\'].checked;
        }
    }
}

function collector_clearAll(formname, action)
{
    if (confirm("';  echo S_tra(array('0' => 'Are you sure to remove all Items?'), $this); echo '")) {
        document.forms[formname].elements[\'all\'].checked = true;
        collector_switchAll(formname);
        collector_submit(formname, action);
    }
}
</script>
'; ?>
