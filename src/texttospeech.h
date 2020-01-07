#ifndef TEXTTOSPEECH_H
#define TEXTTOSPEECH_H

#include <QObject>

#include <QObject>
#include <QDebug>
#include <QtCore/QVariantList>
#include <QtCore/QBuffer>
#include <QtCore/QJsonDocument>
#include <QtCore/QUrlQuery>
#include <QtMultimedia/QAudioOutput>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QCoreApplication>

QT_BEGIN_NAMESPACE
class QAudioOutput;
class QBuffer;
class QNetworkAccessManager;
QT_END_NAMESPACE

#include <string>

class TextToSpeech : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QVariantList voices READ voices NOTIFY voices_changed)

public:
    explicit TextToSpeech(QObject *parent = nullptr);
    ~TextToSpeech();

    Q_INVOKABLE void synthesize_helper(const QString &inputText,
                                const QString &voiceLanguageCode,
                                const QString &voiceName = QString(),
                                const QVariantMap &audioConfig = QVariantMap());

    Q_INVOKABLE void update_voices(const QString &languageCode = QString());

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void interrupt();

    QVariantList voices() const;

signals:
    void finished_initializing();
    void voices_changed();
    void speech_started();
    void speech_completed();
    void is_talking(bool);

public slots:
    void initialize();
    void synthesize(QString);

private:
    const QString m_key;
    QVariantList m_voices;
    QNetworkAccessManager *m_network_access_manager;
    QBuffer *m_audioBuffer;
    QAudioOutput *m_audioOutput;
    bool m_paused;
};

#endif // TEXTTOSPEECH_H
