<?php
/**
 * This script processes every file upload for the Programmer
 */
session_start();

$target_dir = "../upload/";

// Tell the client whether the file exists, it wants to upload 
if(isset($_POST['file_exist'])){
    echo file_exists($target_dir . $_POST['file_exist']);
    exit;
}

$target_file = $target_dir . basename($_FILES["upload"]["name"]);
$file_type = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));

// everything is ok, try to upload file
if (move_uploaded_file($_FILES["upload"]["tmp_name"], $target_file)) {
    $_SESSION['uploaded_file'] = $target_file;
    echo "The file " . basename( $_FILES["upload"]["name"]). " has been uploaded.";        
} else {
    echo "Sorry, there was an error uploading your file.";
}
exit;