#!/usr/bin/python3
#-*- coding: utf-8 -*-

import numpy
import subprocess
from PIL import Image, ImageDraw, ImageFont
from inky import InkyPHAT

hostname = str(subprocess.run(["hostname"], capture_output=True).stdout)
ip = str(subprocess.run(["hostname", "-I"], capture_output=True).stdout)

hostname = hostname.strip("b'\\n ")
ip = ip.strip("b'\\n ")

display = InkyPHAT("black")

font = ImageFont.truetype("/home/pi/.local/resources/NotoMono-Regular.ttf",12)

img = Image.new("L", (212,104), 0)
draw = ImageDraw.Draw(img)
draw.text((10,30), "Hostname: " + hostname, display.BLACK, font=font)
draw.text((10,50), "Ip: " + ip, display.BLACK, font=font)

display.set_image(img)
display.show()

