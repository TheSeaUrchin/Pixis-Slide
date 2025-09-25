# Pixis-Slide
Repository for my Embedded Musical Synthesis Project. The Pixis Slide is an 8 bit digital musical synthesizer.

## Features:
- 4 base waveforms:
     - Square
     - Triangle
     - Sawtooth
     - Sine
- Adjustable Low Frequency Oscillator
- Envelope Generator
- Pentatonic Scale Slide strip
- User friendly UI

## Housing Drawings:

![Drawing](/media/Pixis%20Drawing.png)
STL files for 3D printing are available under the [Releases](https://github.com/TheSeaUrchin/Pixis-Slide/releases) section.


## Electrical Components:

- ESP32 devkit-V1
- I2C LCD Screen
- Potentiometer
- 3w 8ohm Speaker
- Rotary Encoder (x4)
- Softpot Potentiometer (touch strip)
- PAM8403 Amplifier
- Mini USB Host Shield 2.0
- 10k ohm Resistors
- 7mm Push Buttons


## Wiring Diagrams:

### Full System:

![full_wiring](/media/overall_wiring.png)

### LCD Wiring:

![LCD_wiring](/media/LCD_wiring.png)

|LCD Pin  | ESP Pin |
| -------- | ------- |
| VCC | 5v/Vin|
| GND | GND|
| SCL | GPIO 22 |
| SDA | GPIO 21 |

### Encoder Wiring:

![Encoder_Wiring](/media/encoder_wiring.png)

| Encoder Number | Encoder DT pin| Encoder CLK pin |
| -------- | ------- | -----|
|1| GPIO 4 | GPIO 2|
|2| GPIO 13| GPIO 12|
|3| GPIO 15| GPIO 14|
|4| GPIO 27| GPIO 26|

### Button Wiring:

![Button_wiring](/media/button_wiring.png)

|Button|ESP Pin|
|--|--|
|Button 1|GPIO 32|
|Button 2|GPIO 35|
|Button 3|GPIO 34|
|Button 4|GPIO 33|
|Button 5|GPIO 39|
|GND|10 Kohm resistor to GND|


### USB Host shield wiring:

![Shield_wiring](/media/host_shield_wiring.png)

![Shield_diagram](/media/shield_diagram.png)

Note: due to poor design of the host shiled, the tread connecting Vbus to the chip must be cut, due to the chip opperating at 3.3v and not 5v from USB.

|USB Host Shield Pin  | ESP Pin |
| -------- | ------- |
| VCC | 3.3v |
| INT| GPIO 17|
| SS| GPIO 5|
| MOSI| GPIO 23|
| MISO| GPIO 19|
| CLK| GPIO 18|
| MAX_RST| 3.3V|
| GND| GND|
| VBUS| Vin/5v|

### Speaker wiring:

![Speaker_wiring](/media/speaker_wiring.png)

|Potentiometer Pin| Connection|
|---|---|
| VCC| ESP GPIO 25|
| OUT| PAM8403 Rin|
| GND| ESP GND|

|PAM8403 Pin| Connection|
|---|---|
|RIN| Potentiometer OUT|
|VCC| ESP Vin/5v|
|GND| ESP GND|
|R+| Speaker +|
|R-| Speaker -|


### Sofpot Potentiometer wiring:

![Softpot_wiring](/media/softpot_wiring.png)

|SoftPot pin| ESP Pin|
|--|--|
|VCC|3.3v|
|GND| GND |
|DOUT|GPIO 36|
|DOUT| 10 kohm Resistor to GND|