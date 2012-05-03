/*******************************************************************************
*                                                                              *
*  Arduino UNO sketch file.                                                    *
*                                                                              *
*  Copyright (C) 2012 Kirill Chuvilin.                                         *
*  All rights reserved.                                                        *
*  Contact: Kirill Chuvilin (kirill.chuvilin@gmail.com, kirik-ch.ru)           *
*                                                                              *
*  This file is part of the QtDruino project.                                  *
*                                                                              *
*  $QT_BEGIN_LICENSE:GPL$                                                      *
*  You may use this file under the terms of the GNU General Public License     *
*  as published by the Free Software Foundation; version 3 of the License.     *
*                                                                              *
*  This file is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
*  GNU General Public License for more details.                                *
*                                                                              *
*  You should have received a copy of the GNU General Public License           *
*  along with this package; if not, write to the Free Software                 *
*  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.   *
*  $QT_END_LICENSE$                                                            *
*                                                                              *
*******************************************************************************/


#define ARDUINO_OUTPUT        0 //!< pin is used for output
#define ARDUINO_DIGITAL_INPUT 1 //!< pin is used only for digital input
#define ARDUINO_ANALOG_INPUT  2 //!< pin is used only for analog input
#define ARDUINO_INPUT         3 //!< pin is used both for digital and analog input (equils to DIGITAL_INPUT | ANALOG_INPUT)

#define ARDUINO_LOW  0
#define ARDUINO_HIGH 1

#define ARDUINO_DEFAULT      0 //!< the default analog reference of 5 volts (on 5V Arduino boards) or 3.3 volts (on 3.3V Arduino boards)
#define ARDUINO_EXTERNAL     1 //!< the voltage applied to the AREF pin (0 to 5V only) is used as the reference.
#define ARDUINO_INTERNAL     2 //!< an built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328 and 2.56 volts on the ATmega8 (not available on the Arduino Mega)
//#define ARDUINO_INTERNAL1V1  3 //!< a built-in 1.1V reference (Arduino Mega only)
//#define ARDUINO_INTERNAL2V56 4 //!< a built-in 2.56V reference (Arduino Mega only)

// command types
#define ARDUINO_COMMAND_NULL                  0  //!< just do nothing
#define ARDUINO_COMMAND_STOP                  1  //!< stop scanning pin values
#define ARDUINO_COMMAND_RESET                 2  //!< reset device state
#define ARDUINO_COMMAND_PINMODE               3  //!< pinMode
#define ARDUINO_COMMAND_DIGITALWRITE          4  //!< digitalWrite
#define ARDUINO_COMMAND_ANALOGWRITE           5  //!< analogWrite
#define ARDUINO_COMMAND_ANALOGREFERENCE       6  //!< analogReference
#define ARDUINO_COMMAND_PINANALOGSENSETIVITY  7  //!< set analog sensetivity for pin
#define ARDUINO_COMMAND_PINANALOGSMOOTHFACTOR 8  //!< set analog smooth factor for pin
#define ARDUINO_COMMAND_TONE                  9  //!< tone
#define ARDUINO_COMMAND_NOTONE                10 //!< noTone

// info types
#define ARDUINO_INFO_NULL            0 //!< no info
#define ARDUINO_INFO_PINMODE         1 //!< mode of pin
#define ARDUINO_INFO_PINDIGITALVALUE 2 //!< digital pin value
#define ARDUINO_INFO_PINANALOGVALUE  3 //!< analog pin value


volatile boolean isRunning = false; //!< is running or not
volatile word loopDelay = 20000; //!< microseconds delay on each loop
volatile word serialWriteDelay = 0; //!< microseconds delay on bytes serial write
volatile byte pinModes[20]; //!< mode of each pin
volatile byte pinDigitalValues[20]; //!< readed digital value of each pin
volatile word pinAnalogValues[20]; //!< readed analog value of each pin
volatile word pinAnalogSensetivities[20]; //!< threshold difference between the readed value and the current value of each pin, above which the readed value read will be updated
volatile byte pinAnalogSmoothFactors[20]; //!< proportion of old value ​​(from 0 to 255) for each pin, which together with the readed value forms a new value
volatile float pinAnalogCurrentValues[20]; //!< current smothed values of each pin


/*!
 * \brief Whait a byte on serial port and read it.
 * \returns readed byte
 */ 
byte serialReadByte() {
    while (!(Serial.available() > 0)); // whait for next byte
    return Serial.read(); // return readed byte
}


/*!
 * \brief Write a byte to serial port.
 * \param b the byte to be written
 */ 
void serialWriteByte(byte b) {
    Serial.write(b); // write the byte to serial port
    if (serialWriteDelay) delayMicroseconds(serialWriteDelay); // whait before writing next byte
}


/*!
 * \brief Whait a word on serial port and read it.
 * \returns readed word
 */ 
word serialReadWord() {
    byte low = serialReadByte(); // low byte
    byte high = serialReadByte(); // high byte
    return low + 256 * high;
}


/*!
 * \brief Write a word to serial port.
 * \param w the word to be written
 */ 
void serialWriteWord(word w) {
    Serial.write(w % 256); // low byte
    Serial.write(w / 256); // high byte
}


/*!
 * \brief Whait an unsigned long on serial port and read it.
 * \returns readed unsigned long
 */ 
unsigned long serialReadULong() {
    word low = serialReadWord(); // low word
    word high = serialReadWord(); // high word
    return low + 65536 * high;
}


/*!
 * \brief Write a mode of a pin to serial port.
 * \param pin the number of the pin
 */ 
void serialWritePinMode(byte pin) {
    serialWriteByte(ARDUINO_INFO_PINMODE);
    serialWriteByte(pin);
    serialWriteByte(pinModes[pin]);
}


/*!
 * \brief Write a digital value on a pin to serial port.
 * \param pin the number of the pin
 */ 
void serialWritePinDigitalValue(byte pin) {
    serialWriteByte(ARDUINO_INFO_PINDIGITALVALUE);
    serialWriteByte(pin);
    serialWriteByte(pinDigitalValues[pin]);
}


/*!
 * \brief Write an analog value on a pin to serial port.
 * \param pin the number of the pin
 */ 
void serialWritePinAnalogValue(byte pin) {
    serialWriteByte(ARDUINO_INFO_PINANALOGVALUE);
    serialWriteByte(pin);
    serialWriteWord(pinAnalogValues[pin]);
}


/*!
 * \brief Reset device state.
 * \param loopDelayMicroseconds microseconds delay on each loop
 * \param writeDealyMicroseconds microseconds delay on bytes serial write
 */ 
void commandReset(word loopDelayMicroseconds, word writeDealyMicroseconds) {
    loopDelay = loopDelayMicroseconds;
    serialWriteDelay = writeDealyMicroseconds;
    for (byte pin = 2; pin <= 19; pin++) {
        pinAnalogSensetivities[pin] = 0; // exact sensetivity
        pinAnalogSmoothFactors[pin] = 0; // without smooth
        commandPinMode(pin, ARDUINO_OUTPUT);
    }
    isRunning = true;
}


/*!
 * \brief Configure the specified pin to behave either as an input or an output.
 * \param pin the number of the pin whose mode will be set
 * \param mode the type of behavior
 */ 
void commandPinMode(byte pin, byte mode) {
    if (mode & ARDUINO_INPUT) { // some of inut modes
        pinMode(pin, INPUT);
        pinModes[pin] = mode;
        if (mode & ARDUINO_DIGITAL_INPUT) {
            pinDigitalValues[pin] = digitalRead(pin);
        } else {
            pinDigitalValues[pin] = 0;
        }
        if (mode & ARDUINO_ANALOG_INPUT) {
            pinAnalogCurrentValues[pin] = analogRead(pin);
        } else {
            pinAnalogCurrentValues[pin] = 0;
        }
    } else { // not inut mode - output mode
        digitalWrite(pin, LOW);
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        pinModes[pin] = ARDUINO_OUTPUT;
        pinDigitalValues[pin] = ARDUINO_LOW;
        pinAnalogCurrentValues[pin] = 0;
    }
    pinAnalogValues[pin] = pinAnalogCurrentValues[pin];
    serialWritePinMode(pin);
    serialWritePinDigitalValue(pin);
    serialWritePinAnalogValue(pin);
}


/*!
 * \brief Write a value to a digital pin.
 * \param pin the number of the pin to write to
 * \param value ARDUINO_HIGH  or ARDUINO_LOW
 */
void commandDigitalWrite(byte pin, byte value) {
    if (value == ARDUINO_LOW) {
        digitalWrite(pin, LOW);
    } else {
        digitalWrite(pin, HIGH);
    }
}


/*!
 * \brief Writes an analog value (PWM wave) to a pin.
 * \param pin the number of the pin to write to
 * \param value the duty cycle: between 0 (always off) and 255 (always on)
 */
void commandAnalogWrite(byte pin, word value) {
    analogWrite(pin, value);
}


/*!
 * \brief Configures the reference voltage used for analog input (i.e. the value used as the top of the input range).
 * \param reference the reference type
 */
void commandAnalogReference(byte reference) {
    switch (reference) {
    case ARDUINO_INTERNAL:
        analogReference(INTERNAL);
        break;
//    case ARDUINO_INTERNAL1V1:
//        analogReference(INTERNAL1V1);
//        break;
//    case ARDUINO_INTERNAL2V56:
//        analogReference(INTERNAL2V56);
//        break;
    case ARDUINO_EXTERNAL:
        analogReference(EXTERNAL);
        break;
    default:
        analogReference(DEFAULT);
    }
}


/*!
 * \brief Set sensetivity of an analog pin.
 * \param pin the number of pin whose sensetivity will be set
 * \param sensetivity the sensetivity value
 */
void commandPinAnalogSensetivity(byte pin, word sensetivity) {
    pinAnalogSensetivities[pin] = sensetivity;
}


/*!
 * \brief Set smooth fator of an analog pin.
 * \param pin the number of pin whose smooth factor will be set
 * \param factor the smooth factor value
 */
void commandPinAnalogSmoothFactor(byte pin, byte factor) {
    pinAnalogSmoothFactors[pin] = factor;
}


/*!
 * \brief Generates a square wave of the specified frequency (and 50% duty cycle) on a pin.
 * \param pin the pin on which to generate the tone
 * \param frequency the frequency of the tone in hertz
 * \param duration the duration of the tone in milliseconds, if 0 the wave continues until a call to noTone()
 */
void commandTone(byte pin, word frequency, unsigned long duration) {
    if (duration > 0) {
        tone(pin, frequency, duration);
    } else {
        tone(pin, frequency);
    }
}


/*!
 * \brief Stops the generation of a square wave triggered by tone(). Has no effect if no tone is being generated.
 * \param pin the pin on which to stop generating the tone
 */
void commandNoTone(byte pin) {
    noTone(pin);
}


void setup() {
    Serial.begin(9600);
    commandReset(0, 0);
}


void serialEvent() {
//void serialRead() {
    byte command = Serial.read(); // command
    switch (command) { // depends on command
    case ARDUINO_COMMAND_STOP: //!stop scanning pin values
        isRunning = false;
        Serial.flush();
        break;
    case ARDUINO_COMMAND_RESET: { // reset device state
        word loopDelayMicroseconds = serialReadWord(); // microseconds delay on each loop
        word writeDealyMicroseconds = serialReadWord(); // microseconds delay on bytes serial write
        commandReset(loopDelayMicroseconds, writeDealyMicroseconds);
        break;
    }
    case ARDUINO_COMMAND_PINMODE: { // pinMode
        byte pin = serialReadByte(); // pin for command
        byte mode = serialReadByte(); // mode of pin
        commandPinMode(pin, mode);
        break;
    }
    case ARDUINO_COMMAND_DIGITALWRITE: { // digitalWrite
        byte pin = serialReadByte(); // pin for command
        byte value = serialReadByte(); // value byte
        commandDigitalWrite(pin, value);
        break;
    }
    case ARDUINO_COMMAND_ANALOGWRITE: { // analogWrite
        byte pin = serialReadByte(); // pin for command
        word value = serialReadWord(); // value word
        commandAnalogWrite(pin, value);
        break;
    }
    case ARDUINO_COMMAND_ANALOGREFERENCE: { // analogReference
        byte reference = serialReadByte(); // refrance type
        commandAnalogReference(reference); // the reference type
        break;
    }
    case ARDUINO_COMMAND_PINANALOGSENSETIVITY: { // set analog sensetivity
        byte pin = serialReadByte(); // pin for command
        word sensetivity = serialReadWord(); // sensetivity value
        commandPinAnalogSensetivity(pin, sensetivity);
        break;
    }
    case ARDUINO_COMMAND_PINANALOGSMOOTHFACTOR: { // set analog smooth factor
        byte pin = serialReadByte(); // pin for command
        byte factor = serialReadByte(); // factor value
        commandPinAnalogSmoothFactor(pin, factor);
        break;
    }
    case ARDUINO_COMMAND_TONE: { // tone
        byte pin = serialReadByte(); // the pin on which to generate the tone
        word frequency = serialReadWord(); // the frequency of the tone in hertz
        unsigned long duration = serialReadULong(); // the duration of the tone in milliseconds
        commandTone(pin, frequency, duration);
        break;
    }
    case ARDUINO_COMMAND_NOTONE: { // tone
        byte pin = serialReadByte(); // the pin on which to stop generating the tone
        commandNoTone(pin);
        break;
    }
    default:
        break;
    }
}


void loop() {
//    if (Serial.available() > 0) serialRead();
    if (isRunning) {
        for (byte pin = 2; pin <= 19; pin++) {
            if (pinModes[pin] & ARDUINO_DIGITAL_INPUT) { // pin used for digital input
                byte digitalValue = digitalRead(pin);
                if (digitalValue != pinDigitalValues[pin]) { // value changed
                    pinDigitalValues[pin] = digitalValue; // update value
                    serialWritePinDigitalValue(pin); // send new value to serial port
                }
            }
            if (pinModes[pin] & ARDUINO_ANALOG_INPUT) { // pin used for analog input
                pinAnalogCurrentValues[pin] = (pinAnalogSmoothFactors[pin] * pinAnalogCurrentValues[pin] + (255 - pinAnalogSmoothFactors[pin]) * analogRead(pin)) / 255; // new float value
                int analogValue = round(pinAnalogCurrentValues[pin]); // new int value
                if (abs(analogValue - pinAnalogValues[pin]) > pinAnalogSensetivities[pin]) { // value changed
                    pinAnalogValues[pin] = analogValue; // update value
                    serialWritePinAnalogValue(pin); // send new value to serial port
                }
            }
        }
    }
    if (loopDelay) delayMicroseconds(loopDelay);
}

