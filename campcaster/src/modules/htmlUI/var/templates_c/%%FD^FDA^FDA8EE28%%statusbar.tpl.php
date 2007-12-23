<?php /* Smarty version 2.6.6, created on 2007-12-21 23:12:48
         compiled from statusbar.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'date_format', 'statusbar.tpl', 16, false),)), $this); ?>
<?php $this->assign('_nowplaying', $this->_tpl_vars['SCHEDULER']->getNowNextClip());  $this->assign('_nextplaying', $this->_tpl_vars['SCHEDULER']->getNowNextClip(1)); ?>

<div id="masterpalette"> 
<table border="0" class="masterpalette">
	<tr>
		<td>
            <div id="logo">
              <img src="img/logo.png">
            </div>
		</td>
		
		<td>
    		<div id="time">
    		    ##Station Time##
                <h1><span id="servertime" class="clock"><?php echo ((is_array($_tmp=time())) ? $this->_run_mod_handler('date_format', true, $_tmp, "%H:%M:%S") : smarty_modifier_date_format($_tmp, "%H:%M:%S")); ?>
</span></h1>
                <?php echo ((is_array($_tmp=time())) ? $this->_run_mod_handler('date_format', true, $_tmp, "%Z") : smarty_modifier_date_format($_tmp, "%Z")); ?>

    		</div>
		</td>
		
		<td>	
    		<div id="nowplaying">
            <div class="whatplaying">
                <?php if ($this->_tpl_vars['_nowplaying']): ?>
                    <div class="title">##Now Playing##: <span id="now_title"></span></div>
                    <div class="scala">
                        <div class="scala_in" id="now_scala" style="width: <?php echo $this->_tpl_vars['_nowplaying']['percentage']; ?>
%;">&nbsp;</div>
                    </div>
                    <div class="time">
                        <div class="left">Elapsed:    <strong id="now_elapsed"></strong></div>
                        <div class="right">Remaining: <strong id="now_remaining"></strong></div>
                    </div>
                <?php endif; ?>
                <div  style="height:5px"> </div>
                <div id="next_clip">
                <?php if ($this->_tpl_vars['_nextplaying']): ?>
                    <p class="next">##Playing Next##: <span id="next_title"></span>&nbsp; <span id="next_duration"></span></p>
                <?php endif; ?>
                </div>
            </div>  
    		</div>
		</td>
		
		<td>
        <?php if ($this->_tpl_vars['_nowplaying']): ?>
            <div id="nowplaying_indicator"><div id="onair">##on air##</div></div>            
        <?php else: ?>
            <div id="nowplaying_indicator"><div id="offair">##off air##</div></div>
        <?php endif; ?>
    		
		</td>
		
		<td>
    		<div id="station">
    		  <img src="<?php echo $this->_tpl_vars['STATIONPREFS']['stationLogoPath']; ?>
" alt="<?php echo $this->_tpl_vars['STATIONPREFS']['stationName']; ?>
">
    		</div>
		</td>
    </tr>
</table>
</div>


<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "script/clock.js.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
  $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "script/progressbar.js.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>


<?php $this->assign('_nowplaying', null);  $this->assign('_nextplaying', null); ?>