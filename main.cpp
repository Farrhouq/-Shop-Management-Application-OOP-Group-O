#include <iostream>
#include <vector>
#include <ctime>
#include <random>

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

    // Log activity
    void logActivity(const string &message)
    {
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

    // Process sale
    void processSale(Shop &shop, const string &productName, int quantity);

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
    Cashier cashier1("John");
    Cashier cashier2("Alice");
    Cashier currentCashier("");
    // Add cashiers
    shop.addCashier(cashier1);
    shop.addCashier(cashier2);
    cout << cashier1.id << endl;
    cout << cashier2.id << endl;

    cout << "Welcome to The Shop" << endl;
    char role;
    cout << "1. Worker\n2. Customer" << endl;
    cin >> role;

    if (role == '1')
    {
        currentCashier = shop.loginCashier();
        cout << "ID: " << currentCashier.id << endl;
        cout << "What do you want to do?\n1. Update Inventory\n2. Process a sale\n3. Exit\n";
        char option;
        cin >> option;
        // ...
        return 0;
    }

    if (role == '2')
    {
        cout << "What goods would you like to buy?...// then we show them the goods, they buy, and we move";
    };

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
void Cashier::processSale(Shop &shop, const string &productName, int quantity)
{
    if (!this->loggedIn)
    {
        cout << "You are not logged in\n";
        return;
    }
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
