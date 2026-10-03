#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace std;

class Product;

struct CartItem {
    Product* product;
    int quantity;
};

// Global Vectors
vector<Product*> products;
vector<CartItem> cart;
vector<string> orderHistory;

// Input Clear Utility Tool to prevent infinite menu loops
void clearInputBuffer() {
    if (cin.fail()) {
        cin.clear();
        string dummy;
        getline(cin, dummy);
    }
}

// Safe ignore helper that only clears when a newline is pending
void safeClearNewline() {
    if (cin.peek() == '\n') {
        cin.ignore();
    }
}

//=================================================
// SECTION 1: CORE OOP CLASSES
//=================================================

class User {
protected:
    string username;
    string password;
public:
    User(string u = "", string p = "") : username(u), password(p) {}
    virtual ~User() = default;

    string getUsername() const { return username; }
    string getPassword() const { return password; }
};

class Admin : public User {
public:
    Admin(string u = "", string p = "") : User(u, p) {}
};

class Customer : public User {
public:
    Customer(string u = "", string p = "") : User(u, p) {}
};

// --- BASE PRODUCT CLASS ---
class Product {
protected:
    int id;
    string name;
    double price;
    int stock;
public:
    Product(int i = 0, string n = "", double p = 0, int s = 0)
        : id(i), name(n), price(p), stock(s) {}

    virtual ~Product() = default;

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    void reduceStock(int qty) { if (stock >= qty) stock -= qty; }
    void increaseStock(int qty) { stock += qty; }
    virtual int getType() const = 0; 

    virtual double calculateTax() = 0;
    virtual double calculateShipping() = 0;

    virtual void display() const {
        cout << left << setw(6) << id 
             << setw(20) << name 
             << setw(10) << fixed << setprecision(2) << price 
             << setw(8) << stock;
    }
};

class PhysicalProduct : public Product {
public:
    PhysicalProduct(int i, string n, double p, int s) : Product(i, n, p, s) {}
    int getType() const override { return 1; }
    double calculateTax() override { return price * 0.10; }
    double calculateShipping() override { return 200.0; }
    void display() const override {
        Product::display();
        cout << setw(12) << "Physical" << endl;
    }
};

class DigitalProduct : public Product {
public:
    DigitalProduct(int i, string n, double p, int s) : Product(i, n, p, s) {}
    int getType() const override { return 2; }
    double calculateTax() override { return price * 0.05; }
    double calculateShipping() override { return 0.0; }
    void display() const override {
        Product::display();
        cout << setw(12) << "Digital" << endl;
    }
};

//=================================================
// FUNCTION DECLARATIONS & SYSTEM TOOLS
//=================================================
void loginSystem();
void adminMenu();
void customerMenu();

void viewProducts();
void addProduct();
void restockProduct();      
void searchProduct();       
void lowStockAlert();
void adminProductReport();

void addToCart();
void viewCart();
void removeFromCart();
void checkout();
void viewOrderHistory();

void saveData();            
void loadData();            
void clearMemory();

Admin adminUser("admin", "1234");
Customer customerUser("user", "1234");

//=================================================
// DATA PERSISTENCE (FILE I/O)
//=================================================
void saveData() {
    ofstream prodFile("inventory.txt");
    if (prodFile.is_open()) {
        for (const auto& p : products) {
            prodFile << p->getType() << "," << p->getId() << "," << p->getName() << "," 
                     << p->getPrice() << "," << p->getStock() << "\n";
        }
        prodFile.close();
    }

    ofstream orderFile("orders.txt");
    if (orderFile.is_open()) {
        for (const auto& order : orderHistory) {
            orderFile << order << "\n";
        }
        orderFile.close();
    }
}

void loadData() {
    // Clear current data structure state to handle hot updates safely
    clearMemory();

    ifstream prodFile("inventory.txt");
    if (prodFile.is_open()) {
        string line;
        while (getline(prodFile, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string sType, sId, name, sPrice, sStock;

            if (getline(ss, sType, ',') &&
                getline(ss, sId, ',') &&
                getline(ss, name, ',') &&
                getline(ss, sPrice, ',') &&
                getline(ss, sStock, ',')) {
                
                int type = stoi(sType);
                int id = stoi(sId);
                double price = stod(sPrice);
                int stock = stoi(sStock);

                if (type == 1) products.push_back(new PhysicalProduct(id, name, price, stock));
                else if (type == 2) products.push_back(new DigitalProduct(id, name, price, stock));
            }
        }
        prodFile.close();
    }

    ifstream orderFile("orders.txt");
    if (orderFile.is_open()) {
        string line;
        while (getline(orderFile, line)) {
            if (!line.empty()) orderHistory.push_back(line);
        }
        orderFile.close();
    }
}

//=================================================
// PRODUCT ENGINE EXTRA FEATURES
//=================================================
void searchProduct() {
    cout << "\n========== SEARCH PRODUCTS ==========\n";
    cout << "1. Search by Name\n2. Filter by Type\nEnter Choice: ";
    int choice; cin >> choice;
    if(cin.fail()) { clearInputBuffer(); cout << "\nInvalid Input Type.\n"; return; }

    if (choice == 1) {
        string query;
        cout << "Enter product name to look up: ";
        safeClearNewline();
        getline(cin, query);
        
        transform(query.begin(), query.end(), query.begin(), ::tolower);

        cout << "\n" << left << setw(6) << "ID" << setw(20) << "Name" << setw(10) << "Price" << setw(8) << "Stock" << setw(12) << "Type" << endl;
        cout << "--------------------------------------------------------\n";
        
        bool found = false;
        for (const auto& p : products) {
            string pName = p->getName();
            transform(pName.begin(), pName.end(), pName.begin(), ::tolower);
            if (pName.find(query) != string::npos) {
                p->display();
                found = true;
            }
        }
        if (!found) cout << "No matching products found in inventory.\n";

    } else if (choice == 2) {
        cout << "1. Physical Only\n2. Digital Only\nEnter Type: ";
        int type; cin >> type;
        if(cin.fail()) { clearInputBuffer(); return; }

        cout << "\n" << left << setw(6) << "ID" << setw(20) << "Name" << setw(10) << "Price" << setw(8) << "Stock" << setw(12) << "Type" << endl;
        cout << "--------------------------------------------------------\n";
        for (const auto& p : products) {
            if (p->getType() == type) p->display();
        }
    }
}

void restockProduct() {
    cout << "\n========== RESTOCK INVENTORY ==========\n";
    int id, qty;
    cout << "Enter Product ID to Restock: "; cin >> id;
    if(cin.fail()) { clearInputBuffer(); return; }
    
    for (auto& p : products) {
        if (p->getId() == id) {
            cout << "Current Stock: " << p->getStock() << "\nEnter Quantity to Add: ";
            cin >> qty;
            if(cin.fail()) { clearInputBuffer(); return; }

            if (qty > 0) {
                p->increaseStock(qty);
                cout << "\nStock Updated Successfully!\n";
                saveData();
            } else {
                cout << "\nInvalid quantity amount.\n";
            }
            return;
        }
    }
    cout << "\nProduct ID not found.\n";
}

void addProduct() {
    int id, stock, type; string name; double price;
    cout << "\n========== ADD PRODUCT ==========\n";
    cout << "Enter Product ID: "; cin >> id;
    if(cin.fail()) { clearInputBuffer(); cout << "\nInvalid ID Input.\n"; return; }
    
    for (const auto& p : products) {
        if (p->getId() == id) {
            cout << "\nError: Product ID already exists!\n";
            return;
        }
    }

    cout << "Enter Product Name: "; 
    safeClearNewline(); // FIXED: Prevents dropping the first letter of name string input
    getline(cin, name);

    cout << "Enter Price: "; cin >> price;
    cout << "Enter Stock: "; cin >> stock;
    cout << "Type (1-Physical, 2-Digital): "; cin >> type;
    if(cin.fail()) { clearInputBuffer(); cout << "\nBad numerical type parameter mapping.\n"; return; }

    if (type == 1) products.push_back(new PhysicalProduct(id, name, price, stock));
    else if (type == 2) products.push_back(new DigitalProduct(id, name, price, stock));
    else { cout << "\nInvalid Type Selected.\n"; return; }

    cout << "\nProduct Dynamic Entry Registered!\n";
    saveData();
}

void viewProducts() {
    cout << "\n==================== SYSTEM CATALOG ====================\n";
    if (products.empty()) { cout << "No inventory records found.\n"; return; }

    cout << left << setw(6) << "ID" << setw(20) << "Name" << setw(10) << "Price" << setw(8) << "Stock" << setw(12) << "Type" << endl;
    cout << "--------------------------------------------------------\n";
    for (const auto& p : products) {
        p->display();
    }
}

//=================================================
// STRUCT-BASED CLEAN CART OPERATIONS
//=================================================
void addToCart() {
    int id, qty;
    cout << "\n========== CUSTOMER ADD TO CART ==========\n";
    cout << "Enter Product ID: "; cin >> id;
    cout << "Enter Quantity: "; cin >> qty;
    if(cin.fail()) { clearInputBuffer(); return; }

    if (qty <= 0) { cout << "\nInvalid quantity!\n"; return; }

    for (auto& p : products) {
        if (p->getId() == id) {
            if (p->getStock() >= qty) {
                p->reduceStock(qty);
                
                for (auto& item : cart) {
                    if (item.product->getId() == id) {
                        item.quantity += qty;
                        cout << "\nCart updated with extra quantity!\n";
                        return;
                    }
                }
                cart.push_back({p, qty});
                cout << "\nProduct successfully secured in cart!\n";
            } else {
                cout << "\nInsufficient inventory overhead!\n";
            }
            return;
        }
    }
    cout << "\nProduct selection mapping failure!\n";
}

void viewCart() {
    cout << "\n========== CURRENT SHOPPING CART ==========\n";
    if (cart.empty()) { cout << "Your cart is running empty.\n"; return; }

    double total = 0;
    cout << left << setw(20) << "Name" << setw(10) << "Price" << setw(10) << "Qty" << setw(12) << "Subtotal" << endl;
    cout << "--------------------------------------------------------\n";
    for (const auto& item : cart) {
        double base = item.product->getPrice() * item.quantity;
        double addedCosts = (item.product->calculateTax() + item.product->calculateShipping()) * item.quantity;
        double subtotal = base + addedCosts;
        
        cout << left << setw(20) << item.product->getName()
             << setw(10) << item.product->getPrice()
             << setw(10) << item.quantity
             << setw(12) << subtotal << endl;
        total += subtotal;
    }
    cout << "--------------------------------------------------------\n";
    cout << "TOTAL PAYABLE DUE: $" << total << endl;
}

void removeFromCart() {
    int id;
    cout << "\nEnter Product ID to drop from cart: "; cin >> id;
    if(cin.fail()) { clearInputBuffer(); return; }

    for (size_t i = 0; i < cart.size(); i++) {
        if (cart[i].product->getId() == id) {
            cart[i].product->increaseStock(cart[i].quantity); 
            cart.erase(cart.begin() + i);
            cout << "\nItem removed. Stock inventory balanced out.\n";
            return;
        }
    }
    cout << "\nTarget item not found in cart allocation.\n";
}

void checkout() {
    if (cart.empty()) { cout << "\nCart empty. Action aborted.\n"; return; }

    viewCart();
    int confirm;
    cout << "\nProcess Transaction and Invoice? (1-Yes / 0-No): "; cin >> confirm;
    if(cin.fail()) { clearInputBuffer(); return; }

    if (confirm == 1) {
        string order = "ORDER -> Items: ";
        double total = 0;
        for (const auto& item : cart) {
            order += item.product->getName() + " (x" + to_string(item.quantity) + "), ";
            total += (item.product->getPrice() + item.product->calculateTax() + item.product->calculateShipping()) * item.quantity;
        }
        order += " | Total Paid: $" + to_string((int)total);
        orderHistory.push_back(order);
        
        cart.clear();
        saveData(); 
        cout << "\nInvoice Settled! Thank you for purchasing.\n";
    } else {
        for (const auto& item : cart) {
            item.product->increaseStock(item.quantity);
        }
        cart.clear();
        cout << "\nTransaction Cancelled. Holds released back to inventory.\n";
    }
}

void viewOrderHistory() {
    cout << "\n========== TRANSACTION MEMORY LOGS ==========\n";
    if (orderHistory.empty()) { cout << "No records available.\n"; return; }
    for (size_t i = 0; i < orderHistory.size(); i++) {
        cout << i + 1 << ". " << orderHistory[i] << endl;
    }
}

//=================================================
// METRICS & INTERFACE CONTROL
//=================================================
void lowStockAlert() {
    cout << "\n========== LIVE INVENTORY ALERTS ==========\n";
    bool clean = true;
    for (const auto& p : products) {
        if (p->getStock() <= 5) {
            cout << "?? ALERT: [ID: " << p->getId() << "] " << p->getName() << " drops to " << p->getStock() << " units!\n";
            clean = false;
        }
    }
    if (clean) cout << "All storage units running on optimal parameters.\n";
}

void adminProductReport() {
    cout << "\n========== SYSTEM METRICS MATRIX ==========\n";
    int totalStock = 0; double totalValue = 0;
    for (const auto& p : products) {
        totalStock += p->getStock();
        totalValue += p->getPrice() * p->getStock();
    }
    cout << "Unique SKU Records   : " << products.size() << endl;
    cout << "Cumulative Stock Units : " << totalStock << endl;
    cout << "Gross Liquid Value     : $" << totalValue << endl;
}

void clearMemory() {
    for (auto& p : products) delete p;
    products.clear();
    cart.clear();
}

//=================================================
// ENTRY MENUS LOGIC ENGINE
//=================================================
void loginSystem() {
    int role; string u, p;
    cout << "\n========== IMS GATEWAY PRO ==========\n1. Admin Terminal\n2. Customer Terminal\nEnter choice: ";
    cin >> role;
    if(cin.fail()) { clearInputBuffer(); cout << "\nInvalid selection input.\n"; return; }
    
    cout << "Username: "; cin >> u;
    cout << "Password: "; cin >> p;

    if (role == 1 && u == adminUser.getUsername() && p == adminUser.getPassword()) adminMenu();
    else if (role == 2 && u == customerUser.getUsername() && p == customerUser.getPassword()) customerMenu();
    else cout << "\nAccess Denied: Invalid Credentials.\n";
}

void adminMenu() {
    int choice;
    do {
        cout << "\n========== ADMIN DASHBOARD ==========\n"
             << "1. View Catalog\n2. Add New SKU\n3. Restock Line Item\n4. Search/Filter\n5. Low Stock Auditing\n6. System Report Metrics\n7. Terminate Session\nChoice: ";
        cin >> choice;
        if(cin.fail()) { clearInputBuffer(); continue; }
        
        switch (choice) {
            case 1: viewProducts(); break;
            case 2: addProduct(); break;
            case 3: restockProduct(); break;
            case 4: searchProduct(); break;
            case 5: lowStockAlert(); break;
            case 6: adminProductReport(); break;
        }
    } while (choice != 7);
}

void customerMenu() {
    int choice;
    do {
        cout << "\n========== CUSTOMER HUB ==========\n"
             << "1. Browse Store Catalog\n2. Search Items\n3. Cart Controls Interface\n4. Settle Checkout Invoice\n5. Review Purchases History\n6. Terminate Session\nChoice: ";
        cin >> choice;
        if(cin.fail()) { clearInputBuffer(); continue; }
        
        switch (choice) {
            case 1: viewProducts(); break;
            case 2: searchProduct(); break;
            case 3: {
                cout << "\n1. View Cart\n2. Add To Cart\n3. Remove Allocation\nChoice: ";
                int c; cin >> c;
                if(cin.fail()) { clearInputBuffer(); break; }
                if (c == 1) viewCart();
                else if (c == 2) addToCart();
                else if (c == 3) removeFromCart();
                break;
            }
            case 4: checkout(); break;
            case 5: viewOrderHistory(); break;
        }
    } while (choice != 6);
}

int main() {
    loadData(); 
    int choice;
    do {
        cout << "\n=========================================\n"
             << "   ENTERPRISE INVENTORY CONTROL (C++)\n"
             << "=========================================\n"
             << "1. Identity Authentication Gateway\n2. Safe Shutdown Routine\nChoice: ";
        cin >> choice;
        if(cin.fail()) { clearInputBuffer(); continue; }
        
        if (choice == 1) loginSystem();
    } while (choice != 2);

    saveData(); 
    clearMemory();
    cout << "\nSystem Shutdown Sequence Completed cleanly.\n";
    return 0;
}