#include <iostream>
#include <cstdio>
#include <iomanip>
#include <limits>
#include <sstream>
#include <fstream>
#include <regex>
#ifdef _WIN32
    #include <conio.h>
    #include <direct.h>
    #include <io.h>
    #define ACCESS access
    #define MKDIR(dir) _mkdir(dir)
#else
    #include <termios.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #define ACCESS access
    #define MKDIR(dir) mkdir(dir, 0777);

#endif

using namespace std;
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

enum  SessionType { TREATMENT, CONSULTATION, UNAVAILABLE };

struct Service {
    string name;
    string description;
    double price;
};

struct TimeSlot {
    bool isBooked;
    string timeRange;
    SessionType type;
};

struct Expert {
    char name[50];
    TimeSlot schedule[DAYS_IN_WEEK][MAX_SLOTS_PER_DAY]; 
    int hoursWorkedPerDay[DAYS_IN_WEEK];

};

struct Receipt {
    string bookingNumber;
    Customer customer;
    Expert expert;
    SessionType sessionType;
    string serviceName;
    string date;
    string timeSlot;
    PaymentMethod paymentMethod;
    double amountPaid;
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

enum UserType { ADMIN, EXPERT };

struct User {
    string username;
    string password;
    UserType type;
    bool isLoggedIn;
};

void displayLogo();
void displayMainMenu();
void customerManagement();
void loadCustomersFromFile(Customer[], int&);
void saveCustomersToFile(Customer[], int);
void customerMenu(Customer&);
void customerSignUp(Customer []);
int customerLogin(Customer [], int);
bool isValidEmail(const string&);
bool isValidPassword(const string&);
void initializeExpert(Expert&, const string&);
void initializeService(Service&, string, double);
string paymentMethodToString(PaymentMethod);
void generateReceipt(const Receipt&);
void saveBooking(const Receipt&);
int loadBookings(Receipt[]);
void displayCustomerBookings(Customer customer);
void displaySchedule(const Expert&, int);
bool canBookSlot(const Expert&, int, int, SessionType);
int chooseWeek();
int* selectTimeSlot(const Expert&, int, SessionType);
void saveScheduleToFile(const Expert&);
void loadScheduleFromFile(Expert&, int);
PaymentMethod selectPaymentMethod();
int loadBookingCounter(const string&);
void saveBookingCounter(const string&, int);
string generateBookingNumber();
void generateReceiptFile(const Receipt&, const string&);
void printReceipt(const string&);
void makeBooking(Expert&, Service, SessionType, Customer&);
void adminExpertMenu();
void aboutUs();
int getValidatedInput(int min, int max);
bool login();
string getPasswordInput();
void logout();
void serviceDesc(Service, Customer&);
void viewServices(Customer&);
void botoxDesc();
void manicureDesc();
void viewExperts();
void checkSchedule();
void viewBookedSchedule();
void pauseAndClear();
void clearScreen();



int main() {
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
    const int optionWidth = 7;
    const int descriptionWidth = 28;
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

void initializeExpert(Expert &expert, const string &name) {
    strcpy(expert.name, name.c_str());
        for (int day = 0; day < DAYS_IN_WEEK; ++day) {
            expert.hoursWorkedPerDay[day] = 0;
            for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                expert.schedule[day][slot].isBooked = false;
                expert.schedule[day][slot].timeRange = to_string(START_HOUR + slot) + ":00 - " + to_string(START_HOUR + slot + 1) + ":00";
                expert.schedule[day][slot].type = SessionType::CONSULTATION; // Default to consultation
            }
        }
    }
    

string paymentMethodToString(PaymentMethod paymentMethod) {
    switch(paymentMethod) {
        case EWALLET: return "E-Wallet";
        case BANK_TRANSFER: return "Bank Transfer";
        case CREDIT_CARD: return "Credit Card";
        default: return "Unknown";
    }
}

void generateReceipt(const Receipt &receipt) {
    cout << "Receipt\n";
    cout << "-------------------------------------\n";
    cout << "Booking Number: " << receipt.bookingNumber << endl;
    cout << "Customer Name: " << receipt.customer.name << endl;
    cout << "Expert: " << receipt.expert.name << endl;
    cout << "Session: " << (receipt.sessionType == SessionType::TREATMENT ? "Treatment" : "Consultation") << endl;
    cout << "Service: " << receipt.serviceName << endl;
    cout << "Time Slot: " << receipt.timeSlot << endl;
    cout << "Payment Method: " << paymentMethodToString(receipt.paymentMethod) << "\n";
    cout << "Amount Paid: RM " << fixed << setprecision(2) << receipt.amountPaid << endl;
    cout << "-------------------------------------\n";
}

void displaySchedule(const Expert &expert, int week) {
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string BLUE = "\033[34m";
    const string RESET = "\033[0m";

    if (week < 0 || week >= 4) {
        cout << "Invalid week number. Please select a week from 1 to 4." << endl;
        return;
    }
    const int dayWidth = 19;  // Width for each day (3 slots + separators)
    const int slotWidth = 12;

    const string days[5] = {"Mon", "Tue", "Wed", "Thu", "Fri"};
    int startDate = 1 + (week * 7);
    int padding = (dayWidth - 1 - days[0].length()) / 2;

    cout << string(dayWidth * 6 + 2, '-') << endl;
    cout << "|" << setw(dayWidth) << left << "Time"; 
    for (int i=0; i<5; ++i) {
        int date = startDate + i;
        string header = days[i] + " (" + to_string(date) + ")";
        cout << "|" << setw(padding) << "" << header << setw(dayWidth - 1 - padding - header.length()) << "";
    }
    cout << "|" << endl;

    cout << string(dayWidth * 6 + 2, '-') << endl;

    for (int i = 0; i < MAX_SLOTS_PER_DAY; i++) {
        cout << "|" << BLUE << setw(4) << left << "[" + to_string(i+1) + "]" << RESET // Align the index
     << setw(dayWidth - 4) << left << expert.schedule[0][i].timeRange; // Align the time range

        for (int day = 0; day < DAYS_IN_WEEK; day++) {
            if (expert.schedule[day][i].isBooked) {
                padding = (dayWidth - string("Booked").length()) / 2;
                cout << "|" << RED << string(padding, ' ') << "Booked" << string(padding, ' ');
                cout << RESET;
            } else if (expert.hoursWorkedPerDay[day] >= MAX_WORK_HOURS) {
                padding = (dayWidth - string("Unavailable").length()) / 2;
                int rightPadding = dayWidth - string("Unavailable").length() - padding - 1;
                cout << "|" << RED << string(padding, ' ') << "Unavailable" << string(rightPadding, ' ');
                cout << RESET;
            } else {
                padding = (dayWidth - string("Open").length()) / 2;
                cout << "|" << GREEN << string(padding, ' ') << "Open" << string(padding, ' ') << "";
                cout << RESET;
            }
        }
        cout << "|\n";
    }
}

bool canBookSlot(const Expert &expert, int day, int slot, SessionType sessionType) {
    int duration = (sessionType == SessionType::TREATMENT) ? TREATMENT_SLOT_DURATION : CONSULTATION_SLOT_DURATION;
    if (expert.hoursWorkedPerDay[day] + duration > MAX_WORK_HOURS) {
        return false;
    }
    for (int i = 0; i < duration; ++i) {
        if (slot + i >= MAX_SLOTS_PER_DAY || expert.schedule[day][slot + i].isBooked) {
            return false;
        }
    }
    return true;
}

int chooseWeek() {
    int choice;
    cout << "Enter week number (1-4): ";
    cin >> choice;

    if (choice < 1 || choice > 4) {
        cout << "Invalid choice. Please select a valid week.\n";
        return chooseWeek();
    }
    return --choice;
}

int* selectTimeSlot(const Expert &expert,int chosenWeek, SessionType sessionType) {
    static int result[2];
    
    cout << "Select a day (1-5 for Mon-Fri): ";
    int selectedDay;
    cin >> selectedDay;
    selectedDay -= 1;
    
    cout << "Select a starting time slot (1-" << MAX_SLOTS_PER_DAY << "): ";
    int selectedSlot;
    cin >> selectedSlot;
    selectedSlot -= 1;

    if (chosenWeek >=0 && chosenWeek < 4 &&
        selectedDay >= 0 && selectedDay < DAYS_IN_WEEK &&
        selectedSlot >= 0 && selectedSlot < MAX_SLOTS_PER_DAY &&
        canBookSlot(expert, selectedDay, selectedSlot, sessionType)) {
        result[0] = selectedDay;
        result[1] = selectedSlot;
        return result;
    }

    cout << "Invalid selection, slot(s) already booked, or exceeds daily work limit." << endl;
    result[0] = -1;
    result[1] = -1;
    return result;
}

void saveBooking(const Receipt& receipt) {
    ofstream bookingsFile("bookings.txt", ios::app);
    if (!bookingsFile.is_open()) {
        cerr << "Error: Unable to open receipts file for writing." << endl;
        return;
    }

    bookingsFile << receipt.bookingNumber << ", " 
         << receipt.customer.name << ", "
         << receipt.customer.email << ", " 
         << receipt.customer.contact << ", "
         << receipt.expert.name << ", "
         << receipt.serviceName << ", "
         << static_cast<int>(receipt.sessionType) << ", "
         << receipt.date << ", " 
         << receipt.timeSlot << ", "
         << static_cast<int> (receipt.paymentMethod) << ", "
         << receipt.amountPaid << "\n";

    bookingsFile.close();
}

int loadBookings(Receipt receipts[]) {
   const int MAX_BOOKINGS = 150;
    ifstream bookingsFile("bookings.txt");
    if (!bookingsFile.is_open()) {
        cerr << "Error: Unable to open bookings file for reading." << endl;
        return 0;
    }
    string line;
    int count = 0;

    while (getline(bookingsFile, line) && count < MAX_BOOKINGS) {
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);
        string item;
        string row[11];

        int row_count = 0;
        while (getline(ss, item, ',')) {
            
            row[row_count] = item;
            row_count++;
        }

        receipts[count].bookingNumber = row[0];
        receipts[count].customer.name = row[1];
        receipts[count].customer.email = row[2];
        receipts[count].customer.contact = row[3];
        strncpy(receipts[count].expert.name, row[4].c_str(), sizeof(receipts[count].expert.name) - 1);
        receipts[count].serviceName = row[5];
        receipts[count].sessionType = static_cast<SessionType> (stoi(row[6]));
        receipts[count].date =row[7];
        receipts[count].timeSlot = row[8];
        receipts[count].paymentMethod = static_cast<PaymentMethod>(stoi(row[9]));
        receipts[count].amountPaid = stod(row[10]);

        count++;
    } 
    bookingsFile.close();
    return count;
}

void displayCustomerBookings(Customer customer) {
    Receipt allReceipts[150];
    int receiptCount = loadBookings(allReceipts);

    cout << "Bookings for " << customer.name << endl;
    cout << "+--------+------------------------------+" << endl;
    for (int i=0; i<receiptCount; ++i) {
        cout << "\nBooking Number: " << allReceipts[i].bookingNumber << endl
             << "Service: " << allReceipts[i].serviceName << endl
             << "Expert: " << allReceipts[i].expert.name << endl
             << "Date: " << allReceipts[i].date << " October 2024" << endl
             << "Time: " << allReceipts[i].timeSlot << endl
             << "Price: RM " << fixed << setprecision(2) << allReceipts[i].amountPaid << endl;
    }

}
void createDirectoryIfNotExists(const string &directoryName) {
    if (ACCESS(directoryName.c_str(), 0) != 0) {
        MKDIR(directoryName.c_str());
    }

}

void saveScheduleToFile(const Expert &expert, int weekNumber) {
    createDirectoryIfNotExists("schedules");
    string filename = "schedules/" + string(expert.name) + "_week" + to_string(weekNumber + 1) + "_schedule.txt";
    ofstream outSchedule(filename);

    if (outSchedule.is_open()) {
        for (int day = 0; day < DAYS_IN_WEEK; ++day) {
            outSchedule << "Day " << day + 1 << endl; 
            outSchedule << expert.hoursWorkedPerDay[day];

            for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                outSchedule << ' ' << expert.schedule[day][slot].isBooked;
                outSchedule << ' ' << (expert.schedule[day][slot].type == SessionType::TREATMENT ? "T" : 
                                (expert.schedule[day][slot].type == SessionType::CONSULTATION ? "C" : "U")) << ' ';
            }
            outSchedule << endl;
        }
    outSchedule.close();
    } else {
        cerr << "Error opening file for writing: " << filename << endl;
    }
}

void loadScheduleFromFile(Expert &expert, int weekNumber) {
    string filename = "schedules/" + string(expert.name) + "_week" + to_string(weekNumber + 1) + "_schedule.txt";
    ifstream scheduleFile(filename);

    if (scheduleFile.is_open()) {
        string line;
        int week = -1;
        int day = -1;

        while (getline(scheduleFile, line)) {
            if (line.find("Day") == 0) {
                day = stoi(line.substr(4)) -1;
                continue;
            }
            if (day >= 0) {
                stringstream ss(line);
                int hoursWorked;
                if (!(ss >> hoursWorked)) {
                    cerr << "Error parsing hoursWorked: " << line << endl;
                    continue;
                }
                expert.hoursWorkedPerDay[day] = hoursWorked;
                
                for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                    char isBooked, typeChar;
                    if (!(ss >> isBooked >> typeChar)) {
                        cerr << "Error reading slot data for Week " << week + 1 << " Day " << day + 1 << " from line: " << line << endl;
                        break;
                    }
                    expert.schedule[day][slot].isBooked = (isBooked == '1');
                    if (typeChar == 'T') expert.schedule[day][slot].type = SessionType::TREATMENT;
                    else if (typeChar == 'C') expert.schedule[day][slot].type = SessionType::CONSULTATION;
                    else expert.schedule[day][slot].type = SessionType::UNAVAILABLE;
                }
            }
        }
        scheduleFile.close();
    } else {
        cout << "No existing schedule found for " << expert.name << ". Starting with a clean schedule." << endl;
    }
}

PaymentMethod selectPaymentMethod() {
    system("clear");
    cout << "Select payment method:\n";
    cout << "1. E-Wallet\n";
    cout << "2. Bank Transfer\n";
    cout << "3. Credit Card\n";
    cout << "Enter your choice (1-3): ";
    int choice;
    cin >> choice;

    switch (choice) {
        case 1: return EWALLET;
        case 2: return BANK_TRANSFER;
        case 3: return CREDIT_CARD;
        default: 
            cout << "Invalid choice. Defaulting to E-Wallet." << endl;
            return EWALLET;
    }    
}

int loadBookingCounter(const string& filename) {
    int counter = 0;
    ifstream bookingCounterFile(filename);

    if (bookingCounterFile.is_open()) {
        bookingCounterFile >> counter;
        bookingCounterFile.close();
    }
    return counter;
}

void saveBookingCounter(const string& filename, int counter) {
    ofstream bookingCounterFile(filename);
    if (bookingCounterFile.is_open()) {
        bookingCounterFile << counter;
        bookingCounterFile.close();
    } else {
        cerr << "Error: Unable to open file " << filename << endl;
    }
}

string generateBookingNumber() {
    static int bookingCounter = loadBookingCounter("booking_counter.txt");
    stringstream ss;
    ss << "B" << setw(3) << setfill('0') << ++bookingCounter;
    saveBookingCounter("booking_counter.txt", bookingCounter);
    return ss.str();
}

void generateReceiptFile(const Receipt &receipt, const string& filename) {
    ofstream receiptFile(filename);

    if (!receiptFile.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }
    receiptFile << "Receipt\n";
    receiptFile << "-----------------------------\n";
    receiptFile << "Booking Number: " << receipt.bookingNumber << endl;
    receiptFile << "Customer Name: " << receipt.customer.name << endl;
    receiptFile << "Expert: " << receipt.expert.name << endl;
    receiptFile << "Session: " << (receipt.sessionType == SessionType::TREATMENT ? "Treatment" : "Consultation") << endl;
    receiptFile << "Service: " << receipt.serviceName << endl;
    receiptFile << "Time Slot: " << receipt.timeSlot << endl;
    receiptFile << "Payment Method: " << paymentMethodToString(receipt.paymentMethod) << "\n";
    receiptFile << "Amount Paid: RM " << fixed << setprecision(2) << receipt.amountPaid << endl;
    receiptFile << "-----------------------------\n";

    receiptFile.close();
}

void printReceipt (const string &filename) {
    #ifdef _WIN32
    ShellExecute(NULL, "open", filename.c_str(), NULL, NULL, SW_SHOWNORMAL); 
    #else   
    string command = "open " + filename;
    system(command.c_str());
    #endif
}


void makeBooking(Expert &expert, Service service, SessionType sessionType, Customer &customer) {
    string weekStartDates[4] = {"2024-070", "2024-09-09", "2024-09-16", "2024-09-23"};    
    int chosenWeek = chooseWeek();
    loadScheduleFromFile(expert, chosenWeek);
    displaySchedule(expert, chosenWeek);
    double price = sessionType == SessionType::TREATMENT ? service.price : 60.0;

    int* result = selectTimeSlot(expert, chosenWeek, sessionType);
    int day = result[0];
    int slot = result[1];

    int startDate = 1 + (chosenWeek * 7);
    int date = startDate + day;

    if (day != -1 && slot != -1) {
        int duration = (sessionType == SessionType::TREATMENT) ? TREATMENT_SLOT_DURATION : CONSULTATION_SLOT_DURATION;
        for (int i = 0; i < duration; ++i) {
            expert.schedule[day][slot + i].isBooked = true;
            expert.schedule[day][slot + i].type = sessionType;
        }
        expert.hoursWorkedPerDay[day] += duration;

        string startTime = to_string(START_HOUR + slot) + ":00";
        string endTime = to_string(START_HOUR + slot + duration) + ":00";
        system("clear");
        cout << "Please confirm your booking details: \n";
        cout << "Service: " << service.name << endl;
        cout << "Session Type: " << (sessionType == SessionType::TREATMENT ? "Treatment" : "Consultation") << endl;
        cout << "Date: " << to_string(date) << " October 2024" << endl;
        cout << "Time Slot: " << startTime << " - " << endTime << endl;
        cout << "Price: RM " << fixed << setprecision(2) << price << endl;
        cout << "Confirm booking? (Y/N): ";
        char confirm;
        cin >> confirm;

        if (tolower(confirm) == 'y') {
            PaymentMethod PaymentMethod = selectPaymentMethod();
            string bookingNumber = generateBookingNumber();
            Receipt receipt = {bookingNumber, customer, expert, sessionType, service.name, to_string(date), startTime + " - " + endTime, PaymentMethod, price};
            generateReceipt(receipt);

            cout << "You have successfully booked the slot on Day " << day + 1
                << " from " << expert.schedule[day][slot].timeRange.substr(0, 5) << " to " << endTime
                << " with " << expert.name << " for " << service.name << " (" 
                << (sessionType == SessionType::TREATMENT ? "Treatment" : "Consultation") << ")." << endl;

            string receiptFileName = "receipt_" + bookingNumber + ".txt";
            generateReceiptFile(receipt, receiptFileName);
            printReceipt(receiptFileName);

            saveBooking(receipt);
            saveScheduleToFile(expert, chosenWeek); // Save updated schedule to file
        } else {
            cout << "Booking cancelled." << endl;
        }
    }
}

void customerManagement() {
    Customer customers[100];
    const int optionWidth = 7;
    const int descriptionWidth = 28;
    int choice, customerCount;
    int loggedInCustomerIndex = -1;
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
                loggedInCustomerIndex = customerLogin(customers, customerCount);
                if (loggedInCustomerIndex != -1){
                    customerMenu(customers[loggedInCustomerIndex]);
                }
                break;
            case 3: 
                cout << "Returning to Main Menu\n"; break;
        }
        // if (choice != 3) pauseAndClear();
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
    const int MAX_CUSTOMERS = 100;
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

int customerLogin(Customer customers[], int customerCount) {
    string email, password;
    cout << "\n=== Customer Login ===" << endl;
    cout << "Enter your email: ";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    getline(cin, email);

    cout << "Enter your password: ";
    password = getPasswordInput();

    for (int i=0; i<customerCount; i++) {
        if (customers[i].email == email && customers[i].password == password) {
            return i;
        }
    }
    cout << "Login failed. Please try again.\n";
    pauseAndClear();
    return -1;
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
    const int MAX_CUSTOMERS = 100;
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

void customerMenu(Customer &customer) {
    const int optionWidth = 7;
    const int descriptionWidth = 28;
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
        cout << "| " << setw(optionWidth - 1) << "6" << " | " << setw(descriptionWidth) << "View My Bookings" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "7" << " | " << setw(descriptionWidth) << "Return to Main Menu" << " |" << endl;

        cout << "+--------+------------------------------+" << endl;
        cout << "Enter your choice: ";

        choice = getValidatedInput(1, 7);

        switch (choice) {
            case 1: aboutUs(); break;
            case 2: viewServices(customer); break;
            case 3: viewExperts(); break;
            // case 4: checkSchedule(); break;
            // case 5: makeBooking(); break;
            case 6: displayCustomerBookings(customer); break;
            case 7: cout << "Returning to Main Menu\n"; break;
        }

        if (choice != 7) pauseAndClear();
    } while (choice != 7);
}

string getPasswordInput() {
#ifdef _WIN32
    string password;
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

    User alice = {"alice123", "Alice1234$", UserType::EXPERT};
    User bob = {"bob123", "Bob1234$", UserType::EXPERT};
    User carol = {"carol123", "Carol1234$", UserType::EXPERT};
    User admin = {"admin123", "Admin1234$", UserType::ADMIN};

    User users[] = {alice, bob, carol, admin};

    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter password: ";
    password = getPasswordInput();

    for (int i=0; i<4; i++) {
        if (users[i].username == username && users[i].password == password) {
            return true;
        }
    }

    cout << "Login failed. Please try again.\n";
    pauseAndClear();
    return false;
}

// void logout() {
//     if (currentUser) {
//         currentUser->isLoggedIn = false;
//         cout << "Logged out successfully. Goodbye, " << currentUser->username << "!\n";
//         currentUser = nullptr;
//     } else {
//         cout << "No user is currently logged in.\n";
//     }
// }

void adminExpertMenu() {
    const int optionWidth = 7;
    const int descriptionWidth = 28;
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
            // case 2: viewServices(); break;
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

void serviceDesc(Service service, Customer &customer) {
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
        Expert alice, bob, carol;
        initializeExpert(alice, "Alice");
        initializeExpert(bob, "Bob");
        initializeExpert(carol, "Carol");
        Expert experts[] = {alice, bob, carol};

        cout << "Available experts:\n";
        cout << "1. Alice\n";
        cout << "2. Bob\n";
        cout << "3. Carol\n";
        cout << "Select an expert (1-3): ";
        int expertChoice;
        cin >> expertChoice;

        Expert &selectedExpert = experts[expertChoice - 1];

        cout << "Select session type:\n";
        cout << "1. Treatment (2 hours)\n";
        cout << "2. Consultation (1 hour)\n";
        cout << "Enter your choice (1-2): ";
        
        int sessionChoice;
        cin >> sessionChoice;

        SessionType sessionType;
        switch (sessionChoice) {
            case 1: sessionType = SessionType::TREATMENT; break;
            case 2: sessionType = SessionType::CONSULTATION; break;
            default:
                cout << "Invalid choice. Exiting.";
                return;
        }
        makeBooking(selectedExpert, service, sessionType, customer);
    }

}

void viewServices(Customer &customer) {
    clearScreen();
    displayLogo();

    Service facial = { "Facial", "A rejuvenating facial treatment.", 150.00};
    Service botox = { "Botox and Fillers", "Cosmetic injections for wrinkle treatment.", 250.00};
    Service manicure = {"Manicure", "A relaxing manicure sesion.", 100.00};
    
    Service services[3] = {facial, botox, manicure};
    const int optionWidth = 7;
    const int descriptionWidth = 28;
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

    serviceDesc(services[choice - 1], customer);
}

void viewExperts() {
    clearScreen();
    displayLogo();

    cout << "\nExperts:\n";
    

}

void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("pause");
    system("cls");
#else
    // system("pause");
    system("clear");
#endif
}

void pauseAndClear() {
    cout << "\nPress Enter to continue...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
    clearScreen();
}



