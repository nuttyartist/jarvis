#ifndef MICROPHONERECORDER_H
#define MICROPHONERECORDER_H

#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QtMultimedia>
#include <QtMultimedia/QAudioInput>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QIODevice>

class MicrophoneRecorder : public QObject
{
    Q_OBJECT
public:
    explicit MicrophoneRecorder(QObject *parent = nullptr);

signals:
    void microphone_data_raw(const QByteArray &);

public slots:
    void start_listen();

private slots:
    void listen(const QByteArray &);

private:
    QAudioInput *audioInput;
    QIODevice *ioDevice;
    QByteArray audioDataBuffer;
};

#endif // MICROPHONERECORDER_H
