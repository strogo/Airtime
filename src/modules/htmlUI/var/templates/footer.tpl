        <div class="copyright">{$UI_VERSION} &copy;{$UI_COPYRIGHT_DATE}
            <a href="http://www.Sourcefabric.org" target="_blank">Sourcefabric</a>
            - maintained and distributed under GNU/GPL
        </div>
    </div>

<script>
    {UIBROWSER->getAlertMsg assign='alertMsg'}
    {if $alertMsg}
        alert('{$alertMsg|escape:quotes}');
    {/if}
</script>
</body>
</html>
