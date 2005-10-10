<html>

<head>
<meta http-equiv=Content-Type content="text/html; charset=utf-8">
<title>LiveSupport Help</title>

<style>
{literal}
body {
    font-family: Arial, Verdana, Helvetica, sans-serif;
    font-size: 12px;
    color: #2C2C2C;
    margin: 0px;
    background: #fff;
}

.copyright {
    font-size: 9px;
}


img {
    border: none;
}

.container {
    width: 100%;
}


/* #################################### CONTENTAREA ############################### */


.content {
    margin: 19px;
    float: left;
    width: 90%;
}

.container_elements {
    border: 1px solid #cfcfcf;
    padding: 6px;
    margin-bottom: 21px;
}

.container_elements h1
    {
    font-size: 18px;
    margin: 20px  0 0 0;
    color: #666;
    }
.container_elements h2
    {
    font-size: 16px;
    margin: 20px 0 0 0;
    color: #666;
    }
.container_elements h3
    {
    font-size: 14px;
    margin: 10px 0 0 0;
    color: #666;
    }
.container_elements h4
    {
    font-size: 12px;
    margin: 5px 0 0 0;
    color: #666;
    }
DIV.blockquote
{
    padding-left: 20px;
}
{/literal}
</style>

</head>

<body>
 <div class="container">
<div class="content">
<div class="container_elements">
<img src="img/logo_livesupport.png" border="0" />

<h1>Quick Start - {$UI_VERSION_FULLNAME}</h1>
<div class="copyright">{$UI_VERSION} &copy;2005 <a href="http://www.mdlf.org" target="_blank">MDLF</a> - maintained and distributed under GNU/GPL by <a href="http://www.campware.org" target="_blank">CAMPWARE</a></div>
  <h2>Navigation</h2>

<div class=blockquote>

    <h3>Context menus</h3>

  The LiveSupport
    web client uses context menus extensively throughout the program. For example, by clicking
    on the ScratchPad, you can perform a number of operations on your file, including
    adding it to a playlist and removing it from the ScratchPad.
  &nbsp;

    <h3>ScratchPad</h3>

  The ScratchPad
    provides you with a list of all the files (both sound files and playlists)
    you have worked with recently. This serves as both a &quot;history&quot; as
    well as a &quot;clipboard&quot; for working with files between windows. You
    can listen to a sound file from the ScratchPad by right-clicking 'Listen',
    which will send the sound file to you for playback.
  &nbsp;

    <h3>Playlists and ScratchPad</h3>

  Playlists appear
    in the ScratchPad differently than sound files. You can 'release' a playlist
    directly from the ScratchPad (see the Playlist section for more on releasing
    a playlist). A playlist that has been activated will appear in <b>bold</b>.
    A playlist that has been 'released' will appear in normal text.
</div>

<h2>Playlists</h2>
  <p >Playlists are the core of how LiveSupport works. You add sound
    files to a playlist, and then schedule that playlist to be played at a date
    and time of your choosing. Playlists are edited in the Playlist Editor. You
    can include an unlimited number of playlists inside playlists; for example,
    if you have a one-hour show and want to have two commercial breaks, each made
    up of their own playlists.</p>

<div class=blockquote>

    <h3>Adding a file from the ScratchPad to the active playlist</h3>

  You can add a
    file (either a sound file or a playlist file) from the ScratchPad to the active
    playlist by right-clicking it in the ScratchPad and selecting &quot;Add to
    active playlist&quot;. This will add it to the end of the active playlist.
  &nbsp;

    <h3>Changing file order in the playlist</h3>

  You can change
    a file's order in the playlist by clicking on the up or down arrows on the
    right.
  &nbsp;

    <h3>Change transition</h3>

  Transitions between
    sound files are set to 0 ms by default. You can change this by moving your
    mouse over the &quot;Fade&quot; line in the Playlist window and selecting
    &quot;Change Transition.&quot; In this menu, you can set the length of the
    transition, as well as the type of transition.
  &nbsp;

    <h3>Activating and releasing a playlist</h3>

  Playlists are
    either 'active' or 'released'. An released playlist is one that has been 'released'
    for broadcast, and can be added to the scheduler. An 'active' playlist can
    continue to be edited. Playlists can be activated for editing by right-clicking
    &quot;Activate&quot; in the ScratchPad menu. This will take you to the playlist
    editor. If a playlist has been released, it will appear in a list of playlists
    available for scheduling.
</div>

<h2>Schedule</h2>
  <p  >Once you've created a playlist, schedule it for playback using the
    Schedule window. (Please remember that only released playlists can be added
    to the schedule.)</p>

<div class=blockquote>

    <h3>Adding a playlist to the schedule</h3>

  You can add it
    to the schedule by opening the Schedule window, then choosing a schedule view
    that suits you. Then you can add your playlist to the schedule by right-clicking
    and choosing &quot;Insert playlist here&quot;. A popup window will then appear
    allowing you to select the exact start time for your playlist, as well as
    a pull-down list of available released playlists.
  &nbsp;

    <h3>Removing a playlist from the schedule</h3>

  You can remove
    a playlist from the schedule by right clicking on its time [soon to be its
    title] and choosing &quot;Remove Playlist&quot;. This does not delete the
    playlist, however. It will remain in the database.
</div>

<h2>Files</h2>
  <p  >Sound files are added to the LiveSupport system in two steps.</p>

<div class=blockquote>

    <h3>Uploading and analyzing</h3>

  The first step is to select the sound file you want to upload into
    the LiveSupport system. This is handled in the Upload -&gt; New file menu.
    You select the file to upload by clicking on the &quot;Browse&quot; button,
    choosing your sound file, then clicking &quot;Submit&quot;. LiveSupport automatically
    analyses the sound file for any information that may be stored in ID3 tags.
    <br/>
    If you encounter the error "unknown method" most probably the file size is bigger than allowed in the system settings of the machine you are running LiveSupport on. Contact your system administrator and tell him to consult the LiveSupport manual.

    <h3>Describing your file using metadata</h3>

  The second step allows you to edit this information (called &quot;metadata&quot;)
    and add your own. If your sound file is a music file, you have a number of
    options for entering metadata under the &quot;Music&quot; tab. If your sound
    file is a news report or other talk, choose the &quot;Talk&quot; tab. The
    Talk tab allows reporters to include the time and date their report is about,
    the organizations covered in the report, and the location the report takes
    place in. Good metadata will help you and other colleagues to later find and
    use the material you upload. It's in your best interest to be as thorough
    as possible in inputting this metadata.
</div>

<h2>Browse and Search</h2>
  <p  >The browse and search functions are
    designed to be both easy to use and powerful, helping you to search not only
    file titles but also other metadata. Both browse and search let you search
    for both sound files as well as playlists. </p>

<p  >Search works more or less like a regular search
  engine. You can type in a word to be searched, and the results will appear below
  the search input window. The difference is that you are searching the metadata
  that you and your colleagues input to describe the sound files you put in. Good
  metadata will mean better search results.</p>
  <div class=blockquote>


  <h3>Using the browse function</h3>

  The browse function is a powerful feature that lets you
    browse all files according to general criteria you specify. The first column
    lets you choose a category to browse from, such as �Genre�. Under that category,
    you can choose one of the options that appears in that category, which refines
    the number of files displayed. The second and third columns work in the same
    ways, and let you continue to refine your browsing.
  <h3>Using multiple search terms</h3>
  Let's say you want to finding all files created by �John
    Doe� in the year 2005. Do the following:
  <ul>
    <li>
      Select first the field to search and
        input its value. In this case it would be to pull down the �Creator� field
        and type in �John Doe� in the window.
    </li>
    <li>
      Then press the �Add One Row� button
        and an additional search row appears. Select the �Year� field and type
        in 2005 in the window.
    </li>
    <li>
      Press �Submit� and your results �
        if any � will appear below.
    </li>
  </ul>

    <h3>Working with the files you find</h3>

  You can add the files you find � in both
    browse and search � either to your ScratchPad or directly to a new playlist.
    Right-clicking on a file in the results window gives you a number of options,
    such as:
  <ul>
    <li>
      Add to ScratchPad
    </li>
    <li>
      Listen
    </li>
    <li>
      New playlist using file
    </li>
    <li>
      Edit
    </li>
    <li>
      Delete
    </li>
  </ul>
</div>

<h2>System Preferences</h2>
  <p  >Your station's system administrator can change a number of system-wide
    settings in the System Preferences menu. In the System Preferences you can:</p>

<ul>
  <li>
    Add your station's logo and frequency
  </li>
  <li>
    Set the length of the ScratchPad
  </li>
  <li>
    Set the maximum file size that can be uploaded to the system
  </li>
  <li>
    Administer users and groups for the system, including assigning their
      access priveleges
  </li>
</ul>

<h1>Where to go for help</h1>
      <p >LiveSupport has a mailing list for support-related
        questions at <a href="mailto:ls-support@campware.org">ls-support@campware.org</a>,
        or you can visit the LiveSupport forums at <a href="http://www.campware.org/" target="_blank">http://www.campware.org</a></p>

</div>
</div>
</div>
</body>

</html>