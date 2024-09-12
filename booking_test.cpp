#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>

using namespace std;

const int START_HOUR = 9;
const int END_HOUR = 17; // Operating hours from 9 AM to 5 PM
const int MAX_WORK_HOURS = 6; // Maximum work hours per day for an expert
const int CONSULTATION_SLOT_DURATION = 1;
const int TREATMENT_SLOT_DURATION = 2;
const int DAYS_IN_WEEK = 5; // Monday to Friday
const int MAX_SLOTS_PER_DAY = 8;  // Total slots available (8 hours)

enum PaymentMethod {
    EWALLET,
    BANK_TRANSFER,
    CREDIT_CARD
};

enum class SessionType {
    Treatment,
    Consultation,
    Unavailable
};

struct TimeSlot {
    bool isBooked;
    string timeRange;
    SessionType type;
};

struct Service {
    string name;
    double price;

    Service() : name(""), price(0.0) {} // Default constructor

    Service(string name, double price) : name(name), price(price) {}
};


struct Expert {
    string name;
    Service service;
    TimeSlot schedule[4][DAYS_IN_WEEK][MAX_SLOTS_PER_DAY];
    int hoursWorkedPerDay[4][DAYS_IN_WEEK];
    string dates[4];

    Expert() = default;

    Expert(const string &name, const Service &service) : name(name), service(service) {
        for (int week=0; week<4; ++week) {
            for (int day = 0; day < DAYS_IN_WEEK; ++day) {
                hoursWorkedPerDay[week][day] = 0;
                for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                    schedule[week][day][slot].isBooked = false;
                    schedule[week][day][slot].timeRange = to_string(START_HOUR + slot) + ":00 - " + to_string(START_HOUR + slot + 1) + ":00";
                    schedule[week][day][slot].type = SessionType::Consultation; // Default to consultation
                }
            }
        }
    }
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

void displaySchedule(const Expert &expert, int week) {
    if (week < 0 || week >= 4) {
        cout << "Invalid week number. Please select a week from 1 to 4." << endl;
        return;
    }
    const int dayWidth = 19;  // Width for each day (3 slots + separators)
    const int slotWidth = 12;

    const string days[5] = {"Mon", "Tue", "Wed", "Thu", "Fri"};
    int startDate = 1 + (week * 7);
    int padding = (dayWidth - 1 - days[0].length()) / 2;

    cout << "Schedule for " << expert.name << " (" << expert.service.name << "):\n";
    cout << "|" << setw(dayWidth) << left << "Time";
    for (int i=0; i<5; ++i) {
        cout << "|" << setw(padding) << "" << days[i] << setw(dayWidth - 1 - padding - days[i].length()) << "";
    }
    cout << "|" << endl;

    cout << "|" << setw(dayWidth) << left << "Date"; 
    for (int i=0; i<5; ++i) {
        int date = startDate + i;
        string dateStr = to_string(date);
        cout << "|" << setw(padding) << "" << dateStr << setw(dayWidth - 1 - padding - dateStr.length()) << "";
    }
    cout << "|" << endl;

    cout << string(dayWidth * 6 + 1, '-') << endl;

    for (int i = 0; i < MAX_SLOTS_PER_DAY; i++) {
        cout << "|" << setw(dayWidth) << left << expert.schedule[week][0][i].timeRange;
        for (int day = 0; day < DAYS_IN_WEEK; day++) {
            if (expert.schedule[week][day][i].isBooked) {
                if (expert.schedule[week][day][i].type == SessionType::Treatment) {
                    padding = (dayWidth - string("T-Booked").length()) / 2;
                    cout << "|" << string(padding, ' ') << "T-Booked" << string(padding, ' ');
                } else {
                    padding = (dayWidth - string("C-Booked").length()) / 2;
                    cout << "|" << string(padding, ' ') << "C-Booked" << string(padding, ' ');
                }
            } else if (expert.hoursWorkedPerDay[week][day] >= MAX_WORK_HOURS) {
                cout << "|" << setw(padding) << "Unavailable" << setw(dayWidth - 1 - padding - 11) << "";
            } else {
                padding = (dayWidth - string("Open").length()) / 2;
                cout << "|" << string(padding, ' ') << "Open" << string(padding, ' ');
                // cout << "|" << setw(padding) << "Open" << setw(dayWidth - 1 - padding) << "";f
            }
        }
        cout << "|\n";
    }
}

bool canBookSlot(const Expert &expert, int week, int day, int slot, SessionType sessionType) {
    int duration = (sessionType == SessionType::Treatment) ? TREATMENT_SLOT_DURATION : CONSULTATION_SLOT_DURATION;
    if (expert.hoursWorkedPerDay[week][day] + duration > MAX_WORK_HOURS) {
        return false;
    }
    for (int i = 0; i < duration; ++i) {
        if (slot + i >= MAX_SLOTS_PER_DAY || expert.schedule[week][day][slot + i].isBooked) {
            return false;
        }
    }
    return true;
}

void displayWeekChoices(const string &month, const string weekStartDates[4]) {
    cout << "Choose a week for the month of " << month << ":\n";
    for (int i=0; i<4; ++i) {
        struct tm date = {};
        strptime(weekStartDates[i].c_str(), "%Y-%m-%d", &date);
        date.tm_mday += 4;
        mktime(&date);
        string endDate = to_string(1900 + date.tm_year) +
                        "-" + to_string(1 + date.tm_mon) +
                        "-" + to_string(date.tm_mday);
        cout << "Week " << i+1 << ": " << weekStartDates[i] << " - " << endDate << "\n";
    }
}

int chooseWeek(const string &month, const string weekStartDates[4]) {
    int choice;
    displayWeekChoices(month, weekStartDates);
    cout << "Enter week number (1-4): ";
    cin >> choice;

    if (choice < 1 || choice > 4) {
        cout << "Invalid choice. Please select a valid week.\n";
        return chooseWeek(month, weekStartDates);
    }
    return --choice;
}

pair<int, int> selectTimeSlot(const Expert &expert,int chosenWeek, SessionType sessionType) {
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
        canBookSlot(expert, chosenWeek ,selectedDay, selectedSlot, sessionType)) {
        return {selectedDay, selectedSlot};
    }

    cout << "Invalid selection, slot(s) already booked, or exceeds daily work limit." << endl;
    return {-1, -1};
}

void displayWeeklyScheduleWithRealDates() {
    time_t t = time(0);
    struct tm *now = localtime(&t);

    // Adjust `tm_wday` to get the start of the week (Monday)
    int daysFromMonday = now->tm_wday - 1;
    if (daysFromMonday < 0) {
        daysFromMonday += 7; // Adjust for Sunday
    }
    now->tm_mday -= daysFromMonday;

    cout << "Weekly Schedule:\n";
    int weekdayCount = 0;

    while (weekdayCount < DAYS_IN_WEEK) {
        struct tm date = *now;
        mktime(&date); // Normalize the date structure

        // Skip Saturday (tm_wday == 6) and Sunday (tm_wday == 0)
        if (date.tm_wday == 6 || date.tm_wday == 0) {
            now->tm_mday += 1; // Skip to the next day
            continue;
        }

        // Display the date and increment to the next day
        cout << "Day " << weekdayCount + 1 << " (" << 1900 + date.tm_year << "-" 
             << 1 + date.tm_mon << "-" << date.tm_mday << "):\n";
        
        weekdayCount++;
        now->tm_mday += 1; // Move to the next day
    }
}

void saveScheduleCsv(const Expert &expert) {
    string filename = expert.service.name + "_schedule.csv";
    ofstream outFile(filename);

    if (!outFile) {
        cerr << "Error opening file for writing." << endl;
        return;
    }

    // Write header
    outFile << "Day,HoursWorked,8:00-9:00,Type,9:00-10:00,Type,10:00-11:00,Type,11:00-12:00,Type,12:00-1:00,Type,1:00-2:00,Type,2:00-3:00,Type,3:00-4:00,Type\n";

    // Write each day's schedule
    for (int week = 0; week < 4; ++week) {
        for (int day = 0; day < DAYS_IN_WEEK; ++day) {
            outFile << "Day " << day + 1 << ',' << expert.hoursWorkedPerDay[week][day];
            for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                outFile << ',' << expert.schedule[week][day][slot].isBooked;
                outFile << ',' << (expert.schedule[week][day][slot].type == SessionType::Treatment ? "T" : 
                                (expert.schedule[week][day][slot].type == SessionType::Consultation ? "C" : "U"));
                }
                outFile << endl;
            }
        }
    outFile.close();
}

void saveScheduleToFile(const Expert &expert) {
    string filename = expert.service.name + "_schedule.txt";
    ofstream outFile(filename);

    if (outFile.is_open()) {
        for (int week = 0; week < 4; ++week) {
            outFile << "Week " << week + 1 << endl;
            for (int day = 0; day < DAYS_IN_WEEK; ++day) {
                outFile << "Day " << day + 1 << endl; 
                outFile << expert.hoursWorkedPerDay[week][day];

                for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                    outFile << ' ' << expert.schedule[week][day][slot].isBooked;
                    outFile << ' ' << (expert.schedule[week][day][slot].type == SessionType::Treatment ? "T" : 
                                    (expert.schedule[week][day][slot].type == SessionType::Consultation ? "C" : "U")) << ' ';
                }
                outFile << endl;
            }
    }
    outFile.close();
    } else {
        cerr << "Error opening file for writing: " << filename << endl;
    }
}

void loadScheduleFromFile(Expert &expert) {
    string filename = expert.service.name + "_schedule.txt";
    ifstream inFile(filename);

    if (inFile.is_open()) {
        string line;
        int week = -1;
        int day = -1;

        while (getline(inFile, line)) {
            if (line.find("Week") == 0) {
                week = stoi(line.substr(5)) -1;
                continue;
            }
            if (line.find("Day") == 0) {
                day = stoi(line.substr(4)) -1;
                continue;
            }
            if (week >= 0 && day >= 0) {
                stringstream ss(line);
                int hoursWorked;
                if (!(ss >> hoursWorked)) {
                    cerr << "Error parsing hoursWorked: " << line << endl;
                    continue;
                }
                expert.hoursWorkedPerDay[week][day] = hoursWorked;
                
                for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                    char isBooked, typeChar;
                    if (!(ss >> isBooked >> typeChar)) {
                        cerr << "Error reading slot data for Week " << week + 1 << " Day " << day + 1 << " from line: " << line << endl;
                        break;
                    }
                    expert.schedule[week][day][slot].isBooked = (isBooked == '1');
                    if (typeChar == 'T') expert.schedule[week][day][slot].type = SessionType::Treatment;
                    else if (typeChar == 'C') expert.schedule[week][day][slot].type = SessionType::Consultation;
                    else expert.schedule[week][day][slot].type = SessionType::Unavailable;
                }
            }
        }
        // for (int day = 0; day < DAYS_IN_WEEK; ++day) {
        //     inFile >> expert.hoursWorkedPerDay[day];
        //     for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
        //         char typeChar;
        //         inFile >> expert.schedule[day][slot].isBooked >> typeChar;
        //         if (typeChar == 'T') expert.schedule[day][slot].type = SessionType::Treatment;
        //         else if (typeChar == 'C') expert.schedule[day][slot].type = SessionType::Consultation;
        //         else expert.schedule[day][slot].type = SessionType::Unavailable;
        //     }
        // }
        inFile.close();
    } else {
        cout << "No existing schedule found for " << expert.name << ". Starting with a clean schedule." << endl;
    }
}

void loadScheduleFromCsv(Expert &expert) {
    string filename = expert.service.name + "_schedule.csv";
    ifstream inFile(filename);

    if (inFile.is_open()) {
        string line;
        getline(inFile, line); // Skip header line

        int week = 0;
        int dayCounter = 0;

        while (getline(inFile, line)) {
            if (line.empty()) {
                continue;
            }

            stringstream ss(line);
            string temp;
            int day, hoursWorked;

            ss >> temp >> day >> hoursWorked;

            if (ss.fail() || temp != "Day" || day < 1 || day > 5) {
                cerr << "Error parsing day or hoursWorked: " << line << endl;
                continue;
            }
            --day;

            // If day is 0 and not the first line, increment the week
            // if (day == 0 && week < 4) {
            //     week++;
            // }

            // ss >> hoursWorked;
            expert.hoursWorkedPerDay[week][day] = hoursWorked;
            cout << "Week " << week << " Day " << day << " Hours worked: " << hoursWorked << endl;
            
            for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
                char isBooked, typeChar;

                ss >> isBooked >> typeChar;

                if (!(ss >> isBooked >> typeChar)) {
                    cerr << "Error reading slot data for Week " << week + 1 << " Day " << day + 1 << " Slot " << slot << endl;
                    break;
                }

                expert.schedule[week][day][slot].isBooked = (isBooked == '1');
                // if (typeChar == 'T') expert.schedule[week][day][slot].type = SessionType::Treatment;
                // else if (typeChar == 'C') expert.schedule[week][day][slot].type = SessionType::Consultation;
                // else expert.schedule[week][day][slot].type = SessionType::Unavailable;
                expert.schedule[week][day][slot].type = (typeChar == 'T') ? SessionType::Treatment : 
                                                         ((typeChar == 'C') ? SessionType::Consultation : SessionType::Unavailable);
                cout << "Slot: " << slot << ", isBooked: " << expert.schedule[week][day][slot].isBooked << ", typeChar: " << typeChar << endl;

            }
            dayCounter++;
            if (dayCounter % 5 == 0 && dayCounter != 0) {
                week++;
            } 
        }
        inFile.close();
    } else {
        cout << "No existing schedule found for " << expert.service.name << ". Creating a new schedule." << endl;
        // Optionally, initialize the schedule with default values or leave it empty
    }

    // for (int week = 0; week < 4; ++week) {
    //     for (int day = 0; day < DAYS_IN_WEEK; ++day) {
    //         for (int slot = 0; slot < MAX_SLOTS_PER_DAY; ++slot) {
    //             cout << "Week: " << week + 1 << ", Day: " << day + 1 << ", Slot: " << slot + 1 
    //                 << " - isBooked: " << expert.schedule[week][day][slot].isBooked 
    //                 << ", Type: " << static_cast<int>(expert.schedule[week][day][slot].type) << endl;
    //         }
    //     }
    // }
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
    ifstream file(filename);

    if (file.is_open()) {
        file >> counter;
        file.close();
    }
    return counter;
}

void saveBookingCounter(const string& filename, int counter) {
    ofstream file(filename);
    if (file.is_open()) {
        file << counter;
        file.close();
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
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }
    file << "Receipt\n";
    file << "-----------------------------\n";
    file << "Booking Number: " << receipt.bookingNumber << endl;
    file << "Customer Name: " << receipt.customerName << endl;
    file << "Session: " << (receipt.sessionType == SessionType::Treatment ? "Treatment" : "Consultation") << endl;
    file << "Service: " << receipt.serviceName << endl;
    file << "Time Slot: " << receipt.timeSlot << endl;
    file << "Payment Method: " << receipt.paymentMethodToString() << "\n";
    file << "Amount Paid: RM " << fixed << setprecision(2) << receipt.amountPaid << endl;
    file << "-----------------------------\n";

    file.close();
}

void printReceipt (const string &filename) {
    #ifdef _WIN32
    ShellExecute(NULL, "open", filename.c_str(), NULL, NULL, SW_SHOWNORMAL); 
    #else   
    string command = "open " + filename;
    system(command.c_str());
    #endif
}


void makeBooking(Expert &expert, SessionType sessionType) {
    loadScheduleFromFile(expert);
    string weekStartDates[4] = {"2024-09-02", "2024-09-09", "2024-09-16", "2024-09-23"};    
    int chosenWeek = chooseWeek("September", weekStartDates);
    displaySchedule(expert, chosenWeek);
    double price = sessionType == SessionType::Treatment ? expert.service.price : 60.0;

    auto [day, slot] = selectTimeSlot(expert, chosenWeek, sessionType);

    if (day != -1 && slot != -1) {
        int duration = (sessionType == SessionType::Treatment) ? TREATMENT_SLOT_DURATION : CONSULTATION_SLOT_DURATION;
        for (int i = 0; i < duration; ++i) {
            expert.schedule[chosenWeek][day][slot + i].isBooked = true;
            expert.schedule[chosenWeek][day][slot + i].type = sessionType;
        }
        expert.hoursWorkedPerDay[chosenWeek][day] += duration;

        string startTime = to_string(START_HOUR + slot) + ":00";
        string endTime = to_string(START_HOUR + slot + duration) + ":00";
        system("clear");
        cout << "Please confirm your booking details: \n";
        cout << "Service: " << expert.service.name << endl;
        cout << "Session Type: " << (sessionType == SessionType::Treatment ? "Treatment" : "Consultation") << endl;
        cout << "Date: I HAVEN'T DONE THIS YET" << endl;
        cout << "Time Slot: " << startTime << " - " << endTime << endl;
        cout << "Price: RM " << fixed << setprecision(2) << price << endl;
        cout << "Confirm booking? (Y/N): ";
        char confirm;
        cin >> confirm;

        if (tolower(confirm) == 'y') {
            PaymentMethod PaymentMethod = selectPaymentMethod();
            string bookingNumber = generateBookingNumber();
            Receipt receipt = {bookingNumber, "steve", sessionType, expert.service.name, startTime + " - " + endTime, PaymentMethod, price};
            receipt.generateReceipt();

            cout << "You have successfully booked the slot on Day " << day + 1
                << " from " << expert.schedule[chosenWeek][day][slot].timeRange.substr(0, 5) << " to " << endTime
                << " with " << expert.name << " for " << expert.service.name << " (" 
                << (sessionType == SessionType::Treatment ? "Treatment" : "Consultation") << ")." << endl;

            string receiptFileName = "receipt_" + bookingNumber + ".txt";
            generateReceiptFile(receipt, receiptFileName);
            printReceipt(receiptFileName);

            saveScheduleToFile(expert); // Save updated schedule to file
            saveScheduleCsv(expert);
        } else {
            cout << "Booking cancelled." << endl;
        }
    }
}

int main() {
    Service facial("Facial", 150.00);
    Service botox("Botox", 250.00);
    Service manicure("Manicure", 100.00);
    
    Expert aliceFacial("Alice", facial);
    Expert bobBotox("Bob", botox);
    Expert carolManicure("Carol", manicure);

    Expert experts[] = {aliceFacial, bobBotox, carolManicure};

    cout << "Available services:\n";
    cout << "1. Facial\n";
    cout << "2. Botox\n";
    cout << "3. Manicure\n";
    cout << "Select a service (1-3): ";

    int choice;
    cin >> choice;

    string selectedService;
    switch (choice) {
        case 1: selectedService = "Facial"; break;
        case 2: selectedService = "Botox"; break;
        case 3: selectedService = "Manicure"; break;
        default: 
            cout << "Invalid choice. Exiting."; 
            return 1;
    }

    Expert &selectedExpert = experts[choice - 1];

    cout << "Select session type:\n";
    cout << "1. Treatment (2 hours)\n";
    cout << "2. Consultation (1 hour)\n";
    cout << "Enter your choice (1-2): ";
    
    int sessionChoice;
    cin >> sessionChoice;

    SessionType sessionType;
    switch (sessionChoice) {
        case 1: sessionType = SessionType::Treatment; break;
        case 2: sessionType = SessionType::Consultation; break;
        default:
            cout << "Invalid choice. Exiting.";
            return 1;
    }

    makeBooking(selectedExpert, sessionType);

    return 0;
}