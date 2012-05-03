#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include <QtDeclarative>

#include "MyArduino.h"

//#include <QList>
//#include "serialportinfo.h"
//#include <QDebug>

Q_DECL_EXPORT int main(int argc, char *argv[]) {

//    QList<SerialPortInfo> list = SerialPortInfo::availablePorts();
//    int item = 0;
//    foreach(SerialPortInfo info, list) {
//        qDebug() << "--> Item: " << item++;
//        qDebug() << "Port name        : " << info.portName();
//        qDebug() << "Port location    : " << info.systemLocation();
//        qDebug() << "Port description : " << info.description();
//        qDebug() << "Port manufacturer: " << info.manufacturer();
//    }

//    SerialPort* serialPort = new SerialPort("COM10");
//    if (serialPort->open(QIODevice::ReadWrite | QIODevice::Unbuffered))
//        return 0;
//    else
//        return 1;


    qmlRegisterType<MyArduino>("Arduino", 1, 0, "Arduino");
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
#if defined(Q_WS_MAEMO_5)
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/Main_fremantle.qml"));
    viewer.showFullScreen();
#elif defined(Q_WS_HARMATTAN)
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/Main_harmattan.qml"));
    viewer.showFullScreen();
#else
    viewer.setMainQmlFile(QLatin1String("qml/Main_desktop.qml"));
    viewer.show();
#endif
    return app->exec();
}
