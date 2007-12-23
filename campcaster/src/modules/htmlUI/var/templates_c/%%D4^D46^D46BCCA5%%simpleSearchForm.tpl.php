<?php /* Smarty version 2.6.6, created on 2007-12-21 23:13:49
         compiled from library/simpleSearchForm.tpl */ ?>
<?php $this->assign('dynform', $this->_tpl_vars['simpleSearchForm']); ?>
        <!-- start library search -->
            <div class="container_elements">
                <h1>##Library Search##</h1>
                <form action="ui_handler.php" method="post" name="simplesearch" id="simplesearch"><input name="act" type="hidden" value="SEARCH.simpleSearch" />
                    <div>
                        <input size="20" maxlength="50" name="criterium" type="text" style="width: 184px;" />
                        <input type="button" class="button_small" value="##Go##" onClick="submit()"/>
                    </div>
                </form>
            </div>
        <!-- end library search -->