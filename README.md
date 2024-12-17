Dialogix
![Screenshot 2024-12-17 002227](https://github.com/user-attachments/assets/d82225be-f839-4953-ae28-376b90553fae)


Dialogix is a simple desktop chat application built with wxWidgets that integrates with the OpenAI ChatGPT API (using gpt-3.5-turbo) to provide dynamic responses to user queries. It displays a chat-like interface, allowing the user to type a message and receive a generated response from the language model.
Features

    Multi-Chat History: Create multiple chat sessions that you can revisit by selecting from a history list.
    Modern UI Design: Utilizes a clean, minimalistic interface with styled text, icons, and color-coded roles for user and AI responses.
    OpenAI Integration: Fetches responses from the OpenAI ChatGPT API for natural language interactions.

Prerequisites

    wxWidgets:
    Install wxWidgets following their official documentation.
    On Linux, you can often install with your package manager (e.g. sudo apt install libwxgtk3.0-gtk3-dev on Ubuntu).

    cURL:
    Ensure libcurl is installed. Most Linux systems have it by default. On Windows, install cURL and ensure it is accessible in your PATH or linked during compile time.

    JsonCpp:
    The code uses JsonCpp for JSON parsing. Make sure you have it installed and accessible for your compiler.

    C++ Compiler:
    A C++11-compatible compiler (such as g++, clang, or MSVC) is required.

    OpenAI API Key:
    You must have a valid OpenAI API key. Obtain one from https://platform.openai.com/account/api-keys.
    In the source code, replace "sk-your-api-key" with your actual API key.

Installation and Building

    Clone the Repository:

git clone https://github.com/yourusername/dialogix.git
cd dialogix

Configure Dependencies:

    Ensure wxWidgets, cURL, and JsonCpp libraries are installed and properly linked.

Set Your API Key: Open the main .cpp file (e.g., ChatApp.cpp) and find the line containing:

const std::string apiKey = "sk-your-api-key";

Replace "sk-your-api-key" with your actual OpenAI API key.

Build the Application: On Linux (example using g++):

g++ -std=c++11 -o Dialogix ChatApp.cpp `wx-config --cxxflags --libs` -lcurl -ljsoncpp

On Windows (example using MSVC and assuming wxWidgets, cURL, and JsonCpp are set up):

    cl /EHsc /FeDialogix.exe ChatApp.cpp /I"path_to_wx_include" /I"path_to_jsoncpp_include" /I"path_to_curl_include" /link "path_to_wx_libs" "path_to_curl_libs" "path_to_jsoncpp_libs"

    Adjust include and library paths as needed.

Running the Application

    After successful compilation, simply run:

./Dialogix

On Windows:

    Dialogix.exe

How It Works

    User Input:
    Type a message into the input field (with the placeholder text "type here to Dialogix") and press Enter or click Send.

    OpenAI Integration: When you send a message, the application sends your text to the OpenAI API using your API key. The returned response from the model (e.g., gpt-3.5-turbo) is displayed in the chat window as "Dialogix:" followed by the response text.

    Multiple Chats: Click Create chat to start a new conversation. Each chat is listed in the History panel on the left. Selecting a history item loads its conversation so you can continue from where you left off.

Customization

    Styling: You can adjust fonts, colors, and spacing within the source code, changing wxColour values, fonts, or layout sizers as desired.

    Model and Parameters: By default, this uses gpt-3.5-turbo. You can switch to another model by changing the model value in GetChatGPTResponse().

    Conversation Context: Currently, only the latest user message is sent. To provide more context, you could modify the code to include the entire conversation history in the messages array when calling the API.

Troubleshooting

    No Response From Server:
    Ensure you have a valid API key and a network connection. Check if your API key has access to the model and verify that the curl request succeeds.

    Linking Errors: If you encounter linking errors, double-check that all required libraries (wxWidgets, cURL, JsonCpp) are correctly installed and that you're passing the proper flags to your compiler.

License

This project is available under the MIT License. See the LICENSE file for more details.

By following the instructions above, you should be able to build and run Dialogix locally, enabling you to chat with the model directly from a desktop GUI application.
