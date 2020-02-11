/**
 * Clientside js script with all functions for the frontpage
 */
$(document).ready(function(){
    var socket = io();

    // receive a answer from the server 
    socket.on("answer", function(answer){
        switch(answer.id){
            case "file_exist":
                console.log(answer.data);
                if(answer.data === true){
                    show_confirmation_modal("The file already exists! Do you want to overwrite it?");
                }else{
                    upload_file();
                }
                break;
        }
    });

    // display output of the command
    socket.on("command_output", function(data){
        console.log(data);
        // display data, if a "statusbar" is recived, animate it properly
        if(data.match(/(\[(\s|\d{1,2})\d%\])/) !== null){ // "statusbar" run
            var now = data.match(/((\s|\d{1,2})\d%)/)[0];
            $('#command_progress').text();
            $('#command_progress .progress-bar').attr({
                style: "width: " + now,
                "aria-valuenow": now,
                "aria-valuemax": "100%"
            });
            $('#command_progress .progress-bar').text(now);
        }else{
            $('#command_output').append('<li>' + data + '</li>');
        }
        updateScroll();
    });

    //  // display a warning if a commmand takes to long
    //  empty_counter++;
    //  if(empty_counter === 500){
    //      show_info_modal('The Server seems to be stuck. You might want to abort the current command');
    //      empty_counter = 0;
    //  }

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
            socket.emit("ask", {
                id: "file_exist",
                data: $('.custom-file-label').html()
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
        var pymcuprog_container = $('#pymcuprog_command_container');
        
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
                if(pymcuprog_container.hasClass('d-block')){
                    pymcuprog_container.removeClass('d-block');
                    pymcuprog_container.addClass('d-none');
                }
                break;
            case 'picberry': // show picberry container
                if(cortex_container.hasClass('d-block')){
                    cortex_container.removeClass('d-block');
                    cortex_container.addClass('d-none');
                }
                if(pymcuprog_container.hasClass('d-block')){
                    pymcuprog_container.removeClass('d-block');
                    pymcuprog_container.addClass('d-none');
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
                if(pymcuprog_container.hasClass('d-block')){
                    pymcuprog_container.removeClass('d-block');
                    pymcuprog_container.addClass('d-none');
                }
                if(cortex_container.hasClass('d-none')){
                    cortex_container.removeClass('d-none');
                    cortex_container.addClass('d-block');
                }
                break;
            case 'pymcuprog': // show pymcuprog container
                if(pic_container.hasClass('d-block')){
                    pic_container.removeClass('d-block');
                    pic_container.addClass('d-none');
                }
                if(cortex_container.hasClass('d-block')){
                    cortex_container.removeClass('d-block');
                    cortex_container.addClass('d-none');
                }
                if(pymcuprog_container.hasClass('d-none')){
                    pymcuprog_container.hasClass('d-none');
                    pymcuprog_container.addClass('d-block');
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
        }else if($('#select_programmer').val() === 'pymcuprog'){
            command_form = $('#command_form_pymcuprog');
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
            // var no_command_output = false;
            // if($('#input_additional_option').val() === 'debug' && $('#input_action').val() === 'write'){
            //     var no_command_output = true;
            //     show_info_modal('With the debug option enabled this action will produce a hugh amount of output. There there will no output from the command on the website. After completion there will be the log downloadable.');
            // }else{
            //     do_poll();
            // }
            // send data to exec script, to start the command
            socket.emit('command',{
                firmware: $('#select_programmer').val(),
                file: $('.custom-file-label').html(),
                family: command_form.find('#input_family').val(),
                connector: command_form.find('#input_connector').val(),
                action: command_form.find('#input_action').val(),
                option: command_form.find('#input_additional_option').val()
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
        else if($('#select_programmer').val() === 'pymcuprog'){
            command = 'pymcuprog';
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
        url: '/fileupload',
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
                        $('#upload_progress .progress-bar').attr({
                            style: "width: " + ((e.loaded/e.total) * 100) + "%;",
                            "aria-valuenow": e.loaded,
                            "aria-valuemax": e.total
                        });
                        $('#upload_progress .progress-bar').text(e.loaded + " byte");
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

// scroll to the bottom of #command_output
function updateScroll(){
    $('#command_output').stop().animate({
        scrollTop: $('#command_output')[0].scrollHeight
    }, 1);
}