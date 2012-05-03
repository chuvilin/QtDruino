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


#include "MyArduino.h"

MyArduino::MyArduino():
#if defined(Q_WS_MAEMO_5)
    Arduino("ttyACM0") { // arduino's port name on my Nokia N900
#elif defined (Q_WS_WIN)
    Arduino("COM10") { // arduino's port name on my windows
#else
    Arduino("ttyACM0") { // arduino's port name on my linux
#endif
    this->open();
    QObject::connect(this, SIGNAL(digitalValueChanged(unsigned char, Arduino::DigitalValue)), this, SLOT(digitalValueChangedHandler(unsigned char, Arduino::DigitalValue)));
    QObject::connect(this, SIGNAL(analogValueChanged(unsigned char,uint)), this, SLOT(analogValueChangedHandler(unsigned char,uint)));
}


void MyArduino::startLightSensorInput() {
    this->pinAnalogSensetivity(PIN_LIGHTSENSOR, 10);
    this->pinAnalogSmoothFactor(PIN_LIGHTSENSOR, 0.9);
    this->pinMode(PIN_LIGHTSENSOR, Arduino::ANALOG_INPUT);
}


QColor MyArduino::floatToColor(float value) {
    if (value > 1) value = 1;
    if (value < 0) value = 0;
    return QColor(255 * value, 255 * value, 255 * value);
}


void MyArduino::digitalValueChangedHandler(unsigned char pin, Arduino::DigitalValue value) {
    // fill to keep track of digital pins
}


void MyArduino::analogValueChangedHandler(unsigned char pin, unsigned int value) {
    switch (pin) {
    case PIN_REDDIOD:
        emit this->redDiodValueChanged();
        break;
    case PIN_GREENDIOD:
        emit this->greenDiodValueChanged();
        break;
    case PIN_BLUEDIOD:
        emit this->blueDiodValueChanged();
        break;
    case PIN_WHITEDIOD:
        emit this->whiteDiodValueChanged();
        break;
    case PIN_LIGHTSENSOR:
        emit this->lightSensorValueChanged();
        break;
    case PIN_SERVO:
        emit this->servoValueChanged();
        break;
    }
}
