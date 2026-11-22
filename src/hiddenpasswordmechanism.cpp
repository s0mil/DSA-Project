#include "../includes/hiddenpasswordmechanism.h"

string getHiddenPassword(){
    string password = "";
    char ch;

    while(true){
        ch = _getch();

        if(ch == 13){
            cout<<endl;
            break;
        }
        else if(ch == 8){
            if(password.length() > 0){
                password.pop_back();
                cout<<"\b \b";
            }
        }
        else{
            password += ch;
            cout<<"*";
        }
    }

    return password;
}