void generateSalesReport() {
    Receipt allReceipts[200];
    int receiptCount = loadBookings(allReceipts);
    string services[] = { "Facial", "Botox and Fillers", "Manicure" };
    string experts[] = { "Alice", "Bob", "Carol" };
    double facialRevenue = 0, botoxRevenue = 0, manicureRevenue = 0;
    double aliceRevenue = 0, bobRevenue = 0, carolRevenue = 0;
    double serviceRevenue[3] = { 0 };
    double expertRevenue[3] = { 0 };

    if (receiptCount == 0) {
        cout << RED << "No bookings found. Unable to generate sales report." << RESET << endl;
        return;
    }

    double totalRevenue = 0;
    int totalBookings = 0;

    cout << "\n+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "|                                                  Detailed Sales Report                                                      |" << endl;
    cout << "+-----------------------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| Booking #  | Date          | Time Slot         | Service Name        | Expert  | Customer Email          | Amount Paid (RM) |" << endl;
    cout << "+------------+---------------+-------------------+---------------------+---------+-------------------------+------------------+" << endl;

    for (int i = 0; i < receiptCount; ++i) {
        // Calculate total revenue and bookings
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

        // Display each receipt's details, including customer email
        cout << "| " << setw(10) << left << allReceipts[i].bookingNumber
            << " | " << setw(13) << left << allReceipts[i].date + " July 2024"
            << " | " << setw(17) << left << allReceipts[i].timeSlot
            << " | " << setw(19) << left << allReceipts[i].serviceName
            << " | " << setw(7) << left << allReceipts[i].expert.name
            << " | " << setw(23) << left << allReceipts[i].customer.email
            << " | RM " << setw(13) << right << fixed << setprecision(2) << allReceipts[i].amountPaid << " |" << endl;
    }
    cout << "+------------+---------------+-------------------+---------------------+---------+-------------------------+------------------+" << endl;

    double allSales[6] = { facialRevenue, botoxRevenue, manicureRevenue, aliceRevenue, bobRevenue, carolRevenue };
    const char* serviceNames[6] = { "Facial", "Botox", "Manicure", "Alice", "Bob", "Carol"};

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
