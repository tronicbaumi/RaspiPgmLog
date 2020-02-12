# RaspiPgmLog

## a Raspberrypi to in circuit serial program (ICSP) Microchip microcontrollers through the ICSP and JTAG Interface. (SNAP may be supported in the future)

Additionally an Arduino can be connected to the Raspberrypi for datalogging.
RaspiPgmLog uses a web based userinterface which allows to use any kind of browser to upload a HEX file to program the device and to visualize the logged data.	

Due to the Raspberry's WIFI interface this solution allows a simple way of isolation between the PC and the target application.
PICBERRY, openOCD and pymcuprog are used.

# Contents

- [RaspiPgmLog](#raspipgmlog)
  - [a Raspberrypi to in circuit serial program (ICSP) Microchip microcontrollers through the ICSP and JTAG Interface. (SNAP may be supported in the future)](#a-raspberrypi-to-in-circuit-serial-program-icsp-microchip-microcontrollers-through-the-icsp-and-jtag-interface-snap-may-be-supported-in-the-future)
- [Contents](#contents)
- [Setup RaspiPgmLog](#setup-raspipgmlog)
  - [Initial setup](#initial-setup)
  - [Setup via setup-script](#setup-via-setup-script)
  - [Setup by hand](#setup-by-hand)
  - [Final configuration](#final-configuration)
  - [Wiring it up](#wiring-it-up)
      - [Jtag / SWD (for OpenOCD):](#jtag--swd-for-openocd)
      - [ICSP 6-Pin Row (for Picberry):](#icsp-6-pin-row-for-picberry)
      - [ICSP RJ11 (for Picberry):](#icsp-rj11-for-picberry)
  - [Adding the logger device](#adding-the-logger-device)
- [Setup InkyPHAT Display](#setup-inkyphat-display)

# Setup RaspiPgmLog

## Initial setup

Download Raspbian from the official website (https://www.raspberrypi.org/downloads/raspbian/) and flash it onto an SD card. I used the “Lite” version. 
Log in on your raspi, the standard user is ‘pi’ and the standard password is ‘raspberry’.

Note that the standard keyboard layout is the UK style.
Information on how to change user and password can be found here: https://www.raspberrypi.org/documentation/linux/usage/users.md
Then type in the command line:

```bash
sudo raspi-config
```

A Gui will appear, where you can change some option:
* Under `Localization Options` you can change the keyboard layout if you want.
* Under `Network Options` you can connect you pi to Wi-Fi (this is necessary if you have no ethernet connection. You need to be connected to the internet!!!)
* Finally, you need to enable `Serial` in `Interfacing Options`. When enabling it, you will also be asked if you want to enable command line access via serial, make sure to read carefully and select `No` for that option, to increase security

If you want to install everything by hand, skip to [setup by hand](#setup-by-hand). Else read on, how to install all via the install script:

## Setup via setup-script

Type in your command line:

```bash
sudo apt-get install git
git clone https://github.com/tronicbaumi/RaspiPgmLog.git raspipgmlog
cd raspipgmlog/software/
sudo sh install_raspiPgmLog.sh --device [device] [--no-display]
```

For `[device]` insert the device you are using. The available options are: `rpi2` for Raspberry Pi 2 and greater and `rpizero` for Raspberry Pi Zero and Raspberry Pi 1. The `--no-diplay` option is optional and is only required, if you are not using the inkyPHAT display.

The command for the recommended hardware, the Raspberry Pi Zero with an inkyPHAT display attached, would then look like this: 

```bash
sudo sh install_raspiPgmLog.sh --device rpizero
```

To read about the final configuration options, skip to [final configuration](#final-configuration).

## Setup by hand

You have chosen to install the whole project by hand.
Start by typing in the command line:

```bash
sudo apt-get update
sudo apt-get upgrade -y
sudo apt-get dist-upgrade -y
```

Hit enter after every command.
Now the raspi is up to date and configure, now we need to install all dependencies for the Project:
Type in command line:

```bash
sudo apt-get install python3 python3-pip git autoconf libtool make pkg-config libusb-1.0-0 libusb-1.0-0-dev -y
pip3 install pyserial
```

We also need nodejs. This is a little more complicated to install:
First we need to determine which chip our raspi is using. Type:

```bash
uname -m
```

It will return something like this: `armv6l` or `armv7l` or `armv8l` (Output might differ, important is the version number.  
If it says `v6`, follow the steps below:

```bash
wget https://nodejs.org/download/release/v11.15.0/node-v11.15.0-linux-armv6l.tar.gz
tar -xzf node-v11.15.0-linux-armv6l.tar.gz
cd node-v11.15.0-linux-armv6l/
sudo cp -R * /usr/local/
```

Else, if it says `v7` or `v8`, follow these steps:

```bash
curl -sL https://deb.nodesource.com/setup_13.x | sudo -E bash –
sudo apt-get install -y nodejs
```

Now Test if NodeJs and Npm are installed correctly. Type in command line:

```bash
node -v
npm -v
```

If they return a version number, everything is fine.

Now we need the software to program/flash the target chips. 
First the Software for Cortex-M chips:
Type:

```bash
git clone https://git.code.sf.net/p/openocd/code openocd-code
cd openocd-code
./bootstrap
./configure --enable-sysfsgpio --enable-bcm2835gpio
make
sudo make install
cd ..
```

Now we need to clone the git-repository of the RaspiPgmLog project.
Type in command line:

```bash
git clone https://github.com/tronicbaumi/RaspiPgmLog.git raspipgmlog
```

Here we have the software for dsPICs and PICs:
Type in command line:

```bash
cd  raspipgmlog/firmware/picberry/
make [TARGET]
```

where you replace `[TARGET]` by `raspberrypi` if you use a Raspberry Pi v1 or Zero, or by `raspberrypi2`, if you use a Raspberry v2 or v3.
Then type:

```bash
sudo make install
cd ..
```

Finally, we come to the actual project.
Type in command line:

```bash
cd ~/raspipgmlog/software
sudo sh ./setup
cd src/
npm install
```

One final step needs to be done, to be able to use everything correctly. Type in command line:

```bash
cd /home/pi/raspipgmlog/software/src/OpenocdCustomConfigFiles
```

If you use a Raspberry Pi Zero or Raspberry Pi 1, type:

```bash
sudo mv rpi1.cfg rpi.cfg
```

Else, if you are on Raspberry Pi 2 or greater, type:

```bash
sudo mv rpi2.cfg rpi.cfg
```

## Final configuration

Now you can open a Browser on a computer on the same Network as the raspi and type [http://[your raspis ip address]:3000](). You can find out what it is, by typing:

```bash
hostname -I
```

in your raspis command line, or by following the documentation for [setting up the InkyPHAT display](#setup-inkyphat-display) down below.
Now you should see a login form. The standard password is `pass`, you can change it by going to your raspis command line and typing:

```bash
sudo nano /home/pi/raspipgmlog/software/src/datastorage.php
```

Change the `password` variable to the password you like and save the file.
The last thing to now is, to do the physical wiring, if you didn’t do it already. If you want the full functionality, you need all the connections listed below:

## Wiring it up

#### Jtag / SWD (for OpenOCD):

| Connector pin | Connection name | Raspberry Pi GPIO / pin |
| :-----------: | :-------------: | :---------------------: |
| 1 | MCLR | GPIO18 / pin12 |
| 2 | none | none |
| 3	| GND |	GND / pin14 |
| 4	| TDO |	GPIO22 / pin15 |
| 5	| TCK / SWDCLK | GPIO23 / pin16 |
| 6	| none | none |
| 7	| TDI | GPIO24 / pin18 |
| 8	| TMS / SWDIO | GPIO25 / pin22 |

#### ICSP 6-Pin Row (for Picberry):

| Connector pin | Connection name | Raspberry Pi GPIO / pin |
| :-----------: | :-------------: | :---------------------: |
| 1 | MCLR | GPIO12 / pin32 |
| 2	| 3.3V | 3.3V / pin1 |
| 3	| GND | GND / pin30 |
| 4	| PGD | GPIO6 / pin31 |
| 5	| PGC | GPIO5 / pin30 |
| 6	| none | none |
 
#### ICSP RJ11 (for Picberry):

| Connector pin | Connection name | Raspberry Pi GPIO / pin |
| :-----------: | :-------------: | :---------------------: |
| 1 | MCLR | GPIO21 / pin40 |
| 2	| 3.3V | 3.3V / pin1 |
| 3	| GND | GND / pin39 |
| 4	| PGD | GPIO20 / pin38 |
| 5	| PGC | GPIO26 / pin37 |
| 6	| none |none |

You can see the pinout for the Raspberry Pi here: https://pinout.xyz/ or you can look at the picture below:

![raspi pinout](https://www.elektronik-kompendium.de/sites/raspberry-pi/fotos/raspberry-pi-15b.jpg)

## Adding the logger device

The logger firmware was written for Arduino, so you need an Arduino microcontroller board.

If you did not already, download the Arduino Ide from here: https://www.arduino.cc/en/main/software, load the sketch from the `uart_logger` folder in the `firmware` folder and programm your Arduino board with it. A turtorial how to programm an Arduino Board can be found here: https://www.arduino.cc/en/Guide/Environment#toc9

Now you need to connect the Arduino board to the Raspberry Pi over their serial ports:

|   | Arduino Uno | Raspberry Pi |
|---|:-----------:|:------------:|
| Name | TXD <br> RXD | RXD <br> TXD |
| Pin | D1 <br> D0 | GPIO15 (10) <br> GPIO14 (8) |

The Arduino will now send the data from six analogue ports and the status of five digital pwm pins, that are configured to output pwm signals. The pins can be controlled from the graph page on the website. 

If you followed this documentation closely, everything should work now.

__Hooray, you successfully made a programming device out of your raspi.__ 
 
# Setup InkyPHAT Display

If you have an InkyPHAT display to mount on your raspi, this documentation will tell you how to set it up to display the hostname and the ip address of the raspi.
First, we need to do some more configuring on the raspi. Type in command line:

```bash
sudo raspi-config
```

Go to `Interfacing Options` and enable `SPI` and `I2C`.
Now install all dependencies. Type in raspi command line:

```bash
sudo apt-get install libjpeg-turbo-progs libjpeg62-turbo-dev zlib1g-dev python3-numpy -y
```

Now we can install the main libraries for the display. Type in command line:

```bash
sudo pip3 install inky pillow
```

Then we can setup the display to display the hostname and the ip address on boot.
Type in command line:

```bash
cd /home/pi/raspipgmlog/software
sudo sh ./setup_display
```

Then reboot:

```bash
sudo reboot
```

Now the display should show the hostname and the ip address of the raspi. 
 
