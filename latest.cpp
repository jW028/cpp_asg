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
// Color codes for console output
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

using namespace std;

// Struct to represent a customer
struct Customer {
    string name;
    string email;
    string contact;
    string password;
};

// Enum to define different payment methods
enum PaymentMethod {
    EWALLET,
    BANK_TRANSFER,
    CREDIT_CARD, 
    CANCELLED
};

// Enum to define the type of session 
enum  SessionType { TREATMENT, CONSULTATION, UNAVAILABLE };

// Struct representing a service offered by an expert
struct Service {
    string name;
    string description;
    double price;
};

// Struct representing a timeslot in an expert's schedule
struct TimeSlot {
    bool isBooked; // Indicates if the slot is booked
    string timeRange; // Time range of the slot (e.g. "9:00 - 10:00")
    SessionType type; // Type of session (consultation or treatment)
};

// Struct representing an expert with a name, schedule, and working hours
struct Expert {
    string name; // Expert's name
    TimeSlot schedule[DAYS_IN_WEEK][MAX_SLOTS_PER_DAY]; // Weekly schedule
    int hoursWorkedPerDay[DAYS_IN_WEEK]; // Hours worked per day

};

// Struct representing a receipt generated after a booking
struct Receipt {
    string bookingNumber;     // Unique booking number
    Customer customer;        // The customer who made the booking
    Expert expert;            // The expert assigned for the booking
    SessionType sessionType;  // Type of session booked
    string serviceName;       // Name of the service booked
    string date;              // Date of the booking
    string timeSlot;          // Time of the booking
    PaymentMethod paymentMethod; // Payment method used
    double amountPaid;        // Amount paid for the booking
};

// Struct representing a booking with details of the session, customer, and expert
struct Booking {
    string referenceNum;  // Unique reference number for the booking
    Customer customer;    // Customer making the booking
    Expert expert;        // Expert assigned to the session
    Service service;      // Service being booked
    SessionType treatment; // Type of treatment or consultation
    int weekNumber;       // Week number of the booking
    int day;              // Day of the week (0-4 for Mon-Fri)
    int slot;             // Time slot booked
};

// Enum to define types of users (admin or expert)
enum UserType { ADMIN, EXPERT };

struct User {
    string username;
    string password;
    UserType type;
};

// Function declarations 
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
        displayLogo(); // Display the system logo
        displayMainMenu(); // Show the main menu options

        choice = getValidatedInput(1, 3); // Get the user's choice (1 to 3)

        switch (choice) {
        case 1:
            customerManagement(); // Customer management section
            break;
        case 2:
            if (adminExpertLogin(userType, userName)) {  // Login as admin or expert
                adminExpertMenu(userType, userName);     // Show the admin/expert menu
            }
            break;
        case 3:
            cout << "Exiting program...\n";
            return 0; // Exit the program
        }
    } while (choice != 3); // Loop until the user selects 'Exit'

    return 0;
}

// Function to display the system logo
void displayLogo() {
    cout << "   __             _                                       __                              " << endl;
    cout << "  / /  ___   ___ | | _____ _ __ ___   __ ___  ____  __   / /  ___  _   _ _ __   __ _  ___ " << endl;
    cout << " / /  / _ \\ / _ \\| |/ / __| '_ ` _ \\ / _` \\ \\/ /\\ \\/ /  / /  / _ \\| | | | '_ \\ / _` |/ _ \\" << endl;
    cout << "/ /__| (_) | (_) |   <\\__ \\ | | | | | (_| |>  <  >  <  / /__| (_) | |_| | | | | (_| |  __/" << endl;
    cout << "\\____/\\___/ \\___/|_|\\_\\___/_| |_| |_|\\__,_/_/\\_\\/_/\\_\\ \\____/\\___/ \\__,_|_| |_|\\__, |\\___|" << endl;
    cout << "                                                                               |___/      " << endl;
}

// Function to display the main menu options

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

// Function to initialize an expert's schedule
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

// Converts the payment method enum to a string for display purposes
string paymentMethodToString(PaymentMethod paymentMethod) {
    switch (paymentMethod) {
    case EWALLET: return "E-Wallet";
    case BANK_TRANSFER: return "Bank Transfer";
    case CREDIT_CARD: return "Credit Card";
    default: return "Unknown";
    }
}

// Displays the expert's schedule for a specific week
void displaySchedule(const Expert& expert, int week) {

    if (week < 0 || week >= 5) {
        cout << RED << "Invalid week number. Please select a week from 1 to 5." << RESET << endl;
        return;
    }

    const int DAYWIDTH = 19;  // Set the width for each day's column // Set the width for each slot
    const string days[5] = { "Mon", "Tue", "Wed", "Thu", "Fri" }; // Days of the week

    int startDate = 1 + (week * 7); // Calculate the starting date for the selected week
    int padding = (DAYWIDTH - 1 - days[0].length()) / 2; // Calculate padding for centering the day names

    // Display the header for time and day names
    cout << string(DAYWIDTH * 6 + 2, '-') << endl;
    cout << "|" << setw(DAYWIDTH) << left << "Time";

    // Loop through each day to display its date
    for (int i = 0; i < 5; ++i) {
        int date = startDate + i;
        string header;
        if (date > 31) {
            header = days[i] + " (Unavailable)"; // If date exceeds 31, mark as unavailable
        }
        else {
            header = days[i] + " (" + to_string(date) + ")"; // Display day with the date
        }
        padding = (DAYWIDTH - header.length()) / 2;
        cout << "|" << setw(padding) << "" << header << setw(DAYWIDTH - 1 - padding - header.length()) << "";
    }

    cout << "|" << endl;
    cout << string(DAYWIDTH * 6 + 2, '-') << endl;

    // Loop through each slot to display its status for each day
    for (int i = 0; i < MAX_SLOTS_PER_DAY; i++) {
        cout << "|" << BLUE << setw(4) << left << "[" + to_string(i + 1) + "]" << RESET // Align the index
            << setw(DAYWIDTH - 4) << left << expert.schedule[0][i].timeRange; // Align the time range

        // Loop through each day in the week
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
        // Display the bottom border of the schedule
        cout << "|\n";
    }

    cout << string(DAYWIDTH * 6 + 2, '-') << "\n\n";
}



// Checks if a slot can be booked based on availability and session duration
bool canBookSlot(const Expert& expert, int day, int slot, SessionType sessionType) {
    int duration = (sessionType == TREATMENT) ? TREATMENT_SLOT_DURATION : CONSULTATION_SLOT_DURATION;
    if (expert.hoursWorkedPerDay[day] + duration > MAX_WORK_HOURS) {
        return false;  // Cannot book if it exceeds the max working hours
    }
    // Ensure all required slots for the session are open
    for (int i = 0; i < duration; ++i) {
        if (slot + i >= MAX_SLOTS_PER_DAY || expert.schedule[day][slot + i].isBooked) {
            return false;
        }
    }
    return true;
}

// Trims leading and trailing spaces from a string
string trim(const string& str) {
    if (str.empty()) {
        return "";
    }
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));  // Return trimmed string
}

// Prompts the user to select a valid week number for booking
int chooseWeek() {
    int choice;
    cout << "Enter week number (1-5, -999 to go back): ";
    choice = getValidatedInput(1, 5);  // Get a validated input within the range
    if (choice == -999) {
        return -1;  // Allow exit from the menu
    }
    if (choice < 1 || choice > 5) {
        cout << RED << "Invalid choice. Please select a valid week.\n" << RESET;
        return chooseWeek();  // Recursively prompt for a valid input
    }
    return --choice;  // Convert 1-based week to 0-based index
}

// Displays the available weeks and days in a calendar format
void displayCalendar(Expert& expert) {

    const int DAYWIDTH = 12;  // Width for each day's display
    const string days[5] = { "Mon", "Tue", "Wed", "Thu", "Fri" };

    // Display header for weeks
    cout << "\nAvailable Weeks and Days:" << endl;

    // Loop through each week and day
    for (int week = 0; week < 5; ++week) {
        loadScheduleFromFile(expert, week); // Load the expert's schedule for the given week
        int availableSlots = 0;
        int startDate = 1 + (week * 7); // Calculate the starting date for the week
        for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
            for (int day = 0; day < 5; ++day) {
                if (week == 4 && (startDate + day) > 31) break; // Skip slots after 31st
                if (!expert.schedule[day][slot].isBooked && expert.schedule[day][slot].type != UNAVAILABLE) {
                    availableSlots++; // Count available slots
                }
            }
        }
        if (availableSlots > 0) {
            cout << "Week " << (week + 1) << ":" << "(" << GREEN << availableSlots  << " slots available)" << RESET << endl;
        } else {
            cout << "Week " << (week + 1) << ":" << "(" << RED << 0  << " slots available)" << RESET << endl;
        }

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

// Function to select a time slot for booking
int* selectTimeSlot(const Expert& expert, int chosenWeek, SessionType sessionType) {
    static int result[2]; // Array to store selected day and slot
    int selectedDay;
    // Choose a day for the booking depending on the week number
    if (chosenWeek != 4) {
        cout << "Select a day (1-5 for Mon-Fri, -999 to go back): ";
        selectedDay = getValidatedInput(1, 5); // Validate day input
        if (selectedDay == -999) { // Return nullptr if user chooses to go back
            return nullptr;
        }
        selectedDay -= 1; // Convert to 0-based index
    }
    else {
        cout << "Select a day (1-3 for Mon-Wed, -999 to go back): ";
        selectedDay = getValidatedInput(1, 3); // Validate day input
        if (selectedDay == -999) { // Return nullptr if user chooses to go back
            return nullptr;
        }
        selectedDay -= 1; // Convert to 0-based index
    }

    cout << "Select a starting time slot (1-" << MAX_SLOTS_PER_DAY << ", -999 to go back): ";
    int selectedSlot = getValidatedInput(1, MAX_SLOTS_PER_DAY); // Validate time slot input
    if (selectedSlot == -999) { // Return nullptr if user chooses to go back
        return nullptr;
    }
    selectedSlot -= 1; // Convert to 0-based index

    // Validate the selected time slot based on availability and session type
    if (chosenWeek >= 0 && chosenWeek < 5 &&
        selectedDay >= 0 && selectedDay < DAYS_IN_WEEK &&
        selectedSlot >= 0 && selectedSlot < MAX_SLOTS_PER_DAY &&
        canBookSlot(expert, selectedDay, selectedSlot, sessionType)) {
        // Store selected day and slot in result
        result[0] = selectedDay;
        result[1] = selectedSlot;
        return result;
    }

    // Handle invalid selection
    cout << RED << "Invalid selection, slot(s) already booked, or exceeds daily work limit." << RESET << endl;
    result[0] = -1;
    result[1] = -1;
    return result;
}

// Function to save a booking to the bookings file
void saveBooking(const Receipt& receipt) {
    // Open bookings.txt file in append mode
    ofstream bookingsFile("bookings.txt", ios::app);
    if (!bookingsFile.is_open()) {
        cerr << RED << "Error: Unable to open bookings file for writing." << RESET << endl;
        return;
    }

    // Write booking details to the file
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

    // Close the file after writing
    bookingsFile.close();
}

// Function to load bookings from the bookings file
int loadBookings(Receipt receipts[]) {
    const int MAX_BOOKINGS = 200; // Maximum number of bookings to load
    ifstream bookingsFile("bookings.txt");
    if (!bookingsFile.is_open()) {
        cerr << RED << "Error: Unable to open bookings file for reading." << RESET << endl;
        return 0;
    }
    string line;
    int count = 0;

    // Read the bookings file line by line
    while (getline(bookingsFile, line) && count < MAX_BOOKINGS) {
        if (line.empty()) {
            continue; // Skip empty lines
        }
        stringstream ss(line); // Parse the line into a stringstream
        string item;
        string row[11]; // Array to store fields in the line

        int row_count = 0;

        // Split the line by commas and store each item in the row array
        while (getline(ss, item, ',')) {

            row[row_count] = item;
            row_count++;
        }

        // Populate the receipts array with the parsed data
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

        count++; // Increment the booking count
    }
    // Close the file after reading
    bookingsFile.close();
    return count; // Return the number of bookings loaded
}

// Function to save updated receipts to the bookings file
void saveUpdatedReceipts(Receipt allReceipts[], int receiptCount) {
    ofstream file("bookings.txt"); // Open the bookings file for writing

    if (!file) { // Check if the file was opened successfully
        cout << RED <<  "Error opening file for saving receipts." << RESET << endl;
        return; // Exit the function if file opening fails
    }

    // Write updated receipt data back to the file
    for (int i = 0; i < receiptCount; i++) {
        // Write each field of the receipt into file
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

    file.close(); // Close the file after writing
}

// Function to update an expert's schedule after a refund
void updateExpertSchedule(Receipt& receipt, Expert& expert) {
    int week = -1, receiptDay = -1, slot = -1;  // Initialize week, day, and slot variables
    int date = stoi(trim(receipt.date));  // Convert date from string to integer
    string timeSlot = trim(receipt.timeSlot);  // Trim whitespace from the time slot string
    timeSlot = timeSlot.substr(0, timeSlot.find(' '));  // Extract the starting time from the time slot

    // Determine the week and day based on the date
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
    else if (date >= 29 && date <= 31) {
        week = 4;
        receiptDay = date - 29;
    }

    // Determine the time slot
    if (timeSlot == "9:00") {
        slot = 0;
    }
    else if (timeSlot == "10:00") {
        slot = 1;
    }
    else if (timeSlot == "11:00") {
        slot = 2;
    }
    else if (timeSlot == "12:00") {
        slot = 3;
    }
    else if (timeSlot == "13:00") {
        slot = 4;
    }
    else if (timeSlot == "14:00") {
        slot = 5;
    }
    else if (timeSlot == "15:00") {
        slot = 6;
    }
    else if (timeSlot == "16:00") {
        slot = 7;
    }

    // Ensure the slot, week, and day are valid
    if (week != -1 && receiptDay != -1 && slot != -1) {
        // Load the expert's schedule for the corresponding week
        loadScheduleFromFile(expert, week);

        // Mark the slot(s) as available (not booked) and adjust the expert's working hours
        expert.schedule[receiptDay][slot].isBooked = false;
        if (receipt.sessionType == TREATMENT) {
            expert.schedule[receiptDay][slot + 1].isBooked = false;  // Unbook the next slot for treatment
            expert.hoursWorkedPerDay[receiptDay] -= 2;  // Deduct 2 hours for a treatment session
        }
        else {
            expert.hoursWorkedPerDay[receiptDay]--; // Deduct 1 hour for a consultation session
        } 

        // Reset the slot type to consultation
        expert.schedule[receiptDay][slot].type = CONSULTATION; // Default to consultation

        // Save the updated schedule back to the file
        saveScheduleToFile(expert, week);
    }
    else {
        // Output an error message if the week, day, or slot is invalid
        cout << "Error: Invalid week, day, or time slot." << endl;
    }
}

// Function to process a refund for a booking
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

    if (receiptIndex == -1) { // If receipt not found
        cout << RED << "Error: Booking not found." << RESET << endl;
        return;
    }

    // Shift all subsequent receipts to fill the gap left by the removed receipt
    for (int i = receiptIndex; i < receiptCount - 1; i++) {
        allReceipts[i] = allReceipts[i + 1];
    }

    receiptCount--;  // Decrease the receipt count after removing the booking
    saveUpdatedReceipts(allReceipts, receiptCount);  // Save the updated receipts
    updateExpertSchedule(receipt, receipt.expert);  // Update the expert's schedule

    cout << "Refund has been processed successfully." << endl; // Output a success message

}

// Function to display detailed booking information for a customer
void displayBookingInfo(Receipt receipt) {
    cout << "********************************************\n";
    cout << "*           CUSTOMER BOOKING DETAILS       *\n";
    cout << "********************************************\n\n";

    // Display each detail of the booking in a formatted manner
    cout << left << setw(20) << "Booking Number:" << receipt.bookingNumber << endl;
    cout << left << setw(20) << "Service:" << trim(receipt.serviceName) << endl;
    cout << left << setw(20) << "Expert:" << trim(receipt.expert.name) << endl;
    cout << left << setw(20) << "Customer Email:" << trim(receipt.customer.email) << endl;
    cout << left << setw(20) << "Booking Date:" << trim(receipt.date) << " July 2024" << endl;
    cout << left << setw(20) << "Time Slot:" << trim(receipt.timeSlot) << endl;
    cout << left << setw(20) << "Price: RM " << fixed << setprecision(2) << receipt.amountPaid << endl;

    // Additional user guidance
    cout << "\n--------------------------------------------\n";
    cout << "Please arrive 10 minutes before your time slot.\n";
    cout << "--------------------------------------------\n";
}

// Function to display all bookings for a specific customer
void displayCustomerBookings(Customer customer) {
    Receipt allReceipts[150]; // Array to hold all receipts
    int receiptCount = loadBookings(allReceipts); // Load receipts from file

    cout << "********************************************\n";
    cout << "*           CUSTOMER BOOKING DETAILS       *\n";
    cout << "********************************************\n\n";
    cout << "Bookings for " << customer.name << endl;
    cout << "\n----------------------------------------------\n";
    cout << "Please arrive 10 minutes before your time slot.\n";
    cout << "----------------------------------------------\n";
    Receipt customerReceipts[30]; // Store customer receipts, assuming a customer can book up to 30 times
    int bookingCount = 0; // Counter for customer bookings

    bool hasBookings = false; // Flag to check if the customer has bookings
    for (int i = 0; i < receiptCount; ++i) {
        // Check if the email matches to store customer bookings
        if (trim(allReceipts[i].customer.email) == trim(customer.email)) {
            hasBookings = true;
            customerReceipts[bookingCount] = allReceipts[i]; // Store matching booking
            bookingCount++; // Increment booking count

        }
    }

    if (hasBookings) { // If customer has bookings, display them
        int choice;
        cout << "+------+---------------------------------------------------------------------------+" << endl;
        cout << "|  No  | Booking                                                                   |" << endl;
        cout << "+------+---------------------------------------------------------------------------+" << endl;

        // Display each booking in a formatted table
        for (int i = 0; i < bookingCount; ++i) {
            string sessionType = customerReceipts[i].sessionType == CONSULTATION ? " Consultation" : " Treatment";
            string bookingInfo = customerReceipts[i].timeSlot + " " + customerReceipts[i].date + " July 2024 with" + customerReceipts[i].expert.name + " (" + trim(customerReceipts[i].serviceName) + sessionType + ")";
            cout << "| " << BLUE << "[" << setw(2) << i + 1 << "]" << RESET << "  | " << setw(72) << left << bookingInfo << " |" << endl;
        }
        cout << "+------+---------------------------------------------------------------------------+" << endl;
        cout << "Select a booking to view its information (-999 to go back): ";

        // Validate user input for booking selection
        choice = getValidatedInput(1, bookingCount);
        if (choice == -999) {
            return; // Exit if user chooses to go back
        }
        displayBookingInfo(customerReceipts[choice - 1]); // Display selected booking information

        // Prompt user for refund option
        char refundOption;
        cout << "Enter 'R' to request a refund or enter to continue: ";
        cin.ignore(); // Clear input buffer
        refundOption = cin.get(); // Get refund option
        cin.ignore(1000, '\n');
        if (tolower(refundOption) == 'r') {
             processRefund(customerReceipts[choice - 1], allReceipts, receiptCount); // Process refund if requested
        }
    }
    else {
        // Inform user if no bookings are found
        cout << RED << "No bookings found for this customer."  << RESET << endl;
    }
}

// Function to create a directory if it does not already exist
void createDirectoryIfNotExists(const string& directoryName) {
    if (ACCESS(directoryName.c_str(), 0) != 0) { // Check if directory exists
        MKDIR(directoryName.c_str()); // Create directory if it does not exist
    }
}

// Function to save the expert's schedule to a file
void saveScheduleToFile(const Expert& expert, int weekNumber) {
    createDirectoryIfNotExists("schedules"); // Ensure the schedules directory exists
    string filename = "schedules/" + trim(expert.name) + "_week" + to_string(weekNumber + 1) + "_schedule.txt"; // Generate filename based on expert and week number
    ofstream outSchedule(filename); // Open file for output

    if (outSchedule.is_open()) { // Check if file opened successfully
        // Write each day's schedule to the file
        for (int day = 0; day < DAYS_IN_WEEK; ++day) {
            outSchedule << "Day " << day + 1 << endl;
            outSchedule << expert.hoursWorkedPerDay[day]; // Write hours worked for the day

            // Write each time slot's booking status and type
            for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                outSchedule << ' ' << expert.schedule[day][slot].isBooked; // Write booking status
                outSchedule << ' ' << (expert.schedule[day][slot].type == TREATMENT ? "T" :
                    (expert.schedule[day][slot].type == CONSULTATION ? "C" : "U")) << ' '; // Write type (Treatment/Consultation/Unavailable)
            }
            outSchedule << endl; // New line for next day
        }
        outSchedule.close(); // Close file after writing
    }
    else {
        // Error handling if file fails to open
        cerr << RED << "Error opening file for writing: " << filename  << RESET << endl;
    }
}

// Function to load an expert's schedule from a file
void loadScheduleFromFile(Expert& expert, int weekNumber) {
    // Construct the filename for the schedule based on the expert's name and week number
    string filename = "schedules/" + trim(expert.name) + "_week" + to_string(weekNumber + 1) + "_schedule.txt";
    ifstream scheduleFile(filename); // Open the schedule file for reading

    if (scheduleFile.is_open()) {
        string line;
        int week = -1; // Variable to track the current week (not used)
        int day = -1; // Variable to track the current day

        // Read the file line by line
        while (getline(scheduleFile, line)) {
            // Check for lines indicating a new day
            if (line.find("Day") == 0) {
                day = stoi(line.substr(4)) - 1; // Extract day number
                continue; // Move to the next line
            }
            if (day >= 0) {
                stringstream ss(line); // Create a string stream to parse the line
                int hoursWorked;
                // Attempt to read hours worked, handle errors if parsing fails
                if (!(ss >> hoursWorked)) {
                    cerr << RED << "Error parsing hoursWorked: " << line << RESET << endl;
                    continue; // Skip to the next line
                }
                expert.hoursWorkedPerDay[day] = hoursWorked; // Store hours worked for the day

                // Read the schedule for each time slot of the day
                for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                    char isBooked, typeChar;
                    // Attempt to read booking status and type, handle errors if parsing fails
                    if (!(ss >> isBooked >> typeChar)) {
                        cerr << RED << "Error reading slot data for Week " << week + 1 << " Day " << day + 1 << " from line: " << line << RESET << endl;
                        break; // Break out of the loop if there's an error
                    }
                    // Set booking status based on the read data
                    expert.schedule[day][slot].isBooked = (isBooked == '1');
                    // Determine the type of session based on the character read
                    if (typeChar == 'T') expert.schedule[day][slot].type = TREATMENT;
                    else if (typeChar == 'C') expert.schedule[day][slot].type = CONSULTATION;
                    else expert.schedule[day][slot].type = UNAVAILABLE;
                }
            }
        }
        scheduleFile.close(); // Close the schedule file after reading
    }
    else {
        // If the file doesn't exist, initialize a clean schedule
        cout << "No existing schedule found for " << expert.name << ". Starting with a clean schedule." << endl;
        initializeCleanSchedule(expert);
    }
}

// Function to handle payment methods and process the payment
bool handlePaymentMethod(PaymentMethod method) {
    string otp; // Variable to store the one-time password
    int retryCount = 3; // Number of retries for OTP verification
    int inputRetryCount = 3; // Number of retries for user input

    switch(method) {
        case EWALLET: { // Handle E-Wallet payment
            string phoneNumber;
            // Loop until a valid phone number is entered or retries are exhausted
            while (inputRetryCount > 0) {
                cout << "Enter your phone number: ";
                cin >> phoneNumber; // Read the phone number
                if (isValidPhoneNumber(phoneNumber)) { // Validate phone number
                    otp = generateOTP(); // Generate OTP
                    cout << "OTP for E-Wallet sent to " + phoneNumber + ": " + otp << endl;
                    // Loop for OTP verification
                    while (retryCount > 0) {
                        if (verifyOTP(otp)) {
                            cout << "OTP verified successfully!" << endl;
                            cout << "Proceeding to payment..." << endl;
                            return true; // Payment successful
                            break;
                        } else {
                            retryCount--; // Decrement retry count
                            if (retryCount > 0) {
                                cout << RED << "Invalid OTP. You have " << retryCount << " attempts remaining." << RESET << endl;
                            } else {
                                cout << RED << "Failed OTP verification. Cancelling payment process." << RESET << endl;
                                return false; // Payment failed
                            }
                        }
                    }
                } else {
                    inputRetryCount--; // Decrement input retry count
                    if (inputRetryCount > 0) {
                        cout << RED << "Invalid phone number format! Phone number should be 01X-XXXXXXX or 01X-XXXXXXXX" << endl;
                        cout << "You have " << inputRetryCount << " tries left." << RESET << endl;
                    } else {
                        cout << RED << "Failed to provide a valid phone number. Cancelling payment process." << RESET << endl;
                        return false; // Payment failed
                    }
                }
            }
            break;
        }

        case BANK_TRANSFER: { // Handle Bank Transfer payment
            string bankAccountNumber;
            // Loop until a valid bank account number is entered or retries are exhausted
            while (retryCount > 0) {
                cout << "Enter your bank account number (10-12 digits): ";
                cin >> bankAccountNumber; // Read the bank account number
                if (isValidBankAccountNumber(bankAccountNumber)) { // Validate account number
                    otp = generateOTP(); // Generate OTP
                    cout << "OTP for Bank Transfer sent to your registered email: " + otp << endl;
                    // Loop for OTP verification
                    while (retryCount > 0) {
                        if (verifyOTP(otp)) {
                            cout << "OTP verified successfully!" << endl;
                            cout << "Proceeding to payment..." << endl;
                            return true; // Payment successful
                        } else {
                            retryCount--; // Decrement retry count
                            if (retryCount > 0) {
                                cout << RED << "Invalid OTP. You have " << retryCount << " attempts remaining." << RESET << endl;
                            } else {
                                cout << RED <<  "Failed OTP verification. Cancelling payment process." << RESET << endl;
                                return false; // Payment failed
                            }
                        }
                    }
                } else {
                    inputRetryCount--; // Decrement input retry count
                    if (inputRetryCount > 0) {
                        cout << RED << "Invalid bank account number!" << endl;  
                        cout << "You have " << inputRetryCount << " tries left." << RESET << endl; 
                    } else {
                        cout << RED << "Failed to provide a valid bank account number. Cancelling payment process." << RESET << endl;
                        return false; // Payment failed
                    }
                }
            }
            break;
        }

        case CREDIT_CARD: { // Handle Credit Card payment
            string cardNumber, cvv;
            // Loop until valid credit card details are entered or retries are exhausted
            while (inputRetryCount > 0) {
                cout << "Enter your 16-digit Credit Card Number: ";
                cin >> cardNumber; // Read credit card number
                cout << "Enter your 3-digit CVV: ";
                cin >> cvv; // Read CVV
                if (isValidCreditCard(cardNumber, cvv)) { // Validate card details
                    otp = generateOTP(); // Generate OTP
                    cout << "OTP for Credit Card sent to your registered email: " + otp << endl;
                    // Loop for OTP verification
                    while (retryCount > 0) {
                        if (verifyOTP(otp)) {
                            cout << "OTP verified sucessfully!" << endl;
                            cout << "Proceeding to payment..." << endl;
                            return true; // Payment successful
                        } else { 
                            retryCount--; // Decrement retry count
                            if (retryCount > 0) {
                                cout << RED << "Invalid OTP. You have " << retryCount << " attempts remaining." << RESET << endl;
                            } else {
                                cout << RED << "Failed OTP verification. Cancelling payment process." << RESET << endl;
                                return false; // Payment failed
                            }
                        }
                    }
                } else {
                    inputRetryCount--; // Decrement input retry count
                    if (inputRetryCount > 0) {
                        cout << RED << "Invalid credit card details!" << endl;
                        cout << "You have " << inputRetryCount << " tries left." << RESET << endl;
                    } else {
                        cout << RED << "Failed to provide valid credit card details. Cancelling payment process." << RESET << endl;
                        return false; // Payment failed
                    }
                }
            }
            break;
        }

        default: // Handle invalid payment method
            cout << RED << "Invalid payment method selected!" << RESET << endl;
            return false;  // Payment failed
    }
    return false; // Return false if no valid method was selected
}

// Function to display a menu for selecting a payment method
PaymentMethod selectPaymentMethod() {
    clearScreen(); // Clear the screen before displaying the menu
    cout << "Select payment method:\n";
    cout << "1. E-Wallet\n";
    cout << "2. Bank Transfer\n";
    cout << "3. Credit Card\n";
    cout << "Enter your choice (1-3 or -999 to go back): "; 
    int choice;
    choice = getValidatedInput(1, 3); // Get user input and validate it

    if (choice == -999) { 
        return CANCELLED; // Return cancelled if user chooses to go back
    }

    // Return the corresponding payment method based on user choice
    switch (choice) {
    case 1: 
        return EWALLET;
    case 2: 
        return BANK_TRANSFER;
    case 3: 
        return CREDIT_CARD;
    default:
        cout << RED << "Invalid choice. Defaulting to E-Wallet." << RESET << endl;
        return EWALLET; // Default to E-Wallet if an invalid choice is made
    }
}

// Function to load the booking counter from a file
int loadBookingCounter(const string& filename) {
    int counter = 0; // Initialize counter to 0
    ifstream bookingCounterFile(filename); // Open the booking counter file

    // Check if the file is successfully opened
    if (bookingCounterFile.is_open()) {
        bookingCounterFile >> counter; // Read the counter value
        bookingCounterFile.close(); // Close the file
    }
    return counter; // Return the booking counter
}

// Function to save the booking counter to a file
void saveBookingCounter(const string& filename, int counter) {
    ofstream bookingCounterFile(filename); // Open the booking counter file for writing
    if (bookingCounterFile.is_open()) {
        bookingCounterFile << counter; // Write the counter value
        bookingCounterFile.close(); // Close the file
    }
    else {
        cerr << RED << "Error: Unable to open file " << filename << RESET << endl; // Handle file opening error
    }
}

// Function to generate a new booking number
string generateBookingNumber() {
    static int bookingCounter = loadBookingCounter("booking_counter.txt"); // Load the current booking counter
    stringstream ss; // Create a string stream for formatting the booking number
    ss << "B" << setw(3) << setfill('0') << ++bookingCounter; // Format the booking number
    saveBookingCounter("booking_counter.txt", bookingCounter); // Save the updated booking counter
    return ss.str(); // Return the formatted booking number
}

// Function to generate a receipt and print it to the console
void generateReceipt(const Receipt& receipt) {
    // Print receipt header and company information
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
    // Print receipt details
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

// Function to generate a receipt file and save it to the specified filename
void generateReceiptFile(const Receipt& receipt, const string& filename) {
    // Open the receipt file for writing
    createDirectoryIfNotExists("receipts"); // Ensure the schedules directory exists
    ofstream receiptFile(filename);

    // Check if the file opened successfully
    if (!receiptFile.is_open()) {
        cerr << RED << "Error: Unable to open file " << filename << RESET << endl;
        return; // Exit if unable to open file
    }
    // Print receipt header and company information to the file
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
    // Print receipt details to the file
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

    // Close the receipt file after writing
    receiptFile.close();
}

// Function to print the receipt file using the default application
void printReceipt(const string& filename) {
#ifdef _WIN32
    // On Windows, use ShellExecute to open the receipt file
    ShellExecute(NULL, L"open", wstring(filename.begin(), filename.end()).c_str(), NULL, NULL, SW_SHOWNORMAL);
#else   
    // On other platforms, use the open command to print the file
    string command = "open " + filename;
    system(command.c_str());
#endif
}

// Function to make a booking for a service with an expert
void makeBooking(Expert& expert, Service service, SessionType sessionType, Customer& customer) {
    // Display the calendar for the expert
    displayCalendar(expert);
    int chosenWeek = chooseWeek(); // Let user choose a week for booking
    if (chosenWeek == -1) {
        return; // Exit if the week selection is canceled
    }
    loadScheduleFromFile(expert, chosenWeek); // Load the expert's schedule for the chosen week
    displaySchedule(expert, chosenWeek);  // Display the loaded schedule
    // Set the price based on session type
    double price = sessionType == TREATMENT ? service.price : 60.0;

    // Prompt user to select a time slot
    int* result = selectTimeSlot(expert, chosenWeek, sessionType);
    if (result == nullptr) {
        return; // Exit if no valid time slot is selected
    }
    int day = result[0];
    int slot = result[1];

    // Calculate the date for the booking
    int startDate = 1 + (chosenWeek * 7);
    int date = startDate + day;

    // Check if a valid day and slot are selected
    if (day != -1 && slot != -1) {
        // Book the selected time slot
        int duration = (sessionType == TREATMENT) ? TREATMENT_SLOT_DURATION : CONSULTATION_SLOT_DURATION;
        for (int i = 0; i < duration; ++i) {
            expert.schedule[day][slot + i].isBooked = true; // Mark the slot as booked
            expert.schedule[day][slot + i].type = sessionType; // Set the session type
        }
        expert.hoursWorkedPerDay[day] += duration; // Update hours worked for the expert

        // Generate time range for the booking
        string startTime = to_string(START_HOUR + slot) + ":00";
        string endTime = to_string(START_HOUR + slot + duration) + ":00";
        clearScreen();  // Clear the screen for confirmation
        // Display booking confirmation details
        cout << "==========================================" << endl;
        cout << "          Booking Confirmation           " << endl;
        cout << "==========================================" << endl;
        cout << "Please confirm your booking details: \n";
        cout << "Service: " << service.name << endl; // Display selected service
        cout << "Session Type: " << (sessionType == TREATMENT ? "Treatment" : "Consultation") << endl; // Show session type
        cout << "Date: " << to_string(date) << " July 2024" << endl; // Display booking date
        cout << "Time Slot: " << startTime << " - " << endTime << endl; // Show time slot
        cout << "Price: RM " << fixed << setprecision(2) << price << endl; // Show total price
        cout << "==========================================" << endl;
        // Prompt the user to confirm the booking
        cout << "Confirm booking? (Y to proceed to payment/ any other key to stop booking): ";
        char confirm; // Variable to hold user confirmation input
        cin >> confirm; // Read user input

        // Check if the user confirmed the booking
        if (tolower(confirm) == 'y') {
            // User confirmed, proceed to payment selection
            PaymentMethod paymentMethod = selectPaymentMethod();  // Select payment method
            if (paymentMethod == CANCELLED) { // Check if payment selection was canceled
                return; // Exit the function if canceled
            }
            // Verify payment method
            if (handlePaymentMethod(paymentMethod)) {
                // Verify payment method
                string bookingNumber = generateBookingNumber();
                // Create a receipt with booking details
                Receipt receipt = { bookingNumber, customer, expert, sessionType, service.name, to_string(date), startTime + " - " + endTime, paymentMethod, price };
                generateReceipt(receipt); // Generate the receipt for printing

                // Display success message for the booking
                cout << "==========================================" << endl;
                cout << "             Booking Succeed              " << endl;
                cout << "==========================================" << endl;
                cout << "You have successfully booked the slot on Day " << day + 1
                    << " from " << expert.schedule[day][slot].timeRange.substr(0, 5) << " to " << endTime
                    << " with " << expert.name << " for " << service.name << " ("
                    << (sessionType == TREATMENT ? "Treatment" : "Consultation") << ")." << endl;
                cout << "==========================================" << endl;

                if (expert.hoursWorkedPerDay[day] >= MAX_WORK_HOURS) {
                    // If the expert reaches the max hours, mark remaining slots as unavailable
                    for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                        if (!expert.schedule[day][slot].isBooked) {
                            expert.schedule[day][slot].type = UNAVAILABLE;
                        }
                    }
                    cout << "Expert has reached the maximum working hours for the day. Remaining slots are now unavailable.\n";
                }

                // Prepare to save the receipt to a file
                string receiptFileName = "receipts/receipt_" + bookingNumber + ".txt";
                generateReceiptFile(receipt, receiptFileName); // Save the receipt to a file
                printReceipt(receiptFileName); // Print the receipt

                // Save the booking information and updated schedule
                saveBooking(receipt); // Save booking details
                saveScheduleToFile(expert, chosenWeek); // Save updated schedule to file
            } else {
                // Payment verification failed, inform the user
                cout << RED << "Payment verification failed. Booking canceled." << RESET << endl;
            }
        }
        else {
            // User chose not to confirm the booking
            cout << "Booking cancelled." << endl;
        }
    }
}

// Function to manage customer-related operations
void customerManagement() {
    Customer customers[100]; // Array to hold customer data
    int choice, customerCount = 0; // Variables for user choice and customer count
    int loggedInCustomerIndex = -1; // Index of the logged-in customer
    clearScreen(); // Clear the screen for a fresh display
    loadCustomersFromFile(customers, customerCount); // Load customer data from a file

    // Loop for customer management options
    do {
        displayLogo(); // Display the logo
        cout << "Welcome!\n";
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "Option" << " | " << left << setw(DESC_WIDTH) << "Description" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "1" << " | " << setw(DESC_WIDTH) << "Sign Up (New Customer)" << " |" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "2" << " | " << setw(DESC_WIDTH) << "Login (Existing Customer)" << " |" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "3" << " | " << setw(DESC_WIDTH) << "Back" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;
        cout << "Enter your choice: "; // Prompt for user choice

        choice = getValidatedInput(1, 3); // Validate user input

        switch (choice) { // Handle user choice
        case 1:
            customerSignUp(customers, customerCount); // Call sign-up function for new customers
            pauseAndClear(); // Pause and clear the screen after signing up
            break;
        case 2:
            loggedInCustomerIndex = customerLogin(customers, customerCount); // Call login function
            if (loggedInCustomerIndex != -1) { // Check if login was successful
                customerMenu(customers[loggedInCustomerIndex]); // Show customer menu for the logged-in customer
            }
            pauseAndClear(); // Pause and clear the screen after login
            break;
        case 3:
            cout << "Returning to Main Menu\n"; // Inform the user they are returning to the main menu
            clearScreen(); // Clear the screen
            break;
        }
    } while (choice != 3); // Continue until the user chooses to go back
}

// Function to validate customer names using a regex pattern
bool isValidName(const string& name) {
    const regex pattern("^[A-Za-z]+(?: [A-Za-z]+)*$");
    return regex_match(trim(name), pattern);
}

// Function to validate phone numbers
bool isValidPhoneNumber(const string &phoneNumber) {
// Check for formats like 01X-XXXXXXX or 01X-XXXXXXXX
regex phonePattern("^(01[0-9]-[0-9]{7,8})$");
return regex_match(trim(phoneNumber), phonePattern);
}

// Function to validate email addresses
bool isValidEmail(const string& email) {
    const regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
    return regex_match(trim(email), pattern);
}

// Function to validate passwords
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

// Function to generate a 6-digit OTP (One Time Password)
string generateOTP() {
    srand(time(0)); // Seed random number generator
    string otp = "";
    for (int i = 0; i < 6; ++i) {
        otp += to_string(rand() % 10);  // Generate a 6-digit random number
    }
    return otp; // Return the generated OTP
}

// Function to verify the OTP entered by the user
bool verifyOTP(const string &generatedOTP) {
    string enteredOTP; // Variable to hold user input for OTP
    cout << "\nEnter the OTP sent to you: ";
    cin >> enteredOTP; // Read user input
    return enteredOTP == generatedOTP; // Check if entered OTP matches generated OTP
}

// Function for customer signup process
void customerSignUp(Customer customers[], int &customerCount) {
    const int MAX_CUSTOMERS = 100; // Maximum number of customers allowed
    if (customerCount >= MAX_CUSTOMERS) { // Check if the maximum limit is reached
        cout << RED << "Maximum number of customers reached!" << RESET << endl;
        return; // Exit if limit is reached
    }
    Customer newCustomer; // Create a new Customer object

    cout << "\n=== Customer Signup ===" << endl;
    cout << "[Enter -999 to go back]\n" << endl;
    // Loop to get a valid name from the user
    do {
        cout << "Enter your name: ";
        cin.ignore(); // Ignore any leftover input in the buffer
        getline(cin, newCustomer.name); // Read the full name
        if (trim(newCustomer.name) == "-999") { // Check for exit condition
            cout << YELLOW << "Returning to previous menu." << RESET << endl;
            return; // Exit if user chooses to go back
        }
        if (!isValidName(newCustomer.name)) { // Validate the name
            cout << RED <<  "\nName can only contain letters. Please try again.\n" << RESET;
            cin.clear(); // Clear input stream for new input
        }
    } while (!isValidName(newCustomer.name)); // Repeat until valid name is entered

    // Loop to get a valid contact number from the user
    do {
        cout << "Enter your contact number (including -): ";
        getline(cin, newCustomer.contact); // Read contact number
        if (trim(newCustomer.contact) == "-999") { // Check for exit condition
            cout << YELLOW << "Returning to previous menu." << RESET << endl;
            return; // Exit if user chooses to go back
        }
        if(!isValidPhoneNumber(newCustomer.contact)) {  // Validate contact number
            cout << RED <<  "\nInvalid contact number format.\nExample of contact: 012-3456789\nPlease try again.\n" << RESET;
        }
    } while (!isValidPhoneNumber(newCustomer.contact)); // Repeat until valid number is entered

    bool isUniqueEmail; // Flag to check for unique email

    // Loop to get a valid and unique email from the user
    do {
        cout << "Enter your email: ";
        getline(cin, newCustomer.email); // Read email
        if (trim(newCustomer.email) == "-999") { // Check for exit condition
            cout << YELLOW << "Returning to previous menu." << RESET << endl;
            return; // Exit if user chooses to go back
        }
        isUniqueEmail = true; // Assume the email is unique
        for (int i=0; i<customerCount; i++) { // Check against existing customers
            if (trim(customers[i].email) == trim(newCustomer.email)) {
                cout << RED << "\nThis email is already registered. Please try a different email.\n" << RESET;
                isUniqueEmail = false; // Mark as not unique
                break; // Exit loop as duplicate found
            }
        }
        if (!isValidEmail(newCustomer.email)) { // Validate email format
            cout << RED << "\nInvalid email format. Please try again.\n " << RESET;
            isUniqueEmail = false;
        }
    } while (!isValidEmail(newCustomer.email) || !isUniqueEmail);  // Validate email format


    // Loop to get a valid password from the user
    do {
        cout << "Enter your password (min 8 characters, must include uppercase, lowercase, digit, and special character):\n";
        newCustomer.password = getPasswordInput(); // Get password securely
        if (trim(newCustomer.password) == "-999") { // Check for exit condition
            cout << YELLOW << "Returning to previous menu." << RESET << endl;
            return; // Exit if user chooses to go back
        }
        if (!isValidPassword(trim(newCustomer.password))) { // Validate password format
            cout << RED << "Invalid password format.\n"
                << "Password should be:\n "
                << "- least 8 characters long\n"
                << "- contain at least one uppercase letter, one lowercase letter\n"
                << "- one digit and one special character" << RESET << endl;
        }
    }
     while (!isValidPassword(trim(newCustomer.password))); // Repeat until valid password is entered

    // Save the new customer to the array and increment the customer count
    customers[customerCount] = newCustomer;
    customerCount++;

    // Save updated customer list to file
    saveCustomersToFile(customers, customerCount);
}

// Function for customer login process
int customerLogin(Customer customers[], int customerCount) {
    string email, password; // Function for customer login process
    cout << "\n=== Customer Login ===" << endl;
    cout << "[Enter -999 to go back]\n" << endl;
    cout << "Enter your email: ";
    cin.ignore(1000, '\n'); // Ignore any leftover input
    getline(cin, email); // Read email
    if (trim(email) == "-999") { // Check for exit condition
        cout << YELLOW << "Returning to previous menu." << RESET << endl;
        return -1; // Exit if user chooses to go back
    }

    cout << "Enter your password: ";
    password = getPasswordInput(); // Get password securely
    if (trim(password) == "-999") { // Check for exit condition
        cout << YELLOW << "Returning to previous menu." << RESET << endl;
        return -1; // Exit if user chooses to go back
    }

    // Loop to validate email and password against existing customers
    for (int i = 0; i < customerCount; i++) {
        if (customers[i].email == email && customers[i].password == password) { // Check for matching credentials
            return i; // Return index of the logged-in customer
        }
    }
    cout << RED << "Login failed. Please try again.\n" << RESET; // Inform user of failed login
    pauseAndClearInput(); // Pause and clear input
    return -1; // Return -1 if login failed
}

// Function to save customer data to a file.
void saveCustomersToFile(Customer customers[], int customerCount) {
    // Open the file in output mode
    ofstream outFile("customers.txt", ios::out);

    // Check if the file was successfully opened for writing
    if (!outFile) {
        cerr << RED << "Error opening file for writing." << RESET << endl;
        return;
    }

    // Loop through each customer and save their details 
    for (int i = 0; i < customerCount; i++) {
        outFile << customers[i].name << ","
            << customers[i].contact << ","
            << customers[i].email << ","
            << customers[i].password << endl;
    }

    // Close the file after writing
    outFile.close();
    cout << "Customer saved to file successfully!" << endl;
}   

// Function to load customer data from a file.
void loadCustomersFromFile(Customer customers[], int& customerCount) {
    const int MAX_CUSTOMERS = 100;
    // Open the file in input mode
    ifstream inFile("customers.txt");

    // Check if the file exists, if not, start fresh
    if (!inFile) {
        cout << "No existing file for customers. Starting with a clean file." << endl;
        return;
    }

    string line;
    customerCount = 0;

    // Read each line, split the data by commas, and store the customer details
    while (getline(inFile, line) && customerCount < MAX_CUSTOMERS) {
        stringstream ss(line); // For splitting the line
        string name, email, contact, password;

        getline(ss, name, ',');
        getline(ss, contact, ',');
        getline(ss, email, ',');
        getline(ss, password, ',');

        // Populate the customers array with the data
        customers[customerCount].name = name;
        customers[customerCount].email = email;
        customers[customerCount].contact = contact;
        customers[customerCount].password = password;

        customerCount++;
    }

    // Close the file after reading
    inFile.close();
}

// Function to check and display an expert's schedule based on user input.
void checkSchedule() {
    int choice, week;

    // Initialize the experts with names
    Expert alice, bob, carol;
    initializeExpert(alice, "Alice");
    initializeExpert(bob, "Bob");
    initializeExpert(carol, "Carol");
    Expert experts[] = { alice, bob, carol };

    // Display options for selecting an expert
    cout << "Experts: " << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "| Option    | Description                                |" << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "| [1]       | Alice                                      |" << endl;
    cout << "| [2]       | Bob                                        |" << endl;
    cout << "| [3]       | Carol                                      |" << endl;
    cout << "+-----------+--------------------------------------------+" << endl;

    // Get the user's choice for expert
    cout << "Check schedule for: (-999 to go back)";
    choice = getValidatedInput(1, 3);
    if (choice == -999) {
        return;
    }
    // Get the week number from the user
    cout << "Enter week number (1-5): ";
    week = getValidatedInput(1, 5);
    if (week == -999) {
        return; // Return to the previous menu if the user enters -999
    }
    --week; // Adjust for zero-based index

    // Clear the screen and load/display the schedule for the selected expert and week
    clearScreen();
    loadScheduleFromFile(experts[choice - 1], week);
    displaySchedule(experts[choice - 1], week);

}

// Function to display the customer menu and handle customer-related actions.
void customerMenu(Customer& customer) {
    int choice;

    do {
        clearScreen();  // Clear the screen before displaying the menu
        displayLogo();   // Display the logo
        // Display the customer menu options
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

        // Get the user's menu choice
        choice = getValidatedInput(1, 7);

        // Handle the different options based on user choice
        switch (choice) {
         case 1: aboutUs(); break;                             // Option 1: View organization details
        case 2: viewServices(customer); break;                // Option 2: View services
        case 3: viewExperts(); break;                         // Option 3: View experts
        case 4: checkSchedule(); break;                       // Option 4: Check schedule
        case 5: viewServices(customer); break;                // Option 5: Make booking
        case 6: displayCustomerBookings(customer); pauseAndClear(); break;  // Option 6: View customer bookings
        case 7: cout << "Returning to Main Menu\n";           // Option 7: Return to main menu
            clearScreen();
            break;
        }

        // Pause and clear the screen after actions
        if (choice != 7 && choice != 6) pauseAndClear();
    } while (choice != 7); // Loop until the user chooses to return to the main menu
}

// Function to get masked password input
string getPasswordInput() {
#ifdef _WIN32
    string password;
    char ch;
    // Initialize password and read character-by-character from input
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                password.pop_back(); // Remove last character from password
                cout << "\b \b"; // Remove last '*' from screen
            }
        }
        else {
            password.push_back(ch); // Append character to password
            cout << '*'; // Display '*' for hidden input
        }
    }
    cout << endl;
#else
    // Disable terminal echo and canonical mode for Linux/Unix
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

// Function to handle login for admin and experts
bool adminExpertLogin(UserType& userType, string& userName) {
    clearScreen();
    displayLogo();

    // Display prompt to go back to previous menu
    cout << "[Input -999 to go back]" << endl;

    // Predefined user data
    User alice = { "alice123", "Alice1234$", EXPERT };
    User bob = { "bob123", "Bob1234$", EXPERT };
    User carol = { "carol123", "Carol1234$", EXPERT };
    User admin = { "admin123", "Admin1234$", ADMIN };

    User users[] = { alice, bob, carol, admin };

    string username, password;
    // Prompt for username input
    cout << "\nEnter username: ";
    cin >> username;
    if (trim(username) == "-999") { // Handle return to previous menu
        cout << YELLOW << "Returning to previous menu." << RESET << endl;
        pauseAndClearInput();
        return false;
    }
    cin.ignore(1000, '\n'); // Ignore leftover input buffer
    // Prompt for password input
    cout << "Enter password: ";
    password = getPasswordInput();
    if(trim(password) == "-999") { // Handle return to previous menu
        cout << YELLOW << "Returning to previous menu." << RESET << endl;
        pauseAndClearInput();
        return false;
    }

    // Check if username and password match any predefined user
    for (int i = 0; i < 4; i++) {
        if (users[i].username == username && users[i].password == password) {
            userType = users[i].type;  // Set user type (Admin/Expert)
            // Remove last 3 characters from username
            userName = users[i].username.substr(0, users[i].username.length() - 3);
            return true;
        }
    }

    // Display login failure message if no match is found
    cout << RED << "Login failed. Please try again.\n" << RESET;
    pauseAndClearInput();
    return false;
}

// Function to view schedule from expert menu
void viewExpertSchedule(const string& expertName) {
    Expert expert;
    initializeExpert(expert, expertName);
    for (int week = 0; week < 5; ++week) {
        loadScheduleFromFile(expert, week);
        cout << "Week " << week + 1 << " Schedule for " << expertName << ":\n";
        bool hasBookings = false;
        // Check if there are any bookings for the week
        for (int day = 0; day < 5; ++day) {
            for (int slot = 0; slot < 8; ++slot) {
                if (expert.schedule[day][slot].isBooked) {
                    hasBookings = true;
                    break;
                }
            }
            if (hasBookings) break;
        }
        // Display message if no bookings are found
        if (!hasBookings) {
            cout << "No bookings for this week.\n";
        }
        // Display the week's schedule
        displaySchedule(expert, week);
    }
}

// Function to initialize empty schedule for expert
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

// Function to view schedule for all experts
void viewAllSchedules() {
    string expertNames[] = { "Alice", "Bob", "Carol" };
    for (int i = 0; i < 3; i++) {
        viewExpertSchedule(expertNames[i]);
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get(); // Wait for user input before continuing
    }
}

// Function to display list of customers with booking counts
void displayCustomers(const Customer customers[], int customerCount, int bookingCounts[]) {
    int choice;
    cout << "+-----+-----------------------------+---------------------------+-----------------+" << endl;
    cout << "| No  | Customer Name               | Email                     | Booking Count   |" << endl;
    cout << "+-----+-----------------------------+---------------------------+-----------------+" << endl;
    for (int i = 0; i < customerCount; ++i) {
        // Display customer details in a formatted table
        cout << "| " << setw(2) << BLUE << "[" << i + 1 << "]" << RESET << " | " << setw(27) << customers[i].name
            << " | " << setw(25) << customers[i].email
            << " | " << setw(15) << bookingCounts[i] << " |" << endl;
    }
    cout << "+----+------------------------------+---------------------------+-----------------+" << endl;
    cout << "\nSelect a customer to view their information (-999 to go back): " << endl;
    choice = getValidatedInput(1, customerCount); // Get valid input
    if (choice == -999) {
        return; // Handle return to previous menu 
    }

    displayCustomerDetails(customers[choice - 1]); // Display selected customer details
}

// Function to generate and display sales report
void generateSalesReport() {
    // Initialize receipts and load booking data
    Receipt allReceipts[200];
    int receiptCount = loadBookings(allReceipts);
    string services[] = { "Facial", "Botox and Fillers", "Manicure" };
    string experts[] = { "Alice", "Bob", "Carol" };
    double facialRevenue = 0, botoxRevenue = 0, manicureRevenue = 0;
    double aliceRevenue = 0, bobRevenue = 0, carolRevenue = 0;
    double serviceRevenue[3] = { 0 };
    double expertRevenue[3] = { 0 };

    // Check if there are any bookings
    if (receiptCount == 0) {
        cout << RED << "No bookings found. Unable to generate sales report." << RESET << endl;
        return;
    }

    double totalRevenue = 0;
    int totalBookings = 0;

    // Display detailed sales report table
    cout << "\n+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "|                                                  Detailed Sales Report                                                      |" << endl;
    cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| Booking #  | Date          | Time Slot         | Service Name        | Expert  | Customer Email          | Amount Paid (RM) |" << endl;
    cout << "+------------+---------------+-------------------+---------------------+---------+-------------------------+------------------+" << endl;

    // Loop through each receipt to calculate totals and display details
    for (int i = 0; i < receiptCount; ++i) {
        // Calculate total revenue and bookings
        totalRevenue += allReceipts[i].amountPaid;
        totalBookings++;

        // Calculate revenue by service
        if (trim(allReceipts[i].serviceName) == "Facial") {
            facialRevenue += allReceipts[i].amountPaid;
        }
        else if (trim(allReceipts[i].serviceName) == "Botox and Fillers") {
            botoxRevenue += allReceipts[i].amountPaid;
        }
        else if (trim(allReceipts[i].serviceName) == "Manicure") {
            manicureRevenue += allReceipts[i].amountPaid;
        }

        // Calculate revenue by expert
        if (trim(allReceipts[i].expert.name) == "Alice") {
            aliceRevenue += allReceipts[i].amountPaid;
        }
        else if (trim(allReceipts[i].expert.name) == "Bob") {
            bobRevenue += allReceipts[i].amountPaid;
        }
        else if (trim(allReceipts[i].expert.name) == "Carol") {
            carolRevenue += allReceipts[i].amountPaid;
        }

        // Display each receipt's booking details in a table
        cout << "| " << setw(10) << left << allReceipts[i].bookingNumber
            << " | " << setw(13) << left << allReceipts[i].date + " July 2024"
            << " | " << setw(17) << left << allReceipts[i].timeSlot
            << " | " << setw(19) << left << allReceipts[i].serviceName
            << " | " << setw(7) << left << allReceipts[i].expert.name
            << " | " << setw(23) << left << allReceipts[i].customer.email
            << " | RM " << setw(12) << right << fixed << setprecision(2) << allReceipts[i].amountPaid << "  |" << endl;
    }
    // Display table footer
    cout << "+------------+---------------+-------------------+---------------------+---------+-------------------------+------------------+" << endl;
    
    // Calculate service and expert revenues
    double allSales[6] = { facialRevenue, botoxRevenue, manicureRevenue, aliceRevenue, bobRevenue, carolRevenue };
    const char* serviceNames[6] = { "Facial", "Botox", "Manicure", "Alice", "Bob", "Carol"};

    // Find maximum revenue for scaling histogram
    double MAX = 0;
    for (int i = 0; i < 6; i++) {
        if (allSales[i] > MAX) {
            MAX = allSales[i];
        }
    }

    int roundedMax = ((int)(MAX / 50) + 1) * 50;

    // Display overall sales summary
    cout << "\n+-----------------------------------+" << endl;
    cout << "|          Sales Summary            |" << endl;
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

    // Display revenue breakdown in a histogram
    printf("\n\t\tSALES REVENUE HISTOGRAM\n\n");

    for (int yaxis = roundedMax; yaxis >= 0; yaxis -= 50) {
        printf("%4d %c", yaxis, 179);
        for (int i = 0; i < 6; i++) {
            if (allSales[i] >= yaxis) {
                printf("   %c%c%c%c", 178, 178, 178, 178);
            }
            else {
                printf("       ");
            }
        }
        printf("\n");
    }

    printf("     %c", 192);
    for (int i = 0; i < 50; i++) {
        printf("%c", 196);
    }
    printf("\n       ");
    for (int i = 0; i < 6; i++) {
        printf(" %s ", serviceNames[i]);
    }
    printf("\n\n");
}

// Function to display details about a customer
void displayCustomerDetails(Customer customer) {
    cout << "\n+-----------------------------------+" << endl;
    cout << "|          Customer Details         |" << endl;
    cout << "+-----------------------------------+" << endl;
    cout << "| Name    : " << setw(23) << left << customer.name << " |" << endl;
    cout << "| Email   : " << setw(23) << left << customer.email << " |" << endl;
    cout << "| Contact : " << setw(23) << left << customer.contact << " |" << endl;
    cout << "+-----------------------------------+" << endl;
    // Note: For security reasons, customer password is not displayed
}

// Function to to count bookings for a specific customer with a specific expert
int countCustomerExpertBookings(const Receipt receipts[], int receiptCount, const string& customerName, const string& expertName) {
    int count = 0;
    // Loop through all receipts to find matches for the customer and expert
    for (int i = 0; i < receiptCount; ++i) {
        // Check if both customer name and expert name match the receipt
        if (trim(receipts[i].customer.name) == trim(customerName) && trim(receipts[i].expert.name) == trim(expertName)) {
            ++count; // Increment count for each match found
        }
    }
    return count; // Return the total count of bookings for that customer and expert
}

// Function to sort customers by the number of bookings with a specific expert
void sortCustomersByExpertBookings(Customer customers[], int customerCount, const Receipt receipts[], int receiptCount, const string& expertName, int bookingCounts[]) {
    // Sort customers using bubble sort based on the number of bookings with the given expert
    for (int i = 0; i < customerCount - 1; ++i) {
        for (int j = i + 1; j < customerCount; ++j) {
            // Get the number of bookings for both customers
            int bookingsA = countCustomerExpertBookings(receipts, receiptCount, customers[i].name, expertName);
            int bookingsB = countCustomerExpertBookings(receipts, receiptCount, customers[j].name, expertName);

            // Swap customers if bookingsA is less than bookingsB (i.e., sort in descending order)
            if (bookingsA < bookingsB) {
                // Swap customers[i] and customers[j]
                Customer tempCustomer = customers[i];
                customers[i] = customers[j];
                customers[j] = tempCustomer;

                // Swap corresponding booking counts
                int tempCount = bookingCounts[i];
                bookingCounts[i] = bookingCounts[j];
                bookingCounts[j] = tempCount;
            }
        }
    }
}

// Function to sort customers by name alphabetically (case-insensitive)
void sortCustomersByName(Customer customers[], int customerCount, int bookingCounts[]) {
    for (int i = 0; i < customerCount - 1; ++i) {
        int minIndex = i;
        
        for (int j = i + 1; j < customerCount; ++j) {
            // Convert both customer names to lowercase for comparison
            string currentName = customers[j].name;
            string minName = customers[minIndex].name;

            // Convert both strings to lowercase
            for (int k = 0; k < currentName.length(); ++k) {
                currentName[k] = tolower(currentName[k]);
            }
            for (int k = 0; k < minName.length(); ++k) {
                minName[k] = tolower(minName[k]);
            }

            // Compare the lowercase names
            if (currentName < minName) {
                minIndex = j;
            }
        }

        // Swap the found minimum element with the current element
        if (minIndex != i) { 
            // Swap customer objects
            Customer tempCustomer = customers[i];
            customers[i] = customers[minIndex];
            customers[minIndex] = tempCustomer;

            // Swap corresponding booking counts
            int tempCount = bookingCounts[i];
            bookingCounts[i] = bookingCounts[minIndex];
            bookingCounts[minIndex] = tempCount;
        }
    }
}

// Function to sort customers by their total number of bookings
void sortCustomersByTotalBookings(Customer customers[], int customerCount, int bookingCounts[]) {
    // Bubble sort to sort customers by total bookings (in descending order)
    for (int i = 0; i < customerCount - 1; i++) {
        for (int j = 0; j < customerCount - i - 1; j++) {
            // Compare adjacent elements and swap if the earlier element has fewer bookings
            if (bookingCounts[j] < bookingCounts[j + 1]) {
                // Swap customers[i] and customers[j]
                Customer tempCustomer = customers[j];
                customers[j] = customers[j + 1];
                customers[j + 1] = tempCustomer;
                // Swap corresponding booking counts
                int tempCount = bookingCounts[j];
                bookingCounts[j] = bookingCounts[j + 1];
                bookingCounts[j + 1] = tempCount;
            }
        }
    }
}

// Function to view customers based on expert or admin context
void viewCustomers(string expertName = "") {
    Receipt allReceipts[200]; // Array to hold all booking receipts
    int receiptCount = loadBookings(allReceipts); // Load bookings into allReceipts
    const int MAX_CUSTOMERS = 100; // Maximum number of customers that can be processed

    // Capitalize the first letter of expert name for comparison
    if (!expertName.empty()) {
        expertName[0] = toupper(expertName[0]);
    }

    Customer customers[MAX_CUSTOMERS]; // Array to store unique customers
    int bookingCounts[MAX_CUSTOMERS] = { 0 }; // Array to store booking counts for each customer
    int customerCount = 0; // Keeps track of the number of unique customers

    // Loop through all the receipts to filter and count customers
    for (int i = 0; i < receiptCount; i++) {
        // Skip receipts that do not match the expert name if one is provided
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
            bookingCounts[customerCount] = 1; // Start with 1 booking for the new customer
            customerCount++;
        }
    }
    // Display options for viewing or sorting customers
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
    choice = getValidatedInput(1, 3); // Get user input with validation
    if (choice == -999) {
        return; // Return to the previous menu if sentinel value (-999) is entered
    }

    // Handle the user's choice
    switch (choice) {
    case 1:
        displayCustomers(customers, customerCount, bookingCounts);  // Display unsorted customer data
        break;
        break;
    case 2:
        sortCustomersByName(customers, customerCount, bookingCounts); // Sort by customer name
        displayCustomers(customers, customerCount, bookingCounts); // Display sorted customers
        break;
    case 3:
        if (expertName.empty()) {
            sortCustomersByTotalBookings(customers, customerCount, bookingCounts); // Sort by total bookings for admin
        }
        else { // Sort by expert-specific bookings
            sortCustomersByExpertBookings(customers, customerCount, allReceipts, receiptCount, expertName, bookingCounts);
        }
        displayCustomers(customers, customerCount, bookingCounts); // Display sorted customers
        break;
    }
}

// Function to display admin/expert-specific menu based on user type
void adminExpertMenu(UserType& userType, string& userName) {
    int adminChoice, expertChoice;

    do {
        clearScreen();
        displayLogo(); // Display company logo
        cout << "\nAdmin/Expert Menu:\n";
        cout << "+--------+------------------------------+" << endl;
        cout << "| " << setw(OPTION_WIDTH - 1) << "Option" << " | " << left << setw(DESC_WIDTH) << "Description" << " |" << endl;
        cout << "+--------+------------------------------+" << endl;

        // Display expert-specific or admin-specific options
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

        // Expert options
        if (userType == EXPERT) {
            expertChoice = getValidatedInput(1, 3); // Validate input for expert

            switch (expertChoice) {
            case 1:
                viewExpertSchedule(userName); // Display the expert's own schedule
                break;
            case 2:
                viewCustomers(userName); // Display customers for the expert
                break;
            case 3: cout << "Returning to Main Menu\n"; 
                clearScreen(); // Return to the main menu
                break;
            }
            if (expertChoice != 3) pauseAndClear(); // Pause and clear screen unless returning to main menu
        }
        // Admin options
        else if (userType == ADMIN) {
            adminChoice = getValidatedInput(1, 4); // Validate input for admin

            switch (adminChoice) {
            case 1:
                viewAllSchedules(); // View all expert schedules for admin
                break;
            case 2:
                viewCustomers(); // View all customers for admin
                break;
            case 3:
                generateSalesReport(); // Generate sales report for admin
                break;
            case 4: cout << "Returning to Main Menu\n"; 
                clearScreen(); // Return to the main menu
                break;
            }

            if (adminChoice != 4) pauseAndClear(); // Pause and clear screen unless returning to main menu
        }
    } while ((userType == EXPERT && expertChoice != 3) || (userType == ADMIN && adminChoice != 4)); // Loop until user returns to the main menu
}

// Function to get validated input between min and max, with error handling for invalid inputs
int getValidatedInput(int min, int max) {
    int input, sentinel = -999;
    while (true) {
        cin >> input; // Read user input

        // Handle invalid inputs
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

// Function to display details about the organization (About Us page)
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

// Function to display service details and allow customers to book or return to the main menu
void serviceDesc(Service service, Customer& customer) {
    clearScreen(); // Clears the screen
    displayLogo(); // Displays the application logo
    cout << "===============================\n";
    cout << "         Service Details        \n";
    cout << "===============================\n";
    cout << "\n" << service.name << "\n"; // Displays the service name
    cout << "Description: " << service.description << "\n"; // Displays service description
    cout << "Treatment Price: RM" << fixed << setprecision(2) << service.price << "\n"; // Displays the price
    cout << "===============================\n";

    // Asks the user if they want to book the service
    cout << "Enter Y to book or any other key to return to main menu: ";
    char choice;
    cin >> choice;
    if (choice == 'Y' || choice == 'y') { // If 'Y' or 'y' is entered, proceed with booking
        // Initialize experts Alice, Bob, and Carol
        Expert alice, bob, carol;
        initializeExpert(alice, "Alice");
        initializeExpert(bob, "Bob");
        initializeExpert(carol, "Carol");
        Expert experts[] = { alice, bob, carol };

        // Displays a list of available experts and prompts the customer to choose one
        cout << "\n---------------------------\n";
        cout << "     Select Your Expert\n";
        cout << "---------------------------\n";
        cout << "We have the following beauty experts available:\n";
        cout << "  [1]  Alice\n";
        cout << "  [2]  Bob\n";
        cout << "  [3]  Carol\n";
        cout << "\nPlease select an expert by entering the number (1-3 or -999 to go back): ";
        int expertChoice;
        expertChoice = getValidatedInput(1, 3); // Ensures valid input between 1-3
        if (expertChoice == -999) {
            return; // Returns to the main menu if -999 is entered
        }

        Expert& selectedExpert = experts[expertChoice - 1]; // Assigns the chosen expert

        // Prompts the customer to choose between treatment or consultation session
        cout << "\n---------------------------\n";
        cout << "     Choose Session Type\n";
        cout << "---------------------------\n";
        cout << "Please choose the type of session you’d like:\n";
        cout << "  [1] Full Treatment - 2 hours of treatment\n";
        cout << "  [2] Consultation   - 1 hour personalized consultation\n";
        cout << "\nEnter your choice (1-2, -999 to go back): ";

        int sessionChoice;
        sessionChoice = getValidatedInput(1, 2); // Ensures valid input between 1-2
        if (sessionChoice == -999) {
            return; // Returns to the main menu if -999 is entered
        } 

        SessionType sessionType;
        // Assigns session type based on customer choice
        switch (sessionChoice) {
        case 1: sessionType = TREATMENT; break;
        case 2: sessionType = CONSULTATION; break;
        default:
            cout << RED <<  "Invalid choice. Exiting." << RESET;
            return;
        }
        // Calls the makeBooking function to book the selected service with the chosen expert
        makeBooking(selectedExpert, service, sessionType, customer);
    }
}

// Displays available services and allows the customer to choose a service to book
void viewServices(Customer& customer) {
    clearScreen(); // Clears the screen
    displayLogo(); // Displays the application logo

    // Predefined service details
    Service facial = { "Facial", "A rejuvenating facial treatment.", 150.00 };
    Service botox = { "Botox and Fillers", "Cosmetic injections for wrinkle treatment.", 250.00 };
    Service manicure = { "Manicure", "A relaxing manicure sesion.", 100.00 };

    Service services[3] = { facial, botox, manicure }; // Array of services
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

    choice = getValidatedInput(1, 3); // Ensures valid input between 1-3
    if (choice == -999) { 
        return; // Returns to the main menu if -999 is entered
    }

    // Calls the serviceDesc function to display the chosen service details
    serviceDesc(services[choice - 1], customer);
}

// Displays the details of a specific expert
void displayExpertDetails(Expert& expert) {
    // Different output depending on which expert is selected
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
        cout << "| Name: Bob                                              |" << endl;
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
    // Asks the customer if they want to view the expert's schedule
    char viewSchedule;
    cout << "Do you want to view this expert's schedule? (Enter Y or any other key ): " << endl;
    cin >> viewSchedule;

    if (tolower(viewSchedule) == 'y') {
        viewExpertSchedule(expert.name); // Calls function to display the expert's schedule
    }
}

// Displays available experts and their details
void viewExperts() {
    clearScreen(); // Clears the screen
    displayLogo(); // Displays the application logo

    int choice, week;
    Expert alice, bob, carol;
    initializeExpert(alice, "Alice");
    initializeExpert(bob, "Bob");
    initializeExpert(carol, "Carol");
    Expert experts[] = { alice, bob, carol };

    // Displays a list of experts
    cout << "Experts: " << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "| Option    | Description                                |" << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "| [1]       | Alice                                      |" << endl;
    cout << "| [2]       | Bob                                        |" << endl;
    cout << "| [3]       | Carol                                      |" << endl;
    cout << "+-----------+--------------------------------------------+" << endl;
    cout << "Select an expert to view their details (-999 to go back): ";
    choice = getValidatedInput(1, 3); // Ensures valid input between 1-3

    if (choice == -999) {
        return; // Returns to the main menu if -999 is entered
    }

    // Calls the function to display the chosen expert's details
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

// Clears the screen, cross-platform compatible
void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("pause"); // Pauses the screen on Windows
    system("cls");
#else
    system("clear"); // Clears the screen on UNIX-based systems
#endif
}

// Pauses the program by prompting the user to press Enter, then clears the screen
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
