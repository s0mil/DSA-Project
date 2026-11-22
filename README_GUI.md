# Maze Lab GUI — Terminal Features Integrated

The Raylib GUI uses the terminal project's same account record format:
`email|username|encoded-password|completed-level|hint-keys|score`

Included features:
- Sign Up with email OTP; Login (3 attempts); Change Password with OTP; Delete Account with OTP; Exit
- Start/select unlocked levels (1–10), Next Level, Replay Level
- Demo Mode (7×7 maze with automatic recursive-solver playback)
- Instructions and Global Ranking
- Arrow keys/WASD movement, Hint (costs 1 key), Show Path (costs 3 keys), Quit Level
- Score formula, level progression, +2 hint-key reward, persistent progress
- Audio toggle and effects

## Build (Windows PowerShell, from project root)

```powershell
g++ gui/MazeRaylib.cpp src/Maze.cpp src/MazeGenerator.cpp src/MazeSolver.cpp src/OTP.cpp -Iincludes -I"C:/raylib/raylib/src" -L"C:/raylib/raylib/src" -lraylib -lopengl32 -lgdi32 -lwinmm -o MazeGUI.exe
.\MazeGUI.exe
```

## Real email OTP

The sender's Gmail app password is not stored in the code. Set the following in the same PowerShell window before starting the app:

```powershell
$env:MAZE_SMTP_EMAIL="your-sender@gmail.com"
$env:MAZE_SMTP_APP_PASSWORD="your-new-Gmail-app-password"
```

Gmail SMTP usually requires 2-Step Verification and an App Password. The project uses `curl.exe`. If email sending fails, the OTP view shows the failure and includes a Resend button.

## Account-file compatibility

The GUI reads/writes `SignUp.txt` from the current working directory, or `src/SignUp.txt` if that file is present. For compatibility, run the terminal version and GUI from the same working directory so they use the same data file. The uploaded project has `src/SignUp.txt`.

Keep `assets/audio/` in the project root for the sound effects/music.

### OTP troubleshooting
- Set the two SMTP environment variables in the same PowerShell window from which you run `MazeGUI.exe`.
- Use a Gmail App Password (requires 2-Step Verification), not your normal Gmail password.
- Run the executable from the project root so `mail.txt`, `SignUp.txt`, and audio paths are available.
- On failure, the GUI now reports missing settings or a general SMTP failure; `smtp_error.log` records curl's diagnostic. Do not share this log if it contains your email address.
