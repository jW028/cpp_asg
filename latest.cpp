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
#define MKDIR(dir) mkdir(dir, 0777)
#endif
#define START_HOUR 9
#define END_HOUR 17 // Operating hours from 9 AM to 5 PM
#define MAX_WORK_HOURS 6 // Maximum work hours per day for an expert
#define CONSULTATION_SLOT_DURATION 1
#define TREATMENT_SLOT_DURATION 2
#define DAYS_IN_WEEK 5 // Monday to Friday
#define MAX_SLOTS_PER_DAY 8 // Total slots available (8 hours)
#define OPTION_WIDTH 7
#define DESC_WIDTH 28
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

using namespace std;

struct Customer {
    string name;
    string email;
    string contact;
    string password;
};

enum PaymentMethod {
    EWALLET,
    BANK_TRANSFER,
    CREDIT_CARD, 
    CANCELLED
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
void customerSignUp(Customer[], int &customerCount);
int customerLogin(Customer[], int);
bool isValidName(const string&);
bool isValidEmail(const string&);
bool isValidPassword(const string&);
bool isValidPhoneNumber(const string&);
bool isValidBankAccountNumber(const string&);
bool isValidCreditCard(const string&, const string&);
string generateOTP();
bool verifyOTP(const string&);
bool handlePaymentMethod(PaymentMethod);
void initializeExpert(Expert&, const string&);
void initializeService(Service&, string, double);
string paymentMethodToString(PaymentMethod);
void displayExpertDetails(Expert&);
void generateReceipt(const Receipt&);
void saveBooking(const Receipt&);
int loadBookings(Receipt[]);
void saveUpdatedReceipts(Receipt[], int);
void displayCustomerBookings(Customer customer);
void displayBookingInfo(Receipt);
void generateSalesReport();
void displayCalendar(Expert&);
void displaySchedule(const Expert&, int);
bool canBookSlot(const Expert&, int, int, SessionType);
void displayCustomers(const Customer[], int, int[]);
void displayCustomerDetails(Customer);
void sortCustomersByName(Customer[], int, int[]);
void sortCustomersByExpertBookings(Customer[], int, const Receipt[], int, const string&, int[]);
void sortCustomersByTotalBookings(Customer[], int, int[]);
int countCustomerExpertBookings(const Receipt[], int, const string&, const string&);
void processRefund(Receipt&, Receipt[], int&);
void updateExpertSchedule(Receipt&, Expert&);
int chooseWeek();
int* selectTimeSlot(const Expert&, int, SessionType);
void saveScheduleToFile(const Expert&, int);
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
bool adminExpertLogin(UserType&, string&);
string getPasswordInput();
void serviceDesc(Service, Customer&);
void viewServices(Customer&);
void viewExperts();
void checkSchedule();
void viewBookedSchedule();
void pauseAndClearInput();
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

        switch (choice) {
        case 1:
            customerManagement();
            break;
        case 2:
            if (adminExpertLogin(userType, userName)) {
                adminExpertMenu(userType, userName);
            }
            break;
        case 3:
            cout << "Exiting program...\n";
            return 0;
        }
    } while (choice != 3);

    return 0;
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
    cout << "| " << setw(OPTION_WIDTH - 1) << "Option" << " | " << left << setw(DESC_WIDTH) << "Description" << " |" << endl;
    cout << "+--------+------------------------------+" << endl;
    cout << "| " << setw(OPTION_WIDTH - 1) << "1" << " | " << setw(DESC_WIDTH) << "Customer" << " |" << endl;
    cout << "| " << setw(OPTION_WIDTH - 1) << "2" << " | " << setw(DESC_WIDTH) << "Staff" << " |" << endl;
    cout << "| " << setw(OPTION_WIDTH - 1) << "3" << " | " << setw(DESC_WIDTH) << "Exit" << " |" << endl;
    cout << "+--------+------------------------------+" << endl;
    cout << "Enter your choice: ";
}

void initializeExpert(Expert& expert, const string& name) {
    expert.name = name;
    for (int day = 0; day < DAYS_IN_WEEK; ++day) {
        expert.hoursWorkedPerDay[day] = 0;
        for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
            expert.schedule[day][slot].isBooked = false;
            expert.schedule[day][slot].timeRange = to_string(START_HOUR + slot) + ":00 - " + to_string(START_HOUR + slot + 1) + ":00";
            expert.schedule[day][slot].type = CONSULTATION; // Default to consultation
        }
    }
}


string paymentMethodToString(PaymentMethod paymentMethod) {
    switch (paymentMethod) {
    case EWALLET: return "E-Wallet";
    case BANK_TRANSFER: return "Bank Transfer";
    case CREDIT_CARD: return "Credit Card";
    default: return "Unknown";
    }
}

void displaySchedule(const Expert& expert, int week) {

    if (week < 0 || week >= 5) {
        cout << RED << "Invalid week number. Please select a week from 1 to 5." << RESET << endl;
        return;
    }

    const int DAYWIDTH = 19;  // Width for each day (3 slots + separators)
    const int SLOTWIDTH = 12;
    const string days[5] = { "Mon", "Tue", "Wed", "Thu", "Fri" };

    int startDate = 1 + (week * 7);
    int padding = (DAYWIDTH - 1 - days[0].length()) / 2;

    cout << string(DAYWIDTH * 6 + 2, '-') << endl;
    cout << "|" << setw(DAYWIDTH) << left << "Time";

    for (int i = 0; i < 5; ++i) {
        int date = startDate + i;
        string header;
        if (date > 31) {
            header = days[i] + " (Unavailable)";
        }
        else {
            header = days[i] + " (" + to_string(date) + ")";
        }
        padding = (DAYWIDTH - header.length()) / 2;
        cout << "|" << setw(padding) << "" << header << setw(DAYWIDTH - 1 - padding - header.length()) << "";
    }

    cout << "|" << endl;
    cout << string(DAYWIDTH * 6 + 2, '-') << endl;

    for (int i = 0; i < MAX_SLOTS_PER_DAY; i++) {
        cout << "|" << BLUE << setw(4) << left << "[" + to_string(i + 1) + "]" << RESET // Align the index
            << setw(DAYWIDTH - 4) << left << expert.schedule[0][i].timeRange; // Align the time range

        for (int day = 0; day < DAYS_IN_WEEK; day++) {
            int currentDate = startDate + day;

            // Determine the status based on the date
            if (currentDate > 31) {
                // Beyond the last valid day, mark as "Unavailable"
                padding = (DAYWIDTH - string("Unavailable").length()) / 2;
                int rightPadding = DAYWIDTH - string("Unavailable").length() - padding - 1;
                cout << "|" << RED << string(padding, ' ') << "Unavailable" << string(rightPadding, ' ') << RESET;
            }
            else if (expert.schedule[day][i].isBooked) {
                // Slot is booked
                padding = (DAYWIDTH - string("Booked").length()) / 2;
                cout << "|" << RED << string(padding, ' ') << "Booked" << string(padding, ' ') << RESET;
            }
            else if (expert.hoursWorkedPerDay[day] >= MAX_WORK_HOURS) {
                // Slot is unavailable due to max hours worked
                padding = (DAYWIDTH - string("Unavailable").length()) / 2;
                int rightPadding = DAYWIDTH - string("Unavailable").length() - padding - 1;
                cout << "|" << RED << string(padding, ' ') << "Unavailable" << string(rightPadding, ' ') << RESET;
            }
            else {
                // Slot is open
                padding = (DAYWIDTH - string("Open").length()) / 2;
                cout << "|" << GREEN << string(padding, ' ') << "Open" << string(padding, ' ') << RESET;
            }
        }
        cout << "|\n";
    }

    cout << string(DAYWIDTH * 6 + 2, '-') << "\n\n";
}



bool canBookSlot(const Expert& expert, int day, int slot, SessionType sessionType) {
    int duration = (sessionType == TREATMENT) ? TREATMENT_SLOT_DURATION : CONSULTATION_SLOT_DURATION;
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

string trim(const string& str) {
    if (str.empty()) {
        return "";
    }
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

int chooseWeek() {
    int choice;
    cout << "Enter week number (1-5, -999 to go back): ";
    choice = getValidatedInput(1, 5);
    if (choice == -999) {
        return -1;
    }

    if (choice < 1 || choice > 5) {
        cout << RED << "Invalid choice. Please select a valid week.\n" << RESET;
        return chooseWeek();
    }
    return --choice;
}

void displayCalendar(Expert& expert) {

    const int DAYWIDTH = 12;  // Width for each day's display
    const int SLOTWIDTH = 12;
    const string days[5] = { "Mon", "Tue", "Wed", "Thu", "Fri" };

    // Display header for weeks
    cout << "\nAvailable Weeks and Days:" << endl;

    // Loop through each week and day
    for (int week = 0; week < 5; ++week) {
        loadScheduleFromFile(expert, week);
        int availableSlots = 0;
        int startDate = 1 + (week * 7);
        for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
            for (int day = 0; day < 5; ++day) {
                if (week == 4 && (startDate + day) > 31) break; // Skip slots after 31st
                if (!expert.schedule[day][slot].isBooked && expert.schedule[day][slot].type != UNAVAILABLE) {
                    availableSlots++;
                }
            }
        }
        cout << "Week " << (week + 1) << ":" << "(" << GREEN << availableSlots << RESET << " slots available)" << endl;

        // Display days and dates for the week
        cout << string(DAYWIDTH * 6 + 2, '-') << endl;
        cout << "|" << setw(DAYWIDTH) << left << "Day";
        for (int i = 0; i < 5; ++i) {
            int date = startDate + i;
            if (week == 4 && date > 31) break;  // Stop after the 31st
            string header = days[i] + " (" + to_string(date) + ")";
            cout << "|" << setw(DAYWIDTH - 1) << left << header;
        }
        cout << "|" << endl;
        cout << string(DAYWIDTH * 6 + 2, '-') << "\n\n";

        // Display the slots for each day
    }
}


int* selectTimeSlot(const Expert& expert, int chosenWeek, SessionType sessionType) {
    static int result[2];
    int selectedDay;
    if (chosenWeek != 4) {
        cout << "Select a day (1-5 for Mon-Fri, -999 to go back): ";
        selectedDay = getValidatedInput(1, 5);
        if (selectedDay == -999) {
            return nullptr;
        }
        selectedDay -= 1;
    }
    else {
        cout << "Select a day (1-3 for Mon-Wed, -999 to go back): ";
        selectedDay = getValidatedInput(1, 3);
        if (selectedDay == -999) {
            return nullptr;
        }
        selectedDay -= 1;
    }

    cout << "Select a starting time slot (1-" << MAX_SLOTS_PER_DAY << ", -999 to go back): ";
    int selectedSlot;
    selectedSlot = getValidatedInput(1, 8);
    if (selectedSlot == -999) {
        return nullptr;
    }
    selectedSlot -= 1;

    if (chosenWeek >= 0 && chosenWeek < 5 &&
        selectedDay >= 0 && selectedDay < DAYS_IN_WEEK &&
        selectedSlot >= 0 && selectedSlot < MAX_SLOTS_PER_DAY &&
        canBookSlot(expert, selectedDay, selectedSlot, sessionType)) {
        result[0] = selectedDay;
        result[1] = selectedSlot;
        return result;
    }

    cout << RED << "Invalid selection, slot(s) already booked, or exceeds daily work limit." << RESET << endl;
    result[0] = -1;
    result[1] = -1;
    return result;
}

void saveBooking(const Receipt& receipt) {
    ofstream bookingsFile("bookings.txt", ios::app);
    if (!bookingsFile.is_open()) {
        cerr << RED << "Error: Unable to open bookings file for writing." << RESET << endl;
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
        cerr << RED << "Error: Unable to open bookings file for reading." << RESET << endl;
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
        receipts[count].date = row[7];
        receipts[count].timeSlot = row[8];
        receipts[count].paymentMethod = static_cast<PaymentMethod>(stoi(row[9]));
        receipts[count].amountPaid = stod(row[10]);

        count++;
    }
    bookingsFile.close();
    return count;
}

void saveUpdatedReceipts(Receipt allReceipts[], int receiptCount) {
    ofstream file("bookings.txt"); // Replace with actual file name

    if (!file) {
        cout << RED <<  "Error opening file for saving receipts." << RESET << endl;
        return;
    }

    // Write update << d receipt data back to the file
    for (int i = 0; i < receiptCount; i++) {
        file << allReceipts[i].bookingNumber << ",";
        file << allReceipts[i].customer.name << ",";
        file << allReceipts[i].customer.email << ",";
        file << allReceipts[i].customer.contact << ",";
        file << allReceipts[i].expert.name << ",";
        file << allReceipts[i].serviceName << ",";
        file << allReceipts[i].sessionType << ",";
        file << allReceipts[i].date << ",";
        file << allReceipts[i].timeSlot << ",";
        file << allReceipts[i].paymentMethod << ",";
        file << allReceipts[i].amountPaid << "\n";
    }

    file.close();
}


void updateExpertSchedule(Receipt& receipt, Expert& expert) {
    int week, receiptDay, slot;
    int date = stoi(receipt.date);
    string timeSlot = trim(receipt.timeSlot);
    if (date >= 1 && date <= 5) {
        week = 0;
        receiptDay = date - 1;
    }
    else if (date >= 8 && date <= 12) {
        week = 1;
        receiptDay = date - 8;
    }
    else if (date >= 15 && date <= 19) {
        week = 2;
        receiptDay = date - 15;
    }
    else if (date >= 22 && date <= 26) {
        week = 3;
        receiptDay = date - 22;
    }
    if (timeSlot == "9:00 - 10:00") {
        slot = 0;
    }
    else if (timeSlot == "10:00 - 11:00") {
        slot = 1;
    }
    else if (timeSlot == "11:00 - 12:00") {
        slot = 2;
    }
    else if (timeSlot == "12:00 - 13:00") {
        slot = 3;
    }
    else if (timeSlot == "13:00 - 14:00") {
        slot = 4;
    }
    else if (timeSlot == "14:00 - 15:00") {
        slot = 5;
    }
    else if (timeSlot == "15:00 - 16:00") {
        slot = 6;
    }
    else if (timeSlot == "16:00 - 17:00") {
        slot = 7;
    }

    cout << week << " " << date << endl;

    loadScheduleFromFile(expert, week);

    expert.schedule[receiptDay][slot].isBooked = false;
    if (receipt.sessionType == TREATMENT) {
        expert.schedule[receiptDay][slot + 1].isBooked = false;
        expert.hoursWorkedPerDay[receiptDay] -= 2;
    }
    else {
        expert.hoursWorkedPerDay[receiptDay]--;
    }
    expert.schedule[receiptDay][slot].type = CONSULTATION; // Default to consultation
    saveScheduleToFile(expert, week);
}

void processRefund(Receipt& receipt, Receipt allReceipts[], int& receiptCount) {
    cout << "Processing refund for Booking Number: " << receipt.bookingNumber << endl;

    // Find index of the receipt to remove
    int receiptIndex = -1;
    for (int i = 0; i < receiptCount; i++) {
        if (allReceipts[i].bookingNumber == receipt.bookingNumber) {
            receiptIndex = i;
            break;
        }
    }

    if (receiptIndex == -1) {
        cout << RED << "Error: Booking not found." << RESET << endl;
        return;
    }

    // Remove the receipt by shifting the subsequent elements
    for (int i = receiptIndex; i < receiptCount - 1; i++) {
        allReceipts[i] = allReceipts[i + 1];
    }

    receiptCount--;
    saveUpdatedReceipts(allReceipts, receiptCount);
    updateExpertSchedule(receipt, receipt.expert);

    cout << "Refund has been processed successfully." << endl;

}

void displayBookingInfo(Receipt receipt) {
    cout << "********************************************\n";
    cout << "*           CUSTOMER BOOKING DETAILS       *\n";
    cout << "********************************************\n\n";

    cout << left << setw(20) << "Booking Number:" << receipt.bookingNumber << endl;
    cout << left << setw(20) << "Service:" << trim(receipt.serviceName) << endl;
    cout << left << setw(20) << "Expert:" << trim(receipt.expert.name) << endl;
    cout << left << setw(20) << "Customer Email:" << trim(receipt.customer.email) << endl;
    cout << left << setw(20) << "Booking Date:" << trim(receipt.date) << " July 2024" << endl;
    cout << left << setw(20) << "Time Slot:" << trim(receipt.timeSlot) << endl;
    cout << left << setw(20) << "Price: RM " << fixed << setprecision(2) << receipt.amountPaid << endl;

    cout << "\n--------------------------------------------\n";
    cout << "Please arrive 10 minutes before your time slot.\n";
    cout << "--------------------------------------------\n";
}

void displayCustomerBookings(Customer customer) {
    Receipt allReceipts[150];
    int receiptCount = loadBookings(allReceipts);

    cout << "********************************************\n";
    cout << "*           CUSTOMER BOOKING DETAILS       *\n";
    cout << "********************************************\n\n";
    cout << "Bookings for " << customer.name << endl;
    cout << "\n----------------------------------------------\n";
    cout << "Please arrive 10 minutes before your time slot.\n";
    cout << "----------------------------------------------\n";
    Receipt customerReceipts[30]; // Store customer receipts, assuming a customer can book up to 30 times
    int bookingCount = 0;

    bool hasBookings = false;
    for (int i = 0; i < receiptCount; ++i) {

        if (trim(allReceipts[i].customer.email) == trim(customer.email)) {
            hasBookings = true;
            customerReceipts[bookingCount] = allReceipts[i];
            bookingCount++;

        }
    }

    if (hasBookings) {
        int choice;
        cout << "+------+---------------------------------------------------------------------------+" << endl;
        cout << "|  No  | Booking                                                                   |" << endl;
        cout << "+------+---------------------------------------------------------------------------+" << endl;

        for (int i = 0; i < bookingCount; ++i) {
            string sessionType = customerReceipts[i].sessionType == CONSULTATION ? " Consultation" : " Treatment";
            string bookingInfo = customerReceipts[i].timeSlot + " " + customerReceipts[i].date + " July 2024 with" + customerReceipts[i].expert.name + " (" + trim(customerReceipts[i].serviceName) + sessionType + ")";
            cout << "| " << BLUE << "[" << setw(2) << i + 1 << "]" << RESET << "  | " << setw(72) << left << bookingInfo << " |" << endl;
        }
        cout << "+------+---------------------------------------------------------------------------+" << endl;
        cout << "Select a booking to view its information (-999 to go back): ";

        choice = getValidatedInput(1, bookingCount);
        if (choice == -999) {
            return; 
        }
        displayBookingInfo(customerReceipts[choice - 1]);

        char refundOption;
        cout << "Enter 'R' to request a refund or enter to continue: ";
        cin.ignore();
        refundOption = cin.get();
        cin.ignore(1000, '\n');
        if (tolower(refundOption) == 'r') {
             processRefund(customerReceipts[choice - 1], allReceipts, receiptCount);
        }
    }
    else {
        cout << RED << "No bookings found for this customer."  << RESET << endl;
    }
}

void createDirectoryIfNotExists(const string& directoryName) {
    if (ACCESS(directoryName.c_str(), 0) != 0) {
        MKDIR(directoryName.c_str());
    }
}

void saveScheduleToFile(const Expert& expert, int weekNumber) {
    createDirectoryIfNotExists("schedules");
    string filename = "schedules/" + trim(expert.name) + "_week" + to_string(weekNumber + 1) + "_schedule.txt";
    ofstream outSchedule(filename);

    if (outSchedule.is_open()) {
        for (int day = 0; day < DAYS_IN_WEEK; ++day) {
            outSchedule << "Day " << day + 1 << endl;
            outSchedule << expert.hoursWorkedPerDay[day];

            for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                outSchedule << ' ' << expert.schedule[day][slot].isBooked;
                outSchedule << ' ' << (expert.schedule[day][slot].type == TREATMENT ? "T" :
                    (expert.schedule[day][slot].type == CONSULTATION ? "C" : "U")) << ' ';
            }
            outSchedule << endl;
        }
        outSchedule.close();
    }
    else {
        cerr << RED << "Error opening file for writing: " << filename  << RESET << endl;
    }
}

void loadScheduleFromFile(Expert& expert, int weekNumber) {
    string filename = "schedules/" + trim(expert.name) + "_week" + to_string(weekNumber + 1) + "_schedule.txt";
    ifstream scheduleFile(filename);

    if (scheduleFile.is_open()) {
        string line;
        int week = -1;
        int day = -1;

        while (getline(scheduleFile, line)) {
            if (line.find("Day") == 0) {
                day = stoi(line.substr(4)) - 1;
                continue;
            }
            if (day >= 0) {
                stringstream ss(line);
                int hoursWorked;
                if (!(ss >> hoursWorked)) {
                    cerr << RED << "Error parsing hoursWorked: " << line << RESET << endl;
                    continue;
                }
                expert.hoursWorkedPerDay[day] = hoursWorked;

                for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                    char isBooked, typeChar;
                    if (!(ss >> isBooked >> typeChar)) {
                        cerr << RED << "Error reading slot data for Week " << week + 1 << " Day " << day + 1 << " from line: " << line << RESET << endl;
                        break;
                    }
                    expert.schedule[day][slot].isBooked = (isBooked == '1');
                    if (typeChar == 'T') expert.schedule[day][slot].type = TREATMENT;
                    else if (typeChar == 'C') expert.schedule[day][slot].type = CONSULTATION;
                    else expert.schedule[day][slot].type = UNAVAILABLE;
                }
            }
        }
        scheduleFile.close();
    }
    else {
        cout << "No existing schedule found for " << expert.name << ". Starting with a clean schedule." << endl;
        initializeCleanSchedule(expert);
    }
}

bool handlePaymentMethod(PaymentMethod method) {
    string otp;
    int retryCount = 3;
    int inputRetryCount = 3;

    switch(method) {
        case EWALLET: {
            string phoneNumber;
            while (inputRetryCount > 0) {
                cout << "Enter your phone number: ";
                cin >> phoneNumber;
                if (isValidPhoneNumber(phoneNumber)) {
                    otp = generateOTP();
                    cout << "OTP for E-Wallet sent to " + phoneNumber + ": " + otp << endl;
                    while (retryCount > 0) {
                        if (verifyOTP(otp)) {
                            cout << "OTP verified successfully!" << endl;
                            cout << "Proceeding to payment..." << endl;
                            return true;
                            break;
                        } else {
                            retryCount--;
                            if (retryCount > 0) {
                                cout << RED << "Invalid OTP. You have " << retryCount << " attempts remaining." << RESET << endl;
                            } else {
                                cout << RED << "Failed OTP verification. Cancelling payment process." << RESET << endl;
                                return false;
                            }
                        }
                    }
                } else {
                    inputRetryCount--;
                    if (inputRetryCount > 0) {
                        cout << RED << "Invalid phone number format! Phone number should be 01X-XXXXXXX or 01X-XXXXXXXX" << endl;
                        cout << "You have " << inputRetryCount << " tries left." << RESET << endl;
                    } else {
                        cout << RED << "Failed to provide a valid phone number. Cancelling payment process." << RESET << endl;
                        return false;
                    }
                }
            }
            break;
        }

        case BANK_TRANSFER: {
            string bankAccountNumber;
            while (retryCount > 0) {
                cout << "Enter your bank account number (10-12 digits): ";
                cin >> bankAccountNumber;
                if (isValidBankAccountNumber(bankAccountNumber)) {
                    otp = generateOTP();
                    cout << "OTP for Bank Transfer sent to your registered email: " + otp << endl;
                    while (retryCount > 0) {
                        if (verifyOTP(otp)) {
                            cout << "OTP verified successfully!" << endl;
                            cout << "Proceeding to payment..." << endl;
                            return true;
                        } else {
                            retryCount--;
                            if (retryCount > 0) {
                                cout << RED << "Invalid OTP. You have " << retryCount << " attempts remaining." << RESET << endl;
                            } else {
                                cout << RED <<  "Failed OTP verification. Cancelling payment process." << RESET << endl;
                                return false;
                            }
                        }
                    }
                } else {
                    inputRetryCount--;
                    if (inputRetryCount > 0) {
                        cout << RED << "Invalid bank account number!" << endl;  
                        cout << "You have " << inputRetryCount << " tries left." << RESET << endl; 
                    } else {
                        cout << RED << "Failed to provide a valid bank account number. Cancelling payment process." << RESET << endl;
                        return false;
                    }
                }
            }
            break;
        }

        case CREDIT_CARD: {
            string cardNumber, cvv;
            while (inputRetryCount > 0) {
                cout << "Enter your 16-digit Credit Card Number: ";
                cin >> cardNumber;
                cout << "Enter your 3-digit CVV: ";
                cin >> cvv;
                if (isValidCreditCard(cardNumber, cvv)) {
                    otp = generateOTP();
                    cout << "OTP for Credit Card sent to your registered email: " + otp << endl;

                    while (retryCount > 0) {
                        if (verifyOTP(otp)) {
                            cout << "OTP verified sucessfully!" << endl;
                            cout << "Proceeding to payment..." << endl;
                            return true;
                        } else {
                            retryCount--;
                            if (retryCount > 0) {
                                cout << RED << "Invalid OTP. You have " << retryCount << " attempts remaining." << RESET << endl;
                            } else {
                                cout << RED << "Failed OTP verification. Cancelling payment process." << RESET << endl;
                                return false;
                            }
                        }
                    }
                } else {
                    inputRetryCount--;
                    if (inputRetryCount > 0) {
                        cout << RED << "Invalid credit card details!" << endl;
                        cout << "You have " << inputRetryCount << " tries left." << RESET << endl;
                    } else {
                        cout << RED << "Failed to provide valid credit card details. Cancelling payment process." << RESET << endl;
                        return false;
                    }
                }
            }
            break;
        }

        default: 
            cout << RED << "Invalid payment method selected!" << RESET << endl;
            return false;
    }
    return false;
}

PaymentMethod selectPaymentMethod() {
    clearScreen(); //try
    cout << "Select payment method:\n";
    cout << "1. E-Wallet\n";
    cout << "2. Bank Transfer\n";
    cout << "3. Credit Card\n";
    cout << "Enter your choice (1-3 or -999 to go back): ";
    int choice;
    choice = getValidatedInput(1, 3);
    if (choice == -999) {
        return CANCELLED;
    }

    switch (choice) {
    case 1: 
        return EWALLET;
    case 2: 
        return BANK_TRANSFER;
    case 3: 
        return CREDIT_CARD;
    default:
        cout << RED << "Invalid choice. Defaulting to E-Wallet." << RESET << endl;
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
    }
    else {
        cerr << RED << "Error: Unable to open file " << filename << RESET << endl;
    }
}

string generateBookingNumber() {
    static int bookingCounter = loadBookingCounter("booking_counter.txt");
    stringstream ss;
    ss << "B" << setw(3) << setfill('0') << ++bookingCounter;
    saveBookingCounter("booking_counter.txt", bookingCounter);
    return ss.str();
}

void generateReceipt(const Receipt& receipt) {
    cout << "   __             _                                       __                              " << endl;
    cout << "  / /  ___   ___ | | _____ _ __ ___   __ ___  ____  __   / /  ___  _   _ _ __   __ _  ___ " << endl;
    cout << " / /  / _ \\ / _ \\| |/ / __| '_ ` _ \\ / _` \\ \\/ /\\ \\/ /  / /  / _ \\| | | | '_ \\ / _` |/ _ \\" << endl;
    cout << "/ /__| (_) | (_) |   <\\__ \\ | | | | | (_| |>  <  >  <  / /__| (_) | |_| | | | | (_| |  __/" << endl;
    cout << "\\____/\\___/ \\___/|_|\\_\\___/_| |_| |_|\\__,_/_/\\_\\/_/\\_\\ \\____/\\___/ \\__,_|_| |_|\\__, |\\___|" << endl;
    cout << "                                                                               |___/      " << endl;
    cout << "                                  LOOKSMAXXLOUNGE @LOOKSMAXXAREA" << endl;
    cout << "                                         Lot 23, 2nd Floor,\n";
    cout << "                                     Plaza Crystal, Jalan Ampang,\n";
    cout << "                                         50450 Kuala Lumpur,\n";
    cout << "                                 Wilayah Persekutuan Kuala Lumpur,\n";
    cout << "                                             Malaysia\n";     
    cout << "                           *********************************************\n";
    cout << "                           *               SERVICE RECEIPT             *\n";
    cout << "                           *********************************************\n\n";
    cout << "                           Booking Number:" << receipt.bookingNumber << endl;
    cout << "                           Customer Name:" << receipt.customer.name << endl;
    cout << "                           Expert:" << receipt.expert.name << endl;
    cout << "                           Session:" << (receipt.sessionType == TREATMENT ? "Treatment" : "Consultation") << endl;
    cout << "                           Service:" << receipt.serviceName << endl;
    cout << "                           Date:" << trim(receipt.date) + " July 2024" << endl;
    cout << "                           Time Slot:" << receipt.timeSlot << endl;
    cout << "                           Payment Method:" << paymentMethodToString(receipt.paymentMethod) << endl;
    cout << "                           +-------------------------------------------+\n\n";
    cout << "                           Amount Paid:" << "RM " << fixed << setprecision(2) << receipt.amountPaid << endl;
    cout << "                           ---------------------------------------------\n\n";
    cout << "                           Thank you for choosing our services!\n";
    cout << "                           For inquiries, call us at +60-123-4567 or\n";
    cout << "                           email us at looksmaxxlounget@serviceprovider.com\n";
    cout << "                           ---------------------------------------------\n";
}


void generateReceiptFile(const Receipt& receipt, const string& filename) {
    ofstream receiptFile(filename);

    if (!receiptFile.is_open()) {
        cerr << RED << "Error: Unable to open file " << filename << RESET << endl;
        return;
    }
    receiptFile << "   __             _                                       __                              " << endl;
    receiptFile << "  / /  ___   ___ | | _____ _ __ ___   __ ___  ____  __   / /  ___  _   _ _ __   __ _  ___ " << endl;
    receiptFile << " / /  / _ \\ / _ \\| |/ / __| '_ ` _ \\ / _` \\ \\/ /\\ \\/ /  / /  / _ \\| | | | '_ \\ / _` |/ _ \\" << endl;
    receiptFile << "/ /__| (_) | (_) |   <\\__ \\ | | | | | (_| |>  <  >  <  / /__| (_) | |_| | | | | (_| |  __/" << endl;
    receiptFile << "\\____/\\___/ \\___/|_|\\_\\___/_| |_| |_|\\__,_/_/\\_\\/_/\\_\\ \\____/\\___/ \\__,_|_| |_|\\__, |\\___|" << endl;
    receiptFile << "                                                                               |___/      " << endl;
    receiptFile << "                                  LOOKSMAXXLOUNGE @LOOKSMAXXAREA" << endl;
    receiptFile << "                                         Lot 23, 2nd Floor,\n";
    receiptFile << "                                     Plaza Crystal, Jalan Ampang,\n";
    receiptFile << "                                         50450 Kuala Lumpur,\n";
    receiptFile << "                                 Wilayah Persekutuan Kuala Lumpur,\n";
    receiptFile << "                                             Malaysia\n";
    receiptFile << "                           *********************************************\n";
    receiptFile << "                           *               SERVICE RECEIPT             *\n";
    receiptFile << "                           *********************************************\n\n";
    receiptFile << "                           Booking Number:" << receipt.bookingNumber << endl;
    receiptFile << "                           Customer Name:" << receipt.customer.name << endl;
    receiptFile << "                           Expert:" << receipt.expert.name << endl;
    receiptFile << "                           Session:" << (receipt.sessionType == TREATMENT ? "Treatment" : "Consultation") << endl;
    receiptFile << "                           Service:" << receipt.serviceName << endl;
    receiptFile << "                           Date:" << trim(receipt.date) + " July 2024" << endl;
    receiptFile << "                           Time Slot:" << receipt.timeSlot << endl;
    receiptFile << "                           Payment Method:" << paymentMethodToString(receipt.paymentMethod) << endl;
    receiptFile << "                           +-------------------------------------------+\n\n";
    receiptFile << "                           Amount Paid:" << "RM " << fixed << setprecision(2) << receipt.amountPaid << endl;
    receiptFile << "                           ---------------------------------------------\n\n";
    receiptFile << "                           Thank you for choosing our services!\n";
    receiptFile << "                           For inquiries, call us at +60-123-4567 or\n";
    receiptFile << "                           email us at looksmaxxlounge@serviceprovider.com\n";
    receiptFile << "                           ---------------------------------------------\n";


    receiptFile.close();
}


void printReceipt(const string& filename) {
#ifdef _WIN32
    // ShellExecute expects wide-character strings for the operation and filename
    ShellExecute(NULL, L"open", wstring(filename.begin(), filename.end()).c_str(), NULL, NULL, SW_SHOWNORMAL);
#else   
    string command = "open " + filename;
    system(command.c_str());
#endif
}


void makeBooking(Expert& expert, Service service, SessionType sessionType, Customer& customer) {
    displayCalendar(expert);
    int chosenWeek = chooseWeek();
    if (chosenWeek == -1) {
        return;
    }
    loadScheduleFromFile(expert, chosenWeek);
    displaySchedule(expert, chosenWeek);
    double price = sessionType == TREATMENT ? service.price : 60.0;

    int* result = selectTimeSlot(expert, chosenWeek, sessionType);
    if (result == nullptr) {
        return;
    }
    int day = result[0];
    int slot = result[1];

    int startDate = 1 + (chosenWeek * 7);
    int date = startDate + day;

    if (day != -1 && slot != -1) {
        int duration = (sessionType == TREATMENT) ? TREATMENT_SLOT_DURATION : CONSULTATION_SLOT_DURATION;
        for (int i = 0; i < duration; ++i) {
            expert.schedule[day][slot + i].isBooked = true;
            expert.schedule[day][slot + i].type = sessionType;
        }
        expert.hoursWorkedPerDay[day] += duration;

        string startTime = to_string(START_HOUR + slot) + ":00";
        string endTime = to_string(START_HOUR + slot + duration) + ":00";
        clearScreen(); 
        cout << "==========================================" << endl;
        cout << "          Booking Confirmation I          " << endl;
        cout << "==========================================" << endl;
        cout << "Please confirm your booking details: \n";
        cout << "Service: " << service.name << endl;
        cout << "Session Type: " << (sessionType == TREATMENT ? "Treatment" : "Consultation") << endl;
        cout << "Date: " << to_string(date) << " July 2024" << endl;
        cout << "Time Slot: " << startTime << " - " << endTime << endl;
        cout << "Price: RM " << fixed << setprecision(2) << price << endl;
        cout << "==========================================" << endl;
        cout << "Confirm booking? (Y to proceed to payment/N to stop booking): ";
        char confirm;
        cin >> confirm;

        if (tolower(confirm) == 'y') {
            PaymentMethod paymentMethod = selectPaymentMethod();
            if (paymentMethod == CANCELLED) {
                return;
            }
            if (handlePaymentMethod(paymentMethod)) {
                string bookingNumber = generateBookingNumber();
                Receipt receipt = { bookingNumber, customer, expert, sessionType, service.name, to_string(date), startTime + " - " + endTime, paymentMethod, price };
                generateReceipt(receipt);

                cout << "==========================================" << endl;
                cout << "             Booking Succeed              " << endl;
                cout << "==========================================" << endl;
                cout << "You have successfully booked the slot on Day " << day + 1
                    << " from " << expert.schedule[day][slot].timeRange.substr(0, 5) << " to " << endTime
                    << " with " << expert.name << " for " << service.name << " ("
                    << (sessionType == TREATMENT ? "Treatment" : "Consultation") << ")." << endl;
                cout << "==========================================" << endl;

                string receiptFileName = "receipt_" + bookingNumber + ".txt";
                generateReceiptFile(receipt, receiptFileName);
                printReceipt(receiptFileName);

                saveBooking(receipt);
                saveScheduleToFile(expert, chosenWeek); // Save updated schedule to file
            } else {
                cout << RED << "Payment verification failed. Booking canceled." << RESET << endl;
            }
        }
        else {
            cout << "Booking cancelled." << endl;
        }
    }
}

void customerManagement() {
    Customer customers[100];
    int choice, customerCount = 0;
    int loggedInCustomerIndex = -1;
    clearScreen();
    loadCustomersFromFile(customers, customerCount);


    do {
        displayLogo();
        cout << "Welcome!\n";
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "Option" << " | " << left << setw(DESC_WIDTH) << "Description" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "1" << " | " << setw(DESC_WIDTH) << "Sign Up (New Customer)" << " |" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "2" << " | " << setw(DESC_WIDTH) << "Login (Existing Customer)" << " |" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "3" << " | " << setw(DESC_WIDTH) << "Back" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;
        cout << "Enter your choice: ";

        choice = getValidatedInput(1, 3);

        switch (choice) {
        case 1:
            customerSignUp(customers, customerCount);
            pauseAndClear();
            break;
        case 2:
            loggedInCustomerIndex = customerLogin(customers, customerCount);
            if (loggedInCustomerIndex != -1) {
                customerMenu(customers[loggedInCustomerIndex]);
            }
            break;
        case 3:
            cout << "Returning to Main Menu\n";
            clearScreen();
            break;
        }
    } while (choice != 3);
}

bool isValidName(const string& name) {
    const regex pattern("^[A-Za-z]+(?: [A-Za-z]+)*$");
    return regex_match(trim(name), pattern);
}

bool isValidPhoneNumber(const string &phoneNumber) {
// Check for formats like 01X-XXXXXXX or 01X-XXXXXXXX
regex phonePattern("^(01[0-9]-[0-9]{7,8})$");
return regex_match(trim(phoneNumber), phonePattern);
}

bool isValidEmail(const string& email) {
    const regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
    return regex_match(trim(email), pattern);
}

bool isValidPassword(const string& password) {
    const regex pattern("(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[a-zA-Z\\d!@#$%^&*]{8,}");
    return regex_match(trim(password), pattern);
}

// Bank Account Number Validation (10-12 digits)
bool isValidBankAccountNumber(const string &accountNumber) {
    regex accountPattern("^[0-9]{10,12}$");  // Ensure it's a 10-12 digit number
    return regex_match(trim(accountNumber), accountPattern);
}

// Credit Card Validation (16 digits) and CVV (3 digits)
bool isValidCreditCard(const string &cardNumber, const string &cvv) {
    regex cardPattern("^[0-9]{16}$");   // 16-digit card number
    regex cvvPattern("^[0-9]{3}$");     // 3-digit CVV number
    return regex_match(trim(cardNumber), cardPattern) && regex_match(trim(cvv), cvvPattern);
}

string generateOTP() {
    srand(time(0));
    string otp = "";
    for (int i = 0; i < 6; ++i) {
        otp += to_string(rand() % 10);  // Generate a 6-digit random number
    }
    return otp;
}

bool verifyOTP(const string &generatedOTP) {
    string enteredOTP;
    cout << "\nEnter the OTP sent to you: ";
    cin >> enteredOTP;
    if (enteredOTP == generatedOTP) {
        cout << "OTP verified successfully!" << endl;
        return true;
    } else {
        cout << RED << "\nInvalid OTP. Please try again." << RESET << endl;
        return false;
    }
}

void customerSignUp(Customer customers[], int &customerCount) {
    const int MAX_CUSTOMERS = 100;
    if (customerCount >= MAX_CUSTOMERS) {
        cout << RED << "Maximum number of customers reached!" << RESET << endl;
        return;
    }
    Customer newCustomer;

    cout << "\n=== Customer Signup ===" << endl;
    do {
        cout << "Enter your name: ";
        cin.ignore();
        getline(cin, newCustomer.name);
        if (trim(newCustomer.name) == "-999") {
            return;
        }
        if (!isValidName(newCustomer.name)) {
            cout << RED <<  "\nName can only contain letters. Please try again.\n" << RESET;
            cin.clear();
        }
    } while (!isValidName(newCustomer.name));

    do {
        cout << "Enter your contact number (including -): ";
        getline(cin, newCustomer.contact);
        if (trim(newCustomer.contact) == "-999") {
            return;
        }
        if(!isValidPhoneNumber(newCustomer.contact)) {
            cout << RED <<  "\nInvalid contact number format.\nExample of contact: 012-3456789\nPlease try again.\n" << RESET;
        }
    } while (!isValidPhoneNumber(newCustomer.contact));

    bool isUniqueEmail;

    do {
        cout << "Enter your email: ";
        getline(cin, newCustomer.email);
        if (trim(newCustomer.email) == "-999") {
            return;
        }
        isUniqueEmail = true;
        for (int i=0; i<customerCount; i++) {
            if (trim(customers[i].email) == trim(newCustomer.email)) {
                cout << RED << "\nThis email is already registered. Please try a different email.\n" << RESET;
                isUniqueEmail = false;
                break;
            }
        }
        if (!isValidEmail(newCustomer.email)) {
            cout << RED << "\nInvalid email format. Please try again.\n " << RESET;
            isUniqueEmail = false;
        }
    } while (!isValidEmail(newCustomer.email) || !isUniqueEmail);


    do {
        cout << "Enter your password (min 8 characters, must include uppercase, lowercase, digit, and special character):\n";
        newCustomer.password = getPasswordInput();
        if (trim(newCustomer.password) == "-999") {
            return;
        }
        if (!isValidPassword(trim(newCustomer.password))) {
            cout << RED << "Invalid password format.\n"
                << "Password should be:\n "
                << "- least 8 characters long\n"
                << "- contain at least one uppercase letter, one lowercase letter\n"
                << "- one digit and one special character" << RESET << endl;
        }
    }
     while (!isValidPassword(trim(newCustomer.password)));

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
    if (trim(email) == "-999") {
        return -1;
    }

    cout << "Enter your password: ";
    password = getPasswordInput();
    if (trim(password) == "-999") {
        return -1;
    }

    for (int i = 0; i < customerCount; i++) {
        if (customers[i].email == email && customers[i].password == password) {
            return i;
        }
    }
    cout << RED << "Login failed. Please try again.\n" << RESET;
    pauseAndClearInput();
    return -1;
}

void saveCustomersToFile(Customer customers[], int customerCount) {
    ofstream outFile("customers.txt", ios::out);

    if (!outFile) {
        cerr << RED << "Error opening file for writing." << RESET << endl;
        return;
    }

    for (int i = 0; i < customerCount; i++) {
        outFile << customers[i].name << ","
            << customers[i].contact << ","
            << customers[i].email << ","
            << customers[i].password << endl;
    }

    outFile.close();
    cout << "Customer saved to file successfully!" << endl;
}   

void loadCustomersFromFile(Customer customers[], int& customerCount) {
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
}

void checkSchedule() {
    int choice, week;
    Expert alice, bob, carol;
    initializeExpert(alice, "Alice");
    initializeExpert(bob, "Bob");
    initializeExpert(carol, "Carol");
    Expert experts[] = { alice, bob, carol };
    cout << "Experts: " << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "| Option    | Description                                |" << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "| [1]       | Alice                                      |" << endl;
    cout << "| [2]       | Bob                                        |" << endl;
    cout << "| [3]       | Carol                                      |" << endl;
    cout << "+-----------+--------------------------------------------+" << endl;

    cout << "Check schedule for: ";
    choice = getValidatedInput(1, 3);
    if (choice == -999) {
        return;
    }
    cout << "Enter week number (1-5): ";
    week = getValidatedInput(1, 5);
    if (week == -999) {
        return;
    }
    --week;

    clearScreen();
    loadScheduleFromFile(experts[choice - 1], week);
    displaySchedule(experts[choice - 1], week);

}

void customerMenu(Customer& customer) {
    int choice;

    do {
        clearScreen();
        displayLogo();
        cout << "\nCustomer Menu:\n";
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "Option" << " | " << left << setw(DESC_WIDTH) << "Description" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "1" << " | " << setw(DESC_WIDTH) << "View Organization Details" << " |" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "2" << " | " << setw(DESC_WIDTH) << "View Services" << " |" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "3" << " | " << setw(DESC_WIDTH) << "View Experts" << " |" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "4" << " | " << setw(DESC_WIDTH) << "Check Schedule" << " |" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "5" << " | " << setw(DESC_WIDTH) << "Make Booking" << " |" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "6" << " | " << setw(DESC_WIDTH) << "View My Bookings" << " |" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "7" << " | " << setw(DESC_WIDTH) << "Return to Main Menu" << " |" << endl;

        cout << "+--------+------------------------------+" << endl;
        cout << "Enter your choice: ";

        choice = getValidatedInput(1, 7);

        switch (choice) {
        case 1: aboutUs(); break;
        case 2: viewServices(customer); break;
        case 3: viewExperts(); break;
        case 4: checkSchedule(); break;
        case 5: viewServices(customer); break;
        case 6: displayCustomerBookings(customer); pauseAndClear(); break;
        case 7: cout << "Returning to Main Menu\n"; 
            clearScreen();
            break;
        }

        if (choice != 7 && choice != 6) pauseAndClear();
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
                cout << "\b \b";
            }
        }
        else {
            password.push_back(ch);
            cout << '*';
        }
    }
    cout << endl;
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
        }
        else if (ch == 127 || ch == 8) {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else {
            password.push_back(ch);
            cout << '*';
        }
        cout.flush();
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    return password;
}

bool adminExpertLogin(UserType& userType, string& userName) {
    clearScreen();
    displayLogo();

    cout << "[Input -999 to go back]" << endl;

    User alice = { "alice123", "Alice1234$", EXPERT };
    User bob = { "bob123", "Bob1234$", EXPERT };
    User carol = { "carol123", "Carol1234$", EXPERT };
    User admin = { "admin123", "Admin1234$", ADMIN };

    User users[] = { alice, bob, carol, admin };

    string username, password;
    cout << "\nEnter username: ";
    cin >> username;
    if (trim(username) == "-999") {
        cout << YELLOW << "Returning to previous menu." << RESET << endl;
        pauseAndClearInput();
        return false;
    }
    cin.ignore(1000, '\n');
    cout << "Enter password: ";
    password = getPasswordInput();
    if(trim(password) == "-999") {
        cout << YELLOW << "Returning to previous menu." << RESET << endl;
        pauseAndClearInput();
        return false;
    }

    for (int i = 0; i < 4; i++) {
        if (users[i].username == username && users[i].password == password) {
            userType = users[i].type;
            userName = users[i].username.substr(0, users[i].username.length() - 3);
            return true;
        }
    }

    cout << RED << "Login failed. Please try again.\n" << RESET;
    pauseAndClearInput();
    return false;
}

void viewExpertSchedule(const string& expertName) {
    Expert expert;
    initializeExpert(expert, expertName);
    for (int week = 0; week < 5; ++week) {
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

void initializeCleanSchedule(Expert& expert) {
    for (int day = 0; day < DAYS_IN_WEEK; ++day) {
        expert.hoursWorkedPerDay[day] = 0;
        for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
            expert.schedule[day][slot].isBooked = false;
            expert.schedule[day][slot].timeRange = to_string(START_HOUR + slot) + ":00 - " + to_string(START_HOUR + slot + 1) + ":00";
            expert.schedule[day][slot].type = CONSULTATION; // Default to consultation
        }
    }
}

void viewAllSchedules() {
    string expertNames[] = { "Alice", "Bob", "Carol" };
    for (int i = 0; i < 3; i++) {
        viewExpertSchedule(expertNames[i]);
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

void displayCustomers(const Customer customers[], int customerCount, int bookingCounts[]) {
    int choice;
    cout << "+-----+-----------------------------+---------------------------+-----------------+" << endl;
    cout << "| No  | Customer Name               | Email                     | Booking Count   |" << endl;
    cout << "+-----+-----------------------------+---------------------------+-----------------+" << endl;
    for (int i = 0; i < customerCount; ++i) {
        cout << "| " << setw(2) << BLUE << "[" << i + 1 << "]" << RESET << " | " << setw(27) << customers[i].name
            << " | " << setw(25) << customers[i].email
            << " | " << setw(15) << bookingCounts[i] << " |" << endl;
    }
    cout << "+----+-----------------------------+---------------------------+-----------------+" << endl;
    cout << "\nSelect a customer to view their information: " << endl;
    choice = getValidatedInput(1, customerCount);
    if (choice == -999) {
        return;
    }

    displayCustomerDetails(customers[choice - 1]);
}

void generateSalesReport() {
    Receipt allReceipts[200];
    int receiptCount = loadBookings(allReceipts);
    string services[] = { "Facial", "Botox and Fillers", "Manicure" };
    string experts[] = { "Alice", "Bob", "Carol" };
    double facialRevenue = 0, botoxRevenue = 0, manicureRevenue = 0;
    double aliceRevenue = 0, bobRevenue = 0, carolRevenue = 0;

    if (receiptCount == 0) {
        cout << RED << "No bookings found. Unable to generate sales report." << RESET << endl;
        return;
    }

    double totalRevenue = 0;
    int totalBookings = 0;

    for (int i = 0; i < receiptCount; ++i) {
        totalRevenue += allReceipts[i].amountPaid;
        totalBookings++;

        // Revenue per service
        if (trim(allReceipts[i].serviceName) == "Facial") {
            facialRevenue += allReceipts[i].amountPaid;
        }
        else if (trim(allReceipts[i].serviceName) == "Botox and Fillers") {
            botoxRevenue += allReceipts[i].amountPaid;
        }
        else if (trim(allReceipts[i].serviceName) == "Manicure") {
            manicureRevenue += allReceipts[i].amountPaid;
        }

        // Revenue per expert
        if (trim(allReceipts[i].expert.name) == "Alice") {
            aliceRevenue += allReceipts[i].amountPaid;
        }
        else if (trim(allReceipts[i].expert.name) == "Bob") {
            bobRevenue += allReceipts[i].amountPaid;
        }
        else if (trim(allReceipts[i].expert.name) == "Carol") {
            carolRevenue += allReceipts[i].amountPaid;
        }
    }
    // Display the sales report
    cout << "\n+-----------------------------------+" << endl;
    cout << "|          Sales Report             |" << endl;
    cout << "+-----------------------------------+" << endl;
    cout << "| Total Bookings: " << setw(17) << totalBookings << " |" << endl;
    cout << "| Total Revenue : RM " << setw(14) << fixed << setprecision(2) << totalRevenue << " |" << endl;
    cout << "+-----------------------------------+" << endl;

    // Display breakdown by service
    cout << "\nRevenue by Service:\n";
    cout << "+-------------------------+------------------+" << endl;
    cout << "| Service                 | Revenue (RM)     |" << endl;
    cout << "+-------------------------+------------------+" << endl;
    cout << "| " << setw(23) << left << "Facial" << " | RM " << setw(13) << right << fixed << setprecision(2) << facialRevenue << " |" << endl;
    cout << "| " << setw(23) << left << "Botox and Fillers" << " | RM " << setw(13) << right << fixed << setprecision(2) << botoxRevenue << " |" << endl;
    cout << "| " << setw(23) << left << "Manicure" << " | RM " << setw(13) << right << fixed << setprecision(2) << manicureRevenue << " |" << endl;
    cout << "+-------------------------+------------------+" << endl;

    // Display breakdown by expert
    cout << "\nRevenue by Expert:\n";
    cout << "+-------------------------+------------------+" << endl;
    cout << "| Expert                  | Revenue (RM)     |" << endl;
    cout << "+-------------------------+------------------+" << endl;
    cout << "| " << setw(23) << left << "Alice" << " | RM " << setw(13) << right << fixed << setprecision(2) << aliceRevenue << " |" << endl;
    cout << "| " << setw(23) << left << "Bob" << " | RM " << setw(13) << right << fixed << setprecision(2) << bobRevenue << " |" << endl;
    cout << "| " << setw(23) << left << "Carol" << " | RM " << setw(13) << right << fixed << setprecision(2) << carolRevenue << " |" << endl;
    cout << "+-------------------------+------------------+" << endl;

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

    if (!expertName.empty()) {
        expertName[0] = toupper(expertName[0]);
    }

    Customer customers[MAX_CUSTOMERS];
    int bookingCounts[MAX_CUSTOMERS] = { 0 };
    int customerCount = 0;

    for (int i = 0; i < receiptCount; i++) {
        if (!expertName.empty() && trim(allReceipts[i].expert.name) != expertName) {
            continue;
        }

        //Check if the customer already exists in the array
        bool found = false;
        for (int j = 0; j < customerCount; j++) {
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
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "| Option    | Description                                |" << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "| [1]       | View without sorting                       |" << endl;
    cout << "| [2]       | Sort by customer name                      |" << endl;
    cout << "| [3]       | Sort by customer bookings                  |" << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "\nEnter your choice (-999 to go back): ";
    choice = getValidatedInput(1, 3);
    if (choice == -999) {
        return;
    }

    switch (choice) {
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
        }
        else {
            sortCustomersByExpertBookings(customers, customerCount, allReceipts, receiptCount, expertName, bookingCounts);
        }
        displayCustomers(customers, customerCount, bookingCounts);
        break;
    }
}


void adminExpertMenu(UserType& userType, string& userName) {
    int adminChoice, expertChoice;

    do {
        clearScreen();
        displayLogo();
        cout << "\nAdmin/Expert Menu:\n";
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "Option" << " | " << left << setw(DESC_WIDTH) << "Description" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;

        if (userType == EXPERT) {
            cout << "| " << setw(OPTION_WIDTH - 1) << "1" << " | " << setw(DESC_WIDTH) << "View My Schedule" << " |" << endl;
        }
        else if (userType == ADMIN) {
            cout << "| " << setw(OPTION_WIDTH - 1) << "1" << " | " << setw(DESC_WIDTH) << "View All Schedules" << " |" << endl;
        }
        cout << "| " << setw(OPTION_WIDTH - 1) << "2" << " | " << setw(DESC_WIDTH) << "View Customers" << " |" << endl;
        if (userType == ADMIN) {
            cout << "| " << setw(OPTION_WIDTH - 1) << "3" << " | " << setw(DESC_WIDTH) << "Generate Sales Report" << " |" << endl;
            cout << "| " << setw(OPTION_WIDTH - 1) << "4" << " | " << setw(DESC_WIDTH) << "Return to Main Menu" << " |" << endl;
        }
        else if (userType == EXPERT) {
            cout << "| " << setw(OPTION_WIDTH - 1) << "3" << " | " << setw(DESC_WIDTH) << "Return to Main Menu" << " |" << endl;
        }

        cout << "+--------+------------------------------+" << endl;
        cout << "Enter your choice: ";

        if (userType == EXPERT) {
            expertChoice = getValidatedInput(1, 3);

            switch (expertChoice) {
            case 1:
                viewExpertSchedule(userName);
                break;
            case 2:
                viewCustomers(userName);
                break;
            case 3: cout << "Returning to Main Menu\n"; 
                clearScreen();
                break;
            }
            if (expertChoice != 3) pauseAndClear();
        }
        else if (userType == ADMIN) {
            adminChoice = getValidatedInput(1, 4);

            switch (adminChoice) {
            case 1:
                viewAllSchedules();
                break;
            case 2:
                viewCustomers();
                break;
            case 3:
                generateSalesReport();
                break;
            case 4: cout << "Returning to Main Menu\n"; 
                clearScreen();
                break;
            }

            if (adminChoice != 4) pauseAndClear();
        }
    } while ((userType == EXPERT && expertChoice != 3) || (userType == ADMIN && adminChoice != 4));
}

int getValidatedInput(int min, int max) {
    int input, sentinel = -999;
    while (true) {
        cin >> input;

        if (cin.fail()) {
            // Clear error flag on cin
            cin.clear();
            // Ignore the rest of the invalid input until the newline
            cin.ignore(1000, '\n');
            cout << RED << "Invalid input. Please enter a valid integer: " << RESET;
        } else if (input == sentinel) {
            cout << YELLOW << "Returning to the previous menu." << RESET << endl;
            return sentinel;
        }
        else if (input < min || input > max) {
            // Ignore any extra characters from valid input
            cin.ignore(1000, '\n');
            cout << RED <<  "Out of range. Please enter a number between " << min << " and " << max << ": " << RESET;
        }
        else {
            // Return the value
            return input;
        }
    }
}


void aboutUs() {
    clearScreen();
    displayLogo();
    cout << "===============================" << endl;
    cout << "        Looksmaxx Lounge        " << endl;
    cout << "===============================" << endl;
    cout << "\nOrganization Details:\n";
    cout << "Name:       Looksmaxx Lounge\n";
    cout << "Address:    Jalan Genting Kelang, Setapak,\n";
    cout << "            53300 Kuala Lumpur\n";
    cout << "Phone:      03-12345678\n";
    cout << "Email:      looksmaxxlounge@enquiries.com\n";
    cout << "\nDescription:\n";
    cout << "   Looksmaxx Lounge is a premium beauty and wellness hub\n";
    cout << "   designed to help individuals enhance their natural features\n";
    cout << "   and boost their confidence. Specializing in a range of\n";
    cout << "   personalized beauty services—from skincare treatments and\n";
    cout << "   cosmetic enhancements to hairstyling and body contouring.\n";
    cout << "   Looksmaxx Lounge caters to clients seeking expert guidance\n";
    cout << "   and high-quality results. With a team of experienced\n";
    cout << "   professionals and state-of-the-art technology, the lounge\n";
    cout << "   provides a relaxing, luxurious environment where clients\n";
    cout << "   can achieve their aesthetic goals in a welcoming atmosphere.\n";
    cout << "===============================\n";
}

void serviceDesc(Service service, Customer& customer) {
    clearScreen();
    displayLogo();
    cout << "===============================\n";
    cout << "         Service Details        \n";
    cout << "===============================\n";
    cout << "\n" << service.name << "\n";
    cout << "Description: " << service.description << "\n";
    cout << "Treatment Price: RM" << fixed << setprecision(2) << service.price << "\n";
    cout << "===============================\n";

    cout << "Press Y to book or any other key to return to main menu: ";
    char choice;
    cin >> choice;
    if (choice == 'Y' || choice == 'y') {
        Expert alice, bob, carol;
        initializeExpert(alice, "Alice");
        initializeExpert(bob, "Bob");
        initializeExpert(carol, "Carol");
        Expert experts[] = { alice, bob, carol };

        cout << "\n---------------------------\n";
        cout << "     Select Your Expert\n";
        cout << "---------------------------\n";
        cout << "We have the following beauty experts available:\n";
        cout << "  [1]  Alice\n";
        cout << "  [2]  Bob\n";
        cout << "  [3]  Carol\n";
        cout << "\nPlease select an expert by entering the number (1-3 or -999 to go back): ";
        int expertChoice;
        expertChoice = getValidatedInput(1, 3);
        if (expertChoice == -999) {
            return;
        }

        Expert& selectedExpert = experts[expertChoice - 1];

        cout << "\n---------------------------\n";
        cout << "     Choose Session Type\n";
        cout << "---------------------------\n";
        cout << "Please choose the type of session you’d like:\n";
        cout << "  [1] Full Treatment - 2 hours of treatment\n";
        cout << "  [2] Consultation   - 1 hour personalized consultation\n";
        cout << "\nEnter your choice (1-2, -999 to go back): ";

        int sessionChoice;
        sessionChoice = getValidatedInput(1, 2);
        if (sessionChoice == -999) {
            return;
        }

        SessionType sessionType;
        switch (sessionChoice) {
        case 1: sessionType = TREATMENT; break;
        case 2: sessionType = CONSULTATION; break;
        default:
            cout << RED <<  "Invalid choice. Exiting." << RESET;
            return;
        }
        makeBooking(selectedExpert, service, sessionType, customer);
    }
}

void viewServices(Customer& customer) {
    clearScreen();
    displayLogo();

    Service facial = { "Facial", "A rejuvenating facial treatment.", 150.00 };
    Service botox = { "Botox and Fillers", "Cosmetic injections for wrinkle treatment.", 250.00 };
    Service manicure = { "Manicure", "A relaxing manicure sesion.", 100.00 };

    Service services[3] = { facial, botox, manicure };
    int choice, service_count = 1;
    cout << "\nServices\n";
    cout << "+--------+------------------------------+" << endl;
    cout << "| " << setw(OPTION_WIDTH - 1) << "Option" << " | " << left << setw(DESC_WIDTH) << "Service Name" << " |" << endl;
    cout << "+--------+------------------------------+" << endl;
    cout << "| " << setw(OPTION_WIDTH - 1) << "1" << " | " << setw(DESC_WIDTH) << "Facial" << " |" << endl;
    cout << "| " << setw(OPTION_WIDTH - 1) << "2" << " | " << setw(DESC_WIDTH) << "Botox and Fillers" << " |" << endl;
    cout << "| " << setw(OPTION_WIDTH - 1) << "3" << " | " << setw(DESC_WIDTH) << "Manicure" << " |" << endl;

    cout << "+--------+------------------------------+" << endl;
    cout << "Enter your choice (-999 to go back): ";

    choice = getValidatedInput(1, 3);
    if (choice == -999) {
        return;
    }

    serviceDesc(services[choice - 1], customer);
}

void displayExpertDetails(Expert& expert) {
    if (expert.name == "Alice") {
        cout << "+--------------------------------------------------------+" << endl;
        cout << "|                    Expert Details                      |" << endl;
        cout << "+--------------------------------------------------------+" << endl;
        cout << "| Name: Alice                                            |" << endl;
        cout << "| Years of Experience: 5                                 |" << endl;
        cout << "| Rating: 4.9/5                                          |" << endl;
        cout << "+--------------------------------------------------------+" << endl;
    }
    else if (expert.name == "Bob") {
        cout << "+--------------------------------------------------------+" << endl;
        cout << "|                    Expert Details                      |" << endl;
        cout << "+--------------------------------------------------------+" << endl;
        cout << "| Name: Bob                                            |" << endl;
        cout << "| Years of Experience: 2                                 |" << endl;
        cout << "| Rating: 4.7/5                                          |" << endl;
        cout << "+--------------------------------------------------------+" << endl;
    }
    else if (expert.name == "Carol") {
        cout << "+--------------------------------------------------------+" << endl;
        cout << "|                    Expert Details                      |" << endl;
        cout << "+--------------------------------------------------------+" << endl;
        cout << "| Name: Carol                                            |" << endl;
        cout << "| Years of Experience: 8                                 |" << endl;
        cout << "| Rating: 4.8/5                                          |" << endl;
        cout << "+--------------------------------------------------------+" << endl;
    }
    char viewSchedule;
    cout << "Do you want to view this expert's schedule? (Y or N): " << endl;
    cin >> viewSchedule;

    if (tolower(viewSchedule) == 'y') {
        viewExpertSchedule(expert.name);
    }
}

void viewExperts() {
    clearScreen();
    displayLogo();

    int choice, week;
    Expert alice, bob, carol;
    initializeExpert(alice, "Alice");
    initializeExpert(bob, "Bob");
    initializeExpert(carol, "Carol");
    Expert experts[] = { alice, bob, carol };

    cout << "Experts: " << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "| Option    | Description                                |" << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "| [1]       | Alice                                      |" << endl;
    cout << "| [2]       | Bob                                        |" << endl;
    cout << "| [3]       | Carol                                      |" << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "Select an expert to view their details: ";
    choice = getValidatedInput(1, 3);

    if (choice == -999) {
        return;
    }

    switch (choice) {
    case 1:
        displayExpertDetails(alice); break;
    case 2:
        displayExpertDetails(bob); break;
    case 3:
        displayExpertDetails(carol); break;
    default:
        cout << "No expert found." << endl;
    }
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

void pauseAndClearInput() {
    cout << "\nPress Enter to continue...";
    cin.ignore(1000, '\n');
    clearScreen();
}
