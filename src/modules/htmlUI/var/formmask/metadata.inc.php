<?php    /* =========================================================== Matadata-Mask */
$mask = array(
    'basics' => array(
        array(
            'element' => 'act',
            'type'    => 'hidden',
        ),
        array(
            'element' => 'id',
            'type'    => 'hidden'
        ),
        array(
            'element' => 'curr_langid',
            'type'    => 'hidden'
        ),
        array(
            'element' => 'target_langid',
            'type'    => 'hidden'
        ),
    ),
    'buttons' => array (
        array(
            'element' =>'reset',
            'type'    =>'reset',
            'label'   =>'Reset',
            'groupit' => TRUE,
            'attributes' => array(
                'class'=> "button",
                ),
        ),
        array(
            'element'   => 'cancel',
            'type'      => 'button',
            'label'     => 'Cancel',
            'attributes'=> array(
                'class'=> "button",
                'onClick'  => "MData_cancel()"
                ),
            'groupit'   => TRUE,
        ),
        array(
            'element' =>'button',
            'type'    =>'button',
            'label'   =>'Submit',
            'groupit' => TRUE,
            'attributes'=> array(
                'class'=> "button",
                //'onClick' => 'return switchMDataLang();'
                'onClick'   => 'MData_submit()'
                ),
        ),
        array(
            'group'   => array('reset', 'cancel', 'button'),
        )
    ),
    'langswitch'    => array(
        array(
            'element'  => 'target_langid',
            'type'     => 'select',
            'label'    => 'Language',
            'options'  => _getLanguages(),
            'attributes'=> array('onChange' => 'MData_switchLang()')
        )
    ),
    'pages' => array(
        'Main'  => array(
            array(
                'element'   => 'dc:title',
                'type'      => 'text',
                'label'     => 'Title',
                'required'  => TRUE,
                'id3'       => array('Title')
            ),
            array(
                'element'   => 'dc:creator',
                'type'      => 'text',
                'label'     => 'Creator',
                'required'  => TRUE,
                'id3'       => array('Artist')
            ),
            array(
                'element'   => 'dc:type',
                'type'      => 'text',
                'label'     => 'Genre',
                'required'  => TRUE,
                'id3'       => array('Genre')
            ),
            array(
                'element'   => 'dc:format',
                'type'      => 'select',
                'label'     => 'File format',
                'required'  => TRUE,
                'options'   => array(
                                'File'          => 'Audioclip',
                                'live stream'   => 'Webstream'
                               ),
                'attributes'=> array('disabled' => 'on'),
                'id3'       => FALSE
            ),
            array(
                'element'   => 'dcterms:extent',
                'type'      => 'text',
                'label'     => 'Length',
                'attributes'=> array('disabled' => 'on'),
                'id3'       => FALSE
            ),
        ),
        'Music'  => array(
            array(
                'element'   => 'dc:title',
                'type'      => 'text',
                'label'     => 'Title',
                #'id3'       => 'Title'
            ),
            array(
                'element'   => 'dc:creator',
                'type'      => 'text',
                'label'     => 'Creator',
                #'id3'       => 'Artist'
            ),
            array(
                'element'   => 'dc:source',
                'type'      => 'text',
                'label'     => 'Album',
                'id3'       => array('Album')
            ),
            array(
                'element'   => 'ls:year',
                'type'      => 'select',
                'label'     => 'Year',
                'options'   => _getNumArr(1900, date('Y')+5),
                'id3'       => array('Year')
            ),
            array(
                'element'   => 'dc:type',
                'type'      => 'text',
                'label'     => 'Genre',
                #'id3'       => 'Genre'
            ),
            array(
                'element'   => 'dc:description',
                'type'      => 'textarea',
                'label'     => 'Description',
                'id3'       => array('Comment', 'Comments')
            ),
            array(
                'element'   => 'dc:format',
                'type'      => 'select',
                'label'     => 'Format',
                'options'   => array(
                                'File'          => 'Audioclip',
                                'live stream'   => 'Webtream'
                               ),
                'attributes'=> array('disabled' => 'on'),
                'id3'       => FALSE
            ),
            array(
                'element'   => 'ls:bpm',
                'type'      => 'text',
                'label'     => 'BPM',
                'rule'      => 'numeric',
                'id3'       => array('BPM')
            ),
            array(
                'element'   => 'ls:rating',
                'type'      => 'text',
                'label'     => 'Rating',
                'rule'      => 'numeric',
                'id3'       => array('Rating')
            ),
            array(
                'element'   => 'dcterms:extent',
                'type'      => 'text',
                'label'     => 'Length',
                'attributes'=> array('disabled' => 'on'),
                'id3'       => FALSE
            ),
            array(
                'element'   => 'ls:encoded_by',
                'type'      => 'text',
                'label'     => 'Encoded by',
                'id3'       => array('Encoded by')
            ),
            array(
                'element'   => 'ls:track_num',
                'type'      => 'select',
                'label'     => 'Track number',
                'options'   => _getNumArr(0, 99),
                'id3'       => array('Track')
            ),
            array(
                'element'   => 'ls:disc_num',
                'type'      => 'select',
                'label'     => 'Disc number',
                'options'   => _getNumArr(0, 20),
                'id3'       => array('Disk')
            ),
            array(
                'element'   => 'ls:mood',
                'type'      => 'text',
                'label'     => 'Mood',
                'id3'       => array('Mood')
            ),
            array(
                'element'   => 'dc:publisher',
                'type'      => 'text',
                'label'     => 'Label',
                'id3'       => array('Label')
            ),
            array(
                'element'   => 'ls:composer',
                'type'      => 'text',
                'label'     => 'Composer',
                'id3'       => array('Composer')
            ),
            array(
                'element'   => 'ls:bitrate',
                'type'      => 'text',
                'label'     => 'Bitrate',
                'rule'      => 'numeric',
                'id3'       => array('Bitrate')
            ),
            array(
                'element'   => 'ls:channels',
                'type'      => 'select',
                'label'     => 'Channels',
                'options'   => array(
                                ''  => '',
                                1   => 'Mono',
                                2   => 'Stereo',
                                6   => '5.1'
                               ),
                'id3'       => array('Channels')
            ),
            array(
                'element'   => 'ls:samplerate',
                'type'      => 'text',
                'label'     => 'Sample rate',
                'rule'      => 'numeric',
                'attributes'=> array('disabled' => 'on'),
                'id3'       => array('Samplerate')
            ),
            array(
                'element'   => 'ls:encoder',
                'type'      => 'text',
                'label'     => 'Encoder software used',
                'id3'       => array('Encoder')
            ),
            array(
                'element'   => 'ls:crc',
                'type'      => 'text',
                'label'     => 'Checksum',
                'rule'      => 'numeric',
                'id3'       => array('CRC')
            ),
            array(
                'element'   => 'ls:lyrics',
                'type'      => 'textarea',
                'label'     => 'Lyrics',
                'id3'       => array('Lyrics')
            ),
            array(
                'element'   => 'ls:orchestra',
                'type'      => 'text',
                'label'     => 'Orchestra or band',
                'id3'       => array('Orchestra or band')
            ),
            array(
                'element'   => 'ls:conductor',
                'type'      => 'text',
                'label'     => 'Conductor',
                'id3'       => array('Conductor')
            ),
            array(
                'element'   => 'ls:lyricist',
                'type'      => 'text',
                'label'     => 'Lyricist',
                'id3'       => array('Lyricist')
            ),
            array(
                'element'   => 'ls:originallyricist',
                'type'      => 'text',
                'label'     => 'Original lyricist',
                'id3'       => array('Original lyricist')
            ),
            array(
                'element'   => 'ls:radiostationname',
                'type'      => 'text',
                'label'     => 'Radio station name',
                'id3'       => array('Radio station name')
            ),
            array(
                'element'   => 'ls:audiofileinfourl',
                'type'      => 'text',
                'label'     => 'Audio file information web page',
                'id3'       => array('Audio file information web page'),
                'attributes'=> array('maxlength' => 256)
            ),
            array(
                'rule'      => 'regex',
                'element'   => 'ls:audiofileinfourl',
                'format'    => UI_REGEX_URL,
                'rulemsg'   => 'Audio file information web page seems not to be valid URL'
            ),
            array(
                'element'   => 'ls:artisturl',
                'type'      => 'text',
                'label'     => 'Artist web page',
                'id3'       => array('Artist web page'),
                'attributes'=> array('maxlength' => 256)
            ),
            array(
                'rule'      => 'regex',
                'element'   => 'ls:artisturl',
                'format'    => UI_REGEX_URL,
                'rulemsg'   => 'Artist web page seems not to be valid URL'
            ),
            array(
                'element'   => 'ls:audiosourceurl',
                'type'      => 'text',
                'label'     => 'Audio source web page',
                'id3'       => array('Audio source web page'),
                'attributes'=> array('maxlength' => 256)
            ),
            array(
                'rule'      => 'regex',
                'element'   => 'ls:audiosourceurl',
                'format'    => UI_REGEX_URL,
                'rulemsg'   => 'Audio source web page seems not to be valid URL'
            ),
            array(
                'element'   => 'ls:radiostationurl',
                'type'      => 'text',
                'label'     => 'Radio station web page',
                'id3'       => array('Radio station web page'),
                'attributes'=> array('maxlength' => 256)
            ),
            array(
                'rule'      => 'regex',
                'element'   => 'ls:radiostationurl',
                'format'    => UI_REGEX_URL,
                'rulemsg'   => 'Radio station web page seems not to be valid URL'
            ),
            array(
                'element'   => 'ls:buycdurl',
                'type'      => 'text',
                'label'     => 'Buy CD web page',
                'id3'       => array('Buy CD webpage'),
                'attributes'=> array('maxlength' => 256)
            ),
            array(
                'rule'      => 'regex',
                'element'   => 'ls:buycdurl',
                'format'    => UI_REGEX_URL,
                'rulemsg'   => 'Buy CD web page seems not to be valid URL'
            ),
            array(
                'element'   => 'ls:isrcnumber',
                'type'      => 'text',
                'label'     => 'ISRC number',
                'rule'      => 'numeric',
                'id3'       => array('ISRC')
            ),
            array(
                'element'   => 'ls:catalognumber',
                'type'      => 'text',
                'label'     => 'Catalog number',
                'rule'      => 'numeric',
                'id3'       => array('Catalog')
            ),
            array(
                'element'   => 'ls:originalartist',
                'type'      => 'text',
                'label'     => 'Original artist',
                'id3'       => array('Original Artist')
            ),
            array(
                'element'   => 'dc:rights',           ## ???
                'type'      => 'text',
                'label'     => 'Copyright',
                'id3'       => array('Copyright')
            ),
        ),
        'Talk'   => array(
            array(
                'element'   => 'dc:title',
                'type'      => 'text',
                'label'     => 'Title',
            ),
            array(
                'element'   => 'dcterms:temporal',      ## time/date!!
                'type'      => 'text',
                'label'     => 'Report date/time',
            ),
            array(
                'element'   => 'dcterms:spatial',      ## menu
                'type'      => 'textarea',
                'label'     => 'Report location',
            ),
            array(
                'element'   => 'dcterms:entity',
                'type'      => 'textarea',
                'label'     => 'Report organizations',
            ),
            array(
                'element'   => 'dc:description',
                'type'      => 'textarea',
                'label'     => 'Description',
            ),
            array(
                'element'   => 'dc:creator',       ## menu??
                'type'      => 'text',
                'label'     => 'Creator',
            ),
            array(
                'element'   => 'dc:subject',
                'type'      => 'text',
                'label'     => 'Subject',
            ),
            array(
                'element'   => 'dc:type',            ## menu
                'type'      => 'text',
                'label'     => 'Genre',
            ),
            array(
                'element'   => 'dc:format',
                'type'      => 'select',
                'label'     => 'Format',
                'options'   => array(
                                'File'          => 'Audioclip',
                                'live stream'   => 'Webstream'
                                ),
                'attributes'=> array('disabled' => 'on')
            ),
            array(
                'element'   => 'dc:contributor',
                'type'      => 'text',
                'label'     => 'Contributor',
            ),
            array(
                'element'   => 'dc:language',       ##menu
                'type'      => 'text',
                'label'     => 'Language',
            ),
            array(
                'element'   => 'dc:rights',
                'type'      => 'text',
                'label'     => 'Copyright',
            ),
        )
    ),
    'playlist'  => array(
            array(
                'element'   => 'dc:title',
                'type'      => 'text',
                'label'     => 'Title',
                'required'  => TRUE
            ),
            array(
                'element'   => 'dc:creator',
                'type'      => 'text',
                'label'     => 'Creator',
                'required'  => TRUE
            ),
            array(
                'element'   => 'dcterms:extent',
                'type'      => 'text',
                'label'     => 'Length',
                'attributes'=> array('disabled' => 'on')
            ),
            array(
                'element'   => 'dc:description',
                'type'      => 'textarea',
                'label'     => 'Description'
            ),
    )
);
?>
