<?php /* Smarty version 2.6.6, created on 2007-12-21 23:12:49
         compiled from footer.tpl */ ?>
        <div class="copyright"><?php echo $this->_tpl_vars['UI_VERSION']; ?>
 &copy;<?php echo $this->_tpl_vars['UI_COPYRIGHT_DATE']; ?>

            <a href="http://www.mdlf.org" target="_blank">MDLF</a>
            - maintained and distributed under GNU/GPL by 
            <a href="http://www.campware.org" target="_blank">CAMPWARE</a>
        </div>
    </div>

<script>
    <?php $this->assign('alertMsg',  $this->_reg_objects['UIBROWSER'][0]->getAlertMsg(array(), $this));?>
    <?php if ($this->_tpl_vars['alertMsg']): ?>
        alert('<?php echo $this->_tpl_vars['alertMsg']; ?>
');
    <?php endif; ?>
</script>
</body>
</html>