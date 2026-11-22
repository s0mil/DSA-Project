
#include <iostream>
#include <windows.h>

#include "../includes/player.h"
#include "../includes/Game.h"

using namespace std;

int main()
{
    Player player;
    int choice;

    while(true)
    {
        system("cls");

        cout << "\n========== MAZE GAME ==========";
        cout << "\n1. Sign Up";
        cout << "\n2. Login";
        cout << "\n3. Change Password";
        cout << "\n4. Delete Account";
        cout << "\n5. Exit";

        cout << "\n\nEnter Choice: ";
        cin >> choice;

        if(choice == 1)
        {
            player.signUp();
        }

        else if(choice == 2)
        {
            if(player.login())
            {
                cout << "\nLogin Successful!";
                Sleep(1000);

                Game game(player);
                game.run();

                player.logout();
            }
            else
            {
                cout << "\nLogin Failed!";
                Sleep(1000);
            }
        }

        else if(choice == 3)
        {
            player.changePassword();
        }

        else if(choice == 4)
        {
            char confirm;

            cout << "\nAre you sure you want to delete your account? (Y/N): ";
            cin >> confirm;

            if(confirm == 'Y' || confirm == 'y')
            {
                if(player.deleteAccount())
                    cout << "\nAccount Deleted Successfully!";
                else
                    cout << "\nAccount Deletion Failed!";
            }
        }

        else if(choice == 5)
        {
            cout << "\nThanks for Playing!";
            break;
        }

        else
        {
            cout << "\nInvalid Choice!";
        }

        cout << "\n\nPress any key to continue...";
        cin.ignore();
        cin.get();
    }

    return 0;
}
