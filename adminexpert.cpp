#include <iostream>
#include <cstdio>
#include <iomanip>
#include <limits>
#include <sstream>
#include <fstream>
#include <regex>
#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #include <direct.h>
    #include <io.h>
    #define ACCESS _access
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
    string name;
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
bool isValidName(const string&);
bool isValidContact(const string&);
bool isValidEmail(const string&);
bool isValidPassword(const string&);
void initializeExpert(Expert&, const string&);
void initializeService(Service&, string, double);
string paymentMethodToString(PaymentMethod);
void generateReceipt(const Receipt&);
void saveBooking(const Receipt&);
int loadBookings(Receipt[]);
void displayCustomerBookings(Customer customer);
void generateSalesReport();
void displaySchedule(const Expert&, int);
bool canBookSlot(const Expert&, int, int, SessionType);
void displayCustomers(const Customer[], int, int[]);
void displayCustomerDetails(Customer);
void sortCustomersByName(Customer[], int, int[]);
void sortCustomersByExpertBookings(Customer[], int, const Receipt[], int, const string&, int[]);
void sortCustomersByTotalBookings(Customer[], int, int[]);
int countCustomerExpertBookings(const Receipt[], int, const string&, const string&);
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
void adminExpertMenu(UserType&, string&);
void viewExpertSchedule(const string&);
void initializeCleanSchedule(Expert&);
void viewAllShedules();
void aboutUs();
void viewCustomers(string);
int getValidatedInput(int min, int max);
bool login(UserType&, string&);
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
    UserType userType;
    string userName;
    do {
        displayLogo();
        displayMainMenu();


        choice = getValidatedInput(1, 3);

        switch(choice) {
            case 1:
                customerManagement();
                break;
            case 2:
                if (login(userType, userName)) {
                    adminExpertMenu(userType, userName);
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
    expert.name = name;
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

void generateReceipt(const Receipt& receipt) {
    cout << "*********************************************\n";
    cout << "*               SERVICE RECEIPT             *\n";
    cout << "*********************************************\n\n";
    cout << left << setw(20) << "Booking Number:" << receipt.bookingNumber << endl;
    cout << left << setw(20) << "Customer Name:" << receipt.customer.name << endl;
    cout << left << setw(20) << "Expert:" << receipt.expert.name << endl;
    cout << left << setw(20) << "Session:" << (receipt.sessionType == SessionType::TREATMENT ? "Treatment" : "Consultation") << endl;
    cout << left << setw(20) << "Service:" << receipt.serviceName << endl;
    cout << left << setw(20) << "Time Slot:" << receipt.timeSlot << endl;
    cout << left << setw(20) << "Payment Method:" << paymentMethodToString(receipt.paymentMethod) << endl;
    cout << left << setw(20) << "Amount Paid:" << "RM " << fixed << setprecision(2) << receipt.amountPaid << endl;
    cout << "\n---------------------------------------------\n";
    cout << "Thank you for choosing our services!\n";
    cout << "For inquiries, call us at +60-123-4567 or\n";
    cout << "email us at looksmaxxlounget@serviceprovider.com\n";
    cout << "---------------------------------------------\n";
}

void displaySchedule(const Expert &expert, int week) {
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string BLUE = "\033[34m";
    const string RESET = "\033[0m";

    if (week < 0 || week >= 5) {
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
    cout << string(dayWidth * 6 + 2, '-') << "\n\n";
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
   const int MAX_BOOKINGS = 200;
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
        receipts[count].expert.name = row[4];
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

string trim(const string& str) {
    if (str.empty()) {
        return "";
    }
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}


void displayCustomerBookings(Customer customer) {
    Receipt allReceipts[150];
    int receiptCount = loadBookings(allReceipts);

    cout << "Bookings for " << customer.name << " " << customer.email << endl;
    cout << "+--------+------------------------------+" << endl;

    bool hasBookings = false;
    for (int i = 0; i < receiptCount; ++i) {

        if (trim(allReceipts[i].customer.email) == trim(customer.email)) {
            hasBookings = true;
            cout << "********************************************\n";
            cout << "*           CUSTOMER BOOKING DETAILS       *\n";
            cout << "********************************************\n\n";

            cout << left << setw(20) << "Booking Number:" << allReceipts[i].bookingNumber << endl;
            cout << left << setw(20) << "Service:" << allReceipts[i].serviceName << endl;
            cout << left << setw(20) << "Expert:" << allReceipts[i].expert.name << endl;
            cout << left << setw(20) << "Customer Email:" << allReceipts[i].customer.email << endl;
            cout << left << setw(20) << "Booking Date:" << allReceipts[i].date << " October 2024" << endl;
            cout << left << setw(20) << "Time Slot:" << allReceipts[i].timeSlot << endl;
            cout << left << setw(20) << "Price: RM " << fixed << setprecision(2) << allReceipts[i].amountPaid << endl;

            cout << "\n--------------------------------------------\n";
            cout << "Please arrive 10 minutes before your time slot.\n";
            cout << "--------------------------------------------\n";
        }
    }

    if (!hasBookings) {
        cout << "No bookings found for this customer." << endl;
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
        initializeCleanSchedule(expert);
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
    receiptFile << "*********************************************\n";
    receiptFile << "*               SERVICE RECEIPT             *\n";
    receiptFile << "*********************************************\n\n";
    receiptFile << left << setw(20) << "Booking Number:" << receipt.bookingNumber << endl;
    receiptFile << left << setw(20) << "Customer Name:" << receipt.customer.name << endl;
    receiptFile << left << setw(20) << "Expert:" << receipt.expert.name << endl;
    receiptFile << left << setw(20) << "Session:" << (receipt.sessionType == SessionType::TREATMENT ? "Treatment" : "Consultation") << endl;
    receiptFile << left << setw(20) << "Service:" << receipt.serviceName << endl;
    receiptFile << left << setw(20) << "Time Slot:" << receipt.timeSlot << endl;
    receiptFile << left << setw(20) << "Payment Method:" << paymentMethodToString(receipt.paymentMethod) << endl;
    receiptFile << left << setw(20) << "Amount Paid:" << "RM " << fixed << setprecision(2) << receipt.amountPaid << endl;
    receiptFile << "+-----------------------------+\n";
    receiptFile << "\n---------------------------------------------\n";
    receiptFile << "Thank you for choosing our services!\n";
    receiptFile << "For inquiries, call us at +60-123-4567 or\n";
    receiptFile << "email us at looksmaxxlounge@serviceprovider.com\n";
    receiptFile << "---------------------------------------------\n";

    receiptFile.close();
}

void printReceipt (const string &filename) {
    #ifdef _WIN32
    // ShellExecute expects wide-character strings for the operation and filename
    ShellExecute(NULL, L"open", wstring(filename.begin(), filename.end()).c_str(), NULL, NULL, SW_SHOWNORMAL);
    #else   
    string command = "open " + filename;
    system(command.c_str());
    #endif
}


void makeBooking(Expert &expert, Service service, SessionType sessionType, Customer &customer) {
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

bool isValidName(const string& name) {
    const regex pattern("^[A-Za-z]+(?: [A-Za-z]+)*$");
    return regex_match(name, pattern);
}

bool isValidContact(const string& contact) {
    const regex pattern("^\\d{3}-\\d{3}-\\d{4}$");
    return regex_match(contact, pattern);
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
    do {
        cout << "Enter your name: ";
        cin.ignore();
        getline(cin, newCustomer.name);
        if (!isValidName(newCustomer.name)) {
            cout << "\nInvalid name. Please try again.\n";
        }
    } while (!isValidName(newCustomer.name));

    do {
        cout << "Enter your contact number: ";
        getline(cin, newCustomer.contact);
        if (!isValidContact(newCustomer.contact)) {
            cout << "\nInvalid contact number format.\nExample of contact: 123-456-7890\nPlease try again.\n";
        }
    } while (!isValidContact(newCustomer.contact));

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
    cin.ignore(1000, '\n');
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

bool login(UserType &userType, string &userName) {
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
    cin.ignore(1000, '\n');
    cout << "Enter password: ";
    password = getPasswordInput();

    for (int i=0; i<4; i++) {
        if (users[i].username == username && users[i].password == password) {
            userType = users[i].type;
            userName = users[i].username.substr(0, users[i].username.length()-3);
            return true;
        }
    }

    cout << "Login failed. Please try again.\n";
    pauseAndClear();
    return false;
}

void viewExpertSchedule(const string& expertName) {
    Expert expert;
    initializeExpert(expert, expertName);
    for (int week=0; week < 4; ++week) {
        loadScheduleFromFile(expert, week);
        cout << "Week " << week + 1 << " Schedule for " << expertName << ":\n";
        bool hasBookings = false;
        for (int day = 0; day < 5; ++day) {
            for (int slot = 0; slot < 8; ++slot) {
                if (expert.schedule[day][slot].isBooked) {
                    hasBookings = true;
                    break;
                }
            }
            if (hasBookings) break;
        }
        if (!hasBookings) {
            cout << "No bookings for this week.\n";
        }
        displaySchedule(expert, week);
    }
}

void initializeCleanSchedule(Expert &expert) {
    for (int day = 0; day < DAYS_IN_WEEK; ++day) {
        expert.hoursWorkedPerDay[day] = 0;
        for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
            expert.schedule[day][slot].isBooked = false;
            expert.schedule[day][slot].timeRange = to_string(START_HOUR + slot) + ":00 - " + to_string(START_HOUR + slot + 1) + ":00";
            expert.schedule[day][slot].type = SessionType::CONSULTATION; // Default to consultation
        }
    }
}

void viewAllSchedules() {
    string expertNames [] = {"Alice", "Bob", "Carol"};
    for (int i = 0; i < 3; i++) {
        viewExpertSchedule(expertNames[i]);
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

void displayCustomers(const Customer customers[], int customerCount, int bookingCounts[]) {
    const string BLUE = "\033[34m";
    const string RESET = "\033[0m";
    int choice;
    cout << "+-----+-----------------------------+---------------------------+-----------------+" << endl;
    cout << "| No  | Customer Name               | Email                     | Booking Count   |" << endl;
    cout << "+-----+-----------------------------+---------------------------+-----------------+" << endl;
    for (int i = 0; i < customerCount; ++i) {
        cout << "| " << setw(2) << BLUE << "[" << i+1 << "]" << RESET << " | " << setw(27) << customers[i].name 
             << " | " << setw(25) << customers[i].email 
             << " | " << setw(15) << bookingCounts[i] << " |" << endl;
    }
    cout << "+----+-----------------------------+---------------------------+-----------------+" << endl;
    cout << "\nSelect a customer to view their information: " << endl;
    choice = getValidatedInput(1, customerCount);

    displayCustomerDetails(customers[choice-1]);
}

void generateSalesReport() {
    Receipt allReceipts[200];
    int receiptCount = loadBookings(allReceipts);
    string services[] = {"Facial", "Botox and Fillers", "Manicure"};
    string experts[] = {"Alice", "Bob", "Carol"};
    double facialRevenue = 0, botoxRevenue = 0, manicureRevenue = 0;
    double aliceRevenue = 0, bobRevenue = 0, carolRevenue = 0;

    if(receiptCount == 0) {
        cout << "No bookings found. Unable to generate sales report." << endl;
        return;
    }

    double totalRevenue = 0;
    int totalBookings = 0;

    for (int i=0; i<receiptCount; ++i) {
        totalRevenue += allReceipts[i].amountPaid;
        totalBookings++;

        // Revenue per service
        if (trim(allReceipts[i].serviceName) == "Facial") {
            facialRevenue += allReceipts[i].amountPaid;
        } else if (trim(allReceipts[i].serviceName) == "Botox and Fillers") {
            botoxRevenue += allReceipts[i].amountPaid;
        } else if (trim(allReceipts[i].serviceName) == "Manicure") {
            manicureRevenue += allReceipts[i].amountPaid;
        }

        // Revenue per expert
        if (trim(allReceipts[i].expert.name) == "Alice") {
            aliceRevenue += allReceipts[i].amountPaid;
        } else if (trim(allReceipts[i].expert.name) == "Bob") {
            bobRevenue += allReceipts[i].amountPaid;
        } else if (trim(allReceipts[i].expert.name) == "Carol") {
            carolRevenue += allReceipts[i].amountPaid;
        }
    }
    // Display the sales report
    cout << "\n+-----------------------------------+" << endl;
    cout << "|          Sales Report             |" << endl;
    cout << "+-----------------------------------+" << endl;
    cout << "| Total Bookings: " << setw(20) << totalBookings << " |" << endl;
    cout << "| Total Revenue : RM " << setw(20) << fixed << setprecision(2) << totalRevenue << " |" << endl;
    cout << "+-----------------------------------+" << endl;

    // Display breakdown by service
    cout << "\nRevenue by Service:\n";
    cout << "+-------------------------+-----------------+" << endl;
    cout << "| Service                 | Revenue (RM)     |" << endl;
    cout << "+-------------------------+-----------------+" << endl;
    cout << "| " << setw(23) << left << "Facial" << " | RM " << setw(13) << right << fixed << setprecision(2) << facialRevenue << " |" << endl;
    cout << "| " << setw(23) << left << "Botox and Fillers" << " | RM " << setw(13) << right << fixed << setprecision(2) << botoxRevenue << " |" << endl;
    cout << "| " << setw(23) << left << "Manicure" << " | RM " << setw(13) << right << fixed << setprecision(2) << manicureRevenue << " |" << endl;
    cout << "+-------------------------+-----------------+" << endl;

    // Display breakdown by expert
    cout << "\nRevenue by Expert:\n";
    cout << "+-------------------------+-----------------+" << endl;
    cout << "| Expert                  | Revenue (RM)     |" << endl;
    cout << "+-------------------------+-----------------+" << endl;
    cout << "| " << setw(23) << left << "Alice" << " | RM " << setw(13) << right << fixed << setprecision(2) << aliceRevenue << " |" << endl;
    cout << "| " << setw(23) << left << "Bob" << " | RM " << setw(13) << right << fixed << setprecision(2) << bobRevenue << " |" << endl;
    cout << "| " << setw(23) << left << "Carol" << " | RM " << setw(13) << right << fixed << setprecision(2) << carolRevenue << " |" << endl;
    cout << "+-------------------------+-----------------+" << endl;

}

void displayCustomerDetails(Customer customer) {
    cout << "\n+-----------------------------------+" << endl;
    cout << "|          Customer Details         |" << endl;
    cout << "+-----------------------------------+" << endl;
    cout << "| Name    : " << setw(23) << left << customer.name << " |" << endl;
    cout << "| Email   : " << setw(23) << left << customer.email << " |" << endl;
    cout << "| Contact : " << setw(23) << left << customer.contact << " |" << endl;
    cout << "+-----------------------------------+" << endl;
    // For security reasons, don't display the password
}

// Function to to count bookings for a specific customer and expert
int countCustomerExpertBookings(const Receipt receipts[], int receiptCount, const string& customerName, const string& expertName) {
    int count = 0;
    for (int i = 0; i < receiptCount; ++i) {
        if (trim(receipts[i].customer.name) == trim(customerName) && trim(receipts[i].expert.name) == trim(expertName)) {
            ++count;
        }
    }
    return count;
}

// Sort customers by their bookings with a specific expert
void sortCustomersByExpertBookings(Customer customers[], int customerCount, const Receipt receipts[], int receiptCount, const string& expertName, int bookingCounts[]) {
    for (int i = 0; i < customerCount - 1; ++i) {
        for (int j = i + 1; j < customerCount; ++j) {
            int bookingsA = countCustomerExpertBookings(receipts, receiptCount, customers[i].name, expertName);
            int bookingsB = countCustomerExpertBookings(receipts, receiptCount, customers[j].name, expertName);
            
            if (bookingsA < bookingsB) {
                // Swap customers[i] and customers[j]
                Customer tempCustomer = customers[i];
                customers[i] = customers[j];
                customers[j] = tempCustomer;
                int tempCount = bookingCounts[i];
                bookingCounts[i] = bookingCounts[j];
                bookingCounts[j] = tempCount;
            }
        }
    }
}


void sortCustomersByName(Customer customers[], int customerCount, int bookingCounts[]) {
    for (int i = 0; i < customerCount - 1; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < customerCount; ++j) {
            if (customers[j].name < customers[minIndex].name) {
                minIndex = j;
            }
        }
        // Swap the found minimum element with the first element
        if (minIndex != i) {
            Customer tempCustomer = customers[i];
            customers[i] = customers[minIndex];
            customers[minIndex] = tempCustomer;
            int tempCount = bookingCounts[i];
            bookingCounts[i] = bookingCounts[minIndex];
            bookingCounts[minIndex] = tempCount;
        }
    }
}

void sortCustomersByTotalBookings(Customer customers[], int customerCount, int bookingCounts[]) {
    for (int i = 0; i < customerCount - 1; i++) {
        for (int j = 0; j < customerCount - i - 1; j++) {
            if (bookingCounts[j] < bookingCounts[j + 1]) {
                // Swap customers[i] and customers[j]
                Customer tempCustomer = customers[i];
                customers[i] = customers[j];
                customers[j] = tempCustomer;
                int tempCount = bookingCounts[i];
                bookingCounts[i] = bookingCounts[j];
                bookingCounts[j] = tempCount;
            }
        }
    }
}

void viewCustomers(string expertName = "") {
    Receipt allReceipts[200];
    int receiptCount = loadBookings(allReceipts);
    const int MAX_CUSTOMERS = 100;

    if(!expertName.empty()) {
        expertName[0] = toupper(expertName[0]);
    }

    Customer customers[MAX_CUSTOMERS];
    int bookingCounts[MAX_CUSTOMERS] = {0};
    int customerCount = 0;

    for (int i=0; i<receiptCount; i++) {
        if (!expertName.empty() && trim(allReceipts[i].expert.name) != expertName){
            continue;
        }

        //Check if the customer already exists in the array
        bool found = false;
        for (int j=0; j<customerCount; j++) {
            if (customers[j].email == allReceipts[i].customer.email) {
                bookingCounts[j]++; // Increment booking count fo rexisting customer
                found = true;
                break;
            }
        }

        // If customer is new, add them to the array
        if (!found && customerCount < MAX_CUSTOMERS) {
            customers[customerCount] = allReceipts[i].customer;
            bookingCounts[customerCount] = 1;
            customerCount++;
        }
    }
        int choice;
        clearScreen();
        if (expertName.empty()) {
            cout << "Empty name" << endl;
        } else {
            cout << expertName << endl;
        }
        cout << "+-----------+--------------------------------------------+" << endl;
        cout << "| Option    | Description                                |" << endl;
        cout << "+-----------+--------------------------------------------+" << endl;
        cout << "| 1         | View without sorting                       |" << endl; 
        cout << "| 2         | Sort by customer name                      |" << endl; 
        cout << "| 3         | Sort by customer bookings                  |" << endl;
        cout << "+-----------+--------------------------------------------+" << endl;
        cout << "\nEnter your choice: ";
        choice = getValidatedInput(1, 3);

        switch(choice) {
            case 1:
                displayCustomers(customers, customerCount, bookingCounts);
                break;
            case 2:
                sortCustomersByName(customers, customerCount, bookingCounts);
                displayCustomers(customers, customerCount, bookingCounts);
                break;
            case 3:
                if (expertName.empty()) {
                    sortCustomersByTotalBookings(customers, customerCount, bookingCounts);
                } else {
                    sortCustomersByExpertBookings(customers, customerCount, allReceipts, receiptCount, expertName, bookingCounts);
                }
                displayCustomers(customers, customerCount, bookingCounts);
                break;

        }
    }


void adminExpertMenu(UserType &userType, string &userName) {
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

        if (userType == UserType::EXPERT) {
            cout << "| " << setw(optionWidth - 1) << "1" << " | " << setw(descriptionWidth) << "View My Schedule" << " |" << endl;
        } else if (userType == UserType::ADMIN) {
            cout << "| " << setw(optionWidth - 1) << "1" << " | " << setw(descriptionWidth) << "View All Schedules" << " |" << endl;
        }
        cout << "| " << setw(optionWidth - 1) << "2" << " | " << setw(descriptionWidth) << "View Customers" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "3" << " | " << setw(descriptionWidth) << "Generate Sales Report" << " |" << endl;
        cout << "| " << setw(optionWidth - 1) << "4" << " | " << setw(descriptionWidth) << "Return to Main Menu" << " |" << endl;

        cout << "+--------+------------------------------+" << endl;
        cout << "Enter your choice: ";

        choice = getValidatedInput(1, 4);

        switch (choice) {
            case 1: 
                if (userType == UserType::EXPERT) {
                    viewExpertSchedule(userName);
                } else if (userType == UserType::ADMIN) {
                    viewAllSchedules();
                }
                break;
            case 2: 
                if (userType == UserType::EXPERT) {
                    viewCustomers(userName);
                } else if (userType == ADMIN) {
                    viewCustomers();
                }
            case 3: 
                generateSalesReport();   
                break;
            case 4: cout << "Returning to Main Menu\n"; break;
        }

        if (choice != 4) pauseAndClear();
    } while (choice != 4);
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

        cout << "\n---------------------------\n";
        cout << "     Select Your Expert\n";
        cout << "---------------------------\n";
        cout << "We have the following beauty experts available:\n";
        cout << "  [1]  Alice\n";
        cout << "  [2]  Bob\n";
        cout << "  [3]  Carol\n";
        cout << "\nPlease select an expert by entering the number (1-3): ";
        int expertChoice;
        cin >> expertChoice;

        Expert& selectedExpert = experts[expertChoice - 1];

        cout << "\n---------------------------\n";
        cout << "     Choose Session Type\n";
        cout << "---------------------------\n";
        cout << "Please choose the type of session you’d like:\n";
        cout << "  [1] Full Treatment - 2 hours of deep facial care\n";
        cout << "  [2] Consultation   - 1 hour personalized consultation\n";
        cout << "\nEnter your choice (1-2): ";
        
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
    cin.ignore(1000, '\n');
    cin.get();
    clearScreen();
}



