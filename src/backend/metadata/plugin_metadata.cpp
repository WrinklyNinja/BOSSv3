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

#include "plugin_metadata.h"
#include "../game/game.h"
#include "../helpers/helpers.h"
#include "../error.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <boost/locale.hpp>
#include <regex>

using namespace std;

namespace loot {
    PluginMetadata::PluginMetadata() : enabled(true), _isPriorityExplicit(false), isPriorityGlobal(false), priority(0) {}

    PluginMetadata::PluginMetadata(const std::string& n) : name(n), enabled(true), _isPriorityExplicit(false), isPriorityGlobal(false), priority(0) {
        //If the name passed ends in '.ghost', that should be trimmed.
        if (boost::iends_with(name, ".ghost"))
            name = name.substr(0, name.length() - 6);
    }

    void PluginMetadata::MergeMetadata(const PluginMetadata& plugin) {
        BOOST_LOG_TRIVIAL(trace) << "Merging metadata for: " << name;
        if (plugin.HasNameOnly())
            return;

        //For 'enabled' and 'priority' metadata, use the given plugin's values, but if the 'priority' user value is not explicit, ignore it.
        enabled = plugin.Enabled();
        if (plugin.IsPriorityExplicit()) {
            Priority(plugin.Priority());
            SetPriorityGlobal(plugin.IsPriorityGlobal());
            _isPriorityExplicit = true;
        }

        //Merge the following. If any files in the source already exist in the destination, they will be skipped. Files have display strings and condition strings which aren't considered when comparing them, so will be lost if the plugin being merged in has additional data in these strings.
        std::set<File> files = plugin.LoadAfter();
        loadAfter.insert(files.begin(), files.end());

        files = plugin.Reqs();
        requirements.insert(files.begin(), files.end());

        files = plugin.Incs();
        incompatibilities.insert(files.begin(), files.end());

        //Merge Bash Tags too. Conditions are ignored during comparison, but if a tag is added and removed, both instances will be in the set.
        std::set<Tag> bashTags = plugin.Tags();
        tags.insert(bashTags.begin(), bashTags.end());

        //Messages are in an ordered list, and should be fully merged.
        std::list<Message> pMessages = plugin.Messages();
        messages.insert(messages.end(), pMessages.begin(), pMessages.end());

        set<PluginDirtyInfo> dirtyInfo = plugin.DirtyInfo();
        _dirtyInfo.insert(dirtyInfo.begin(), dirtyInfo.end());

        set<Location> locations = plugin.Locations();
        _locations.insert(locations.begin(), locations.end());

        return;
    }

    PluginMetadata PluginMetadata::DiffMetadata(const PluginMetadata& plugin) const {
        BOOST_LOG_TRIVIAL(trace) << "Calculating metadata difference for: " << name;
        PluginMetadata p(*this);

        if (Priority() == plugin.Priority() && IsPriorityGlobal() == plugin.IsPriorityGlobal()) {
            p.Priority(0);
            p.SetPriorityGlobal(false);
            p.SetPriorityExplicit(false);
        }

        //Compare this plugin against the given plugin.
        set<File> files = plugin.LoadAfter();
        set<File> filesDiff;
        set_symmetric_difference(loadAfter.begin(), loadAfter.end(), files.begin(), files.end(), inserter(filesDiff, filesDiff.begin()));
        p.LoadAfter(filesDiff);

        filesDiff.clear();
        files = plugin.Reqs();
        set_symmetric_difference(requirements.begin(), requirements.end(), files.begin(), files.end(), inserter(filesDiff, filesDiff.begin()));
        p.Reqs(filesDiff);

        filesDiff.clear();
        files = plugin.Incs();
        set_symmetric_difference(incompatibilities.begin(), incompatibilities.end(), files.begin(), files.end(), inserter(filesDiff, filesDiff.begin()));
        p.Incs(filesDiff);

        list<Message> msgs1 = plugin.Messages();
        list<Message> msgs2 = messages;
        msgs1.sort();
        msgs2.sort();
        list<Message> mDiff;
        set_symmetric_difference(msgs2.begin(), msgs2.end(), msgs1.begin(), msgs1.end(), inserter(mDiff, mDiff.begin()));
        p.Messages(mDiff);

        set<Tag> bashTags = plugin.Tags();
        set<Tag> tagDiff;
        set_symmetric_difference(tags.begin(), tags.end(), bashTags.begin(), bashTags.end(), inserter(tagDiff, tagDiff.begin()));
        p.Tags(tagDiff);

        set<PluginDirtyInfo> dirtyInfo = plugin.DirtyInfo();
        set<PluginDirtyInfo> dirtDiff;
        set_symmetric_difference(_dirtyInfo.begin(), _dirtyInfo.end(), dirtyInfo.begin(), dirtyInfo.end(), inserter(dirtDiff, dirtDiff.begin()));
        p.DirtyInfo(dirtDiff);

        set<Location> locations = plugin.Locations();
        set<Location> locationsDiff;
        set_symmetric_difference(_locations.begin(), _locations.end(), locations.begin(), locations.end(), inserter(locationsDiff, locationsDiff.begin()));
        p.Locations(locationsDiff);

        return p;
    }

    PluginMetadata PluginMetadata::NewMetadata(const PluginMetadata& plugin) const {
        BOOST_LOG_TRIVIAL(trace) << "Comparing new metadata for: " << name;
        PluginMetadata p(*this);

        //Compare this plugin against the given plugin.
        set<File> files = plugin.LoadAfter();
        set<File> filesDiff;
        set_difference(loadAfter.begin(), loadAfter.end(), files.begin(), files.end(), inserter(filesDiff, filesDiff.begin()));
        p.LoadAfter(filesDiff);

        filesDiff.clear();
        files = plugin.Reqs();
        set_difference(requirements.begin(), requirements.end(), files.begin(), files.end(), inserter(filesDiff, filesDiff.begin()));
        p.Reqs(filesDiff);

        filesDiff.clear();
        files = plugin.Incs();
        set_difference(incompatibilities.begin(), incompatibilities.end(), files.begin(), files.end(), inserter(filesDiff, filesDiff.begin()));
        p.Incs(filesDiff);

        list<Message> msgs1 = plugin.Messages();
        list<Message> msgs2 = messages;
        msgs1.sort();
        msgs2.sort();
        list<Message> mDiff;
        set_difference(msgs2.begin(), msgs2.end(), msgs1.begin(), msgs1.end(), inserter(mDiff, mDiff.begin()));
        p.Messages(mDiff);

        set<Tag> bashTags = plugin.Tags();
        set<Tag> tagDiff;
        set_difference(tags.begin(), tags.end(), bashTags.begin(), bashTags.end(), inserter(tagDiff, tagDiff.begin()));
        p.Tags(tagDiff);

        set<PluginDirtyInfo> dirtyInfo = plugin.DirtyInfo();
        set<PluginDirtyInfo> dirtDiff;
        set_difference(_dirtyInfo.begin(), _dirtyInfo.end(), dirtyInfo.begin(), dirtyInfo.end(), inserter(dirtDiff, dirtDiff.begin()));
        p.DirtyInfo(dirtDiff);

        set<Location> locations = plugin.Locations();
        set<Location> locationsDiff;
        set_difference(_locations.begin(), _locations.end(), locations.begin(), locations.end(), inserter(locationsDiff, locationsDiff.begin()));
        p.Locations(locationsDiff);

        return p;
    }

    std::string PluginMetadata::Name() const {
        return name;
    }

    bool PluginMetadata::Enabled() const {
        return enabled;
    }

    int PluginMetadata::Priority() const {
        return priority;
    }

    bool PluginMetadata::IsPriorityExplicit() const {
        return priority != 0 || _isPriorityExplicit;
    }

    bool PluginMetadata::IsPriorityGlobal() const {
        return isPriorityGlobal;
    }

    std::set<File> PluginMetadata::LoadAfter() const {
        return loadAfter;
    }

    std::set<File> PluginMetadata::Reqs() const {
        return requirements;
    }

    std::set<File> PluginMetadata::Incs() const {
        return incompatibilities;
    }

    std::list<Message> PluginMetadata::Messages() const {
        return messages;
    }

    std::set<Tag> PluginMetadata::Tags() const {
        return tags;
    }

    std::set<PluginDirtyInfo> PluginMetadata::DirtyInfo() const {
        return _dirtyInfo;
    }

    std::set<Location> PluginMetadata::Locations() const {
        return _locations;
    }

    void PluginMetadata::Enabled(const bool e) {
        enabled = e;
    }

    void PluginMetadata::Priority(const int p) {
        if (abs(p) >= yamlGlobalPriorityDivisor)
            throw error(error::invalid_args, "Cannot set priority that has an absolute value greater than or equal to " + to_string(yamlGlobalPriorityDivisor));

        priority = p;
    }

    void PluginMetadata::SetPriorityExplicit(bool state) {
        _isPriorityExplicit = state;
    }

    void PluginMetadata::SetPriorityGlobal(bool state) {
        isPriorityGlobal = state;
    }

    void PluginMetadata::LoadAfter(const std::set<File>& l) {
        loadAfter = l;
    }

    void PluginMetadata::Reqs(const std::set<File>& r) {
        requirements = r;
    }

    void PluginMetadata::Incs(const std::set<File>& i) {
        incompatibilities = i;
    }

    void PluginMetadata::Messages(const std::list<Message>& m) {
        messages = m;
    }

    void PluginMetadata::Tags(const std::set<Tag>& t) {
        tags = t;
    }

    void PluginMetadata::DirtyInfo(const std::set<PluginDirtyInfo>& dirtyInfo) {
        _dirtyInfo = dirtyInfo;
    }

    void PluginMetadata::Locations(const std::set<Location>& locations) {
        _locations = locations;
    }

    PluginMetadata& PluginMetadata::EvalAllConditions(Game& game, const unsigned int language) {
        for (auto it = loadAfter.begin(); it != loadAfter.end();) {
            if (!it->EvalCondition(game))
                loadAfter.erase(it++);
            else
                ++it;
        }

        for (auto it = requirements.begin(); it != requirements.end();) {
            if (!it->EvalCondition(game))
                requirements.erase(it++);
            else
                ++it;
        }

        for (auto it = incompatibilities.begin(); it != incompatibilities.end();) {
            if (!it->EvalCondition(game))
                incompatibilities.erase(it++);
            else
                ++it;
        }

        for (auto it = messages.begin(); it != messages.end();) {
            if (!it->EvalCondition(game, language))
                it = messages.erase(it);
            else
                ++it;
        }

        for (auto it = tags.begin(); it != tags.end();) {
            if (!it->EvalCondition(game))
                tags.erase(it++);
            else
                ++it;
        }

        if (IsRegexPlugin())  // Remove any dirty metadata from a regex plugin.
            _dirtyInfo.clear();
        else {
            for (auto it = _dirtyInfo.begin(); it != _dirtyInfo.end();) {
                if (!it->EvalCondition(game, name))
                    _dirtyInfo.erase(it++);
                else
                    ++it;
            }
        }

        return *this;
    }

    bool PluginMetadata::HasNameOnly() const {
        return !IsPriorityExplicit() && loadAfter.empty() && requirements.empty() && incompatibilities.empty() && messages.empty() && tags.empty() && _dirtyInfo.empty() && _locations.empty();
    }

    bool PluginMetadata::IsRegexPlugin() const {
        // Treat as regex if the plugin filename contains any of ":\*?|" as
        // they are not valid Windows filename characters, but have meaning
        // in regexes.
        return strpbrk(name.c_str(), ":\\*?|") != nullptr;
    }

    bool PluginMetadata::operator == (const PluginMetadata& rhs) const {
        if (IsRegexPlugin() == rhs.IsRegexPlugin())
            return boost::iequals(name, rhs.Name());

        if (IsRegexPlugin())
            return regex_match(rhs.Name(), regex(name, regex::ECMAScript | regex::icase));
        else
            return regex_match(name, regex(rhs.Name(), regex::ECMAScript | regex::icase));
    }

    bool PluginMetadata::operator != (const PluginMetadata& rhs) const {
        return !(*this == rhs);
    }

    bool PluginMetadata::operator == (const std::string& rhs) const {
        if (IsRegexPlugin())
            return regex_match(PluginMetadata(rhs).Name(), regex(name, regex::ECMAScript | regex::icase));
        else
            return boost::iequals(name, PluginMetadata(rhs).Name());
    }

    bool PluginMetadata::operator != (const std::string& rhs) const {
        return !(*this == rhs);
    }

    int PluginMetadata::GetYamlPriorityValue() const {
        int priorityValue = Priority();
        if (IsPriorityGlobal()) {
            if (priorityValue < 0)
                priorityValue -= loot::yamlGlobalPriorityDivisor;
            else
                priorityValue += loot::yamlGlobalPriorityDivisor;
        }
        return priorityValue;
    }
}

namespace YAML {
    Emitter& operator << (Emitter& out, const loot::PluginMetadata& rhs) {
        if (!rhs.HasNameOnly()) {
            out << BeginMap
                << Key << "name" << Value << YAML::SingleQuoted << rhs.Name();

            if (rhs.IsPriorityExplicit()) {
                out << Key << "priority" << Value << rhs.GetYamlPriorityValue();
            }

            if (!rhs.Enabled())
                out << Key << "enabled" << Value << rhs.Enabled();

            if (!rhs.LoadAfter().empty())
                out << Key << "after" << Value << rhs.LoadAfter();

            if (!rhs.Reqs().empty())
                out << Key << "req" << Value << rhs.Reqs();

            if (!rhs.Incs().empty())
                out << Key << "inc" << Value << rhs.Incs();

            if (!rhs.Messages().empty())
                out << Key << "msg" << Value << rhs.Messages();

            if (!rhs.Tags().empty())
                out << Key << "tag" << Value << rhs.Tags();

            if (!rhs.DirtyInfo().empty())
                out << Key << "dirty" << Value << rhs.DirtyInfo();

            if (!rhs.Locations().empty())
                out << Key << "url" << Value << rhs.Locations();

            out << EndMap;
        }

        return out;
    }
}
