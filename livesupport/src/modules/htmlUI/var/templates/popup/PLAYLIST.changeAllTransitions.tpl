{include file="popup/header.tpl"}

<table height="100%" width="100%">
    <tr>
        <td style="border: 0">
            <center>
                <table width="100%" height="100%">
                    <tr><td style="border: 0">
                        {include file="sub/dynForm_plain.tpl}
                    </td></tr>
                </table>
            </center>
        </td>
    </tr>
</table>

<script type="text/javascript">
{literal}
function pl_switchUp()
{
    if (Number(document.forms['PL_changeTransition'].elements['duration'].value) < 5000) {
        document.forms['PL_changeTransition'].elements['duration'].value  = Number(document.forms['PL_changeTransition'].elements['duration'].value) + 100;
    } else {
        alert('Maximun reached');
    }
}

function pl_switchDown()
{
    if (Number(document.forms['PL_changeTransition'].elements['duration'].value) > 99) {
        document.forms['PL_changeTransition'].elements['duration'].value = Number(document.forms['PL_changeTransition'].elements['duration'].value) - 100;
    }
    /* else {
        if (document.forms['PL_changeTransition'].elements['type'][0].checked) document.forms['PL_changeTransition'].elements['type'][1].checked = true;
        else document.forms['PL_changeTransition'].elements['type'][0].checked = true;
    }  */
}

var pl_loop;

function pl_start(direction)
{
    pl_loop = setInterval("pl_switch"+ direction + "()", 300);
}

function pl_stop()
{
    clearInterval(pl_loop);
}

{/literal}
</script>

</body>
</html>

