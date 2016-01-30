/*  LOOT

    A load order optimisation tool for Oblivion, Skyrim, Fallout 3 and
    Fallout: New Vegas.

    Copyright (C) 2012-2016    WrinklyNinja

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

#ifndef __LOOT_GAME_CRC_CACHE__
#define __LOOT_GAME_CRC_CACHE__

#include "../metadata_list.h"
#include "../masterlist.h"
#include "../plugin/plugin.h"

#include <string>
#include <mutex>
#include <unordered_map>

namespace loot {
    class GameCache {
    public:
        GameCache();
        GameCache(const GameCache& cache);

        GameCache& operator=(const GameCache& cache);

        Masterlist& GetMasterlist();
        MetadataList& GetUserlist();

        // Returns false for second bool if no cached condition.
        std::pair<bool, bool> GetCachedCondition(const std::string& condition) const;
        void CacheCondition(const std::string& condition, bool result);

        std::set<Plugin> GetPlugins() const;
        const Plugin& GetPlugin(const std::string& pluginName) const;
        void AddPlugin(const Plugin&& plugin);

        std::vector<Message> GetMessages() const;
        void AppendMessage(const Message& message);

        void SetLoadOrderSorted(bool isLoadOrderSorted);

        void ClearCachedConditions();
        void ClearCachedPlugins();
        void ClearMessages();
    private:
        Masterlist masterlist;
        MetadataList userlist;
        std::unordered_map<std::string, bool> conditionCache;
        std::unordered_map<std::string, Plugin> plugins;
        std::vector<Message> messages;
        bool isLoadOrderSorted;

        mutable std::mutex mutex;
    };
}

#endif
