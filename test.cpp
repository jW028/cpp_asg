#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <limits>

using namespace std;

struct Customer {
    string name;
    string contact;
};

struct Expert {
    string name;
    int weeklyHours;
    vector<bool> availability; // 5 days, 6 slots each
};

struct Service {
    string name;
    double price;
    int duration; // in hours
};

struct Booking {
    Customer customer;
    Expert* expert;
    Service service;
    int day;
    int slot;
};

vector<Customer> customers;
vector<Expert> experts;
vector<Service> services;
vector<Booking> bookings;

// Function prototypes
void displayLogo();
void displayMainMenu();
void customerMenu();
void ownerExpertMenu();
void viewOrganizationDetails();
void viewServices();
void viewExperts();
void checkSchedule();
void makeBooking();
void processPayment(double amount);
void viewBookedSchedule();
void expertLogin();
void viewIndividualSchedule(Expert& expert);
void viewOverallSchedule();
void viewCustomers();
void generateSalesReport();
void initializeData();
void saveData();
void loadData();
int getValidatedInput(int min, int max);
void clearScreen();
void pauseAndClear();
void updateExpertAvailability(Expert& expert, int day, int slot, int duration);
bool isSlotAvailable(const Expert& expert, int day, int slot, int duration);

int main() {
    loadData();
    initializeData();

    int choice;
    do {
        clearScreen();
        displayLogo();
        displayMainMenu();
        choice = getValidatedInput(1, 3);

        switch (choice) {
        case 1:
            customerMenu();
            break;
        case 2:
            ownerExpertMenu();
            break;
        case 3:
            cout << "Exiting program. Goodbye!\n";
            break;
        }
    } while (choice != 3);

    saveData();
    displayLogo(); // Ending logo
    return 0;
}

void displayLogo() {
    std::cout << "   __             _                                       __                              " << std::endl;
    std::cout << "  / /  ___   ___ | | _____ _ __ ___   __ ___  ____  __   / /  ___  _   _ _ __   __ _  ___ " << std::endl;
    std::cout << " / /  / _ \\ / _ \\| |/ / __| '_ ` _ \\ / _` \\ \\/ /\\ \\/ /  / /  / _ \\| | | | '_ \\ / _` |/ _ \\" << std::endl;
    std::cout << "/ /__| (_) | (_) |   <\\__ \\ | | | | | (_| |>  <  >  <  / /__| (_) | |_| | | | | (_| |  __/" << std::endl;
    std::cout << "\\____/\\___/ \\___/|_|\\_\\___/_| |_| |_|\\__,_/_/\\_\\/_/\\_\\ \\____/\\___/ \\__,_|_| |_|\\__, |\\___|" << std::endl;
    std::cout << "                                                                               |___/      " << std::endl;
    std::vector<std::string> ascii_art = {
        u8"⠀⠀⠀⠀⠀⠀⠀⣠⣤⣤⣤⣤⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        u8"⠀⠀⠀⠀⠀⢰⡿⠋⠁⠀⠀⠈⠉⠙⠻⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        u8"⠀⠀⠀⠀⢀⣿⠇⠀⢀⣴⣶⡾⠿⠿⠿⢿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        u8"⠀⠀⣀⣀⣸⡿⠀⠀⢸⣿⣇⠀⠀⠀⠀⠀⠀⠙⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        u8"⠀⣾⡟⠛⣿⡇⠀⠀⢸⣿⣿⣷⣤⣤⣤⣤⣶⣶⣿⠇⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀",
        u8"⢀⣿⠀⢀⣿⡇⠀⠀⠀⠻⢿⣿⣿⣿⣿⣿⠿⣿⡏⠀⠀⠀⠀⢴⣶⣶⣿⣿⣿⣆",
        u8"⢸⣿⠀⢸⣿⡇⠀⠀⠀⠀⠀⠈⠉⠁⠀⠀⠀⣿⡇⣀⣠⣴⣾⣮⣝⠿⠿⠿⣻⡟",
        u8"⢸⣿⠀⠘⣿⡇⠀⠀⠀⠀⠀⠀⠀⣠⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⠉⠀",
        u8"⠸⣿⠀⠀⣿⡇⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠉⠀⠀⠀⠀",
        u8"⠀⠻⣷⣶⣿⣇⠀⠀⠀⢠⣼⣿⣿⣿⣿⣿⣿⣿⣛⣛⣻⠉⠁⠀⠀⠀⠀⠀⠀⠀",
        u8"⠀⠀⠀⠀⢸⣿⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀",
        u8"⠀⠀⠀⠀⢸⣿⣀⣀⣀⣼⡿⢿⣿⣿⣿⣿⣿⡿⣿⣿⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        u8"⠀⠀⠀⠀⠀⠙⠛⠛⠛⠋⠁⠀⠙⠻⠿⠟⠋⠑⠛⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
    };

    for (const auto& line : ascii_art) {
        std::cout << line << std::endl;
    }
}

void displayMainMenu() {
    cout << "1. Customer\n";
    cout << "2. Owner/Expert\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
}

void customerMenu() {
    int choice;
    do {
        clearScreen();
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

        switch (choice) {
        case 1: viewOrganizationDetails(); break;
        case 2: viewServices(); break;
        case 3: viewExperts(); break;
        case 4: checkSchedule(); break;
        case 5: makeBooking(); break;
        case 6: viewBookedSchedule(); break;
        case 7: cout << "Returning to main menu...\n"; break;
        }
        if (choice != 7) pauseAndClear();
    } while (choice != 7);
}

void ownerExpertMenu() {
    expertLogin();
    int choice;
    do {
        clearScreen();
        displayLogo();
        cout << "\nOwner/Expert Menu:\n";
        cout << "1. View Individual Schedule\n";
        cout << "2. View Overall Schedule\n";
        cout << "3. View Customers\n";
        cout << "4. Generate Sales Report\n";
        cout << "5. Return to Main Menu\n";
        cout << "Enter your choice: ";

        choice = getValidatedInput(1, 5);

        switch (choice) {
        case 1: viewIndividualSchedule(experts[0]); break; // Simplified: always show first expert's schedule
        case 2: viewOverallSchedule(); break;
        case 3: viewCustomers(); break;
        case 4: generateSalesReport(); break;
        case 5: cout << "Returning to main menu...\n"; break;
        }
        if (choice != 5) pauseAndClear();
    } while (choice != 5);
}

void viewOrganizationDetails() {
    cout << "\nOrganization Details:\n";
    cout << "Name: Beauty Service Co.\n";
    cout << "Address: 123 Beauty Street, Glamour City\n";
    cout << "Phone: (555) 123-4567\n";
    cout << "Email: info@beautyservice.com\n";
}

void viewServices() {
    cout << "\nServices:\n";
    for (const auto& service : services) {
        cout << service.name << " - $" << fixed << setprecision(2) << service.price << " (" << service.duration << " hours)\n";
    }
}

void viewExperts() {
    cout << "\nExperts:\n";
    for (const auto& expert : experts) {
        cout << expert.name << "\n";
    }
}

void checkSchedule() {
    cout << "\nSchedule for this week:\n";
    cout << "   Mon Tue Wed Thu Fri\n";
    for (int slot = 0; slot < 6; ++slot) {
        cout << slot + 1 << "  ";
        for (int day = 0; day < 5; ++day) {
            bool available = false;
            for (const auto& expert : experts) {
                if (expert.availability[day * 6 + slot]) {
                    available = true;
                    break;
                }
            }
            cout << (available ? "O   " : "X   ");
        }
        cout << "\n";
    }
    cout << "O: Available, X: Booked\n";
}

void makeBooking() {
    cout << "\nMaking a booking:\n";

    // Select service
    viewServices();
    cout << "Select a service (1-" << services.size() << "): ";
    int serviceChoice = getValidatedInput(1, services.size()) - 1;
    Service& selectedService = services[serviceChoice];

    // Select expert
    viewExperts();
    cout << "Select an expert (1-" << experts.size() << "): ";
    int expertChoice = getValidatedInput(1, experts.size()) - 1;
    Expert& selectedExpert = experts[expertChoice];

    // Select day and slot
    cout << "Select a day (1-5, Monday to Friday): ";
    int day = getValidatedInput(1, 5) - 1;
    cout << "Select a starting time slot (1-6): ";
    int slot = getValidatedInput(1, 6) - 1;

    if (!isSlotAvailable(selectedExpert, day, slot, selectedService.duration)) {
        cout << "Sorry, this slot is not available for the selected service duration.\n";
        return;
    }

    // Create customer
    Customer customer;
    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, customer.name);
    cout << "Enter your contact number: ";
    getline(cin, customer.contact);

    // Create booking
    Booking booking{ customer, &selectedExpert, selectedService, day, slot };
    bookings.push_back(booking);

    // Update expert availability
    updateExpertAvailability(selectedExpert, day, slot, selectedService.duration);

    cout << "Booking confirmed!\n";
    processPayment(selectedService.price);
}

void processPayment(double amount) {
    cout << "Processing payment of $" << fixed << setprecision(2) << amount << "...\n";
    cout << "Payment successful!\n";
}

void viewBookedSchedule() {
    cout << "\nYour booked schedule:\n";
    for (const auto& booking : bookings) {
        cout << "Service: " << booking.service.name << "\n";
        cout << "Expert: " << booking.expert->name << "\n";
        cout << "Day: " << booking.day + 1 << ", Starting Slot: " << booking.slot + 1 << "\n";
        cout << "Duration: " << booking.service.duration << " hour(s)\n\n";
    }
}

void expertLogin() {
    cout << "Expert login successful.\n";
}

void viewIndividualSchedule(Expert& expert) {
    cout << "\nSchedule for " << expert.name << ":\n";
    cout << "   Mon Tue Wed Thu Fri\n";
    for (int slot = 0; slot < 6; ++slot) {
        cout << slot + 1 << "  ";
        for (int day = 0; day < 5; ++day) {
            cout << (expert.availability[day * 6 + slot] ? "O   " : "X   ");
        }
        cout << "\n";
    }
    cout << "O: Available, X: Booked\n";
    cout << "Weekly hours: " << expert.weeklyHours << "\n";
}

void viewOverallSchedule() {
    cout << "\nOverall Schedule:\n";
    checkSchedule();
}

void viewCustomers() {
    cout << "\nCustomer List:\n";
    for (const auto& customer : customers) {
        cout << "Name: " << customer.name << ", Contact: " << customer.contact << "\n";
    }
}

void generateSalesReport() {
    cout << "\nSales Report:\n";
    double totalSales = 0;
    for (const auto& booking : bookings) {
        totalSales += booking.service.price;
    }
    cout << "Total bookings: " << bookings.size() << "\n";
    cout << "Total sales: $" << fixed << setprecision(2) << totalSales << "\n";
}

void initializeData() {
    if (services.empty()) {
        services = {
            {"Haircut", 50.0, 1},
            {"Manicure", 30.0, 1},
            {"Facial", 80.0, 2}
        };
    }

    if (experts.empty()) {
        experts = {
            {"Alice", 0, vector<bool>(30, true)},
            {"Bob", 0, vector<bool>(30, true)},
            {"Charlie", 0, vector<bool>(30, true)}
        };
    }
}

void saveData() {
    // Implement file saving logic here
}

void loadData() {
    // Implement file loading logic here
}

int getValidatedInput(int min, int max) {
    int input;
    while (true) {
        cin >> input;
        if (cin.fail() || input < min || input > max) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between " << min << " and " << max << ": ";
        }
        else {
            return input;
        }
    }
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseAndClear() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    clearScreen();
}

void updateExpertAvailability(Expert& expert, int day, int slot, int duration) {
    for (int i = 0; i < duration; ++i) {
        if (slot + i < 6) {
            expert.availability[day * 6 + slot + i] = false;
        }
    }
    expert.weeklyHours += duration;
}

bool isSlotAvailable(const Expert& expert, int day, int slot, int duration) {
    for (int i = 0; i < duration; ++i) {
        if (slot + i >= 6 || !expert.availability[day * 6 + slot + i]) {
            return false;
        }
    }
    return true;
}