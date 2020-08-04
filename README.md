# arduino-tetris
Tetris game in arduino + 8x8x4 LED Matrix display and a joystick!

## Libraries
The library you will need is [LedControl](https://github.com/wayoda/LedControl).

## Usage
### Code
Open the .ino file in Arduino IDE, and upload it to your Arduino.
### Circuit Diagram
```cpp
/*
 ***********************************
 *        CIRCUIT DIAGRAM          *
 ***********************************
 *     
 *    --8x8x4 MATRIX LED--
 *  pin 12 -------> DIN(Data in)
 *  pin 11 -------> CLK(clock)
 *  pin 10 -------> LOAD
 *  +5V    -------> VCC
 *  GND    -------> GND
 *  
 *    --JOYSTICK--
 *  pin 7  -------> SW (joystick button pin)
 *  #         -     VRy (we dont use VRy pin of the joystick)
 *  pin A0 -------> VRx (joystick x axis pin)
 *  +5V    -------> VCC 
 *  GND    -------> GND
 *  
 *    --BUZZER--
 *  GND    -------> GND
 *  VCC    -------> A1
 */
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.
