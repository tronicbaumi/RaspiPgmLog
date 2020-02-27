#!/bin/sh

# print the help in a callable function
usage(){
	cat << EO
Install script for RaspiPgmLog

Usage: sh ${0} [options]

Options: 

    -h | --help         Show this message
    --device <device>   This option is mandatory! Specifes the device class that this script is run on.
                        Allowed arguments are: 
                            rpizero (for Raspberry Pi Zero and Raspberry Pi 1)
                            rpi2    (for Raspberry Pi 2 and higher)
                        Other devices are not supported!
    --no-display        The standard setup of this project uses a InkyPHAT display. If you dont use that, use this flag to prevent its drivers from beeing installed
EO
}

# A POSIX variable
OPTIND=1         # Reset in case getopts has been used previously in the shell.

# Initialize our own variables:
device=""
use_display=1

# define commandline options
SHORTOPTS="h"
LONGOPTS="help,no-display,device:"

# get command line options
ARGS=$(getopt -s bash --options $SHORTOPTS --longoptions $LONGOPTS --name RaspiPgmLog-Installer -- "$@")

# error handling on input options
if [ $? -ne 0 ] ; then echo "Invalid Option provided...exiting." >&2 ; exit 1 ; fi

eval set -- "$ARGS"
unset ARGS

# parse options and set internal variables
while true ; do
    case $1 in
    	-h|--help) 
			usage
			exit 0
			;;
		--device)
			shift
			device="$1"
			shift
			;;
		--no-display)
			use_display=0
			shift
			;;
		--)
			shift
			break
			;;
		*)
			if [ -z "$1" ] ; then 
				break; 
			else 
				echo "$1 is not a valid option"; 
				exit 1; 
			fi
			;;
    esac
done

echo "use_display=$use_display, device='$device'"

# additional validation
if [ "$device" != "rpizero" ] && [ "$device" != "rpi2" ] ; then
	echo "unknown device! Allowed options are: rpizero, rpi2";
	exit 1;
fi

# the actual installation process
# all of that is documented in the README.md in the top folder

apt-get update
apt-get upgrade -y
apt-get dist-upgrade -y
apt-get install python3 python3-pip git autoconf libtool make pkg-config libusb-1.0-0 libusb-1.0-0-dev -y
pip3 install pyserial
if [ "$device" = "rpizero" ] ; then
	mv node_server_zero.service node_server.service
	echo "installing node js for armvl6";
	cd /home/pi/
	wget https://nodejs.org/download/release/v11.15.0/node-v11.15.0-linux-armv6l.tar.gz
	tar -xzf node-v11.15.0-linux-armv6l.tar.gz
	cd node-v11.15.0-linux-armv6l/
	cp -R * /usr/local/
else
	mv node_server_2.service node_server.service
	echo "installing node js for armvl7/armvl8";
	curl -sL https://deb.nodesource.com/setup_13.x | sudo -E bash -
	apt install -y nodejs
fi

cd /home/pi/

git clone https://git.code.sf.net/p/openocd/code openocd-code
cd openocd-code/
./bootstrap
./configure --enable-sysfsgpio --enable-bcm2835gpio
make
make install

cd /home/pi/

# git clone https://github.com/tronicbaumi/RaspiPgmLog.git raspipgmlog

cd raspipgmlog/firmware/picberry/
if [ "$device" = "rpizero" ] ; then
	make raspberrypi
else
	make raspberrypi2
fi
make install

cd /home/pi/

cd raspipgmlog/software/
sh ./setup
cd src/
npm install
cd OpenocdCustomConfigFiles/
if [ "$device" = "rpizero" ] ; then
	mv rpi1.cfg rpi.cfg
else
	mv rpi2.cfg rpi.cfg
fi

cd /home/pi/

if [ $use_display -eq 1 ] ; then
	echo "installing display drivers"
	apt-get install libjpeg-turbo-progs libjpeg62-turbo-dev libopenjp2-7-dev libtiff5-dev zlib1g-dev python3-numpy -y
	pip3 install inky pillow
	cd /home/pi/raspipgmlog/software/
	sh ./setup_display
fi
