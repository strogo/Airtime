<?php

require_once(dirname(__FILE__)."/../Schedule.php");

class SchedulerExportTests extends PHPUnit_TestCase {
  function setup() {
    global $CC_CONFIG, $CC_DBC;

    // Clear the files table
    $sql = "DELETE FROM ".$CC_CONFIG["filesTable"];
    $CC_DBC->query($sql);

    // Add a file
    $values = array("filepath" => dirname(__FILE__)."/test10001.mp3");
    $this->storedFile = StoredFile::Insert($values, false);

    // Add a file
    $values = array("filepath" => dirname(__FILE__)."/test10002.mp3");
    $this->storedFile2 = StoredFile::Insert($values, false);

    // Clear the schedule table
    $sql = "DELETE FROM ".$CC_CONFIG["scheduleTable"];
    $CC_DBC->query($sql);

    // Create a playlist
    $playlist = new Playlist();
    $playlist->create("Scheduler Unit Test");
    $result = $playlist->addAudioClip($this->storedFile->getId());
    $result = $playlist->addAudioClip($this->storedFile2->getId());
    $result = $playlist->addAudioClip($this->storedFile2->getId());

    // Schedule it
    $i = new ScheduleGroup();
    $this->groupIdCreated = $i->add('2010-11-11 01:30:23', null, $playlist->getId());
  }

   public function testExport() {
     echo Schedule::ExportRangeAsJson("2010-01-01 00:00:00", "2011-01-01 00:00:00");
   }
}

?>