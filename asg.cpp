#include <iostream>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <regex>
#include <vector>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;
const int optionWidth = 7;
const int descriptionWidth = 28;
const int MAX_CUSTOMERS = 100;
const int START_HOUR = 9;
const int END_HOUR = 17; // Operating hours from 9 AM to 5 PM
const int MAX_WORK_HOURS = 6; // Maximum work hours per day for an expert
const int CONSULTATION_SLOT_DURATION = 1;
const int TREATMENT_SLOT_DURATION = 2;
const int DAYS_IN_WEEK = 5; // Monday to Friday
const int MAX_SLOTS_PER_DAY = 8;  // Total slots available (8 hours)
 
struct Customer {
    string name;
    string email;
    string contact;
    string password;
};

enum PaymentMethod {
    EWALLET,
    BANK_TRANSFER,
    CREDIT_CARD
};

enum class SessionType { Treatment, Consultation, Unavailable };

struct Service {
    string name;
    string description;
    double price;

    Service() : name(""), price(0.0) {} // Default constructor

    Service(string name, string desc, double price) : name(name), description(desc), price(price) {}
};

struct TimeSlot {
    bool isBooked;
    string timeRange;
    SessionType type;
};

struct Expert {
    string name;
    Service service;
    TimeSlot schedule[4][DAYS_IN_WEEK][MAX_SLOTS_PER_DAY];
    int hoursWorkedPerDay[4][DAYS_IN_WEEK];
    string dates[4];
};

struct Owner {
    string name;
    string contact;
}; 

struct Receipt {
    string bookingNumber;
    string customerName = "steve";
    SessionType sessionType;
    string serviceName;
    string timeSlot;
    PaymentMethod paymentMethod;
    double amountPaid;

    void generateReceipt() {
        cout << "Receipt\n";
        cout << "-------------------------------------\n";
        cout << "Booking Number: " << bookingNumber << endl;
        cout << "Customer Name: " << customerName << endl;
        cout << "Session: " << (sessionType == SessionType::Treatment ? "Treatment" : "Consultation") << endl;
        cout << "Service: " << serviceName << endl;
        cout << "Time Slot: " << timeSlot << endl;
        cout << "Payment Method: " << paymentMethodToString() << "\n";
        cout << "Amount Paid: RM " << fixed << setprecision(2) << amountPaid << endl;
        cout << "-------------------------------------\n";
    }

    string paymentMethodToString() const {
        switch(paymentMethod) {
            case EWALLET: return "E-Wallet";
            case BANK_TRANSFER: return "Bank Transfer";
            case CREDIT_CARD: return "Credit Card";
            default: return "Unknown";
        }
    }
};

struct Booking {
    string referenceNum;
    Customer customer;
    Expert expert;
    Service service;
    SessionType treatment;
    int weekNumber;
    int day;
    int slot;
};

enum class UserType { ADMIN, EXPERT };

struct User {
    string username;
    string password;
    UserType type;
    bool isLoggedIn;

    User(string uname, string pwd, UserType t) 
        : username(uname), password(pwd), type(t), isLoggedIn(false) {}
};

vector<User> users;
User* currentUser = nullptr;


void displayLogo();
void displayMainMenu();
void customerManagement();
void loadCustomersFromFile(Customer[], int&);
void saveCustomersToFile(Customer[], int);
void customerMenu();
void customerSignUp(Customer []);
bool customerLogin(Customer [], int);
bool isValidEmail(const string&);
bool isValidPassword(const string&);
void initializeUsers();
void initializeExpert(Expert, string, Service);
void adminExpertMenu();
void aboutUs();
int getValidatedInput(int min, int max);
bool login();
string getPasswordInput();
void logout();
void serviceDesc(Service service);
void viewServices();
void botoxDesc();
void manicureDesc();
void viewExperts();
void checkSchedule();
void makeBooking(Service service);
void viewBookedSchedule();
void pauseAndClear();
void clearScreen();



int main() {
    initializeUsers();
    int choice;
    do {
        displayLogo();
        displayMainMenu();


        choice = getValidatedInput(1, 3);

        switch(choice) {
            case 1:
                customerManagement();
                break;
            case 2:
                if (login()) {
                    adminExpertMenu();
                }
                break;
            case 3:
                cout << "Exiting program...\n";
                return 0;
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
    cout << "Main Menu:\n";
    cout << "+--------+------------------------------+" << endl;
    cout << "| " << setw(optionWidth - 1)  << "Option" << " | " << left << setw(descriptionWidth)  <<  "Description" << " |" << endl;
    cout << "+--------+------------------------------+" << endl;
    cout << "| " << setw(optionWidth - 1) << "1" << " | " << setw(descriptionWidth) << "Customer" << " |" << endl;
    cout << "| " << setw(optionWidth - 1) << "2" << " | " << setw(descriptionWidth) << "Staff" << " |" << endl;
    cout << "| " << setw(optionWidth - 1) << "3" << " | " << setw(descriptionWidth) << "Exit" << " |" << endl;
    cout << "+--------+------------------------------+" << endl;
    cout << "Enter your choice: " ;
}

void initializeUsers() {
    users.push_back(User("admin", "admin123", UserType::ADMIN));
    users.push_back(User("Alice", "alice123", UserType::EXPERT));
    users.push_back(User("Ben", "ben123", UserType::EXPERT));
    users.push_back(User("Joanne", "joanne123", UserType::EXPERT));
}

void initializeExpert(Expert &expert, const string &name, const Service &service) {
    expert.name = name;
    expert.service = service;
        for (int week=0; week<4; ++week) {
            for (int day = 0; day < DAYS_IN_WEEK; ++day) {
                expert.hoursWorkedPerDay[week][day] = 0;
                for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                    expert.schedule[week][day][slot].isBooked = false;
                    expert.schedule[week][day][slot].timeRange = to_string(START_HOUR + slot) + ":00 - " + to_string(START_HOUR + slot + 1) + ":00";
                    expert.schedule[week][day][slot].type = SessionType::Consultation; // Default to consultation
                }
            }
        }
    }


void customerManagement() {
    Customer customers[100];
    int choice, customerCount;
    clearScreen();
    loadCustomersFromFile(customers, customerCount);
    cout << customers[0].email << endl;
    cout << customers[1].email << endl;


    do {
        displayLogo();
        cout << "Welcome!\n";
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(optionWidth - 1)  << "Option" << " | " << left << setw(descriptionWidth)  <<  "Description" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(optionWidth - 1) << "1" << " | " << setw(descriptionWidth) << "Sign Up (New Customer)" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "2" << " | " << setw(descriptionWidth) << "Login (Existing Customer)" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "3" << " | " << setw(descriptionWidth) << "Back" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;
        cout << "Enter your choice: ";

        choice = getValidatedInput(1,3);

        switch(choice) {
            case 1: 
                customerSignUp(customers); 
                break;
            case 2: 
                if (customerLogin(customers, customerCount)){
                    customerMenu();
                }
                break;
            case 3: 
                cout << "Returning to Main Menu\n"; break;
        }
        if (choice != 3) pauseAndClear();
    } while (choice != 3);


}

bool isValidEmail(const string& email) {
    const regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
    return regex_match(email, pattern);
}

bool isValidPassword(const string& password) {
    const regex pattern("(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[a-zA-Z\\d!@#$%^&*]{8,}");
    return regex_match(password, pattern);
}

void customerSignUp(Customer customers[]){
    int customerCount = 0;
    if (customerCount >= MAX_CUSTOMERS) {
        cout << "Maximum number of customers reached!" << endl;
        return;
    }
    Customer newCustomer;

    cout << "\n=== Customer Signup ===" << endl;
    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, newCustomer.name);

    cout << "Enter your contact number: ";
    getline(cin, newCustomer.contact);

    do {
        cout << "Enter your email: ";
        getline(cin, newCustomer.email);
        if (!isValidEmail(newCustomer.email)) {
            cout << "\nInvalid email format. Please try again.\n ";
        }
    } while (!isValidEmail(newCustomer.email));


    do {
        cout << "Enter your password (min 8 characters, must include uppercase, lowercase, digit, and special character): ";
        newCustomer.password = getPasswordInput();
        if (!isValidPassword(newCustomer.password)) {
            cout << "Invalid password format.\n"
                 << "Password should be:\n "
                 << "- least 8 characters long\n" 
                 << "contain at least one uppercase letter, one lowercase letter\n"
                 << "one digit and one special character";
        }
    } while (!isValidPassword(newCustomer.password));

    customers[customerCount] = newCustomer;
    customerCount++;

    saveCustomersToFile(customers, customerCount);

}

bool customerLogin(Customer customers[], int customerCount) {
    string email, password;
    cout << "\n=== Customer Login ===" << endl;
    cout << "Enter your email: ";
    cin.ignore();
    getline(cin, email);

    cout << "Enter your password: ";
    password = getPasswordInput();

    for (int i=0; i<customerCount; i++) {
        if (customers[i].email == email && customers[i].password == password) {
            return true;
        }
    }
    cout << "Login failed. Please try again.\n";
    pauseAndClear();
    return false;
}

void saveCustomersToFile(Customer customers[], int customerCount) {
    ofstream outFile("customers.txt", ios::app);

    if (!outFile) {
        cerr << "Error opening file for writing." << endl;
        return;
    }

    for (int i=0; i<customerCount; i++) {
        outFile << customers[i].name << ","
                << customers[i].contact << ","
                << customers[i].email << ","
                << customers[i].password << endl;
    }

    outFile.close();
    cout << "Customers saved to file successfully!" << endl;
}

void loadCustomersFromFile(Customer customers[], int &customerCount) {
    ifstream inFile("customers.txt");

    if (!inFile) {
        cout << "No existing file for customers. Starting with a clean file." << endl;
        return;
    }

    string line;
    customerCount = 0;

    while (getline(inFile, line) && customerCount < MAX_CUSTOMERS) {
        stringstream ss(line);
        string name, email, contact, password;

        getline(ss, name, ',');
        getline(ss, contact, ',');
        getline(ss, email, ',');
        getline(ss, password, ',');

        customers[customerCount].name = name;
        customers[customerCount].email = email;
        customers[customerCount].contact = contact;
        customers[customerCount].password = password;

        customerCount++;
    }

    inFile.close();
    cout << "Customers loaded from file successfully!" << endl;
}

void customerMenu() {
    int choice;

    do {
        clearScreen();
        displayLogo();
        cout << "\nCustomer Menu:\n";
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(optionWidth - 1)  << "Option" << " | " << left << setw(descriptionWidth)  <<  "Description" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(optionWidth - 1) << "1" << " | " << setw(descriptionWidth) << "View Organization Details" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "2" << " | " << setw(descriptionWidth) << "View Services" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "3" << " | " << setw(descriptionWidth) << "View Experts" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "4" << " | " << setw(descriptionWidth) << "Check Schedule" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "5" << " | " << setw(descriptionWidth) << "Make Booking" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "6" << " | " << setw(descriptionWidth) << "View Booked Schedule" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "7" << " | " << setw(descriptionWidth) << "Return to Main Menu" << " |" << endl;

        cout << "+--------+------------------------------+" << endl;
        cout << "Enter your choice: ";

        choice = getValidatedInput(1, 7);

        switch (choice) {
            case 1: aboutUs(); break;
            case 2: viewServices(); break;
            case 3: viewExperts(); break;
            // case 4: checkSchedule(); break;
            // case 5: makeBooking(); break;
            // case 6: viewBookedSchedule(); break;
            case 7: cout << "Returning to Main Menu\n"; break;
        }

        if (choice != 7) pauseAndClear();
    } while (choice != 7);
}

string getPasswordInput() {
#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        } else {
            password.push_back(ch);
            std::cout << '*';
        }
    }
    std::cout << std::endl;
#else
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    string password;
    char ch;
    while (true) {
        ch = getchar();
        if (ch == '\n' || ch == '\r') {
            cout << endl;
            break;
        } else if (ch == 127 || ch == 8) {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else {
            password.push_back(ch);
            cout << '*';
        }
        cout.flush();
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    return password;
}

bool login() {
    clearScreen();
    displayLogo();
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter password: ";
    password = getPasswordInput();

    for (auto& user: users) {
        if (user.username == username && user.password == password) {
            user.isLoggedIn = true;
            currentUser = &user;
            cout << "Login successful. Welcome, " << username << "!\n";
            return true;
        }
    }
    cout << "Login failed. Please try again.\n";
    pauseAndClear();
    return false;
}

void logout() {
    if (currentUser) {
        currentUser->isLoggedIn = false;
        cout << "Logged out successfully. Goodbye, " << currentUser->username << "!\n";
        currentUser = nullptr;
    } else {
        cout << "No user is currently logged in.\n";
    }
}

void adminExpertMenu() {
    int choice;

    do {
        clearScreen();
        displayLogo();
        cout << "\nAdmin/Expert Menu:\n";
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(optionWidth - 1)  << "Option" << " | " << left << setw(descriptionWidth)  <<  "Description" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(optionWidth - 1) << "1" << " | " << setw(descriptionWidth) << "View Individual Schedule" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "2" << " | " << setw(descriptionWidth) << "View Overall Schedule" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "3" << " | " << setw(descriptionWidth) << "View Customers" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "4" << " | " << setw(descriptionWidth) << "Generate Sales Report" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "5" << " | " << setw(descriptionWidth) << "Return to Main Menu" << " |" << endl;

        cout << "+--------+------------------------------+" << endl;
        cout << "Enter your choice: ";

        choice = getValidatedInput(1, 5);

        switch (choice) {
            case 1: aboutUs(); break;
            case 2: viewServices(); break;
            case 3: viewExperts(); break;
            // case 4: checkSchedule(); break;
            // case 5: makeBooking(); break;
            // case 6: viewBookedSchedule(); break;
            case 5: cout << "Returning to Main Menu\n"; break;
        }

        if (choice != 5) pauseAndClear();
    } while (choice != 5);
}

int getValidatedInput(int min, int max) {
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

void aboutUs() {
    clearScreen();
    displayLogo();
    cout << "\nOrganization Details:\n";
    cout << "Name: Looksmaxx Lounge\n";
    cout << "Address: Jalan Genting Kelang, Setapak, 53300 Kuala Lumpur\n";
    cout << "Phone: 03-12345678\n";
    cout << "Email: looksmaxxlounge@enquiries.com\n";
}

void serviceDesc(Service service) {
    clearScreen();
    displayLogo();
    cout << "\n" << service.name << "\n";
    cout << "Description: " << service.description << "\n";
    cout << "Price: RM" << fixed << setprecision(2) << service.price << "\n";
    cout << "Duration: " << 2 << " hours\n";

    cout << "Press Y to book or N to return to main menu: ";
    char choice;
    cin >> choice;
    if (choice == 'Y' || choice == 'y') {
        makeBooking(service);
    }

}

void viewServices() {
    clearScreen();
    displayLogo();
    Service services[3] = {
        {"Facial", "A rejuvenating facial treatment.", 50.00},
        {"Botox and Fillers", "Cosmetic injections for wrinkle treatment.", 200.00},
        {"Manicure", "A relaxing manicure session.", 30.00}
    };
    int choice, service_count = 1;
    cout << "\nServices\n";
    cout << "+--------+------------------------------+" << endl;
    cout << "| " << setw(optionWidth - 1)  << "Option" << " | " << left << setw(descriptionWidth)  <<  "Service Name" << " |" << endl;
    cout << "+--------+------------------------------+" << endl;
    cout << "| " << setw(optionWidth - 1) << "1" << " | " << setw(descriptionWidth) << "Facial" << " |" << endl;
    cout << "| " << setw(optionWidth - 1) << "2" << " | " << setw(descriptionWidth) << "Botox and Fillers" << " |" << endl;
    cout << "| " << setw(optionWidth - 1) << "3" << " | " << setw(descriptionWidth) << "Manicure" << " |" << endl;

    cout << "+--------+------------------------------+" << endl;
    cout << "Enter your choice: ";

    choice = getValidatedInput(1, 3);

    serviceDesc(services[choice - 1]);
}

void makeBooking(Service service) {
    int choice;
    cout << "\nMeet expert for: \n";
    cout << "+--------+------------------------------+" << endl;
    cout << "| " << setw(optionWidth - 1)  << "Option" << " | " << left << setw(descriptionWidth)  <<  "Booking Type" << " |" << endl;
    cout << "+--------+------------------------------+" << endl;
    cout << "| " << setw(optionWidth - 1) << "1" << " | " << setw(descriptionWidth) << "Treatment" << " |" << endl;
    cout << "| " << setw(optionWidth - 1) << "2" << " | " << setw(descriptionWidth) << "Consultation" << " |" << endl;
    cout << "+--------+------------------------------+" << endl;

    cout << "Enter your choice: ";

    choice = getValidatedInput(1, 2);

    switch (choice) {
            case 1: cout << "Treatment\n"; break;
            case 2: cout << "Consultation\n"; break;
        }
}

void viewExperts() {
    clearScreen();
    displayLogo();

    cout << "\nExperts:\n";
    // Expert experts[3] = {
    //     {"Alice", 40, vector<bool>(7, true)},
    //     {"Bob", 30, vector<bool>(7, true)},
    //     {"Charlie", 35, vector<bool>(7, true)}
    // };

}

void pauseAndClear() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    clearScreen();
}

void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    system("clear");
#endif
}

