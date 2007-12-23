<?php /* Smarty version 2.6.6, created on 2007-12-21 23:12:49
         compiled from script/clock.js.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'date_format', 'script/clock.js.tpl', 19, false),array('modifier', 'string_format', 'script/clock.js.tpl', 19, false),)), $this); ?>
<script language="javascript">

<?php echo '

function myClock(y, m, d, h, i, s, interval) {
    clock = new Array();
    clock[\'interval\']  = interval;
    clock[\'time\']      = new Date(y, m, d, h, i ,s);
    clock[\'run\']       = setInterval("incClock();", clock[\'interval\']);
}

function incClock() {
    clock[\'time\'].setTime(clock[\'time\'].getTime() + clock[\'interval\']);
    document.getElementById("servertime").innerHTML = twoDigit(clock[\'time\'].getHours()) + ":" + twoDigit(clock[\'time\'].getMinutes()) + ":" + twoDigit(clock[\'time\'].getSeconds());
}

'; ?>


myClock(<?php echo ((is_array($_tmp=((is_array($_tmp=time())) ? $this->_run_mod_handler('date_format', true, $_tmp, "%Y") : smarty_modifier_date_format($_tmp, "%Y")))) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
, <?php echo ((is_array($_tmp=((is_array($_tmp=time())) ? $this->_run_mod_handler('date_format', true, $_tmp, "%m") : smarty_modifier_date_format($_tmp, "%m")))) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
, <?php echo ((is_array($_tmp=((is_array($_tmp=time())) ? $this->_run_mod_handler('date_format', true, $_tmp, "%d") : smarty_modifier_date_format($_tmp, "%d")))) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
,
             <?php echo ((is_array($_tmp=((is_array($_tmp=time())) ? $this->_run_mod_handler('date_format', true, $_tmp, "%H") : smarty_modifier_date_format($_tmp, "%H")))) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
, <?php echo ((is_array($_tmp=((is_array($_tmp=time())) ? $this->_run_mod_handler('date_format', true, $_tmp, "%M") : smarty_modifier_date_format($_tmp, "%M")))) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
, <?php echo ((is_array($_tmp=((is_array($_tmp=time())) ? $this->_run_mod_handler('date_format', true, $_tmp, "%S") : smarty_modifier_date_format($_tmp, "%S")))) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
,
             1000);

</script>      