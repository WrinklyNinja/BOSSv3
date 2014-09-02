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

#include "misc.h"
#include "../backend/helpers.h"

#include <boost/log/trivial.hpp>



using namespace std;

MessageList::MessageList(wxWindow * parent, wxWindowID id, const unsigned int language) : wxListView(parent, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_VIRTUAL | wxLC_SINGLE_SEL), _language(language) {
    InsertColumn(0, translate("Type"));
    InsertColumn(1, translate("Content"));
    InsertColumn(2, translate("Condition"));
    InsertColumn(3, translate("Language"));

    Bind(wxEVT_LIST_DELETE_ITEM, &MessageList::OnDeleteItem, this);

    SetItemCount(0);
}

std::vector<loot::Message> MessageList::GetItems() const {
    return _messages;
}

void MessageList::SetItems(const std::vector<loot::Message>& messages) {
    _messages = messages;
    SetItemCount(_messages.size());
    RefreshItems(0, _messages.size() - 1);
}

loot::Message MessageList::GetItem(long item) const {
    return _messages[item];
}

void MessageList::SetItem(long item, const loot::Message& message) {
    _messages[item] = message;
    RefreshItem(item);
}

void MessageList::AppendItem(const loot::Message& message) {
    _messages.push_back(message);
    SetItemCount(_messages.size());
    RefreshItem(_messages.size() - 1);
}

void MessageList::OnDeleteItem(wxListEvent& event) {
    _messages.erase(_messages.begin() + event.GetIndex());
    SetItemCount(_messages.size());
    RefreshItems(event.GetIndex(), _messages.size() - 1);
}

wxString MessageList::OnGetItemText(long item, long column) const {
    if (column < 0 || column > 3 || item < 0 || item > _messages.size() - 1)
        return wxString();

    if (column == 0) {
        if (_messages[item].Type() == loot::Message::say)
            return Type[0];
        else if (_messages[item].Type() == loot::Message::warn)
            return Type[1];
        else
            return Type[2];
    }
    else if (column == 1) {
        return FromUTF8(_messages[item].ChooseContent(_language).Str());
    }
    else if (column == 2) {
        return FromUTF8(_messages[item].Condition());
    }
    else {
        return FromUTF8(loot::Language(_messages[item].ChooseContent(_language).Language()).Name());
    }
}


FileEditDialog::FileEditDialog(wxWindow *parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {

    _name = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_EMPTY));
    _display = new wxTextCtrl(this, wxID_ANY);
    _condition = new wxTextCtrl(this, wxID_ANY);

    wxSizerFlags leftItem(0);
    leftItem.Left();

    wxSizerFlags rightItem(1);
    rightItem.Right().Expand();

    wxBoxSizer * bigBox = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer * GridSizer = new wxFlexGridSizer(2, 5, 5);
    GridSizer->AddGrowableCol(1, 1);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Filename (required):")), leftItem);
    GridSizer->Add(_name, rightItem);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Displayed Name:")), leftItem);
    GridSizer->Add(_display, rightItem);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Condition:")), leftItem);
    GridSizer->Add(_condition, rightItem);

    bigBox->Add(GridSizer, 0, wxEXPAND | wxALL, 15);

    bigBox->AddSpacer(10);
    bigBox->AddStretchSpacer(1);

    //Need to add 'OK' and 'Cancel' buttons.
    wxSizer * sizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
    if (sizer != nullptr)
        bigBox->Add(sizer, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 15);

    SetBackgroundColour(wxColour(255, 255, 255));
    SetIcon(wxIconLocation("LOOT.exe"));
    SetSizerAndFit(bigBox);
}

void FileEditDialog::SetValues(const wxString& name, const wxString& display, const wxString& condition) {

    _name->SetValue(name);
    _display->SetValue(display);
    _condition->SetValue(condition);
}

wxString FileEditDialog::GetName() const {
    return _name->GetValue();
}

wxString FileEditDialog::GetDisplayName() const {
    return _display->GetValue();
}

wxString FileEditDialog::GetCondition() const {
    return _condition->GetValue();
}

MessageEditDialog::MessageEditDialog(wxWindow *parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {

    wxArrayString languages;
    vector<string> langs = loot::Language::Names();
    for (size_t i = 0; i < langs.size(); i++) {
        languages.Add(FromUTF8(langs[i]));
    }

    //Initialise controls.
    _type = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, Type);
    _language = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, languages);

    _condition = new wxTextCtrl(this, wxID_ANY);
    _str = new wxTextCtrl(this, wxID_ANY);

    _content = new wxListView(this, LIST_MessageContent, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);

    addBtn = new wxButton(this, BUTTON_AddContent, translate("Add Content"));
    editBtn = new wxButton(this, BUTTON_EditContent, translate("Edit Content"));
    removeBtn = new wxButton(this, BUTTON_RemoveContent, translate("Remove Content"));

    _content->InsertColumn(0, translate("Language"));
    _content->InsertColumn(1, translate("String"));

    //Set up event handling.
    Bind(wxEVT_BUTTON, &MessageEditDialog::OnAdd, this, BUTTON_AddContent);
    Bind(wxEVT_BUTTON, &MessageEditDialog::OnEdit, this, BUTTON_EditContent);
    Bind(wxEVT_BUTTON, &MessageEditDialog::OnRemove, this, BUTTON_RemoveContent);
    Bind(wxEVT_LIST_ITEM_SELECTED, &MessageEditDialog::OnSelect, this, LIST_MessageContent);

    wxSizerFlags leftItem(0);
    leftItem.Left();

    wxSizerFlags rightItem(1);
    rightItem.Right();

    wxSizerFlags wholeItem(0);
    wholeItem.Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 15);

    wxBoxSizer * bigBox = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer * GridSizer = new wxFlexGridSizer(2, 5, 5);
    GridSizer->AddGrowableCol(1, 1);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Type:")), leftItem);
    GridSizer->Add(_type, rightItem);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Condition:")), leftItem);
    GridSizer->Add(_condition, rightItem);

    bigBox->AddSpacer(15);

    bigBox->Add(GridSizer, wholeItem);

    bigBox->Add(_content, wholeItem);

    wxFlexGridSizer * GridSizer2 = new wxFlexGridSizer(2, 5, 5);
    GridSizer2->AddGrowableCol(1, 1);

    GridSizer2->Add(new wxStaticText(this, wxID_ANY, translate("Language:")), leftItem);
    GridSizer2->Add(_language, rightItem);

    GridSizer2->Add(new wxStaticText(this, wxID_ANY, translate("Content:")), leftItem);
    GridSizer2->Add(_str, rightItem);

    bigBox->Add(GridSizer2, wholeItem);

    wxBoxSizer * hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->Add(addBtn, 0, wxRIGHT, 5);
    hbox->Add(editBtn, 0, wxLEFT | wxRIGHT, 5);
    hbox->Add(removeBtn, 0, wxLEFT, 5);
    bigBox->Add(hbox, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT, 15);

    bigBox->AddSpacer(10);
    bigBox->AddStretchSpacer(1);

    //Need to add 'OK' and 'Cancel' buttons.
    wxSizer * sizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
    if (sizer != nullptr)
        bigBox->Add(sizer, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 15);

    //Set defaults.
    _type->SetSelection(0);
    _language->SetSelection(0);
    editBtn->Enable(false);
    removeBtn->Enable(false);

    SetBackgroundColour(wxColour(255, 255, 255));
    SetIcon(wxIconLocation("LOOT.exe"));
    SetSizerAndFit(bigBox);
}

void MessageEditDialog::SetMessage(const loot::Message& message) {

    if (message.Type() == loot::Message::say)
        _type->SetSelection(0);
    else if (message.Type() == loot::Message::warn)
        _type->SetSelection(1);
    else
        _type->SetSelection(2);

    _condition->SetValue(FromUTF8(message.Condition()));

    vector<loot::MessageContent> contents = message.Content();
    for (size_t i = 0, max = contents.size(); i < max; ++i) {
        _content->InsertItem(i, FromUTF8(loot::Language(contents[i].Language()).Name()));
        _content->SetItem(i, 1, FromUTF8(contents[i].Str()));
    }
}

loot::Message MessageEditDialog::GetMessage() const {

    unsigned int type;
    string condition;
    if (_type->GetSelection() == 0)
        type = loot::Message::say;
    else if (_type->GetSelection() == 1)
        type = loot::Message::warn;
    else
        type = loot::Message::error;

    condition = string(_condition->GetValue().ToUTF8());

    vector<loot::MessageContent> contents;
    for (size_t i = 0, max = _content->GetItemCount(); i < max; ++i) {

        string str = string(_content->GetItemText(i, 1).ToUTF8());
        unsigned int lang = loot::Language(string(_content->GetItemText(i, 0).ToUTF8())).Code();

        contents.push_back(loot::MessageContent(str, lang));
    }

    return loot::Message(type, contents, condition);
}

void MessageEditDialog::OnSelect(wxListEvent& event) {
    _language->SetSelection(loot::Language(string(_content->GetItemText(event.GetIndex(), 0).ToUTF8())).Code());
    _str->SetValue(_content->GetItemText(event.GetIndex(), 1));
    editBtn->Enable(true);
    removeBtn->Enable(true);
}

void MessageEditDialog::OnAdd(wxCommandEvent& event) {
    long i = _content->GetItemCount();
    _content->InsertItem(i, FromUTF8(loot::Language(_language->GetSelection()).Name()));
    _content->SetItem(i, 1, _str->GetValue());
}

void MessageEditDialog::OnEdit(wxCommandEvent& event) {
    if (_content->GetFirstSelected() == -1) {
        BOOST_LOG_TRIVIAL(error) << "Attempting to edit message content, but no content row selected.";
        wxMessageBox(
            translate("Error: No content row selected."),
            translate("LOOT: Error"),
            wxOK | wxICON_ERROR,
            nullptr);
        return;
    }
    long i = _content->GetFirstSelected();
    _content->SetItem(i, 0, FromUTF8(loot::Language(_language->GetSelection()).Name()));
    _content->SetItem(i, 1, _str->GetValue());
}

void MessageEditDialog::OnRemove(wxCommandEvent& event) {
    if (_content->GetFirstSelected() == -1) {
        BOOST_LOG_TRIVIAL(error) << "Attempting to remove message content, but no content row selected.";
        wxMessageBox(
            translate("Error: No content row selected."),
            translate("LOOT: Error"),
            wxOK | wxICON_ERROR,
            nullptr);
        return;
    }
    _content->DeleteItem(_content->GetFirstSelected());
    editBtn->Enable(false);
    removeBtn->Enable(false);
}

TagEditDialog::TagEditDialog(wxWindow *parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {

    //Initialise controls.
    _state = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, State);

    _name = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_EMPTY));
    _condition = new wxTextCtrl(this, wxID_ANY);

    wxSizerFlags leftItem(0);
    leftItem.Left();

    wxSizerFlags rightItem(1);
    rightItem.Right().Expand();

    wxBoxSizer * bigBox = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer * GridSizer = new wxFlexGridSizer(2, 5, 5);
    GridSizer->AddGrowableCol(1, 1);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Add/Remove:")), leftItem);
    GridSizer->Add(_state, rightItem);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Name (required):")), leftItem);
    GridSizer->Add(_name, rightItem);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Condition:")), leftItem);
    GridSizer->Add(_condition, rightItem);

    bigBox->Add(GridSizer, 0, wxEXPAND | wxALL, 15);

    bigBox->AddSpacer(10);
    bigBox->AddStretchSpacer(1);

    //Need to add 'OK' and 'Cancel' buttons.
    wxSizer * sizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
    if (sizer != nullptr)
        bigBox->Add(sizer, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 15);

    //Set defaults.
    _state->SetSelection(0);

    SetBackgroundColour(wxColour(255, 255, 255));
    SetIcon(wxIconLocation("LOOT.exe"));
    SetSizerAndFit(bigBox);
}

void TagEditDialog::SetValues(int state, const wxString& name, const wxString& condition) {

    _state->SetSelection(state);
    _name->SetValue(name);
    _condition->SetValue(condition);
}

wxString TagEditDialog::GetState() const {
    return State[_state->GetSelection()];
}

wxString TagEditDialog::GetName() const {
    return _name->GetValue();
}

wxString TagEditDialog::GetCondition() const {
    return _condition->GetValue();
}

DirtInfoEditDialog::DirtInfoEditDialog(wxWindow * parent, const wxString& title) : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
    //Initialise controls.
    wxTextValidator val(wxFILTER_EMPTY | wxFILTER_INCLUDE_CHAR_LIST);
    val.SetCharIncludes("0123456789ABCDEFabcdef");

    _itm = new wxSpinCtrl(this, wxID_ANY, "0");
    _udr = new wxSpinCtrl(this, wxID_ANY, "0");
    _nav = new wxSpinCtrl(this, wxID_ANY, "0");
    _crc = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val);
    _utility = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_EMPTY));

    wxSizerFlags leftItem(0);
    leftItem.Left();

    wxSizerFlags rightItem(1);
    rightItem.Right().Expand();

    wxBoxSizer * bigBox = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer * GridSizer = new wxFlexGridSizer(2, 5, 5);
    GridSizer->AddGrowableCol(1, 1);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("CRC (required):")), leftItem);
    GridSizer->Add(_crc, rightItem);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("ITM Count:")), leftItem);
    GridSizer->Add(_itm, rightItem);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("UDR Count:")), leftItem);
    GridSizer->Add(_udr, rightItem);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Deleted Navmesh Count:")), leftItem);
    GridSizer->Add(_nav, rightItem);

    GridSizer->Add(new wxStaticText(this, wxID_ANY, translate("Cleaning Utility (required):")), leftItem);
    GridSizer->Add(_utility, rightItem);

    bigBox->Add(GridSizer, 0, wxEXPAND | wxALL, 15);

    bigBox->AddSpacer(10);
    bigBox->AddStretchSpacer(1);

    //Need to add 'OK' and 'Cancel' buttons.
    wxSizer * sizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
    if (sizer != nullptr)
        bigBox->Add(sizer, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 15);

    SetBackgroundColour(wxColour(255, 255, 255));
    SetIcon(wxIconLocation("LOOT.exe"));
    SetSizerAndFit(bigBox);
}

void DirtInfoEditDialog::SetValues(const wxString& crc, unsigned int itm, unsigned int udr, unsigned int nav, const wxString& utility) {
    _crc->SetValue(crc);
    _itm->SetValue(itm);
    _udr->SetValue(udr);
    _nav->SetValue(nav);
    _utility->SetValue(utility);
}

wxString DirtInfoEditDialog::GetCRC() const {
    return _crc->GetValue();
}

unsigned int DirtInfoEditDialog::GetITMs() const {
    return _itm->GetValue();
}

unsigned int DirtInfoEditDialog::GetUDRs() const {
    return _udr->GetValue();
}

unsigned int DirtInfoEditDialog::GetDeletedNavmeshes() const {
    return _nav->GetValue();
}

wxString DirtInfoEditDialog::GetUtility() const {
    return _utility->GetValue();
}