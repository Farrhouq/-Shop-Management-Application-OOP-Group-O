#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

string generateUniqueID()
{
    std::string uniqueID;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 9);

    // Generate random numbers
    for (int i = 0; i < 5; ++i)
    {
        uniqueID += std::to_string(distribution(generator));
    }

    // Get current timestamp
    time_t currentTime = std::time(nullptr);
    struct tm *localTime = std::localtime(&currentTime);
    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day = localTime->tm_mday;

    // Append timestamp
    uniqueID += std::to_string(year % 100);
    if (month < 10)
    {
        uniqueID += "0";
    }
    uniqueID += std::to_string(month);
    if (day < 10)
    {
        uniqueID += "0";
    }
    uniqueID += std::to_string(day);

    // Truncate or pad to 6 digits
    uniqueID = uniqueID.substr(0, 6);

    return uniqueID;
}

// Product class
class Product
{
public:
    string name;
    int quantity;
    double price;

    Product(string name, int quantity, double price)
    {
        this->name = name;
        this->quantity = quantity;
        this->price = price;
    }
};
class Cashier;

class Shop
{
private:
    vector<string> activityLog;

public:
    float totalBalance;
    vector<Cashier> cashiers;
    vector<Product> inventory;
    // Add a cashier
    void addCashier(const Cashier &cashier)
    {
        cashiers.push_back(cashier);
        // logActivity(cashier.name + " joined the shop.");
    }

    // Login a cashier
    Cashier loginCashier();

    // Logout a cashier
    void logoutCashier(const string &id);

    // Add an item to the inventory
    void addItem(const Product &item)
    {
        inventory.push_back(item);
        logActivity("Added " + to_string(item.quantity) + " " + item.name + "(s) to the inventory.");
    }

    // Display items
    void displayItems()
    {
        int counter = 0;
        cout << endl
             << setw(20) << "\t\t\t___Inventory___\n\n";
        cout << left << setw(5) << "S/N" << setw(15) << "Product Name" << setw(10) << "Price" << setw(10) << "In Stock" << endl;
        cout << left << setw(15) << "-------------------------------------\n";
        for (auto &product : inventory)
        {
            cout << setw(5) << ++counter << setw(15) << product.name << setw(10) << product.price << setw(10) << product.quantity << endl
                 << endl;
        }
    }

    // Log activity
    void logActivity(const string &message)
    {
        cout << message << endl;
        time_t now = time(0);
        string timestamp = ctime(&now);
        activityLog.push_back(timestamp.substr(0, timestamp.length() - 1) + " - " + message);
    }

    // Display activity log
    void displayActivityLog()
    {
        cout << "Activity Log:" << endl;
        for (const auto &log : activityLog)
        {
            cout << log << endl;
        }
    }
};

// Cashier class
class Cashier
{
public:
    string name;
    string id;
    bool loggedIn;

    Cashier(string name)
    {
        this->name = name;
        if (!(name == ""))
            this->id = generateUniqueID();
        else
            this->id = "";
        this->loggedIn = false;
    }

    Cashier(string name, string id)
    {
        this->name = name;
        this->id = id;
        this->loggedIn = false;
    }

    // Process sale
    void processSale(Shop &shop);

    void restock(Shop &shop)
    {
        if (!this->loggedIn)
        {
            cout << "You are not logged in\n";
            return;
        }
        cout << "Name of product: ";
        string name;
        cin >> name;
        cout << "Quantity to restock: ";
        int quantity;
        cin >> quantity;
        for (auto &product : shop.inventory)
        {
            if (product.name == name)
            {
                double totalPrice = product.price * quantity;
                product.quantity += quantity;
                shop.totalBalance -= totalPrice;
                shop.logActivity(this->name + " restocked " + to_string(quantity) + " " + name + "(s) for a total of $" + to_string(totalPrice) + ".");
                return;
            }
        }
    }

    // Set price for an item
    void setPrice(Shop &shop, const string &name, double price)
    {
        for (auto &product : shop.inventory)
        {
            if (product.name == name)
            {
                product.price = price;
                shop.logActivity("Price of " + name + " set to $" + to_string(price) + ".");
                return;
            }
        }
        cout << "Item not found in the inventory." << endl;
    }
};

int main()
{
    Shop shop;
    Cashier currentCashier("");

    // Populate the store inventory
    ifstream inputFile("inventory.txt"); // Open file for reading
    if (inputFile.is_open())
    {
        string line;
        while (getline(inputFile, line))
        {
            istringstream iss(line); // Create an input string stream
            string name;
            float price;
            int stock;
            // let's push the items to the inventory

            if (iss >> name >> price >> stock)
            {
                // Values successfully extracted and stored in variables
                Product product(name, stock, price);
                shop.addItem(product);
            }
            else
            {
                cout << "Failed to extract values from the line." << endl;
                return 1;
            }
        }
        cout << endl;
        inputFile.close(); // Close the file
    }
    else
    {
        cout << "Unable to open the file." << endl;
    }

    // Set up cashier database
    ifstream cashierFile("cashier_db.txt"); // Open file for reading
    if (cashierFile.is_open())
    {
        string line;
        while (getline(cashierFile, line))
        {
            istringstream iss(line); // Create an input string stream
            string name;
            string id;

            if (iss >> name >> id)
            {
                // Values successfully extracted and stored in variables
                cout << "Name: " << name << "\tID: " << id << endl;
            }
            else
            {
                cout << "Failed to extract values from the line." << endl;
                return 1;
            }

            Cashier cashier(name, id);
            shop.addCashier(cashier);
        }
        cashierFile.close(); // Close the file
    }
    else
    {
        cout << "Unable to open the file." << endl;
    }

    cout << "Welcome to The Shop" << endl
         << endl;
    char role;

    do
    {
        cout << "What are you?\n"
             << "1. Worker\n2. Customer\n3.Exit" << endl;
        cin >> role;

        if (role == '1')
        {
            currentCashier = shop.loginCashier();
            cout << "ID: " << currentCashier.id << endl;
            int option;
            do
            {
                cout << "\nWhat do you want to do?\n1. Restock Inventory\n2. Process a sale\n3. Exit\n";
                cin >> option;
                switch (option)
                {
                case 1:
                    currentCashier.restock(shop);
                    break;
                case 2:
                    currentCashier.processSale(shop);
                    break;
                }
            } while (option != 3);
        }
        if (role == '2')
        {
            shop.displayItems();
            string choice;

            do
            {
                cout << "\n[e] - Exit\nWhat would you like to buy? (Enter S/N from table or type name): ";
                cin >> choice;
                if (choice == "e")
                    break;
                Product *productToBuy;
                try
                {
                    int sn = stoi(choice);
                    int counter = 0;
                    for (auto &product : shop.inventory)
                    {
                        if (++counter == sn)
                            productToBuy = &product;
                    }
                }
                catch (const std::exception &e)
                {
                    for (auto &product : shop.inventory)
                    {
                        if (product.name == choice)
                            productToBuy = &product;
                    }
                }

                cout << "Quantity of " << productToBuy->name << "(s) to buy: ";
                int quantity;
                cin >> quantity;
                if (productToBuy->quantity >= quantity)
                    cout << "Successfully purchased " << productToBuy->name << " for $" << productToBuy->price * quantity << endl;
                else
                    cout << "Insufficient amount in stock!" << endl;
            } while (choice != "e");
        }
    } while (role != '3');

    // Login cashiers
    //    shop.loginCashier();
    //    shop.loginCashier();
    //
    //    // Add inventory items
    //    Product item1("Chair", 10, 25.0);
    //    Product item2("Table", 5, 50.0);
    //    shop.addItem(item1);
    //    shop.addItem(item2);
    //
    //    // Set prices for items
    //    shop.setPrice("Chair", 30.0);
    //    shop.setPrice("Table", 60.0);
    //
    //    // Process sales
    //    shop.processSale("John", "Chair", 3);
    //    shop.processSale("Alice", "Table", 2);
    //    shop.processSale("John", "Chair", 5);
    //
    //    // Logout cashiers
    //    shop.logoutCashier("John");
    //    shop.logoutCashier("Alice");
    //
    //    // Display activity log
    //    shop.displayActivityLog();

    return 0;
}

// Process sale
void Cashier::processSale(Shop &shop)
{
    if (!this->loggedIn)
    {
        cout << "You are not logged in\n";
        return;
    }
    string productName;
    int quantity;
    cout << "Name of product to be sold: ";
    cin >> productName;
    cout << "Quantity sold: ";
    cin >> quantity;
    for (auto &product : shop.inventory)
    {
        if (product.name == productName)
        {
            if (product.quantity >= quantity)
            {
                double totalPrice = product.price * quantity;
                product.quantity -= quantity;
                shop.totalBalance += totalPrice;
                shop.logActivity(this->name + " sold " + to_string(quantity) + " " + name + "(s) for a total of $" + to_string(totalPrice) + ".");
                return;
            }
            else
            {
                cout << "Insufficient quantity in the inventory." << endl;
                return;
            }
        }
    }
    cout << "Item not found in the inventory." << endl;
}

// Login a cashier
Cashier Shop::loginCashier()
{
    string id;

    for (;;)
    {
        cout << "Enter your id: ";
        cin >> id;
        for (auto &cashier : cashiers)
        {
            if (cashier.id == id)
            {
                if (!cashier.loggedIn)
                {
                    cashier.loggedIn = true;
                    cout << cashier.name << endl;
                    logActivity(cashier.name + " logged in.");
                    cout << "Login Successful" << endl;
                    return cashier;
                }
                else
                {
                    cout << "Cashier is already logged in." << endl;
                    return cashier;
                }
            }
        }
        cout << "Wrong! Try Again\n";
    }
    cout << "\nCashier not found." << endl;
}

// Logout a cashier
void Shop::logoutCashier(const string &id)
{
    for (auto &cashier : cashiers)
    {
        if (cashier.id == id)
        {
            if (cashier.loggedIn)
            {
                cashier.loggedIn = false;
                logActivity(cashier.name + " logged out.");
                return;
            }
            else
            {
                cout << "Cashier is already logged out." << endl;
                return;
            }
        }
    }
    cout << "Cashier not found." << endl;
}
