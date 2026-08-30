#include <iostream>

class InputInfo {
    public:
    bool is_in_focus = false;
    std::string placeholder = "Text";
    std::string value = "";

    InputInfo();
    InputInfo(bool is_in_focus, std::string placeholder, std::string value);
};
