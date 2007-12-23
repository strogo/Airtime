<?php /* Smarty version 2.6.6, created on 2007-12-21 23:13:22
         compiled from popup/header.tpl */ ?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="de" lang="de">
<html>

<head>
    <title>Campcaster</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <link href="styles_popup.css" rel="stylesheet" type="text/css" />

    <script type="text/javascript">
    <?php $this->assign('alertMsg',  $this->_reg_objects['UIBROWSER'][0]->getAlertMsg(array(), $this));?>
    <?php if ($this->_tpl_vars['alertMsg']): ?>
       alert('<?php echo $this->_tpl_vars['alertMsg']; ?>
');
    <?php endif; ?>
    </script>
</head>

<body>
