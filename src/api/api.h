/*  LOOT

    A load order optimisation tool for Oblivion, Skyrim, Fallout 3 and
    Fallout: New Vegas.

    Copyright (C) 2013-2014    WrinklyNinja

    This file is part of LOOT.

    LOOT is free software: you can redistribute
    it and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    LOOT is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LOOT.  If not, see
    <http://www.gnu.org/licenses/>.
*/

/**
    @file api.h
    @brief This file contains the API frontend.

    @note The LOOT API is *not* thread safe. Thread safety is a goal, but one that has not yet been achieved. Bear this in mind if using it in a multi-threaded client.

    @section var_sec Variable Types

    The LOOT API uses character strings and integers for information input/output.
      - All strings are null-terminated byte character strings encoded in UTF-8.
      - All codes are unsigned integers at least 16 bits in size.
      - All array sizes are unsigned integers at least 16 bits in size.
      - File paths are case-sensitive if and only if the underlying file system is case-sensitive.

    @section memory_sec Memory Management

    The LOOT API manages the memory of strings and arrays it returns internally, so such strings and arrays should not be deallocated by the client.

    Data returned by a function lasts until a function is called which returns data of the same type (eg. a string is stored until the client calls another function which returns a string, an integer array lasts until another integer array is returned, etc.).

    All allocated memory is freed when loot_destroy_db() is called, except the string allocated by loot_get_error_message(), which must be freed by calling loot_cleanup().
*/

#ifndef __LOOT_API_H__
#define __LOOT_API_H__

#include <stddef.h>

#if defined(_MSC_VER)
//MSVC doesn't support C99, so do the stdbool.h definitions ourselves.
//START OF stdbool.h DEFINITIONS.
#   ifndef __cplusplus
#       define bool  _Bool
#       define true  1
#       define false 0
#   endif
#   define __bool_true_false_are_defined 1
//END OF stdbool.h DEFINITIONS.
#else
#   include <stdbool.h>
#endif

// set up dll import/export decorators
// when compiling the dll on windows, ensure LOOT_EXPORT is defined. clients
// that use this header do not need to define anything to import the symbols
// properly.
#if defined(_WIN32) || defined(_WIN64)
#   ifdef LOOT_STATIC
#       define LOOT_API
#   elif defined LOOT_EXPORT
#       define LOOT_API __declspec(dllexport)
#   else
#       define LOOT_API __declspec(dllimport)
#   endif
#else
#   define LOOT_API
#endif

#ifdef __cplusplus
extern "C"
{
#endif


////////////////////////
// Types
////////////////////////

/**
    @brief A structure that holds all game-specific data used by the LOOT API.
    @details Used to keep each game's data independent. Abstracts the definition of the API's internal state while still providing type safety across the library. Multiple handles can also be made for each game, though it should be kept in mind that the API is not thread-safe.
*/
typedef struct _loot_db_int * loot_db;

/**
    @brief A structure that holds the type of a message and the message string itself.
    @var loot_message::type The type of the message, specified using one of the message type codes given below.
    @var loot_message::message The message string itself.
*/
typedef struct {
    unsigned int type;
    const char * message;
} loot_message;


/*********************//**
    @name Return Codes
    @brief Error codes signify an issue that caused a function to exit prematurely. If a function exits prematurely, a reversal of any changes made during its execution is attempted before it exits.
*************************/
///@{

LOOT_API extern const unsigned int loot_ok;  ///< The function completed successfully.
LOOT_API extern const unsigned int loot_error_liblo_error;  ///< There was an error in performing a load order operation.
LOOT_API extern const unsigned int loot_error_file_write_fail;  ///< A file could not be written to.
LOOT_API extern const unsigned int loot_error_parse_fail;  ///< There was an error parsing the file.
LOOT_API extern const unsigned int loot_error_condition_eval_fail;  ///< There was an error evaluating the conditionals in a metadata file.
LOOT_API extern const unsigned int loot_error_regex_eval_fail;  ///< There was an error evaluating the regular expressions in a metadata file.
LOOT_API extern const unsigned int loot_error_no_mem;  ///< The API was unable to allocate the required memory.
LOOT_API extern const unsigned int loot_error_invalid_args;  ///< Invalid arguments were given for the function.
LOOT_API extern const unsigned int loot_error_no_tag_map;  ///< No Bash Tag map has been generated yet.
LOOT_API extern const unsigned int loot_error_path_not_found;  ///< A file or folder path could not be found.
LOOT_API extern const unsigned int loot_error_no_game_detected;  ///< The given game could not be found.
LOOT_API extern const unsigned int loot_error_windows_error;  ///< An error occurred during a call to the Windows API.
LOOT_API extern const unsigned int loot_error_sorting_error;  ///< An error occurred while sorting plugins.

/**
    @brief Matches the value of the highest-numbered return code.
    @details Provided in case clients wish to incorporate additional return codes in their implementation and desire some method of avoiding value conflicts.
*/
LOOT_API extern const unsigned int loot_return_max;

///@}

/*******************//**
    @name Game Codes
    @brief Used with loot_create_db().
***********************/
///@{

LOOT_API extern const unsigned int loot_game_tes4;  ///< Game code for The Elder Scrolls IV: Oblivion.
LOOT_API extern const unsigned int loot_game_tes5;  ///< Game code for The Elder Scrolls V: Skyrim.
LOOT_API extern const unsigned int loot_game_fo3;  ///< Game code for Fallout 3.
LOOT_API extern const unsigned int loot_game_fonv;  ///< Game code for Fallout: New Vegas.

///@}

/***************************//**
    @name Message Type Codes
    @brief Used with the loot_message structure.
*******************************/
///@{
LOOT_API extern const unsigned int loot_message_say;  ///< Denotes a generic note-type message.
LOOT_API extern const unsigned int loot_message_warn;  ///< Denotes a warning message.
LOOT_API extern const unsigned int loot_message_error;  ///< Denotes an error message.

/**
    @brief Denites a Bash Tag suggestion message.
    @details This type should never be seen client-side as it is only used during conversion between internal Bash Tag and message structures, but it is provided just in case.
*/
LOOT_API extern const unsigned int loot_message_tag;

///@}

/*******************************//**
    @name Message Language Codes
    @brief Used with loot_eval_lists().
***********************************/
///@{
LOOT_API extern const unsigned int loot_lang_any;  ///< Tells the API to select messages of any language.
LOOT_API extern const unsigned int loot_lang_english;  ///< Tells the API to preferentially select English messages.
LOOT_API extern const unsigned int loot_lang_spanish;  ///< Tells the API to preferentially select Spanish messages.
LOOT_API extern const unsigned int loot_lang_russian;  ///< Tells the API to preferentially select Russian messages.
LOOT_API extern const unsigned int loot_lang_french;  ///< Tells the API to preferentially select French messages.
LOOT_API extern const unsigned int loot_lang_chinese;  ///< Tells the API to preferentially select Chinese messages.
LOOT_API extern const unsigned int loot_lang_polish;  ///< Tells the API to preferentially select Polish messages.
LOOT_API extern const unsigned int loot_lang_brazilian_portuguese;  ///< Tells the API to preferentially select Polish messages.
///@}

/*********************************//**
    @name Plugin Cleanliness Codes
    @brief Used with loot_get_dirty_message().
*************************************/
///@{
LOOT_API extern const unsigned int loot_needs_cleaning_no;  ///< Denotes that the plugin queried does not need cleaning.
LOOT_API extern const unsigned int loot_needs_cleaning_yes;  ///< Denotes that the plugin queried needs cleaning.
LOOT_API extern const unsigned int loot_needs_cleaning_unknown;  ///< Denotes that the API is unable to determine whether or not the plugin queried needs cleaning.

///@}


/*********************************//**
    @name Error Handling Functions
*************************************/
///@{

/**
   @brief Returns the message for the last error or warning encountered.
   @details Outputs a string giving the a message containing the details of the last error or warning encountered by a function. Each time this function is called, the memory for the previous message is freed, so only one error message is available at any one time.
   @param details A pointer to the error details string outputted by the function.
   @returns A return code.
*/
LOOT_API unsigned int loot_get_error_message (const char ** const message);

/**
   @brief Frees the memory allocated to the last error details string.
*/
LOOT_API void     loot_cleanup ();

///@}


/**************************//**
    @name Version Functions
******************************/
///@{

/**
    @brief Checks for API compatibility.
    @details Checks whether the loaded API is compatible with the given version of the API, abstracting API stability policy away from clients. The version numbering used is major.minor.patch.
    @param versionMajor The major version number to check.
    @param versionMinor The minor version number to check.
    @param versionPatch The patch version number to check.
    @returns True if the API versions are compatible, false otherwise.
*/
LOOT_API bool loot_is_compatible (const unsigned int versionMajor, const unsigned int versionMinor, const unsigned int versionPatch);

/**
    @brief Gets the API version.
    @details Outputs the major, minor and patch version numbers for the loaded API. The version numbering used is major.minor.patch.
    @param versionMajor A pointer to the major version number.
    @param versionMinor A pointer to the minor version number.
    @param versionPatch A pointer to the patch version number.
*/
LOOT_API unsigned int loot_get_version (unsigned int * const versionMajor, unsigned int * const versionMinor, unsigned int * const versionPatch);


/***************************************//**
    @name Lifecycle Management Functions
*******************************************/
///@{

/**
    @brief Initialise a new database handle.
    @details Creates a handle for a database, which is then used by all database functions.
    @param db A pointer to the handle that is created by the function.
    @param gameId A game code specifying which game to create the handle for.
    @param gamePath The relative or absolute path to the game folder, ot `NULL`. If `NULL`, the API will attempt to detect the data path of the specified game.
    @returns A return code.
*/
LOOT_API unsigned int loot_create_db (loot_db * const db, const unsigned int clientGame, const char * const gamePath);

/**
    @brief Destroy an existing database handle.
    @details Destroys the given database handle, freeing up memory allocated during its use, excluding any memory allocated to error messages.
    @param db The database handle to destroy.
*/
LOOT_API void     loot_destroy_db (loot_db db);

///@}


/***********************************//**
    @name Database Loading Functions
***************************************/
///@{

/**
    @brief Loads the masterlist and userlist from the paths specified.
    @details Can be called multiple times, each time replacing the previously-loaded data.
    @param db The database the function acts on.
    @param masterlistPath A string containing the relative or absolute path to the masterlist file that should be loaded. The API supports loading both LOOT and BOSS masterlists.
    @param userlistPath A string containing the relative or absolute path to the userlist file that should be loaded, or `NULL`. If `NULL`, no userlist will be loaded. The API only upports loading v3 userlists.
    @returns A return code.
*/
LOOT_API unsigned int loot_load_lists (loot_db db, const char * const masterlistPath,
                                    const char * const userlistPath);

/**
    @brief Evaluates all conditions and regular expression metadata entries.
    @details Repeated calls re-evaluate the metadata from scratch. This function affects the output of all the database access functions.
    @param db The database the function acts on.
    @param language The language code that is used for message language comparisons.
    @returns A return code.
*/
LOOT_API unsigned int loot_eval_lists (loot_db db, const unsigned int language);

///@}


/**********************************//**
    @name Database Access Functions
**************************************/
///@{

/**
    @brief Outputs an array of the Bash Tags that are suggested in the database.
    @details This function must be called prior to calling loot_get_plugin_tags() to ensure that the latter can return the Tags using the correct array indicies.
    @param db The database the function acts on.
    @param tagMap A pointer to the outputted array of Bash Tags. The array functions as a map where the indicies are the keys, allowing the API to use them as UIDs for Bash Tags instead of passing their names as strings every time loot_get_plugin_tags() is called. If no Bash Tags are suggested, this will be `NULL`.
    @param numTags A pointer to the size of the outputted array. If no Bash Tags are suggested, this will be `0`.
    @returns A return code.
*/
LOOT_API unsigned int loot_get_tag_map (loot_db db, char *** const tagMap, size_t * const numTags);

/**
    @brief Outputs the Bash Tags suggested for addition and removal by the database for the given plugin.
    @details loot_get_tag_map() must be called before this to ensure that the Bash Tag UIDs outputted by this function can be matched up to name strings.
    @param db The database the function acts on.
    @param plugin The filename of the plugin to look up Bash Tag suggestions for.
    @param tags_added A pointer to the outputted array of UIDs of the Bash Tags suggested for addition to the specified plugin. `NULL` if no Bash Tag additions are suggested.
    @param numTags_added A pointer to the size of the tags_added array. `0` if `tags_added` is `NULL`.
    @param tags_removed A pointer to the outputted array of UIDs of the Bash Tags suggested for removal from the specified plugin. `NULL` if no Bash Tag removals are suggested.
    @param numTags_removed A pointer to the size of the `tags_removed` array. `0` if `tags_removed` is `null`.
    @param userlistModified `true` if the Bash Tag suggestions were modified by the data in the userlist, `false` otherwise.
    @returns A return code.
*/
LOOT_API unsigned int loot_get_plugin_tags (loot_db db, const char * const plugin,
                                            unsigned int ** const tags_added,
                                            size_t * const numTags_added,
                                            unsigned int ** const tags_removed,
                                            size_t * const numTags_removed,
                                            bool * const userlistModified);

/**
    @brief Outputs the messages associated with the given plugin in the database.
    @param db The database the function acts on.
    @param plugin The filename of the plugin to look up messages for.
    @param messages A pointer to the outputted array of messages associated with the specified plugin, given as loot_message structures. `NULL` if the plugin has no messages associated with it.
    @param numMessages A pointer to the size of the outputted array. If no messages are outputted, this will be `0`.
    @returns A return code.
*/
LOOT_API unsigned int loot_get_plugin_messages (loot_db db, const char * const plugin,
                                                loot_message ** const messages,
                                                size_t * const numMessages);

/**
    @brief Determines the database's knowledge of a plugin's dirtiness.

    @details Outputs whether the plugin should be cleaned or not, or if no data is available.
    @param db The database the function acts on.
    @param plugin The plugin to look up dirty status information for.
    @param needsCleaning A pointer to a plugin cleanliness code.
    @returns A return code.
*/

LOOT_API unsigned int loot_get_dirty_info (loot_db db, const char * const plugin,
                                            unsigned int * const needsCleaning);

/**
    @brief Writes a minimal metadata file that only contains plugins with Bash Tag suggestions and/or dirty info, plus the suggestions and info themselves.
    @param db The database the function acts on.
    @param outputFile The path to which the file shall be written.
    @param overwrite If `false` and `outputFile` already exists, no data will be written. Otherwise, data will be written.
    @returns A return code.
*/
LOOT_API unsigned int loot_write_minimal_list (loot_db db, const char * const outputFile, const bool overwrite);

///@}


#ifdef __cplusplus
}
#endif

#endif
