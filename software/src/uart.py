# Script for communicating with a custom log device for RaspiPgmLog via Uart port /dev/serial0 (/dev/ttyS0)
# This script was built to be used by the software of the "Log" part of the RaspiPgmLog, 
# but it can be used standalone, too.
# Notice: to use this script standalone you need a folder named 'downloads' in the same directory
# 
# Script tasks:
# - tell the logger to start/stop logging
# - tell the logger the sample speed
# - read data send by the logger
# - output read to stdout
# - write read data to csv file

# import neccesary libraries
import time, serial, sys, getopt, select

# create standard name for logfile
localtime = time.localtime(time.time())
log_file_name = "timeplot_" + str(localtime.tm_year) + str(localtime.tm_mon) + str(localtime.tm_mday) + str(localtime.tm_hour) + str(localtime.tm_min) + str(localtime.tm_sec)

# open serial port serial0 (ttyS0) at baudrate 230400
serialPort0 = serial.Serial(
    port = '/dev/serial0',
    baudrate = 230400,
    parity = serial.PARITY_NONE,
    stopbits = serial.STOPBITS_ONE,
    bytesize = serial.EIGHTBITS,
    timeout = 1
)

# be sure, the serial port has been opened, else throw exception
if serialPort0.isOpen:
    #check if the script was called with options
    if len(sys.argv) > 1:
        # get options for processing
        opts, args = getopt.getopt(sys.argv[1:],"v:f:")
        for opt, val in opts:
            # option 'v' for velocity (speed)
            if opt == '-v':
                # send samplespeed to logger 
                serialPort0.write(("v|" + str(val).lstrip() + "\n").encode('utf-8'))
            # option 'f' for filename
            if opt == '-f':
                # set Logfilename, but without any kind of file extension
                log_file_name = val.split('.')[0]

    # complete logfile name with direktory and file extension
    log_file_name = "./downloads/timeplots/" + log_file_name + '.csv'
    # open log file for writing 
    f = open(log_file_name, "w")
    f.write("time(ms),A0,A1,A2,A3,A4,A5,A6,D5,D6,D9,D10,D11\n")
    # set run dependency for the recive loop
    read_uart = True
    # tell the logger to start logging
    serialPort0.write("start\n".encode('utf-8'))
    # initialise a buffer for stdin input
    buff = ''
    # start uart read loop
    while read_uart:
        # check if somthing has been sent to standard in
        rfds, wfds, efds = select.select( [sys.stdin], [], [], 0.005)
        if rfds:
            # 'stop' has been read, tell the logger to stop logging, set the stop dependency for the read loop 
            # and report to the caller
            while not buff.endswith('\n'):
                buff += sys.stdin.read(1)
            if ("stop" in buff):
                sys.stdout.write("Uart: stopping\n")
                serialPort0.write("stop\n".encode('utf-8'))
                read_uart = False
            if("g" in buff):
                sys.stdout.write(buff)
                serialPort0.write(buff.encode('utf-8'))
            buff = ''
        # do some errorhandling
        if not serialPort0.isOpen:
            raise Exception("Serial port closed unexpected")
            break
        # read data that was sent via uart
        data = serialPort0.readline().decode("utf-8")
        # write data to stdout
        sys.stdout.write(data)
        # if data is no debug/error message, write it to the logfile
        if len(data.split(',')) > 1:
            f.write(data)
else:
    raise Exception("Serial port could not be opened")

# do some cleanup
if serialPort0.isOpen:
    serialPort0.close()
if not f.closed:
    f.close()
# End