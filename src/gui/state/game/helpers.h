/*  LOOT

    A load order optimisation tool for
    Morrowind, Oblivion, Skyrim, Skyrim Special Edition, Skyrim VR,
    Fallout 3, Fallout: New Vegas, Fallout 4 and Fallout 4 VR.

    Copyright (C) 2018    Oliver Hamlet

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
    <https://www.gnu.org/licenses/>.
    */

#ifndef LOOT_GUI_STATE_GAME_HELPERS
#define LOOT_GUI_STATE_GAME_HELPERS

#include <filesystem>
#include <tuple>
#include <vector>

#include <loot/enum/game_type.h>
#include <loot/metadata/message.h>
#include <loot/metadata/plugin_cleaning_data.h>
#include <loot/vertex.h>

namespace loot {
bool ExecutableExists(const GameType& gameType,
                      const std::filesystem::path& gamePath);

void BackupLoadOrder(const std::vector<std::string>& loadOrder,
                     const std::filesystem::path& backupDirectory);

// Escape any Markdown special characters in the input text.
std::string EscapeMarkdownSpecialChars(std::string text);

// Create a Message, escaping any Markdown special characters in the input text.
Message PlainTextMessage(MessageType type, std::string text);

Message ToMessage(const PluginCleaningData& cleaningData);

std::string DescribeCycle(const std::vector<Vertex>& cycle);

std::vector<Message> CheckForRemovedPlugins(
    const std::vector<std::string> pluginsBefore,
    const std::vector<std::string> pluginsAfter);

std::tuple<std::string, std::string, std::string> SplitRegistryPath(
  const std::string& registryPath);
}

#endif
