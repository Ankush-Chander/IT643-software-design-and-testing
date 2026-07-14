#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

class Solution {
    public:
    
    string vec2string(vector<int> vec){
        string result = "";
        for(auto it:vec){
            result += int2char(it);
        }
        return result;
    }
    pair<int, int> add2digits(int x, int y, int previous_carry){
        int sum = (x+y+previous_carry) %10;
        int carry = (x+y+previous_carry)/10;
        return make_pair(sum,carry);
    }
    int char2int(char& digit){
        return digit - '0';
    }
    
    char int2char(int& digit){
        return digit + '0';
    }
    string addStrings(string num1, string num2) {
            // reverse
            std::reverse(num1.begin(), num1.end());
            std::reverse(num2.begin(), num2.end());
            if(num1.length()<num2.length()){
                swap(num1, num2);
            }
            // add
            vector<int> res;
            int carry = 0;
            for(int i = 0; i < num1.length();i++){
                int d1 = char2int(num1[i]);
                int d2 = i<num2.length()?char2int(num2[i]): 0; 
                pair<int, int> result = this->add2digits(d1, d2, carry);
                carry = result.second;
                res.push_back(result.first);
                // cout << result.first << ", "<<result.second << endl;
                // int d2 = 
            }
            if(carry>0){
                res.push_back(carry);
            }
            // reverse
            reverse(res.begin(), res.end());
            return vec2string(res);
        }
    };
