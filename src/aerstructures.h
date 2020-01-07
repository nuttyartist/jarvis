#ifndef AERSTRUCTURES_H
#define AERSTRUCTURES_H

#include <string>
#include <vector>
#include <QJsonObject>
#include <QVariant>

#include <boost/property_tree/ptree.hpp>

#define BLANK_SPACE " "
#define _NULL_ "NULL"
#define NAME_ELEMENT "name"
#define ACTION_ELEMENT "action"
#define FUNCTION_ELEMENT "function"
#define KEYWORDS_ELEMENT "keywords"
#define SAVE_ELEMENT "save"
#define ENTITY_ELEMENT "entity"
#define COMMAND_NOT_FOUND "command-not-found"

#define COMMANDS_DATABASE_PATH "://commands_database/Commands_Database.xml"
#define KNOWLEDGE_FILES_PATH "://knowledge_files"

#define TIME_RECOGNITION 5
#define GOOGLE_RECOGNITION_TIME_LIMIT 60 // 1 minute

/**
 * @brief The Entity_Data struct
 * Holds information about entities in the user's input
 * So the command could be executed with the right information
 */
struct Entity_Data
{
    std::string element_type; // NAME_ELEMENT, SAVE_ELEMENT, directions, where am i, etc...
    std::string entity_name; // destination, searchTerm, etc...
    std::string content; // New York, San Fransisco, etc...
};

struct KnowledgeFile
{
    QString name;
    QJsonObject object;
};

struct KnowledgeData
{
  std::string category_name;
  std::string key;
  QVariant value;
};

struct ElementPlusKnowledgeData
{
    std::string element_plus_key;
    KnowledgeData* knowledge_data;
};

using boost::property_tree::ptree;
struct CommandNode
{
    ptree* node;
    CommandNode* parent;
    int level;
    bool is_visited; // Why we need this?
    std::string user_input_cut;
    std::vector<Entity_Data> saved_entities_datas;
};

/**
 * @brief The Command struct
 * Contain the command name that should be executed
 * And all the entities information needed to perform that command.
 * The longer num_connection - the more suitable it is for the user input.
 */
struct Command
{
    std::string action_name; // skill/action name - direction, google, youtube, etc...
    std::vector<Entity_Data> data; // All suitable information to perform the command
    std::vector<ElementPlusKnowledgeData> element_plus_knowledge_datas;
    int num_connection; // How long is the path of this command on the tree

    bool operator < (const Command& otherCommand) const
    {
        return (num_connection < otherCommand.num_connection);
    }
};

#endif // AERSTRUCTURES_H
