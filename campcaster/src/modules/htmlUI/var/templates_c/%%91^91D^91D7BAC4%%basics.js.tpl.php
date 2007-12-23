<?php /* Smarty version 2.6.6, created on 2007-12-21 23:12:48
         compiled from script/basics.js.tpl */ ?>
<script type="text/javascript">
    //  Browser detection
    ie5 = (document.getElementById && document.all && document.styleSheets) ? 1 : 0;
    nn6 = (document.getElementById && !document.all) ? 1 : 0;

    function frename(name, id)<?php echo '{'; ?>

        var s=document.getElementById('ID'+id);
        s.innerHTML='<form action="<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
" method="post" style="display:inline">'+
                        '<input type="text" name="newname" value="'+name+'" size="12">'+
                        '<input type="hidden" name="id" value="'+id+'">'+
                        '<input type="hidden" name="act" value="rename">'+
                    '</form>';
     <?php echo '}'; ?>


    function fmove(id, relPath)<?php echo '{'; ?>

        var newPath=prompt('Destination folder (relative path):', relPath);
        if(newPath==null) return;
        location.href='<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?id='+id+'&act=move&newPath='+newPath;
     <?php echo '}'; ?>


    function fcopy(id, relPath)<?php echo '{'; ?>

        var newPath=prompt('Destination folder (relative path):', relPath);
        if(newPath==null) return;
        location.href='<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?id='+id+'&act=copy&newPath='+newPath;
     <?php echo '}'; ?>


    function freplicate(name, id)<?php echo '{'; ?>

        var np=prompt('Destination folder (relative path):', id);
        if(np==null) return;
        location.href='<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?id='+id+'&act=repl&newparid='+np;
     <?php echo '}'; ?>


    function newFolder()<?php echo '{'; ?>

        var nn=prompt('New folder name:');
        if(nn==null) return;
        location.href='<?php echo $this->_tpl_vars['UI_HANDLER']; ?>
?id=<?php echo $this->_tpl_vars['START']['id']; ?>
&act=newFolder&newname='+nn;
    <?php echo '}


    function popup(url, name, width, height)   // popup in center of perent window
    {
        var name = \'popup\';
        var screenX;
        var screenY;
        var Xpos;
        var Ypos;
        var Pwidth;
        var Pheight;

        if (ie5) {
            Xpos     = window.screenLeft;
            Ypos     = window.screenTop;
            Pwidth   = document.body.clientWidth;
            Pheight  = document.body.clientHeight;
        } else {
            Xpos     = window.screenX;
            Ypos     = window.screenY;
            Pwidth   = window.innerWidth;
            Pheight  = window.innerHeight;
        }

        screenX = (Xpos + Pwidth/2  - width/2);
        screenY = (Ypos + Pheight/2 - height/2);
        url     =  url  + \'&is_popup=1\';
        arg     = \'width=\'+width+\', height=\'+height+\',resizable=yes, scrollbars=no, menubar=no, dependent=yes, left=\'+screenX+\', top=\'+screenY;

        popupwin = window.open(url, name, arg);
        window.popupwin.focus();
    }

    function hpopup(url)
    {
        var width  = 180;
        var height = 120;
        var name = Math.round(Math.random()*100000);
        var screenX;
        var screenY;
        var Xpos;
        var Ypos;
        var Pwidth;
        var Pheight;
        var popupwin;

        if (ie5) {
            Xpos     = window.screenLeft;
            Ypos     = window.screenTop;
            Pwidth   = document.body.clientWidth;
            Pheight  = document.body.clientHeight;
        } else {
            Xpos     = window.screenX;
            Ypos     = window.screenY;
            Pwidth   = window.innerWidth;
            Pheight  = window.innerHeight;
        }

        screenX = (Xpos + Pwidth/2  - width/2);
        screenY = (Ypos + Pheight/2 - height/2);
        url     =  url  + \'&is_popup=1\';
        arg     = \'width=\'+width+\', height=\'+height+\', scrollbars=no, menubar=no, dependent=no, left=\'+screenX+\', top=\'+screenY;

        popupwin = window.open(url, name, arg);
        //window.parent.focus();
    }

    function twoDigit(i) {
        i = Math.round(i);
        if(i < 10) i = "0" + i;

        return i;
    }
    '; ?>




</script>