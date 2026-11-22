#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include<iostream>
#include<fstream>
#include<string>
#include "hiddenpasswordmechanism.h"

class Player;

using namespace std;

class AccountManager {

    private:
    string email;
    string name;
    string password;

    void encrypt(string &str);
    bool isValidPassword();
    bool loginStatus = false;

    public:
    AccountManager();
    string getUsername();

    bool verifyOTP(string email);
    void signUp();
    bool userLogin(Player *p);
    void logout();
    void changePassword();
    bool deleteAccount();
    bool saveProgress(Player *p);
    bool isLoggedIn();

};

#endif
