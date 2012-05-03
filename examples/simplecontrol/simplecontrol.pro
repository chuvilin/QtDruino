# QKit
folder_qkit_qml.source = ../../include/QKit/qml
folder_qkit_qml.target = .
DEPLOYMENTFOLDERS += folder_qkit_qml

# Add more folders to ship with the application, here
folder_qml.source = qml
folder_qml.target = .
DEPLOYMENTFOLDERS += folder_qml

# Additional import path used to resolve QML modules in Creator's code model
#QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE7921105

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian {
    TARGET.CAPABILITY += NetworkServices

    my_deployment.pkg_prerules += vendorinfo

    DEPLOYMENT += my_deployment

    vendorinfo += "%{\"\"}" ":\"\""
}

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

contains(MEEGO_EDITION,harmattan) {
    DEFINES += Q_WS_HARMATTAN # definition for harmattan
    target.path = /opt/foldergallery/bin
    INSTALLS += target
    # enable booster
    QMAKE_CXXFLAGS += -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
    QMAKE_LFLAGS += -pie -rdynamic
}

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable

# Add dependency to Symbian components
# CONFIG += qt-components

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

# QtDruino
include(../../source/QtDruino.pri)

HEADERS += \
    MyArduino.h

SOURCES += \
    main.cpp \
    MyArduino.cpp

OTHER_FILES += \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog
