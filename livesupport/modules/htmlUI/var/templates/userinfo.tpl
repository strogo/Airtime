{if $USER.userid}
    <div class="loginname">##Signed in## : {$USER.login}</div>
    <input type="button" class="button" value="##logout##" onClick='javascript: popup("{$UI_BROWSER}?popup[]=logout", "name", "400", "50");'/>
    {*
    <input type="button" class="button" value="{##sign over##" onClick='javascript: popup("{$UI_BROWSER}?popup[]=signover_1", "name", "400", "150");'/>
    *}
{else}
    <input type="button" class="button" value="##login##" onClick='javascript: popup("{$UI_BROWSER}?popup[]=login", "name", "400", "150");'/>
{/if}
