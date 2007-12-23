<?php /* Smarty version 2.6.6, created on 2007-12-21 23:20:13
         compiled from library/main.tpl */ ?>
<?php require_once(SMARTY_CORE_DIR . 'core.load_plugins.php');
smarty_core_load_plugins(array('plugins' => array(array('modifier', 'cat', 'library/main.tpl', 1, false),)), $this); ?>
<?php $this->assign('mainURL', ((is_array($_tmp=((is_array($_tmp=((is_array($_tmp=$this->_tpl_vars['UI_BROWSER'])) ? $this->_run_mod_handler('cat', true, $_tmp, "?id=") : smarty_modifier_cat($_tmp, "?id=")))) ? $this->_run_mod_handler('cat', true, $_tmp, $this->_tpl_vars['START']['id']) : smarty_modifier_cat($_tmp, $this->_tpl_vars['START']['id'])))) ? $this->_run_mod_handler('cat', true, $_tmp, "&act=") : smarty_modifier_cat($_tmp, "&act="))); ?>

<?php $this->assign('browseURL', ((is_array($_tmp=$this->_tpl_vars['mainURL'])) ? $this->_run_mod_handler('cat', true, $_tmp, 'BROWSE') : smarty_modifier_cat($_tmp, 'BROWSE')));  $this->assign('searchURL', ((is_array($_tmp=$this->_tpl_vars['mainURL'])) ? $this->_run_mod_handler('cat', true, $_tmp, 'SEARCH') : smarty_modifier_cat($_tmp, 'SEARCH')));  $this->assign('hubBrowseURL', ((is_array($_tmp=((is_array($_tmp=((is_array($_tmp=$this->_tpl_vars['UI_BROWSER'])) ? $this->_run_mod_handler('cat', true, $_tmp, "?id=") : smarty_modifier_cat($_tmp, "?id=")))) ? $this->_run_mod_handler('cat', true, $_tmp, $this->_tpl_vars['START']['id']) : smarty_modifier_cat($_tmp, $this->_tpl_vars['START']['id'])))) ? $this->_run_mod_handler('cat', true, $_tmp, "&popup[]=HUBBROWSE.getResults") : smarty_modifier_cat($_tmp, "&popup[]=HUBBROWSE.getResults")));  $this->assign('hubSearchURL', ((is_array($_tmp=$this->_tpl_vars['mainURL'])) ? $this->_run_mod_handler('cat', true, $_tmp, 'HUBSEARCH') : smarty_modifier_cat($_tmp, 'HUBSEARCH')));  $this->assign('transfersURL', ((is_array($_tmp=$this->_tpl_vars['mainURL'])) ? $this->_run_mod_handler('cat', true, $_tmp, 'TRANSFERS') : smarty_modifier_cat($_tmp, 'TRANSFERS'))); ?>

<?php if ($this->_tpl_vars['searchForm']): ?>
		<div id="tabnav">
			<ul>
				<li><a href="<?php echo $this->_tpl_vars['browseURL']; ?>
">##Browse##</a></li>
				<li><a href="#" class="active">##Search##</a></li>
								<li><a href="<?php echo $this->_tpl_vars['hubSearchURL']; ?>
">##Hub Search##</a></li>
				<li><a href="<?php echo $this->_tpl_vars['transfersURL']; ?>
">##Transfers##</a></li>
			</ul>
		</div>
        <div class="content">
        <!-- start search -->
        <div class="container_elements" style="width: 607px;">
        <h1>##Search##</h1>
    <?php $this->assign('_act_prefix', 'SEARCH'); ?>
    <?php $this->assign('_form', $this->_tpl_vars['searchForm']); ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "library/searchForm.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
    <?php $this->assign('_results',  $this->_reg_objects['SEARCH'][0]->getResult(array(), $this));?>
    <?php $this->assign('_criteria',  $this->_reg_objects['SEARCH'][0]->getCriteria(array(), $this)); endif; ?>


<?php if ($this->_tpl_vars['browseForm']): ?>
		<div id="tabnav">
			<ul>
				<li><a href="#" class="active">##Browse##</a></li>
				<li><a href="<?php echo $this->_tpl_vars['searchURL']; ?>
">##Search##</a></li>
								<li><a href="<?php echo $this->_tpl_vars['hubSearchURL']; ?>
">##Hub Search##</a></li>
				<li><a href="<?php echo $this->_tpl_vars['transfersURL']; ?>
">##Transfers##</a></li>
			</ul>
		</div>
        <div class="content">
        <!-- start browsing -->
		<div class="container_elements" style="width: 607px;">
		<h1>##Browse##</h1>
    <?php $this->assign('_act_prefix', 'BROWSE'); ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "library/browseForm.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
    <?php $this->assign('_results',  $this->_reg_objects['BROWSE'][0]->getResult(array(), $this));?>
    <?php $this->assign('_criteria',  $this->_reg_objects['BROWSE'][0]->getCriteria(array(), $this)); endif; ?>


<?php if ($this->_tpl_vars['hubBrowseForm']): ?>
		<div id="tabnav">
			<ul>
				<li><a href="<?php echo $this->_tpl_vars['browseURL']; ?>
">##Browse##</a></li>
				<li><a href="<?php echo $this->_tpl_vars['searchURL']; ?>
">##Search##</a></li>
								<li><a href="<?php echo $this->_tpl_vars['hubSearchURL']; ?>
">##Hub Search##</a></li>
				<li><a href="<?php echo $this->_tpl_vars['transfersURL']; ?>
">##Transfers##</a></li>
			</ul>
		</div>
        <div class="content">
        <!-- start hub browsing -->
		<div class="container_elements" style="width: 607px;">
		<h1>##Hub Browse##</h1>
    <?php $this->assign('_act_prefix', 'HUBBROWSE'); ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "library/hubBrowseForm.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
    <?php $this->assign('_results',  $this->_reg_objects['HUBBROWSE'][0]->getResult(array(), $this));?>
    <?php $this->assign('_criteria',  $this->_reg_objects['HUBBROWSE'][0]->getCriteria(array(), $this)); endif; ?>


<?php if ($this->_tpl_vars['hubSearchForm']): ?>
		<div id="tabnav">
			<ul>
				<li><a href="<?php echo $this->_tpl_vars['browseURL']; ?>
">##Browse##</a></li>
				<li><a href="<?php echo $this->_tpl_vars['searchURL']; ?>
">##Search##</a></li>
								<li><a href="#" class="active">##Hub Search##</a></li>
				<li><a href="<?php echo $this->_tpl_vars['transfersURL']; ?>
">##Transfers##</a></li>
			</ul>
		</div>
        <div class="content">
        <!-- start hub search -->
		<div class="container_elements" style="width: 607px;">
		<h1>##Hub Search##</h1>
    <?php $this->assign('_act_prefix', 'HUBSEARCH'); ?>
    <?php $this->assign('_form', $this->_tpl_vars['hubSearchForm']); ?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "library/searchForm.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
    <?php $this->assign('_results',  $this->_reg_objects['HUBSEARCH'][0]->getResult(array(), $this));?>
    <?php $this->assign('_criteria',  $this->_reg_objects['HUBSEARCH'][0]->getCriteria(array(), $this)); endif; ?>


<?php if ($this->_tpl_vars['transfersForm']): ?>
		<div id="tabnav">
			<ul>
				<li><a href="<?php echo $this->_tpl_vars['browseURL']; ?>
">##Browse##</a></li>
				<li><a href="<?php echo $this->_tpl_vars['searchURL']; ?>
">##Search##</a></li>
								<li><a href="<?php echo $this->_tpl_vars['hubSearchURL']; ?>
">##Hub Search##</a></li>
				<li><a href="#" class="active">##Transfers##</a></li>
			</ul>
		</div>
        <div class="content">
        <!-- start transfers -->
		<div class="container_elements" style="width: 607px;">
		<h1>##Transfers##</h1>
    <?php $this->assign('_act_prefix', 'TRANSFERS'); ?>
    <?php $this->assign('_results',  $this->_reg_objects['TRANSFERS'][0]->getTransfers(array(), $this));?>
    <?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "library/transfersForm.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
  else: ?>
	<?php $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => "library/results.tpl", 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
  endif; ?>

    </div>
</div>