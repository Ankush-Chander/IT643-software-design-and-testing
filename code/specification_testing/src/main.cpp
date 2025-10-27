#include "string_between.h"

int main(int argc, char* argv[]) {
    // collect from arguments
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <open> <close>\n";
        return 1;
    }
    string s = argv[1];
    string open = argv[2];
    string close = argv[3];

    std::vector<string> parts = substringsBetween(s, open, close);

    for (const auto& p : parts) {
        std::cout << p << "\n";
    }
    return 0;
}