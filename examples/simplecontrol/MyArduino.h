/*******************************************************************************
*                                                                              *
*  Sample special arduino device interface imlementation.                      *
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


#ifndef _MyArduino_h_
#define _MyArduino_h_

#include "Arduino.h"
#include <QColor>

#define PIN_REDDIOD     9  // the number of red LED pin
#define PIN_GREENDIOD   6  // the number of green LED pin
#define PIN_BLUEDIOD    5  // the number of blue LED pin
#define PIN_WHITEDIOD   3  // the number of white LED pin
#define PIN_LIGHTSENSOR 14 // the number of light sensor's pin
#define PIN_SERVO       11 // the number of servos's pin

/*!
 * \brief Sample special arduino device interface class.
 */
class MyArduino : public Arduino {
    Q_OBJECT

    Q_PROPERTY(float redDiodValue READ getRedDiodValue WRITE setRedDiodValue NOTIFY redDiodValueChanged) // brightness of the red LED
    Q_PROPERTY(float greenDiodValue READ getGreenDiodValue WRITE setGreenDiodValue NOTIFY greenDiodValueChanged) // brightness of the green LED
    Q_PROPERTY(float blueDiodValue READ getBlueDiodValue WRITE setBlueDiodValue NOTIFY blueDiodValueChanged) // brightness of the blue LED
    Q_PROPERTY(float whiteDiodValue READ getWhiteDiodValue WRITE setWhiteDiodValue NOTIFY whiteDiodValueChanged) // brightness of the while LED
    Q_PROPERTY(float lightSensorValue READ getLightSensorValue NOTIFY lightSensorValueChanged) // light sensor's state
    Q_PROPERTY(float servoValue READ getServoValue WRITE setServoValue NOTIFY servoValueChanged) // servo's state

public:
    /*!
     * \brief Constructor.
     */
    MyArduino();

public slots:
    /*!
     * \brief Get brightness of the red LED.
     * \return the brightness value from 0 to 1.0
     */
    float getRedDiodValue() {return this->analogRead(PIN_REDDIOD) * 1.0 / 255;}

    /*!
     * \brief Set brightness of the red LED.
     * \param value the brightness value from 0 to 1.0
     */
    void setRedDiodValue(float value) {this->analogWrite(PIN_REDDIOD, value * 255);}

    /*!
     * \brief Get brightness of the green LED.
     * \return the brightness value from 0 to 1.0
     */
    float getGreenDiodValue() {return this->analogRead(PIN_GREENDIOD) * 1.0 / 255;}

    /*!
     * \brief Set brightness of the green LED.
     * \param value the brightness value from 0 to 1.0
     */
    void setGreenDiodValue(float value) {this->analogWrite(PIN_GREENDIOD, value * 255);}

    /*!
     * \brief Get brightness of the blue LED.
     * \return the brightness value from 0 to 1.0
     */
    float getBlueDiodValue() {return this->analogRead(PIN_BLUEDIOD) * 1.0 / 255;}

    /*!
     * \brief Set brightness of the blue LED.
     * \param value the brightness value from 0 to 1.0
     */
    void setBlueDiodValue(float value) {this->analogWrite(PIN_BLUEDIOD, value * 255);}

    /*!
     * \brief Get brightness of the white LED.
     * \return the brightness value from 0 to 1.0
     */
    float getWhiteDiodValue() {return this->analogRead(PIN_WHITEDIOD) * 1.0 / 255;}

    /*!
     * \brief Set brightness of the white LED.
     * \param value the brightness value from 0 to 1.0
     */
    void setWhiteDiodValue(float value) {this->analogWrite(PIN_WHITEDIOD, value * 255);}

    /*!
     * \brief Start scanning light sensor pin.
     */
    void startLightSensorInput();

    /*!
     * \brief Get brightness from light sensor.
     * \return the brightness value from 0 to 1.0
     */
    float getLightSensorValue() {return this->analogRead(PIN_LIGHTSENSOR) * 1.0 / 1023;}

    /*!
     * \brief Get controlling voltage value of the servomotor.
     * \return the controlling voltage factor from 0 to 1.0
     */
    float getServoValue() {return this->analogRead(PIN_SERVO) * 1.0 / 1023;}

    /*!
     * \brief Set controlling voltage value of the servomotor.
     * \param value the controlling voltage factor from 0 to 1.0
     */
    void setServoValue(float value) {this->analogWrite(PIN_SERVO, value * 1023);}

    /*!
     * \brief Converts a number to the color of corresponding brightness.
     * \param value the value from 0 to 1.0
     */
    QColor floatToColor(float value);

signals:
    /*!
     * \brief Emits when the redDiodValue property changes.
     */
    void redDiodValueChanged();

    /*!
     * \brief Emits when the greenDiodValue property changes.
     */
    void greenDiodValueChanged();

    /*!
     * \brief Emits when the blueDiodValue property changes.
     */
    void blueDiodValueChanged();

    /*!
     * \brief Emits when the whiteDiodValue property changes.
     */
    void whiteDiodValueChanged();

    /*!
     * \brief Emits when the lightSensorValue property changes.
     */
    void lightSensorValueChanged();

    /*!
     * \brief Emits when the servoValue property changes.
     */
    void servoValueChanged();

private slots:
    void digitalValueChangedHandler(unsigned char pin, Arduino::DigitalValue value);
    void analogValueChangedHandler(unsigned char pin, unsigned int value);
};

#endif // _MyArduino_h_
