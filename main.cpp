#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>


using namespace std;

string generateUniqueID() {
    string uniqueID;
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(0, 9);

    // Generate random numbers
    for (int i = 0; i < 5; ++i) {
        uniqueID += to_string(distribution(generator));
    }

    // Get current timestamp
    time_t currentTime = time(nullptr);
    struct tm* localTime = localtime(&currentTime);
    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day = localTime->tm_mday;

    // Append timestamp
    uniqueID += to_string(year % 100);
    if (month < 10) {
        uniqueID += "0";
    }
    uniqueID += to_string(month);
    if (day < 10) {
        uniqueID += "0";
    }
    uniqueID += to_string(day);

    // Truncate or pad to 6 digits
    uniqueID = uniqueID.substr(0, 6);

    return uniqueID;
}


// Product class
class Product {
public:
    string name;
    int quantity;
    double price;

    Product(string name, int quantity, double price) {
        this->name = name;
        this->quantity = quantity;
        this->price = price;
    }
};

class Cashier;

class Sale {
public:
    string user;
    string product;
    string id;
    double price;
    int quantity;
    bool isCompleted;

    Sale(string user, string product, int quantity, double price) {
        this->user = user;
        this->product = product;
        this->quantity = quantity;
        this->isCompleted = 0;
        this->id = generateUniqueID();
        this->price = price;
    }
};


class Shop {
private:
    vector<string> activityLog;

public:
    float totalBalance;
    vector<Cashier> cashiers;
    vector<Product> inventory;
    vector<Sale> sales;

    // Add a cashier
    void addCashier(Cashier& cashier);

    void appendSale(Sale& sale) {
        this->sales.push_back(sale);
        logActivity(sale.user + " has purchased " + to_string(sale.quantity) + " " + sale.product + "(s)");
        cout << endl << sale.id << " is your unique transaction reference. Copy it and use it to track the progress of your transaction\n";
    }

    int getNumberofPendingSales() {
        int count = 0;
        for (auto& sale: sales) {
            if (!sale.isCompleted) count ++;
        }
        return count;
    }

    // Login a cashier
    Cashier* loginCashier();

    void showPendingSales() {
        if (!this->getNumberofPendingSales()) {
            cout << "There are no pending transactions\n";
            return;
        }
        cout << endl;
        cout << "Pending Sales to be processed: \n\n";
        cout << left << setw(15) << "User" << setw(15) << "ID" << setw(15) << "Product" << setw(15) << "Price"<< setw(15) << "Quantity" << endl;
        cout << "--------------------------------------------------------------------\n";
        for (auto& sale: sales) {
            if(!sale.isCompleted)
                cout << left << setw(15) << sale.user << setw(15) << sale.id << setw(15) << sale.product << setw(15) << sale.price << setw(15) << sale.quantity << endl;
        }
    }

    // Logout a cashier
    void logoutCashier(const string& id);

    void trackTransaction() {
        string id;
        cout << "Transaction ID: ";
        cin >> id;
        for(auto& sale: this->sales) {
            if (sale.id == id) {
                string status = sale.isCompleted ? "True" : "False";
                cout << endl << sale.user << endl;
                cout << sale.quantity << " " << sale.product << "(s) " << endl;
                cout << "Transaction Completed: " << status << endl;
                return;
            }
        }
        cout << "Transaction not found!\n";
    }


    // Add an item to the inventory
    void addItem(const Product& item) {
        inventory.push_back(item);
        string log = "Added " + to_string(item.quantity) + " " + item.name + "(s) to the inventory.";
        logActivity(log);
    }

    // Display items
    void displayItems() {
        int counter = 0;
        cout << left << setw(5) << "S/N" << setw(15) << "Product Name" << setw(10) << "Price"<< setw(10) << "In Stock" << endl;
        cout << left << setw(15) << "-------------------------------------\n";
        for (auto& product: inventory) {
            cout << setw(5) << ++counter << setw(15) << product.name << setw(10) << product.price << setw(10) << product.quantity << endl << endl;
        }
    }

    // Log activity
    void logActivity(const string& message) {
        time_t now = time(0);
        string timestamp = ctime(&now);
        string log = timestamp.substr(0, timestamp.length() - 1) + " - " + message;
        activityLog.push_back(log);
        ofstream activityLogFile("activity_log.txt", ios::app); // Open the file for writing
        if (!activityLogFile) { // Check if the file was opened successfully
            cerr << "Failed to open the file for writing." << endl;
            return;
        }
        activityLogFile << log << endl;  // Write data to the file
        activityLogFile.close(); // Close the file
    }

    // Display activity log
    void displayActivityLog() {
        cout << "Activity Log:" << endl;
        for (const auto& log : activityLog) {
            cout << log << endl;
        }
    }
};


// Cashier class
class Cashier {
public:
    string name;
    string id;
    bool loggedIn;

    Cashier(string name) {
        this->name = name;
        if (!(name == ""))
            this->id = generateUniqueID();
        else
            this->id = "";
        this->loggedIn = false;
    }

    Cashier(string name, string id) {
        this->name = name;
        this->id = id;
        this->loggedIn = false;
    }

    // Process sale
    void processSale(Shop& shop);


    void restock(Shop& shop) {
        if (!this->loggedIn) {
            cout << "You are not logged in\n";
            return;
        }
        cout << "Name of product: ";
        string name;
        cin >> name;
        cout << "Quantity to restock: ";
        int quantity;
        cin >> quantity;
        for (auto& product : shop.inventory) {
            if (product.name == name) {
                double totalPrice = product.price * quantity;
                if (totalPrice >= shop.totalBalance) {
                    cout << "You can't do that. Insufficient funds!\n";
                    return;
                }
                product.quantity += quantity;
                shop.totalBalance -= totalPrice;
                string log = this->name + " restocked " + to_string(quantity) + " " + name + "(s) for a total of $" + to_string(totalPrice) + ".";
                cout << log << endl;
                shop.logActivity(log);
                return;
            }
        }
    }

    // Set price for an item
    void setPrice(Shop& shop, const string& name, double price) {
        for (auto& product : shop.inventory) {
            if (product.name == name) {
                product.price = price;
                shop.logActivity("Price of " + name + " set to $" + to_string(price) + ".");
                return;
            }
        }
        cout << "Item not found in the inventory." << endl;
    }
};


int main() {
    Shop shop;
    Cashier* currentCashier;

    // Populate the store inventory
    ifstream inputFile("inventory.txt"); // Open file for reading
    if (inputFile.is_open()) {
        string line;
        int i = 1;
        float totalBalance;
        while (getline(inputFile, line)) {
            istringstream iss(line); // Create an input string stream
            if (i++ == 1)  {  // To ensure that the first value is read as the total balance of the shop.
                if (iss >> totalBalance) {
                    shop.totalBalance = totalBalance;
                    continue;
                }
            }
            string name;
            float price;
            int stock;
            // push the items to the inventory
            if (iss >> name >> price >> stock) {
                // Values successfully extracted and stored in variables
                Product product(name, stock, price);
                shop.addItem(product);
            } else {
                cout << "Failed to extract values from the line." << endl;
                return 1;
            }
        }
        cout << endl;
        inputFile.close(); // Close the file
    } else
        cout << "Unable to open the file." << endl;


    // Set up cashier database
    ifstream cashierFile("cashier_db.txt"); // Open file for reading
    if (cashierFile.is_open()) {
        string line;
        while (getline(cashierFile, line)) {
            istringstream iss(line); // Create an input string stream
            string name;
            string id;

            if (iss >> name >> id) {
                // Values successfully extracted and stored in variables
                Cashier cashier(name, id);
                shop.addCashier(cashier);
            } else {
                cout << "Failed to extract values from the line." << endl;
                return 1;
            }


        }
        cashierFile.close(); // Close the file
    } else {
        cout << "Unable to open the file." << endl;
    }
    char role;

    do {
        cout << "Welcome to The Shop" << endl;
        cout << "\nWhat are you?\n" << "1. Customer\n2. Cashier\n3. Register as a cashier\n4. Exit\n\n>> ";
        cin >> role;

        if (role == '2') {
            currentCashier = shop.loginCashier();
            if (!currentCashier) {
                cout << "\033[2J\033[1;1H";
                continue;
            }

            cout << endl;
            int option;
            do {
                cout << "\nWhat do you want to do?\n1. Restock Inventory\n2. Process a sale\n3. View Inventory\n4. View Pending Transactions\n5. Logout\n\n>> ";
                cin >> option;
                switch (option) {
                case 1:
                    currentCashier->restock(shop);
                    break;
                case 2:
                    currentCashier->processSale(shop);
                    break;
                case 3:
                    cout << "\nTotal Balance: $" << shop.totalBalance << endl<<endl;
                    shop.displayItems();
                    break;
                case 4:
                    shop.showPendingSales();
                    break;
                case 5:
                    currentCashier->loggedIn = 0;
                    cout << currentCashier->name << " has logged out.\n";
                    shop.logActivity(currentCashier->name + " has logged out\n.");
                    break;
                }
            } while (option != 5);
        }

        else if (role == '1') {
            shop.displayItems();
            string choice;
            string userName;
            bool found = false;

            do {
                cout << "\n[t] - View and track transactions\n[e] - Exit\nWhat would you like to buy? (Enter S/N from table or type name): ";
                cin >> choice;
                if (choice == "e") break;
                if (choice == "t") {
                    shop.trackTransaction();
                    continue;
                };
                Product *productToBuy;
                try {
                    int sn = stoi(choice);
                    int counter = 0;
                    for (auto& product: shop.inventory) {
                        if (++counter == sn) {
                            productToBuy = &product;
                            found = true;
                        }
                    }
                } catch (const exception& e) {
                    for (auto& product: shop.inventory) {
                        if (product.name == choice) {
                            productToBuy = &product;
                            found = true;
                        }
                    }
                }

                if (!found) {
                    cout << "Product not found!\n";
                    continue;
                }

                cout << "Quantity of " << productToBuy->name << "(s) to buy: ";
                int quantity;
                cin >> quantity;
                if (productToBuy->quantity >= quantity) {
                    cout << "What is your name (for sale processing): ";
                    cin >> userName;
                    Sale sale(userName, productToBuy->name, quantity, productToBuy->price);
                    shop.appendSale(sale);
                } else
                    cout << "Insufficient amount in stock!" << endl;
            } while (choice != "e");
        }

        else if (role == '3') {
            string name;
            cout << "What's your name?: ";
            cin >> name;
            Cashier cashier(name);
            shop.addCashier(cashier);

            // write them to the db file
            ofstream cashierFile("cashier_db.txt", ios::app);
            if (!cashierFile) {
                cerr << "Failed to open cashier file" << endl;
                return 1;
            }
            cout << "You have registered successfully. " << cashier.id << " is your unique ID. Use it to login as a cashier\n";
            cashierFile << cashier.name << " " << cashier.id << endl;
            continue;
        }
        cout << "\033[2J\033[1;1H";
    } while (role != '4');

    // Update inventory to file
    ofstream inventoryFile("inventory.txt"); // Open the file for writing
    if (!inventoryFile) { // Check if the file was opened successfully
        cerr << "Failed to open the file for writing." << endl;
        return 1;
    }
    inventoryFile << shop.totalBalance << endl;
    for (auto& product: shop.inventory)
        inventoryFile << product.name << " " << product.price << " " << product.quantity << endl;
    inventoryFile.close(); // Close the file


    return 0;
}


// Process sale
void Cashier::processSale(Shop& shop) {
    if (!this->loggedIn) {
        cout << "You are not logged in\n";
        return;
    }
    shop.showPendingSales();
    if (!shop.getNumberofPendingSales()) return;
    string id;
    cout << "\nTransaction Reference (from table above): ";
    cin >> id;
    for (auto& sale : shop.sales) {
        if (sale.id == id) {
            double totalPrice = sale.price * sale.quantity;
            for(auto& product: shop.inventory) {
                if(sale.product == product.name) {
                    product.quantity -= sale.quantity;
                    sale.isCompleted = 1;
                    shop.totalBalance += totalPrice;
                    string log = this->name + " sold " + to_string(sale.quantity) + " " + product.name + "(s) for a total of $" + to_string(totalPrice) + ".";
                    cout << log << endl;
                    shop.logActivity(log);
                    return;
                }
            }
        }
    }
    cout << "\nTransaction with the given ID not found. Check the table well.\n";
}

// Login a cashier
Cashier* Shop::loginCashier() {
    string id;
    for(;;) {
        cout << "\n[b] - Back\n";
        cout << "Enter your id: ";
        cin >> id;
        if (id == "b") return nullptr;

        for (auto& cashier : cashiers) {
            if (cashier.id == id) {
                if (!cashier.loggedIn) {
                    cashier.loggedIn = true;
                    cout << cashier.name << endl;
                    logActivity(cashier.name + " logged in.");
                    cout << "Login Successful" << endl;
                    return &cashier;
                } else {
                    cout << "Cashier is already logged in." << endl;
                    return &cashier;
                }
            }
        }
        cout << "Wrong! Try Again\n";
    }
    cout << "\nCashier not found." << endl;
}


// Logout a cashier
void Shop::logoutCashier(const string& id) {
    for (auto& cashier : cashiers) {
        if (cashier.id == id) {
            if (cashier.loggedIn) {
                cashier.loggedIn = false;
                logActivity(cashier.name + " logged out.");
                return;
            } else {
                cout << "Cashier is already logged out." << endl;
                return;
            }
        }
    }
    cout << "Cashier not found." << endl;
}

void Shop::addCashier(Cashier& cashier) {
    cashiers.push_back(cashier);
    string log = cashier.name + " has joined the shop";
    logActivity(log);
}

