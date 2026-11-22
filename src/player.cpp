#include "../includes/player.h"

Player::Player(){
    level = 0;
    keys = 5;
    score = 0;
}

void Player::signUp() {
    level = 0;
    keys = 5;
    score=0;
    account.signUp();

}

bool Player::login() {
    if(account.userLogin(this)) {
        return true;
    }
    return false;
}

void Player::logout() {
    account.saveProgress(this);
    account.logout();
    level = 0;
    keys = 5;
    score = 0;
}

void Player::changePassword() {
    account.changePassword();
}

bool Player::deleteAccount() {
    if(account.deleteAccount()) {
        if(!account.isLoggedIn()) {
            level = 0;
            keys = 5;
            score = 0;
        }
        return true;
    }
    return false;
}

void Player::saveProgress() {
    account.saveProgress(this);
}

bool Player::isLoggedIn() {
    return account.isLoggedIn();
}

string Player::getUsername() {
    return userName;
}

void Player::setUserName(string name){
    userName = name;
}



void Player::setLevel(int l) {
    level = l;
}

void Player::setKeys(int k) {
    keys = k;
}

void Player::setScore(int s) {
    score = s;
}

int Player::getLevel() {
    return level;
}

int Player::getKeys() {
    return keys;
}

int Player::getScore() {
    return score;
}