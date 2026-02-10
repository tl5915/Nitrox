Nitrox Analyser (Divesoft DNA clone)

Updated from basic version, use ADS1110 ADC for better accuracy. Use ESP32C3 board with OLED to show oxygen percentage and voltage. Include a supercapacitor for power, can be used unpluged now.


Material required:

Sensor tube to be 3D printed
Screen protector - transparent acrylic folder page, cut to approriate size
Oxygen Cell (any 3-Pin Molex type, e.g. Vandagraph R-22VAN, retired sensors from Diversoft/Kiss/XCCR)
ESP32-C3 Development Board with 0.42 Inch OLED Display (https://amzn.eu/d/0FZCRB2)
ADS1110 Analog To Digital Converter Module (https://ebay.us/m/G5uvKG)
Switch (https://amzn.eu/d/fneWX5r)
M2 x 3mm screw (https://amzn.eu/d/fZkoRWB)
5.5V 5F H-Type Button Super Capacitor (https://amzn.eu/d/1qX6MRr)
5 ohm resistor
Prototype Board (cut to 8x8)
8-Pin Female Headers
JST-XH (2.54mm) 3-Pin Female Connector
cut the locking arrows off to fit Molex header on oxygen cell
Circuit connection:

ADS1110 VCC connects to ESP32C3 V3 pin (3V3)
ADS1110 GND connects to ESP32C3 GD pin (Gnd)
ADS1110 SCL connects to ESP32C3 pin 6 (GPIO6)
ADS1110 SDA connects to ESP32C3 pin 5 (GPIO5)
ADS1110 VIN- connects to Oxygen Cell pin 1 (-)
ADS1110 VIN+ connects to Oxygen Cell pin 3 (+)
ESP32C3 V5 pin (Vcc) connects to one end of the switch
Other end of switch connects to + side of the super capacitor
ESP32C3 GD pin (Gnd) connects to - side of the super capacitor (arrow side)
Assemble

wire all electonics
Place super capacitor in the seat in sensor tube, snap on electronics tube
Cut acylic sheet to appropriate size and place in the slot in electronics tube, secure it by pushing in the screen protector plug
Place ESP32C3 in electrnics tube, OLED side towards screen opening, USB-C side towards outside
Screw the switch onto end cap with two M2 x 3mm screws
Thread a lanyard on end cap through the slots if desired
Align USB-C with the opening on end cap, snap on end cap
Screw oxygen cell on front cap
Connect molex connector to oxygen cell
Snap on front cap
UI instruction:

Charge the super capacitor with USB-C cable with switch on, this will provide about 10 minutes working time for the analyser. Switch off after charging
Switch on in air, analyser will calibrate automatically, screen shows 21% after air calibration (available when voltage is between 5 to 15 mV)
To calibrate with 99% oxygen, place censor on oxygen cylinder for 30 seconds when switched off. Switch on, screen shows 99% after oxygen calibration (available when voltage > 30 mV)