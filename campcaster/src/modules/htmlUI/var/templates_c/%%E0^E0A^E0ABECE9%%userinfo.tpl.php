<?php /* Smarty version 2.6.6, created on 2007-12-21 23:12:49
         compiled from userinfo.tpl */ ?>

<?php if ($this->_tpl_vars['USER']['userid']): ?>
    <div class="loginname">
    <div id="nav">
    ##Signed in## : <?php echo $this->_tpl_vars['USER']['login']; ?>

    &nbsp;
    <input type="button" class="button" value="##logout##" onClick='javascript: popup("<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?popup[]=logout", "name", "420", "50");'/>
        &nbsp;
    </div>
    </div>
<?php else: ?>
    <div id="nav">&nbsp;
    <input type="button" class="button" value="##login##" onClick='javascript: popup("<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?popup[]=login", "name", "420", "150");'/>
    </div>
<?php endif; ?>
