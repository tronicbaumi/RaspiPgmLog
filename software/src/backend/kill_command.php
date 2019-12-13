<?php
/**
 * kill a command
 */

$errors = array();
exec('sudo killall -9 ' . $_POST['kill_command'] . ' 2>&1', $errors);
echo $errors;
exit;

