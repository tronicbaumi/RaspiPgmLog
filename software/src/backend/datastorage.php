<?php
/**
 * This file just contains arrays and variables with different static data for the website.
 * It is made this way, to have no trouble to add new options 
 */

$password = "pass";

$pic_families = array(
    'dspic33e',
    'dspic33f',
    'dspic33ck',
    'pic10f322',
    'pic18fj',
    'pic24fj',
    'pic24fjxxxga0xx',
    'pic24fjxxxga3xx',
    'pic24fjxxga1xx',
    'pic24fjxxgb0xx',
    'pic24fjxxxga1xx',
    'pic24fjxxxgb1xx',
    'pic32mx1',
    'pic32mx2',
    'pic32mx3',
    'pic32mz',
    'pic32mk'
);

$pic_connectors = array(
    'rj11' => 'RJ11',
    'row' => '6-Pin Single Row'
);

$pic_actions = array(
    'read' => 'Read',
    'write' => 'Write',
    'erase' =>'Bulk erase',
    'blankcheck' => 'Blank Check',
    'regdump' => 'Dump Registers',
    'fulldump' => 'Full Dump'
);

$pic_options = array(
    'debug' => 'Debug',
    'noverify' => 'Dont Verify',
    'program-only' => 'Program Section only',
    'boot-only' => 'Boot Section only'
);

$cortex_m_families = array(
    'atsame5x.cfg' => 'atsame5x',
    'at91samdXX.cfg' => 'atsamc21',
    'atsame70.cfg' => 'atsame70'
); 

$cortex_m_actions = array(
    'write' => 'Write',
    'read' => 'Read',
    'erase' => 'Erase',
    'probe' => 'Probe Flash',
    'info' => 'Flash info',
    'list' => 'List Flash',
    'banks' => 'List Flash Banks'
);

$cortex_m_options = array(
    'd3' => 'debug'
);

$pymcuprog_chips = array(
    'atsamd21'
);

$pymcuprog_actions = array(
    'ping' => 'Ping',
    'erase' => 'Erase',
    'read' => 'Read',
    'write' => 'Write'
);

$pymcuprog_programmers = array(
    'pickit4'   => 'MPLAB PicKit4',
    'snap'      => 'MPLAB Snap'
);

$pymcuprog_options = array(
    'verbose' => 'Verbose Output',
    'verbosedebug' => 'Verbose debug Output'
);