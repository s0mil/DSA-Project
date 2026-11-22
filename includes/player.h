#ifndef PLAYER_H
#define PLAYER_H
#include "AccountManager.h"
#include "ranking.h"

class Player {
private:
    AccountManager account;
    Ranking ranking;
    string userName;
    int level;
    int keys;
    int score;
public:
    
    Player();

    void signUp();
    bool login();
    void logout();
    void changePassword();
    bool deleteAccount();
    void saveProgress();
    bool isLoggedIn();
    string getUsername();
    void setUserName(string name);
    void setLevel(int l);
    void setKeys(int k);
    void setScore(int s);
    int getLevel();
    int getKeys();
    int getScore();
    
};

#endif