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

#ifndef LOOT_GUI_QUERY_SAVE_FILTER_STATE_QUERY
#define LOOT_GUI_QUERY_SAVE_FILTER_STATE_QUERY

#include "gui/cef/query/query.h"
#include "gui/state/loot_settings.h"

namespace loot {
class SaveFilterStateQuery : public Query {
public:
  SaveFilterStateQuery(LootSettings& settings,
                       const std::string& filterId,
                       bool enabled) :
      settings_(settings),
      filterId_(filterId),
      enabled_(enabled) {}

  std::string executeLogic() {
    auto logger = getLogger();
    if (logger) {
      logger->trace("Saving filter states.");
    }
    settings_.storeFilterState(filterId_, enabled_);
    return "";
  }

private:
  LootSettings& settings_;
  const std::string filterId_;
  const bool enabled_;
};
}

#endif
