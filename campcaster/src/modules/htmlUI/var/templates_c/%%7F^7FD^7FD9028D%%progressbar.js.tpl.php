<?php /* Smarty version 2.6.6, created on 2007-12-21 23:12:49
         compiled from script/progressbar.js.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'string_format', 'script/progressbar.js.tpl', 103, false),)), $this); ?>
<script language="javascript">

<?php echo '
// play-progress-bar object

function plPrBar(tit, eh, ei, es, dh, di, ds, next, ntit, nh, ni, ns){
    this.tit        = tit.slice(0, 40);
    this.next       = next;
    this.interval   = 333;
    this.ntit       = ntit.slice(0, 35);
    this.nh         = nh;
    this.ni         = ni;
    this.ns         = ns;

    // inits:
    this.elapsed    = new Date();
    this.duration   = new Date();
    this.remaining  = new Date();
    this.elapsed.setTime (Date.UTC(1970, 0, 1, eh, ei, es));
    this.duration.setTime(Date.UTC(1970, 0, 1, dh, di, ds));

    // methods:
    this.init   = plPrBar_init;
    this.tick   = plPrBar_tick;
    this.update = plPrBar_update;
    this.stop   = plPrBar_stop;
    this.show   = plPrBar_show;
    this.hide   = plPrBar_hide;
    this.create = plPrBar_create;
    this.run    = setInterval("ppb.tick();", this.interval);
}

function plPrBar_init() {
    document.getElementById("now_title").innerHTML              = this.tit;

    if (this.next == 0) {
        document.getElementById("next_clip").innerHTML          = \'\';
    } else {
        document.getElementById("next_title").innerHTML         = this.ntit;
        document.getElementById("next_duration").innerHTML      = \'(\' + twoDigit(this.nh) + \':\' + twoDigit(this.ni) + \':\' + twoDigit(this.ns) + \')\';
    }

    this.show();
    this.update();
}

function plPrBar_tick() {
    if (this.remaining.getTime() <= this.interval*2)  {
        this.stop();
        return;
    }

    this.elapsed.setTime(this.elapsed.getTime() + this.interval);
    this.remaining.setTime(this.duration.getTime() - this.elapsed.getTime());

    this.update();
}

function plPrBar_update() {
    document.getElementById("now_elapsed").innerHTML   = twoDigit(this.elapsed.getUTCHours())   + ":" + twoDigit(this.elapsed.getUTCMinutes())   + ":" + twoDigit(this.elapsed.getUTCSeconds());
    document.getElementById("now_remaining").innerHTML = twoDigit(this.remaining.getUTCHours()) + ":" + twoDigit(this.remaining.getUTCMinutes()) + ":" + twoDigit(this.remaining.getUTCSeconds());
    document.getElementById("now_scala").style.width   = (100 / this.duration.getTime() * this.elapsed.getTime()) + "%";
}

function plPrBar_stop() {
    clearInterval(this.run);

    if (this.next == 0) {
        // just if no next item to play
        this.hide();
    } else {
        // all values for next clip needed here:
        jsCom("jscom_wrapper", ["uiBrowser", "SCHEDULER", "getNowNextClip4jscom"], this.create);
    }
}

function plPrBar_create(jscomRes) {
        if (jscomRes !== \'\') {
            eval(\'var parms = \' + jscomRes + \';\');
            ppb = new plPrBar(parms[0],
                              parms[1], parms[2], parms[3],
                              parms[4], parms[5], parms[6],
                              parms[7], parms[8], parms[9], parms[10], parms[11] 
                             );
            ppb.init();
        } else {
            plPrBar_hide();
        };
}

function plPrBar_show() {
    document.getElementById("nowplaying_indicator").innerHTML   = \'<div id="onair">##on air##</div>\';
}

function plPrBar_hide() {
    document.getElementById("nowplaying_indicator").innerHTML   = \'<div id="offair">##off air##</div>\';
    document.getElementById("nowplaying").innerHTML             = "";
}
'; ?>


<?php if (( is_array ( $this->_tpl_vars['_nowplaying']['duration'] ) )): ?> 
    ppb = new plPrBar  ("<?php echo $this->_tpl_vars['_nowplaying']['title']; ?>
",
                        <?php echo ((is_array($_tmp=$this->_tpl_vars['_nowplaying']['elapsed']['h'])) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
, <?php echo ((is_array($_tmp=$this->_tpl_vars['_nowplaying']['elapsed']['m'])) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
, <?php echo ((is_array($_tmp=$this->_tpl_vars['_nowplaying']['elapsed']['s'])) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
,
                        <?php echo ((is_array($_tmp=$this->_tpl_vars['_nowplaying']['duration']['h'])) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
, <?php echo ((is_array($_tmp=$this->_tpl_vars['_nowplaying']['duration']['m'])) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
, <?php echo ((is_array($_tmp=$this->_tpl_vars['_nowplaying']['duration']['s'])) ? $this->_run_mod_handler('string_format', true, $_tmp, "%d") : smarty_modifier_string_format($_tmp, "%d")); ?>
,
                        <?php if (is_array ( $this->_tpl_vars['_nextplaying'] )): ?>
                            1, "<?php echo $this->_tpl_vars['_nextplaying']['title']; ?>
", <?php echo ((is_array($_tmp=$this->_tpl_vars['_nextplaying']['duration']['h'])) ? $this->_run_mod_handler('string_format', true, $_tmp, '%d') : smarty_modifier_string_format($_tmp, '%d')); ?>
, <?php echo ((is_array($_tmp=$this->_tpl_vars['_nextplaying']['duration']['m'])) ? $this->_run_mod_handler('string_format', true, $_tmp, '%2d') : smarty_modifier_string_format($_tmp, '%2d')); ?>
, <?php echo ((is_array($_tmp=$this->_tpl_vars['_nextplaying']['duration']['s'])) ? $this->_run_mod_handler('string_format', true, $_tmp, '%2d') : smarty_modifier_string_format($_tmp, '%2d')); ?>

                        <?php else: ?>
                            0, "", 0, 0, 0
                        <?php endif; ?>
                       );
    ppb.init();
<?php endif; ?>


<?php echo $this->_tpl_vars['JSCOM']->genJsCode(); ?>


</script>


