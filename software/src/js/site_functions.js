/**
 * Clientside js script with all functions for the frontpage
 */
$(document).ready(function(){
    // show selected file in file input
    $('input[type="file"]').change(function(e){
        var fileName = e.target.files[0].name;
        $('.custom-file-label').html(fileName);
    });
    
    // upload file
    $('#input_group_file_addon').click(function () {
        if($("#auto_overwrite")[0].checked){
            upload_file();
        }else{
            // ask if the file already exists, if yes, show a warning
            $.ajax({
                url: 'backend/upload.php',
                type: 'POST',
                data: {file_exist: $('.custom-file-label').html()},
                success: function(data){
                    if(data === '1'){
                        show_confirmation_modal("The file already exists! Do you want to overwrite it?");
                    }else{
                        upload_file();
                    }
                }
            });
        }
    });
    
    // confirm file upload through warning message
    $('#continue').click(function(){
        $('#confirmation_modal').modal('hide');
        upload_file();
    });
    
    // handle programmer select by showing and hiding the respective containers
    $('#select_programmer').change(function(){
        var pic_container = $('#pic_command_container');
        var cortex_container = $('#cortex_command_container');
        
        switch($(this).val()){
            case '0': // hide all 
                if(pic_container.hasClass('d-block')){
                    pic_container.removeClass('d-block');
                    pic_container.addClass('d-none');
                }
                if(cortex_container.hasClass('d-block')){
                    cortex_container.removeClass('d-block');
                    cortex_container.addClass('d-none');
                }
                break;
            case 'picberry': // show picberry container
                if(cortex_container.hasClass('d-block')){
                    cortex_container.removeClass('d-block');
                    cortex_container.addClass('d-none');
                }
                if(pic_container.hasClass('d-none')){
                    pic_container.removeClass('d-none');
                    pic_container.addClass('d-block');
                }
                break;
            case 'openocd': // show openocd container
                if(pic_container.hasClass('d-block')){
                    pic_container.removeClass('d-block');
                    pic_container.addClass('d-none');
                }
                if(cortex_container.hasClass('d-none')){
                    cortex_container.removeClass('d-none');
                    cortex_container.addClass('d-block');
                }
                break;
        }
    });
    
    // send a command to execute on the server
    $('.send_command').click(function(){
        // select command form
        var command_form = null;
        if($('#select_programmer').val() === 'picberry'){
            command_form = $('#command_form_pic');
        }else if($('#select_programmer').val() === 'openocd'){
            command_form = $('#command_form_cortex');
        }
        // validate, that all neccessary selections haven been made
        var valid = true;
        command_form.find('select').each(function(){
            if($(this).val() === '0' && $(this).prop('disabled') !== true){
                valid = false;
                $(this).addClass('error');
            }
        });
        if(valid){
            // check if debug option was selected: with some actions this will produce a huge amount of output
            // -> dont show this ouutput on the website in that case
            $('#command_output').html('');
             var no_command_output = false;
            if($('#input_additional_option').val() === 'debug' && $('#input_action').val() === 'write'){
                var no_command_output = true;
                show_info_modal('With the debug option enabled this action will produce a hugh amount of output. There there will no output from the command on the website. After completion there will be the log downloadable.');
            }else{
                do_poll();
            }
            // send data to exec script, to start the command
            $.ajax({
                url: 'backend/exec.php',
                type: 'POST',
                data: new FormData(command_form[0]),
                cache: false,
                contentType: false,
                processData: false,
                success: function(data){
                    if(no_command_output === true){
                        $('#command_output').append('<li>' + data + '</li>');
                    }
                    console.log(data);
                    
                }
            });
        }else{
            show_info_modal('Please fill in all needed fields');
        }
    });
    
    // kill a command
    $('.abort_command').click(function(){
        var command = '';
        if($('#select_programmer').val() === 'openocd'){
            command = 'openocd';
        }else if($('#select_programmer').val() === 'picberry'){
            command = 'picberry';
        }
        $.ajax({
            url: 'backend/kill_command.php',
            type: 'POST',
            data: {kill_command: command}
        });
    });
});

// upload a file
function upload_file(){
    $.ajax({
        url: 'backend/upload.php',
        type: 'POST',
        data: new FormData($('#file_form')[0]),

        // Tell jQuery not to process data or worry about content-type
        // You *must* include these options!
        cache: false,
        contentType: false,
        processData: false,

        // Custom XMLHttpRequest
        xhr: function () {
            var myXhr = $.ajaxSettings.xhr();
            if (myXhr.upload) {
                // For handling the progress of the upload
                myXhr.upload.addEventListener('progress', function (e) {
                    if (e.lengthComputable) {
                        // fill progressbar
                        $('.progress-bar').attr({
                            style: "width: " + ((e.loaded/e.total) * 100) + "%;",
                            "aria-valuenow": e.loaded,
                            "aria-valuemax": e.total
                        });
                        $('.progress-bar').text(e.loaded + " byte");
                    }
                }, false);
            }
            return myXhr;
        },
        
        success: function(data){
            show_info_modal(data);
        }
    });
}

// show a modal with a information/message/warning
function show_info_modal(data){
    $('#information_modal').find('.modal-body').html(data);
    $('#information_modal').modal({
        backdrop: 'static',
        keyboard: false,
        focus:    true,
        show:     true
    });
}

// show a modal with the option to confirm something
function show_confirmation_modal(data){
    $('#confirmation_modal').find('.modal-body').html(data);
    $('#confirmation_modal').modal({
        backdrop: 'static',
        keyboard: false,
        focus:    true,
        show:     true
    });
}

var empty_counter = 0;
// poll the server for data to display in #command_outpput
function do_poll(){
    var interval = setInterval(function(){
        $.post('backend/read_log.php', function(data) {
            console.log(data);
            if(data.trim() !== ''){
                empty_counter = 0;
                // display data, if a "statusbar" is recived, animate it properly
                if(data.includes('[ 0%]')){// "statusbar" start
                    $('#command_output').append('<li id="command_progress">' + data + '</li>');
                }else if(data.match(/(\[(\s|\d{1,2})\d%\])/) !== null){ // "statusbar" run
                    $('#command_progress').text(data.match(/(\[(\s|\d{1,2})\d%\])/)[0]);
                }else{
                    $('#command_output').append('<li>' + data + '</li>');
                }
                updateScroll();
            }else{
                // display a waring commmand takes to long
                empty_counter++;
                if(empty_counter === 500){
                    show_info_modal('The Server seems to be stuck. You might want to abort the current command');
                    empty_counter = 0;
                }
            }
            if(data.includes('Exit status')){
                clearInterval(interval);
            }
        });
    }, 100);
}

// scroll to the bottom of #command_output
function updateScroll(){
    $('#command_output').stop().animate({
        scrollTop: $('#command_output')[0].scrollHeight
    }, 1);
}