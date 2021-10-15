#include "speechtotext.h"
#include "aerstructures.h"

using google::cloud::speech::v1p1beta1::StreamingRecognitionConfig;
using google::cloud::speech::v1p1beta1::RecognitionConfig;
using google::cloud::speech::v1p1beta1::Speech;
using google::cloud::speech::v1p1beta1::StreamingRecognizeRequest;
using google::cloud::speech::v1p1beta1::StreamingRecognizeResponse;

SpeechToText::SpeechToText(QObject *parent)
    : QObject(parent),
      start_time(0),
      start_time_not_changing(0),
      m_talking(false),
      m_start_time_talking(0),
      m_time_left_before_talking(0),
      m_stop_listening(false),
      m_microphone_thread_running(false)
{
}

void SpeechToText::initialize()
{
    m_stop_listening = false;
    m_talking = false;

    QString path_to_google_credentials = qApp->applicationDirPath() + "/application_default_credentials.json";
    std::string path_to_gc = path_to_google_credentials.toStdString();
    char path_to_gc_array[path_to_gc.length()+1];
    strcpy(path_to_gc_array, path_to_gc.c_str());
    // TODO: Why do I need to do all of that?
    setenv("GOOGLE_APPLICATION_CREDENTIALS", path_to_gc_array, 1);

    QString path_to_roots_pem = qApp->applicationDirPath() + "/roots.pem";
    std::string path_to_rpem = path_to_roots_pem.toStdString();
    char path_to_rpem_array[path_to_rpem.length()+1];
    strcpy(path_to_rpem_array, path_to_rpem.c_str());
    setenv("GRPC_DEFAULT_SSL_ROOTS_FILE_PATH", path_to_rpem_array, 1);

    QString path_to_roots_pem_2 = qApp->applicationDirPath() + "/roots.pem";
    std::string path_to_rpem_2 = path_to_roots_pem_2.toStdString();
    char path_to_rpem_array_2[path_to_rpem_2.length()+1];
    strcpy(path_to_rpem_array_2, path_to_rpem_2.c_str());
    setenv("GRPC_DEFAULT_SSL_ROOTS_FILE_PATH_ENV_VAR", path_to_rpem_array_2, 1);

    emit finished_initializing();
}

void SpeechToText::MicrophoneThreadMain(grpc::ClientReaderWriterInterface<StreamingRecognizeRequest,
                                        StreamingRecognizeResponse> *streamer)
{
    m_microphone_thread_running = true;

    bool is_ended_because_of_google_limit = false;

    StreamingRecognizeRequest request;
    std::size_t size_read;
    while ((time(nullptr) - start_time <= TIME_RECOGNITION || m_talking) && !m_stop_listening)
    {
        //qDebug() << (time(nullptr) - start_time) << (time(nullptr) - start_time_not_changing);
        if(time(nullptr) - start_time_not_changing >= GOOGLE_RECOGNITION_TIME_LIMIT)
        {
            is_ended_because_of_google_limit = true;
            break;
        }

        if(!m_talking)
        {
            int chunk_size = 64 * 1024;
            if (audioDataBuffer.size() >= chunk_size)
            {
                QByteArray bytes_read = QByteArray(audioDataBuffer);
                size_read = std::size_t(bytes_read.size());

                // And write the chunk to the stream.
                request.set_audio_content(&bytes_read.data()[0], size_read);

                bool ok = streamer->Write(request);
                Q_UNUSED(ok)
                /*if (ok)
                {
                    std::cout << "Sending " << size_read / 1024 << "k bytes." << std::endl;
                }*/

                audioDataBuffer.clear();
                audioDataBuffer.resize(0);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        QCoreApplication::processEvents();
    }

    QDateTime dateTime = QDateTime::currentDateTime();
    QString end_date = dateTime.toString(Qt::ISODate);
    qDebug() << "Out of speech recognition: " << end_date;

    streamer->WritesDone();

    m_microphone_thread_running = false;

    if(is_ended_because_of_google_limit)
    {
        start_speech_to_text();
    }
    else
    {
        emit out_of_speech_to_text();
    }
}

void SpeechToText::StreamerThread(grpc::ClientReaderWriterInterface<StreamingRecognizeRequest,
                                      StreamingRecognizeResponse> *streamer)
{
    int count_of_intermediate_results = 0;
    // Read responses.
    StreamingRecognizeResponse response;

    m_stop_listening = false;

    while ((time(nullptr) - start_time <= TIME_RECOGNITION || m_talking) && !m_stop_listening)
    {
        if(time(nullptr) - start_time_not_changing >= GOOGLE_RECOGNITION_TIME_LIMIT)
        {
            break;
        }

        if(streamer->Read(&response) && !m_talking) // Returns false when no more to read.
        {
            // Dump the transcript of all the results.
            if (response.results_size() > 0)
            {
                auto result = response.results(0);
                if (result.alternatives_size() > 0)
                {
                    auto alternative = result.alternatives(0);
                    auto transcript = QString::fromStdString(alternative.transcript());
                    if (result.is_final())
                    {
                        qDebug() << "Speech recognition: " << transcript;

                        if(transcript[0] == ' ')
                        {
                            transcript.remove(0, 1); // remove first element if blank space
                        }

                        emit finished_speech_to_text(transcript);
                    }
                    else
                    {
                        emit intermediate_speech_to_text(transcript);

                        count_of_intermediate_results += 1;
                        if(count_of_intermediate_results >= 8)
                        {
                            qDebug() << "Adding 6 seconds to start_time. Remaining: "
                                     << QString::number(TIME_RECOGNITION - (time(nullptr) - start_time))
                                     << "sec...";
                            start_time += 7;
                            count_of_intermediate_results = 0;
                        }
                    }
                }
            }
        }
    }
}

void SpeechToText::listen(const QByteArray &audioData)
{
    if (!m_microphone_thread_running)
        return;

    if (!m_talking)
    {
        audioDataBuffer.append(audioData);
    }
}

void SpeechToText::start_speech_to_text()
{
    qDebug() << Q_FUNC_INFO << "Size of audioDataBuffer is" << audioDataBuffer.size();
    audioDataBuffer.clear();
    qDebug() << Q_FUNC_INFO << "Cleared audioDataBuffer.";
    audioDataBuffer.resize(0);
    qDebug() << Q_FUNC_INFO << "Resized audioDataBuffer to 0.";

    QDateTime dateTime = QDateTime::currentDateTime();
    QString start_date = dateTime.toString(Qt::ISODate);
    start_time = time(nullptr);
    start_time_not_changing = start_time;

    qDebug() << "in start_speech_to_text: " << start_date;
    emit in_speech_to_text();

    m_stop_listening = false;

    StreamingRecognizeRequest request;
    auto *streaming_config = request.mutable_streaming_config();
    RecognitionConfig *recognition_config = new RecognitionConfig();
    recognition_config->set_language_code("en-US");
    recognition_config->set_sample_rate_hertz(16000);
    recognition_config->set_encoding(RecognitionConfig::LINEAR16);
    recognition_config->enable_automatic_punctuation();
    streaming_config->set_allocated_config(recognition_config);

    // Create a Speech Stub connected to the speech service.
    auto creds = grpc::GoogleDefaultCredentials();
    auto channel = grpc::CreateChannel("speech.googleapis.com", creds);
    std::unique_ptr<Speech::Stub> speech(Speech::NewStub(channel));

    // Begin a stream.
    grpc::ClientContext context;
    auto streamer = speech->StreamingRecognize(&context);
    // Write the first request, containing the config only.
    streaming_config->set_interim_results(true);
    streamer->Write(request);

    // The microphone thread writes the audio content.
    std::thread microphone_thread(&SpeechToText::MicrophoneThreadMain, this, streamer.get());
    std::thread streamer_thread(&SpeechToText::StreamerThread, this, streamer.get());

    microphone_thread.join();
    streamer_thread.join();
}

void SpeechToText::set_talking(bool is_talking)
{
    // Add the seconds it takes for Jarvis to talk.
    // While preserving the timer since Jarvis started to talk
    if(is_talking)
    {
        m_start_time_talking = time(nullptr);
        m_time_left_before_talking = TIME_RECOGNITION -  (time(nullptr) - start_time);
    }
    else
    {
        time_t time_took_bot_to_talk = time(nullptr) - m_start_time_talking;
        time_t time_to_add_from_what_left_and_bot_talking = TIME_RECOGNITION - (time_took_bot_to_talk + m_time_left_before_talking);
        start_time = time(nullptr) - time_to_add_from_what_left_and_bot_talking;
    }

    m_talking = is_talking;
}

void SpeechToText::set_stop_listening()
{
    m_stop_listening = true;

    /*if(m_stop_listening)
    {
        m_stop_listening = false;
    }
    else
    {
        m_stop_listening = true;
    }*/
}
