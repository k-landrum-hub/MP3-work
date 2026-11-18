/*
* MP3 gas across a trip
* Korbin Landrum
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cctype>

using namespace std;

// Constants
const int MAX_FILLUPS = 20;
const int DAYS_IN_YEAR_SIZE = 366; 
const double LITERS_TO_GALLONS = 0.2641720526;

// Function Prototypes
double toGallons(double amount, char unit);
double fillUpCost(double amount, double pricePerUnit);
double milesPerGallon(double miles, double gallons);
double costPerMile(double cost, double miles);
double sumOf(double values[], int count);
int indexOfSmallest(double values[], int count);
int indexOfLargest(double values[], int count);
int dayOfYear(int month, int day);
int loadRates(string fileName, double rates[], int size);
void fillMissingRates(double rates[], int size);

int main() {
    // Load and process 2026 exchange rates
    double rates[DAYS_IN_YEAR_SIZE];
    int ratesCount = loadRates("rates2026.csv", rates, DAYS_IN_YEAR_SIZE);

    if (ratesCount == -1) {
        cout << "Error: Could not open exchange rates file 'rates2026.csv'. Terminating program.\n";
        return 1;
    }
    fillMissingRates(rates, DAYS_IN_YEAR_SIZE);

    // Parallel arrays for raw log input
    int fillupMonth[MAX_FILLUPS];
    int fillupDay[MAX_FILLUPS];
    double odometer[MAX_FILLUPS + 1]; // Index 0 holds starting odometer
    double amountPumped[MAX_FILLUPS];
    char unit[MAX_FILLUPS];
    double pricePerUnit[MAX_FILLUPS];
    double totalSale[MAX_FILLUPS];

    // Computed leg arrays
    double legMiles[MAX_FILLUPS];
    double legGallons[MAX_FILLUPS];
    double legMPG[MAX_FILLUPS];
    double legCost[MAX_FILLUPS]; // Stored in USD
    double legCostPerMile[MAX_FILLUPS];

    int numFillups = 0;
    double budget = 0.0;

    // Get Starting Odometer Reading
    cout << "Enter starting odometer reading: ";
    cin >> odometer[0];

    // Get Number of Fill-ups and check
    cout << "Enter number of fill-ups: ";
    cin >> numFillups;
    while (numFillups < 1 || numFillups > MAX_FILLUPS) {
        cout << "Error: Number of fill-ups must be between 1 and " << MAX_FILLUPS << ".\n";
        cout << "Enter number of fill-ups: ";
        cin >> numFillups;
    }

    // Input each fill-up with updated prompt and validation
    for (int i = 0; i < numFillups; i++) {
        bool valid = false;
        while (!valid) {
            cout << "Fill-up " << (i + 1)
                << " (month, day, odometer, amount, G or L, price per unit, total sale): ";

            cin >> fillupMonth[i]
                >> fillupDay[i]
                >> odometer[i + 1]
                >> amountPumped[i]
                >> unit[i]
                >> pricePerUnit[i]
                >> totalSale[i];

            unit[i] = toupper(unit[i]);
            int doy = dayOfYear(fillupMonth[i], fillupDay[i]);

            // Validation checks
            if (doy == -1) {
                cout << "Error: " << fillupMonth[i] << "/" << fillupDay[i]
                    << " is not a valid 2026 calendar date. Please re-enter this fill-up.\n";
            }
            else if (unit[i] != 'G' && unit[i] != 'L') {
                cout << "Error: Unit must be 'G' (gallons) or 'L' (liters). Please re-enter this fill-up.\n";
            }
            else if (unit[i] == 'L' && rates[doy] < 0) {
                cout << "Error: No exchange rate available for Canadian fill-up on "
                    << fillupMonth[i] << "/" << fillupDay[i] << ". Please re-enter this fill-up.\n";
            }
            else if (odometer[i + 1] <= odometer[i]) {
                cout << "Error: Current odometer reading (" << odometer[i + 1]
                    << ") must be greater than previous reading (" << odometer[i]
                    << "). Please re-enter this fill-up.\n";
            }
            else if (amountPumped[i] <= 0 || pricePerUnit[i] <= 0 || totalSale[i] <= 0) {
                cout << "Error: Amount, price, and total sale must be positive numbers. Please re-enter.\n";
            }
            else {
                valid = true;
            }
        }
    }

    // Get Fuel Budget
    cout << "Enter fuel budget for the trip: ";
    cin >> budget;
    cout << "\n";

    // Compute Leg Metrics (Converting Canadian 'L' fill-ups to USD)
    for (int i = 0; i < numFillups; i++) {
        legMiles[i] = odometer[i + 1] - odometer[i];
        legGallons[i] = toGallons(amountPumped[i], unit[i]);
        legMPG[i] = milesPerGallon(legMiles[i], legGallons[i]);

        if (unit[i] == 'L') {
            int doy = dayOfYear(fillupMonth[i], fillupDay[i]);
            legCost[i] = totalSale[i] * rates[doy];
        }
        else {
            legCost[i] = totalSale[i];
        }

        legCostPerMile[i] = costPerMile(legCost[i], legMiles[i]);
    }

    // Compute Whole Trip Totals
    double totalMiles = sumOf(legMiles, numFillups);
    double totalGallons = sumOf(legGallons, numFillups);
    double totalCost = sumOf(legCost, numFillups);

    double overallMPG = milesPerGallon(totalMiles, totalGallons);
    double overallCostPerMile = costPerMile(totalCost, totalMiles);

    int cheapestIdx = indexOfSmallest(legCostPerMile, numFillups);
    int expensiveIdx = indexOfLargest(legCostPerMile, numFillups);

    // 8. Print Report using printf
    printf("%-5s %10s %10s %8s %10s %12s\n", "Leg", "Miles", "Gallons", "MPG", "Cost", "Cost/mile");
    for (int i = 0; i < numFillups; i++) {
        printf("%-5d %10.1f %10.2f %8.1f $%9.2f $%11.3f\n",
            (i + 1), legMiles[i], legGallons[i], legMPG[i], legCost[i], legCostPerMile[i]);
    }

    printf("\n");
    printf("Total miles: %.1f\n", totalMiles);
    printf("Total gallons: %.2f\n", totalGallons);
    printf("Overall MPG: %.1f\n", overallMPG);
    printf("Total fuel cost: $%.2f\n", totalCost);
    printf("Overall cost per mile: $%.3f\n", overallCostPerMile);
    printf("Cheapest leg per mile: Leg %d\n", cheapestIdx + 1);
    printf("Most expensive leg per mile: Leg %d\n", expensiveIdx + 1);

    // Budget Comparison
    double diff = budget - totalCost;
    if (diff > 0.0001) {
        printf("Under budget by $%.2f\n", diff);
    }
    else if (diff < -0.0001) {
        printf("Over budget by $%.2f\n", -diff);
    }
    else {
        printf("Exactly on budget!\n");
    }

    return 0;
}

int dayOfYear(int month, int day) {
    // Days in each month for 2026 (non-leap year)
    int daysInMonth[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (month < 1 || month > 12) {
        return -1;
    }
    if (day < 1 || day > daysInMonth[month]) {
        return -1;
    }

    int doy = 0;
    for (int m = 1; m < month; m++) {
        doy += daysInMonth[m];
    }
    doy += day;

    return doy;
}

int loadRates(string fileName, double rates[], int size) {
    // Initialize all elements to -1 (no rate)
    for (int i = 0; i < size; i++) {
        rates[i] = -1.0;
    }

    ifstream inFile(fileName);
    if (!inFile.is_open()) {
        return -1;
    }

    string line;
    // Skip header line
    if (!getline(inFile, line)) {
        inFile.close();
        return 0;
    }

    int count = 0;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string monthStr, dayStr, rateStr;

        if (getline(ss, monthStr, ',') &&
            getline(ss, dayStr, ',') &&
            getline(ss, rateStr, ',')) {

            int month = stoi(monthStr);
            int day = stoi(dayStr);
            double rate = stod(rateStr);

            int doy = dayOfYear(month, day);
            if (doy >= 1 && doy < size) {
                rates[doy] = rate;
                count++;
            }
        }
    }

    inFile.close();
    return count;
}

void fillMissingRates(double rates[], int size) {
    int firstValid = -1;
    int lastValid = -1;

    // Find the first day with a valid rate
    for (int i = 1; i < size; i++) {
        if (rates[i] >= 0) {
            firstValid = i;
            break;
        }
    }

    // Find the last day with a valid rate
    for (int i = size - 1; i >= 1; i--) {
        if (rates[i] >= 0) {
            lastValid = i;
            break;
        }
    }

    // If there are valid rates, forward-fill gaps strictly between firstValid and lastValid
    if (firstValid != -1 && lastValid != -1) {
        for (int i = firstValid + 1; i < lastValid; i++) {
            if (rates[i] < 0) {
                rates[i] = rates[i - 1];
            }
        }
    }
}

double toGallons(double amount, char unit) {
    if (toupper(unit) == 'L') {
        return amount * LITERS_TO_GALLONS;
    }
    return amount;
}

double fillUpCost(double amount, double pricePerUnit) {
    return amount * pricePerUnit;
}

double milesPerGallon(double miles, double gallons) {
    if (gallons == 0) return 0.0;
    return miles / gallons;
}

double costPerMile(double cost, double miles) {
    if (miles == 0) return 0.0;
    return cost / miles;
}

double sumOf(double values[], int count) {
    double total = 0.0;
    for (int i = 0; i < count; i++) {
        total += values[i];
    }
    return total;
}

int indexOfSmallest(double values[], int count) {
    if (count <= 0) return -1;
    int minIndex = 0;
    for (int i = 1; i < count; i++) {
        if (values[i] < values[minIndex]) {
            minIndex = i;
        }
    }
    return minIndex;
}

int indexOfLargest(double values[], int count) {
    if (count <= 0) return -1;
    int maxIndex = 0;
    for (int i = 1; i < count; i++) {
        if (values[i] > values[maxIndex]) {
            maxIndex = i;
        }
    }
    return maxIndex;
}