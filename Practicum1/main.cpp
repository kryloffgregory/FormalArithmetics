#include <iostream>
#include <CAutomat.h>
using namespace std;
bool isValid(const string &regExp) {
    int balance = 0;
    for(char lexem : regExp) {
            if(lexem == ' ')
                continue;
            if(lexem == '+') {
                    if(balance < 2)
                        return false;
                    else
                        balance--;
                } else if(lexem == '.') {
                    if(balance < 2)
                        return false;
                    else
                        balance--;
                } else if(lexem == '*') {
                    if(balance < 1)
                        return false;
                } else if(lexem == 'a' || lexem == 'b' || lexem == 'c' || lexem == '1'){
                    balance++;
                } else return false;
        }
    if(balance != 1)
        return false;
    else
        return true;
}

int main()
{
    string regExp;
    char symbol;
    int count;
    cin >> regExp >> symbol >> count;
    if(!isValid(regExp)) {
        cout << "Incorrect regex";
        return 0;
    }
    CAutomat G = CAutomat(regExp);
    cout << G.calculateMinWordLength(symbol, count);
    return 0;
}

