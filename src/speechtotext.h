#ifndef SPEECHTOTEXT_H
#define SPEECHTOTEXT_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QDateTime>

#include <stdlib.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <functional>

#include <QtMultimedia>
#include <QtMultimedia/QAudioInput>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QIODevice>
#include <QtConcurrent>
#include <QMutex>

#include <grpc++/grpc++.h>
#include "google/cloud/speech/v1p1beta1/cloud_speech.grpc.pb.h"

using google::cloud::speech::v1p1beta1::StreamingRecognitionConfig;
using google::cloud::speech::v1p1beta1::RecognitionConfig;
using google::cloud::speech::v1p1beta1::Speech;
using google::cloud::speech::v1p1beta1::StreamingRecognizeRequest;
using google::cloud::speech::v1p1beta1::StreamingRecognizeResponse;

class SpeechToText : public QObject
{
    Q_OBJECT
public:
    explicit SpeechToText(QObject *parent = nullptr);

signals:
    void finished_initializing();
    void in_speech_to_text();
    void out_of_speech_to_text();
    void finished_speech_to_text(QString);
    void intermediate_speech_to_text(const QString &);

public slots:
    void initialize();
    void listen(const QByteArray &);
    void start_speech_to_text();
    void set_talking(bool);
    void set_stop_listening();

private:
    void MicrophoneThreadMain(grpc::ClientReaderWriterInterface<StreamingRecognizeRequest,
                                          StreamingRecognizeResponse> *);
    void StreamerThread(grpc::ClientReaderWriterInterface<StreamingRecognizeRequest,
                                          StreamingRecognizeResponse> *);


    QByteArray audioDataBuffer;
    time_t start_time;
    time_t start_time_not_changing;
    bool m_talking;
    time_t m_start_time_talking;
    time_t m_time_left_before_talking;
    bool m_stop_listening;
    bool m_microphone_thread_running;
};

#endif // SPEECHTOTEXT_H
