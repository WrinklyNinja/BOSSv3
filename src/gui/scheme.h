/*  LOOT

A load order optimisation tool for Oblivion, Skyrim, Fallout 3 and
Fallout: New Vegas.

Copyright (C) 2014-2016    WrinklyNinja

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

#ifndef __LOOT_GUI_SCHEME_HANDLER__
#define __LOOT_GUI_SCHEME_HANDLER__

#include <include/cef_base.h>
#include <include/cef_scheme.h>

namespace loot {
    class LootSchemeHandlerFactory : public CefSchemeHandlerFactory {
    public:
        virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                                     CefRefPtr<CefFrame> frame,
                                                     const CefString& scheme_name,
                                                     CefRefPtr<CefRequest> request)
                                                     OVERRIDE;

        IMPLEMENT_REFCOUNTING(LootSchemeHandlerFactory);
    };
}

#endif
