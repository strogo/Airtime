<?php /* Smarty version 2.6.6, created on 2007-12-21 23:12:49
         compiled from menu.tpl */ ?>
<?php echo '
<script type=\'text/javascript\'><!--//--><![CDATA[//><!--
sfHover = function() {
    var sfEls = document.getElementById("nav").getElementsByTagName("LI");
    for (var i=0; i<sfEls.length; i++) {
        sfEls[i].onmouseover=function() {
            this.className+=" sfhover";
        }
        sfEls[i].onmouseout=function() {
            this.className=this.className.replace(new RegExp(" sfhover\\\\b"), "");
        }
    }
}
if (window.attachEvent) window.attachEvent("onload", sfHover);
//--><!]]></script>
'; ?>


<div class="container_nav">
<?php if ($this->_tpl_vars['USER']['userid']): ?>
    <ul id="nav">
        <li class="nav-main"><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?folderId=<?php echo $this->_tpl_vars['START']['fid']; ?>
&act=addFileData">##Add Audio##</a>
            <ul>
                <li><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?folderId=<?php echo $this->_tpl_vars['START']['fid']; ?>
&act=addFileData">##Audioclip##</a></li>
                            </ul>
        </li>
        <li class="nav-main"><a>##Media Library##</a>
            <ul>
                <li><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?id=<?php echo $this->_tpl_vars['START']['id']; ?>
&act=BROWSE">##Browse##</a></li>
                <li><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?id=<?php echo $this->_tpl_vars['START']['id']; ?>
&act=SEARCH">##Search##</a></li>
                                <li><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?id=<?php echo $this->_tpl_vars['START']['id']; ?>
&act=HUBSEARCH">##Hub Search##</a></li>
                <li><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?id=<?php echo $this->_tpl_vars['START']['id']; ?>
&act=TRANSFERS">##Transfers##</a></li>
            </ul>
        </li>
        <li class="nav-main"><a>##Playlists##</a>
            <ul>
                <?php if ($this->_tpl_vars['PL']->getActiveArr()): ?>
                    <li><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?id=<?php echo $this->_tpl_vars['START']['fid']; ?>
&act=PL.simpleManagement">##Edit Playlist##</a></li>
                <?php else: ?>
                    <?php if ($this->_tpl_vars['PL']->reportLookedPL()): ?>
                        <li><a onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=PL.unlook')">##Open last Playlist##</a></li>
                    <?php else: ?>
                        <li><a onClick="hpopup('<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=PL.create')">##New empty Playlist##</a></li>
                    <?php endif; ?>
                <?php endif; ?>
                <li><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=PL.import">##Import Playlist##</a></li>
            </ul>
        </li>
        <li class="nav-main"><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=SCHEDULER">##Scheduler##</a>
            <ul>
                <li><a href="<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=SCHEDULER.set&view=month&target=SCHEDULER">##Month##</a></li>
                <li><a href="<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=SCHEDULER.set&view=week&target=SCHEDULER">##Week##</a></li>
                <li><a href="<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=SCHEDULER.set&view=day&target=SCHEDULER">##Day##</a></li>
                <li><a href="<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=SCHEDULER.set&view=day&today=1&target=SCHEDULER">##Today##</a></li>
                                    <li><a href="<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?act=SCHEDULER.set&view=status&target=SCHEDULER">##Status##</a></li>
                                                                                                                
            </ul>
        </li>

        <li class="nav-main"><a>##Preferences##</a>
            <ul>
                            <li><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=changeStationPrefs">##Station Settings##</a></li>
                                        <li><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=SUBJECTS">##User/Groups##</a></li>
                            <li><a href="<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?act=SUBJECTS.chgPasswd&id=<?php echo $this->_tpl_vars['USER']['userid']; ?>
">##Change Password##</a></li>
                                                            </ul>
        </li>

        <li class="nav-main"><a>##Help##</a>
            <ul>
                <li><a href="" onclick="window.open('http://code.campware.org/manuals/campcaster/1.1/', 'help', 'menubar=yes,toolbar=yes,location=yes,status=yes,scrollbars=yes,resizable=yes,width=850,height=800')">##User Manual##</a></li>
                <li><a href="" onclick="window.open('<?php echo $this->_tpl_vars['UI_BROWSER']; ?>
?popup[]=help', 'help', 'scrollbars=yes,resizable=yes,width=500,height=800')">##Quickstart##</a></li>
                <li><a href="" onclick="window.open('http://campcaster.campware.org/', 'help', 'menubar=yes,toolbar=yes,location=yes,status=yes,scrollbars=yes,resizable=yes,width=850,height=800')">##Homepage##</a></li>

            </ul>
        </li>
    </ul>

<?php endif;  $_smarty_tpl_vars = $this->_tpl_vars;
$this->_smarty_include(array('smarty_include_tpl_file' => 'userinfo.tpl', 'smarty_include_vars' => array()));
$this->_tpl_vars = $_smarty_tpl_vars;
unset($_smarty_tpl_vars);
 ?>
 </div>