{include file="popup/header.tpl"}

<center>
##Do you want to save changes?##
<br><br>
<input type="button" class="button" onClick="window.close()" value="Cancel">
<input type="button" class="button" onClick="location.href='{$UI_HANDLER}?act=PL.revertANDclose'" value="No">
<input type="button" class="button" onClick="location.href='{$UI_HANDLER}?act=PL.release'" value="Yes">
</center>

</body>
</html>
