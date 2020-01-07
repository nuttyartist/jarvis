#include "skills.h"

Skills::Skills(QObject *parent) : QObject(parent)
{

}

/**
* @brief
* Initialize the vectors that contain pointer to skill functions
* and skill names. This allows us to match between the skill name to be exectued
* and its crrosponding function in the code.
*/
void Skills::initialize()
{
    skill_names_and_pointers = {
        {"google", &Skills::GoogleSearch},
        {"youtube", &Skills::YoutubeSearch},
        {"googlePictures", &Skills::GooglePicturesSearch},
        {"wikipedia", &Skills::WikipediaSearch},
        {"mapsDirection", &Skills::MapsDirection},
        {"searchAndShow", &Skills::SearchAndShow},
        {"openSpecificApp", &Skills::OpenSpecificApp},
        {"showDesktop", &Skills::ShowDesktop},
        {"showWindowFullScreen", &Skills::ShowWindowFullScreen},
        {"minimizeWindow", &Skills::MinimizeWindow},
        {"hideWindow", &Skills::HideWindow},
        {"startDictation", &Skills::StartDictation},
        {"showAllWindows", &Skills::ShowAllWindows},
        {"showOnlyCurrentWindow", &Skills::ShowOnlyCurrentWindow},
        {"takeScreenScreenshot", &Skills::TakeScreenScreenshot},
        {"takeSelectionScreenshot", &Skills::TakeSelectionScreenshot},
        {"takeWindowScreenshot", &Skills::TakeWindowScreenshot},
        {"quickLook", &Skills::QuickLook},
        {"createNewFolder", &Skills::CreateNewFolder},
        {"searchComputer", &Skills::SearchComputer},
        {"putWindowToRight", &Skills::PutWindowToRight},
        {"putWindowToLeft", &Skills::PutWindowToLeft},
        {"putWindowToCenter", &Skills::PutWindowToCenter},
        {"showTextWindow", &Skills::ShowTextWindow},
        {"hideTextWindow", &Skills::HideTextWindow},
        {"stopListening", &Skills::StopListening},
        {"disableConversation", &Skills::DisableConversation},
        {"enableConversation", &Skills::EnableConversation},
        {"findOnPage", &Skills::FindOnPage},
        {"copyPathOfFolder", &Skills::CopyPathOfFolder},
        {"refreshPage", &Skills::RefreshPage},
        {"arrangeFolderByName", &Skills::ArrangeFolderByName},
        {"arrangeFolderByKind", &Skills::ArrangeFolderByKind},
        {"arrangeFolderByDateLastOpened", &Skills::ArrangeFolderByDateLastOpened},
        {"arrangeFolderByDateAdded", &Skills::ArrangeFolderByDateAdded},
        {"arrangeFolderByDateModified", &Skills::ArrangeFolderByDateModified},
        {"arrangeFolderBySize", &Skills::ArrangeFolderBySize},
        {"arrangeFolderByTags", &Skills::ArrangeFolderByTags},
        {"quitCertainApp", &Skills::QuitCertainApp},
        {"zoomIn", &Skills::ZoomIn},
        {"zoomOut", &Skills::ZoomOut},
        {"zoomReset", &Skills::ZoomReset},
        {"whatIsTheTime", &Skills::WhatTimeIsIt},
        {"startTimer", &Skills::StartTimer},
        {"startTimerSaved", &Skills::StartTimerSaved},
        {"whatCommandsCanISay", &Skills::WhatCommandsCanISay},
        {"googlePicturesSaved", &Skills::GooglePicturesSearchSaved},
        {"goToSpecificWebsite", &Skills::GoToSpecificWebsite},
        {"tryToOpenSpecificApp", &Skills::TryToOpenSpecificApp},
        {"openTabsInSpecificWebBrowser", &Skills::OpenTabsInSpecificWebBrowser},
        {"openWindowInSpecificWebBrowser", &Skills::OpenWindowInSpecificWebBrowser},
        {"tryToOpenTabs", &Skills::TryToOpenTabs},
        {"tryToOpenWindow", &Skills::TryToOpenWindow},
        {"searchSpecificSearchEngine", &Skills::SearchSpecificSearchEngine},
        {"pronouncePhrase", &Skills::PronouncePhrase},
        {"definePhrase", &Skills::DefinePhrase},
        {"quitCurrentApp", &Skills::QuitCurrentApp},
        {"whatIsTheWeather", &Skills::WhatIsTheWeather}
    };
}

/**
* @brief
* Here we decide what skill found by the AER to execute.
* These are the criterias:
* 1. The more 'num_connection' a command has, the more suitable it is for the user input.
* 2. A command with a knowledge is more suitable then a command without one (more specific to the user input).
* 3. If we have more than one command with knowledge, the longer knowledge is the more suitable one (more specific to the user input).
* (?) If more than one knowledge is present in a single command?
* (?) If we have more than one command without knowledge with the same 'num_connection', what should we do?
*/
void Skills::decide_command_to_execute(std::vector<Command> commands_found)
{
    int number_of_commads_with_knowledge = 0;
    std::vector<Command*> commands_with_knowledge;
    int most_connections = -1;
    Command* command_with_most_connections;

    for (unsigned int i = 0; i < commands_found.size(); i++)
    {
        Command* command = &commands_found.at(i);

        if(!command->element_plus_knowledge_datas.empty())
        {
            number_of_commads_with_knowledge++;
            commands_with_knowledge.push_back(command);
        }

        if(command->num_connection > most_connections)
        {
            most_connections = command->num_connection;
            command_with_most_connections = command;
        }
    }

    // Check if we have commands with knowledge.
    if(number_of_commads_with_knowledge == 1) // If only one, execute it.
    {
        qDebug() << "here 4";
        this->execute_skill(commands_with_knowledge.at(0));
    }
    else if(number_of_commads_with_knowledge > 1) // If more than one, choose the one with longest knowledge length.
    {
        int best_length = -1;
        Command* best_command;

        for (unsigned int i = 0; i < commands_with_knowledge.size(); i++)
        {
            Command* command = commands_with_knowledge.at(i);
            std::string command_knowledge_key = command->element_plus_knowledge_datas.at(0).knowledge_data->key;
            int current_command_knowledge_length = command_knowledge_key.length();

            if(current_command_knowledge_length > best_length)
            {
                best_length = current_command_knowledge_length;
                best_command = command;
            }
        }

        this->execute_skill(best_command);

        // (?) If more than one knowledge is present in a command?
    }
    else // If no command with knowledge found, choose the one with biggest num_connection
    {
        this->execute_skill(command_with_most_connections);

        // (?) If more than one command without knowledge found with the same num_connection?
    }
}

/**
* @brief
* Here we exectue the skill found using the AER.
* We do this by matching between the skill name to be exectued
* and its corosponding function in the code held by the skill_names_and_pointers.
*/
void Skills::execute_skill(Command* command_found)
{
    print_command_attributes(command_found);

    QString command_action_name = QString::fromStdString(command_found->action_name);
    if(skill_names_and_pointers.contains(command_action_name))
    {
        (this->*skill_names_and_pointers.value(command_action_name))(command_found);
    }
    else
    {
        emit to_say("This command isn't supported yet.");
    }
}

/**
* @brief
* Print the attributes of a given command.
*/
void Skills::print_command_attributes(Command* command)
{
    qDebug() << "    action_name: " << QString::fromStdString(command->action_name);
    qDebug() << "    num_connection: " << QString::number(command->num_connection);
    qDebug() << "        ------------------------------" ;

    // Knowledge

    for(unsigned i = 0; i < command->data.size(); i++)
    {
        qDebug() << "        element_type: " << QString::fromStdString(command->data.at(i).element_type);
        qDebug() << "        entity_name: " << QString::fromStdString(command->data.at(i).entity_name);
        qDebug() << "        content: " << QString::fromStdString(command->data.at(i).content);
        qDebug() << "        ------------------------------" ;
    }
}

/************************************************************************/
/***************************Auxiliary functions**************************/
/************************************************************************/

/**
* @brief
* Search in the commandData vector to find an Entity_Data with the
* same entity as 'entity'. For example, we may want to get the entity
* 'searchTerm' that is held inside the given command. So we'll do:
* GetEntity (&bestMatch->data, "searchTerm");
*/
Entity_Data Skills::GetEntity (std::vector<Entity_Data>* commandData, std::string entity)
{
    Entity_Data tempData;
    tempData.element_type = _NULL_;
    tempData.entity_name = _NULL_;
    tempData.content = _NULL_;

    for(unsigned i = 0; i < commandData->size(); i++)
    {
        if(commandData->at(i).entity_name == entity)
        {
            tempData = commandData->at(i); // return commandData->at(i);

            if(tempData.element_type == ACTION_ELEMENT)
            {
                return tempData;
            }
        }
    }

    return tempData;
}

/**
* @brief
* Open a url in default browser.
*/
void Skills::OpenUrl (std::string url)
{
    QString link = QString::fromStdString(url);
    QDesktopServices::openUrl(QUrl(link));
}

/**
* @brief
* Tries to open a given app.
* Search the installed applications folder for matching apps.
* Return true if an app was found, else, return false.
* TODO: search in subdirectories. I tried to do it but i don't
* know how to ignore searching inside .app while still getting
* .app files.
*/
bool Skills::OpenAppMac(std::string app_to_open)
{
    QString app_to_try_to_open = QString::fromStdString(app_to_open);
    QString current_app_found;

    //QDirIterator it("/Applications", QStringList() << "*.app", QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QDirIterator it("/Applications", QDir::AllEntries);
    while (it.hasNext())
    {
        current_app_found = it.next();
        if(current_app_found.contains(app_to_try_to_open, Qt::CaseInsensitive))
        {
            current_app_found = current_app_found.replace(current_app_found.lastIndexOf("/"), 1, "/\"");
            current_app_found = current_app_found.replace(".app", ".app\"");
            QString open_cmd = QString("open %1").arg(current_app_found); // may need QUrl::fromLocalFile(fullpath)
            QProcess::startDetached(open_cmd);
            return true;
        }
    }

    return false;
}

/**
* @brief
* Given a string key name, returns the corrosponding key code
* for the conversion to Mac's CGKeyCode to work.
* TODO: This is not a sufficient implementation. It's not pretty,
* and it only works for U.S. QWERTY. We should make it better.
*/
int Skills::GetkeyCodeFromStringMac(std::string keyString)
{
    if (keyString == "a") return 0;
    if (keyString == "s") return 1;
    if (keyString == "d") return 2;
    if (keyString == "f") return 3;
    if (keyString == "h") return 4;
    if (keyString == "g") return 5;
    if (keyString == "z") return 6;
    if (keyString == "x") return 7;
    if (keyString == "c") return 8;
    if (keyString == "v") return 9;
    // what is 10?
    if (keyString == "b") return 11;
    if (keyString == "q") return 12;
    if (keyString == "w") return 13;
    if (keyString == "e") return 14;
    if (keyString == "r") return 15;
    if (keyString == "y") return 16;
    if (keyString == "t") return 17;
    if (keyString == "1") return 18;
    if (keyString == "2") return 19;
    if (keyString == "3") return 20;
    if (keyString == "4") return 21;
    if (keyString == "6") return 22;
    if (keyString == "5") return 23;
    if (keyString == "=") return 24;
    if (keyString == "9") return 25;
    if (keyString == "7") return 26;
    if (keyString == "-") return 27;
    if (keyString == "8") return 28;
    if (keyString == "0") return 29;
    if (keyString == "]") return 30;
    if (keyString == "o") return 31;
    if (keyString == "u") return 32;
    if (keyString == "[") return 33;
    if (keyString == "i") return 34;
    if (keyString == "p") return 35;
    if (keyString == "RETURN") return 36;
    if (keyString == "l") return 37;
    if (keyString == "j") return 38;
    if (keyString == "'") return 39;
    if (keyString == "k") return 40;
    if (keyString == ";") return 41;
    if (keyString == "\\") return 42;
    if (keyString == ",") return 43;
    if (keyString == "/") return 44;
    if (keyString == "n") return 45;
    if (keyString == "m") return 46;
    if (keyString == ".") return 47;
    if (keyString == "TAB") return 48;
    if (keyString == "SPACE") return 49;
    if (keyString == "`") return 50;
    if (keyString == "DELETE") return 51;
    if (keyString == "ENTER") return 52;
    if (keyString == "ESCAPE") return 53;
    if (keyString == "COMMAND") return 55;
    if (keyString == "SHIFT") return 56;
    if (keyString == "CAPSLOCK") return 57;
    if (keyString == "OPTION") return 58;
    if (keyString == "CONTROL") return 59;
    // some more missing codes abound, reserved I presume, but it would
    // have been helpful for Apple to have a document with them all listed
    if (keyString == "FN") return 63;
    if (keyString == ".") return 65;
    if (keyString == "*") return 67;
    if (keyString == "+") return 69;
    if (keyString == "CLEAR") return 71;
    if (keyString == "/") return 75;
    if (keyString == "ENTER") return 76;  // numberpad on full kbd
    if (keyString == "=") return 78;
    if (keyString == "=") return 81;
    if (keyString == "0") return 82;
    if (keyString == "1") return 83;
    if (keyString == "2") return 84;
    if (keyString == "3") return 85;
    if (keyString == "4") return 86;
    if (keyString == "5") return 87;
    if (keyString == "6") return 88;
    if (keyString == "7") return 89;
    if (keyString == "8") return 91;
    if (keyString == "9") return 92;
    if (keyString == "F5") return 96;
    if (keyString == "F6") return 97;
    if (keyString == "F7") return 98;
    if (keyString == "F3") return 99;
    if (keyString == "F8") return 100;
    if (keyString == "F9") return 101;
    if (keyString == "F11") return 103;
    if (keyString == "F13") return 105;
    if (keyString == "F14") return 107;
    if (keyString == "F10") return 109;
    if (keyString == "F12") return 111;
    if (keyString == "F15") return 113;
    if (keyString == "HELP") return 114;
    if (keyString == "HOME") return 115;
    if (keyString == "PGUP") return 116;
    if (keyString == "DELETE") return 117;
    if (keyString == "F4") return 118;
    if (keyString == "END") return 119;
    if (keyString == "F2") return 120;
    if (keyString == "PGDN") return 121;
    if (keyString == "F1") return 122;
    if (keyString == "LEFT") return 123;
    if (keyString == "RIGHT") return 124;
    if (keyString == "DOWN") return 125;
    if (keyString == "UP") return 126;

    return -1;
    //fprintf(stderr, "keyString %s Not Found. Aborting...\n", keyString);
    //exit(EXIT_FAILURE);
}

/**
* @brief
* Given CGEventFlags and CGKeyCode it execute a keyboard shortcut on MacOS.
*/
void Skills::ExectueKeyboardShortcutHelperMac(std::vector<CGEventFlags> flags_vector, CGKeyCode key_mac)
{
    CGEventFlags flags;
    if(!flags_vector.empty())
    {
        flags = flags_vector[0];
        for(unsigned int i = 1; i < flags_vector.size(); i++)
        {
            flags = flags | flags_vector[i];
        }
    }

    CGEventRef ev;
    CGEventSourceRef source = CGEventSourceCreate (kCGEventSourceStateCombinedSessionState);

    //press down
    ev = CGEventCreateKeyboardEvent (source, key_mac, true);
    if(!flags_vector.empty()) CGEventSetFlags(ev,flags | CGEventGetFlags(ev)); //combine flags if exist
    CGEventPost(kCGHIDEventTap,ev);
    CFRelease(ev);

    //press up
    ev = CGEventCreateKeyboardEvent (source, key_mac, false);
    if(!flags_vector.empty()) CGEventSetFlags(ev,flags | CGEventGetFlags(ev)); //combine flags if exist
    CGEventPost(kCGHIDEventTap,ev);
    CFRelease(ev);

    CFRelease(source);
}

/**
* @brief
* Exectue a given keyboard shortcut on MacOS given a string vector
* that holds the desired keyboard shortcut. For example, given
* {"command", "option", "h"} -> it will hide all windows except the current one.
* TODO: QHash here require converting the special_keys_strings to QString
* all the time since keys in QHash must be QStrings. Is it worth it?
*/
void Skills::ExectueKeyboardShortcutMac(std::vector<std::string>* keyboard_shortcut)
{
    QHash<QString, CGEventFlags> special_keys_strings_and_keys_flags = {
        {"command", kCGEventFlagMaskCommand},
        {"option", kCGEventFlagMaskAlternate},
        {"control", kCGEventFlagMaskControl},
        {"shift", kCGEventFlagMaskShift},
        {"function", kCGEventFlagMaskSecondaryFn}
    };

    std::vector<CGEventFlags> special_keys_flags_to_send;
    for(unsigned int i = 0; i < keyboard_shortcut->size(); i++)
    {
        QString special_key_string = QString::fromStdString(keyboard_shortcut->at(i));
        if(special_keys_strings_and_keys_flags.contains(special_key_string))
        {
            special_keys_flags_to_send.push_back(special_keys_strings_and_keys_flags.value(special_key_string));
        }
    }

    std::string key_string = keyboard_shortcut->at(keyboard_shortcut->size()-1); // the regular key - not flag
    CGKeyCode key_mac = (CGKeyCode)GetkeyCodeFromStringMac(key_string);
    ExectueKeyboardShortcutHelperMac(special_keys_flags_to_send, key_mac);
}

/*(BOOL) terminateService:(NSString *) bundleId{
    BOOL res =NO;
    for ( NSRunningApplication *app in [[NSWorkspace sharedWorkspace] runningApplications] ){
        if ( [bundleId isEqualToString:[app bundleIdentifier]] ){
            res = [app forceTerminate];
        }
    }
    return res;
}*/

int Skills::FindAppId(std::string app_name)
{
    std::transform(app_name.begin(), app_name.end(), app_name.begin(), ::tolower);
    pid_t pids[2048];
    int bytes = proc_listpids(PROC_ALL_PIDS, 0, pids, sizeof(pids));
    int n_proc = bytes / sizeof(pids[0]);
    for (int i = 0; i < n_proc; i++) {
        struct proc_bsdinfo proc;
        int st = proc_pidinfo(pids[i], PROC_PIDTBSDINFO, 0,
                             &proc, PROC_PIDTBSDINFO_SIZE);
        if (st == PROC_PIDTBSDINFO_SIZE) {
            std::string pbi_name = proc.pbi_name;
            std::transform(pbi_name.begin(), pbi_name.end(), pbi_name.begin(), ::tolower);
            if(app_name == pbi_name)
            {
                return pids[i];
                //qDebug() << pids[i] << " " << proc.pbi_comm << " " << proc.pbi_name;
            }
        }
    }
    return -1;
}

void Skills::QuitAppHelper(std::string app_name)
{
    int app_pid = FindAppId(app_name);
    std::string string_command = "kill " + std::to_string(app_pid);
    system(string_command.c_str());
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int Skills::GetTimerDurationFromUserInput(std::string user_input)
{
    std::vector<std::string> time_strings = {"second", "minute", "hour"};
    QHash<QString, std::string> numbers {{"one", "1"},{"two", "2"}, {"three", "3"}, {"four", "4"}, {"five", "5"}, {"six", "6"},
                                 {"seven", "7"}, {"eight", "8"}, {"nine", "9"}, {"ten", "10"}};

    std::string input_string = " " + user_input;

    boost::replace_all(input_string, " and 1/2", "0.5");
    boost::replace_all(input_string, " and 1/4", "0.25");

    int total_duration = 0;
    for(unsigned int i = 0; i < time_strings.size(); i++)
    {
        std::string time_to_find = time_strings[i];

        std::size_t timeStringPos = input_string.find(time_to_find, 0); // first occurrence
        while(timeStringPos != std::string::npos)
        {
            std::size_t durationPos = input_string.rfind(BLANK_SPACE, timeStringPos-2) + 1;
            std::string duration_str = input_string.substr(durationPos, timeStringPos-durationPos-1);

            QString duration_str_qstring = QString::fromStdString(duration_str);
            if(numbers.contains(duration_str_qstring))
            {
                duration_str = numbers.value(duration_str_qstring);
            }

            qDebug() << "duration_str: " << QString::fromStdString(duration_str);
            if(!is_number(duration_str))
            {
                return -1;
            }

            int duration = std::stoi(duration_str);

            if(time_to_find == "second")
            {
                total_duration += duration;
            }
            else if(time_to_find == "minute")
            {
                total_duration += duration*60;
            }
            else if(time_to_find == "hour")
            {
                total_duration += duration*60*60;
            }

            timeStringPos = input_string.find(time_to_find, timeStringPos + 1);
        }
    }

    return total_duration;
}

int Skills::GetNumberFromString(std::string user_input)
{
    QString user_input_qstring = QString::fromStdString(user_input);
    QHash<QString, int> numbers {{"one", 1},{"two", 2}, {"three", 3}, {"four", 4}, {"five", 5}, {"six", 6},
                                 {"seven", 7}, {"eight", 8}, {"nine", 9}, {"ten", 10}};

    if(numbers.contains(user_input_qstring))
    {
        return numbers.value(user_input_qstring);
    }
    else
    {
        if(is_number(user_input))
        {
            return user_input_qstring.toInt();
        }
    }

    return -1;
}

void Skills::StartNewTimer(int total_duration)
{
    QTimer* new_timer = new QTimer();
    new_timer->setSingleShot(true);
    m_timers.push_back(new_timer);

    connect(new_timer, &QTimer::timeout, this, [this](){
        QString string_to_say;

        for(unsigned int i = 0; i < m_timers.size(); i++)
        {
            if(m_timers[i] == QObject::sender())
            {
                string_to_say = "Timer " +  QString::number(i+1) + " has ended.";
            }
        }

        bool found_active_timer = false;
        for(unsigned int i = 0; i < m_timers.size(); i++)
        {
            if(m_timers[i]->isActive())
            {
                found_active_timer = true;
                break;
            }
        }

        if(!found_active_timer)
        {
            m_timers.clear();
        }

        emit to_say(string_to_say);
    });

    qDebug() << "total_duration: " << total_duration;
    new_timer->start(total_duration*1000);
}

/************************************************************************/
/***************************Skills functions*****************************/
/************************************************************************/

/**
* @brief
* Do a Google search.
*/
void Skills::GoogleSearch (Command* bestMatch)
{
    Entity_Data searchTerm = GetEntity (&bestMatch->data, "searchTerm");

    std::string url_to_open = "http://www.google.com/search?q=" + searchTerm.content;

    QString string_to_say = "Searching for '" + QString::fromStdString(searchTerm.content) + "'.";
    emit to_say(string_to_say);

    OpenUrl(url_to_open);

    emit finished_executing_skill();
}

/**
* @brief
* Do a Youtube search.
*/
void Skills::YoutubeSearch(Command* bestMatch)
{
    Entity_Data searchTerm = GetEntity (&bestMatch->data, "searchTerm");

    std::string url_to_open = "http://www.youtube.com/results/?q=" + searchTerm.content;

    QString string_to_say = "Searching for '" + QString::fromStdString(searchTerm.content) + "' on YouTube.";
    emit to_say(string_to_say);

    OpenUrl(url_to_open);

    emit finished_executing_skill();
}

/**
* @brief
* Do a search on a specific search engine.
*/
void Skills::SearchSpecificSearchEngine(Command* bestMatch)
{
    std::string search_engine_name;
    std::string search_engine_url;
    for(unsigned int i = 0; i < bestMatch->element_plus_knowledge_datas.size(); i++)
    {
        if(bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->category_name == "search_engines")
        {
            search_engine_name = bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->key;
            search_engine_url = bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->value.toString().toStdString();
            break;
        }
    }

    Entity_Data searchTerm = GetEntity (&bestMatch->data, "searchTerm");
    std::string url_to_open = search_engine_url + searchTerm.content;
    QString string_to_say = "Searching for '" +
            QString::fromStdString(searchTerm.content) + "' on " +
            QString::fromStdString(search_engine_name) + ".";

    OpenUrl(url_to_open);
    emit to_say(string_to_say);
    emit finished_executing_skill();
}

/**
* @brief
* Do a Google Pictures search.
*/
void Skills::GooglePicturesSearch(Command* bestMatch)
{
    Entity_Data searchTerm = GetEntity (&bestMatch->data, "searchTermPicture");

    std::string url_to_open = "http://www.google.com/search?tbm=isch&q=" + searchTerm.content;
    QString string_to_say = "Showing pictures of '" + QString::fromStdString(searchTerm.content) + "'.";

    emit to_say(string_to_say);
    OpenUrl(url_to_open);
    emit finished_executing_skill();
}

/**
* @brief
* Do a Google Pictures search.
*/
void Skills::GooglePicturesSearchSaved(Command* bestMatch)
{
    Entity_Data searchTermSaved = GetEntity (&bestMatch->data, "searchTermSaved");

    std::string url_to_open = "http://www.google.com/search?tbm=isch&q=" + searchTermSaved.content;
    QString string_to_say = "Showing pictures of '" + QString::fromStdString(searchTermSaved.content) + "'.";

    emit to_say(string_to_say);
    OpenUrl(url_to_open);
    emit finished_executing_skill();
}

/**
* @brief
* Do a Wikipedia search.
*/
void Skills::WikipediaSearch(Command* bestMatch)
{
    Entity_Data searchTerm = GetEntity (&bestMatch->data, "searchTerm");

    std::string url_to_open = "http://en.wikipedia.org/?search=" + searchTerm.content;

    QString string_to_say = "Searching Wikipedia for '" + QString::fromStdString(searchTerm.content) + "'.";
    emit to_say(string_to_say);

    OpenUrl(url_to_open);

    emit finished_executing_skill();
}

/**
* @brief
* Open Google Mpas with the desired directions.
* TODO: Support transit type in link.
*/
void Skills::MapsDirection(Command* bestMatch)
{
    Entity_Data origin = GetEntity (&bestMatch->data, "origin");
    Entity_Data destination = GetEntity (&bestMatch->data, "destination");
    Entity_Data transport = GetEntity (&bestMatch->data, "transport");

    std::string url_to_open = "http://www.google.com/maps/?saddr=" + origin.content + "&daddr=" + destination.content;

    QString string_to_say;
    // TODO: We should make this better - origin.content should
    // have _NULL_ when not found origin. This needs to work:
    // if(origin.content != _NULL_)
    if(origin.content != "to ")
    {
        string_to_say = "Getting directions to '" +
                QString::fromStdString(destination.content) + "' from '" +
                QString::fromStdString(origin.content) + "'.";
    }
    else
    {
        string_to_say = "Getting directions to '" + QString::fromStdString(destination.content) + "'.";
    }
    emit to_say(string_to_say);

    OpenUrl(url_to_open);

    emit finished_executing_skill();
}

/**
* @brief
* Do a Lucky Search -> search using a search-engine and goes to the first result.
* We have the option to use both Google and DuckDuckGo engines.
* http://www.google.com/search?q=enable+javascript+firefox&btnI
* https://duckduckgo.com/?q=!ducky+github+foo+bar+foobar
*/
void Skills::SearchAndShow(Command* bestMatch)
{
    Entity_Data searchTerm = GetEntity (&bestMatch->data, "savedInOpen");

    std::string url_to_open = "http://www.google.com/search?q=" + searchTerm.content + "&btnI";

    QString string_to_say = "Trying to show '" + QString::fromStdString(searchTerm.content) + "'.";
    emit to_say(string_to_say);

    OpenUrl(url_to_open);

    emit finished_executing_skill();
}

/**
* @brief
* Do a Wikipedia search.
*/
void Skills::GoToSpecificWebsite(Command* bestMatch)
{
    for(unsigned int i = 0; i < bestMatch->element_plus_knowledge_datas.size(); i++)
    {
        if(bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->category_name == "websites")
        {
            std::string url_to_open = bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->value.toString().toStdString();
            QString string_to_say = "Showing '" +
                    QString::fromStdString(bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->key) + "'.";

            emit to_say(string_to_say);
            OpenUrl(url_to_open);

            break;
        }
    }

    // TODO?
    // if we implement bestMatch's element_plus_knowledge_datas in QHash:
    // (The only drawback is that the keys are unique so we can't have multiplt knowlesges pushed with the same name
    // eg, more than one [websites] knowledges.
    // ElementPlusKnowledgeData* element_plus_knowledge_data = &bestMatch->element_plus_knowledge_datas["websites"];
    // std::string url_to_open = element_plus_knowledge_data->knowledge_data->value.toString().toStdString();
    // QString string_to_say = "Showing '" + QString::fromStdString(element_plus_knowledge_data->knowledge_data->key) + "'.";
    // emit to_say(string_to_say);
    // OpenUrl(url_to_open);

    emit finished_executing_skill();
}

/**
* @brief
* Tries to open a given app.
*/
void Skills::OpenSpecificApp(Command* bestMatch)
{
    std::string app_to_open;

    for(unsigned int i = 0; i < bestMatch->element_plus_knowledge_datas.size(); i++)
    {
        if(bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->category_name == "applications")
        {
            app_to_open = bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->value.toString().toStdString();
            break;
        }
    }

    QString string_to_say = "Opening '" + QString::fromStdString(app_to_open) + "'.";

#ifdef __APPLE__
    if(app_to_open == "finder")
    {
        QString path = QDir::homePath();
        // TODO: we don't want to open a certain path, we just want to show the app
        // as if someone clicked on Finder. Sometimes you see the recent folder you'd been in.
        QDesktopServices::openUrl(QUrl("file://"+path));
    }
    else
    {
        bool is_succeded_finding_app = OpenAppMac(app_to_open);

        if(!is_succeded_finding_app)
        {
            string_to_say = "It doesn't look like you have an app named '" + QString::fromStdString(app_to_open) + "'.";
            //string_to_say = "I don't see an app named '" + QString::fromStdString(app_to_open) + "'.";
        }
    }
#endif

    emit to_say(string_to_say);
    emit finished_executing_skill();
}

/**
* @brief
* Tries to open a given app.
*/
void Skills::TryToOpenSpecificApp(Command* bestMatch)
{
    Entity_Data app_to_open = GetEntity (&bestMatch->data, "appToOpen");

    QString string_to_say = "Opening '" + QString::fromStdString(app_to_open.content) + "'.";

#ifdef __APPLE__
    bool is_succeded_finding_app = OpenAppMac(app_to_open.content);

    if(!is_succeded_finding_app)
    {
        string_to_say = "It doesn't look like you have an app named '" + QString::fromStdString(app_to_open.content) + "'.";
        //string_to_say = "I don't see an app named '" + QString::fromStdString(app_to_open) + "'.";
    }
#endif

    emit to_say(string_to_say);
    emit finished_executing_skill();
}

/**
* @brief
* Shows the desktop.
*/
void Skills::ShowDesktop(Command* bestMatch)
{
    QString string_to_say = "Showing desktop.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"F11"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Shows the current window in a full-screen mode.
*/
void Skills::ShowWindowFullScreen(Command* bestMatch)
{
    QString string_to_say = "Showing window in full screen...";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "control", "f"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Minimize the current window.
*/
void Skills::MinimizeWindow(Command* bestMatch)
{
    QString string_to_say = "Minimizing window...";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "m"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Hides the current window.
*/
void Skills::HideWindow(Command* bestMatch)
{
    QString string_to_say = "Hiding window...";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "h"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Starts dictation.
*/
void Skills::StartDictation(Command* bestMatch)
{
    QString string_to_say = "Starting dictation...";
    emit to_say(string_to_say);

#ifdef __APPLE__
    // Twice Fn
    std::vector<std::string> keyboard_shortcut = {"FN"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Shows all visible and open windows.
*/
void Skills::ShowAllWindows(Command* bestMatch)
{
    QString string_to_say = "Showing all visible windows...";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"control", "UP"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Focus only on the current window - hides all other windows.
*/
void Skills::ShowOnlyCurrentWindow(Command* bestMatch)
{
    QString string_to_say = "Focusing on current window...";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"option", "command", "h"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Taking screenshot of screen.
* TODO: Make it so only on the first time this command is ever fired
* it will also say "Your screenshot is saved on your desktop."
*/
void Skills::TakeScreenScreenshot(Command* bestMatch)
{
    QString string_to_say = "Taking screenshot of screen...";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"shift", "command", "3"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Taking a selection screenshot of screen.
*/
void Skills::TakeSelectionScreenshot(Command* bestMatch)
{
    QString string_to_say = "Select area for screenshot...";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"shift", "command", "4"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Taking a window screenshot.
*/
void Skills::TakeWindowScreenshot(Command* bestMatch)
{
    QString string_to_say = "Choose window to capture...";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"shift", "command", "4"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);

    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    std::vector<std::string> keyboard_shortcut_2 = {"SPACE"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut_2);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Showing a file in quick-look way in Finder.
*/
void Skills::QuickLook(Command* bestMatch)
{
#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"SPACE"}; // or command+y
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Creating a new folder.
* TODO: Please let's not use keyboard shortcut for this.
*/
void Skills::CreateNewFolder(Command* bestMatch)
{
    Entity_Data folder_name = GetEntity (&bestMatch->data, "folderName");

    if(folder_name.content != _NULL_)
    {
        QString string_to_say = "Creating a new folder named '" + QString::fromStdString(folder_name.content) + "'.";
        emit to_say(string_to_say);
    }
    else
    {
        QString string_to_say = "Creating a new untitled folder.";
        emit to_say(string_to_say);
    }

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "shift", "n"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);

    if(folder_name.content != _NULL_)
    {
        QClipboard *clipboard = QApplication::clipboard();
        QString folder_name_qstring = QString::fromStdString(folder_name.content);
        clipboard->setText(folder_name_qstring);

        std::this_thread::sleep_for (std::chrono::milliseconds(500));
        keyboard_shortcut = {"command", "v"};
        ExectueKeyboardShortcutMac(&keyboard_shortcut);
    }
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Search inside the computer.
*/
void Skills::SearchComputer(Command* bestMatch)
{
    Entity_Data search_term = GetEntity (&bestMatch->data, "searchTermComputer");

    QString string_to_say = "Searching for '" + QString::fromStdString(search_term.content) + "' on your computer.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "SPACE"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);

    QClipboard *clipboard = QApplication::clipboard();
    QString search_term_qstring = QString::fromStdString(search_term.content);
    clipboard->setText(search_term_qstring);

    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    keyboard_shortcut = {"command", "v"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Puts the window to the right of the screen.
* TODO: Currently, on Mac, we use Magnet app keyboard shortcuts
* to achieve this. We shall male our own functions, or partner with
* Magnet for their code.
*/
void Skills::PutWindowToRight(Command* bestMatch)
{
    emit to_say("This command isn't supported yet.");

#ifdef __APPLE__
    //QString string_to_say = "Putting window to the right side.";
    //emit to_say(string_to_say);
    //std::vector<std::string> keyboard_shortcut = {"control", "option","RIGHT"};
    //ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Puts the window to the left of the screen.
* TODO: Currently, on Mac, we use Magnet app keyboard shortcuts
* to achieve this. We shall male our own functions, or partner with
* Magnet for their code.
*/
void Skills::PutWindowToLeft(Command* bestMatch)
{
    emit to_say("This command isn't supported yet.");

#ifdef __APPLE__
    //QString string_to_say = "Putting window to the left side.";
    //emit to_say(string_to_say);
    //std::vector<std::string> keyboard_shortcut = {"control", "option","LEFT"};
    //ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Puts the window in the center of the screen.
* TODO: Currently, on Mac, we use Magnet app keyboard shortcuts
* to achieve this. We shall male our own functions, or partner with
* Magnet for their code.
*/
void Skills::PutWindowToCenter(Command* bestMatch)
{
    emit to_say("This command isn't supported yet.");

#ifdef __APPLE__
    //QString string_to_say = "Putting window on the center.";
    //emit to_say(string_to_say);
    //std::vector<std::string> keyboard_shortcut = {"control", "option","c"};
    //ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Shows the text/chat window.
*/
void Skills::ShowTextWindow(Command* bestMatch)
{
    QString string_to_say = "Ok.";
    emit to_say(string_to_say);

    emit show_text_window();

    emit finished_executing_skill();
}

/**
* @brief
* Hides the text/chat window.
*/
void Skills::HideTextWindow(Command* bestMatch)
{
    QString string_to_say = "Done.";
    emit to_say(string_to_say);

    emit hide_text_window();

    emit finished_executing_skill();
}

/**
* @brief
* Trigger the "stop listening" action.
*/
void Skills::StopListening(Command* bestMatch)
{
    emit stop_listening_action();

    emit finished_executing_skill();
}

/**
* @brief
* Disable chit-chat abilities (remain only with commands).
*/
void Skills::DisableConversation(Command* bestMatch)
{
    QString string_to_say = "Conversation disabled.";
    emit to_say(string_to_say);

    emit disable_conversation();

    emit finished_executing_skill();
}

/**
* @brief
* Enable chit-chat abilities (on top of commands).
*/
void Skills::EnableConversation(Command* bestMatch)
{
    QString string_to_say = "Conversation enabled.";
    emit to_say(string_to_say);

    emit enable_conversation();

    emit finished_executing_skill();
}

/**
* @brief
* Seach and find for a certain keyword on a page/document.
*/
void Skills::FindOnPage(Command* bestMatch)
{
    Entity_Data search_term = GetEntity (&bestMatch->data, "searchTerm");

    QString string_to_say = "Searching for '" + QString::fromStdString(search_term.content) + "' on this page.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "f"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);

    QClipboard *clipboard = QApplication::clipboard();
    QString search_term_qstring = QString::fromStdString(search_term.content);
    clipboard->setText(search_term_qstring);

    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    keyboard_shortcut = {"command", "v"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Copy path of current folder.
*/
void Skills::CopyPathOfFolder(Command* bestMatch)
{
    QString string_to_say = "Copied.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "option", "c"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Refresh current page.
*/
void Skills::RefreshPage(Command* bestMatch)
{

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "r"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Arrange folder by name.
*/
void Skills::ArrangeFolderByName(Command* bestMatch)
{
    QString string_to_say = "Arranging folder by name.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "control", "1"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Arrange folder by kind.
*/
void Skills::ArrangeFolderByKind(Command* bestMatch)
{
    QString string_to_say = "Arranging folder by kind.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "control", "2"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Arrange folder by date last opened.
*/
void Skills::ArrangeFolderByDateLastOpened(Command* bestMatch)
{
    QString string_to_say = "Arranging folder by date last opened.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "control", "3"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Arrange folder by date added.
*/
void Skills::ArrangeFolderByDateAdded(Command* bestMatch)
{
    QString string_to_say = "Arranging folder by date added.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "control", "4"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Arrange folder by date modified.
*/
void Skills::ArrangeFolderByDateModified(Command* bestMatch)
{
    QString string_to_say = "Arranging folder by date modified.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "control", "5"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Arrange folder by size.
*/
void Skills::ArrangeFolderBySize(Command* bestMatch)
{
    QString string_to_say = "Arranging folder by size.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "control", "6"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Arrange folder by tags.
*/
void Skills::ArrangeFolderByTags(Command* bestMatch)
{
    QString string_to_say = "Arranging folder by tags.";
    emit to_say(string_to_say);

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "control", "7"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Quits a certain app by name.
*/
void Skills::QuitCertainApp(Command* bestMatch)
{
    Entity_Data app_to_quit = GetEntity (&bestMatch->data, "appToQuit");

#ifdef __APPLE__
    QString app_to_quit_qstring = QString::fromStdString(app_to_quit.content);
    if(app_to_quit_qstring.simplified().isEmpty())
    {
        QString string_to_say = "Please specify what app you want to quit in your command.";
        emit to_say(string_to_say);
    }
    else
    {
        QuitAppHelper(app_to_quit.content);
        QString string_to_say = "Quitting '" + QString::fromStdString(app_to_quit.content) + " .";
        emit to_say(string_to_say);
    }
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Zooms in.
*/
void Skills::ZoomIn(Command* bestMatch)
{

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "option", "+"};
    for(int i = 0; i < 10; i++)
    {
        ExectueKeyboardShortcutMac(&keyboard_shortcut);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Zooms out.
*/
void Skills::ZoomOut(Command* bestMatch)
{

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "option", "-"};
    for(int i = 0; i < 5; i++)
    {
        ExectueKeyboardShortcutMac(&keyboard_shortcut);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Reset zoom.
*/
void Skills::ZoomReset(Command* bestMatch)
{

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"command", "option", "8"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Remind the user something on a given date.
*/
void Skills::RemindWithDate(Command* bestMatch)
{
    /*Entity_Data thing_to_remind = GetEntity(&bestMatch->data, "thingToRemindSaved");
    Entity_Data date_to_remind = GetEntity(&bestMatch->data, "dateToRemind");

    QString string_to_say = "OK, I'll remind you to " +
        QString::fromStdString(thing_to_remind.content) + " at " +
        QString::fromStdString(date_to_remind.content);
    emit to_say(string_to_say);

    emit finished_executing_skill();*/
}

/**
* @brief
* Remind the user something, asks for date to be reminded.
*/
void Skills::RemindWithNoDate(Command* bestMatch)
{
    /*Entity_Data thing_to_remind = GetEntity(&bestMatch->data, "thingToRemind");

    if(thing_to_remind.content == "")
    {
        QString string_to_say = "OK, just tell me what you want to be reminded about.";
        emit to_say(string_to_say);
        emit set_follow_up_command(&Skills::FollowUpThingToRemind);
    }
    else
    {
        QString string_to_say = "OK, when do you want me remind you?";
        emit to_say(string_to_say);
    }

    emit finished_executing_skill();*/
}

/**
* @brief
* Follow up on response from user on what he wants to be reminded about.
*/
void Skills::FollowUpThingToRemind(Command* bestMatch)
{
    /*QString string_to_say = "OK, just tell me what you want to be reminded about, second time man.";
    emit to_say(string_to_say);

    emit finished_executing_skill();*/
}

/**
* @brief
* Tell the user the current time.
*/
void Skills::WhatTimeIsIt(Command* bestMatch)
{
    QDateTime current_time = QDateTime::currentDateTime();
    QString current_time_string = current_time.toString("h:mm AP");

    QString string_to_say = "It's " + current_time_string + ".";
    emit to_say(string_to_say);

    emit finished_executing_skill();
}

/**
* @brief
* Start a timer for a given time.
*/
void Skills::StartTimer(Command* bestMatch)
{
    Entity_Data timer_duration = GetEntity(&bestMatch->data, "timerDuration");
    int total_duration = GetTimerDurationFromUserInput(timer_duration.content);

    if(total_duration == -1)
    {
        emit to_say("There was an error parsing your request. Make sure you're giving valid numbers.");
        return;
    }

    StartNewTimer(total_duration);

    QString string_to_say = "Ok, Timer " +
            QString::number(m_timers.size()) + " is set for " +
            QString::fromStdString(timer_duration.content) + ".";
    emit to_say(string_to_say);

    emit finished_executing_skill();
}

/**
* @brief
* Start a timer for a given time.
*/
void Skills::StartTimerSaved(Command* bestMatch)
{
    Entity_Data timer_duration = GetEntity(&bestMatch->data, "timerDurationSaved");
    int total_duration = GetTimerDurationFromUserInput(timer_duration.content);

    if(total_duration == -1)
    {
        emit to_say("There was an error parsing your request. Make sure you're giving valid numbers.");
        return;
    }

    StartNewTimer(total_duration);

    QString string_to_say = "Ok, Timer " +
            QString::number(m_timers.size()) + " is set for " +
            QString::fromStdString(timer_duration.content) + ".";
    emit to_say(string_to_say);

    emit finished_executing_skill();
}

/**
* @brief
* Shows the user what commands are supported.
*/
void Skills::WhatCommandsCanISay(Command* bestMatch)
{
    QString string_to_say = "Showing all supported commands. There are going to be many more to come.";
    emit to_say(string_to_say);

    std::string url_to_open = "https://www.deus-ai.com";
    OpenUrl(url_to_open);

    emit finished_executing_skill();
}

/**
* @brief
* Copying and pasting the object the mouse is pointing to into the desired app.
* Not working currently.
*/
void Skills::CopyAndPateThisInto(Command* bestMatch)
{
    Entity_Data app_to_paste_into = GetEntity(&bestMatch->data, "appToPasteInto");

#ifdef __APPLE__
    std::vector<std::string> keyboard_shortcut = {"function", "control", "i"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    keyboard_shortcut = {"command", "c"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    OpenAppMac(app_to_paste_into.content);
    keyboard_shortcut = {"command", "v"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
#endif

    emit finished_executing_skill();
}

/**
* @brief
* Open 'n' or '1' new tabs in a specified web browser.
*/
void Skills::OpenTabsInSpecificWebBrowser(Command* bestMatch)
{
    QString string_to_say;
    std::string browser_to_open;
    for(unsigned int i = 0; i < bestMatch->element_plus_knowledge_datas.size(); i++)
    {
        if(bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->category_name == "web_browsers")
        {
            browser_to_open = bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->value.toString().toStdString();
            break;
        }
    }

    Entity_Data saved_number_of_tabs = GetEntity(&bestMatch->data, "savedInOpen");
    int number_of_tabs = GetNumberFromString(saved_number_of_tabs.content);

#ifdef __APPLE__
    OpenAppMac(browser_to_open);

    if(number_of_tabs != -1)
    {
        std::vector<std::string> keyboard_shortcut = {"command", "t"};
        for(int i = 0; i < number_of_tabs; i++)
        {
            ExectueKeyboardShortcutMac(&keyboard_shortcut);
        }
        string_to_say = "Opening " + QString::number(number_of_tabs) + " new tabs in " + QString::fromStdString(browser_to_open) + ".";
    }
    else
    {
        std::vector<std::string> keyboard_shortcut = {"command", "t"};
        ExectueKeyboardShortcutMac(&keyboard_shortcut);
        string_to_say = "Opening a new tab in " + QString::fromStdString(browser_to_open) + ".";
    }
#endif

    emit to_say(string_to_say);
    emit finished_executing_skill();
}

/**
* @brief
* Open a new window in a specified web browser.
*/
void Skills::OpenWindowInSpecificWebBrowser(Command* bestMatch)
{
    QString string_to_say;
    std::string browser_to_open;
    for(unsigned int i = 0; i < bestMatch->element_plus_knowledge_datas.size(); i++)
    {
        if(bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->category_name == "web_browsers")
        {
            browser_to_open = bestMatch->element_plus_knowledge_datas.at(i).knowledge_data->value.toString().toStdString();
            break;
        }
    }

#ifdef __APPLE__
    OpenAppMac(browser_to_open);
    std::vector<std::string> keyboard_shortcut = {"command", "n"};
    ExectueKeyboardShortcutMac(&keyboard_shortcut);
    string_to_say = "Opening a new window in " + QString::fromStdString(browser_to_open) + ".";
#endif

    emit to_say(string_to_say);
    emit finished_executing_skill();
}

/**
* @brief
* Open 'n' or '1' new tabs in some web browser.
* Since we the broweser isn't specified here, we have to options:
* 1. To ask the user for clarification on which browser should we choose.
* 2. Use the default browser. But we need a way to detect what it is.
* TODO:
* If we go with 1, we need to implement set_follow_up_command(SkillsFunc); first.
* If we go with 2, we need a way to detect what is the default browser.
*/
void Skills::TryToOpenTabs(Command* bestMatch)
{
    QString string_to_say;
    Entity_Data saved_number_of_tabs = GetEntity(&bestMatch->data, "savedInOpen");
    int number_of_tabs = GetNumberFromString(saved_number_of_tabs.content);

    if(number_of_tabs != -1)
    {
        string_to_say = "Please specify in your command what browser you want to open these tabs in.";

    }
    else
    {
        string_to_say = "Please specify in your command what browser you want to open a tab in.";
    }

    emit to_say(string_to_say);
    emit finished_executing_skill();
}


/**
* @brief
* Open a new window in some web browser.
* Since we the broweser isn't specified here, we have to options:
* 1. To ask the user for clarification on which browser should we choose.
* 2. Use the default browser. But we need a way to detect what it is.
* TODO:
* If we go with 1, we need to implement set_follow_up_command(SkillsFunc); first.
* If we go with 2, we need a way to detect what is the default browser.
*/
void Skills::TryToOpenWindow(Command* bestMatch)
{
    QString string_to_say;
    string_to_say = "Please specify in your command what browser you want to open a window in.";
    emit to_say(string_to_say);
    emit finished_executing_skill();
}

/**
* @brief
* Try to pronounce a given phrase.
*/
void Skills::PronouncePhrase(Command* bestMatch)
{
    Entity_Data phrase_to_pronounce = GetEntity(&bestMatch->data, "phraseToPronounce");
    QString string_to_say = QString::fromStdString(phrase_to_pronounce.content) + ".";
    string_to_say.replace(0, 1, string_to_say[0].toUpper());
    emit to_say(string_to_say);
    emit finished_executing_skill();
}

/**
* @brief
* Try to define a given phrase.
*/
void Skills::DefinePhrase(Command* bestMatch)
{
    Entity_Data phrase_to_define = GetEntity(&bestMatch->data, "phraseToDefine");
    phrase_to_define.content[0] = std::toupper(phrase_to_define.content[0]);

    std::string url_to_open = "https://www.dictionary.com/browse/" + phrase_to_define.content;
    OpenUrl(url_to_open);

    QString string_to_say = "Here's the definition for '" + QString::fromStdString(phrase_to_define.content) + "'...";
    emit to_say(string_to_say);
    emit finished_executing_skill();
}

/**
* @brief
* Quits the current (focused) app.
* TODO: Automatically detect the current active app.
*/
void Skills::QuitCurrentApp(Command* bestMatch)
{
    //QString string_to_say = "Quitting current focused app.";
    QString string_to_say = "Please specify in your command what app you want to quit. In the future, I will automatically detect the current app.";
    emit to_say(string_to_say);

#ifdef __APPLE__

#endif

    emit finished_executing_skill();
}

/**
* @brief
* Give information about the current weather.
*/
void Skills::WhatIsTheWeather(Command* bestMatch)
{
    std::string url_to_open = "http://www.google.com/search?q=weather";
    QString string_to_say = "Here's the forcast for today...";
    // QString string_to_say = "Here's the weather today...";

    OpenUrl(url_to_open);

    emit to_say(string_to_say);
    emit finished_executing_skill();
}
