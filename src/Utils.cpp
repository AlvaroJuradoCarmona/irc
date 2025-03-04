#include "Utils.hpp"

Utils::Utils() {}

Utils::~Utils() {}

std::vector<std::string> Utils::split(const std::string &s, const char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim))
        if (!item.empty())
            elems.push_back(item);
    return elems;
}

bool Utils::isNumber(const std::string& s) {
    if (s == "-")
        return false;
    std::string::const_iterator it = s.begin();
    
    while (it != s.end() && (std::isdigit(*it) || (*it == '-' && it == s.begin())))
        ++it;
    return !s.empty() && it == s.end();
}