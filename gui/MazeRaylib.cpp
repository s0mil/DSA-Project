#include "raylib.h"
#include "../includes/Maze.h"
#include "../includes/MazeGenerator.h"
#include "../includes/MazeSolver.h"
#include "../includes/OTP.h"
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cctype>

const int SW = 1280, SH = 720;
Color BG = {9, 13, 24, 255}, PANEL = {17, 24, 39, 255}, PANEL2 = {24, 34, 52, 255}, ACC_CYAN = {58, 220, 238, 255}, ACC_PURPLE = {155, 112, 255, 255}, ACC_GREEN = {76, 220, 150, 255}, ACC_RED = {255, 95, 115, 255}, TEXT = {231, 239, 250, 255}, MUTED = {137, 154, 180, 255}, BORDER = {58, 76, 101, 255};
struct Button
{
    Rectangle r;
    const char *label;
};
bool Hit(Rectangle r) { return CheckCollisionPointRec(GetMousePosition(), r); }
bool Click(Button b) { return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && Hit(b.r); }
void Frame(Rectangle r, Color fill = PANEL, Color edge = BORDER)
{
    DrawRectangleRounded(r, .08f, 8, fill);
    DrawRectangleRoundedLines(r, .08f, 8, edge);
}
void Btn(Button b, bool active = false, bool danger = false)
{
    Color c = active ? ColorAlpha(ACC_CYAN, .14f) : PANEL2;
    if (Hit(b.r))
        c = ColorAlpha(danger ? ACC_RED : ACC_CYAN, .22f);
    DrawRectangleRounded(b.r, .15f, 8, c);
    DrawRectangleRoundedLines(b.r, .15f, 8, active ? ACC_CYAN : (danger ? ColorAlpha(ACC_RED, .65f) : BORDER));
    int fs = 16, w = MeasureText(b.label, fs);
    DrawText(b.label, (int)(b.r.x + b.r.width / 2 - w / 2), (int)(b.r.y + b.r.height / 2 - fs / 2), fs, TEXT);
}
std::string Encode(std::string s)
{
    const char *h = "0123456789ABCDEF";
    std::string o;
    for (unsigned char c : s)
    {
        c ^= 5;
        o += h[c >> 4];
        o += h[c & 15];
    }
    return o;
}
std::string Decode(std::string s)
{
    std::string o;
    auto v = [](char c)
    {if(c>='0'&&c<='9')return c-'0';if(c>='A'&&c<='F')return c-'A'+10;if(c>='a'&&c<='f')return c-'a'+10;return 0; };
    for (size_t i = 0; i + 1 < s.size(); i += 2)
        o += (char)(((v(s[i]) << 4) | v(s[i + 1])) ^ 5);
    return o;
}
std::string dataFile() { return "SignUp.txt"; }
struct UserRecord
{
    std::string email, user, pass;
    int level = 0, keys = 5, score = 0;
};
std::vector<UserRecord> readUsers()
{
    std::vector<UserRecord> v;
    std::ifstream f(dataFile());
    std::string e, u, p, l, k, s;
    while (std::getline(f, e, '|') && std::getline(f, u, '|') && std::getline(f, p, '|') && std::getline(f, l, '|') && std::getline(f, k, '|') && std::getline(f, s))
    {
        try
        {
            v.push_back({e, u, p, std::stoi(l), std::stoi(k), std::stoi(s)});
        }
        catch (...)
        {
        }
    }
    return v;
}
bool saveUsers(const std::vector<UserRecord> &v)
{
    std::ofstream f(dataFile(), std::ios::trunc);
    if (!f)
        return false;
    for (auto &r : v)
        f << r.email << '|' << r.user << '|' << r.pass << '|' << r.level << '|' << r.keys << '|' << r.score << '\n';
    return (bool)f;
}
int byUser(const std::vector<UserRecord> &v, std::string s)
{
    for (int i = 0; i < (int)v.size(); i++)
        if (v[i].user == s)
            return i;
    return -1;
}
int byEmail(const std::vector<UserRecord> &v, std::string s)
{
    for (int i = 0; i < (int)v.size(); i++)
        if (v[i].email == s)
            return i;
    return -1;
}
void rankMerge(std::vector<UserRecord> &a, int l, int m, int r)
{
    std::vector<UserRecord> x(a.begin() + l, a.begin() + m + 1), y(a.begin() + m + 1, a.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < (int)x.size() && j < (int)y.size())
        a[k++] = (x[i].score >= y[j].score) ? x[i++] : y[j++];
    while (i < (int)x.size())
        a[k++] = x[i++];
    while (j < (int)y.size())
        a[k++] = y[j++];
}
void rankSort(std::vector<UserRecord> &a, int l, int r)
{
    if (l < r)
    {
        int m = (l + r) / 2;
        rankSort(a, l, m);
        rankSort(a, m + 1, r);
        rankMerge(a, l, m, r);
    }
}
bool emailValid(const std::string &e)
{
    size_t a = e.find('@'), d = e.find('.', a == std::string::npos ? 0 : a);
    return a > 0 && a != std::string::npos && d != std::string::npos && d > a + 1 && d + 1 < e.size() && e.find('|') == std::string::npos;
}
bool passValid(const std::string &p, std::string &why)
{
    bool u = 0, l = 0, d = 0, s = 0;
    if (p.size() < 8)
    {
        why = "Password must be at least 8 characters.";
        return false;
    }
    for (unsigned char c : p)
    {
        if (std::isupper(c))
            u = 1;
        else if (std::islower(c))
            l = 1;
        else if (std::isdigit(c))
            d = 1;
        else
            s = 1;
    }
    if (!(u && l && d && s))
    {
        why = "Password needs uppercase, lowercase, digit and special character.";
        return false;
    }
    return true;
}

class MazeGUI
{
    enum Screen
    {
        WELCOME,
        OTP_VIEW,
        SET_PASS,
        MENU,
        PLAY,
        INSTRUCTIONS,
        RANKING,
        WIN_VIEW,
        DEMO_FINISH
    };
    enum Task
    {
        NO_TASK,
        SIGNUP_TASK,
        CHANGE_TASK,
        DELETE_TASK
    };
    Screen screen = WELCOME;
    Task task = NO_TASK;
    std::string form = "", email, user, pass, confirm, otpInput, newPass, newConfirm, msg, activeUser;
    int focus = 0, failedLogins = 0, otpAttempts = 0, rankOffset = 0;
    bool replaySelect = false;
    int level = 1, playerRow = 1, playerCol = 1, moves = 0, minMoves = 0, keys = 5, score = 0, levelsDone = 0;
    Maze maze{9};
    MazeGenerator *generator = nullptr;
    MazeSolver *solver = nullptr;
    bool pathVisible = false, won = false, isDemo = false, soundOn = true;
    int hintRow = -1, hintCol = -1, cell = 30, gridX = 0, gridY = 0;
    float demoTick = 0;
    OTP otp;
    Sound moveSound{}, clickSound{}, wallSound{}, hintSound{}, winSound{};
    Music ambience{};
    bool audio = false;
    void play(Sound &s)
    {
        if (soundOn && audio)
            PlaySound(s);
    }
    void toggleSound()
    {
        soundOn = !soundOn;
        if (audio)
        {
            if (soundOn)
                PlayMusicStream(ambience);
            else
                StopMusicStream(ambience);
        }
    }
    void clearInputs()
    {
        email.clear();
        user.clear();
        pass.clear();
        confirm.clear();
        otpInput.clear();
        newPass.clear();
        newConfirm.clear();
        focus = 0;
    }
    void requestOTP(Task t, std::string target)
    {
        task = t;
        email = target;
        otpInput.clear();
        otpAttempts = 0;
        otp.generateOTP();
        bool sent = otp.sendOTP(target);
        msg = sent ? "OTP sent. Check your email and enter the code." : otp.getLastError();
        screen = OTP_VIEW;
        focus = 0;
    }
    void createMaze(int lv)
    {
        level = lv;
        int n = 9 + 2 * (lv - 1);
        maze.setSize(n);
        delete generator;
        delete solver;
        generator = new MazeGenerator(maze);
        generator->generateMaze(maze, 1, 1);
        playerRow = maze.getStartingRow();
        playerCol = maze.getStartingCol();
        moves = 0;
        pathVisible = false;
        won = false;
        hintRow = hintCol = -1;
        solver = new MazeSolver(maze);
        solver->solve();
        minMoves = std::max(0, solver->getPathSize() - 1);
        cell = std::max(8, std::min(34, (int)std::min(620.0 / n, 440.0 / n)));
        int total = cell * n;
        gridX = 24 + (690 - total) / 2;
        gridY = 178 + (460 - total) / 2;
    }
    void persist()
    {
        auto v = readUsers();
        int i = byUser(v, activeUser);
        if (i >= 0)
        {
            v[i].level = levelsDone;
            v[i].keys = keys;
            v[i].score = score;
            saveUsers(v);
        }
    }
    void login()
    {
        auto v = readUsers();
        int i = byUser(v, user);
        if (i >= 0 && Decode(v[i].pass) == pass)
        {
            activeUser = v[i].user;
            levelsDone = v[i].level;
            keys = v[i].keys;
            score = v[i].score;
            failedLogins = 0;
            createMaze(1);
            screen = MENU;
            msg = "Login successful.";
            clearInputs();
            form = "";
        }
        else
        {
            failedLogins++;
            msg = i < 0 ? "Username is not registered." : "Incorrect password.";
            if (failedLogins >= 3)
            {
                failedLogins = 0;
                form = "";
                clearInputs();
                msg = "Login limit reached. Try again.";
            }
        }
    }
    void signUpSubmit()
    {
        if (!emailValid(email))
        {
            msg = "Enter a valid email.";
            return;
        }
        if (user.empty() || user.find('|') != std::string::npos || user.find(' ') != std::string::npos)
        {
            msg = "Username cannot be empty, contain spaces or |.";
            return;
        }
        auto v = readUsers();
        if (byEmail(v, email) >= 0)
        {
            msg = "Email already registered.";
            return;
        }
        if (byUser(v, user) >= 0)
        {
            msg = "Username already exists.";
            return;
        }
        std::string why;
        if (!passValid(pass, why))
        {
            msg = why;
            return;
        }
        if (pass != confirm)
        {
            msg = "Passwords do not match.";
            return;
        }
        requestOTP(SIGNUP_TASK, email);
    }
    void otpSubmit()
    {
        if (otpInput.size() != 4)
        {
            msg = "Enter the four-digit code.";
            return;
        }
        int code = -1;
        try
        {
            code = std::stoi(otpInput);
        }
        catch (...)
        {
            msg = "OTP must be numeric.";
            return;
        }
        if (!otp.verifyOTP(code))
        {
            otpAttempts++;
            msg = "Incorrect OTP. Attempts left: " + std::to_string(3 - otpAttempts);
            otpInput.clear();
            if (otpAttempts >= 3)
            {
                screen = WELCOME;
                form = "";
                task = NO_TASK;
                clearInputs();
                msg = "OTP verification failed after 3 attempts.";
            }
            return;
        }
        auto v = readUsers();
        if (task == SIGNUP_TASK)
        {
            if (byEmail(v, email) >= 0 || byUser(v, user) >= 0)
            {
                msg = "Account already exists.";
                screen = WELCOME;
                form = "";
                return;
            }
            v.push_back({email, user, Encode(pass), 0, 5, 0});
            if (saveUsers(v))
            {
                screen = WELCOME;
                form = "";
                msg = "Signup successful. Please login.";
                clearInputs();
            }
            else
                msg = "Could not save account.";
        }
        else if (task == CHANGE_TASK)
        {
            screen = SET_PASS;
            focus = 0;
            msg = "Verified. Set your new password.";
        }
        else if (task == DELETE_TASK)
        {
            int i = byEmail(v, email);
            if (i < 0)
            {
                screen = WELCOME;
                form = "";
                msg = "Email not registered.";
            }
            else
            {
                v.erase(v.begin() + i);
                if (saveUsers(v))
                {
                    screen = WELCOME;
                    form = "";
                    msg = "Account deleted successfully.";
                    clearInputs();
                }
                else
                    msg = "Account deletion failed.";
            }
        }
        task = NO_TASK;
    }
    void changeSubmit()
    {
        auto v = readUsers();
        if (!emailValid(email) || byEmail(v, email) < 0)
        {
            msg = "Registered email not found.";
            return;
        }
        requestOTP(CHANGE_TASK, email);
    }
    void deleteSubmit()
    {
        auto v = readUsers();
        if (!emailValid(email) || byEmail(v, email) < 0)
        {
            msg = "Registered email not found.";
            return;
        }
        requestOTP(DELETE_TASK, email);
    }
    void newPassSubmit()
    {
        std::string why;
        if (!passValid(newPass, why))
        {
            msg = why;
            return;
        }
        if (newPass != newConfirm)
        {
            msg = "Passwords do not match.";
            return;
        }
        auto v = readUsers();
        int i = byEmail(v, email);
        if (i < 0)
        {
            screen = WELCOME;
            form = "";
            msg = "Email not registered.";
            return;
        }
        v[i].pass = Encode(newPass);
        if (saveUsers(v))
        {
            screen = WELCOME;
            form = "";
            msg = "Password changed successfully.";
            clearInputs();
        }
        else
            msg = "Could not update password.";
    }
    void field(Rectangle r, const char *label, std::string &v, bool focused, bool mask = false)
    {
        DrawText(label, (int)r.x, (int)r.y - 20, 13, MUTED);
        DrawRectangleRounded(r, .12f, 6, focused ? ColorAlpha(ACC_CYAN, .07f) : PANEL2);
        DrawRectangleRoundedLines(r, .12f, 6, focused ? ACC_CYAN : BORDER);
        std::string s = mask ? std::string(v.size(), '*') : v;
        DrawText(s.c_str(), (int)r.x + 12, (int)r.y + 12, 17, TEXT);
        if (focused && ((int)(GetTime() * 2) % 2 == 0))
        {
            int x = (int)r.x + 12 + MeasureText(s.c_str(), 17);
            DrawLine(x, (int)r.y + 10, x, (int)r.y + 34, ACC_CYAN);
        }
    }
    void welcomeDraw()
    {
        BeginDrawing();
        ClearBackground(BG);
        DrawRectangleGradientV(0, 0, SW, SH, {12, 18, 31, 255}, {7, 10, 18, 255});
        for (int i = 0; i < 40; i++)
            DrawCircle((i * 89 + 43) % SW, (i * 61 + 20) % SH, 1, ColorAlpha(ACC_CYAN, .1f));
        DrawText("MAZE", 82, 54, 42, ACC_CYAN);
        DrawText("LAB", 200, 54, 42, TEXT);
        DrawText("DSA PROJECT  |  RECURSIVE GENERATION + BACKTRACKING", 86, 105, 14, MUTED);
        Frame({365, 135, 550, 545}, {17, 25, 41, 255}, ColorAlpha(ACC_CYAN, .4f));
        DrawText("WELCOME TO MAZE LAB", 407, 165, 24, TEXT);
        DrawText("Terminal account menu", 407, 200, 15, MUTED);
        Button a = {{407, 235, 220, 50}, "Sign Up"}, b = {{655, 235, 220, 50}, "Login"}, c = {{407, 300, 220, 50}, "Change Password"}, d = {{655, 300, 220, 50}, "Delete Account"}, e = {{407, 365, 468, 50}, "Exit"};
        Btn(a);
        Btn(b, true);
        Btn(c);
        Btn(d, false, true);
        Btn(e, false, true);
        if (Click(a))
        {
            form = "signup";
            clearInputs();
            msg = "";
        }
        if (Click(b))
        {
            form = "login";
            clearInputs();
            msg = "";
        }
        if (Click(c))
        {
            form = "change";
            clearInputs();
            msg = "";
        }
        if (Click(d))
        {
            form = "delete";
            clearInputs();
            msg = "";
        }
        if (Click(e))
            CloseWindow();
        if (form != "")
        {
            Frame({407, 430, 468, 225}, PANEL2, BORDER);
            if (form == "signup")
            {
                DrawText("SIGN UP", 423, 442, 16, ACC_CYAN);
                Rectangle r1 = {423, 476, 210, 38}, r2 = {645, 476, 214, 38}, r3 = {423, 540, 210, 38}, r4 = {645, 540, 214, 38};
                field(r1, "EMAIL", email, focus == 0);
                field(r2, "USERNAME", user, focus == 1);
                field(r3, "PASSWORD", pass, focus == 2, true);
                field(r4, "CONFIRM PASSWORD", confirm, focus == 3, true);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    if (Hit(r1))
                        focus = 0;
                    else if (Hit(r2))
                        focus = 1;
                    else if (Hit(r3))
                        focus = 2;
                    else if (Hit(r4))
                        focus = 3;
                }
                Button go = {{423, 598, 436, 42}, "Send Email OTP"};
                Btn(go, true);
                if (Click(go))
                    signUpSubmit();
            }
            else if (form == "login")
            {
                DrawText("LOGIN", 423, 442, 16, ACC_CYAN);
                Rectangle r1 = {423, 482, 210, 42}, r2 = {645, 482, 214, 42};
                field(r1, "USERNAME", user, focus == 0);
                field(r2, "PASSWORD", pass, focus == 1, true);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    if (Hit(r1))
                        focus = 0;
                    else if (Hit(r2))
                        focus = 1;
                }
                Button go = {{423, 554, 436, 50}, "Login"};
                Btn(go, true);
                if (Click(go))
                    login();
            }
            else
            {
                DrawText(form == "change" ? "CHANGE PASSWORD" : "DELETE ACCOUNT", 423, 442, 16, ACC_CYAN);
                Rectangle r1 = {423, 482, 436, 42};
                field(r1, "REGISTERED EMAIL", email, focus == 0);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && Hit(r1))
                    focus = 0;
                Button go = {{423, 554, 436, 50}, form == "change" ? "Send Verification OTP" : "Send Delete OTP"};
                Btn(go, true, form == "delete");
                if (Click(go))
                {
                    if (form == "change")
                        changeSubmit();
                    else
                        deleteSubmit();
                }
            }
        }
        if (!msg.empty())
            DrawText(msg.c_str(), 407, 665, 13, (msg.find("successful") != std::string::npos || msg.find("Signup") != std::string::npos || msg.find("deleted") != std::string::npos || msg.find("changed") != std::string::npos) ? ACC_GREEN : ACC_RED);
        DrawText("Compatible with terminal accounts stored in SignUp.txt", 82, 690, 13, MUTED);
        EndDrawing();
    }
    void otpDraw()
    {
        BeginDrawing();
        ClearBackground(BG);
        Frame({390, 185, 500, 350}, {17, 25, 41, 255}, ColorAlpha(ACC_CYAN, .4f));
        DrawText("EMAIL VERIFICATION", 432, 225, 25, TEXT);
        if (msg.size() < 58)
            DrawText(msg.c_str(), 432, 270, 15, MUTED);
        else
        {
            DrawText("Email sending failed:", 432, 266, 14, ACC_RED);
            DrawText(msg.c_str(), 432, 285, 12, MUTED);
        }
        Rectangle r = {432, 320, 416, 48};
        field(r, "4-DIGIT CODE", otpInput, focus == 0);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && Hit(r))
            focus = 0;
        Button a = {{432, 390, 200, 48}, "Verify"}, res = {{648, 390, 200, 48}, "Resend OTP"}, b = {{432, 450, 416, 40}, "Cancel"};
        Btn(a, true);
        Btn(res);
        Btn(b);
        if (Click(a))
            otpSubmit();
        if (Click(res))
        {
            otp.generateOTP();
            bool sent = otp.sendOTP(email);
            otpAttempts = 0;
            otpInput.clear();
            msg = sent ? "A new OTP was sent." : otp.getLastError();
        }
        if (Click(b))
        {
            screen = WELCOME;
            form = "";
            task = NO_TASK;
            clearInputs();
        }
        EndDrawing();
    }
    void newPassDraw()
    {
        BeginDrawing();
        ClearBackground(BG);
        Frame({390, 185, 500, 350}, {17, 25, 41, 255}, ColorAlpha(ACC_CYAN, .4f));
        DrawText("SET NEW PASSWORD", 432, 220, 24, TEXT);
        DrawText(msg.c_str(), 432, 265, 14, MUTED);
        Rectangle a = {432, 300, 416, 44}, b = {432, 365, 416, 44};
        field(a, "NEW PASSWORD", newPass, focus == 0, true);
        field(b, "CONFIRM PASSWORD", newConfirm, focus == 1, true);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (Hit(a))
                focus = 0;
            else if (Hit(b))
                focus = 1;
        }
        Button go = {{432, 425, 416, 48}, "Update Password"};
        Btn(go, true);
        if (Click(go))
            newPassSubmit();
        EndDrawing();
    }
    void header(const char *sub)
    {
        Frame({24, 20, 1232, 78}, {17, 24, 39, 255}, BORDER);
        DrawText("MAZE", 48, 34, 29, ACC_CYAN);
        DrawText("LAB", 128, 34, 29, TEXT);
        DrawText(sub, 48, 69, 12, MUTED);
        DrawText(TextFormat("PLAYER %s", activeUser.c_str()), 760, 42, 15, ACC_CYAN);
        DrawText(TextFormat("SCORE %d  KEYS %d  COMPLETED %d/10", score, keys, levelsDone), 920, 42, 14, TEXT);
    }
    void menuDraw()
    {
        BeginDrawing();
        ClearBackground(BG);
        DrawRectangleGradientV(0, 0, SW, SH, {13, 19, 32, 255}, {7, 10, 18, 255});
        header("GAME MENU  /  TERMINAL OPTIONS");
        Frame({24, 118, 1232, 550}, PANEL, BORDER);
        DrawText("MAIN MENU", 54, 145, 22, TEXT);
        DrawText("Select an option from the original terminal menu", 54, 177, 14, MUTED);
        Button a = {{54, 220, 355, 65}, "Start / Select Level"}, b = {{440, 220, 355, 65}, "Next Level"}, c = {{826, 220, 355, 65}, "Replay Level"}, d = {{54, 310, 355, 65}, "Demo Mode"}, e = {{440, 310, 355, 65}, "Instructions"}, f = {{826, 310, 355, 65}, "Global Ranking"}, g = {{54, 400, 355, 65}, "Logout"};
        Btn(a, true);
        Btn(b);
        Btn(c);
        Btn(d);
        Btn(e);
        Btn(f);
        Btn(g, false, true);
        DrawText(TextFormat("Progress: completed %d/10   |   Next unlock: %d", levelsDone, std::min(10, levelsDone + 1)), 54, 490, 18, ACC_CYAN);
        DrawText("Select a level (locked levels disabled)", 54, 525, 15, TEXT);
        for (int i = 1; i <= 10; i++)
        {
            Rectangle r = {(float)(54 + (i - 1) * 108), 558, 92, 48};
            Button lv = {r, TextFormat("LEVEL %02d", i)};
            Btn(lv, i == level, i > levelsDone + 1);
            DrawText(i <= levelsDone ? "DONE" : i == levelsDone + 1 ? "OPEN"
                                                                    : "LOCKED",
                     (int)r.x + 10, 612, 10, i <= levelsDone ? ACC_GREEN : i == levelsDone + 1 ? ACC_CYAN
                                                                                               : MUTED);
            if (Click(lv) && ((replaySelect && i <= levelsDone) || (!replaySelect && i <= levelsDone + 1)))
            {
                replaySelect = false;
                createMaze(i);
                screen = PLAY;
            }
        }
        if (Click(a))
        {
            createMaze(std::min(10, levelsDone + 1));
            screen = PLAY;
        }
        if (Click(b))
        {
            if (levelsDone >= 10)
                msg = "All 10 levels completed.";
            else if (level > levelsDone)
                msg = "Complete the current level first.";
            else
            {
                createMaze(std::min(10, level + 1));
                screen = PLAY;
            }
        }
        if (Click(c))
        {
            if (levelsDone == 0)
                msg = "No levels unlocked to replay.";
            else
            {
                replaySelect = true;
                msg = "Click a completed level below to replay it.";
            }
        }
        if (Click(d))
            demoStart();
        if (Click(e))
            screen = INSTRUCTIONS;
        if (Click(f))
            screen = RANKING;
        if (Click(g))
        {
            persist();
            activeUser.clear();
            form = "";
            replaySelect = false;
            screen = WELCOME;
            msg = "";
            clearInputs();
        }
        EndDrawing();
    }
    void mazeDraw()
    {
        int n = maze.getSize(), total = cell * n;
        Frame({(float)gridX - 14, (float)gridY - 14, (float)total + 28, (float)total + 28}, {15, 22, 36, 255}, ColorAlpha(ACC_CYAN, .3f));
        for (int r = 0; r < n; r++)
            for (int c = 0; c < n; c++)
            {
                Rectangle q = {(float)(gridX + c * cell), (float)(gridY + r * cell), (float)cell - 1, (float)cell - 1};
                DrawRectangleRec(q, maze.getMaze()[r][c] ? Color{39, 49, 69, 255} : Color{21, 29, 44, 255});
                if (pathVisible)
                    for (int k = 0; k < solver->getPathSize(); k++)
                        if (solver->getPathRow(k) == r && solver->getPathCol(k) == c)
                            DrawRectangleRec(q, ColorAlpha(ACC_PURPLE, .5f));
                if (r == maze.getStartingRow() && c == maze.getStartingCol())
                    DrawRectangleRec(q, ColorAlpha(ACC_GREEN, .78f));
                if (r == maze.getEndingRow() && c == maze.getEndingCol())
                    DrawRectangleRec(q, ColorAlpha(ACC_RED, .8f));
                if (r == hintRow && c == hintCol)
                    DrawRectangleRec(q, ColorAlpha(ACC_GREEN, .65f));
                if (r == playerRow && c == playerCol)
                {
                    DrawCircle((int)(q.x + q.width / 2), (int)(q.y + q.height / 2), cell * .30f, ACC_CYAN);
                    DrawCircle((int)(q.x + q.width / 2), (int)(q.y + q.height / 2), cell * .12f, WHITE);
                }
            }
    }
    void move(int dr, int dc)
    {
        if (won || isDemo)
            return;
        int nr = playerRow + dr, nc = playerCol + dc, n = maze.getSize();
        if (nr >= 0 && nc >= 0 && nr < n && nc < n && maze.getMaze()[nr][nc] == 0)
        {
            playerRow = nr;
            playerCol = nc;
            moves++;
            play(moveSound);
            if (playerRow == maze.getEndingRow() && playerCol == maze.getEndingCol())
            {
                won = true;
                levelComplete();
            }
        }
        else
            play(wallSound);
    }
    void hint()
    {
        if (keys <= 0)
        {
            msg = "No Hint Keys Available!";
            return;
        }
        char h = solver->getHint(playerRow, playerCol);
        if (h == 'E')
        {
            msg = "Hint Not Available From This Position!";
            return;
        }
        keys--;
        hintRow = playerRow;
        hintCol = playerCol;
        if (h == 'U')
            hintRow--;
        else if (h == 'D')
            hintRow++;
        else if (h == 'L')
            hintCol--;
        else if (h == 'R')
            hintCol++;
        msg = std::string("Next Move: ") + (h == 'U' ? "UP" : h == 'D' ? "DOWN"
                                                          : h == 'L'   ? "LEFT"
                                                                       : "RIGHT");
        persist();
        play(hintSound);
    }
    void path()
    {
        if (!pathVisible)
        {
            if (keys < 3)
            {
                msg = "Not Enough Keys! Show Path costs 3 keys.";
                return;
            }
            keys -= 3;
            msg = "Full solution path revealed. 3 keys used.";
            persist();
        }
        pathVisible = !pathVisible;
        if (!pathVisible)
            msg = "Solution path hidden.";
    }
    void levelComplete()
    {
        int earned = moves > 0 ? (minMoves * 100) / moves : 0;
        score += earned;
        keys += 2;
        levelsDone = std::max(levelsDone, level);
        persist();
        msg = TextFormat("Level %d complete! Score earned: %d", level, earned);
        screen = WIN_VIEW;
        play(winSound);
    }
    void playDraw()
    {
        BeginDrawing();
        ClearBackground(BG);
        DrawRectangleGradientV(0, 0, SW, SH, {13, 19, 32, 255}, {7, 10, 18, 255});
        Frame({24, 20, 1232, 78}, {17, 24, 39, 255}, BORDER);
        DrawText("MAZE", 48, 34, 29, ACC_CYAN);
        DrawText("LAB", 128, 34, 29, TEXT);
        DrawText("GAMEPLAY / WASD OR ARROW KEYS", 48, 69, 12, MUTED);
        DrawText(TextFormat("LEVEL %02d", level), 680, 43, 18, TEXT);
        DrawText(TextFormat("MOVES %03d", moves), 805, 43, 18, ACC_CYAN);
        DrawText(TextFormat("KEYS %d", keys), 950, 43, 18, ACC_GREEN);
        Button menu = {{1090, 32, 140, 42}, "Game Menu"};
        Btn(menu);
        if (Click(menu))
        {
            persist();
            screen = MENU;
        }
        Frame({24, 118, 690, 550}, PANEL, BORDER);
        DrawText("MAZE VIEW", 48, 137, 18, TEXT);
        DrawText(isDemo ? "DEMO MODE: watch the recursive solver." : "Reach O. H = Hint, P = Show Path, Q = Quit Level.", 48, 160, 13, MUTED);
        mazeDraw();
        if (isDemo)
        {
            Frame({730, 118, 526, 550}, PANEL, BORDER);
            DrawText("DEMO MODE", 756, 137, 18, TEXT);
            DrawText("Recursive solver playback", 756, 170, 15, ACC_CYAN);
            DrawText("Maze size: 7 x 7", 756, 205, 16, TEXT);
            Button stop = {{756, 250, 462, 50}, "Return to Game Menu"};
            Btn(stop, true);
            if (Click(stop))
            {
                isDemo = false;
                screen = MENU;
            }
            Frame({24, 680, 1232, 28}, {17, 24, 39, 255}, BORDER);
            DrawText("DEMO: automatic path playback", 48, 688, 13, ACC_CYAN);
            EndDrawing();
            return;
        }
        Frame({730, 118, 526, 550}, PANEL, BORDER);
        DrawText("CONTROL CENTER", 756, 137, 18, TEXT);
        Button h = {{756, 190, 218, 46}, "Hint (-1 key)"}, p = {{988, 190, 230, 46}, pathVisible ? "Hide Path" : "Show Path (-3 keys)"}, r = {{756, 250, 218, 46}, "Reset Position"}, n = {{988, 250, 230, 46}, "Generate New Maze"}, q = {{756, 310, 218, 46}, "Quit Level"}, s = {{988, 310, 230, 46}, soundOn ? "Sound ON" : "Sound OFF"};
        Btn(h);
        Btn(p, pathVisible);
        Btn(r);
        Btn(n);
        Btn(q, false, true);
        Btn(s, soundOn);
        Frame({756, 380, 462, 110}, PANEL2, BORDER);
        DrawText("LEVEL STATS", 775, 396, 14, TEXT);
        DrawText(TextFormat("Maze %dx%d  |  Minimum moves %d", maze.getSize(), maze.getSize(), minMoves), 775, 425, 15, MUTED);
        DrawText(TextFormat("Your moves %d  |  Hint keys %d", moves, keys), 775, 452, 15, ACC_CYAN);
        Frame({756, 508, 462, 130}, PANEL2, BORDER);
        DrawText("LEGEND", 775, 524, 14, TEXT);
        DrawCircle(790, 558, 7, ACC_CYAN);
        DrawText("Player", 805, 550, 14, MUTED);
        DrawRectangle(888, 551, 14, 14, ACC_GREEN);
        DrawText("Start", 909, 550, 14, MUTED);
        DrawRectangle(976, 551, 14, 14, ACC_RED);
        DrawText("Exit", 997, 550, 14, MUTED);
        DrawRectangle(1052, 551, 14, 14, {39, 49, 69, 255});
        DrawText("Wall", 1073, 550, 14, MUTED);
        DrawRectangle(775, 590, 14, 14, ACC_PURPLE);
        DrawText("Path", 797, 588, 14, MUTED);
        DrawText("H Hint  P Path  R Reset  Q Quit  M Sound", 775, 616, 12, MUTED);
        Frame({24, 680, 1232, 28}, {17, 24, 39, 255}, BORDER);
        DrawText(msg.c_str(), 48, 688, 13, msg.empty() ? MUTED : ACC_CYAN);
        if (Click(h))
            hint();
        if (Click(p))
            path();
        if (Click(r))
        {
            playerRow = maze.getStartingRow();
            playerCol = maze.getStartingCol();
            moves = 0;
            won = false;
            hintRow = hintCol = -1;
            msg = "Position reset.";
        }
        if (Click(n))
        {
            createMaze(level);
            msg = "New maze generated.";
        }
        if (Click(q))
        {
            persist();
            screen = MENU;
        }
        if (Click(s))
            toggleSound();
        if (!isDemo)
        {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
                move(-1, 0);
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
                move(1, 0);
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
                move(0, -1);
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
                move(0, 1);
            if (IsKeyPressed(KEY_H))
                hint();
            if (IsKeyPressed(KEY_P))
                path();
            if (IsKeyPressed(KEY_R))
            {
                playerRow = maze.getStartingRow();
                playerCol = maze.getStartingCol();
                moves = 0;
                hintRow = hintCol = -1;
            }
            if (IsKeyPressed(KEY_Q))
            {
                persist();
                screen = MENU;
            }
            if (IsKeyPressed(KEY_M))
                toggleSound();
        }
        EndDrawing();
    }
    void demoStart()
    {
        isDemo = true;
        createMaze(1);
        maze.setSize(7);
        delete generator;
        delete solver;
        generator = new MazeGenerator(maze);
        generator->generateMaze(maze, 1, 1);
        playerRow = maze.getStartingRow();
        playerCol = maze.getStartingCol();
        solver = new MazeSolver(maze);
        solver->solve();
        int rr = playerRow, cc = playerCol;
        solver->startAutoPlay(rr, cc);
        demoTick = 0;
        screen = PLAY;
        msg = "Demo Mode: automatic solve";
    }
    void demoUpdate()
    {
        demoTick += GetFrameTime();
        if (demoTick < .4f)
            return;
        demoTick = 0;
        int rr = playerRow, cc = playerCol;
        if (solver->autoPlayStep(rr, cc))
        {
            playerRow = rr;
            playerCol = cc;
        }
        else
        {
            isDemo = false;
            screen = DEMO_FINISH;
        }
    }
    void winDraw()
    {
        BeginDrawing();
        ClearBackground(BG);
        Frame({290, 150, 700, 420}, {17, 25, 41, 255}, ColorAlpha(ACC_CYAN, .4f));
        DrawText("LEVEL COMPLETED", 360, 205, 30, ACC_GREEN);
        DrawText(TextFormat("Level: %d", level), 360, 260, 20, TEXT);
        DrawText(TextFormat("Minimum moves: %d", minMoves), 360, 300, 18, MUTED);
        DrawText(TextFormat("Your moves: %d", moves), 360, 332, 18, MUTED);
        DrawText(TextFormat("Score earned: %d", moves ? minMoves * 100 / moves : 0), 360, 364, 18, ACC_CYAN);
        DrawText(TextFormat("Total score: %d", score), 360, 396, 18, TEXT);
        DrawText(TextFormat("Hint keys: %d (+2 reward)", keys), 360, 428, 18, ACC_GREEN);
        if (levelsDone == 10)
            DrawText("Congratulations! All 10 levels completed!", 360, 460, 18, ACC_GREEN);
        Button a = {{360, 510, 250, 48}, "Continue to Menu"}, b = {{640, 510, 250, 48}, "Next Level"};
        Btn(a);
        Btn(b, true);
        if (Click(a) || IsKeyPressed(KEY_ENTER))
            screen = MENU;
        if (Click(b))
        {
            if (level < 10)
            {
                createMaze(level + 1);
                screen = PLAY;
            }
            else
                screen = MENU;
        }
        EndDrawing();
    }
    void instructionsDraw()
    {
        BeginDrawing();
        ClearBackground(BG);
        header("HOW TO PLAY");
        Frame({24, 118, 1232, 550}, PANEL, BORDER);
        const char *lines[] = {"1. Arrow keys / WASD move the player.", "2. Reach the exit. Walls block movement.", "3. Hint reveals next direction and costs 1 key.", "4. Show Path displays solver route and costs 3 keys.", "5. Completed levels reward 2 hint keys.", "6. Complete all 10 levels to win.", "7. Main menu: Select Level, Next, Replay, Demo, Instructions, Ranking, Logout.", "8. Gameplay keys: H Hint, P Path, R Reset, Q Quit Level, M Sound.", "9. Demo Mode creates a 7x7 maze and animates solver playback.", "10. Score formula: (minimum moves x 100) / your moves."};
        for (int i = 0; i < 10; i++)
            DrawText(lines[i], 70, 165 + i * 38, 18, (i == 3 || i == 2) ? ACC_CYAN : TEXT);
        Button back = {{70, 570, 230, 50}, "Back to Menu"};
        Btn(back);
        if (Click(back) || IsKeyPressed(KEY_ESCAPE))
            screen = MENU;
        EndDrawing();
    }
    void rankingDraw()
    {
        auto v = readUsers();
        if (!v.empty())
            rankSort(v, 0, (int)v.size() - 1);
        BeginDrawing();
        ClearBackground(BG);
        header("GLOBAL RANKING  /  MERGE SORT");
        Frame({24, 118, 1232, 550}, PANEL, BORDER);
        DrawText("RANK", 60, 145, 16, ACC_CYAN);
        DrawText("PLAYER", 180, 145, 16, ACC_CYAN);
        DrawText("LEVEL", 650, 145, 16, ACC_CYAN);
        DrawText("SCORE", 850, 145, 16, ACC_CYAN);
        DrawText("KEYS", 1030, 145, 16, ACC_CYAN);
        for (int i = rankOffset; i < (int)v.size() && i < rankOffset + 12; i++)
        {
            int y = 185 + (i - rankOffset) * 34;
            DrawText(TextFormat("%02d", i + 1), 60, y, 17, TEXT);
            DrawText(v[i].user.c_str(), 180, y, 17, TEXT);
            DrawText(TextFormat("%d/10", v[i].level), 650, y, 17, MUTED);
            DrawText(TextFormat("%d", v[i].score), 850, y, 17, ACC_CYAN);
            DrawText(TextFormat("%d", v[i].keys), 1030, y, 17, MUTED);
            DrawLine(60, y + 25, 1190, y + 25, ColorAlpha(BORDER, .5f));
        }
        if (v.empty())
            DrawText("No players yet.", 60, 190, 18, MUTED);
        DrawText(TextFormat("Showing %d-%d of %d", v.empty() ? 0 : rankOffset + 1, std::min((int)v.size(), rankOffset + 12), (int)v.size()), 850, 600, 14, MUTED);
        Button back = {{60, 600, 220, 48}, "Back to Menu"}, prev = {{900, 600, 130, 48}, "Previous"}, next = {{1050, 600, 130, 48}, "Next"};
        Btn(back);
        Btn(prev);
        Btn(next);
        if (Click(prev))
        {
            rankOffset = std::max(0, rankOffset - 12);
        }
        if (Click(next))
        {
            if (rankOffset + 12 < (int)v.size())
                rankOffset += 12;
        }
        if (Click(back) || IsKeyPressed(KEY_ESCAPE))
        {
            rankOffset = 0;
            screen = MENU;
        }
        EndDrawing();
    }
    void demoDoneDraw()
    {
        BeginDrawing();
        ClearBackground(BG);
        Frame({360, 220, 560, 260}, {17, 25, 41, 255}, ColorAlpha(ACC_CYAN, .4f));
        DrawText("DEMO COMPLETED", 430, 270, 28, ACC_GREEN);
        DrawText("7x7 maze solved using the recursive solver.", 430, 320, 16, TEXT);
        Button b = {{430, 380, 420, 50}, "Return to Menu"};
        Btn(b, true);
        if (Click(b) || IsKeyPressed(KEY_ENTER))
            screen = MENU;
        EndDrawing();
    }
    void textInput()
    {
        int ch = GetCharPressed();
        while (ch > 0)
        {
            if (ch >= 32 && ch <= 126)
            {
                std::string *v = nullptr;
                if (screen == WELCOME)
                {
                    if (form == "signup")
                        v = focus == 0 ? &email : focus == 1 ? &user
                                              : focus == 2   ? &pass
                                                             : &confirm;
                    else if (form == "login")
                        v = focus == 0 ? &user : &pass;
                    else
                        v = &email;
                }
                else if (screen == OTP_VIEW)
                    v = &otpInput;
                else if (screen == SET_PASS)
                    v = focus == 0 ? &newPass : &newConfirm;
                if (v && v->size() < 120)
                    v->push_back((char)ch);
            }
            ch = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            std::string *v = nullptr;
            if (screen == WELCOME)
            {
                if (form == "signup")
                    v = focus == 0 ? &email : focus == 1 ? &user
                                          : focus == 2   ? &pass
                                                         : &confirm;
                else if (form == "login")
                    v = focus == 0 ? &user : &pass;
                else
                    v = &email;
            }
            else if (screen == OTP_VIEW)
                v = &otpInput;
            else if (screen == SET_PASS)
                v = focus == 0 ? &newPass : &newConfirm;
            if (v && !v->empty())
                v->pop_back();
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            if (screen == OTP_VIEW)
                otpSubmit();
            else if (screen == SET_PASS)
                newPassSubmit();
            else if (screen == WELCOME)
            {
                if (form == "signup")
                    signUpSubmit();
                else if (form == "login")
                    login();
                else if (form == "change")
                    changeSubmit();
                else if (form == "delete")
                    deleteSubmit();
            }
        }
    }

public:
    MazeGUI() { createMaze(1); }
    ~MazeGUI()
    {
        delete generator;
        delete solver;
    }
    void run()
    {
        InitWindow(SW, SH, "Maze Lab | DSA Project");
        SetWindowMinSize(1100, 700);
        InitAudioDevice();
        if (IsAudioDeviceReady())
        {
            moveSound = LoadSound("assets/audio/move.wav");
            clickSound = LoadSound("assets/audio/click.wav");
            wallSound = LoadSound("assets/audio/wall.wav");
            hintSound = LoadSound("assets/audio/hint.wav");
            winSound = LoadSound("assets/audio/win.wav");
            ambience = LoadMusicStream("assets/audio/ambient.wav");
            ambience.looping = true;
            audio = true;
            SetSoundVolume(moveSound, .4f);
            SetSoundVolume(clickSound, .3f);
            SetSoundVolume(wallSound, .4f);
            SetSoundVolume(hintSound, .5f);
            SetSoundVolume(winSound, .55f);
            SetMusicVolume(ambience, .14f);
            PlayMusicStream(ambience);
        }
        SetTargetFPS(60);
        while (!WindowShouldClose())
        {
            if (audio && soundOn)
                UpdateMusicStream(ambience);
            switch (screen)
            {
            case WELCOME:
                textInput();
                welcomeDraw();
                break;
            case OTP_VIEW:
                textInput();
                otpDraw();
                break;
            case SET_PASS:
                textInput();
                newPassDraw();
                break;
            case MENU:
                menuDraw();
                break;
            case PLAY:
                if (isDemo)
                    demoUpdate();
                playDraw();
                break;
            case INSTRUCTIONS:
                instructionsDraw();
                break;
            case RANKING:
                rankingDraw();
                break;
            case WIN_VIEW:
                winDraw();
                break;
            case DEMO_FINISH:
                demoDoneDraw();
                break;
            }
        }
        if (audio)
        {
            UnloadSound(moveSound);
            UnloadSound(clickSound);
            UnloadSound(wallSound);
            UnloadSound(hintSound);
            UnloadSound(winSound);
            UnloadMusicStream(ambience);
        }
        CloseAudioDevice();
        CloseWindow();
    }
};
int main()
{
    MazeGUI app;
    app.run();
    return 0;
}
