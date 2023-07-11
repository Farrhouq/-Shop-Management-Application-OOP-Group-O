#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

// Worker class
class Worker {
public:
    string name;
    bool loggedIn;

    Worker(string name) {
        this->name = name;
        loggedIn = false;
    }
};

// Inventory class
class Inventory {
public:
    string item;
    int quantity;
    double price;

    Inventory(string item, int quantity, double price) {
        this->item = item;
        this->quantity = quantity;
        this->price = price;
    }
};

// Shop class
class Shop {
private:
    vector<Worker> workers;
    vector<Inventory> inventory;
    vector<string> activityLog;

public:
    // Add a worker
    void addWorker(const Worker& worker) {
        workers.push_back(worker);
        logActivity(worker.name + " joined the shop.");
    }

    // Login a worker
    void loginWorker(const string& name) {
        for (auto& worker : workers) {
            if (worker.name == name) {
                if (!worker.loggedIn) {
                    worker.loggedIn = true;
                    logActivity(name + " logged in.");
                    return;
                } else {
                    cout << "Worker is already logged in." << endl;
                    return;
                }
            }
        }

        cout << "Worker not found." << endl;
    }

    // Logout a worker
    void logoutWorker(const string& name) {
        for (auto& worker : workers) {
            if (worker.name == name) {
                if (worker.loggedIn) {
                    worker.loggedIn = false;
                    logActivity(name + " logged out.");
                    return;
                } else {
                    cout << "Worker is already logged out." << endl;
                    return;
                }
            }
        }

        cout << "Worker not found." << endl;
    }

    // Add an item to the inventory
    void addItem(const Inventory& item) {
        inventory.push_back(item);
        logActivity("Added " + to_string(item.quantity) + " " + item.item + "(s) to the inventory.");
    }

    // Process sale
    void processSale(const string& workerName, const string& item, int quantity) {
        for (auto& inv : inventory) {
            if (inv.item == item) {
                if (inv.quantity >= quantity) {
                    double totalPrice = inv.price * quantity;
                    inv.quantity -= quantity;
                    logActivity(workerName + " sold " + to_string(quantity) + " " + item + "(s) for a total of $" + to_string(totalPrice) + ".");
                    return;
                } else {
                    cout << "Insufficient quantity in the inventory." << endl;
                    return;
                }
            }
        }

        cout << "Item not found in the inventory." << endl;
    }

    // Set price for an item
    void setPrice(const string& item, double price) {
        for (auto& inv : inventory) {
            if (inv.item == item) {
                inv.price = price;
                logActivity("Price of " + item + " set to $" + to_string(price) + ".");
                return;
            }
        }

        cout << "Item not found in the inventory." << endl;
    }

    // Log activity
    void logActivity(const string& message) {
        time_t now = time(0);
        string timestamp = ctime(&now);
        activityLog.push_back(timestamp.substr(0, timestamp.length() - 1) + " - " + message);
    }

    // Display activity log
    void displayActivityLog() {
        cout << "Activity Log:" << endl;
        for (const auto& log : activityLog) {
            cout << log << endl;
        }
    }
};

int main() {
    Shop shop;

    // Add workers
    Worker worker1("John");
    Worker worker2("Alice");
    shop.addWorker(worker1);
    shop.addWorker(worker2);

    // Login workers
    shop.loginWorker("John");
    shop.loginWorker("Alice");

    // Add inventory items
    Inventory item1("Chair", 10, 25.0);
    Inventory item2("Table", 5, 50.0);
    shop.addItem(item1);
    shop.addItem(item2);

    // Set prices for items
    shop.setPrice("Chair", 30.0);
    shop.setPrice("Table", 60.0);

    // Process sales
    shop.processSale("John", "Chair", 3);
    shop.processSale("Alice", "Table", 2);
    shop.processSale("John", "Chair", 5);

    // Logout workers
    shop.logoutWorker("John");
    shop.logoutWorker("Alice");

    // Display activity log
    shop.displayActivityLog();

    return 0;
}
