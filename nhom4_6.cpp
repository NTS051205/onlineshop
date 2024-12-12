#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

class Product {
private:
    int id;
    string name;
    double price;
    int stock;
    vector<pair<string,int>> reviews;

public:
    Product(int id, const string& name, double price, int stock)
        : id(id), name(name), price(price), stock(stock) {}

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    void setStock(int newStock) { stock = newStock; }
    void addReview(const string &username,int rating){
        reviews.push_back({username,rating});}
    
    const vector<pair<string,int>> &getReviews() const {
        return reviews;
    }
    void displayReviews() const {
        if (reviews.empty()) {
            cout << "No reviews yet.\n";
        } else {
            cout << "\n--- Reviews for " << name << " ---\n";
            for (const auto& review : reviews) {
                cout << "User: " << review.first << " | Rating: " << review.second << "\n";
            }
        }
    }
};
class Promotion {
private:
    int promotionId;
    string description;
    double discountPercentage;
    vector<int> applicableProductIds;

public:
    Promotion(int id, const string& desc, double discount, const vector<int>& productIds)
        : promotionId(id), description(desc), discountPercentage(discount), applicableProductIds(productIds) {}

    double getDiscount() const { return discountPercentage; }
    bool isApplicable(int productId) const {
        return find(applicableProductIds.begin(), applicableProductIds.end(), productId) != applicableProductIds.end();
    }

    string getDescription() const { return description; }
};
class Customer {
private:
    string username;
    string password;
    string email;

public:
    Customer(const string& username, const string& password, const string& email)
        : username(username), password(password), email(email) {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getEmail() const { return email; }
};

class Cart {
private:
    vector<pair<Product, int>> items; // Pair of Product and quantity

public:
    void addProduct(const Product& product, int quantity) {
        for (auto& item : items) {
            if (item.first.getId() == product.getId()) {
                item.second += quantity;
                return;
            }
        }
        items.push_back({product, quantity});
    }

    void removeProduct(int productId) {
        items.erase(
            remove_if(items.begin(), items.end(), [productId](const pair<Product, int>& item) {
                return item.first.getId() == productId;
            }),
            items.end());
    }
    double calculateTotal(const vector<Promotion>& promotions) const {
        double total = 0.0;
        for (const auto& item : items) {
            double discount = 0.0;
            for (const auto& promo : promotions) {
                if (promo.isApplicable(item.first.getId())) {
                    discount = promo.getDiscount();
                    break;
                }
            }
            total += item.first.getPrice() * item.second * (1 - discount / 100);
        }
        return total;
    }
    void viewCart() const {
        cout << "\n--- Cart ---\n";
        for (const auto& item : items) {
            cout << "Product: " << item.first.getName() << " | Quantity: " << item.second
                 << " | Price: " << item.first.getPrice() << "\n";
        }
    }

    const vector<pair<Product, int>>& getItems() const {
        return items;
    }
    void clearCart() { items.clear(); }

};

// class Order {
// private:
//     int orderId;
//     Customer customer;
//     vector<pair<Product, int>> items;
//     double totalAmount;

// public:
//     Order(int orderId, const Customer& customer, const vector<pair<Product, int>>& items)
//         : orderId(orderId), customer(customer), items(items), totalAmount(0.0) {
//         for (const auto& item : items) {
//             totalAmount += item.first.getPrice() * item.second;
//         }
//     }

//     void viewOrder() const {
//         cout << "\n--- Order Summary ---\n";
//         cout << "Order ID: " << orderId << "\n";
//         cout << "Customer: " << customer.getUsername() << " (" << customer.getEmail() << ")\n";
//         for (const auto& item : items) {
//             cout << "Product: " << item.first.getName() << " | Quantity: " << item.second
//                  << " | Price: " << item.first.getPrice() << "\n";
//         }
//         cout << "Total Amount: " << totalAmount << "\n";
//     }
// };

class Order {
private:
    int orderId;
    string customerUsername;
    vector<pair<Product, int>> items;
    double totalAmount;

public:
    Order(int orderId, const string& username, const vector<pair<Product, int>>& items, double total)
        : orderId(orderId), customerUsername(username), items(items), totalAmount(total) {}

    void viewOrder() const {
        cout << "\n--- Order Summary ---\n";
        cout << "Order ID: " << orderId << "\n";
        cout << "Customer: " << customerUsername << "\n";
        for (const auto& item : items) {
            cout << "Product: " << item.first.getName() << " | Quantity: " << item.second
                 << " | Price: " << item.first.getPrice() << "\n";
        }
        cout << "Total Amount: " << totalAmount << "\n";
    }
     const vector<pair<Product, int>>& getItems() const {
        return items;
    }
};
unordered_map<string, string> loadAdminCredentials(const string& filename) {
    unordered_map<string, string> credentials;
    ifstream inFile(filename);
    string line;
    while (getline(inFile, line)) {
        size_t pos = line.find(',');
        string username = line.substr(0, pos);
        string password = line.substr(pos + 1);
        credentials[username] = password;
    }
    inFile.close();
    return credentials;
}

void saveProductsToFile(const vector<Product>& products, const string& filename) {
    ofstream outFile(filename);
    for (const auto& product : products) {
        outFile << product.getId() << "," << product.getName() << "," << product.getPrice()
                << "," << product.getStock() << "\n";
    }
    outFile.close();
}

void saveReviewsToFile(const vector<Product>& products, const string& filename) {
    ofstream outFile(filename);
    for (const auto& product : products) {
        for (const auto& review : product.getReviews()) {
            outFile << product.getId() << "," << review.first << "," << review.second << "\n";
        }
    }
    outFile.close();
}
vector<Product> loadProductsFromFile(const string& filename) {
    vector<Product> products;
    ifstream inFile(filename);
    string line;
    while (getline(inFile, line)) {
        int id, stock;
        double price;
        string name;
        size_t pos = 0;

        pos = line.find(',');
        id = stoi(line.substr(0, pos));
        line.erase(0, pos + 1);

        pos = line.find(',');
        name = line.substr(0, pos);
        line.erase(0, pos + 1);

        pos = line.find(',');
        price = stod(line.substr(0, pos));
        line.erase(0, pos + 1);

        stock = stoi(line);

        products.emplace_back(id, name, price, stock);
    }
    inFile.close();
    return products;
}

void managerMenu(vector<Product>& products) {
    const string adminUsername = "nguyentienson0512@gmail.com";
    const string adminPassword = "son05122005";

    cout << "\n--- Admin Login ---\n";
    string username, password;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    if (username == adminUsername && password == adminPassword) {
        int choice;
        do {
            cout << "\n--- Admin Menu ---\n";
            cout << "1. Add Product\n2. Edit Product\n3. Delete Product\n4. View Products\n5. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 1) {
                int id, stock;
                double price;
                string name;
                cout << "Enter Product ID: ";
                cin >> id;
                cout << "Enter Product Name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter Product Price: ";
                cin >> price;
                cout << "Enter Stock Quantity: ";
                cin >> stock;
                products.emplace_back(id, name, price, stock);
                saveProductsToFile(products, "admin.txt");
            } else if (choice == 2) {
                int id;
                cout << "Enter Product ID to Edit: ";
                cin >> id;
                for (auto& product : products) {
                    if (product.getId() == id) {
                        double price;
                        int stock;
                        string name;
                        cout << "Enter New Product Name: ";
                        cin.ignore();
                        getline(cin, name);
                        cout << "Enter New Product Price: ";
                        cin >> price;
                        cout << "Enter New Stock Quantity: ";
                        cin >> stock;
                        product = Product(id, name, price, stock);
                        saveProductsToFile(products, "admin.txt");
                        break;
                    }
                }
            } else if (choice == 3) {
                int id;
                cout << "Enter Product ID to Delete: ";
                cin >> id;
                products.erase(remove_if(products.begin(), products.end(), [id](const Product& p) {
                    return p.getId() == id;
                }), products.end());
                saveProductsToFile(products, "admin.txt");
            } else if (choice == 4) {
                for (const auto& product : products) {
                    cout << "ID: " << product.getId() << " | Name: " << product.getName()
                         << " | Price: " << product.getPrice() << " | Stock: " << product.getStock() << "\n";
                }
            }
        } while (choice != 5);
    } else {
        cout << "Invalid credentials!\n";
    }
};

void customerMenu(vector<Product>& products, vector<Promotion>& promotions, vector<Order>& orders) {
    string username;
    cout << "Enter Username: ";
    cin >> username;

    Cart cart;
    int choice;
    do {
        cout << "\n--- Customer Menu ---\n";
        cout << "1. View Products\n2. Add to Cart\n3. Remove from Cart\n4. View Cart\n5. Checkout\n6. View Order History\n7. Leave a Review\n8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            for (const auto& product : products) {
                cout << "ID: " << product.getId() << " | Name: " << product.getName()
                     << " | Price: " << product.getPrice() << " | Stock: " << product.getStock() << "\n";
            }
        } else if (choice == 2) {
            int id, quantity;
            cout << "Enter Product ID: ";
            cin >> id;
            cout << "Enter Quantity: ";
            cin >> quantity;
            for (auto& product : products) {
                if (product.getId() == id) {
                    cart.addProduct(product, quantity);
                    break;
                }
            }
        } else if (choice == 3) {
            int id;
            cout << "Enter Product ID to remove: ";
            cin >> id;
            cart.removeProduct(id);
        } else if (choice == 4) {
            cart.viewCart();
        } else if (choice == 5) {
            // double total = cart.calculateTotal(promotions);
            double total = cart.calculateTotal(promotions);
            cout << "\nTotal Amount: " << total << "\nConfirm Checkout? (y/n): ";
            char confirm;
            cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                static int orderId = 1;
                orders.emplace_back(orderId++, username, cart.getItems(), total);

                for (const auto& item : cart.getItems()) {
                    for (auto& product : products) {
                        if (product.getId() == item.first.getId()) {
                            product.setStock(product.getStock() - item.second);
                        }
                    }
                }

                saveProductsToFile(products, "products1.txt");
                saveReviewsToFile(products, "review.txt");

                cart.clearCart();
                cout << "Checkout successful!\n";
            }
        } else if (choice == 6) {
            for (const auto& order : orders) {
                order.viewOrder();
            }
        } else if (choice == 7) {
            int id, rating;
            cout << "Enter Product ID to review: ";
            cin >> id;
            cout << "Enter Rating (1-5): ";
            cin >> rating;
            for (auto& product : products) {
                if (product.getId() == id) {
                    product.addReview(username, rating);
                    break;
                }
            }
        }
    } while (choice != 8);
}

int main() {
    vector<Product> products = loadProductsFromFile("admin.txt");
    vector<Promotion> promotions; 
    vector<Order> orders;        

    int userType;
    do {
        cout << "\n--- Main Menu ---\n";
        cout << "1. Manager\n2. Customer\n3. Exit\n";
        cout << "Enter your choice: ";
        cin >> userType;

        if (userType == 1) {
            managerMenu(products);
        } else if (userType == 2) {
            customerMenu(products, promotions, orders);
        }
    } while (userType != 3);

    return 0;
}
