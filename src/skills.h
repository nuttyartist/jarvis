#ifndef SKILLS_H
#define SKILLS_H

#include "aerstructures.h"

#include <QObject>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QClipboard>
#include <QApplication>
#include <QDateTime>
#include <QTimer>
#include <QDir>
#include <QDirIterator>

#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <libproc.h>
#include <stdio.h>

#include <boost/algorithm/string.hpp>

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h> /* For kVK_ constants, and TIS functions. */
#endif

class Skills : public QObject
{
    Q_OBJECT
public:
    explicit Skills(QObject *parent = nullptr);
    void initialize();
    void decide_command_to_execute(std::vector<Command>);
    //typedef void (Skills::*SkillsFunc)(Command*);

signals:
    void to_say(QString);
    void finished_executing_skill();
    void show_text_window();
    void hide_text_window();
    void stop_listening_action();
    void disable_conversation();
    void enable_conversation();
    //void set_follow_up_command(SkillsFunc);

public slots:

private:
    // Variables
    typedef void (Skills::*SkillsFunc)(Command*);
    QHash<QString, SkillsFunc> skill_names_and_pointers;
    std::vector<QTimer*> m_timers;

    // Auxiliary functions
    void execute_skill(Command*);
    void print_command_attributes(Command*);
    Entity_Data GetEntity (std::vector<Entity_Data>*, std::string);
    void OpenUrl (std::string);
    bool OpenAppMac(std::string);
    int GetkeyCodeFromStringMac(std::string);
    void ExectueKeyboardShortcutHelperMac(std::vector<CGEventFlags>, CGKeyCode);
    void ExectueKeyboardShortcutMac(std::vector<std::string>*);
    int FindAppId(std::string);
    void QuitAppHelper(std::string);
    int GetTimerDurationFromUserInput(std::string);
    int GetNumberFromString(std::string);
    void StartNewTimer(int);

    // Skills functions
    void GoogleSearch (Command*);
    void YoutubeSearch (Command*);
    void GooglePicturesSearch (Command*);
    void WikipediaSearch (Command*);
    void MapsDirection (Command*);
    void SearchAndShow (Command*);
    void OpenSpecificApp (Command*);
    void ShowDesktop (Command*);
    void ShowWindowFullScreen (Command*);
    void MinimizeWindow (Command*);
    void HideWindow (Command*);
    void StartDictation (Command*);
    void ShowAllWindows (Command*);
    void ShowOnlyCurrentWindow (Command*);
    void TakeScreenScreenshot (Command*);
    void TakeSelectionScreenshot (Command*);
    void TakeWindowScreenshot (Command*);
    void QuickLook (Command*);
    void CreateNewFolder (Command*);
    void SearchComputer (Command*);
    void PutWindowToRight (Command*);
    void PutWindowToLeft (Command*);
    void PutWindowToCenter (Command*);
    void ShowTextWindow (Command*);
    void HideTextWindow (Command*);
    void StopListening(Command*);
    void DisableConversation(Command*);
    void EnableConversation(Command*);
    void FindOnPage(Command*);
    void CopyPathOfFolder(Command*);
    void RefreshPage(Command*);
    void ArrangeFolderByName(Command*);
    void ArrangeFolderByKind(Command*);
    void ArrangeFolderByDateLastOpened(Command*);
    void ArrangeFolderByDateAdded(Command*);
    void ArrangeFolderByDateModified(Command*);
    void ArrangeFolderBySize(Command*);
    void ArrangeFolderByTags(Command*);
    void QuitCertainApp(Command*);
    void ZoomIn(Command*);
    void ZoomOut(Command*);
    void ZoomReset(Command*);
    void RemindWithDate(Command*);
    void RemindWithNoDate(Command*);
    void FollowUpThingToRemind(Command*);
    void WhatTimeIsIt(Command*);
    void StartTimer(Command*);
    void StartTimerSaved(Command*);
    void WhatCommandsCanISay(Command*);
    void CopyAndPateThisInto(Command*);
    void GooglePicturesSearchSaved(Command*);
    void GoToSpecificWebsite(Command*);
    void TryToOpenSpecificApp(Command*);
    void OpenTabsInSpecificWebBrowser(Command*);
    void OpenWindowInSpecificWebBrowser(Command*);
    void TryToOpenTabs(Command*);
    void TryToOpenWindow(Command*);
    void SearchSpecificSearchEngine(Command*);
    void PronouncePhrase(Command*);
    void DefinePhrase(Command*);
    void QuitCurrentApp(Command*);
    void WhatIsTheWeather(Command*);
};

#endif // SKILLS_H
