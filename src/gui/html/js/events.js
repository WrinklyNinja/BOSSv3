'use strict';
function onSidebarFilterToggle(evt) {
  if (evt.target.id !== 'contentFilter') {
    loot.filters[evt.target.id] = evt.target.checked;
    loot.query('saveFilterState', evt.target.id, evt.target.checked).catch(handlePromiseError);
  } else {
    loot.filters.contentSearchString = evt.target.value;
  }
  filterPluginData(loot.game.plugins, loot.filters);
}

function onJumpToGeneralInfo() {
  window.location.hash = '';
  document.getElementById('main').scrollTop = 0;
}
function onChangeGame(evt) {
  if (evt.detail.item.getAttribute('value') === loot.game.folder) {
    return;
  }
  /* Send off a CEF query with the folder name of the new game. */
  loot.query('changeGame', evt.detail.item.getAttribute('value')).then((result) => {
    /* Filters should be re-applied on game change, except the conflicts
       filter. Don't need to deactivate the others beforehand. Strictly not
       deactivating the conflicts filter either, just resetting it's value.
       */
    loot.filters.conflictTargetPluginName = undefined;

    /* Clear the UI of all existing game-specific data. Also
       clear the card and li variables for each plugin object. */
    const globalMessages = document.getElementById('summary').getElementsByTagName('ul')[0];
    while (globalMessages.firstElementChild) {
      globalMessages.removeChild(globalMessages.firstElementChild);
    }

    /* Parse the data sent from C++. */
    const gameInfo = JSON.parse(result, loot.Plugin.fromJson);
    loot.game = new loot.Game(gameInfo, loot.l10n);

    /* Reset virtual list positions. */
    document.getElementById('cardsNav').scrollToIndex(0);
    document.getElementById('pluginCardList').scrollToIndex(0);

    /* Now update virtual lists. */
    filterPluginData(loot.game.plugins, loot.filters);

    loot.Dialog.closeProgress();
  }).catch(handlePromiseError);
}
/* Masterlist update process, minus progress dialog. */
function updateMasterlist() {
  loot.Dialog.showProgress('Updating and parsing masterlist...');
  return loot.query('updateMasterlist').then(JSON.parse).then((result) => {
    if (result) {
      /* Update JS variables. */
      loot.game.masterlist = result.masterlist;
      loot.game.globalMessages = result.globalMessages;

      result.plugins.forEach((resultPlugin) => {
        const existingPlugin = loot.game.plugins.find((plugin) => {
          return plugin.name === resultPlugin.name;
        });
        if (existingPlugin) {
          existingPlugin.isDirty = resultPlugin.isDirty;
          existingPlugin.isPriorityGlobal = resultPlugin.isPriorityGlobal;
          existingPlugin.masterlist = resultPlugin.masterlist;
          existingPlugin.messages = resultPlugin.messages;
          existingPlugin.priority = resultPlugin.priority;
          existingPlugin.tags = resultPlugin.tags;
        }
      });

      loot.Dialog.showNotification(loot.l10n.translate('Masterlist updated to revision %s.', loot.game.masterlist.revision));
    } else {
      loot.Dialog.showNotification(loot.l10n.translate('No masterlist update was necessary.'));
    }
  }).catch(handlePromiseError);
}
function onUpdateMasterlist() {
  updateMasterlist().then(() => {
    loot.Dialog.closeProgress();
  }).catch(handlePromiseError);
}
function onSortPlugins() {
  if (undoConflictsFilter()) {
    /* Conflicts filter was undone, update the displayed cards. */
    filterPluginData(loot.game.plugins, loot.filters);
  }

  let promise = Promise.resolve();
  if (loot.settings.updateMasterlist) {
    promise = promise.then(updateMasterlist);
  }
  promise.then(() => {
    return loot.query('sortPlugins').then(JSON.parse);
  }).then((result) => {
    if (!result) {
      return;
    }

    loot.game.globalMessages = result.globalMessages;

    if (!result.plugins) {
      const message = result.globalMessages.find((item) => {
        return item.content[0].str.startsWith('Cyclic interaction detected');
      });
      throw new Error(loot.l10n.translate('Failed to sort plugins. Details: ' + message.content[0].str));
    }

    /* Check if sorted load order differs from current load order. */
    const loadOrderIsUnchanged = result.plugins.every((plugin, index) => {
      return plugin.name === loot.game.plugins[index].name;
    });
    if (loadOrderIsUnchanged) {
      result.plugins.forEach((plugin) => {
        const existingPlugin = loot.game.plugins.find((item) => {
          return item.name === plugin.name;
        });
        if (existingPlugin) {
          existingPlugin.crc = plugin.crc;
          existingPlugin.isEmpty = plugin.isEmpty;
        }
      });
      /* Send discardUnappliedChanges query. Not doing so prevents LOOT's window
         from closing. */
      loot.query('discardUnappliedChanges');
      loot.Dialog.closeProgress();
      loot.Dialog.showNotification(loot.l10n.translate('Sorting made no changes to the load order.'));
      return;
    }
    loot.game.oldLoadOrder = loot.game.plugins;
    loot.game.loadOrder = [];
    result.plugins.forEach((plugin) => {
      let existingPlugin = loot.game.plugins.find((item) => {
        return item.name === plugin.name;
      });
      if (existingPlugin) {
        existingPlugin.crc = plugin.crc;
        existingPlugin.isEmpty = plugin.isEmpty;
      } else {
        existingPlugin = new loot.Plugin(plugin);
      }
      loot.game.loadOrder.push(existingPlugin);
    });

    /* Now update the UI for the new order. */
    loot.game.plugins = loot.game.loadOrder;
    filterPluginData(loot.game.plugins, loot.filters);

    /* Now hide the masterlist update buttons, and display the accept and
       cancel sort buttons. */
    loot.dom.hide('updateMasterlistButton');
    loot.dom.hide('sortButton');
    loot.dom.show('applySortButton');
    loot.dom.show('cancelSortButton');

    /* Disable changing game. */
    document.getElementById('gameMenu').setAttribute('disabled', '');
    loot.Dialog.closeProgress();
  }).catch(handlePromiseError);
}
function onApplySort() {
  const loadOrder = loot.game.plugins.map((plugin) => {
    return plugin.name;
  });
  return loot.query('applySort', loadOrder).then(() => {
    /* Remove old load order storage. */
    delete loot.game.loadOrder;
    delete loot.game.oldLoadOrder;

    /* Now show the masterlist update buttons, and hide the accept and
       cancel sort buttons. */
    loot.dom.show('updateMasterlistButton');
    loot.dom.show('sortButton');
    loot.dom.hide('applySortButton');
    loot.dom.hide('cancelSortButton');

    /* Enable changing game. */
    document.getElementById('gameMenu').removeAttribute('disabled');
  }).catch(handlePromiseError);
}
function onCancelSort() {
  return loot.query('cancelSort').then(JSON.parse).then((messages) => {
    /* Sort UI elements again according to stored old load order. */
    loot.game.plugins = loot.game.oldLoadOrder;
    filterPluginData(loot.game.plugins, loot.filters);
    delete loot.game.loadOrder;
    delete loot.game.oldLoadOrder;

    /* Update general messages */
    loot.game.globalMessages = messages;

    /* Now show the masterlist update buttons, and hide the accept and
       cancel sort buttons. */
    loot.dom.show('updateMasterlistButton');
    loot.dom.show('sortButton');
    loot.dom.hide('applySortButton');
    loot.dom.hide('cancelSortButton');

    /* Enable changing game. */
    document.getElementById('gameMenu').removeAttribute('disabled');
  }).catch(handlePromiseError);
}

function onRedatePlugins(evt) {
  if (evt.target.hasAttribute('disabled')) {
    return;
  }

  loot.Dialog.askQuestion(loot.l10n.translate('Redate Plugins?'), loot.l10n.translate('This feature is provided so that modders using the Creation Kit may set the load order it uses. A side-effect is that any subscribed Steam Workshop mods will be re-downloaded by Steam. Do you wish to continue?'), loot.l10n.translate('Redate'), (result) => {
    if (result) {
      loot.query('redatePlugins').then(() => {
        loot.Dialog.showNotification('Plugins were successfully redated.');
      }).catch(handlePromiseError);
    }
  });
}
function onClearAllMetadata() {
  loot.Dialog.askQuestion('', loot.l10n.translate('Are you sure you want to clear all existing user-added metadata from all plugins?'), loot.l10n.translate('Clear'), (result) => {
    if (!result) {
      return;
    }
    loot.query('clearAllMetadata').then(JSON.parse).then((plugins) => {
      if (!plugins) {
        return;
      }
      /* Need to empty the UI-side user metadata. */
      plugins.forEach((plugin) => {
        const existingPlugin = loot.game.plugins.find((item) => {
          return item.name === plugin.name;
        });
        if (existingPlugin) {
          existingPlugin.userlist = undefined;
          existingPlugin.editor = undefined;

          existingPlugin.priority = plugin.priority;
          existingPlugin.isPriorityGlobal = plugin.isPriorityGlobal;
          existingPlugin.messages = plugin.messages;
          existingPlugin.tags = plugin.tags;
          existingPlugin.isDirty = plugin.isDirty;
        }
      });

      loot.Dialog.showNotification(loot.l10n.translate('All user-added metadata has been cleared.'));
    }).catch(handlePromiseError);
  });
}
function onCopyContent() {
  let messages = [];
  let plugins = [];

  if (loot.game) {
    if (loot.game.globalMessages) {
      messages = loot.game.globalMessages.map((message) => {
        return {
          type: message.type,
          content: message.content[0].str,
        };
      });
    }
    if (loot.game.plugins) {
      plugins = loot.game.plugins.map((plugin) => {
        return {
          name: plugin.name,
          crc: plugin.crc,
          version: plugin.version,
          isActive: plugin.isActive,
          isEmpty: plugin.isEmpty,
          loadsArchive: plugin.loadsArchive,

          priority: plugin.priority,
          isPriorityGlobal: plugin.isPriorityGlobal,
          messages: plugin.messages,
          tags: plugin.tags,
          isDirty: plugin.isDirty,
        };
      });
    }
  } else {
    const message = document.getElementById('summary').getElementsByTagName('ul')[0].firstElementChild;
    if (message) {
      messages.push({
        type: 'error',
        content: message.textContent,
      });
    }
  }

  loot.query('copyContent', {
    messages,
    plugins,
  }).then(() => {
    loot.Dialog.showNotification(loot.l10n.translate("LOOT's content has been copied to the clipboard."));
  }).catch(handlePromiseError);
}
function onCopyLoadOrder() {
  let plugins = [];

  if (loot.game && loot.game.plugins) {
    plugins = loot.game.plugins.map((plugin) => {
      return plugin.name;
    });
  }

  loot.query('copyLoadOrder', plugins).then(() => {
    loot.Dialog.showNotification(loot.l10n.translate('The load order has been copied to the clipboard.'));
  }).catch(handlePromiseError);
}
function onContentRefresh() {
  /* Send a query for updated load order and plugin header info. */
  loot.query('getGameData').then((result) => {
    /* Parse the data sent from C++. */
    const game = JSON.parse(result, loot.Plugin.fromJson);
    loot.game = new loot.Game(game, loot.l10n);

    /* Reapply filters. */
    filterPluginData(loot.game.plugins, loot.filters);

    loot.Dialog.closeProgress();
  }).catch(handlePromiseError);
}

function onOpenReadme() {
  loot.query('openReadme').catch(handlePromiseError);
}
function onOpenLogLocation() {
  loot.query('openLogLocation').catch(handlePromiseError);
}
function onShowAboutDialog() {
  document.getElementById('about').open();
}
function handleUnappliedChangesClose(change) {
  loot.Dialog.askQuestion('', loot.l10n.translate('You have not yet applied or cancelled your %s. Are you sure you want to quit?', change), loot.l10n.translate('Quit'), (result) => {
    if (!result) {
      return;
    }
    /* Discard any unapplied changes. */
    loot.query('discardUnappliedChanges').then(() => {
      window.close();
    }).catch(handlePromiseError);
  });
}
function onQuit() {
  if (!document.getElementById('applySortButton').hidden) {
    handleUnappliedChangesClose(loot.l10n.translate('sorted load order'));
  } else if (document.body.hasAttribute('data-editors')) {
    handleUnappliedChangesClose(loot.l10n.translate('metadata edits'));
  } else {
    window.close();
  }
}

function onSwitchSidebarTab(evt) {
  document.getElementById(evt.target.selected).parentElement.selected = evt.target.selected;
}
function onSidebarClick(evt) {
  if (evt.target.hasAttribute('data-index')) {
    const index = parseInt(evt.target.getAttribute('data-index'), 10);
    document.getElementById('pluginCardList').scrollToIndex(index);

    if (evt.type === 'dblclick') {
      /* Double-clicking can select the item's text, clear the selection in
         case that has happened. */
      window.getSelection().removeAllRanges();

      if (!document.body.hasAttribute('data-editors')) {
        document.getElementById(evt.target.getAttribute('data-id')).onShowEditor();
      }
    }
  }
}

function areSettingsValid() {
  return document.getElementById('gameTable').validate();
}
function onApplySettings(evt) {
  if (!areSettingsValid()) {
    evt.stopPropagation();
  }
}
function onCloseSettingsDialog(evt) {
  if (evt.target.id !== 'settingsDialog') {
    /* The event can be fired by dropdowns in the settings dialog, so ignore
       any events that don't come from the dialog itself. */
    return;
  }
  if (!evt.detail.confirmed) {
    /* Re-apply the existing settings to the settings dialog elements. */
    loot.dom.updateSettingsDialog(loot.settings);
    return;
  }

  /* Update the JS variable values. */
  const settings = {
    enableDebugLogging: document.getElementById('enableDebugLogging').checked,
    game: document.getElementById('defaultGameSelect').value,
    games: document.getElementById('gameTable').getRowsData(false),
    language: document.getElementById('languageSelect').value,
    lastGame: loot.settings.lastGame,
    updateMasterlist: document.getElementById('updateMasterlist').checked,
    filters: loot.settings.filters,
  };

  /* Send the settings back to the C++ side. */
  loot.query('closeSettings', settings).then(JSON.parse).then((installedGames) => {
    loot.installedGames = installedGames;
    loot.dom.updateEnabledGames(installedGames);
  }).catch(handlePromiseError).then(() => {
    loot.settings = settings;
    loot.dom.updateSettingsDialog(loot.settings);
    loot.dom.setGameMenuItems(loot.settings.games);
    loot.dom.updateEnabledGames(loot.installedGames);
    loot.dom.updateSelectedGame(loot.game.folder);
  }).catch(handlePromiseError);
}
function onShowSettingsDialog() {
  document.getElementById('settingsDialog').open();
}

function onEditorOpen(evt) {
  /* Set the editor data. */
  document.getElementById('editor').setEditorData(evt.target.data);

  /* Set body attribute so that sidebar items are styled correctly. */
  document.body.setAttribute('data-editors', true);
  document.getElementById('cardsNav').notifyResize();

  /* Set up drag 'n' drop event handlers. */
  const elements = document.getElementById('cardsNav').getElementsByTagName('loot-plugin-item');
  for (let i = 0; i < elements.length; ++i) {
    elements[i].draggable = true;
    elements[i].addEventListener('dragstart', elements[i].onDragStart);
  }

  /* Disable the toolbar elements. */
  document.getElementById('wipeUserlistButton').setAttribute('disabled', '');
  document.getElementById('copyContentButton').setAttribute('disabled', '');
  document.getElementById('refreshContentButton').setAttribute('disabled', '');
  document.getElementById('settingsButton').setAttribute('disabled', '');
  document.getElementById('gameMenu').setAttribute('disabled', '');
  document.getElementById('updateMasterlistButton').setAttribute('disabled', '');
  document.getElementById('sortButton').setAttribute('disabled', '');

  return loot.query('editorOpened').catch(handlePromiseError);
}
function onEditorClose(evt) {
  /* evt.detail is true if the apply button was pressed. */
  let promise;
  if (evt.detail) {
    /* Need to record the editor control values and work out what's
       changed, and update any UI elements necessary. Offload the
       majority of the work to the C++ side of things. */
    const plugin = loot.game.plugins.find((item) => {
      return item.name === evt.target.querySelector('h1').textContent;
    });
    const edits = evt.target.readFromEditor(plugin);
    promise = loot.query('editorClosed', edits).then(JSON.parse).then((result) => {
      if (result) {
        plugin.priority = result.priority;
        plugin.isPriorityGlobal = result.isPriorityGlobal;
        plugin.messages = result.messages;
        plugin.tags = result.tags;
        plugin.isDirty = result.isDirty;

        plugin.userlist = edits.userlist;

        /* Now perform search again. If there is no current search, this won't
           do anything. */
        document.getElementById('searchBar').search();
      }
    });
  } else {
    /* Don't need to record changes, but still need to notify C++ side that
       the editor has been closed. */
    promise = loot.query('editorClosed');
  }
  promise.then(() => {
    /* Remove body attribute so that sidebar items are styled correctly. */
    document.body.removeAttribute('data-editors');
    document.getElementById('cardsNav').notifyResize();

    /* Remove drag 'n' drop event handlers. */
    const elements = document.getElementById('cardsNav').getElementsByTagName('loot-plugin-item');
    for (let i = 0; i < elements.length; ++i) {
      elements[i].removeAttribute('draggable');
      elements[i].removeEventListener('dragstart', elements[i].onDragStart);
    }

    /* Re-enable toolbar elements. */
    document.getElementById('wipeUserlistButton').removeAttribute('disabled');
    document.getElementById('copyContentButton').removeAttribute('disabled');
    document.getElementById('refreshContentButton').removeAttribute('disabled');
    document.getElementById('settingsButton').removeAttribute('disabled');
    document.getElementById('gameMenu').removeAttribute('disabled');
    document.getElementById('updateMasterlistButton').removeAttribute('disabled');
    document.getElementById('sortButton').removeAttribute('disabled');
  }).catch(handlePromiseError);
}
function undoConflictsFilter() {
  let wasConflictsFilterEnabled = (loot.filters.conflictTargetPluginName);

  loot.filters.conflictTargetPluginName = undefined;
  /* Deactivate any existing plugin conflict filter. */
  loot.game.plugins.forEach((plugin) => {
    plugin.isConflictFilterChecked = false;
  });
  /* Un-highlight any existing filter plugin. */
  const cards = document.getElementById('main').getElementsByTagName('loot-plugin-card');
  for (let i = 0; i < cards.length; ++i) {
    cards[i].classList.toggle('highlight', false);
  }

  return wasConflictsFilterEnabled;
}
function onConflictsFilter(evt) {
  /* Deactivate any existing plugin conflict filter. */
  undoConflictsFilter();
  /* evt.detail is true if the filter has been activated. */
  if (evt.detail) {
    evt.target.data.isConflictFilterChecked = true;
    loot.filters.conflictTargetPluginName = evt.target.getName();
    evt.target.classList.toggle('highlight', true);
  } else {
    loot.filters.conflictTargetPluginName = undefined;
  }
  filterPluginData(loot.game.plugins, loot.filters);
}
function onCopyMetadata(evt) {
  loot.query('copyMetadata', evt.target.getName()).then(() => {
    loot.Dialog.showNotification(loot.l10n.translate('The metadata for "%s" has been copied to the clipboard.', evt.target.getName()));
  }).catch(handlePromiseError);
}
function onClearMetadata(evt) {
  loot.Dialog.askQuestion('', loot.l10n.translate('Are you sure you want to clear all existing user-added metadata from "%s"?', evt.target.getName()), loot.l10n.translate('Clear'), (result) => {
    if (!result) {
      return;
    }
    loot.query('clearPluginMetadata', evt.target.getName()).then(JSON.parse).then((plugin) => {
      if (!result) {
        return;
      }
      /* Need to empty the UI-side user metadata. */
      const existingPlugin = loot.game.plugins.find((item) => {
        return item.id === evt.target.id;
      });
      if (existingPlugin) {
        existingPlugin.userlist = undefined;
        existingPlugin.editor = undefined;

        existingPlugin.priority = plugin.priority;
        existingPlugin.isPriorityGlobal = plugin.isPriorityGlobal;
        existingPlugin.messages = plugin.messages;
        existingPlugin.tags = plugin.tags;
        existingPlugin.isDirty = plugin.isDirty;
      }
      loot.Dialog.showNotification(loot.l10n.translate('The user-added metadata for "%s" has been cleared.', evt.target.getName()));
      /* Now perform search again. If there is no current search, this won't
         do anything. */
      document.getElementById('searchBar').search();
    }).catch(handlePromiseError);
  });
}

function onFocusSearch(evt) {
  if (evt.ctrlKey && evt.keyCode === 70) { // 'f'
    document.getElementById('mainToolbar').classList.add('search');
    document.getElementById('searchBar').focusInput();
  }
}
function onSearchOpen() {
  document.getElementById('mainToolbar').classList.add('search');
  document.getElementById('searchBar').focusInput();
}
function onSearchBegin(evt) {
  loot.game.plugins.forEach((plugin) => {
    plugin.isSearchResult = false;
  });

  if (!evt.detail.needle) {
    return;
  }

  // Don't push to the target's results property directly, as the
  // change observer doesn't work correctly unless special Polymer APIs
  // are used, which I don't want to get into.
  const results = [];
  loot.game.plugins.forEach((plugin, index) => {
    if (plugin.getCardContent(loot.filters).containsText(evt.detail.needle)) {
      results.push(index);
      plugin.isSearchResult = true;
    }
  });

  evt.target.results = results;
}
function onSearchChangeSelection(evt) {
  document.getElementById('pluginCardList').scrollToIndex(evt.detail.selection);
}
function onSearchEnd(evt) {
  loot.game.plugins.forEach((plugin) => {
    plugin.isSearchResult = false;
  });
  document.getElementById('mainToolbar').classList.remove('search');
}
