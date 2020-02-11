/**
 * Client softerware of RaspiPgmLog Logger, to run in browser
 * this script provides the graph and fills it with data
 */
$(document).ready(function(){
    
    // initialise global variables/objects
    var socket = io();                              // socket to recive data
    var layout = {showLegend: true};                // option objects for the graph object
    var config = {scrollZoom: true};
    var sampletime = $('#sampletime_select').val(); // save the value of the sample speed select
    var displaying = true;                          // toggle value, if data is beeing plotted or not

    // object to update add data to the graph
    var update = { y: [[],[],[],[],[],[]] };

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
    },
    {
        y: [0],
        type: 'scatter',
        name: 'A3'
    },
    {
        y: [0],
        type: 'scatter',
        name: 'A4'
    },
    {
        y: [0],
        type: 'scatter',
        name: 'A5'
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
        chartdata[3].y = chartdata[3].y.slice(chartdata[3].y.length - size, chartdata[3].y.length);
        chartdata[4].y = chartdata[4].y.slice(chartdata[4].y.length - size, chartdata[4].y.length);
        chartdata[5].y = chartdata[5].y.slice(chartdata[5].y.length - size, chartdata[5].y.length);
    }
    
    // when the server sends data, add it to the graph
    socket.on('data', function(data){
        // push update object objects
        update.y[0].push(data.yval0);
        update.y[1].push(data.yval1);
        update.y[2].push(data.yval2);
        update.y[3].push(data.yval3);
        update.y[4].push(data.yval4);
        update.y[5].push(data.yval5);
        // keep graph update interval at max 2Hz to avaoid performance issues
        if(displaying && (sampletime >= 500 ||  update.y[0].length > (500/sampletime))){
            Plotly.extendTraces('chart', update, [0, 1, 2, 3, 4, 5]);
            // display max 300 datapoints in the graph, if have more, let the graph run
            if(chartdata[0].y.length > 300) {
                Plotly.relayout('chart',{
                    xaxis: {
                        range: [chartdata[0].y.length-300, chartdata[0].y.length]
                    },
                    yaxis: {
                        range: [0, 1024]
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
            update.y[3] = [];
            update.y[4] = [];
            update.y[5] = [];
        }
    });
    
    $('button').click(function(){
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
            Plotly.relayout('chart',{
                xaxis: {
                    range: [0, 300]
                },
                yaxis: {
                    range: [0, 1024]
                }
            });
            // send start trigger to the server
            socket.emit(id, sampletime);
        }else if(id === "pause"){
            // toggle displaying the plot and the text on the pause button
            var btn_text = $(this).text();
            if(btn_text === "Pause"){
                $(this).text("Unpause");
                displaying = false;
            }else{
                $(this).text("Pause");
                displaying = true;
            }
        }else{
            socket.emit(id);
        }
    });

    // when a checkbox is clicked, tell the log device to toggle the corresponding pwm
    $("input[type=checkbox]").change(function(){
        if(this.checked){
            socket.emit("pin_ctrl", "g|" + $(this).val() + "|" + $("#" + $(this).attr("aria-owns")).val());
        }else{
            socket.emit("pin_ctrl", "g|" + $(this).val() + "|0");            
        }
    });

    // when the value of a range input is changed, change the number in the label and tell the log device the new value, if the corresponding chekcbox was checked before
    $('input[type=range]').change(function(){
        $("label[for=" + $(this).attr('id') + "]").text(Number((scale($(this).val(), 0, 1023, 0, 5)).toFixed(1)));
        if($("input[aria-owns=" + $(this).attr("id") + "]").is(":checked"))
            socket.emit("pin_ctrl", "g|" + $("input[aria-owns=" + $(this).attr("id") + "]").val() + "|" + $(this).val());
    });
});

// scale a value from one range to another
function scale(x, in_min, in_max, out_min, out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}