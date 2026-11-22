#include "../includes/AccountManager.h"
#include "../includes/player.h"
#include "../includes/OTP.h"
#include <cstdio>
#include <cctype>

AccountManager::AccountManager(){}
void AccountManager::encrypt(string &str) {
    const char* hex = "0123456789ABCDEF";
    string out = "";
    for (size_t i = 0; i < str.length(); i++) {
        unsigned char c = (unsigned char)(str[i] ^ 5);
        out += hex[c >> 4];
        out += hex[c & 15];
    }

    str = out;
}

bool AccountManager::verifyOTP(string email) {

    OTP otp;
    if(!otp.sendOTP(email)){
        cout << "OTP Email Could Not Be Sent. Configure SMTP environment variables." << endl;
        return false;
    }

    int enteredOTP;

    for(int i = 1; i <= 3; i++) {

        cout << "Enter OTP: ";
        cin >> enteredOTP;

        if(otp.verifyOTP(enteredOTP)) {
            cout << "Email Verified Successfully!" << endl;
            return true;
        }

        cout << "Incorrect OTP! Attempts left: " << 3 - i << endl;
    }

    cout << "OTP Verification Failed!" << endl;
    return false;
}
bool AccountManager :: isValidPassword(){
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    if(password.size() < 8){
        cout<<"Must contain atleast 8 characters"<<endl;
        return false;
    }

    for (char c : password) {
        unsigned char ch = (unsigned char)c;
        if (isupper(ch)) hasUpper = true;
        else if (islower(ch)) hasLower = true;
        else if (isdigit(ch)) hasDigit = true;
        else hasSpecial = true;
    }

    if(!hasUpper){
        cout<<"Must contain atleast One UpperCase"<<endl;
    }
    if(!hasLower){
        cout<<"Must contain atleast One LowerCase"<<endl;
    }
    if(!hasDigit){
        cout<<"Must contain atleast One Digit"<<endl;
    }
    if(!hasSpecial){
        cout<<"Must contain atleast One Special Character"<<endl;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

void AccountManager :: signUp(){

    cout<<"------Sign Up------"<<endl;

    while(1){
        cout<<"Enter Email"<<endl;
        cin>>email;
        cout<<"Enter User Name (no spaces):"<<endl;
        cin>>name;

        if(email.find('|') != string::npos || name.find('|') != string::npos){
            cout<<"The '|' character is not allowed"<<endl;
            continue;
        }

        ifstream readFile("SignUp.txt");

        int found=0;

        string e;
        string n;
        string rest;
        while(
            getline(readFile, e, '|') &&
            getline(readFile, n, '|') &&
            getline(readFile, rest)
        ){
            if(email==e){
                cout<<" Email Already Registered"<<endl;
                found=1;
                break;
            }
            if(name == n) {
                cout << "Username Already Exists!" << endl;
                found = 1;
                break;
            }
        }
        readFile.close();

        if(!found) break;
    }

    while(1){
        cout<<"Create Password"<<endl;
        password=getHiddenPassword();
        if(isValidPassword()) break;
    }
    encrypt(password);
    if(!verifyOTP(email)) return;
    ofstream writeFile("SignUp.txt", ios::app);
    if (!writeFile.is_open()) {
        cout << "File Opening Failed!" << endl;
        return;
    }
    writeFile << email << "|" << name << "|" << password << "|" << 0 << "|" << 5 << "|" << 0 <<endl;
    writeFile.close();
    cout << "Sign Up Successful!" << endl;
}

bool AccountManager::userLogin(Player *p) {
    fstream file("SignUp.txt", ios::in);

    if(!file) {
        cout << "Error Opening File" << endl;
        return false;
    }

    int attempts = 0;

    while(1) {

        string inpName;
        string inpPass;
        string savedLevel, savedKeys , savedscore;

        cout << "Enter Username :" << endl;
        cin >> inpName;

        cout << "Enter Password:" << endl;
        inpPass = getHiddenPassword();

        int found = 0;
        int correctPass = 0;
        encrypt(inpPass);
        file.clear();
        file.seekg(0, ios::beg);
        string n, e, pass;
        while(
            getline(file, e, '|') &&
            getline(file, n, '|') &&
            getline(file, pass, '|') &&
            getline(file, savedLevel, '|') &&
            getline(file, savedKeys, '|') &&
            getline(file, savedscore)
            ) {

            if(inpName == n) {
                p->setUserName(inpName);
                found = 1;
                if(inpPass == pass) {
                    p->setLevel(stoi(savedLevel));
                    p->setKeys(stoi(savedKeys));
                    p->setScore(stoi(savedscore));
                    cout << "Login Successful..." << endl;
                    correctPass = 1;

                    email = e;
                    name = n;
                    password = pass;
                }
                else {
                    cout << "Incorrect Password" << endl;
                }
                break;
            }
        }

        if(!found) {
            cout << "Username is not Registered" << endl;
            return false;
        }
        if(correctPass) {
            loginStatus = true;
            file.close();
            return true;
        }
        attempts++;
        if(attempts == 3) {
            file.close();
            cout << "Limit Exceeded!" << endl;
            return false;
        }
    }
}

void AccountManager::logout() {
    if(loginStatus) {
        loginStatus = false;
        email = "";
        name = "";
        password = "";
        cout << "Logout Successful!" << endl;

    }
    else {
        cout << "You are not logged in" << endl;
    }
}

bool AccountManager::isLoggedIn() {
    return loginStatus;
}
bool AccountManager::saveProgress(Player *p) {
    if(!loginStatus) return false;

    ifstream readFile("SignUp.txt");
    if(!readFile) return false;

    ofstream tempFile("temp.txt");
    if(!tempFile) return false;

    string e, n, pass, savedLevel, savedKeys, savedscore;

    while(
        getline(readFile, e, '|') &&
        getline(readFile, n, '|') &&
        getline(readFile, pass, '|') &&
        getline(readFile, savedLevel, '|') &&
        getline(readFile, savedKeys, '|') &&
        getline(readFile, savedscore)
    ){
        if(e == email) {
            tempFile << e << "|" << n << "|" << pass << "|"
                     << p->getLevel() << "|" << p->getKeys() << "|" << p->getScore() << endl;
        }
        else {
            tempFile << e << "|" << n << "|" << pass << "|"
                     << savedLevel << "|" << savedKeys << "|" << savedscore << endl;
        }
    }

    readFile.close();
    tempFile.close();

    remove("SignUp.txt");
    rename("temp.txt", "SignUp.txt");
    return true;
}

bool AccountManager::deleteAccount(){
    fstream file("SignUp.txt", ios::in);
    if(!file){
        cout<<"Error Opening File"<<endl;
        return false;
    }

    string target;
    if(loginStatus) {
        target = email;
    }
    else {
        cout<<"Enter Email:"<<endl;
        cin>>target;
    }

    if(!verifyOTP(target)) return false;

    string e, n, pass, savedLevel, savedKeys, savedscore;
    int found = 0;

    while(
        getline(file, e, '|') &&
        getline(file, n, '|') &&
        getline(file, pass, '|') &&
        getline(file, savedLevel, '|') &&
        getline(file, savedKeys, '|') &&
        getline(file, savedscore)
    ){
        if(target == e){
            found = 1;
            break;
        }
    }

    file.close();

    if(!found){
        cout<<"Email not registered"<<endl;
        return false;
    }

    ifstream readFile("SignUp.txt");
    ofstream tempFile("temp.txt");

    while(
        getline(readFile, e, '|') &&
        getline(readFile, n, '|') &&
        getline(readFile, pass, '|') &&
        getline(readFile, savedLevel, '|') &&
        getline(readFile, savedKeys, '|') &&
        getline(readFile, savedscore)
    ){
        if(target != e){
            tempFile << e << "|" << n << "|" << pass << "|"
                     << savedLevel << "|" << savedKeys << "|"
                     << savedscore << endl;
        }
    }

    readFile.close();
    tempFile.close();

    remove("SignUp.txt");
    rename("temp.txt", "SignUp.txt");

    if(loginStatus && target == email) {
        loginStatus = false;
        email = "";
        name = "";
        password = "";
    }

    return true;
}

void AccountManager::changePassword(){
    fstream file("SignUp.txt", ios::in);
    if(!file){
        cout<<"Error Opening File"<<endl;
        return;
    }

    string target;
    if(loginStatus) {
        target = email;
    }
    else {
        cout<<"Enter Email:"<<endl;
        cin>>target;
    }
    
    string e, n, pass, savedLevel, savedKeys, savedscore;
    int found = 0;

    while(
        getline(file, e, '|') &&
        getline(file, n, '|') &&
        getline(file, pass, '|') &&
        getline(file, savedLevel, '|') &&
        getline(file, savedKeys, '|') &&
        getline(file, savedscore)
    ){
        if(target == e){
            found = 1;
            break;
        }
    }

    file.close();

    if(!found){
        cout<<"Email not registered"<<endl;
        return;
    }

    if(!verifyOTP(target)) return;

    string newPass;
    string backup = password;
    while(1){
        cout<<"Enter New Password:"<<endl;
        newPass=getHiddenPassword();
        password = newPass;
        if(isValidPassword()) break;
    }
    password = backup;
    encrypt(newPass);

    ifstream readFile("SignUp.txt");
    ofstream tempFile("temp.txt");

    while(
        getline(readFile, e, '|') &&
        getline(readFile, n, '|') &&
        getline(readFile, pass, '|') &&
        getline(readFile, savedLevel, '|') &&
        getline(readFile, savedKeys, '|') &&
        getline(readFile, savedscore)
    ){
        if(target == e) {
            tempFile << e << "|" << n << "|" << newPass << "|"
                     << savedLevel << "|" << savedKeys << "|"
                     << savedscore << endl;
        }
        else {
            tempFile << e << "|" << n << "|" << pass << "|"
                     << savedLevel << "|" << savedKeys << "|"
                     << savedscore << endl;
        }
    }

    readFile.close();
    tempFile.close();

    remove("SignUp.txt");
    rename("temp.txt", "SignUp.txt");

    if(loginStatus && target == email) {
        password = newPass;
    }

    cout<<"Password Updated Successfully"<<endl;
}

string AccountManager::getUsername() {
    return name;
}
