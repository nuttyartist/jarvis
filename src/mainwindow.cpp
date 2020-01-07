#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "speechtotext.h"

#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFontDatabase>
#include <QTextCodec>
#include <QTimeZone>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include <random>

#define TIP_OF_THE_DAY_TIMER_INTERVAL_CHECK 60000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    microphone_thread(nullptr),
    wake_up_word_thread(nullptr),
    speech_to_text_thread(nullptr),
    text_to_speech_thread(nullptr),
    settingsDatabase(nullptr)
{
    ui->setupUi(this);
    setUpVariables();
    set_up_settings();
    setUpWindow();
    setUpTextEdit();
    setUpTrafficLights();
    setUpFrames();
    setUpLineEdit();
    setUpLabel();
    setupTrayIcon();
    setupTipOfTheDay();
    setupSignalsSlots();
    setUpAerEngine();
    setUpSkillsEngine();
    m_restore_text_window_action->trigger();
    //load_ai_server();

    QTimer::singleShot(50, this, [this] () {
        setUpMicrophoneRecorder();
        setUpSpeechToTextEngine();
        setUpTextToSpeechEngine();
        setUpWakeUpWordEngine();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*************************************/
/**** Setup settings, and widgets ****/
/*************************************/

size_t randomGenerator(size_t min, size_t max) {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    //rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<std::mt19937::result_type> dist{uint(min), uint(max)};

    return dist(rng);
}

QString get_random_number_string()
{
    int random_number_1 = static_cast<int>(randomGenerator(10000000, 1000000000));
    int random_number_2 = static_cast<int>(randomGenerator(10000000, 1000000000));

    return QString::number(random_number_1) + QString::number(random_number_2);
}

void MainWindow::set_up_settings()
{
    settingsDatabase = new QSettings(QStringLiteral("%1/.deus/settings.ini").arg(QDir::homePath()), QSettings::IniFormat);
    settingsDatabase->setFallbacksEnabled(false);

    if(settingsDatabase->value("userID", "NULL") == "NULL")
    {
        QString random_user_id = get_random_number_string();
        settingsDatabase->setValue("userID", random_user_id);
    }

    m_user_id = settingsDatabase->value("userID", "NULL").toString();
}

void MainWindow::setUpWindow()
{
#ifdef _WIN32
    this->setWindowFlags(Qt::CustomizeWindowHint);
#else
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->centralWidget()->hide();
#endif

    if(settingsDatabase && settingsDatabase->value("windowGeometry", "NULL") != "NULL")
    {
        this->restoreGeometry(settingsDatabase->value("windowGeometry").toByteArray());
    }
}

void MainWindow::setUpTrafficLights()
{
    QString ss = "QPushButton { "
                 "  border: none; "
                 "  padding: 0px; "
                 "}";

    ui->redCloseButton->setStyleSheet(ss);
    ui->yellowMinimizeButton->setStyleSheet(ss);
    ui->greenMaximizeButton->setStyleSheet(ss);

    ui->redCloseButton->installEventFilter(this);
    ui->yellowMinimizeButton->installEventFilter(this);
    ui->greenMaximizeButton->installEventFilter(this);

    ui->redCloseButton->setIcon(QIcon(":images/red.png"));
    ui->yellowMinimizeButton->setIcon(QIcon(":images/yellow.png"));
    ui->greenMaximizeButton->setIcon(QIcon(":images/green.png"));

    ui->yellowMinimizeButton->hide();
    ui->greenMaximizeButton->hide();
}

void MainWindow::setUpFrames()
{
    QString ss = "QFrame{ "
                 "  background-color: rgb(39,50,58); "
                 "  border: none;"
                 "  border-radius: 7px"
                 "}";

    // Backgrounds
    ui->frame_2->setStyleSheet(ss);
    ui->frame_3->setStyleSheet(ss);

    // The grey line
    ui->frame->setStyleSheet("QFrame {background-color: rgb(90, 90, 90)}");
}

void MainWindow::setUpTextEdit()
{
    QString ss = QString("QTextEdit {background-color: rgb(39,50,58); color: rgb(255, 255, 255); padding-left: %1px; padding-right: %2px; padding-bottom:2px;} "
                             "QScrollBar::handle:vertical:hover { background: rgb(125, 125, 125); } "
                             "QScrollBar::handle:vertical { border-radius: 4px; background: rgb(90, 90, 90); min-height: 20px; }  "
                             "QScrollBar::vertical {border-radius: 4px; width: 8px; color: rgba(255, 255, 255,0);} "
                             "QScrollBar {margin: 0; background: transparent;} "
                             "QScrollBar::add-line:vertical { width:0px; height: 0px; subcontrol-position: bottom; subcontrol-origin: margin; }  "
                             "QScrollBar::sub-line:vertical { width:0px; height: 0px; subcontrol-position: top; subcontrol-origin: margin; }"
                             ).arg("15", "0");

    ui->textEdit->setStyleSheet(ss);
    ui->textEdit->setReadOnly(true);
    ui->textEdit->setTextColor(QColor(255, 255, 255));

    // Fonts
    int font_id = QFontDatabase::addApplicationFont(":/fonts/roboto_mono/RobotoMono-Regular.ttf");
    QString roboto_family = QFontDatabase::applicationFontFamilies(font_id).at(0);
    QFont roboto(roboto_family, 13);
    //ui->textEdit->setFont(roboto);
    ui->textEdit->setFont(QFont("Avenir Light", 13));
}

void MainWindow::setUpLineEdit()
{
    QString ss = QString("QLineEdit{ "
                             "  padding-right: 15px; "
                             "  padding-left: 15px;"
                             "  padding-right: 19px;"
                             "  border: 0px solid rgb(205, 205, 205);"
                             "  border-radius: 0px;"
                             "  background-color: rgb(39,50,58);"
                             "  color: rgb(255, 255, 255);"
                             "  selection-background-color: rgb(61, 155, 218);"
                             "  border-radius: 7px"
                             "} "
                             "QToolButton { "
                             "  border: none; "
                             "  padding: 0px;"
                             "}"
                             );

    ui->lineEdit->setStyleSheet(ss);
    ui->lineEdit->setFocus();
    ui->lineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);

    // Fonts
    int font_id = QFontDatabase::addApplicationFont(":/fonts/roboto_mono/RobotoMono-Regular.ttf");
    QString roboto_family = QFontDatabase::applicationFontFamilies(font_id).at(0);
    QFont roboto(roboto_family, 13);
    //ui->lineEdit->setFont(roboto);
    ui->lineEdit->setFont(QFont("Avenir Light", 13));
}

void MainWindow::setUpLabel()
{
    ui->label->setText("");
    ui->label->setStyleSheet("QLabel {color: rgb(255,255,255)}");

    int font_id = QFontDatabase::addApplicationFont(":/fonts/roboto_mono/RobotoMono-Regular.ttf");
    QString roboto_family = QFontDatabase::applicationFontFamilies(font_id).at(0);
    QFont roboto(roboto_family, 13);
    //ui->label->setFont(roboto);
    ui->label->setFont(QFont("Avenir Light", 13));
}

void MainWindow::setupTrayIcon()
{   
    m_tray_icon = new QSystemTrayIcon(this);
    m_tray_icon_menu = new QMenu(this);

    m_restore_text_window_action = new QAction(tr("&Show Text Window"), this);
    m_quit_action = new QAction(tr("&Quit"), this);
    m_stop_listening_action = new QAction(tr("&Stop Listening"), this);
    //m_feedback_support_action = new QAction(tr("&Feedback/Support"), this);
    m_tray_icon_menu->addAction(m_restore_text_window_action);
    m_tray_icon_menu->addAction(m_stop_listening_action);
    m_tray_icon_menu->addSeparator();
   // m_tray_icon_menu->addAction(m_feedback_support_action);
    m_tray_icon_menu->addSeparator();
    m_tray_icon_menu->addAction(m_quit_action);

    QIcon icon(":images/deus_system_tray_regular.png");
    m_tray_icon->setIcon(icon);
    m_tray_icon->setContextMenu(m_tray_icon_menu);
    m_tray_icon->show();

    connect(m_restore_text_window_action, &QAction::triggered, this, [this](){
        if(this->centralWidget()->isHidden())
        {
            m_restore_text_window_action->setText(tr("&Hide Text Window"));
            this->centralWidget()->show();
            this->ui->lineEdit->setFocus();
            this->ui->lineEdit->setFocus();
        }
        else
        {
            m_restore_text_window_action->setText(tr("&Show Text Window"));
            this->centralWidget()->hide();
        }
    });

    //connect(m_feedback_support_action, &QAction::triggered, this, [this](){
    //});

    connect(m_quit_action, &QAction::triggered, this, &MainWindow::close);

    connect(m_stop_listening_action, &QAction::triggered, this, [this] () {
        this->speech_to_text_engine.set_stop_listening();
        //this->wake_up_word_engine.set_stop_listening();
    });

    connect(&speech_to_text_engine, &SpeechToText::in_speech_to_text, this, [this] () {
        if (m_stop_listening_action) {
            m_stop_listening_action->setEnabled(true);
        }
    });

    connect(&speech_to_text_engine, &SpeechToText::out_of_speech_to_text, this, [this] () {
        m_stop_listening_action->setEnabled(false);
    });

    connect(&skills_engine, &Skills::stop_listening_action, this, [this] () {
        m_stop_listening_action->trigger();
    });

    connect(&skills_engine, &Skills::disable_conversation, this, [this] () {
        this->m_is_allow_conversation = false;
    });

    connect(&skills_engine, &Skills::enable_conversation, this, [this] () {
        this->m_is_allow_conversation = true;
    });

    connect(&speech_to_text_engine, &SpeechToText::in_speech_to_text, this, [this](){
        if (m_tray_icon) {
            QIcon icon(":images/deus_system_tray_listening.png");
            m_tray_icon->setIcon(icon);
        }
    });

    connect(&speech_to_text_engine, &SpeechToText::out_of_speech_to_text, this, [this](){
        QIcon icon(":images/deus_system_tray_regular.png");
        m_tray_icon->setIcon(icon);
    });
}

void MainWindow::setupTipOfTheDay()
{
    m_tip_of_the_day_view = new QQuickView();

    m_tip_of_the_day_timer = new QTimer();
    m_tip_of_the_day_timer->setSingleShot(false);

    connect(m_tip_of_the_day_timer, &QTimer::timeout, this, [this](){

        // TODO
        // if((wasn't visible today || window is not already visible) && the user didn't
        // say he don't want to see it again)
        // + Check only show if it's between a random time between 9am-12pm
        // *OR* after 1 hour of not being idle.

        if(m_tip_of_the_day_view != NULL && !m_tip_of_the_day_view->isActive())
        {
            m_tip_of_the_day_view->setSource(QUrl::fromLocalFile(QStringLiteral(":/tipoftheday.qml")));
            m_tip_of_the_day_view->setTitle(QStringLiteral("Tip of the day"));
            m_tip_of_the_day_view->setResizeMode(QQuickView::SizeRootObjectToView);
            m_tip_of_the_day_view->setMinimumWidth(410);
            m_tip_of_the_day_view->setMinimumHeight(390);
            m_tip_of_the_day_view->show();
            // TODO
            // If the user saw all the tips, show just a random tip,
            // Otherwise, show random tip that the user *never saw before*
        }
    });

    /*if(m_tip_of_the_day_timer != NULL)
    {
        m_tip_of_the_day_timer->start(TIP_OF_THE_DAY_TIMER_INTERVAL_CHECK);
    }*/
}

void MainWindow::opening()
{
    if(!m_is_already_done_opening)
    {
        m_is_already_done_opening = true;

        QString to_say = "Hello, I'm " + m_assistant_name + ".";
        say_to_user(to_say);

        // Sleep because it may wake her up.
        // TODO: directly stop wake_up_word detection when opening.
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

/*************************************/
/**** Setup variables and engines ****/
/*************************************/

void MainWindow::setUpVariables()
{
    m_is_allow_conversation = true;
    m_assistant_name = "Deus";
    m_is_already_done_opening = false;
    //m_is_follow_up_command = false;
}

void MainWindow::setupSignalsSlots()
{
    // 1. When speech-to-text ends, clear history to make room for a new
    // conversation context
    // 2. Create new session id's for meta and content in database.
    connect(&speech_to_text_engine, &SpeechToText::out_of_speech_to_text, this, [this] () {
        ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
    });

    connect(&skills_engine, SIGNAL(show_text_window()), this, SLOT(show_text_window()));
    connect(&skills_engine, SIGNAL(hide_text_window()), this, SLOT(hide_text_window()));
}

void MainWindow::setUpAerEngine()
{
    aer_engine.initialize();
}

void MainWindow::setUpSkillsEngine()
{
    skills_engine.initialize();
    connect(&skills_engine, SIGNAL(to_say(QString)), this, SLOT(say_to_user(QString)));
    //connect(&skills_engine, SIGNAL(set_follow_up_command(SkillsFunc)), this, SLOT(set_follow_up_command(SkillsFunc)));
}

void MainWindow::setUpMicrophoneRecorder()
{
    microphone_thread = new QThread(this);
    microphone_recorder_engine.moveToThread(microphone_thread);

    connect(microphone_thread, SIGNAL(started()), &microphone_recorder_engine, SLOT(start_listen()));
    connect(&microphone_recorder_engine, SIGNAL(microphone_data_raw(const QByteArray&)), &wake_up_word_engine, SLOT(listen(const QByteArray&)));
    connect(&microphone_recorder_engine, &MicrophoneRecorder::microphone_data_raw,
            this, [this] (const QByteArray &data) {
        this->speech_to_text_engine.listen(data);
    });

    microphone_thread->start();
}

void MainWindow::setUpSpeechToTextEngine()
{
    speech_to_text_thread = new QThread(this);
    speech_to_text_engine.moveToThread(speech_to_text_thread);

    connect(speech_to_text_thread, SIGNAL(started()), &speech_to_text_engine, SLOT(initialize()));
    connect(&speech_to_text_engine, SIGNAL(finished_speech_to_text(QString)), this, SLOT(process_user_input(QString)));
    connect(&speech_to_text_engine, &SpeechToText::finished_speech_to_text, this, &MainWindow::finished_speech_to_text);

    speech_to_text_thread->start();
}

void MainWindow::setUpTextToSpeechEngine()
{
    text_to_speech_thread = new QThread(this);
    text_to_speech_engine.moveToThread(text_to_speech_thread);

    connect(text_to_speech_thread, SIGNAL(started()), &text_to_speech_engine, SLOT(initialize()));
    connect(&text_to_speech_engine, SIGNAL(finished_initializing()), this, SLOT(opening()));
    connect(this, SIGNAL(send_to_text_to_speech(QString)), &text_to_speech_engine, SLOT(synthesize(QString)));
    connect(&text_to_speech_engine, &TextToSpeech::is_talking,
            this, [this] (bool talking) {
        this->speech_to_text_engine.set_talking(talking);
    });

    text_to_speech_thread->start();
}

void MainWindow::setUpWakeUpWordEngine()
{
    wake_up_word_thread = new QThread(this);
    wake_up_word_engine.moveToThread(wake_up_word_thread);

    connect(wake_up_word_thread, SIGNAL(started()), &wake_up_word_engine, SLOT(initialize()));
    connect(&wake_up_word_engine, SIGNAL(wake_up_word_detected()), &speech_to_text_engine, SLOT(start_speech_to_text()));

    connect(&text_to_speech_engine, SIGNAL(is_talking(bool)), &wake_up_word_engine, SLOT(set_stop_listening_for_wake_up_word(bool)));

    connect(&speech_to_text_engine, &SpeechToText::in_speech_to_text, this, [this] () {
        this->wake_up_word_engine.set_stop_listening_for_wake_up_word(true);
    });
    connect(&speech_to_text_engine, &SpeechToText::out_of_speech_to_text, this, [this] () {
       this->wake_up_word_engine.set_stop_listening_for_wake_up_word(false);
    });

    wake_up_word_thread->start();
}

/*void MainWindow::set_follow_up_command(SkillsFunc skill_func)
{
    m_follow_up_command = skill_func;
    m_is_follow_up_command = true;
}*/

/************************/
/**** UI interaction ****/
/************************/

void MainWindow::show_text_window()
{
    if(this->centralWidget()->isHidden())
    {
        m_restore_text_window_action->trigger();
    }
}

void MainWindow::hide_text_window()
{
    if(!this->centralWidget()->isHidden())
    {
        m_restore_text_window_action->trigger();
    }
}

void MainWindow::start_speech_to_text_once()
{
    emit this->wake_up_word_engine.wake_up_word_detected();
}

void MainWindow::stop_speech_to_text()
{
    emit this->speech_to_text_engine.set_stop_listening();
}

/**
 * Send message (both User and AI) to textEdit
*/
void MainWindow::send_message_to_textedit(QString speakerName, QString message, QColor color)
{
    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->setTextColor(color);
    ui->textEdit->insertPlainText(speakerName);
    ui->textEdit->setTextColor(Qt::white);
    ui->textEdit->insertPlainText(message + "\n");
    ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
}

void MainWindow::say_to_user(QString message)
{
    send_message_to_textedit("AI: ", message, QColor(7, 110, 251));

    // Analytics
    QString current_timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    emit send_to_text_to_speech(message);
}

void MainWindow::process_user_input(QString user_input)
{
    send_message_to_textedit("User: ", user_input, QColor(7, 110, 251));

    /*if(m_is_follow_up_command)
    {
        qDebug() << "Follow-up Command Found";
        Command empty_command;
        (this->*m_follow_up_command)(&empty_command);
        return;
    }*/

    std::string std_user_input = user_input.toStdString();
    std::vector<Command> commands_found = aer_engine.is_user_input_command_or_friendly(std_user_input);
    QString current_timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    if(!commands_found.empty())
    {
        qDebug() << "Command Found";

        skills_engine.decide_command_to_execute(commands_found);
    }
    else if(m_is_allow_conversation)
    {
        qDebug() << "Chit Chat";

        do_chatbot_response(user_input);
    }
}

/**
 * @brief
 * User pressed enter in lineEdit
 */
void MainWindow::on_lineEdit_returnPressed()
{
    QString user_input = ui->lineEdit->text();

    process_user_input(user_input);

    ui->lineEdit->clear();
}

/*****************/
/**** Chatbot ****/
/*****************/

void MainWindow::do_chatbot_response(QString user_input)
{

}

/****************************************/
/**** Traffic light buttons handling ****/
/****************************************/

void MainWindow::on_redCloseButton_pressed()
{
    ui->redCloseButton->setIcon(QIcon(":images/redPressed.png"));
}

void MainWindow::on_yellowMinimizeButton_pressed()
{
    ui->yellowMinimizeButton->setIcon(QIcon(":images/yellowPressed.png"));
}

void MainWindow::on_greenMaximizeButton_pressed()
{
    if(this->windowState() == Qt::WindowFullScreen){
        ui->greenMaximizeButton->setIcon(QIcon(":images/greenInPressed.png"));
    }else{
        ui->greenMaximizeButton->setIcon(QIcon(":images/greenPressed.png"));
    }
}

void MainWindow::on_redCloseButton_clicked()
{
    ui->redCloseButton->setIcon(QIcon(":images/red.png"));
    this->m_restore_text_window_action->trigger();
}

void MainWindow::on_yellowMinimizeButton_clicked()
{
    ui->yellowMinimizeButton->setIcon(QIcon(":images/yellow.png"));
    showMinimized();
}

void MainWindow::on_greenMaximizeButton_clicked()
{
    ui->greenMaximizeButton->setIcon(QIcon(":images/green.png"));

    if(isFullScreen()){
        showNormal();
    }else{
        showFullScreen();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    speech_to_text_thread->quit();
    text_to_speech_thread->quit();
    microphone_thread->quit();
    wake_up_word_thread->quit();

    if(this->windowState() != Qt::WindowFullScreen)
    {
        if(settingsDatabase)
        {
            settingsDatabase->setValue("windowGeometry", this->saveGeometry());
        }
    }

    QApplication::quit();
}

/******************************/
/**** Window dragging code ****/
/******************************/

/**
* Set variables to the position of the window when the mouse is pressed
* And set variables for resizing
*/
void MainWindow::mousePressEvent (QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(event->pos().x() < ui->centralWidget->geometry().width() && event->pos().y() < ui->textEdit->y())
        {
            canMoveWindow = true;
            m_nMouseClick_X_Coordinate = event->x();
            m_nMouseClick_Y_Coordinate = event->y();
        }
    }

    event->accept();
}

/**
* Move the window according to the mouse positions
* And resizing
*/
void MainWindow::mouseMoveEvent (QMouseEvent* event)
{
    if(canMoveWindow)
    {
        //this->setCursor(Qt::ClosedHandCursor);

        move (event->globalX() - m_nMouseClick_X_Coordinate, event->globalY() - m_nMouseClick_Y_Coordinate);
    }
}

/**
* Set some variables after releasing the mouse for moving the window and resizing
*/
void MainWindow::mouseReleaseEvent (QMouseEvent *event)
{
    canMoveWindow = false;
    this->unsetCursor();
    event->accept();
}

/**
* When the blank area at the top of window is double-clicked the window get maximized
*/
void MainWindow::mouseDoubleClickEvent (QMouseEvent *e)
{
    /*if(isMaximized() || isFullScreen())
    {
        showNormal();
    }
    else
    {
        showMaximized();
    }*/
    e->accept();
}

bool MainWindow::eventFilter (QObject *object, QEvent *event)
{
    /*if(m_timer != NULL)
    {
        ui->label->setText("Timing: " + QString::number(m_timer->remainingTime()));
    }
    else
    {
        ui->label->setText("Timing: 0");
    }*/

    if(event->type() == QEvent::Enter)
    {
        if(qApp->applicationState() == Qt::ApplicationActive)
        {
            if(object == ui->redCloseButton
                    || object == ui->yellowMinimizeButton
                    || object == ui->greenMaximizeButton)
            {

                ui->redCloseButton->setIcon(QIcon(":images/redHovered.png"));
                ui->yellowMinimizeButton->setIcon(QIcon(":images/yellowHovered.png"));
                if(this->windowState() == Qt::WindowFullScreen)
                {
                    ui->greenMaximizeButton->setIcon(QIcon(":images/greenInHovered.png"));
                }
                else
                {
                    ui->greenMaximizeButton->setIcon(QIcon(":images/greenHovered.png"));
                }
            }
        }
    }

    if(event->type() == QEvent::Leave)
    {
        if(qApp->applicationState() == Qt::ApplicationActive)
        {
            if(object == ui->redCloseButton
                    || object == ui->yellowMinimizeButton
                    || object == ui->greenMaximizeButton)
            {

                ui->redCloseButton->setIcon(QIcon(":images/red.png"));
                ui->yellowMinimizeButton->setIcon(QIcon(":images/yellow.png"));
                ui->greenMaximizeButton->setIcon(QIcon(":images/green.png"));
            }
        }
    }

    if(event->type() == QEvent::WindowActivate)
    {
        ui->lineEdit->setFocus();
    }

    return QObject::eventFilter(object, event);
}
