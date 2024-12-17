#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/splitter.h>
#include <wx/artprov.h>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <json/json.h>
#include <vector>

// Data structure for conversation
struct Conversation {
    wxString title;
    std::vector<std::pair<std::string, std::string>> messages;
};

static void AddHoverEffect(wxButton* button, const wxColour& normalColor, const wxColour& hoverColor) {
    button->Bind(wxEVT_ENTER_WINDOW, [button, hoverColor](wxMouseEvent& event) {
        button->SetBackgroundColour(hoverColor);
        button->Refresh();
        event.Skip();
        });
    button->Bind(wxEVT_LEAVE_WINDOW, [button, normalColor](wxMouseEvent& event) {
        button->SetBackgroundColour(normalColor);
        button->Refresh();
        event.Skip();
        });
}

class ChatApp : public wxFrame {
public:
    ChatApp(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1200, 750))
    {
        // Light color palette
        wxColour bgMain = wxColour("#FFFFFF");
        wxColour bgSidebar = wxColour("#F0F0F0");
        wxColour bgChatArea = wxColour("#FFFFFF");
        wxColour textColor = wxColour("#333333");
        wxColour accentColor = wxColour("#00C58B");
        wxColour hoverColor = wxColour("#00B07D");

        SetBackgroundColour(bgMain);
        SetTitle("Dialogix");

        wxFont mainFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Poppins");

        // Splitter Window
        wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);
        splitter->SetMinimumPaneSize(200);

        // Panels
        wxPanel* sidebarPanel = new wxPanel(splitter, wxID_ANY);
        wxPanel* chatPanel = new wxPanel(splitter, wxID_ANY);

        sidebarPanel->SetBackgroundColour(bgSidebar);
        chatPanel->SetBackgroundColour(bgMain);

        // SIDEBAR
        wxBoxSizer* sidebarSizer = new wxBoxSizer(wxVERTICAL);

        wxBitmap logoBitmap = wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_OTHER, wxSize(32, 32));
        wxStaticBitmap* logo = new wxStaticBitmap(sidebarPanel, wxID_ANY, logoBitmap);
        wxStaticText* logoText = new wxStaticText(sidebarPanel, wxID_ANY, "Dialogix");
        logoText->SetForegroundColour(textColor);
        wxFont logoFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Poppins");
        logoText->SetFont(logoFont);

        wxBoxSizer* logoSizer = new wxBoxSizer(wxHORIZONTAL);
        logoSizer->Add(logo, 0, wxRIGHT, 5);
        logoSizer->Add(logoText, 0, wxALIGN_CENTER_VERTICAL);

        newChatButton = new wxButton(sidebarPanel, wxID_ANY, "Create chat", wxDefaultPosition, wxSize(-1, 40), wxBORDER_NONE);
        newChatButton->SetBackgroundColour(accentColor);
        newChatButton->SetForegroundColour(*wxWHITE);
        newChatButton->SetFont(mainFont);
        newChatButton->SetCursor(wxCURSOR_HAND);
        AddHoverEffect(newChatButton, accentColor, hoverColor);

        wxStaticText* historyTitle = new wxStaticText(sidebarPanel, wxID_ANY, "History");
        historyTitle->SetForegroundColour(textColor);
        wxFont historyFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Poppins");
        historyTitle->SetFont(historyFont);

        chatList = new wxListBox(sidebarPanel, wxID_ANY, wxDefaultPosition, wxSize(-1, -1));
        chatList->SetBackgroundColour(*wxWHITE);
        chatList->SetForegroundColour(textColor);
        chatList->SetFont(mainFont);
        chatList->SetWindowStyleFlag(wxBORDER_NONE);

        sidebarSizer->AddSpacer(20);
        sidebarSizer->Add(logoSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 20);
        sidebarSizer->AddSpacer(20);
        sidebarSizer->Add(newChatButton, 0, wxLEFT | wxRIGHT | wxEXPAND, 20);
        sidebarSizer->AddSpacer(20);
        sidebarSizer->Add(historyTitle, 0, wxLEFT | wxRIGHT, 20);
        sidebarSizer->AddSpacer(10);
        sidebarSizer->Add(chatList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 20);

        sidebarPanel->SetSizer(sidebarSizer);

        // CHAT AREA
        wxBoxSizer* chatSizer = new wxBoxSizer(wxVERTICAL);

        // Top logo
        wxBoxSizer* chatHeaderSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticBitmap* chatLogo = new wxStaticBitmap(chatPanel, wxID_ANY, logoBitmap);
        wxStaticText* chatLogoText = new wxStaticText(chatPanel, wxID_ANY, "Dialogix");
        chatLogoText->SetForegroundColour(textColor);
        wxFont chatLogoFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Poppins");
        chatLogoText->SetFont(chatLogoFont);
        chatHeaderSizer->Add(chatLogo, 0, wxRIGHT, 8);
        chatHeaderSizer->Add(chatLogoText, 0, wxALIGN_CENTER_VERTICAL);

        // Chat display area
        chatDisplay = new wxTextCtrl(chatPanel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 550),
            wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxBORDER_NONE);
        chatDisplay->SetBackgroundColour(bgChatArea);
        chatDisplay->SetForegroundColour(textColor);
        chatDisplay->SetFont(mainFont);
        chatDisplay->SetMinSize(wxSize(-1, 500));

        // Input area
        wxBoxSizer* inputSizer = new wxBoxSizer(wxHORIZONTAL);
        inputField = new wxTextCtrl(chatPanel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 40), wxTE_PROCESS_ENTER | wxBORDER_NONE);
        inputField->SetBackgroundColour(wxColour("#F0F0F0"));
        inputField->SetForegroundColour(textColor);
        inputField->SetFont(mainFont);
        inputField->SetHint("start conversation"); // Placeholder hint text

        sendButton = new wxButton(chatPanel, wxID_ANY, "Send", wxDefaultPosition, wxSize(80, 40), wxBORDER_NONE);
        sendButton->SetBackgroundColour(accentColor);
        sendButton->SetForegroundColour(*wxWHITE);
        sendButton->SetFont(mainFont);
        sendButton->SetCursor(wxCURSOR_HAND);
        AddHoverEffect(sendButton, accentColor, hoverColor);

        inputSizer->Add(inputField, 1, wxEXPAND | wxALL, 5);
        inputSizer->Add(sendButton, 0, wxEXPAND | wxALL, 5);

        chatSizer->AddSpacer(20);
        chatSizer->Add(chatHeaderSizer, 0, wxLEFT | wxRIGHT, 20);
        chatSizer->AddSpacer(20);
        chatSizer->Add(chatDisplay, 1, wxLEFT | wxRIGHT | wxEXPAND, 20);
        chatSizer->Add(inputSizer, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 20);

        chatPanel->SetSizer(chatSizer);

        // Events
        sendButton->Bind(wxEVT_BUTTON, &ChatApp::OnSend, this);
        inputField->Bind(wxEVT_TEXT_ENTER, &ChatApp::OnSend, this);
        newChatButton->Bind(wxEVT_BUTTON, &ChatApp::OnNewChat, this);
        chatList->Bind(wxEVT_LISTBOX, &ChatApp::OnChatSelected, this);

        // Split
        splitter->SplitVertically(sidebarPanel, chatPanel, 300);
        splitter->SetSashGravity(0.2);

        // Create a default conversation and display it immediately
        CreateDefaultConversation("Hello");

        this->Centre();
        this->Show(true);
    }

private:
    wxTextCtrl* chatDisplay;
    wxTextCtrl* inputField;
    wxListBox* chatList;
    wxButton* sendButton;
    wxButton* newChatButton;

    std::vector<Conversation> conversations;
    int currentChatIndex = -1;

    void CreateDefaultConversation(const std::string& firstMessage) {
        Conversation defaultConv;
        defaultConv.title = firstMessage;
        defaultConv.messages.push_back(std::make_pair("user", firstMessage));
        conversations.push_back(defaultConv);
        chatList->Append(defaultConv.title);
        currentChatIndex = (int)conversations.size() - 1;
        LoadConversation(currentChatIndex);
    }

    void OnSend(wxCommandEvent& event) {
        if (currentChatIndex < 0 || currentChatIndex >= (int)conversations.size()) return;

        std::string userMessage = inputField->GetValue().ToStdString();
        if (userMessage.empty()) return;

        conversations[currentChatIndex].messages.push_back(std::make_pair("user", userMessage));

        // If first message of a conversation is "New Chat", rename it
        if (conversations[currentChatIndex].title == "New Chat" && !userMessage.empty()) {
            std::string title = userMessage.substr(0, 30);
            if (userMessage.size() > 30) {
                title += "...";
            }
            conversations[currentChatIndex].title = title;
            chatList->SetString(currentChatIndex, title);
        }

        AppendUserMessage(userMessage);
        inputField->Clear();

        wxTheApp->CallAfter([this, userMessage]() {
            std::string response = GetChatGPTResponse(userMessage);
            wxTheApp->CallAfter([this, response]() {
                if (currentChatIndex < 0 || currentChatIndex >= (int)conversations.size()) return;
                conversations[currentChatIndex].messages.push_back(std::make_pair("assistant", response));
                AppendAssistantMessage(response);
                });
            });
    }

    void OnNewChat(wxCommandEvent& event) {
        Conversation newConv;
        newConv.title = "New Chat";
        conversations.push_back(newConv);
        chatList->Append("New Chat");
        currentChatIndex = (int)conversations.size() - 1;
        chatDisplay->Clear();
    }

    void OnChatSelected(wxCommandEvent& event) {
        int selection = chatList->GetSelection();
        if (selection == wxNOT_FOUND) return;
        currentChatIndex = selection;
        LoadConversation(selection);
    }

    void LoadConversation(int index) {
        if (index < 0 || index >= (int)conversations.size()) return;

        chatDisplay->Clear();
        for (auto& msg : conversations[index].messages) {
            if (msg.first == "user") {
                AppendUserMessage(msg.second);
            }
            else {
                AppendAssistantMessage(msg.second);
            }
        }
    }

    void AppendUserMessage(const std::string& message) {
        chatDisplay->AppendText("\n");
        wxTextAttr boldAttr;
        boldAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
        boldAttr.SetTextColour(wxColour("#1C5DBC")); // Dark blue for user name
        chatDisplay->SetDefaultStyle(boldAttr);
        chatDisplay->AppendText("You: ");

        wxTextAttr normalAttr;
        normalAttr.SetFontWeight(wxFONTWEIGHT_NORMAL);
        normalAttr.SetTextColour(wxColour("#333333"));
        chatDisplay->SetDefaultStyle(normalAttr);
        chatDisplay->AppendText(message + "\n");
    }

    void AppendAssistantMessage(const std::string& message) {
        chatDisplay->AppendText("\n");
        wxTextAttr boldAttr;
        boldAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
        boldAttr.SetTextColour(wxColour("#00A779")); // Teal for assistant name
        chatDisplay->SetDefaultStyle(boldAttr);
        chatDisplay->AppendText("Dialogix: ");

        wxTextAttr normalAttr;
        normalAttr.SetFontWeight(wxFONTWEIGHT_NORMAL);
        normalAttr.SetTextColour(wxColour("#333333"));
        chatDisplay->SetDefaultStyle(normalAttr);
        chatDisplay->AppendText(message + "\n");
    }

    std::string GetChatGPTResponse(const std::string& userMessage) {
        const std::string apiKey = "sk-your-api-key"; // Replace with your actual API key
        const std::string url = "https://api.openai.com/v1/chat/completions";

        CURL* curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (!curl) return "Error initializing CURL!";

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());

        Json::Value root;
        root["model"] = "gpt-3.5-turbo";

        Json::Value messages(Json::arrayValue);
        Json::Value userMessageJson;
        userMessageJson["role"] = "user";
        userMessageJson["content"] = userMessage;
        messages.append(userMessageJson);
        root["messages"] = messages;

        Json::StreamWriterBuilder writer;
        std::string jsonData = Json::writeString(writer, root);
        std::string responseString;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return "No response from server.";
        }

        Json::CharReaderBuilder readerBuilder;
        Json::Value responseJson;
        std::istringstream s(responseString);
        std::string errs;
        if (!Json::parseFromStream(readerBuilder, s, &responseJson, &errs)) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return "Error parsing JSON response!";
        }
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        if (responseJson.isMember("choices") && responseJson["choices"].size() > 0) {
            return responseJson["choices"][0]["message"]["content"].asString();
        }
        return "No response from server.";
    }

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        ChatApp* chatFrame = new ChatApp("Dialogix");
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
