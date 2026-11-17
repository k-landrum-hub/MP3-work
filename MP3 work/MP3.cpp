/*
* MP3 gas across a trip
* Korbin Landrum
*/

#include <iostream>
#include <cstdio>
#include <cctype>

using namespace std;

// Maximum allowed fill-ups 
const int MAX_FILLUPS = 20;

// Conversion constant
const double LITERS_TO_GALLONS = 0.2641720526;

// Function Prototypes
double toGallons(double amount, char unit);
double fillUpCost(double amount, double pricePerUnit);
double milesPerGallon(double miles, double gallons);
double costPerMile(double cost, double miles);
double sumOf(double values[], int count);
int indexOfSmallest(double values[], int count);
int indexOfLargest(double values[], int count);

int main() {
    // Array declarations 
    double odometer[MAX_FILLUPS + 1];
    double amountPumped[MAX_FILLUPS];
    char unit[MAX_FILLUPS];
    double pricePerUnit[MAX_FILLUPS];
    double totalSale[MAX_FILLUPS];

    // Computed leg arrays
    double legMiles[MAX_FILLUPS];
    double legGallons[MAX_FILLUPS];
    double legMPG[MAX_FILLUPS];
    double legCost[MAX_FILLUPS];
    double legCostPerMile[MAX_FILLUPS];

    int numFillups = 0;
    double budget = 0.0;

    // Get Starting Odometer Reading
    cout << "Enter starting odometer reading: ";
    cin >> odometer[0];

    // Get Number of Fill-ups with validation (1 to 20)
    cout << "Enter number of fill-ups: ";
    cin >> numFillups;
    while (numFillups < 1 || numFillups > MAX_FILLUPS) {
        cout << "Error: Number of fill-ups must be between 1 and " << MAX_FILLUPS << ".\n";
        cout << "Enter number of fill-ups: ";
        cin >> numFillups;
    }

    // Input each fill-up with validation
    for (int i = 0; i < numFillups; i++) {
        bool valid = false;
        while (!valid) {
            cout << "Fill-up " << (i + 1)
                << " (odometer, amount, G or L, price, total): ";

            cin >> odometer[i + 1]
                >> amountPumped[i]
                >> unit[i]
                >> pricePerUnit[i]
                >> totalSale[i];

            // Normalize unit character to uppercase
            unit[i] = toupper(unit[i]);

            // Validate inputs
            if (unit[i] != 'G' && unit[i] != 'L') {
                cout << "Error: Unit must be 'G' (gallons) or 'L' (liters). Please re-enter this fill-up.\n";
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

    // Compute Leg Metrics
    for (int i = 0; i < numFillups; i++) {
        legMiles[i] = odometer[i + 1] - odometer[i];
        legGallons[i] = toGallons(amountPumped[i], unit[i]);
        legMPG[i] = milesPerGallon(legMiles[i], legGallons[i]);

        // Using pump total sale for leg cost (see ASSUMPTIONS.txt)
        legCost[i] = totalSale[i];
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

    // Print Report using printf
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

// Functions

double toGallons(double amount, char unit) {
    if (toupper(unit) == 'L') {
        return amount * LITERS_TO_GALLONS;
    }
    return amount; // If 'G'
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