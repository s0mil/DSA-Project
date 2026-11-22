# 🧩 Maze Lab

> **A C++ Data Structures & Algorithms project** featuring recursive maze generation, recursive pathfinding, account management, level progression, scoring, and a graphical interface built with **Raylib**.

<p align="center">
  <strong>Recursive Maze Generation · Backtracking Solver · OTP Authentication · Leaderboard</strong>
</p>

---

## 📌 Overview

Maze Lab is a maze game project with a Raylib graphical version and a console-based implementation. Players can create an account, verify their email using a one-time password (OTP), log in, solve progressively larger mazes, earn scores and hint keys, and view a global ranking.

The maze is generated using a recursive algorithm. A recursive solver finds a route from the starting cell to the exit, which powers hints, path display, and Demo Mode.

## ✨ Features

### 👤 Account & Authentication
- Sign up using an email address and username.
- Email OTP verification during signup.
- Login using username and password.
- Maximum of **3 incorrect login attempts** per login flow.
- Change password after email OTP verification.
- Delete an account after email OTP verification.
- Password validation requires at least 8 characters, an uppercase letter, a lowercase letter, a digit, and a special character.
- Password fields are visually masked in the GUI.

### 🧭 Maze Gameplay
- **10 levels** with increasing maze sizes.
- Level 1 starts at **9×9**; each new level increases each maze dimension by 2 (up to 27×27 at Level 10).
- Start/select unlocked levels, move to the next level, and replay completed/unlocked levels.
- Generate a new maze during gameplay.
- Reset the player position in the current maze.
- Walls block movement; reach the exit to complete the level.
- Move using **Arrow Keys** or **WASD**.

### 🗝️ Hint & Path Tools
- **Hint:** shows the next direction along the solver's route and costs **1 hint key**.
- **Show Path:** displays the solver's route and costs **3 hint keys**.
- Hide the displayed path after revealing it.
- Players begin with **5 hint keys**.
- Completing a level rewards **2 additional hint keys**.

### 🤖 Demo Mode
- Generates a **7×7 maze**.
- Automatically animates the recursive solver as it moves through the solution.
- Demonstrates the solver's route without manual movement.

### 🏆 Score & Progression
- Score is awarded when a level is completed.
- The score for a level is calculated using:

  ```text
  Level Score = (Minimum Moves × 100) / Player Moves
  ```

- Total score, completed levels, and hint keys are saved to the account record.
- Level progression unlocks additional levels.
- Completion screen shows level, moves, score earned, total score, and remaining hint keys.

### 🌐 Global Ranking
- Displays players sorted by score (highest first).
- Ranking is implemented using **Merge Sort**.
- GUI leaderboard includes rank, player name, level progress, score, and hint keys.
- GUI ranking supports pagination (12 players per page).

### 🎨 Raylib GUI
- Dark-themed interface with cyan/purple accents.
- Signup, login, OTP, password-change, account-deletion, menu, gameplay, instructions, leaderboard, and completion screens.
- Mouse-operated buttons and form fields.
- Gameplay status display for level, moves, and hint keys.
- Audio controls and sound effects/background ambience.
- Responsive maze sizing and centering within the game window.

### 🔊 Audio
Audio files are stored in `assets/audio/`:

| File | Purpose |
|---|---|
| `ambient.wav` | Background music |
| `move.wav` | Successful movement |
| `click.wav` | UI interaction |
| `wall.wav` | Blocked movement |
| `hint.wav` | Hint/path action |
| `win.wav` | Level completion |

Sound can be toggled in the GUI.

---

## 🧠 DSA Concepts Used

| Concept | Use in Maze Lab |
|---|---|
| Recursion | Maze generation and maze-solving search |
| Backtracking | Explore paths and return from dead ends |
| 2D Dynamic Arrays | Represent the maze grid |
| Dynamic Memory | Maze, generator, and solver structures |
| Merge Sort | Sort global ranking by score |
| Path Tracking | Store row/column coordinates for a solution |
| File Handling | Persist account and game progress |
| Data Parsing | Read/write delimited account records |

---

## 🗂️ Project Structure

```text
DSA_Project-main/
├── assets/
│   └── audio/
│       ├── ambient.wav
│       ├── click.wav
│       ├── hint.wav
│       ├── move.wav
│       ├── wall.wav
│       └── win.wav
├── gui/
│   └── MazeRaylib.cpp        # Raylib graphical interface
├── includes/
│   ├── AccountManager.h
│   ├── Game.h
│   ├── Maze.h
│   ├── MazeGenerator.h
│   ├── MazeSolver.h
│   ├── OTP.h
│   ├── hiddenpasswordmechanism.h
│   ├── player.h
│   └── ranking.h
├── src/
│   ├── AccountManager.cpp
│   ├── Game.cpp
│   ├── Maze.cpp
│   ├── MazeGenerator.cpp
│   ├── MazeSolver.cpp
│   ├── OTP.cpp
│   ├── hiddenpasswordmechanism.cpp
│   ├── main.cpp              # Console entry point
│   ├── player.cpp
│   └── ranking.cpp
├── README.md
└── README_GUI.md
```

---

## 🛠️ Requirements

- Windows 10/11
- C++ compiler (`g++` / MinGW)
- Raylib installed and configured
- `curl.exe` available in Windows for SMTP email sending
- Internet connection for real email OTP delivery

The build commands below assume Raylib is installed at:

```text
C:/raylib/raylib/src
```

If Raylib is installed somewhere else, update the include and library paths in the command.

---

## 🚀 Build & Run — GUI Version

Run these commands from the project root in PowerShell or a terminal configured with `g++`:

```bash
g++ gui/MazeRaylib.cpp src/Maze.cpp src/MazeGenerator.cpp src/MazeSolver.cpp src/OTP.cpp -Iincludes -I"C:/raylib/raylib/src" -L"C:/raylib/raylib/src" -lraylib -lopengl32 -lgdi32 -lwinmm -o MazeGUI.exe
```

Run the application:

```powershell
.\MazeGUI.exe
```

Keep the `assets/` directory beside the executable and launch the program from the project root so relative asset and data-file paths work.

---

## 📧 Configure Real Email OTP (Gmail)

Maze Lab uses Gmail SMTP to deliver OTP messages. Use a **Google App Password**, not your normal Gmail password.

### 1. Prepare your Google Account

1. Open [Google Account Security](https://myaccount.google.com/security).
2. Enable **2-Step Verification**.
3. Create an **App Password** if the feature is available for your account.
4. Keep the App Password private.

### 2. Set Windows User Environment Variables

Open PowerShell and replace the placeholders with your own values:

```powershell
[Environment]::SetEnvironmentVariable("MAZE_SMTP_EMAIL", "yourgmail@gmail.com", "User")
[Environment]::SetEnvironmentVariable("MAZE_SMTP_APP_PASSWORD", "your-app-password", "User")
```

These commands save the variables for your Windows user, so they do not need to be entered in every terminal session.

### 3. Restart your editor/terminal

Close VS Code completely and reopen it after setting the variables. Start a new terminal and run Maze Lab.

### 4. Verify the settings (optional)

Check the email variable:

```powershell
[Environment]::GetEnvironmentVariable("MAZE_SMTP_EMAIL", "User")
```

Check that the App Password is saved without printing it:

```powershell
if ([Environment]::GetEnvironmentVariable("MAZE_SMTP_APP_PASSWORD", "User")) {
    "App Password is saved"
} else {
    "App Password is missing"
}
```

### Troubleshooting OTP

- Check the recipient's Spam/Junk folder.
- Confirm the sender email and App Password are correct.
- Confirm 2-Step Verification is enabled.
- Ensure `curl.exe` is available.
- Run the application from the project root so it can create/read its mail and data files.
- If sending fails, inspect `smtp_error.log` for diagnostic details. **Do not publish or share logs containing private email information.**

---

## 💾 Account & Progress Storage

The application stores account records in `SignUp.txt` in the working/project directory. Records use this pipe-separated format:

```text
email|username|encoded-password|completed-level|hint-keys|score
```

Example structure (dummy values only):

```text
player@example.com|MazePlayer|<encoded-password>|2|7|450
```

The GUI and console implementation use this record format. Run them from the same project directory to work with the same save file.

> **Security note:** The current project uses a simple XOR-based encoding converted to hexadecimal. This is obfuscation, not secure password hashing. Do not use this authentication implementation for real-world accounts or sensitive data.

---

## 🎮 Controls

| Key | Action |
|---|---|
| `↑` / `W` | Move Up |
| `↓` / `S` | Move Down |
| `←` / `A` | Move Left |
| `→` / `D` | Move Right |
| `H` | Hint (−1 key) |
| `P` | Show / Hide Path (reveal costs 3 keys) |
| `R` | Reset player position |
| `Q` | Quit current level |
| `M` | Toggle sound |
| `Esc` | Back/return where supported |

---

## 🕹️ Main Menu

After login, the menu provides:

1. **Start / Select Level** — choose an unlocked level.
2. **Next Level** — move to the next progression level.
3. **Replay Level** — replay a previously unlocked level.
4. **Demo Mode** — watch the solver complete a maze automatically.
5. **Instructions** — review gameplay controls and rules.
6. **Global Ranking** — view the leaderboard.
7. **Logout** — save progress and return to the welcome screen.

---

## 🧩 Gameplay Flow

1. Sign up and verify your email using OTP.
2. Log in to your account.
3. Select an available maze level.
4. Navigate from the starting point to the exit.
5. Use hints or reveal the solution path if needed (keys are consumed).
6. Complete the level to earn score and 2 hint keys.
7. Progress through all 10 levels.
8. Check your score on the global leaderboard.

---

## 📄 Project Notes

- The GUI is implemented in `gui/MazeRaylib.cpp` and uses the maze/generator/solver modules.
- The console version has its own entry point at `src/main.cpp` and game flow in `src/Game.cpp`.
- Account data and audio use relative file paths; run the program from the project root.
- OTP delivery requires SMTP configuration; without valid configuration, real email delivery may fail.

---

## 👨‍💻 Contributors

_25K-0521 Somil._
_25K-0556 Muhammad Tahir Sharif._
_25K-0646 Muhammad Hammad Khan._

---

<p align="center"><strong>Made with C++ · Raylib · Data Structures & Algorithms</strong></p>
