<?php
/**
 * This script sends one line per call of the command output log to the browser
 */
session_start();

//open the file in read mode
$handle = fopen("../logs/command_output.txt", 'r');

//find next line
if(isset($_SESSION['file_pos'])){
    fseek($handle, $_SESSION['file_pos'], SEEK_SET);
}

// read one line
$line = fgets($handle, 4096);

//get current position of the file pointer
$position = ftell($handle);

// save postion
$_SESSION['file_pos'] = $position;

// cleanup
fclose($handle);

// reset if command is done
if(strpos($line, 'Exit status') !== false){
    unset($_SESSION['file_pos']);
}

// send line to browser, if it contains data
if($line === PHP_EOL){
    echo '';
}else{
    echo $line;
}
exit;