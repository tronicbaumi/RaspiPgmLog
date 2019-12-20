<?php
/**
 * This script provides the control overlay for the programmer
 * If you want to add new options, look in datastorage.php
 */

Out()->Add(<<<EOT
    <h1>RaspiPgmLog Overlay</h1>
    <form action="?url=exec" method="post" id="file_form" enctype="multipart/form-data">
        <div class="form-group ">
            <label>Select File to upload:</label>
        </div>
        <div class="form-group">
            <div class="input-group">
                <div class="custom-file">
                    <input type="file" class="form-control-file custom-file-input" name="upload" id="input_group_file" aria-describedby="input_group_file_addon">
                    <label class="custom-file-label" for="input_group_file">Choose file</label>
                </div>
                <div class="input-group-append">
                    <button class="btn btn-primary" type="button" id="input_group_file_addon">Upload</button>
                </div>
            </div>
        </div>
        <div class="form-group form-check">
            <input type="checkbox" class="form-check-input" id="auto_overwrite" name="auto_overwrite">
            <label class="form-check-label" for="auto_overwrite">Auto Overwrite?</label>
        </div>
        <div id="upload_progress" class="progress">
            <div class="progress-bar" role="progressbar" style="width: 0%;" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100"></div>
        </div>
    </form>
    <h3>Programm Chip:</h3>
    <div class="input-group mb-3">
        <div class="input-group-prepend">
            <label class="input-group-text" for="select_programmer">Select Programmer:</label>
        </div>
        <select class="custom-select" id="select_programmer">
            <option value="0" selected>Choose...</option>
            <option value="picberry">Picberry (for dsPIC/PIC)</option>
            <option value="openocd">OpenOCD (for Cortex-M)</option>
            <option value="pymcuprog">Pymcuporg (for both (external ICD required))</option>
        </select>
    </div>
    <div class="d-none" id="pic_command_container">
        <form action="" method="post" id="command_form_pic">
            <input type="hidden" name="programmer" value="picberry">
            <div class="form-row">
                <div class="form-group col-md-6">
                    <label for="input_family">Chip Family</label>
                    <select id="input_family" class="form-control" name="input_family">
                        <option value="0" selected>Choose...</option>
EOT
);

foreach ($pic_families as $family){
    Out()->Add('<option value="'. $family . '">' . strtoupper($family) . '</option>');
}

Out()->Add(<<<EOT
                    </select>
                </div>
                <div class="form-group col-md-6">
                    <label for="input_connector">Connector</label>
                    <select id="input_connector" class="form-control" name="input_connector">
                        <option value="0" selected>Choose...</option>
EOT
);

foreach ($pic_connectors as $value => $connector){
    Out()->Add('<option value="'. $value . '">' . $connector . '</option>');
}

Out()->Add(<<<EOT
                    </select>
                </div>
            </div>
            <div class="form-row">
                <div class="form-group col-md-6">
                    <label for="input_action">Action</label>
                    <select id="input_action" class="form-control" name="input_action">
                        <option value="0" selected>Choose...</option>
EOT
);

foreach ($pic_actions as $value => $action){
    Out()->Add('<option value="'. $value . '">' . $action . '</option>');
}

Out()->Add(<<<EOT
                    </select>
                </div>
                <div class="form-group col-md-6">
                    <label for="input_additional_option">Additional Option</label>
                    <select id="input_additional_option" class="form-control" name="input_additional_option">
                        <option value="none" selected>None</option>
EOT
);

foreach ($pic_options as $value => $option){
    Out()->Add('<option value="'. $value . '">' . $option . '</option>');
}

Out()->Add(<<<EOT
                    </select>
                </div>
            </div>
            <div class="form-row">
                <div class="form-group col">
                    <button type="button" class="btn btn-primary send_command" >Send Command</button>
                    <button type="button" class="btn btn-primary abort_command" >Abort Command</button>
                </div>
            </div>
        </form>
    </div>
    <div class="d-none" id="cortex_command_container">
        <form action="" method="post" id="command_form_cortex">
            <input type="hidden" name="programmer" value="openocd">
            <div class="form-row">
                <div class="form-group col-md-6">
                    <label for="input_family">Chip Family</label>
                    <select id="input_family" class="form-control" name="input_family">
                        <option value="0" selected>Choose...</option>
EOT
);

foreach ($cortex_m_families as $value => $family){
    Out()->Add('<option value="'. $value . '">' . $family . '</option>');
}

Out()->Add(<<<EOT
                    </select>
                </div>
            </div>
            <div class="form-row">
                <div class="form-group col-md-6">
                    <label for="input_action">Action</label>
                    <select id="input_action" class="form-control" name="input_action">
                        <option value="0" selected>Choose...</option>
EOT
);

foreach ($cortex_m_actions as $value => $action){
    Out()->Add('<option value="'. $value . '">' . $action . '</option>');
}

Out()->Add(<<<EOT
                    </select>
                </div>
                <div class="form-group col-md-6">
                    <label for="input_additional_option">Additional Option</label>
                    <select id="input_additional_option" class="form-control" name="input_additional_option">
                        <option value="none" selected>None</option>
EOT
);

foreach ($cortex_m_options as $value => $option){
    Out()->Add('<option value="'. $value . '">' . $option . '</option>');
}

Out()->Add(<<<EOT
                    </select>
                </div>
            </div>
            <div class="form-row">
                <div class="form-group col">
                    <button type="button" class="btn btn-primary send_command" >Send Command</button>
                    <button type="button" class="btn btn-primary abort_command">Abort Command</button>
                </div>
            </div>
        </form>
    </div>
    <div class="d-none" id="pymcuprog_command_container">
        <form action="" method="post" id="command_form_pymcuprog">
            <input type="hidden" name="programmer" value="pymcuprog">
            <div class="form-row">
                <div class="form-group col-md-6">
                    <label for="input_family">Chip:</label>
                    <select id="input_family" class="form-control" name="input_family">
                        <option value="0" selected>Choose...</option>
EOT
);

foreach ($pymcuprog_chips as $chip){
    Out()->Add('<option value="'. $chip . '">' . $chip . '</option>');
}

Out()->Add(<<<EOT
                    </select>
                </div>
                <div class="form-group col-md-6">
                    <label for="input_connector">Programmer (tool)</label>
                    <select id="input_connector" class="form-control" name="input_connector">
                        <option value="0" selected>Choose...</option>
EOT
);

foreach ($pymcuprog_programmers as $value => $programmer){
    Out()->Add('<option value="'. $value . '">' . $programmer . '</option>');
}

Out()->Add(<<<EOT
                    </select>
                </div>
            </div>
            <div class="form-row">
                <div class="form-group col-md-6">
                    <label for="input_action">Action</label>
                    <select id="input_action" class="form-control" name="input_action">
                        <option value="0" selected>Choose...</option>
EOT
);

foreach ($pymcuprog_actions as $value => $action){
    Out()->Add('<option value="'. $value . '">' . $action . '</option>');
}


Out()->Add(<<<EOT
                    </select>
                </div>
                <div class="form-group col-md-6">
                    <label for="input_additional_option">Additional Option</label>
                    <select id="input_additional_option" class="form-control" name="input_additional_option">
                        <option value="none" selected>None</option>
EOT
);

foreach ($pymcuprog_options as $value => $option){
    Out()->Add('<option value="'. $value . '">' . $option . '</option>');
}

Out()->Add(<<<EOT
                    </select>
                </div>
            </div>
            <div class="form-row">
                <div class="form-group col">
                    <button type="button" class="btn btn-primary send_command" >Send Command</button>
                    <button type="button" class="btn btn-primary abort_command">Abort Command</button>
                </div>
            </div>
        </form>
    </div>
    <div id="console_frame">
        <ul id="command_output"></ul>
        <div id="command_progress" class="progress">
            <div class="progress-bar bg-success" role="progressbar" style="width: 0%;" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100"></div>    
        </div>
    </div>
    <div class="col d-flex justify-content-end">
        <a href="?url=downloads"><button type="button" class="btn btn-primary">Download Files</button></a>
    </div>
    <div class="row">
        <a href="?url=gotograph"><button type="button" class="btn btn-primary">Data Logger</button></a>
    </div>
EOT
);