#include "actionandentitiesrecognizer.h"

ActionAndEntitiesRecognizer::ActionAndEntitiesRecognizer(QObject *parent) : QObject(parent)
{

}

/**
* @brief
* Load the commands database file (xml) into a tree data structure (boost::property_tree::ptree)
*/
void ActionAndEntitiesRecognizer::LoadCommandsDatabaseIntoTreeContainer()
{
    // TODO: Is it ok that we're loading our xml file using stream?
    std::istringstream ss;
    QFile commands_database_file(QString::fromStdString(COMMANDS_DATABASE_PATH));
    if (commands_database_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ss.str(QString(commands_database_file.readAll()).toStdString());
        boost::property_tree::read_xml(ss, this->m_commands_database_tree, boost::property_tree::xml_parser::trim_whitespace);;

    }
}

/**
* @brief
* Load all the external knowledge files and process them.
* Each knowledge file is a JSON file with a key/value pair.
* They key represent something specific a user might say like a
* particular website or food. So if the user says something like
* "I would like to have a good sushi.", We could easily parse it using
* [NAME]want, would like[NAME] [NAME][foods][NAME]
* Where [foods] is an external knowelge
* Each key can include multiple valid elements, eg: "facebook, fb",
* Which will be parsed/added as different keys with the same value.
* A value is some a additional information for a certain key.
* So for the knowledge file [websites] we might have this key/value pairs:
* "facebook, fb": "https://www.facebook.com"
*/
void ActionAndEntitiesRecognizer::LoadAndProcessKnowledgeFiles()
{
    foreach(const QString &knowledge_file_name_temp, QDir(KNOWLEDGE_FILES_PATH).entryList())
    {
        KnowledgeFile knowledge_file;
        QFileInfo knowledge_file_name(knowledge_file_name_temp);
        knowledge_file.name = knowledge_file_name.baseName();
        QString knowledge_file_path = KNOWLEDGE_FILES_PATH + ("/" + knowledge_file_name_temp);
        QFile knowledge_file_real_file(knowledge_file_path);
        knowledge_file_real_file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray buffer = knowledge_file_real_file.readAll();
        QJsonDocument json_doc = QJsonDocument::fromJson(buffer);
        QJsonObject knowledge_file_json_object = json_doc.object();
        knowledge_file.object = knowledge_file_json_object;

        foreach(const QString& key, knowledge_file.object.keys())
        {
            std::vector<std::string> splitted_keys;
            SplitWordsToVector(key.toStdString(), &splitted_keys);

            for(unsigned int i = 0; i < splitted_keys.size(); i++)
            {
                std::string retrieved_key = splitted_keys.at(i);
                KnowledgeData new_knowledge_data;
                new_knowledge_data.category_name = knowledge_file.name.toStdString();
                new_knowledge_data.key = retrieved_key;
                new_knowledge_data.value = knowledge_file.object.value(key).toVariant();
                m_knowledge_files_processed[knowledge_file.name].push_back(new_knowledge_data);
            }
        }
    }
}

/**
* @brief
* Load the commands database file (xml) into a tree data structure (boost::property_tree::ptree)
* And process all the knowledge files so their data will be orginized and ready before manipulations.
*/
void ActionAndEntitiesRecognizer::initialize()
{
    LoadCommandsDatabaseIntoTreeContainer();
    LoadAndProcessKnowledgeFiles();
}

/**
* @brief
* Clean the user input from Upper case chars and symbols
* @param std::string* input | The user input
*/
void ActionAndEntitiesRecognizer::CleanString (std::string *input)
{
    boost::algorithm::to_lower(*input);

    std::vector<std::string> chars_to_remove = {"?", "!", "."};
    for(unsigned int i = 0; i < chars_to_remove.size(); i++)
    {
        boost::algorithm::erase_all(*input, chars_to_remove[i]);
    }
}

/**
* @brief
* Cut The string str from the place value found to the end
* Example: ("i want to search for jdflkg", "for")
* Return: " jdflkg"
* @param std::string str | The string to cut
* @param std::string value | The string to search for in str
*/
std::string ActionAndEntitiesRecognizer::CutString (std::string str, std::string value)
{
    return str.substr(FindByCompleteTokens(str, value)+value.length());
}

/**
* @brief
* Clean if there is a white-space in the start of the string
* Example: (" jdflkg")
* Return: "jdflkg"
* @param std::string value | the string to clean whitespace from
*/
std::string ActionAndEntitiesRecognizer::CleanSpace (std::string value)
{
    if(!value.empty() && value.at(0) == ' ')
    {
        value.erase(0,1);
    }

    return value;
}

/**
* @brief
* Get the data from an element in a tree in a given node path.
* If the path doesn't conain a value, or the path doesn't exist, it returns _NULL_
* @param std::string elementPath | The path of the element to take data from
* @param boost::property_tree::ptree* m_commands_database_tree | The node the element in it.
*/
std::string ActionAndEntitiesRecognizer::GetElement (std::string elementPath, boost::property_tree::ptree* m_commands_database_tree)
{
    return m_commands_database_tree->get(elementPath, _NULL_);
}

/**
* @brief
* Create a path by merging two elements
* @param element1 | First element to merge
* @param element2 | Second element to merge
*/
std::string ActionAndEntitiesRecognizer::MergeElements (std::string element1, std::string element2)
{
    return (element1 + "." + element2);
}

/**
* @brief
* Count and return hoe much word in a given string str
* Example: ("Hello i am your father") -> 5
* @param std::string str | The string to count words in
*/
int ActionAndEntitiesRecognizer::NumWords (std::string str)
{
    int counter = 1;
    std::size_t found = str.find(BLANK_SPACE);

    if(found == std::string::npos)
    {
        return 1;
    }

    while(found != std::string::npos)
    {
        counter++;
        found = str.find(BLANK_SPACE, found+1);
    }

    return counter;
}

/**
* @brief
* Return the start position of a word at place numWord in the string str
* Example: (4, "Hello i am your father") -> 11
* Example: (1, "Hello i am your father") -> 0
* @param int numWord | The number_index of the word ]
* @param std::string str | The string to search in
*/
std::size_t ActionAndEntitiesRecognizer::GetPosStart (int numWord, std::string str)
{
    int counter = 1;
    std::size_t found = str.find(BLANK_SPACE);

    if(numWord == 1)
    {
        return 0;
    }

    while(found != std::string::npos)
    {
        if(numWord == counter+1)
        {
            return found+1;
        }
        counter++;
        found = str.find(BLANK_SPACE, found+1);
    }

    return found-1;
}

/**
* @brief
* Return the end position of a word at place numWord in the string str
* Example: (4, "Hello i am your father") -> 14
* Example: (1, "Hello i am your father") -> 4
* @param int numWord | The number_index of the word ]
* @param std::string str | The string to search in
*/
std::size_t ActionAndEntitiesRecognizer::GetPosEnd (int numWord, std::string str)
{
    int counter = 1;
    std::size_t found = str.find(BLANK_SPACE);

    while(found != std::string::npos)
    {
        if(numWord == counter)
        {
            return found-1;
        }
        counter++;
        found = str.find(BLANK_SPACE, found+1);
    }

    if(found == std::string::npos)
    {
        return str.length()-1;
    }

    return found-1;
}

/**
* @brief
* Make a word/s out of two position
* Example: (2, 3, "Hello i am your father") -> "i am your"
* @param int pos | The place to start
* @param int numWords | The number of words to include in the substring
* @param std::string input | The string to make substring from
*/
std::string ActionAndEntitiesRecognizer::SubTokensInString(int pos, int numWords, std::string input)
{
    std::size_t first = GetPosStart(pos, input);
    std::size_t end = GetPosEnd(pos+numWords-1, input);

    return input.substr(first, end-first+1);
}

/**
* @brief
* Get the position of the complete string str in input
* Example: (""Hello amazing i am your father", "amazing") -> 6
* This function differ from other find function by this examples:
* std::find: (""Hello amazing i am your father", "am") -> 6
* FindByCompleteTokens: (""Hello amazing i am your father", "am") -> 16
* @param std::string input | The string to search in
* @param std::string str | The string to search for
*/
std::size_t ActionAndEntitiesRecognizer::FindByCompleteTokens (std::string input, std::string str)
{
    int strNumWords = NumWords(str);
    int len = NumWords(input) - strNumWords + 1;

    for(int i = 1; i <= len; i++)
    {
        if(SubTokensInString(i, strNumWords, input) == str)
        {
            return GetPosStart(i, input);
        }
    }

    return std::string::npos;
}

/**
* @brief
* If no ACTION element is found in the command (commandEntitiesData), then
* we just push the given action (actionData) to the command.
* If we do find the current command already holds an ACTION element
* in its commandEntitiesData, then we just ovveride it - replace it with
* the given actionData. This is because we probably found an ACTION element
* whose path is deeper then the current ACTION element, thus, is more suitable
* for the user input.
* Make sure that the vector will hold only one name_element data
* @param std::vector<Entity_Data>* commandEntitiesData | The vector to search in
* @param Entity_Data* actionEntityData | The data to replace or add
*/
void ActionAndEntitiesRecognizer::AddOrReplaceActionDataToCommand (std::vector<Entity_Data>* commandEntitiesData, Entity_Data* actionEntityData)
{
    for(unsigned i = 0; i < commandEntitiesData->size(); i++)
    {
        if(commandEntitiesData->at(i).element_type == ACTION_ELEMENT)
        {
            commandEntitiesData->at(i) = *actionEntityData;
            return;
        }
    }

    // not found ACTION element, so pushing this one
    commandEntitiesData->push_back(*actionEntityData);
}

/**
* @brief
* Test whether string str found in string input using simple std::string::find
* The difference between IsStringFoundSimple to IsStringMatchFromTheStart functions by examples:
* ("for the web gg", "the"), ("for the web gg", "for the web"), ("research", "search")
* IsStringFoundSimple: true, true, true
* IsStringMatchFromTheStart: false, true, false
* @param std::string input | The user input that might have been changed
* @param std::string str |The string to search for
*/
bool ActionAndEntitiesRecognizer::IsStringFoundSimple (std::string input, std::string str)
{
    std::size_t found = input.find(str);

    if(found == std::string::npos)
    {
        return false;
    }

    return true;
}

/**
* @brief
* Split words that are separated by comma in a string (taken from
* an element in a node) and push each word into a vector structure
* Example: ("for, the web for, about, on", vector)
* Return: vector: ["for", "the web for", "about", "on"]
* @param std::string element | The string to split words into vector
* @param std::vector<std::string>* words | a reference to the vector of the split words
*/
void ActionAndEntitiesRecognizer::SplitWordsToVector (std::string element, std::vector<std::string>* words)
{
    bool isComma = IsStringFoundSimple(element, ",");
    std::string sub_element;

    while(isComma)
    {
        sub_element = element.substr(0, element.find(","));
        element.erase(0, element.find(",")+2);

        words->push_back(sub_element);

        isComma = IsStringFoundSimple(element, ",");
    }

    words->push_back(element);
}

/**
* @brief
* An element might be something like: "open [websites]"
* This function returns the string between the bruckets,
* Which in this example is "websites"
* Example: (&"open [websites]", 5, 9)
* Return: string: "websites"
* @param std::string element | A string represents a specific element in the commands database
* @param std::size_t found_start_brucket | The position found of the start brucket '['
* @param std::size_t bruckets_length | The position found of the end brucket ']' minus position of start brucket '['
*/
std::string ActionAndEntitiesRecognizer::GetKnowledgeFileNameFromElement(std::string* element, std::size_t found_start_brucket, std::size_t bruckets_length)
{
    std::string knowledge_file_name = element->substr(found_start_brucket+1, bruckets_length-1);

    return knowledge_file_name;
}

/**
* @brief
* Returns a pointer to the processed datas of a specific
* Knowledge file 'knowledge_file_name'.
* @param std::string* knowledge_file_name | The name of the requested knowledge file
*/
std::vector<KnowledgeData>* ActionAndEntitiesRecognizer::GetKnowledgeFileKnowledgeDatas(std::string* knowledge_file_name)
{
    QString knowledge_file_name_qstring = QString::fromStdString(*knowledge_file_name);
    std::vector<KnowledgeData>* knowledge_file_processed_datas = &m_knowledge_files_processed[knowledge_file_name_qstring];

    return knowledge_file_processed_datas;
}

/**
* @brief
* Take a specific element from the commands database
* And add to it all the suitable keys inside the knowledge's processed datas.
* So if we have this in our commands database: [NAME]open [websites][NAME]
* We'll create an ElementPlusKnowledgeData object which will hold, for example:
* object.element_plus_key = "open facebook"
* object.knowledge_data = A pointer to the knowledge data of that key. (Category name, key, value).
* We push all the ElementPlusKnowledgeData objects into words_with_knowledge.
* @param std::string element | A string represents a specific element in the commands database
* @param std::size_t found_start_brucket | The position found of the start brucket '['
* @param std::size_t bruckets_length | The position found of the end brucket ']' minus position of start brucket '['
* @param std::vector<KnowledgeData>* knowledge_file_processed_datas | A pointer to the knowledge file's processed datas
* @param std::vector<ElementPlusKnowledgeData>* words_with_knowledge | A pointer to the vector that holds all the ElementPlusKnowledgeData's
*/
void ActionAndEntitiesRecognizer::PushAllElementPlusKeysIntoWordsWithKnowledge(std::string* element, std::size_t found_start_brucket, std::size_t bruckets_length, std::vector<KnowledgeData>* knowledge_file_processed_datas, std::vector<ElementPlusKnowledgeData>* words_with_knowledge)
{
    for(unsigned int i = 0; i < knowledge_file_processed_datas->size(); i++)
    {
        ElementPlusKnowledgeData new_element_plus_knowledge_data;
        std::string temp_value = *element;
        std::string element_plus_key_temp = temp_value.replace(found_start_brucket, bruckets_length+1, knowledge_file_processed_datas->at(i).key);
        new_element_plus_knowledge_data.element_plus_key = element_plus_key_temp; // element before '[' + key + after ']'
        new_element_plus_knowledge_data.knowledge_data = &knowledge_file_processed_datas->at(i);
        words_with_knowledge->push_back(new_element_plus_knowledge_data);
    }
}

/**
* @brief
* If knowledge is found inside the element (knowledge is represented
* by enclosing bruckets []), we add to it all the suitable keys inside
* The knowledge's processed datas.
* So if we have this in our commands database: [NAME]open [websites][NAME]
* We'll create an ElementPlusKnowledgeData object which will hold, for example:
* object.element_plus_key = "open facebook"
* object.knowledge_data = A pointer to the knowledge data of that key. (Category name, key, value).
* We push all the ElementPlusKnowledgeData objects into words_with_knowledge.
* @param std::string element | A string represents a specific element in the commands database
* @param std::vector<ElementPlusKnowledgeData>* words_with_knowledge | A pointer to the vector that holds all the ElementPlusKnowledgeData's
*/
void ActionAndEntitiesRecognizer::HandleKnowledgeIfFoundInElement(std::string* element, std::vector<ElementPlusKnowledgeData>* words_with_knowledge)
{
    std::size_t found_start_brucket = element->find("[");
    std::size_t found_end_brucket = element->find("]");
    if(found_start_brucket != std::string::npos && found_end_brucket != std::string::npos)
    {
        std::size_t bruckets_length = found_end_brucket-found_start_brucket;
        std::string knowledge_file_name = GetKnowledgeFileNameFromElement(element, found_start_brucket, bruckets_length);
        std::vector<KnowledgeData>* knowledge_file_processed_datas = GetKnowledgeFileKnowledgeDatas(&knowledge_file_name);
        PushAllElementPlusKeysIntoWordsWithKnowledge(element, found_start_brucket, bruckets_length, knowledge_file_processed_datas, words_with_knowledge);
    }
}

/**
* @brief
* This function split an element by multiple elements (if exist) by comma.
* If knowledge is found inside an element (knowledge is represented
* by enclosing bruckets []), we add to it all the suitable keys inside
* The knowledge's processed datas.
* So if we have this in our commands database: [NAME]open [websites][NAME]
* We'll create an ElementPlusKnowledgeData object which will hold, for example:
* object.element_plus_key = "open facebook"
* object.knowledge_data = A pointer to the knowledge data of that key. (Category name, key, value).
* We push all the ElementPlusKnowledgeData objects into words_with_knowledge.
* TODO: currently supports only one instance of knowledge,
* eg: can't hanlde: search in [websites] for [something]
* @param std::string element | A string represents a specific element in the commands database
* @param std::vector<ElementPlusKnowledgeData>* words_with_knowledge | A pointer to the vector that holds all the ElementPlusKnowledgeData's
*/
void ActionAndEntitiesRecognizer::SplitWordsToVectorKnowledge(std::string element, std::vector<ElementPlusKnowledgeData>* words_with_knowledge)
{
    bool isComma = IsStringFoundSimple(element, ",");
    std::string sub_element;

    while(isComma)
    {
        sub_element = element.substr(0, element.find(","));
        element.erase(0, element.find(",")+2);

        HandleKnowledgeIfFoundInElement(&sub_element, words_with_knowledge);

        isComma = IsStringFoundSimple(element, ",");
    }

    HandleKnowledgeIfFoundInElement(&element, words_with_knowledge);
}

/**
* @brief
* Return true if string str found *as a word* in string input
* Example: (""Hello amazing i am your father", "amazing") -> true
* This function differ from other find function by this examples:
* IsStringFoundSimple: (""Hello amazing i your father", "am") -> true
* FindByCompleteTokens: (""Hello amazing i your father", "am") -> false
* @param std::string input | The string to search in
* @param std::string str | The string to search for
*/
bool ActionAndEntitiesRecognizer::IsFoundInStringAsCompletePhrase (std::string input, std::string str)
{
    int strNumWords = NumWords(str);
    int len = NumWords(input) - strNumWords + 1;

    for(int i = 1; i <= len; i++)
    {
        if(SubTokensInString(i, strNumWords, input) == str)
        {
            return true;
        }
    }

    return false;
}

/**
* @brief
* Test whether the strings in the name element in m_commands_database_tree
* (might be <"would like, want">, that's why it use the strings splitter into vector)
* is found in the user input. But unlikely what you might think it returns true when
* element is not found to allow continues search for other nodes
* because if the element is _NULL_ it might just be that it doesn't have value - because
* it is a father of a node/nodes that might have value, so we keep on scanning,
* but, if the node does have value - ie, the element is not _NULL_, we will only continue scanning
* (return true) if it has a NAME element that is found in the user input. Else, we return false.
* @param std::string* input | The user input
* @param boost::property_tree::ptree* m_commands_database_tree | The node might contain the name element.
*/
bool ActionAndEntitiesRecognizer::IsFoundNameElementInNode (std::string* input, boost::property_tree::ptree* m_commands_database_tree, bool accept_null, Command* bestMatch)
{
    std::string element = GetElement (NAME_ELEMENT, m_commands_database_tree);
    std::vector<std::string> words_regular;
    std::vector<ElementPlusKnowledgeData> words_with_knowledge;
    SplitWordsToVector (element, &words_regular);
    SplitWordsToVectorKnowledge(element, &words_with_knowledge);

    if(accept_null && element == _NULL_)
    {
        return true;
    }

    for (unsigned i = 0; i < words_with_knowledge.size(); i++)
    {
        if(IsFoundInStringAsCompletePhrase(*input, words_with_knowledge.at(i).element_plus_key))
        {
            if(accept_null)
            {
                bestMatch->element_plus_knowledge_datas.push_back(words_with_knowledge.at(i));
            }
            return true;
        }
    }

    for (unsigned i = 0; i < words_regular.size(); i++)
    {
        if(IsFoundInStringAsCompletePhrase(*input, words_regular[i]))
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief
 * Return the position of an element in a string.
 * An element can have multiple keywords - like: on google, in google, google
 * If we don't find any keyword of the element inside the string
 * we return std::string::npos
 * @param std::string element | The element to search for in string. Can have multiple keywords.
 * @param std::string str_to_search | The string to search in.
 */
std::size_t ActionAndEntitiesRecognizer::FindElementPositionInString(std::string element, std::string str_to_search)
{
    std::vector<std::string> words;
    SplitWordsToVector (element, &words);

    for (unsigned i = 0; i < words.size(); i++)
    {
        // TODO: doesn't deal with question marks -> ?
        std::size_t found = FindByCompleteTokens(str_to_search, words[i]);
        if(found != std::string::npos)
        {
            return found;
        }
    }

    return std::string::npos;
}

/**
* @brief
* Remove *part of* the unnecessary words and return the
* important data for the command function
* @param std::string* input | The user input
* @param std::string str | The word\s to search in the input
*/
std::string ActionAndEntitiesRecognizer::GetData (std::string* input, std::string str)
{
    std::vector<std::string> words;
    SplitWordsToVector (str, &words);
    std::vector<ElementPlusKnowledgeData> words_with_knowledge;
    SplitWordsToVectorKnowledge(str, &words_with_knowledge);

    for (unsigned i = 0; i < words_with_knowledge.size(); i++)
    {
        if(IsFoundInStringAsCompletePhrase(*input, words_with_knowledge.at(i).element_plus_key))
        {
            return CleanSpace (CutString (*input, words_with_knowledge.at(i).element_plus_key));
        }
    }

    for (unsigned i = 0; i < words.size(); i++)
    {
        if(IsFoundInStringAsCompletePhrase(*input, words[i]))
        {
            return CleanSpace (CutString (*input, words[i]));
        }
    }

    return str;
}

/**
* @brief
* Test whether string str *fully found from the start* in string input,
* The difference between IsStringFoundSimple to IsStringMatchFromTheStart functions by examples:
* ("for the web gg", "the"), ("for the web gg", "for the web"), ("research", "search")
* IsStringFoundSimple: true, true, true
* IsStringMatchFromTheStart: false, true, false
* @param std::string input | The user input that might have been changed
* @param std::string str |The string to search for
*/
bool ActionAndEntitiesRecognizer::IsStringMatchFromTheStart (std::string input, std::string str)
{
    int strLen = str.length();

    if(input.empty())
    {
        return false;
    }

    for(int i = 0, k = 0; k < strLen; i++)
    {
        if(str[k] == ' ')
        {
            k++;
            continue;
        }
        if(str[k] != input[i])
        {
            return false;
        }
        k++;
    }

    return true;
}

/**
* @brief
* Remove the unnecessary words that getting from the keywords
* element and return the important data for the command function
* @param boost::property_tree::ptree* m_commands_database_tree | A node contain the keywords element
* @param std::string str | The user input after GetData needs a clean finish
* @param std::string elementType | The name of elementType's keywords path
*/
std::string ActionAndEntitiesRecognizer::GetMainData (boost::property_tree::ptree* m_commands_database_tree, std::string str, std::string elementType)
{
    std::string element = GetElement (MergeElements(elementType, KEYWORDS_ELEMENT), m_commands_database_tree);
    std::vector<std::string> words;
    SplitWordsToVector (element, &words);

    if(element == _NULL_)
    {
        return str;
    }

    for (unsigned i = 0; i < words.size(); i++)
    {
        if(IsStringMatchFromTheStart(str, words[i]))
        {
            return CleanSpace (CutString (str, words[i]));
        }
    }

    return str;
}

/**
* @brief
* Save important keywords from the user input using
* the SAVE element, if exists in the database, in order
* to save important keywords in the middle of a sentencet
* like: "give me [NAME]directions[NAME] [SAVE]from[SAVE] New York [SAVE]to[SAVE] San Fransisco [NAME]by[NAME] transit."
* BTW, this sentence could also be parsed by our database:
* "give me [NAME]directions[NAME] [SAVE]from[SAVE] New York [NAME]to[NAME] San Fransisco"
*/
void ActionAndEntitiesRecognizer::HandleSaveElementIfFoundInNode(CommandNode* currentNode, std::string* user_input, Command* currentCommand)
{
    if(GetElement(SAVE_ELEMENT, currentNode->node) != _NULL_)
    {
        // maybe a function here?
        Entity_Data saveEntityData;
        saveEntityData.element_type = SAVE_ELEMENT;
        saveEntityData.entity_name = GetElement(MergeElements(SAVE_ELEMENT, ENTITY_ELEMENT), currentNode->node);
        saveEntityData.content = GetMainData (currentNode->node, GetData(user_input, GetElement(NAME_ELEMENT, currentNode->node)), SAVE_ELEMENT);

        // Search for a NAME element to decide up to where to cut the string
        // to get the content of the SAVE element
        bool isNameElementFound = false;
        BOOST_FOREACH(ptree::value_type &v, currentNode->node->get_child(""))
        {
            if(IsFoundNameElementInNode(user_input, &v.second, false, currentCommand))
            {
                std::string name_element = GetElement(NAME_ELEMENT, &v.second);
                std::size_t found = FindElementPositionInString(name_element, saveEntityData.content);
                if(found != std::string::npos && found-1 < saveEntityData.content.length())
                {
                    isNameElementFound = true;
                    saveEntityData.content.erase(found-1);
                    break;
                }
            }
        }

        // If no NAME element was found in the rest of the string after the SAVE
        // element, then just return the SAVE element up to a BLANK_SPACE
        if(!isNameElementFound)
        {
            saveEntityData.content.erase(saveEntityData.content.find(BLANK_SPACE)+1);
        }

        // Push the entity data to our currentNode saved_entities_datas
        currentNode->saved_entities_datas.push_back(saveEntityData);
    }
}

/**
* @brief
* If ACTION element is recognized in this node,
* it means we found a valid command for the user input.
* We then push this command into commandsFound.
*/
void ActionAndEntitiesRecognizer::HandleActionElementIfFoundInNode(CommandNode* currentNode, Command* currentCommand, std::vector<Command>* commandsFound, std::string* user_input)
{
    if(GetElement(ACTION_ELEMENT, currentNode->node) != _NULL_)
    {
        Entity_Data actionEntityData;
        actionEntityData.element_type = ACTION_ELEMENT;
        actionEntityData.entity_name = GetElement(MergeElements(ACTION_ELEMENT, ENTITY_ELEMENT), currentNode->node);
        actionEntityData.content = GetMainData (currentNode->node, GetData(user_input, GetElement(NAME_ELEMENT, currentNode->node)), ACTION_ELEMENT);

        currentCommand->num_connection = currentNode->level;
        currentCommand->action_name = GetElement(MergeElements(ACTION_ELEMENT, FUNCTION_ELEMENT), currentNode->node);
        currentCommand->data.reserve(currentCommand->data.size() + currentNode->saved_entities_datas.size());
        currentCommand->data.insert(currentCommand->data.end(), currentNode->saved_entities_datas.begin(), currentNode->saved_entities_datas.end());
        AddOrReplaceActionDataToCommand(&currentCommand->data, &actionEntityData);
        commandsFound->push_back(*currentCommand);
    }
}

/**
* @brief
* Add currentNode children to the stack
* it means we found a valid command for the user input.
* So we can continue search the tree, in DFS manner.
*/
void ActionAndEntitiesRecognizer::AddCurentNodeChildrenToTheStack(CommandNode* currentNode, Command* currentCommand, std::stack<CommandNode>* commandNodesStack)
{
    BOOST_FOREACH(ptree::value_type &v, currentNode->node->get_child(""))
    {
        CommandNode newChildNode;
        newChildNode.node = &v.second;
        newChildNode.parent = currentNode;
        newChildNode.level = currentNode->level + 1;
        newChildNode.is_visited = false;

        if(IsFoundNameElementInNode(&currentNode->user_input_cut, currentNode->node, false, currentCommand))
        {
            newChildNode.user_input_cut = GetData(&currentNode->user_input_cut, GetElement(NAME_ELEMENT, currentNode->node));
        }
        else
        {
            newChildNode.user_input_cut = currentNode->user_input_cut;
        }

        // Saving the parent SAVE entities to all his children
        newChildNode.saved_entities_datas.reserve(newChildNode.saved_entities_datas.size() + currentNode->saved_entities_datas.size());
        newChildNode.saved_entities_datas.insert(newChildNode.saved_entities_datas.end(), currentNode->saved_entities_datas.begin(), currentNode->saved_entities_datas.end());

        commandNodesStack->push(newChildNode);
    }
}

/**
* @brief
* The main NLU function that does all the scanning and manipulations on the user input
* Use a Depth-first search algorithm to scan the tree
* @param std::string user_input | The user input
*/
std::vector<Command> ActionAndEntitiesRecognizer::is_user_input_command_or_friendly(std::string user_input)
{
    CleanString(&user_input);

    using boost::property_tree::ptree;
    std::stack<CommandNode> commandNodesStack;
    CommandNode rootNode;
    rootNode.node = &this->m_commands_database_tree;
    rootNode.parent = NULL;
    rootNode.level = 1;
    rootNode.is_visited = false;
    rootNode.user_input_cut = user_input;
    commandNodesStack.push(rootNode);

    std::vector<Command> commandsFound;
    Command currentCommand;
    currentCommand.num_connection = 0;

    CommandNode currentNode;
    while(!commandNodesStack.empty())
    {
        currentNode = commandNodesStack.top();
        commandNodesStack.pop();

        if(!currentNode.is_visited) // TODO: Why do we need this?
        {
            currentNode.is_visited = true;
            if(IsFoundNameElementInNode(&currentNode.user_input_cut, currentNode.node, true, &currentCommand))
            {
                HandleSaveElementIfFoundInNode(&currentNode, &user_input, &currentCommand);
                HandleActionElementIfFoundInNode(&currentNode, &currentCommand, &commandsFound, &user_input);

                if(!currentNode.node->empty()) // If currentNode has children
                {
                    AddCurentNodeChildrenToTheStack(&currentNode, &currentCommand, &commandNodesStack);
                }
                else // We got a node without children, so we reset currentCommand for next scannings
                {
                    currentCommand = Command();
                    currentCommand.num_connection = 0;
                }
            }
        }
    }

    return commandsFound;
}

/**
* @brief
* TODO
*/
void ActionAndEntitiesRecognizer::PrintAllPossibilitiesForReachingAnAction()
{
    //print_recursive(pt);
}
