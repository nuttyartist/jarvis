#include "texttospeech.h"

TextToSpeech::TextToSpeech(QObject *parent)
    : QObject(parent),
      m_key(QStringLiteral("AIzaSyArsmLpikL3jCdneTF4fs5AnQI-yoh9El8")),
      m_network_access_manager(new QNetworkAccessManager(this)),
      m_audioBuffer(new QBuffer(this)),
      m_paused(false)
{
    m_audioOutput = new QAudioOutput(QAudioFormat(), this);
}

TextToSpeech::~TextToSpeech()
{
}

void TextToSpeech::initialize()
{
    //m_key = QStringLiteral("AIzaSyArsmLpikL3jCdneTF4fs5AnQI-yoh9El8");
    //m_network_access_manager = new QNetworkAccessManager(this);
    //m_audioBuffer = new QBuffer(this);
    //m_paused = false;

    m_audioBuffer->open(QBuffer::ReadWrite);

    QAudioFormat audioFormat;
    audioFormat.setSampleRate(24000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(16);
    audioFormat.setCodec(QStringLiteral("audio/pcm"));
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo audioDeviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    if (!audioDeviceInfo.isFormatSupported(audioFormat)) {
        qDebug() << "Audio format not supported by backend, cannot play audio.";
        audioFormat = audioDeviceInfo.nearestFormat(audioFormat);
    }

    m_audioOutput = new QAudioOutput(audioFormat, this);
    connect(m_audioOutput, &QAudioOutput::stateChanged, this, [this] {
        switch (this->m_audioOutput->state()) {
        case QAudio::ActiveState:
            qDebug() << "speech_started";
            emit this->is_talking(true);
            emit this->speech_started();
            break;
        default:
            // QAudio::SuspendedState
            // QAudio::StoppedState
            // QAudio::IdleState
            // QAudio::InterruptedState
            emit this->is_talking(false);
            emit this->speech_completed();
            break;
        }
    });

    emit finished_initializing();
}

void TextToSpeech::synthesize_helper(const QString &inputText,
                                   const QString &voiceLanguageCode,
                                   const QString &voiceName,
                                   const QVariantMap &audioConfig)
{
    QVariantMap input;
    if (!inputText.isEmpty())
        input.insert(QStringLiteral("text"), inputText);

    QVariantMap voice;
    voice.insert(QStringLiteral("languageCode"), voiceLanguageCode);
    if (!voiceName.isEmpty())
        voice.insert(QStringLiteral("name"), voiceName);

    const QString audioEncoding = audioConfig.value(QStringLiteral("audioEncoding"),
                                                    QStringLiteral("LINEAR16")).toString();
    const QString speakingRate = audioConfig.value(QStringLiteral("speakingRate"),
                                                   QStringLiteral("1.0")).toString();
    const QString pitch = audioConfig.value(QStringLiteral("pitch"),
                                            QStringLiteral("0.0")).toString();
    const QString volumeGainDb = audioConfig.value(QStringLiteral("volumeGainDb"),
                                                   QStringLiteral("0.0")).toString();;
    const QString sampleRateHertz = audioConfig.value(QStringLiteral("sampleRateHertz"),
                                                      QStringLiteral("24000")).toString();;

    QVariantMap audioConfigChecked;
    audioConfigChecked.insert(QStringLiteral("audioEncoding"), audioEncoding);
    audioConfigChecked.insert(QStringLiteral("speakingRate"), speakingRate);
    audioConfigChecked.insert(QStringLiteral("pitch"), pitch);
    audioConfigChecked.insert(QStringLiteral("volumeGainDb"), volumeGainDb);
    audioConfigChecked.insert(QStringLiteral("sampleRateHertz"), sampleRateHertz);

    QVariantMap json;
    json.insert(QStringLiteral("input"), input);
    json.insert(QStringLiteral("voice"), voice);
    json.insert(QStringLiteral("audioConfig"), audioConfigChecked);

    QByteArray data = QJsonDocument::fromVariant(json).toJson(QJsonDocument::Compact);

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("key"), m_key);

    QUrl url = QUrl(QStringLiteral("https://texttospeech.googleapis.com/v1beta1/text:synthesize"));
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    QNetworkReply *reply = m_network_access_manager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [reply, sampleRateHertz, this] () {
        auto data = QJsonDocument::fromJson(reply->readAll()).toVariant().toMap();
        auto base64 = data.value(QStringLiteral("audioContent"), QByteArray()).toByteArray();
        auto audioContent = QByteArray::fromBase64(base64);
        if (!audioContent.isEmpty()) {
            // NOTE: Remove header info from playback.
            audioContent.remove(0, audioContent.indexOf("data"));
            audioContent.remove(0, 4); // data
            audioContent.remove(0, 4); // \x8a

            m_audioBuffer->buffer().append(audioContent);
        }

        if (m_audioOutput->state() == QAudio::StoppedState)
            m_audioOutput->start(m_audioBuffer);

        reply->close();
        reply->deleteLater();
    });
}

void TextToSpeech::synthesize(QString string_to_say)
{
    QVariantMap audioConfig;
    synthesize_helper(string_to_say, "en-US", "en-US-Wavenet-D", audioConfig);
}

void TextToSpeech::start()
{
    m_audioOutput->start(m_audioBuffer);
    m_paused = false;
}

void TextToSpeech::stop()
{
    m_audioOutput->stop();
    m_audioBuffer->readAll(); // NOTE: Safely clear playback buffer.
    m_paused = true;
}

void TextToSpeech::interrupt()
{
    m_audioBuffer->readAll(); // NOTE: Safely clear playback buffer.
}

void TextToSpeech::update_voices(const QString &languageCode)
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("key"), m_key);
    if (!languageCode.isEmpty())
        query.addQueryItem(QStringLiteral("languageCode"), languageCode);

    QUrl url = QUrl(QStringLiteral("https://texttospeech.googleapis.com/v1beta1/voices"));
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    QNetworkReply *reply = m_network_access_manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [reply, this] () {
        QVariantMap data = QJsonDocument::fromJson(reply->readAll()).toVariant().toMap();
        QVariantList voices = data.value(QStringLiteral("voices")).toList();

        if (this->m_voices != voices) {
            this->m_voices = voices;
            emit this->voices_changed();
        }

        reply->close();
        reply->deleteLater();
    });
}

QVariantList TextToSpeech::voices() const
{
    return m_voices;
}
