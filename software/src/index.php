<?php
/**
 * index script of the programmer webserver of RaspiPgmLog
 * this loads the template and processes the url to load the correct view file into the template
 */
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
ini_set("file_uploads", "On");
error_reporting(E_ALL);

session_start();

require_once('classes/Output.php');
require_once('backend/datastorage.php');

Out()->LoadTemplate("styles/template.html");

// path url (GET) => internal path to file
$addresses = array(
    'login'     => 'view/login.php',
    'start'     => 'view/frontpage.php',
    'downloads' => 'view/downloads.php',
    'gotograph' => 'view/redirect.php',
    'info'      => 'view/php_info.php'
);

// set url
if(!isset($_SESSION["logged_in"]) || $_SESSION['logged_in'] === false){
    $url = "login"; 
}else{
    if(isset($_GET['url'])){
        $url = $_GET['url']; 
    }else{
        $url = "start";
    }
}

// load view file
if(isset($addresses[$url])){
    require_once $addresses[$url];
} else {
    Out()->Add("Error: Unknown Url");
}

//Output the template
Out()->out();
