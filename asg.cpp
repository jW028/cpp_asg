#include <iostream>
#include <vector>

using namespace std;
int getValidatedInput(int min, int max);
void displayLogo();
void customerMenu();
void ownerExpertMenu();

struct Customer {
    string name;
    string contact;
};

struct Expert {
    string name;
    string contact;
    int weeklyHours;
    vector<bool> availability;
};

struct Owner {
    string name;
    string contact;
}; 

struct Services {
    string name;
    string description;
    double price;
    int duration; // in hours
};

struct Booking {
    Customer customer;
    Expert expert;
    Services service;
    int day;
    int slot;
};


int main() {

    int choice;
    do {
        displayLogo();
        // displayMainMenu();

        choice = getValidatedInput(1, 3);

        switch(choice) {
            case 1:
                customerMenu();
                break;
            case 2:
                ownerExpertMenu();
                break;
            case 3:
                cout << "Exiting program\n";
                break;
        }
    } while (choice != 3);

    choice = getValidatedInput(1, 3);
}

void displayLogo() {
    cout << "   __             _                                       __                              " << endl;
    cout << "  / /  ___   ___ | | _____ _ __ ___   __ ___  ____  __   / /  ___  _   _ _ __   __ _  ___ " << endl;
    cout << " / /  / _ \\ / _ \\| |/ / __| '_ ` _ \\ / _` \\ \\/ /\\ \\/ /  / /  / _ \\| | | | '_ \\ / _` |/ _ \\" << endl;
    cout << "/ /__| (_) | (_) |   <\\__ \\ | | | | | (_| |>  <  >  <  / /__| (_) | |_| | | | | (_| |  __/" << endl;
    cout << "\\____/\\___/ \\___/|_|\\_\\___/_| |_| |_|\\__,_/_/\\_\\/_/\\_\\ \\____/\\___/ \\__,_|_| |_|\\__, |\\___|" << endl;
    cout << "                                                                               |___/      " << endl;
}

void displayMainMenu() {
    cout << "1. Customer" << endl;
    cout << "2. Owner/Expert" << endl;
    cout << "3. Exit" << endl;
    cout << "Enter your choice: " ;
}

int getValidatedinput(int min, int max) {
    int input;
    while (true) {
        cin >> input;
        if (cin.fail() || input < min || input > max) {
            cin.clear();
            cin.ignore();
            cout << "Invalid input. Please enter a number between " << min << " and " << max << ": ";
        } else {
            return input;
        }
    }
}

void customerMenu() {
    int choice;
    do {
        displayLogo();
        cout << "\nCustomer Menu:\n";
        cout << "1. View Organization Details\n";
        cout << "2. View Services\n";
        cout << "3. View Experts\n";
        cout << "4. Check Schedule\n";
        cout << "5. Make Booking\n";
        cout << "6. View Booked Schedule\n";
        cout << "7. Return to Main Menu\n";
        cout << "Enter your choice: ";

        choice = getValidatedInput(1, 7);

    
    } while (choice != 7);
}

void ownerExpertMenu() {
    int choice;
}




