#-------------------------------------------------
#
# Project created by QtCreator 2018-05-14T16:20:50
#
#-------------------------------------------------

QT       += core gui network multimedia concurrent qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Deus
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

macx {
    QMAKE_CXXFLAGS += -std=c++11

    # Boost
    BOOST_PATH = /usr/local/Cellar/boost/1.67.0_1
    INCLUDEPATH += "$${BOOST_PATH}/include/"
    LIBS += -L$${BOOST_PATH}/lib
    LIBS += -lboost_chrono-mt -lboost_system

    # Apple's
    LIBS += -framework ApplicationServices
    LIBS += -framework CoreFoundation
    LIBS += -framework Carbon

    #Procupine
    INCLUDEPATH += /Users/ruby/MEGA/Projects/Deus/Packaging/Porcupine/include
    LIBS += -L/Users/ruby/MEGA/Projects/Deus/Packaging/Porcupine/lib/mac/x86_64
    LIBS += -lpv_porcupine

    CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig

    # grpc and Google's Speech API
    INCLUDEPATH += /usr/local/include
    INCLUDEPATH += /usr/local/include/grpc++
    INCLUDEPATH += /usr/local/include/grpcpp
    INCLUDEPATH += /usr/local/include/grpc
    LIBS += -L/usr/local/lib
    LIBS += -lgrpc++ -lgrpc -lprotobuf -lpthread -ldl -lgrpc++_reflection -Wl
    INCLUDEPATH += /Users/ruby/gitrepos/googleapis/gens
}

ICON = $$PWD/images\deus_icon.icns

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        speechtotext.cpp \
    actionandentitiesrecognizer.cpp \
    skills.cpp \
    wakeupword.cpp \
    /Users/ruby/gitrepos/googleapis/gens/google/cloud/speech/v1p1beta1/cloud_speech.grpc.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/cloud/speech/v1p1beta1/cloud_speech.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/longrunning/operations.grpc.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/longrunning/operations.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/rpc/code.grpc.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/rpc/code.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/rpc/error_details.grpc.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/rpc/error_details.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/rpc/status.grpc.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/rpc/status.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/api/annotations.grpc.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/api/annotations.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/api/http.grpc.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/api/http.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/api/httpbody.grpc.pb.cc \
    /Users/ruby/gitrepos/googleapis/gens/google/api/httpbody.pb.cc \
    microphonerecorder.cpp \
    texttospeech.cpp

HEADERS += \
        mainwindow.h \
        speechtotext.h \
    actionandentitiesrecognizer.h \
    aerstructures.h \
    skills.h \
    wakeupword.h \
    microphonerecorder.h \
    texttospeech.h

FORMS += \
      mainwindow.ui

RESOURCES += \
          images.qrc \
          fonts.qrc \
    commands_database.qrc \
    resources.qrc \
    knowledge_files.qrc

OTHER_FILES += \
    tipoftheday.qml

DISTFILES += \
    tipoftheday.qml
