#include "wakeupword.h"

WakeUpWord::WakeUpWord(QObject *parent) : QObject(parent),
    m_stop_listening_for_wake_up_word(false)
{

}

void WakeUpWord::initialize()
{
    //this->m_stop_listening_for_wake_up_word = false;

    //std::string str = qApp->applicationDirPath().toStdString();
    //const char* p = str.c_str();

    QString path_1 = qApp->applicationDirPath() + "/Porcupine/lib/common/porcupine_params.pv";
    QString path_2 = qApp->applicationDirPath() + "/Porcupine/hey_deus_mac_2020-01-26.ppn";
    std::string p1 = path_1.toStdString();
    std::string p2 = path_2.toStdString();
    char p1_array[p1.length()+1];
    char p2_array[p2.length()+1];
    strcpy(p1_array, p1.c_str());
    strcpy(p2_array, p2.c_str());
    // TODO: Why do I need to do all of that?
    const char *model_file_path = p1_array;
    const char *keyword_file_path = p2_array;
    const float sensitivity = 0.5;

    const pv_status_t status = pv_porcupine_init(model_file_path, keyword_file_path, sensitivity, &porcupineObject);

    if (status != PV_STATUS_SUCCESS) {
        // error handling logic
        qDebug() << "Error loading Porcupine!";
    }

    emit finished_initializing();
}

/**
* @brief
* Check if we have enough bytes of audio data for
* Porcupine to work with.
*/
bool WakeUpWord::hasEnoughSamples(int porcupineFrameLength)
{
    // We use porcupineFrameLength * 2 because frame length type is int16_t while audioDataBuffer's type is byte
    return audioDataBuffer.size() >= porcupineFrameLength * 2;
}

/**
* @brief
* Go through the audio data and detect the wake word if there's any
*/
void WakeUpWord::processSamples(int porcupineFrameLength)
{
    while (hasEnoughSamples(porcupineFrameLength)) {
        const int16_t *audioData = reinterpret_cast<int16_t*>(audioDataBuffer.data());
        bool detected;

        pv_porcupine_process(porcupineObject, &audioData[0], &detected);
        if (detected) {
            qDebug() << "Wake up word detected!";
            emit wake_up_word_detected();
        }

        // Remove the audio data that we have processed
        audioDataBuffer.remove(0, porcupineFrameLength * 2);
    }
}

/**
* @brief
* Listen to the microphone's audio data and look for the keyword
*/
void WakeUpWord::listen(const QByteArray &audioData)
{
    //TODO: Why cann't i do soemthing like if(!this->parent()->wake_up_word_engine->is_talking()) ?????
    if(!m_stop_listening_for_wake_up_word)
    {
        const int porcupineFrameLength = pv_porcupine_frame_length();
        audioDataBuffer.append(audioData);
        processSamples(porcupineFrameLength);
    }
}

void WakeUpWord::set_stop_listening_for_wake_up_word(bool stop_listening)
{
    m_stop_listening_for_wake_up_word = stop_listening;
}
