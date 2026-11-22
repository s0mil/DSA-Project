#include "../includes/OTP.h"
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstdio>
#include <algorithm>
#include <filesystem>
#include <cctype>

OTP::OTP() : otp(0), lastError("") {
    static bool seeded = false;
    if (!seeded) { std::srand((unsigned)std::time(nullptr)); seeded = true; }
}

void OTP::generateOTP() { otp = std::rand() % 9000 + 1000; }

const std::string& OTP::getLastError() const { return lastError; }

bool OTP::sendOTP(std::string receiverEmail) {
    lastError.clear();
    if (otp == 0) generateOTP();

    const char* senderEnv = std::getenv("MAZE_SMTP_EMAIL");
    const char* passEnv = std::getenv("MAZE_SMTP_APP_PASSWORD");
    if (!senderEnv || !*senderEnv || !passEnv || !*passEnv) {
        lastError = "SMTP settings missing. Set both variables in the same PowerShell window, then restart the game.";
        return false;
    }

    std::string sender(senderEnv), appPass(passEnv);
    // Google displays App Passwords in groups; remove whitespace if copied that way.
    appPass.erase(std::remove_if(appPass.begin(), appPass.end(), [](unsigned char c){ return std::isspace(c); }), appPass.end());
    auto hasNewline = [](const std::string& s){ return s.find('\n') != std::string::npos || s.find('\r') != std::string::npos; };
    if (hasNewline(sender) || hasNewline(appPass) || hasNewline(receiverEmail)) {
        lastError = "SMTP values contain an invalid line break.";
        return false;
    }

    // curl.exe is included with modern Windows. Keep paths absolute so this works
    // when the project directory contains spaces.
    if (std::system("curl.exe --version >nul 2>&1") != 0) {
        lastError = "curl.exe was not found. Install/update Windows curl or run this on Windows 10/11.";
        return false;
    }

    std::filesystem::path root = std::filesystem::current_path();
    std::filesystem::path mailPath = root / "mail.txt";
    std::filesystem::path configPath = root / ".maze_smtp_config.tmp";
    std::filesystem::path logPath = root / "smtp_error.log";

    std::ofstream mail(mailPath, std::ios::binary | std::ios::trunc);
    if (!mail) { lastError = "Cannot create mail.txt. Run the game from the project folder."; return false; }
    mail << "To: " << receiverEmail << "\r\n"
         << "From: " << sender << "\r\n"
         << "Subject: Maze Lab - OTP Verification\r\n\r\n"
         << "Your Maze Lab verification code is: " << otp << "\r\n\r\n"
         << "This code is for account verification.\r\n";
    mail.close();

    // Escape values for curl's config-file quoted strings.
    auto escape = [](const std::string& s) {
        std::string out;
        for (char c : s) { if (c == '\\' || c == '"') out += '\\'; out += c; }
        return out;
    };
    auto configPathText = mailPath.string();
    auto slashify = [](std::string s) { for (char& c : s) if (c == '\\') c = '/'; return s; };
    std::ofstream cfg(configPath, std::ios::trunc);
    if (!cfg) { lastError = "Cannot create SMTP temporary configuration file."; return false; }
    cfg << "url = \"smtp://smtp.gmail.com:587\"\n"
        << "ssl-reqd\n"
        << "mail-from = \"" << escape(sender) << "\"\n"
        << "mail-rcpt = \"" << escape(receiverEmail) << "\"\n"
        << "upload-file = \"" << escape(slashify(configPathText)) << "\"\n"
        << "user = \"" << escape(sender + ":" + appPass) << "\"\n";
    cfg.close();

    std::string command = "curl.exe --config \"" + slashify(configPath.string()) + "\" --show-error --verbose --stderr \"" + slashify(logPath.string()) + "\"";
    int result = std::system(command.c_str());
    std::remove(configPath.string().c_str());
    if (result == 0) {
        std::remove(logPath.string().c_str());
        std::ofstream cleanup(mailPath, std::ios::trunc); cleanup.close();
        return true;
    }

    std::string detail;
    std::ifstream log(logPath);
    if (log) {
        std::string line;
        while (std::getline(log, line)) {
            if (line.find("error") != std::string::npos || line.find("Error") != std::string::npos || line.find("535") != std::string::npos || line.find("550") != std::string::npos) {
                detail = line;
                if (detail.size() > 100) detail.resize(100);
            }
        }
    }
    lastError = "Email failed. Check Gmail App Password/2-Step Verification and internet.";
    if (!detail.empty()) lastError += " Details: " + detail;
    lastError += " (smtp_error.log)";
    std::ofstream cleanup(mailPath, std::ios::trunc); cleanup.close();
    return false;
}

bool OTP::verifyOTP(int inOTP) { return inOTP == otp; }
