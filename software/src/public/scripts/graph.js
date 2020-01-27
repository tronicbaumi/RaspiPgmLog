/**
 * Client softerware of RaspiPgmLog Logger, to run in browser
 * this script provides the graph and fills it with data
 */
$(document).ready(function(){
    
    // initialise global variables/objects
    var socket = io();                              // socket to recive data
    var layout = {showLegend: true};                // option objects for the graph object
    var config = {scrollZoom: true};
    var sampletime = $('#sampletime_select').val(); // sample speed

    // object to update add data to the graph
    var update = { y: [[],[],[]] };

    // initial data object
    var chartdata = [{
        y: [0],
        type: 'scatter',
        name: 'A0'
    },
    {
        y: [0],
        type: 'scatter',
        name: 'A1'
    },
    {
        y: [0],
        type: 'scatter',
        name: 'A2'
    }];
    
    // initialise the graph
    Plotly.newPlot('chart',{
        type: 'line',
        data: chartdata,
        layout: layout,
        config: config
    });

    // reset the data arrays to avoid memory bloats
    var reset_chartdata = function(size){
        chartdata[0].y = chartdata[0].y.slice(chartdata[0].y.length - size, chartdata[0].y.length);
        chartdata[1].y = chartdata[1].y.slice(chartdata[1].y.length - size, chartdata[1].y.length);
        chartdata[2].y = chartdata[2].y.slice(chartdata[2].y.length - size, chartdata[2].y.length);
    }
    
    // when the server sends data, add it to the graph
    socket.on('data', function(data){
        // push update object objects
        update.y[0].push(data.yval0);
        update.y[1].push(data.yval1);
        update.y[2].push(data.yval2);
        // keep graph update interval at min 2Hz to avaoid performance issues
        if(sampletime >= 500 ||  update.y[0].length > (500/sampletime)){
            Plotly.extendTraces('chart', update, [0, 1, 2]);
            // display max 300 datapoints in the graph, if have more, let the graph run
            if(chartdata[0].y.length > 300) {
                Plotly.relayout('chart',{
                    xaxis: {
                        range: [chartdata[0].y.length-300, chartdata[0].y.length]
                    }
                });
            }
            // reset data arrays, when thy become to big
            if(chartdata[0].y.length > 5000){
                reset_chartdata(500);
            }
            // clear update object arrays
            update.y[0] = [];
            update.y[1] = [];
            update.y[2] = [];
        }
    });
    
    $('button').click(function(){
        // make sure file download is always disabled, when there is no file to download
        $('#downloadfile').prop('disabled', true);
        // button id = event to emit
        var id = $(this).attr('id');
        sampletime = $('#sampletime_select').val();
        if(id === 'start'){
            // reset graph
            reset_chartdata(0);
            Plotly.newPlot('chart',{
                type: 'line',
                data: chartdata,
                layout: layout,
                config: config
            });
            socket.emit(id, sampletime);
        }else if(id === 'stop'){
            // enable download link
            $('#downloadfile').prop('disabled', false);
            socket.emit(id);
        }else{
            socket.emit(id);
        }
    });
});