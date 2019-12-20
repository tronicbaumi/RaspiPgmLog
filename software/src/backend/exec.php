<?php
/**
 * This script executes all commands to start the programmer software
 */
session_start();

// get all needed options
$programmer = $_POST['programmer'];
$action     = $_POST['input_action'];
$family     = $_POST['input_family'];
$option     = $_POST['input_additional_option'];
$file       = $_SESSION['uploaded_file'];

if(isset($_POST['input_connector'])){
    $connector = $_POST['input_connector'];
}

//close the session early, so it is not locked and the 'read_log.php' script can use it
session_write_close();

// assemble the command
// determine, whether picberry or openocd is needed
switch ($programmer) {
    case "picberry":
        //prepare programming connector
        switch ($connector) {
            case 'rj11':
                $gpios = '19,20,21';
                break;
            case 'row':
                $gpios = '5,6,12';
        }

        // prepare programming action
        switch ($action) {
            case 'write':
                $action = 'w ' . $file;
                break;
            case 'read':
                $action = 'r ' . '../download/data_' . date('Y-m-d_H:i:s') . '.hex';
                break;
            default:
                $action = '-' . $action;
                break;
        }

        // prepare additional option
        if($option == 'none'){
            $option = '';
        }else{
            $option = '--' . $option;
        }

        // assemble command
        $command = 'sudo picberry -' . $action . ' --gpio=' . $gpios . ' --family=' . $family . ' ' . $option;
        break;;
    
    case "openocd":
        // prepare driver specific part
        if($family == "atsame5x.cfg"){
            $driver_specific = '-c "atsame5 chip-erase" -c "atsame5 bootloader 0';
        }elseif ($family == "at91samdXX.cfg") {
            $driver_specific = '-c "at91samd chip-erase" -c "at91samd bootloader 0';
        }elseif ($family =="atsamv.cfg"){
            $driver_specific = '-c "flash erase_sector 0 0 0"';
        }

        // prepare programming action
        switch ($action) {
            case 'write': 
                $chip_action = $driver_specific . ' -c "flash write_image '. $file .'" ';//-c "verify_image '. $file .' 0x00000000" 
                break;
            case 'read':
                $chip_action = '-c "flash read_bank 0 ../download/data_' . date('Y-m-d_H:i:s') . '.bin" ';
                break;
            case 'erase':
                $chip_action = $driver_specific . ' -c "flash erase_check 0" ';
                break;
            case 'probe':
                $chip_action = '-c "flash probe 0" ';
                break;
            case 'info':
                $chip_action = '-c " flash info 0" ';
                break;
            case 'list':
                $chip_action = '-c "flash list" ';
                break;
            case 'banks':
                $chip_action = '-c "flash banks" ';
                break;  
        }

        // assemble command
        $command = 'sudo openocd -f "../OpenocdCustomConfigFiles/rpi2.cfg" -f "../OpenocdCustomConfigFiles/targets/' . $family . '" -f "../OpenocdCustomConfigFiles/openocd.cfg" ' . $chip_action . '-c "reset run" -c "shutdown"';
        break;
        
    case "pymcuprog":
        
        if($action == "write"){
            $action = $action . ' -f ' . $file . ' --erase --verify';
        }elseif($action == "read"){
            $action = $action . ' -f ../download/data_' . date('Y-m-d_H:i:s') . '.hex';
        }
        
        $command = 'sudo pymcuprog ' . $action . ' -d ' . $family . ' -t ' . $connector . ' --' . $option;
        break;
}

echo $command . ' 2>&1 ; echo Exit status : $?';
exit;

// open command output log for writing
$handle = fopen('../logs/command_output.txt', 'w');

//execute command
$proc = popen($command . ' 2>&1 ; echo Exit status : $?', 'r');
// read command output and write it to file
while (!feof($proc))
{
    fwrite($handle, fread($proc, 4096) . PHP_EOL);
}
// cleanup handles
pclose($proc);
fclose($handle);

// make log downloadable
$errors = array();
exec('sudo cp ../logs/command_output.txt ../download/log_' . date('Y-m-d_H:i:s') . '.txt 2>&1', $errors);

// tell browser, that programming is finished
if(count($errors) > 0){
    print_r($errors);
}else{
    echo 'Done';
}