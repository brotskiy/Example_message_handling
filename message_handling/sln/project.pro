QT += core gui concurrent network positioning

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += IS_PROJECT_1

PROJECT_NAME = PROJECT_2
contains(DEFINES, IS_PROJECT_1) {
    PROJECT_NAME = PROJECT_1
}

TARGET = $${PROJECT_NAME}
TEMPLATE = app

CONFIG += c++17

include($$PWD/openmp.pri)
include($$PWD/glog.pri)
include($$PWD/applock.pri)

OPENCV_LIBS = core imgproc
OPENCV_VER = undefined
include($$PWD/opencv.pri)

PROJECT_ROOT_PATH = $$PWD/..

APPLOCK_PATH = $${PROJECT_ROOT_PATH}/external/applock
makeApplock($${APPLOCK_PATH})

INCLUDEPATH += $${PROJECT_ROOT_PATH}/external                             \
               $${PROJECT_ROOT_PATH}/src                                  \
               $${PROJECT_ROOT_PATH}/src/base                             \
               $${PROJECT_ROOT_PATH}/src/configer                         \
               $${PROJECT_ROOT_PATH}/src/logging                          \
               $${PROJECT_ROOT_PATH}/src/net                              \
               $${PROJECT_ROOT_PATH}/src/net/ProtocolSupport/Abstract     \
               $${PROJECT_ROOT_PATH}/src/net/ProtocolSupport/Common       

HEADERS += $$files($${PROJECT_ROOT_PATH}/src/base/*.h)                              \
           $$files($${PROJECT_ROOT_PATH}/src/configer/*.h)                          \
           $$files($${PROJECT_ROOT_PATH}/src/helper/*.h)                            \
           $$files($${PROJECT_ROOT_PATH}/src/utils/*.h)                             \
           $$files($${PROJECT_ROOT_PATH}/src/net/*.h)                               \
           $$files($${PROJECT_ROOT_PATH}/src/subsystem/*.h)                         \
           $$files($${PROJECT_ROOT_PATH}/src/net/ProtocolSupport/Abstract/*.h)      \
           $$files($${PROJECT_ROOT_PATH}/src/net/ProtocolSupport/Common/*.h)        \
           $$files($${PROJECT_ROOT_PATH}/src/general/*.h)                           
 

SOURCES += $$files($${PROJECT_ROOT_PATH}/src/base/*.cpp)                            \
           $$files($${PROJECT_ROOT_PATH}/src/configer/*.cpp)                        \
           $$files($${PROJECT_ROOT_PATH}/src/helper/*.cpp)                          \
           $$files($${PROJECT_ROOT_PATH}/src/net/*.cpp)                             \
           $$files($${PROJECT_ROOT_PATH}/src/utils/*.cpp)                           \
           $$files($${PROJECT_ROOT_PATH}/src/subsystem/*.cpp)                       \
           $$files($${PROJECT_ROOT_PATH}/src/net/ProtocolSupport/Abstract/*.cpp)    \
           $$files($${PROJECT_ROOT_PATH}/src/net/ProtocolSupport/Common/*.cpp)      

FORMS   += $$files($${PROJECT_ROOT_PATH}/src/general/*.ui)

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resources.qrc

message($${TARGET})
