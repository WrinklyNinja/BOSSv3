/*  LOOT

A load order optimisation tool for
Morrowind, Oblivion, Skyrim, Skyrim Special Edition, Skyrim VR,
Fallout 3, Fallout: New Vegas, Fallout 4 and Fallout 4 VR.

Copyright (C) 2014 WrinklyNinja

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

#ifndef LOOT_GUI_QUERY_GET_GAME_DATA_QUERY
#define LOOT_GUI_QUERY_GET_GAME_DATA_QUERY

#include <boost/locale.hpp>

#include "gui/cef/query/types/metadata_query.h"
#include "gui/state/game/game.h"
#include "loot/loot_version.h"

namespace loot {
template<typename G = gui::Game>
class GetGameDataQuery : public MetadataQuery<G> {
public:
  GetGameDataQuery(G& game,
                   std::string language,
                   std::function<void(std::string)> sendProgressUpdate) :
      MetadataQuery<G>(game, language),
      sendProgressUpdate_(sendProgressUpdate) {}

  std::string executeLogic() {
    sendProgressUpdate_(boost::locale::translate(
        "Parsing, merging and evaluating metadata..."));

    /* If the game's plugins object is empty, this is the first time loading
       the game data, so also load the metadata lists. */
    bool isFirstLoad = this->getGame().GetPlugins().empty();

    this->getGame().LoadAllInstalledPlugins(true);

    if (isFirstLoad)
      this->getGame().LoadMetadata();

    // Sort plugins into their load order.
    std::vector<std::shared_ptr<const PluginInterface>> installed;
    std::vector<std::string> loadOrder = this->getGame().GetLoadOrder();
    for (const auto& pluginName : loadOrder) {
      const auto plugin = this->getGame().GetPlugin(pluginName);
      if (plugin) {
        installed.push_back(plugin);
      }
    }

    return this->generateJsonResponse(installed.cbegin(), installed.cend());
  }

private:
  std::function<void(std::string)> sendProgressUpdate_;
};
}

#endif
