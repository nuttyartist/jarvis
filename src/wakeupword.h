#ifndef WAKEUPWORD_H
#define WAKEUPWORD_H

#include <QByteArray>
#include <QDebug>
#include <QtMultimedia>
#include <QtMultimedia/QAudioInput>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QIODevice>

#include <pv_porcupine.h>
#include <picovoice.h>

class WakeUpWord : public QObject
{
    Q_OBJECT
public:
    explicit WakeUpWord(QObject *parent = nullptr);

signals:
    void finished_initializing();
    void wake_up_word_detected();

public slots:
    void initialize();
    void listen(const QByteArray &);
    void set_stop_listening_for_wake_up_word(bool);

private:
    bool hasEnoughSamples(int);
    void processSamples(int);

    pv_porcupine_object_t *porcupineObject;
    QByteArray audioDataBuffer;

    bool m_stop_listening_for_wake_up_word;
};

#endif // WAKEUPWORD_H
