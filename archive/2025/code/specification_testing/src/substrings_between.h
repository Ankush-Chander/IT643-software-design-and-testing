#include <iostream>
#include <string>
#include <vector>

typedef std::string string;
using namespace std;

vector<string> substringsBetween(const string& str,
                                           const string& open,
                                           const string& close) {
    // Preconditions
    if (str.empty() || open.empty() || close.empty()) {
        return {};  // Equivalent to null → return empty vector in C++
    }

    size_t strLen = str.length();
    size_t openLen = open.length();
    size_t closeLen = close.length();

    vector<string> result;
    size_t pos = 0;// keeps track of current position till where already searched

    while (pos < strLen - closeLen) { //
        // cout << "pos: " << pos << endl; 
        size_t start = str.find(open, pos); // find the first open tag from pos
        if (start == string::npos) {
            break; // no more open tags
        }

        start += openLen; // move past open tag
        size_t end = str.find(close, start); // find first close tag after start
        if (end == string::npos) {
            break; // no matching close tag
        }

        // Extract substring between open and close
        result.push_back(str.substr(start, end - start));

        // Move past the close tag
        pos = end + openLen;
        
        
    }

    // Return empty vector if nothing found (analogous to returning null in Java)
    if (result.empty()) {
        return {};
    }

    return result;
}


