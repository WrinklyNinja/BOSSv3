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

#include "loot_app.h"
#include "loot_handler.h"
#include "scheme.h"

#include "../backend/globals.h"
#include "../backend/helpers/helpers.h"
#include "../backend/helpers/language.h"

#include <include/cef_browser.h>
#include <include/cef_task.h>
#include <include/cef_runnable.h>

#include <boost/format.hpp>
#include <boost/locale.hpp>
#include <boost/log/trivial.hpp>

#include <yaml-cpp/yaml.h>

using namespace std;
using boost::locale::translate;
using boost::format;

namespace fs = boost::filesystem;

namespace loot {
    LootApp::LootApp() {}

    void LootApp::OnBeforeCommandLineProcessing(const CefString& process_type,
                                                CefRefPtr<CefCommandLine> command_line) {
        if (process_type.empty()) {
            // Browser process, OK to modify the command line.

            // Disable spell checking.
            command_line->AppendSwitch("--disable-spell-checking");
        }
    }

    CefRefPtr<CefBrowserProcessHandler> LootApp::GetBrowserProcessHandler() {
        return this;
    }

    CefRefPtr<CefRenderProcessHandler> LootApp::GetRenderProcessHandler() {
        return this;
    }

    void LootApp::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) {
        // Register "loot" as a standard scheme.
        registrar->AddCustomScheme("loot", true, false, false);
    }

    void LootApp::OnContextInitialized() {
        //Make sure this is running in the UI thread.
        assert(CefCurrentlyOn(TID_UI));

        // Information used when creating the native window.
        CefWindowInfo window_info;

#ifdef _WIN32
        // On Windows we need to specify certain flags that will be passed to CreateWindowEx().
        window_info.SetAsPopup(NULL, "LOOT");
#endif

        // Set the handler for browser-level callbacks.
        CefRefPtr<LootHandler> handler(new LootHandler(lootState));

        // Register the custom "loot" scheme handlers.
        CefRegisterSchemeHandlerFactory("loot", "l10n", new LootSchemeHandlerFactory());

        // Specify CEF browser settings here.
        CefBrowserSettings browser_settings;

        // Need to set the global locale for this process so that messages will
        // be translated.
        BOOST_LOG_TRIVIAL(debug) << "Initialising language settings in UI thread.";
        if (lootState.getLanguage().Code() != Language::english) {
            boost::locale::generator gen;
            gen.add_messages_path(g_path_l10n.string());
            gen.add_messages_domain("loot");

            BOOST_LOG_TRIVIAL(debug) << "Selected language: " << lootState.getLanguage().Name();
            locale::global(gen(lootState.getLanguage().Locale() + ".UTF-8"));
            boost::filesystem::path::imbue(locale());
        }

        // Set URL to load. Ignore any command line values.
        std::string url = ToFileURL(g_path_report);

        // Create the first browser window.
        CefBrowserHost::CreateBrowser(window_info, handler.get(), url, browser_settings, NULL);
    }

    void LootApp::OnWebKitInitialized() {
        // Create the renderer-side router for query handling.
        CefMessageRouterConfig config;
        message_router_ = CefMessageRouterRendererSide::Create(config);
    }

    bool LootApp::OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) {
        // Handle IPC messages from the browser process...
        return message_router_->OnProcessMessageReceived(browser, source_process, message);
    }

    void LootApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context) {
        // Register javascript functions.
        message_router_->OnContextCreated(browser, frame, context);
    }
}
