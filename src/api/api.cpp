/*  BOSS

    A plugin load order optimiser for games that use the esp/esm plugin system.

    Copyright (C) 2012    WrinklyNinja

    This file is part of BOSS.

    BOSS is free software: you can redistribute
    it and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    BOSS is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BOSS.  If not, see
    <http://www.gnu.org/licenses/>.
*/

#include "api.h"
#include "../backend/globals.h"
#include "../backend/game.h"
#include "../backend/metadata.h"
#include "../backend/parsers.h"
#include "../backend/generators.h"
#include "../backend/error.h"
#include "../backend/streams.h"
#include "../backend/legacy-parser.h"

#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <clocale>
#include <list>
#include <vector>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <boost/filesystem.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

const unsigned int boss_ok                          = boss::error::ok;
const unsigned int boss_error_liblo_error           = boss::error::liblo_error;
const unsigned int boss_error_file_write_fail       = boss::error::path_write_fail;
const unsigned int boss_error_parse_fail            = boss::error::path_read_fail;
const unsigned int boss_error_condition_eval_fail   = boss::error::condition_eval_fail;
const unsigned int boss_error_regex_eval_fail       = boss::error::regex_eval_fail;
const unsigned int boss_error_no_mem                = boss::error::no_mem;
const unsigned int boss_error_invalid_args          = boss::error::invalid_args;
const unsigned int boss_error_no_tag_map            = boss::error::no_tag_map;
const unsigned int boss_error_path_not_found        = boss::error::path_not_found;
const unsigned int boss_error_no_game_detected      = boss::error::no_game_detected;
const unsigned int boss_error_subversion_error      = boss::error::subversion_error;
const unsigned int boss_return_max                  = boss_error_subversion_error;

// The following are the games identifiers used by the API.
const unsigned int boss_game_tes4                   = boss::g_game_tes4;
const unsigned int boss_game_tes5                   = boss::g_game_tes5;
const unsigned int boss_game_fo3                    = boss::g_game_fo3;
const unsigned int boss_game_fonv                   = boss::g_game_fonv;

// BOSS message types.
const unsigned int boss_message_say                 = boss::g_message_say;
const unsigned int boss_message_warn                = boss::g_message_warn;
const unsigned int boss_message_error               = boss::g_message_error;

// BOSS message languages.
BOSS_API extern const unsigned int boss_lang_any        = boss::g_lang_any;
BOSS_API extern const unsigned int boss_lang_english    = boss::g_lang_english;
BOSS_API extern const unsigned int boss_lang_spanish    = boss::g_lang_spanish;
BOSS_API extern const unsigned int boss_lang_russian    = boss::g_lang_russian;


struct _boss_db_int {
    _boss_db_int()
        : extTagMap(NULL),
        extAddedTagIds(NULL),
        extRemovedTagIds(NULL),
        extMessageArray(NULL),
        extMessageArraySize(0) {

        extMessage.type = boss_message_say;
        extMessage.message = NULL;
    }

    ~_boss_db_int() {
        delete [] extAddedTagIds;
        delete [] extRemovedTagIds;
        delete [] extMessage.message;

        if (extTagMap != NULL) {
            for (size_t i=0; i < bashTagMap.size(); i++)
                delete [] extTagMap[i];  //Gotta clear those allocated strings.
            delete [] extTagMap;
        }

        if (extMessageArray != NULL) {
            for (size_t i=0; i < extMessageArraySize; i++)
                delete [] extMessageArray[i].message;  //Gotta clear those allocated strings.
            delete [] extMessageArray;
        }
    }

    boss::Game game;
    std::list<boss::Plugin> metadata, rawMetadata, userMetadata, rawUserMetadata;

    boost::unordered_map<std::string, unsigned int> bashTagMap;

    char ** extTagMap;

    unsigned int * extAddedTagIds;
    unsigned int * extRemovedTagIds;

    boss_message extMessage;
    boss_message * extMessageArray;
    size_t extMessageArraySize;
};

const char * extMessageStr = NULL;

// std::string to null-terminated char string converter.
char * ToNewCString(std::string str) {
    char * p = new char[str.length() + 1];
    return strcpy(p, str.c_str());
}

bool IsDirtyMessage(std::string message) {
    const std::string local[3] = {
        "Contains dirty edits: ",
        "Contiene ediciones sucia: ",
        "\"Грязные\" правки: "
    };

    for (int i=0; i < 3; ++i) {
        if (boost::icontains(message, local[i]))
            return true;
    }

    return false;
}

//////////////////////////////
// Error Handling Functions
//////////////////////////////

// Outputs a string giving the details of the last time an error or
// warning return code was returned by a function. The string exists
// until this function is called again or until CleanUpAPI is called.
BOSS_API unsigned int boss_get_error_message (const char ** const message) {
    if (message == NULL)
        return boss_error_invalid_args;

    *message = extMessageStr;

    return boss_ok;
}

// Frees memory allocated to error string.
BOSS_API void     boss_cleanup () {
    delete [] extMessageStr;
}


//////////////////////////////
// Version Functions
//////////////////////////////

// Returns whether this version of BOSS supports the API from the given
// BOSS version. Abstracts BOSS API stability policy away from clients.
BOSS_API bool boss_is_compatible (const unsigned int versionMajor, const unsigned int versionMinor, const unsigned int versionPatch) {
    return versionMajor == boss::g_version_major && versionMinor == boss::g_version_minor;
}

// Returns the version string for this version of BOSS.
// The string exists until this function is called again or until
// CleanUpAPI is called.
BOSS_API unsigned int boss_get_version (unsigned int * const versionMajor, unsigned int * const versionMinor, unsigned int * const versionPatch) {
    if (versionMajor == NULL || versionMinor == NULL || versionPatch == NULL)
        return boss_error_invalid_args;

    *versionMajor = boss::g_version_major;
    *versionMinor = boss::g_version_minor;
    *versionPatch = boss::g_version_patch;

    return boss_ok;
}


////////////////////////////////////
// Lifecycle Management Functions
////////////////////////////////////

// Explicitly manage database lifetime. Allows clients to free memory when
// they want/need to. clientGame sets the game the DB is for, and dataPath
// is the path to that game's Data folder, and is case-sensitive if the
// underlying filesystem is case-sensitive. This function also checks that
// plugins.txt and loadorder.txt (if they both exist) are in sync. If
// dataPath == NULL then the API will attempt to detect the data path of
// the specified game.
BOSS_API unsigned int boss_create_db (boss_db * const db, const unsigned int clientGame, const char * const gamePath) {
    if (db == NULL || (clientGame != boss_game_tes4 && clientGame != boss_game_tes5 && clientGame != boss_game_fo3 && clientGame != boss_game_fonv))
        return boss_error_invalid_args;

    //Set the locale to get encoding conversions working correctly.
    std::setlocale(LC_CTYPE, "");
    std::locale global_loc = std::locale();
    std::locale loc(global_loc, new boost::filesystem::detail::utf8_codecvt_facet());
    boost::filesystem::path::imbue(loc);

    std::string game_path = "";
    if (gamePath != NULL)
        game_path = gamePath;

    boss::Game game;
    try {
        game = boss::Game(clientGame, game_path);  //This also checks to see if the game is installed if game_path is empty and throws an exception if it is not detected.
    } catch (boss::error& e) {
        extMessageStr = e.what();
        return e.code();
    }

    boss_db retVal;
    try {
        retVal = new _boss_db_int;
    } catch (std::bad_alloc& e) {
        extMessageStr = e.what();
        return boss_error_no_mem;
    }
    retVal->game = game;
    *db = retVal;

    return boss_ok;
}

// Destroys the given DB, freeing any memory allocated as part of its use.
BOSS_API void     boss_destroy_db (boss_db db) {
    delete db;
}


///////////////////////////////////
// Database Loading Functions
///////////////////////////////////

// Loads the masterlist and userlist from the paths specified.
// Can be called multiple times. On error, the database is unchanged.
// Paths are case-sensitive if the underlying filesystem is case-sensitive.
// masterlistPath and userlistPath are files.
BOSS_API unsigned int boss_load_lists (boss_db db, const char * const masterlistPath,
                                    const char * const userlistPath) {
    if (db == NULL || masterlistPath == NULL)
        return boss_error_invalid_args;

    std::list<boss::Plugin> temp;
    std::list<boss::Plugin> userTemp;

    try {
        if (boost::algorithm::iends_with(masterlistPath, ".yaml")) {
            YAML::Node tempNode = YAML::LoadFile(masterlistPath);
            temp = tempNode["plugins"].as< std::list<boss::Plugin> >();
        } else {
            boost::filesystem::path p(masterlistPath);
            std::list<boss::Message> filler;
            Loadv2Masterlist(p, temp, filler);
        }
    } catch (YAML::Exception& e) {
        extMessageStr = e.what();
        return boss_error_parse_fail;
    } catch (boss::error& e) {
        extMessageStr = e.what();
        return boss_error_parse_fail;
    }

    try {
        YAML::Node tempNode = YAML::LoadFile(userlistPath);
        userTemp = tempNode["plugins"].as< std::list<boss::Plugin> >();
    } catch (YAML::Exception& e) {
        extMessageStr = e.what();
        return boss_error_parse_fail;
    }

    //Also free memory.
    db->bashTagMap.clear();
    delete [] db->extAddedTagIds;
    delete [] db->extRemovedTagIds;

    if (db->extTagMap != NULL) {
        for (size_t i=0; i < db->bashTagMap.size(); i++)
            delete [] db->extTagMap[i];  //Gotta clear those allocated strings.
        delete [] db->extTagMap;
    }

    if (db->extMessageArray != NULL) {
        for (size_t i=0; i < db->extMessageArraySize; i++)
            delete [] db->extMessageArray[i].message;  //Gotta clear those allocated strings.
        delete [] db->extMessageArray;
    }

    db->rawMetadata = temp;
    db->metadata = temp;
    db->userMetadata = userTemp;
    db->rawUserMetadata = userTemp;

    return boss_ok;
}

// Evaluates all conditional lines and regex mods the loaded masterlist.
// This exists so that Load() doesn't need to be called whenever the mods
// installed are changed. Evaluation does not take place unless this function
// is called. Repeated calls re-evaluate the masterlist from scratch each time,
// ignoring the results of any previous evaluations. Paths are case-sensitive
// if the underlying filesystem is case-sensitive.
BOSS_API unsigned int boss_eval_lists (boss_db db, const unsigned int language) {

    std::list<boss::Plugin> temp = db->rawMetadata;
    try {
        db->game.RefreshActivePluginsList();
        for (std::list<boss::Plugin>::iterator it=temp.begin(); it != temp.end(); ++it) {
            it->EvalAllConditions(db->game, language);
            if (it->IsRegexPlugin()) {
                boost::regex regex;
                try {
                    regex = boost::regex(it->Name(), boost::regex::extended|boost::regex::icase);
                } catch (boost::regex_error& e) {
                    extMessageStr = e.what();
                    return boss_error_regex_eval_fail;
                }

                for (boost::filesystem::directory_iterator itr(db->game.DataPath()); itr != boost::filesystem::directory_iterator(); ++itr) {
                    const std::string filename = itr->path().filename().string();
                    if (boost::regex_match(filename, regex)) {
                        boss::Plugin p = *it;
                        p.Name(filename);
                        temp.push_back(p);
                    }
                }
                it = temp.erase(it);
                --it;
            }
        }
    } catch (boss::error& e) {
        extMessageStr = e.what();
        return e.code();
    }
    db->metadata = temp;

    temp = db->rawUserMetadata;
    try {
        for (std::list<boss::Plugin>::iterator it=temp.begin(), endIt=temp.end(); it != endIt; ++it) {
            it->EvalAllConditions(db->game, language);
            if (it->IsRegexPlugin()) {
                boost::regex regex;
                try {
                    regex = boost::regex(it->Name(), boost::regex::extended|boost::regex::icase);
                } catch (boost::regex_error& e) {
                    extMessageStr = e.what();
                    return boss_error_invalid_args;
                }

                for (boost::filesystem::directory_iterator itr(db->game.DataPath()); itr != boost::filesystem::directory_iterator(); ++itr) {
                    const std::string filename = itr->path().filename().string();
                    if (boost::regex_match(filename, regex)) {
                        boss::Plugin p = *it;
                        p.Name(filename);
                        temp.push_back(p);
                    }
                }
                it = temp.erase(it);
                --it;
            }
        }
    } catch (boss::error& e) {
        extMessageStr = e.what();
        return e.code();
    }
    db->userMetadata = temp;

    return boss_ok;
}


//////////////////////////
// DB Access Functions
//////////////////////////

// Returns an array of the Bash Tags encounterred when loading the masterlist
// and userlist, and the number of tags in the returned array. The array and
// its contents are static and should not be freed by the client.
BOSS_API unsigned int boss_get_tag_map (boss_db db, char *** const tagMap, size_t * const numTags) {
    if (db == NULL || tagMap == NULL || numTags == NULL)
        return boss_error_invalid_args;

    //Clear existing array allocation.
    if (db->extTagMap != NULL) {
        for (size_t i=0, max=db->bashTagMap.size(); i < max; ++i) {
            delete [] db->extTagMap[i];
        }
        delete [] db->extTagMap;
    }

    //Initialise output.
    *tagMap = NULL;
    *numTags = 0;

    boost::unordered_set<std::string> allTags;

    for (std::list<boss::Plugin>::iterator it=db->metadata.begin(), endIt=db->metadata.end(); it != endIt; ++it) {
        std::set<boss::Tag> tags = it->Tags();
        for (std::set<boss::Tag>::const_iterator jt=tags.begin(), endJt=tags.end(); jt != endJt; ++jt) {
            allTags.insert(jt->Name());
        }
    }
    for (std::list<boss::Plugin>::iterator it=db->userMetadata.begin(), endIt=db->userMetadata.end(); it != endIt; ++it) {
        std::set<boss::Tag> tags = it->Tags();
        for (std::set<boss::Tag>::const_iterator jt=tags.begin(), endJt=tags.end(); jt != endJt; ++jt) {
            allTags.insert(jt->Name());
        }
    }

    if (allTags.empty())
        return boss_ok;

    try {
        db->extTagMap = new char*[allTags.size()];
    } catch (std::bad_alloc& e) {
        extMessageStr = e.what();
        return boss_error_no_mem;
    }

    unsigned int UID = 0;
    try {
        for (boost::unordered_set<std::string>::const_iterator it=allTags.begin(), endIt=allTags.end(); it != endIt; ++it) {
            db->bashTagMap.emplace(*it, UID);
            //Also allocate memory.
            db->extTagMap[UID] = ToNewCString(*it);
            UID++;
        }
    } catch (std::bad_alloc& e) {
        extMessageStr = e.what();
        return boss_error_no_mem;
    }

    *tagMap = db->extTagMap;
    *numTags = allTags.size();

    return boss_ok;
}

// Returns arrays of Bash Tag UIDs for Bash Tags suggested for addition and removal
// by BOSS's masterlist and userlist, and the number of tags in each array.
// The returned arrays are valid until the db is destroyed or until the Load
// function is called.  The arrays should not be freed by the client. modName is
// case-insensitive. If no Tags are found for an array, the array pointer (*tagIds)
// will be NULL. The userlistModified bool is true if the userlist contains Bash Tag
// suggestion message additions.
BOSS_API unsigned int boss_get_plugin_tags (boss_db db, const char * const plugin,
                                            unsigned int ** const tagIds_added,
                                            size_t * const numTags_added,
                                            unsigned int ** const tagIds_removed,
                                            size_t * const numTags_removed,
                                            bool * const userlistModified) {
    if (db == NULL || plugin == NULL || tagIds_added == NULL || numTags_added == NULL || tagIds_removed == NULL || numTags_removed == NULL || userlistModified == NULL)
        return boss_error_invalid_args;


    //Clear existing array allocations.
    delete [] db->extAddedTagIds;
    delete [] db->extRemovedTagIds;

    //Initialise output.
    *tagIds_added = NULL;
    *tagIds_removed = NULL;
    *userlistModified = false;
    *numTags_added = 0;
    *numTags_removed = 0;

    if (db->bashTagMap.empty())
        return boss_error_no_tag_map;

    boost::unordered_set<std::string> tagsAdded, tagsRemoved;
    std::list<boss::Plugin>::iterator pluginIt = std::find(db->metadata.begin(), db->metadata.end(), boss::Plugin(plugin));
    if (pluginIt != db->metadata.end()) {
        std::set<boss::Tag> tags = pluginIt->Tags();
        for (std::set<boss::Tag>::const_iterator it=tags.begin(), endIt=tags.end(); it != endIt; ++it) {
            if (it->IsAddition())
                tagsAdded.insert(it->Name());
            else
                tagsRemoved.insert(it->Name());
        }
    }

    pluginIt = std::find(db->userMetadata.begin(), db->userMetadata.end(), boss::Plugin(plugin));
    if (pluginIt != db->userMetadata.end()) {
        *userlistModified = true;
    }

    std::vector<unsigned int> tagsAddedIDs, tagsRemovedIDs;
    for (boost::unordered_set<std::string>::const_iterator it=tagsAdded.begin(), endIt=tagsAdded.end(); it != endIt; ++it) {
        boost::unordered_map<std::string, unsigned int>::const_iterator mapIter = db->bashTagMap.find(*it);
        if (mapIter != db->bashTagMap.end())
            tagsAddedIDs.push_back(mapIter->second);
    }
    for (boost::unordered_set<std::string>::const_iterator it=tagsRemoved.begin(), endIt=tagsRemoved.end(); it != endIt; ++it) {
        boost::unordered_map<std::string, unsigned int>::const_iterator mapIter = db->bashTagMap.find(*it);
        if (mapIter != db->bashTagMap.end())
            tagsRemovedIDs.push_back(mapIter->second);
    }

    //Allocate memory.
    size_t numAdded = tagsAddedIDs.size();
    size_t numRemoved = tagsRemovedIDs.size();
    try {
        if (numAdded != 0) {
            db->extAddedTagIds = new uint32_t[numAdded];
            for (size_t i=0; i < numAdded; i++)
                db->extAddedTagIds[i] = tagsAddedIDs[i];
        }
        if (numRemoved != 0) {
            db->extRemovedTagIds = new uint32_t[numRemoved];
            for (size_t i=0; i < numRemoved; i++)
                db->extRemovedTagIds[i] = tagsRemovedIDs[i];
        }
    } catch (std::bad_alloc& e) {
        extMessageStr = e.what();
        return boss_error_no_mem;
    }

    //Set outputs.
    *tagIds_added = db->extAddedTagIds;
    *tagIds_removed = db->extRemovedTagIds;
    *numTags_added = numAdded;
    *numTags_removed = numRemoved;

    return boss_ok;
}

// Returns the messages attached to the given plugin. Messages are valid until Load,
// DestroyBossDb or GetPluginMessages are next called. plugin is case-insensitive.
// If no messages are attached, *messages will be NULL and numMessages will equal 0.
BOSS_API unsigned int boss_get_plugin_messages (boss_db db, const char * const plugin,
                                                boss_message ** const messages,
                                                size_t * const numMessages) {
    if (db == NULL || plugin == NULL || messages == NULL || numMessages == NULL)
        return boss_error_invalid_args;

    //Clear existing array allocation.
    if (db->extMessageArray != NULL) {
        for (size_t i=0; i < db->extMessageArraySize; ++i) {
            delete [] db->extMessageArray[i].message;
        }
        delete [] db->extMessageArray;
    }

    //Initialise output.
    *messages = NULL;
    *numMessages = 0;

    std::list<boss::Message> pluginMessages;
    std::list<boss::Plugin>::iterator pluginIt = std::find(db->metadata.begin(), db->metadata.end(), boss::Plugin(plugin));
    if (pluginIt != db->metadata.end()) {
        pluginMessages = pluginIt->Messages();
    }

    pluginIt = std::find(db->userMetadata.begin(), db->userMetadata.end(), boss::Plugin(plugin));
    if (pluginIt != db->userMetadata.end()) {
        std::list<boss::Message> temp = pluginIt->Messages();
        pluginMessages.insert(pluginMessages.end(), temp.begin(), temp.end());
    }

    db->extMessageArraySize = pluginMessages.size();
    try {
        db->extMessageArray = new boss_message[db->extMessageArraySize];
        int i = 0;
        for (std::list<boss::Message>::const_iterator it=pluginMessages.begin(), endIt=pluginMessages.end(); it != endIt; ++it) {
            db->extMessageArray[i].type = it->Type();
            db->extMessageArray[i].message = ToNewCString(it->ChooseContent(boss::g_lang_any).Str());
        }
    } catch (std::bad_alloc& e) {
        extMessageStr = e.what();
        return boss_error_no_mem;
    }

    *messages = db->extMessageArray;
    *numMessages = db->extMessageArraySize;

    return boss_ok;
}

// Outputs the first warning message found for the given plugin that warns about dirty edits, and also whether the plugin should be cleaned or not, or if BOSS doesn't know (ie. no message, in which case *message == NULL). needsCleaning is one of the plugin cleanliness codes above.
BOSS_API unsigned int boss_get_dirty_message (boss_db db, const char * const plugin,
                                              boss_message * const message,
                                              unsigned int * const needsCleaning) {
    if (db == NULL || plugin == NULL || message == NULL || needsCleaning == NULL)
        return boss_error_invalid_args;

    //Clear existing allocation.
    if (db->extMessage.message != NULL) {
        delete [] db->extMessage.message;
    }

    *message = db->extMessage;
    *needsCleaning = boss_needs_cleaning_unknown;

    //Get all messages.
    std::list<boss::Message> pluginMessages;
    std::list<boss::Plugin>::iterator pluginIt = std::find(db->metadata.begin(), db->metadata.end(), boss::Plugin(plugin));
    if (pluginIt != db->metadata.end()) {
        pluginMessages = pluginIt->Messages();
    }

    pluginIt = std::find(db->userMetadata.begin(), db->userMetadata.end(), boss::Plugin(plugin));
    if (pluginIt != db->userMetadata.end()) {
        std::list<boss::Message> temp = pluginIt->Messages();
        pluginMessages.insert(pluginMessages.end(), temp.begin(), temp.end());
    }

    //Now discard any that aren't about dirty edits.
    std::list<boss::Message>::iterator it=pluginMessages.begin();
    while (it != pluginMessages.end()) {
        if (IsDirtyMessage(it->ChooseContent(boss::g_lang_any).Str()))
            it = pluginMessages.erase(it);
    }

    if (!pluginMessages.empty()) {
        boss::Message dirtyMessage = pluginMessages.front();

        try {
            db->extMessage.type = dirtyMessage.Type();
            db->extMessage.message = ToNewCString(dirtyMessage.ChooseContent(boss::g_lang_any).Str());
        } catch (std::bad_alloc& e) {
            extMessageStr = e.what();
            return boss_error_no_mem;
        }

        *message = db->extMessage;
        if (dirtyMessage.Type() == boss_message_say)
            *needsCleaning = boss_needs_cleaning_no;
        else
            *needsCleaning = boss_needs_cleaning_yes;

    }

    return boss_ok;
}


// Writes a minimal masterlist that only contains mods that have Bash Tag suggestions,
// and/or dirty messages, plus the Tag suggestions and/or messages themselves and their
// conditions, in order to create the Wrye Bash taglist. outputFile is the path to use
// for output. If outputFile already exists, it will only be overwritten if overwrite is true.
BOSS_API unsigned int boss_write_minimal_list (boss_db db, const char * const outputFile, const bool overwrite) {
    if (db == NULL || outputFile == NULL)
        return boss_error_invalid_args;

    if (boost::filesystem::exists(outputFile) && !overwrite)
        return boss_error_invalid_args;

    std::list<boss::Plugin> temp = db->metadata;
    for (std::list<boss::Plugin>::iterator it=temp.begin(), endIt=temp.end(); it != endIt; ++it) {
        boss::Plugin p(it->Name());
        p.Tags(it->Tags());

        std::list<boss::Message> messages = it->Messages(), newMessages;
        for (std::list<boss::Message>::iterator messageIter = messages.begin(); messageIter != messages.end(); ++messageIter) {
            if (messageIter->Type() == boss_message_warn) {
                if (IsDirtyMessage(messageIter->ChooseContent(boss::g_lang_any).Str()))
                    newMessages.push_back(*messageIter);
            }
        }
        it->Messages(newMessages);

        *it = p;
    }

    YAML::Emitter yout;
    yout.SetIndent(2);
    yout << YAML::BeginMap
         << YAML::Key << "plugins" << YAML::Value << temp
         << YAML::EndMap;

    boost::filesystem::path p(outputFile);
    boss::ofstream out(p);
    if (out.fail())
        return boss_error_invalid_args;
    out << yout.c_str();
    out.close();

    return boss_ok;
}
