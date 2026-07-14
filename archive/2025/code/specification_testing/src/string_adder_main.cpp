#include "string_adder.h"


int main(int argc, char const *argv[])
{
    if(argc < 3){
        cout << "Usage: " << "./a.out num1 num2";
        return 1;
    }
    /* code */
    string num1 = argv[1];
    string num2 = argv[2];
    Solution sol = Solution();
    string result = sol.addStrings(num1, num2);
    // cout << "num1r: " << num1 << ", num2r = " << num2 <<", result =" << result;
    return 0;
}
