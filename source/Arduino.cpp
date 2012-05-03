/*******************************************************************************
*                                                                              *
*  Arduino device interface imlementation.                                     *
*                                                                              *
*  Copyright (C) 2012 Kirill Chuvilin.                                         *
*  All rights reserved.                                                        *
*  Contact: Kirill Chuvilin (kirill.chuvilin@gmail.com, kirik-ch.ru)           *
*                                                                              *
*  This file is a part of the QtDruino project.                                *
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


#include "Arduino.h"

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


Arduino::Arduino(const QString& inputPortName, const QString& outputPortName) {
    this->inputPort = new SerialPort(inputPortName); // input port object
    if (inputPortName == outputPortName || outputPortName.isEmpty()) { // use single port for input & output
        this->outputPort = this->inputPort; // the same object as input
    } else { // use different ports fot input and output
        this->outputPort = new SerialPort(outputPortName); // another output port object
    }
    QObject::connect(this->inputPort, SIGNAL(readyRead()), this, SLOT(serialRead())); // reads info when ready
}


Arduino::~Arduino() {
    if (this->inputPort == this->outputPort) { // single port for input & output
        delete this->inputPort;
    } else { // use different ports fot input and output
        delete this->inputPort;
        delete this->outputPort;
    }
}


bool Arduino::open() {
    if (this->inputPort == this->outputPort) { // single port for input & output
        if (!this->inputPort->open(QIODevice::ReadWrite)) return false;
        if (!this->inputPort->setRate(9600)) return false;
        if (!this->inputPort->setDataBits(SerialPort::Data8)) return false;
        if (!this->inputPort->setParity(SerialPort::NoParity)) return false;
        if (!this->inputPort->setStopBits(SerialPort::OneStop)) return false;
        if (!this->inputPort->setFlowControl(SerialPort::NoFlowControl)) return false;
    } else { // different ports for input and output
        if (!this->inputPort->open(QIODevice::ReadOnly)) return false;
        if (!this->inputPort->setRate(9600)) return false;
        if (!this->inputPort->setDataBits(SerialPort::Data8)) return false;
        if (!this->inputPort->setParity(SerialPort::NoParity)) return false;
        if (!this->inputPort->setStopBits(SerialPort::OneStop)) return false;
        if (!this->inputPort->setFlowControl(SerialPort::NoFlowControl)) return false;
        if (!this->outputPort->open(QIODevice::WriteOnly | QIODevice::Unbuffered)) return false;
        if (!this->outputPort->setRate(9600)) return false;
        if (!this->outputPort->setDataBits(SerialPort::Data8)) return false;
        if (!this->outputPort->setParity(SerialPort::NoParity)) return false;
        if (!this->outputPort->setStopBits(SerialPort::OneStop)) return false;
        if (!this->outputPort->setFlowControl(SerialPort::NoFlowControl)) return false;
    }
    this->reset();
    return true;
}


void Arduino::close() {
    this->inputPort->close();
    this->outputPort->close();
}


void Arduino::reset() {
    this->serialWriteByte(ARDUINO_COMMAND_STOP); // command id
    this->delay(10);
    while (!this->inputPort->atEnd()) this->inputPort->readAll(); // clear input
    this->inputBuffer.clear(); // clear port buffer
    this->pinModes.clear();
    this->digitalValues.clear();
    this->analogValues.clear();
    this->serialWriteByte(ARDUINO_COMMAND_RESET); // command id
    this->serialWriteWord(000);
    this->serialWriteWord(000);
}


void Arduino::delay(unsigned long msecs) {
    this->outputPort->waitForBytesWritten(msecs);
}


bool Arduino::pinMode(unsigned char pin, Arduino::PinMode mode) {
    if (!this->digitalValues.contains(pin)) return false; // false if the pin isn't available
    this->serialWriteByte(ARDUINO_COMMAND_PINMODE); // command ID
    this->serialWriteByte(pin); // pin for command
    this->serialWriteByte(mode); // mode of pin
    this->pinModes[pin] = mode; // update mode
    return true;
}


int Arduino::digitalRead(unsigned char pin) {
    if (!this->digitalValues.contains(pin)) return -1; // error if the digital pin isn't available
    return this->digitalValues[pin];
}


bool Arduino::digitalWrite(unsigned char pin, Arduino::DigitalValue value) {
    if (!this->digitalValues.contains(pin)) return false; // false if the digital pin isn't available
    this->serialWriteByte(ARDUINO_COMMAND_DIGITALWRITE); // command id
    this->serialWriteByte(pin); // pin for command
    this->serialWriteByte(value); // value byte
    this->digitalValues[pin] = value; // update value
    emit this->digitalValueChanged(pin, value);
    return true;
}


int Arduino::analogRead(unsigned char pin) {
    if (!this->analogValues.contains(pin)) return -1; // error if the analog pin isn't available
    return this->analogValues[pin];
}


bool Arduino::analogWrite(unsigned char pin, unsigned short value) {
    if (!this->analogValues.contains(pin)) return false; // false if the analog pin isn't available
    this->serialWriteByte(ARDUINO_COMMAND_ANALOGWRITE); // command id
    this->serialWriteByte(pin); // pin for command
    this->serialWriteWord(value); // value word
    this->analogValues[pin] = value; // update value
    emit this->analogValueChanged(pin, value);
    return true;
}


void Arduino::analogReference(Arduino::AnalogReference reference) {
    this->serialWriteByte(ARDUINO_COMMAND_ANALOGREFERENCE); // command id
    this->serialWriteByte(reference); // the reference type
}


bool Arduino::pinAnalogSensetivity(unsigned char pin, unsigned short sensetivity) {
    if (!this->analogValues.contains(pin)) return false; // false if the analog pin isn't available
    this->serialWriteByte(ARDUINO_COMMAND_PINANALOGSENSETIVITY); // command id
    this->serialWriteByte(pin); // pin for command
    this->serialWriteWord(sensetivity); // sensetivity value
    return true;
}


bool Arduino::pinAnalogSmoothFactor(unsigned char pin, float factor) {
    if (!this->analogValues.contains(pin)) return false; // false if the analog pin isn't available
    this->serialWriteByte(ARDUINO_COMMAND_PINANALOGSMOOTHFACTOR); // command id
    this->serialWriteByte(pin); // pin for command
    this->serialWriteByte(factor * 255); // factor value
    return true;
}


bool Arduino::tone(unsigned char pin, unsigned short frequency, unsigned long duration) {
    if (!this->analogValues.contains(pin)) return false; // false if the analog pin isn't available
    this->serialWriteByte(ARDUINO_COMMAND_TONE); // command id
    this->serialWriteByte(pin); // the pin on which to generate the tone
    this->serialWriteWord(frequency); // the frequency of the tone in hertz
    this->serialWriteULong(duration); // the duration of the tone in milliseconds
    return true;
}


bool Arduino::noTone(unsigned char pin) {
    if (!this->analogValues.contains(pin)) return false; // false if the analog pin isn't available
    this->serialWriteByte(ARDUINO_COMMAND_NOTONE); // command id
    this->serialWriteByte(pin); // the the pin on which to stop generating the tone
    return true;
}


unsigned char Arduino::serialReadByte() {
    return this->inputBuffer.takeFirst();
}


void Arduino::serialWriteByte(unsigned char b) {
    this->outputPort->write((char*)(&b), 1);
}


unsigned short Arduino::serialReadWord() {
    unsigned char lowByte = this->inputBuffer.takeFirst(); // low byte
    unsigned char highByte = this->inputBuffer.takeFirst(); // high byte
    return lowByte + 256 * highByte;
}


void Arduino::serialWriteWord(unsigned short w) {
    this->serialWriteByte(w % 260); // low byte
    this->serialWriteByte(w / 260); // high byte
}


void Arduino::serialWriteULong(unsigned long ul) {
    this->serialWriteWord(ul % 65536); // low word
    this->serialWriteWord(ul / 65536); // high word
}


void Arduino::inputBufferUpdate() {
    int nBytes = this->inputPort->bytesAvailable(); // available bytes number
    char* bytes = new char[nBytes]; // memory for input bytes
    this->inputPort->read(bytes, nBytes); // read bytes from input port
    for (int iByte = 0; iByte < nBytes; iByte++) { // for all readed bytes
        this->inputBuffer.append((unsigned char)bytes[iByte]); // add new byte to buffer
    }
    delete bytes; // clears memory
}


void Arduino::serialRead() {
    while (!this->inputPort->atEnd()) {
        this->inputBufferUpdate();
        bool needMoreBytes = false; // need to read from serial port
        while (this->inputBuffer.length() > 0 && !needMoreBytes) { // buffer contains full commands
            switch (this->inputBuffer[0]) {
            case ARDUINO_INFO_PINMODE: {
                if (this->inputBuffer.size() < 3) {
                    needMoreBytes = true;
                    break;
                }
                this->serialReadByte(); // command id
                unsigned char pin = this->serialReadByte(); // pin number
                this->pinModes[pin] = (Arduino::PinMode)this->serialReadByte(); // pin mode
                break;
            }
            case ARDUINO_INFO_PINDIGITALVALUE: {
                if (this->inputBuffer.size() < 3) {
                    needMoreBytes = true;
                    break;
                }
                this->serialReadByte(); // command id
                unsigned char pin = this->serialReadByte(); // pin number
                this->digitalValues[pin] = (Arduino::DigitalValue)this->serialReadByte(); // pin value
                emit this->digitalValueChanged(pin, this->digitalValues[pin]);
                break;
            }
            case ARDUINO_INFO_PINANALOGVALUE: {
                if (this->inputBuffer.size() < 4) {
                    needMoreBytes = true;
                    break;
                }
                this->serialReadByte(); // command id
                unsigned char pin = this->serialReadByte(); // pin number
                this->analogValues[pin] = this->serialReadWord(); // pin value
                emit this->analogValueChanged(pin, this->analogValues[pin]);
                break;
            }
            default:
                this->serialReadByte(); // unknown command id
                break;
            }
        }
    }
}


void Arduino::serialWrite(unsigned char *data, int length) {
    for (int iByte = 0; iByte < length; iByte++) {
        this->serialWriteByte(data[iByte]);
    }
}
