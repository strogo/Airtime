<?php /* Smarty version 2.6.6, created on 2007-12-21 23:13:22
         compiled from sub/dynForm_plain.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('function', 'cycle', 'sub/dynForm_plain.tpl', 36, false),)), $this); ?>
<?php echo $this->_tpl_vars['dynform']['javascript']; ?>


<form<?php echo $this->_tpl_vars['dynform']['attributes']; ?>
><?php echo $this->_tpl_vars['dynform']['hidden']; ?>


<?php if (count($_from = (array)$this->_tpl_vars['dynform']['elements'])):
    foreach ($_from as $this->_tpl_vars['element']):
?>
    <?php if ($this->_tpl_vars['element']['style']): ?>
        <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "smarty-dynamic-".($this->_tpl_vars['element']['style']).".tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
    <?php endif; ?>
    

    <?php if ($this->_tpl_vars['element']['type'] == 'static'): ?>
        <?php echo $this->_tpl_vars['element']['html']; ?>


    <?php else: ?>
        <div class="container_search">
            <?php if ($this->_tpl_vars['element']['label']): ?>
                <label><?php echo $this->_tpl_vars['element']['label']; ?>

                <?php if ($this->_tpl_vars['element']['required']): ?><font color="red">*</font><?php endif; ?>
                </label>
            <?php endif; ?>

            <?php if ($this->_tpl_vars['element']['error']): ?><font color="red"><?php echo $this->_tpl_vars['element']['error']; ?>
</font><br /><?php endif; ?>

            <?php if ($this->_tpl_vars['element']['type'] == 'group'): ?>
                <?php if (count($_from = (array)$this->_tpl_vars['element']['elements'])):
    foreach ($_from as $this->_tpl_vars['gkey'] => $this->_tpl_vars['gitem']):
?>
                    <?php echo $this->_tpl_vars['gitem']['label'];  echo $this->_tpl_vars['gitem']['html']; ?>

                    <?php if ($this->_tpl_vars['gitem']['required']): ?><font color="red">*</font><?php endif; ?>
                    <?php if ($this->_tpl_vars['element']['separator']):  echo smarty_function_cycle(array('values' => $this->_tpl_vars['element']['separator']), $this);?>

                    <?php endif; ?>
                <?php endforeach; unset($_from); endif; ?>
            <?php else: ?>
                <?php echo $this->_tpl_vars['element']['html']; ?>

            <?php endif; ?>
            <div style="font-size: 80%;"><?php echo $this->_tpl_vars['element']['label_note']; ?>
</div>
        </div>
    <?php endif;  endforeach; unset($_from); endif; ?>

    </form>

<!--
&nbsp;
<p><b>Collected Errors:</b><br />
<?php if (count($_from = (array)$this->_tpl_vars['dynform']['errors'])):
    foreach ($_from as $this->_tpl_vars['name'] => $this->_tpl_vars['error']):
?>
    <font color="red"><?php echo $this->_tpl_vars['error']; ?>
</font> in element [<?php echo $this->_tpl_vars['name']; ?>
]<br />
<?php endforeach; unset($_from); endif; ?>
</p> -->