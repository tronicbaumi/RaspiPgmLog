<?php
/**
 * Provide logs and output of chip read operations to download
 */

// get all names of the files in the download folder
$downloads = array_diff(scandir('download/'), array('..', '.'));

Out()->Add(<<<EOT
    <h1>Files</h1>
    <div class="row">
        <div class="col">
            <h3>Command-output Logs:</h3>
EOT
);

// add download link for logfiles
foreach ($downloads as $file) {
    if(pathinfo('download/' . $file, PATHINFO_EXTENSION) === 'txt'){
        Out()->Add('<div class="row"><div class="col"><a href="download/' . $file . '" download>' . $file . '</a> (' . filesize('download/' . $file) . ' bytes)</div></div>');
    }
}

Out()->Add(<<<EOT
        </div>
        <div class="col">
            <h3>Read Hex-Files:</h3>
EOT
);

// add download link for hex and bin files
foreach ($downloads as $file) {
    if(pathinfo('download/' . $file, PATHINFO_EXTENSION) === 'hex' || pathinfo('download/' . $file, PATHINFO_EXTENSION) === 'bin'){
        Out()->Add('<div class="row"><div class="col"><a href="download/' . $file . '" download>' . $file . '</a> (' . filesize('download/' . $file) . ' bytes)</div></div>');
    }
}

Out()->Add(<<<EOT
        </div>
    </div>
    <div class="row">
        <div class="col d-flex justify-content-end">    
            <a href="?url=start"><button type="button" class="btn btn-primary">Back</button></a>
        </div>
    </div>
EOT
);