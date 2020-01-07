#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "aerstructures.h"
#include "actionandentitiesrecognizer.h"
#include "skills.h"
#include "microphonerecorder.h"
#include "speechtotext.h"
#include "texttospeech.h"
#include "wakeupword.h"

#include <QMainWindow>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QDateTime>
#include <QNetworkReply>
#include <QThread>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QLocale>
#include <QQuickView>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/qqml.h>
#include <QtQml/QQmlContext>

#include <queue>

using google::cloud::speech::v1p1beta1::StreamingRecognitionConfig;
using google::cloud::speech::v1p1beta1::RecognitionConfig;
using google::cloud::speech::v1p1beta1::Speech;
using google::cloud::speech::v1p1beta1::StreamingRecognizeRequest;
using google::cloud::speech::v1p1beta1::StreamingRecognizeResponse;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    Q_INVOKABLE void start_speech_to_text_once();
    Q_INVOKABLE void stop_speech_to_text();
    Q_INVOKABLE void opening();

    //typedef void (Skills::*SkillsFunc)(Command*);

signals:
    void send_to_text_to_speech(QString);
    void finished_speech_to_text(QString);

public slots:
    //void set_follow_up_command(SkillsFunc);

private slots:
    // Setup settings, widgets
    void set_up_settings();
    void setUpWindow();
    void setUpTrafficLights();
    void setUpFrames();
    void setUpTextEdit();
    void setUpLineEdit();
    void setUpLabel();
    void setupTrayIcon();
    void setupTipOfTheDay();

    // Setup variables and engines
    void setUpVariables();
    void setupSignalsSlots();
    void setUpAerEngine();
    void setUpSkillsEngine();
    void setUpMicrophoneRecorder();
    void setUpSpeechToTextEngine();
    void setUpTextToSpeechEngine();
    void setUpWakeUpWordEngine();

    // UI interaction
    void show_text_window();
    void hide_text_window();
    void send_message_to_textedit(QString, QString, QColor);
    void say_to_user(QString);
    void process_user_input(QString);
    void on_lineEdit_returnPressed();

    // Chatbot
    void do_chatbot_response(QString);

    // Traffic light buttons handling
    void on_redCloseButton_pressed();
    void on_yellowMinimizeButton_pressed();
    void on_greenMaximizeButton_pressed();
    void on_redCloseButton_clicked();
    void on_yellowMinimizeButton_clicked();
    void on_greenMaximizeButton_clicked();

    // Window dragging code
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

protected:
    bool eventFilter(QObject* object, QEvent* event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;

private:
    Ui::MainWindow *ui;

    QString m_assistant_name;
    bool m_is_already_done_opening;
    QString m_user_id;

    // Engines variables
    ActionAndEntitiesRecognizer aer_engine;
    Skills skills_engine;
    SpeechToText speech_to_text_engine;
    TextToSpeech text_to_speech_engine;
    WakeUpWord wake_up_word_engine;
    MicrophoneRecorder microphone_recorder_engine;
    //SkillsFunc m_follow_up_command;
    //bool m_is_follow_up_command;

    // Thread varables
    QThread *microphone_thread;
    QThread *wake_up_word_thread;
    QThread *speech_to_text_thread;
    QThread *text_to_speech_thread;

    // Chatbot variables
    bool m_is_allow_conversation;

    // Main window variables
    QSettings *settingsDatabase;
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
    bool canMoveWindow;

    // System Tray variables
    QSystemTrayIcon *m_tray_icon;
    QAction* m_restore_text_window_action;
    QAction* m_quit_action;
    QAction* m_stop_listening_action;
    QAction* m_feedback_support_action;
    QMenu* m_tray_icon_menu;

    // Tip of the day variables
    QTimer *m_tip_of_the_day_timer;
    QQuickView *m_tip_of_the_day_view;
};

#endif // MAINWINDOW_H
