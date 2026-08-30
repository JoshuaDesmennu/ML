#include "input_info.hpp"
InputInfo::InputInfo(){}
InputInfo::InputInfo(bool is_in_focus, std::string placeholder, std::string value) : is_in_focus(is_in_focus), placeholder(placeholder), value(value) {}