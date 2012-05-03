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


#ifndef _Arduino_h_
#define _Arduino_h_

#include "serialport.h"
#include <QMap>
#include <QList>


/*!
 * \brief Arduino device interface class.
 */
class Arduino : public QObject {
    Q_OBJECT

public:
    enum PinMode { //!< modes of pins
        OUTPUT        = 0, //!< pin is used for output
        DIGITAL_INPUT = 1, //!< pin is used only for digital input
        ANALOG_INPUT  = 2, //!< pin is used only for analog input
        INPUT         = 3  //!< pin is used both for digital and analog input (equils to DIGITAL_INPUT | ANALOG_INPUT)
    };

    enum DigitalValue { //!< values of digital pin
        LOW  = 0,
        HIGH = 1
    };

    enum AnalogReference { //!< analog referances
        DEFAULT      = 0, //!< the default analog reference of 5 volts (on 5V Arduino boards) or 3.3 volts (on 3.3V Arduino boards)
        EXTERNAL     = 1, //!< the voltage applied to the AREF pin (0 to 5V only) is used as the reference.
        INTERNAL     = 2, //!< an built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328 and 2.56 volts on the ATmega8 (not available on the Arduino Mega)
        INTERNAL1V1  = 3, //!< a built-in 1.1V reference (Arduino Mega only)
        INTERNAL2V56 = 4  //!< a built-in 2.56V reference (Arduino Mega only)
    };

    /*!
     * \brief Constructor.
     * \param inputPortName string with name of input serial port
     * \param outputPortName string with name of output serial port
     */
    Arduino(const QString& inputPortName, const QString& outputPortName = QString::null);

    /*!
     * \brief Destructor.
     */
    ~Arduino();

public slots:
    /*!
     * \brief Open connection to serial port and start communication.
     * \return true if port was opened, false if something wrong
     */
    virtual bool open();

    /*!
     * \brief Close connection to serial port.
     */
    void close();

    /*!
     * \brief Reset device state.
     */
    void reset();

    /*!
     * \brief Pause the program for the amount of time.
     * \param msecs the number of milliseconds to pause
     */
    void delay(unsigned long msecs);

    /*!
     * \brief Configure the specified pin to behave either as an input or an output.
     * \return true if the mode was set, false if not
     * \param pin the number of the pin whose mode will be set
     * \param mode the type of behavior
     */
    bool pinMode(unsigned char pin, Arduino::PinMode mode);

    /*!
     * \brief Read a value from a digital pin.
     * \return ARDUINO_HIGH or ARDUINO_LOW if pin defined, -1 if not
     * \param pin the number of the digital pin to read from
     */
    int digitalRead(unsigned char pin);

    /*!
     * \brief Write a value to a digital pin.
     * \return true if the value was written, false if not
     * \param pin the number of the pin to write to
     * \param value ARDUINO_HIGH or ARDUINO_LOW
     */
    bool digitalWrite(unsigned char pin, Arduino::DigitalValue value);

    /*!
     * \brief Read a value from an analog pin.
     * \return from 0 to 1023
     * \param pin the number of the analog input pin to read from
     */
    int analogRead(unsigned char pin);

    /*!
     * \brief Write an analog value (PWM wave) to a pin.
     * \return true if the value was written, false if not
     * \param pin the number of the pin to write to
     * \param value the duty cycle: between 0 (always off) and 255 (always on)
     */
    bool analogWrite(unsigned char pin, unsigned short value);

    /*!
     * \brief Configure the reference voltage used for analog input (i.e. the value used as the top of the input range).
     * \param reference the reference type
     */
    void analogReference(AnalogReference reference);

    /*!
     * \brief Set sensetivity of an analog pin.
     * \return true if the sensetivity was set, false if not
     * \param pin the number of pin whose sensetivity will be set
     * \param sensetivity the sensetivity value
     */
    bool pinAnalogSensetivity(unsigned char pin, unsigned short sensetivity);

    /*!
     * \brief Set smooth fator of an analog pin.
     * \return true if the smooth factor was set, false if not
     * \param pin the number of pin whose smooth factor will be set
     * \param factor the smooth factor value (from 0.0 to 1.0)
     */
    bool pinAnalogSmoothFactor(unsigned char pin, float factor);

    /*!
     * \brief Generates a square wave of the specified frequency (and 50% duty cycle) on a pin.
     * \return true if the tone was generated, false if not
     * \param pin the pin on which to generate the tone
     * \param frequency the frequency of the tone in hertz
     * \param duration the duration of the tone in milliseconds, if 0 the wave continues until a call to noTone()
     */
    bool tone(unsigned char pin, unsigned short frequency, unsigned long duration = 0);

    /*!
     * \brief Stops the generation of a square wave triggered by tone(). Has no effect if no tone is being generated.
     * \return true if the tone was stopped, false if not
     * \param pin the pin on which to stop generating the tone
     */
    bool noTone(unsigned char pin);

signals:
    /*!
     * \brief Emits when a pin digital value changes.
     * \param pin the number of pin, on which value was changed
     * \return value new value on pin: ARDUINO_HIGH or ARDUINO_LOW
     */
    void digitalValueChanged(unsigned char pin, Arduino::DigitalValue value);

    /*!
     * \brief Emits when a pin analog value changes.
     * \param pin the number of pin, on which value was changed
     * \return value new value on pin: 0 to 1023
     */
    void analogValueChanged(unsigned char pin, unsigned int value);

protected:
    QMap<unsigned char, Arduino::PinMode> pinModes; //!< modes of pins
    QMap<unsigned char, Arduino::DigitalValue> digitalValues; //!< digital values on pins
    QMap<unsigned char, int> analogValues; //!< analog values on pins
    SerialPort* inputPort; //!< input serial port
    SerialPort* outputPort; //!< output serial port
    QList<unsigned char> inputBuffer; //!< buffer for input bytes

protected slots:
    /*!
     * \brief Read a byte from the input port.
     * \return the readed byte
     */
    unsigned char serialReadByte();

    /*!
     * \brief Write a byte to the output port.
     * \param b the byte to write
     */
    void serialWriteByte(unsigned char b);

    /*!
     * \brief Read a word from the input port.
     * \return the readed word
     */
    unsigned short serialReadWord();

    /*!
     * \brief Write a word to the output port.
     * \param w the word to write
     */
    void serialWriteWord(unsigned short w);

    /*!
     * \brief Write an unsigned long to the output port.
     * \param ul the unsigned long to write
     */
    void serialWriteULong(unsigned long ul);

    /*!
     * \brief Read an available data to input buffer.
     */
    void inputBufferUpdate();

    /*!
     * \brief Read a data from the input port.
     */
    void serialRead();

    /*!
     * \brief Write a data to the output port.
     * \param data the array of bytes
     * \param length the number of bytes to write
     */
    void serialWrite(unsigned char* data, int length);
};

#endif // _Arduino_h_
