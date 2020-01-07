#ifndef ACTIONANDENTITIESRECOGNIZER_H
#define ACTIONANDENTITIESRECOGNIZER_H

#include "aerstructures.h"

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QHash>

#include <string>
#include <queue>
#include <vector>
#include <stack>
#include <istream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

class ActionAndEntitiesRecognizer : public QObject
{
    Q_OBJECT
public:
    explicit ActionAndEntitiesRecognizer(QObject *parent = nullptr);

    void initialize();
    std::vector<Command> is_user_input_command_or_friendly(std::string);
    void PrintAllPossibilitiesForReachingAnAction();

signals:

public slots:

private:
    // Variables
    boost::property_tree::ptree m_commands_database_tree;
    QHash<QString, std::vector<KnowledgeData>> m_knowledge_files_processed;

    // Functions
    void LoadCommandsDatabaseIntoTreeContainer();
    void LoadAndProcessKnowledgeFiles();
    void CleanString(std::string*);
    std::string CutString (std::string, std::string);
    std::string CleanSpace (std::string);
    std::string GetElement (std::string, boost::property_tree::ptree*);
    std::string MergeElements (std::string, std::string);
    bool IsStringFoundSimple (std::string, std::string);
    bool IsStringMatchFromTheStart (std::string, std::string);
    int NumWords (std::string);
    std::size_t GetPosStart (int, std::string);
    std::size_t GetPosEnd (int, std::string);
    std::string SubTokensInString(int, int, std::string);
    std::size_t FindByCompleteTokens (std::string, std::string);
    bool IsFoundInStringAsCompletePhrase (std::string, std::string);
    void AddOrReplaceActionDataToCommand (std::vector<Entity_Data>*, Entity_Data*);
    void SplitWordsToVector (std::string, std::vector<std::string>*);
    std::string GetKnowledgeFileNameFromElement(std::string*, std::size_t, std::size_t);
    std::vector<KnowledgeData>* GetKnowledgeFileKnowledgeDatas(std::string*);
    void PushAllElementPlusKeysIntoWordsWithKnowledge(std::string*, std::size_t, std::size_t, std::vector<KnowledgeData>*, std::vector<ElementPlusKnowledgeData>*);
    void HandleKnowledgeIfFoundInElement(std::string*, std::vector<ElementPlusKnowledgeData>*);
    void SplitWordsToVectorKnowledge(std::string, std::vector<ElementPlusKnowledgeData>*);
    bool IsFoundNameElementInNode (std::string*, boost::property_tree::ptree*, bool, Command*);
    std::size_t FindElementPositionInString(std::string, std::string);
    std::string GetData (std::string*, std::string);
    std::string GetMainData (boost::property_tree::ptree*, std::string, std::string);
    void HandleSaveElementIfFoundInNode(CommandNode*, std::string*, Command*);
    void HandleActionElementIfFoundInNode(CommandNode*, Command*, std::vector<Command>*, std::string*);
    void AddCurentNodeChildrenToTheStack(CommandNode*, Command*, std::stack<CommandNode>*);
};

#endif // ACTIONANDENTITIESRECOGNIZER_H
