
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cmath>

using namespace std;

// =====================================================
// ERL-HUB: Emergency Relief Logistics Hub
// Single-file C++11 Project
// =====================================================

namespace ReliefOps {
namespace Inventory {

// ---------------- UNIT I & II ----------------

struct CargoItem {
    char name[40];
    int quantity;
    double unitWeight;
    int priority;

    CargoItem(const string& n = "", int q = 0,
              double w = 0.0, int p = 1)
        : quantity(q), unitWeight(w), priority(p) {
        setName(n);
    }

    void setName(const string& n) {
        strncpy(name, n.c_str(), sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }

    string getName() const {
        return string(name);
    }

    double totalWeight() const {
        return quantity * unitWeight;
    }
};

// Function overloading
inline int allocateSupply(int available, int requested) {
    if (requested < 0) return 0;
    return min(available, requested);
}

inline int allocateSupply(int available, int requested,
                          bool emergencyMedical) {
    if (requested < 0) return 0;

    if (emergencyMedical)
        return min(available, requested);

    return min(available, requested / 2);
}

// Dynamic memory allocation
class ReliefCargo {
private:
    CargoItem* items;
    int count;
    int capacity;

    void grow() {
        int newCapacity = (capacity == 0) ? 2 : capacity * 2;

        CargoItem* temp = new CargoItem[newCapacity];

        for (int i = 0; i < count; i++)
            temp[i] = items[i];

        delete[] items;

        items = temp;
        capacity = newCapacity;
    }

public:
    ReliefCargo() : items(NULL), count(0), capacity(0) {}

    // Dynamic constructor
    explicit ReliefCargo(int initialCapacity)
        : items(NULL), count(0),
          capacity(initialCapacity > 0 ? initialCapacity : 0) {
        if (capacity > 0)
            items = new CargoItem[capacity];
    }

    // Copy constructor
    ReliefCargo(const ReliefCargo& other)
        : items(NULL), count(other.count),
          capacity(other.capacity) {

        if (capacity > 0) {
            items = new CargoItem[capacity];

            for (int i = 0; i < count; i++)
                items[i] = other.items[i];
        }
    }

    // Assignment operator
    ReliefCargo& operator=(const ReliefCargo& other) {
        if (this != &other) {
            CargoItem* temp =
                (other.capacity > 0)
                ? new CargoItem[other.capacity] : NULL;

            for (int i = 0; i < other.count; i++)
                temp[i] = other.items[i];

            delete[] items;

            items = temp;
            count = other.count;
            capacity = other.capacity;
        }

        return *this;
    }

    // Destructor
    ~ReliefCargo() {
        delete[] items;
    }

    int size() const {
        return count;
    }

    const CargoItem& at(int index) const {
        return items[index];
    }

    int find(const string& itemName) const {
        for (int i = 0; i < count; i++) {
            if (itemName == items[i].getName())
                return i;
        }

        return -1;
    }

    void add(const CargoItem& item) {
        if (item.quantity <= 0)
            throw invalid_argument("Quantity must be positive.");

        int index = find(item.getName());

        if (index >= 0) {
            items[index].quantity += item.quantity;
            items[index].unitWeight = item.unitWeight;
            items[index].priority =
                max(items[index].priority, item.priority);
            return;
        }

        if (count == capacity)
            grow();

        items[count++] = item;
    }

    bool remove(const CargoItem& item) {
        int index = find(item.getName());

        if (index < 0 || item.quantity <= 0 ||
            items[index].quantity < item.quantity)
            return false;

        items[index].quantity -= item.quantity;

        if (items[index].quantity == 0) {
            for (int i = index; i < count - 1; i++)
                items[i] = items[i + 1];

            --count;
        }

        return true;
    }

    double totalWeight() const {
        double total = 0;

        for (int i = 0; i < count; i++)
            total += items[i].totalWeight();

        return total;
    }

    bool sameManifest(const ReliefCargo& other) const {
        if (count != other.count)
            return false;

        for (int i = 0; i < count; i++) {
            int j = other.find(items[i].getName());

            if (j < 0 ||
                items[i].quantity != other.items[j].quantity)
                return false;
        }

        return true;
    }

    // Binary file writing
    bool saveBinary(const string& filename) const {
        string tempName = filename + ".tmp";

        ofstream out(tempName.c_str(),
                     ios::binary | ios::trunc);

        if (!out)
            return false;

        out.write(reinterpret_cast<const char*>(&count),
                  sizeof(count));

        for (int i = 0; i < count; i++) {
            out.write(reinterpret_cast<const char*>(&items[i]),
                      sizeof(CargoItem));
        }

        out.close();

        if (!out) {
            std::remove(tempName.c_str());
            return false;
        }

        return std::rename(tempName.c_str(),
                           filename.c_str()) == 0;
    }

    // Binary file reading
    bool loadBinary(const string& filename) {
        ifstream in(filename.c_str(), ios::binary);

        if (!in)
            return false;

        int newCount = 0;

        in.read(reinterpret_cast<char*>(&newCount),
                sizeof(newCount));

        if (!in || newCount < 0 || newCount > 10000)
            return false;

        ReliefCargo loaded;

        for (int i = 0; i < newCount; i++) {
            CargoItem item;

            in.read(reinterpret_cast<char*>(&item),
                    sizeof(CargoItem));

            if (!in)
                return false;

            loaded.add(item);
        }

        *this = loaded;

        return true;
    }

    // Output operator
    friend ostream& operator<<(ostream& out,
                               const ReliefCargo& cargo);

    // Input operator
    friend istream& operator>>(istream& in,
                               ReliefCargo& cargo);
};

// Output operator overloading
ostream& operator<<(ostream& out, const ReliefCargo& cargo) {
    out << "ITEM|QUANTITY|UNIT WEIGHT(kg)|PRIORITY\n";

    for (int i = 0; i < cargo.count; i++) {
        out << cargo.items[i].name << '|'
            << cargo.items[i].quantity << '|'
            << cargo.items[i].unitWeight << '|'
            << cargo.items[i].priority << '\n';
    }

    return out;
}

// Input operator overloading
istream& operator>>(istream& in, ReliefCargo& cargo) {
    string name;
    int quantity, priority;
    double weight;

    cout << "Item name (one word): ";
    in >> name;

    cout << "Quantity: ";
    in >> quantity;

    cout << "Unit weight in kg: ";
    in >> weight;

    cout << "Priority (1 normal, 2 important, 3 emergency): ";
    in >> priority;

    if (in && quantity > 0 && weight >= 0 &&
        priority >= 1 && priority <= 3) {

        cargo.add(CargoItem(name, quantity, weight, priority));
    }
    else {
        in.setstate(ios::failbit);
    }

    return in;
}

// Distribution Depot
class DistributionDepot {
private:
    string depotName;
    ReliefCargo stock;

public:
    explicit DistributionDepot(
        const string& name = "Central Depot")
        : depotName(name) {}

    const string& getName() const {
        return depotName;
    }

    const ReliefCargo& getStock() const {
        return stock;
    }

    // Restocking
    DistributionDepot& operator+=(const CargoItem& item) {
        stock.add(item);
        return *this;
    }

    // Cargo deduction
    DistributionDepot& operator-=(const CargoItem& item) {
        if (!stock.remove(item))
            throw runtime_error("Insufficient depot stock.");

        return *this;
    }

    // Manifest comparison
    bool operator==(const DistributionDepot& other) const {
        return stock.sameManifest(other.stock);
    }

    friend ostream& operator<<(ostream& out,
                               const DistributionDepot& depot) {
        out << "Depot: " << depot.depotName << '\n';
        out << depot.stock;
        return out;
    }

    friend istream& operator>>(istream& in,
                               DistributionDepot& depot) {
        ReliefCargo item;
        in >> item;

        if (in && item.size() > 0)
            depot.stock.add(item.at(0));

        return in;
    }
};

} // Inventory namespace

// ---------------- UNIT I ----------------

// Call by reference and return by reference
inline int& updateStock(int& stock, int change) {
    stock += change;

    if (stock < 0)
        stock = 0;

    return stock;
}

// Inline distance calculation
inline double calculateDistance(
    double x1, double y1, double x2, double y2) {

    double dx = x2 - x1;
    double dy = y2 - y1;

    return std::sqrt(dx * dx + dy * dy);
}

// Inline payload calculation
inline double calculatePayloadPercentage(
    double current, double capacity) {

    if (capacity <= 0)
        return 0;

    return (current / capacity) * 100;
}

// ---------------- UNIT IV: EXCEPTIONS ----------------

class CargoOverweightException : public runtime_error {
public:
    explicit CargoOverweightException(const string& s)
        : runtime_error(s) {}
};

class FuelDepletionException : public runtime_error {
public:
    explicit FuelDepletionException(const string& s)
        : runtime_error(s) {}
};

class DamagedCorridorException : public runtime_error {
public:
    explicit DamagedCorridorException(const string& s)
        : runtime_error(s) {}
};

// ---------------- COMPOSITION ----------------

class GPSBeacon {
private:
    double x, y;
    bool signalActive;

public:
    GPSBeacon(double px = 0, double py = 0)
        : x(px), y(py), signalActive(true) {}

    void update(double px, double py) {
        x = px;
        y = py;
    }

    void loseSignal() {
        signalActive = false;
    }

    void restoreSignal() {
        signalActive = true;
    }

    bool hasSignal() const {
        return signalActive;
    }

    double distanceTo(double dx, double dy) const {
        return calculateDistance(x, y, dx, dy);
    }

    void display() const {
        cout << "GPS: (" << x << ", " << y << ") - "
             << (signalActive ? "Signal active" :
                                "Signal unavailable") << '\n';
    }
};

class EmergencyLifeSupportUnit {
private:
    int medicalKits;
    int oxygenUnits;

public:
    EmergencyLifeSupportUnit(int kits = 0, int oxygen = 0)
        : medicalKits(kits), oxygenUnits(oxygen) {}

    void display() const {
        cout << "Life support: " << medicalKits
             << " trauma kits, "
             << oxygenUnits << " oxygen units\n";
    }
};

// ---------------- UNIT IV: FILE HANDLING ----------------

struct DispatchRecord {
    char vehicle[40];
    char destination[60];
    double distance;
    double cargoWeight;
    int successful;
};

bool appendAuditRecord(const DispatchRecord& record);

// ---------------- UNIT III: INHERITANCE ----------------

class ReliefVehicle {
protected:
    string vehicleId;
    double maxPayload;
    double fuel;
    double x, y;

    Inventory::ReliefCargo manifest;

    // Composition
    GPSBeacon gps;
    EmergencyLifeSupportUnit lifeSupport;

    // Static members
    static int totalDispatched;
    static double totalDispatchedTonnes;

    void addRecord(const string& destination,
                   double distance, bool success) const {

        DispatchRecord record;

        memset(&record, 0, sizeof(record));

        strncpy(record.vehicle, vehicleId.c_str(),
                sizeof(record.vehicle) - 1);

        strncpy(record.destination, destination.c_str(),
                sizeof(record.destination) - 1);

        record.distance = distance;
        record.cargoWeight = manifest.totalWeight();
        record.successful = success ? 1 : 0;

        appendAuditRecord(record);
    }

public:
    ReliefVehicle(const string& id, double capacity,
                  double initialFuel, double px, double py)
        : vehicleId(id),
          maxPayload(capacity),
          fuel(initialFuel),
          x(px), y(py),
          gps(px, py),
          lifeSupport(2, 4) {}

    // Virtual destructor
    virtual ~ReliefVehicle() {}

    // Pure virtual functions
    virtual string type() const = 0;
    virtual double fuelCostPerKm() const = 0;
    virtual bool routeIsSafe(double distance) const = 0;

    string getId() const {
        return vehicleId;
    }

    double getPayloadWeight() const {
        return manifest.totalWeight();
    }

    double getPayloadCapacity() const {
        return maxPayload;
    }

    const Inventory::ReliefCargo& getManifest() const {
        return manifest;
    }

    void loadCargo(const Inventory::CargoItem& item) {
        manifest.add(item);

        if (manifest.totalWeight() > maxPayload) {
            manifest.remove(item);

            throw CargoOverweightException(
                "Cargo exceeds vehicle payload capacity.");
        }
    }

    void showDetails() const {
        cout << "\nVehicle " << vehicleId
             << " [" << type() << "]\n";

        cout << "Payload: " << manifest.totalWeight()
             << "/" << maxPayload << " kg ("
             << calculatePayloadPercentage(
                    manifest.totalWeight(), maxPayload)
             << "%)\n";

        cout << "Fuel: " << fuel << " units\n";

        gps.display();
        lifeSupport.display();

        cout << manifest;
    }

    // Runtime polymorphism and dispatch evaluation
    bool dispatch(const string& destination,
                  double dx, double dy) {

        double distance = gps.distanceTo(dx, dy);

        try {
            if (!gps.hasSignal())
                throw DamagedCorridorException(
                    "GPS signal unavailable.");

            if (!routeIsSafe(distance))
                throw DamagedCorridorException(
                    "Route is unsafe for this vehicle.");

            double requiredFuel =
                distance * fuelCostPerKm();

            if (fuel < requiredFuel)
                throw FuelDepletionException(
                    "Not enough fuel for this route.");

            if (manifest.totalWeight() > maxPayload)
                throw CargoOverweightException(
                    "Vehicle is overloaded.");

            fuel -= requiredFuel;

            x = dx;
            y = dy;

            gps.update(dx, dy);

            ++totalDispatched;

            totalDispatchedTonnes +=
                manifest.totalWeight() / 1000.0;

            cout << "Dispatch successful: "
                 << vehicleId << " -> " << destination
                 << " | distance " << distance << " km\n";

            addRecord(destination, distance, true);

            return true;
        }
        catch (const exception& e) {
            cout << "Dispatch failed: "
                 << e.what() << '\n';

            addRecord(destination, distance, false);

            return false;
        }
    }

    static int getTotalDispatched() {
        return totalDispatched;
    }

    static double getTotalDispatchedTonnes() {
        return totalDispatchedTonnes;
    }

    // Friend class
    friend class IncidentCommander;
};

// Static member definitions
int ReliefVehicle::totalDispatched = 0;
double ReliefVehicle::totalDispatchedTonnes = 0.0;

// ---------------- FILE HANDLING ----------------

// Binary audit file with temporary-file replacement
bool appendAuditRecord(const DispatchRecord& record) {
    const string filename = "dispatch_audit.bin";
    const string tempName = "dispatch_audit.tmp";

    vector<DispatchRecord> records;

    ifstream in(filename.c_str(), ios::binary);

    if (in) {
        DispatchRecord oldRecord;

        while (in.read(reinterpret_cast<char*>(&oldRecord),
                       sizeof(oldRecord))) {
            records.push_back(oldRecord);
        }

        in.close();
    }

    records.push_back(record);

    ofstream out(tempName.c_str(),
                 ios::binary | ios::trunc);

    if (!out)
        return false;

    for (size_t i = 0; i < records.size(); i++) {
        out.write(reinterpret_cast<const char*>(&records[i]),
                  sizeof(DispatchRecord));
    }

    out.close();

    if (!out) {
        std::remove(tempName.c_str());
        return false;
    }

    return std::rename(tempName.c_str(),
                       filename.c_str()) == 0;
}

// ---------------- UNIT I: FRIEND CLASS ----------------

class IncidentCommander {
public:
    void emergencyTriageOverride(
        ReliefVehicle& vehicle, double extraCapacity) {

        if (extraCapacity > 0) {
            vehicle.maxPayload += extraCapacity;

            cout << "Emergency override approved for "
                 << vehicle.vehicleId
                 << ". New payload capacity: "
                 << vehicle.maxPayload << " kg\n";
        }
    }
};

// ---------------- UNIT III: DERIVED CLASSES ----------------

// Virtual inheritance
class AerialTransport : virtual public ReliefVehicle {
public:
    AerialTransport(const string& id, double cap,
                    double fuel, double x, double y)
        : ReliefVehicle(id, cap, fuel, x, y) {}

    string type() const {
        return "Aerial Transport";
    }

    double fuelCostPerKm() const {
        return 0.8;
    }

    bool routeIsSafe(double distance) const {
        return distance <= 500.0;
    }
};

class GroundConvoy : virtual public ReliefVehicle {
public:
    GroundConvoy(const string& id, double cap,
                 double fuel, double x, double y)
        : ReliefVehicle(id, cap, fuel, x, y) {}

    string type() const {
        return "Ground Convoy";
    }

    double fuelCostPerKm() const {
        return 0.35;
    }

    bool routeIsSafe(double distance) const {
        return distance <= 250.0;
    }
};

class AmphibiousCraft : virtual public ReliefVehicle {
public:
    AmphibiousCraft(const string& id, double cap,
                    double fuel, double x, double y)
        : ReliefVehicle(id, cap, fuel, x, y) {}

    string type() const {
        return "Amphibious Craft";
    }

    double fuelCostPerKm() const {
        return 0.5;
    }

    bool routeIsSafe(double distance) const {
        return distance <= 150.0;
    }
};

// Hybrid multiple inheritance
class AmphibiousMedicalUnit
    : public GroundConvoy, public AmphibiousCraft {

public:
    AmphibiousMedicalUnit(
        const string& id, double cap,
        double fuel, double x, double y)

        : ReliefVehicle(id, cap, fuel, x, y),
          GroundConvoy(id, cap, fuel, x, y),
          AmphibiousCraft(id, cap, fuel, x, y) {}

    string type() const {
        return "Hybrid Amphibious Medical Unit";
    }

    double fuelCostPerKm() const {
        return 0.45;
    }

    bool routeIsSafe(double distance) const {
        return distance <= 180.0;
    }
};

// ---------------- UNIT IV: TEMPLATES ----------------

// Priority comparison functor
template <typename T>
struct greaterPriority {
    bool operator()(const T& a, const T& b) const {
        return a.priority > b.priority;
    }
};

// Generic priority dispatch queue
template <typename T>
class PriorityDispatchQueue {
private:
    vector<T> entries;

public:
    void push(const T& value) {
        entries.push_back(value);
    }

    bool empty() const {
        return entries.empty();
    }

    T pop() {
        if (entries.empty())
            throw runtime_error("Priority queue is empty.");

        sort(entries.begin(), entries.end(),
             greaterPriority<T>());

        T first = entries.front();

        entries.erase(entries.begin());

        return first;
    }

    size_t size() const {
        return entries.size();
    }
};

// User-defined stream manipulators
ostream& tableHeader(ostream& out) {
    out << left
        << setw(18) << "VEHICLE ID"
        << setw(34) << "VEHICLE TYPE"
        << "PAYLOAD (kg)\n";

    out << string(68, '-') << '\n';

    return out;
}

ostream& tableRow(ostream& out,
                  const ReliefVehicle& vehicle) {

    out << left
        << setw(18) << vehicle.getId()
        << setw(34) << vehicle.type()
        << fixed << setprecision(1)
        << vehicle.getPayloadWeight() << '\n';

    return out;
}

// ---------------- AUDIT LOG READING ----------------

void showAuditLog() {
    ifstream in("dispatch_audit.bin", ios::binary);

    if (!in) {
        cout << "No audit log exists yet.\n";
        return;
    }

    DispatchRecord r;

    cout << "\nDISPATCH AUDIT LOG\n";

    cout << left
         << setw(16) << "VEHICLE"
         << setw(24) << "DESTINATION"
         << setw(12) << "DISTANCE"
         << setw(14) << "CARGO kg"
         << "STATUS\n";

    while (in.read(reinterpret_cast<char*>(&r), sizeof(r))) {
        cout << left
             << setw(16) << r.vehicle
             << setw(24) << r.destination
             << setw(12) << fixed << setprecision(1)
             << r.distance
             << setw(14) << r.cargoWeight
             << (r.successful ? "SUCCESS" : "FAILED")
             << '\n';
    }
}

} // ReliefOps namespace

// =====================================================
// MAIN FUNCTION
// =====================================================

int main() {

    using namespace ReliefOps;
    using namespace ReliefOps::Inventory;

    cout << "==============================================\n";
    cout << " POST-EARTHQUAKE EMERGENCY RELIEF LOGISTICS HUB\n";
    cout << "==============================================\n";

    // UNIT I: Reference and function overloading

    int waterStock = 100;

    int& updatedStock = updateStock(waterStock, -10);

    cout << "Available water units after update: "
         << updatedStock << '\n';

    cout << "Normal allocation: "
         << allocateSupply(50, 30) << " units\n";

    cout << "Emergency medical allocation: "
         << allocateSupply(50, 30, true) << " units\n";

    // UNIT II: Dynamic memory and copy constructor

    ReliefCargo dynamicDemo(4);

    dynamicDemo.add(
        CargoItem("DemoBlankets", 1, 1.0, 1));

    ReliefCargo copiedDemo(dynamicDemo);

    cout << "Dynamic/copy constructor demo: "
         << copiedDemo.size()
         << " cargo item(s)\n";

    // Creating central depot

    DistributionDepot centralDepot(
        "Central Relief Depot");

    centralDepot += CargoItem(
        "RationKits", 30, 2.0, 2);

    centralDepot += CargoItem(
        "TraumaKits", 10, 1.0, 3);

    centralDepot += CargoItem(
        "WaterTanks", 5, 8.0, 2);

    cout << "\n" << centralDepot;

    // Comparing depot manifests

    DistributionDepot transitDepot("Transit Depot");

    transitDepot += CargoItem(
        "RationKits", 30, 2.0, 2);

    transitDepot += CargoItem(
        "TraumaKits", 10, 1.0, 3);

    transitDepot += CargoItem(
        "WaterTanks", 5, 8.0, 2);

    cout << "Depot manifests identical? "
         << (centralDepot == transitDepot ? "Yes" : "No")
         << '\n';

    // UNIT III: Polymorphism and inheritance

    vector<ReliefVehicle*> fleet;

    fleet.push_back(
        new AerialTransport("AIR-01", 500, 500, 0, 0));

    fleet.push_back(
        new GroundConvoy("GROUND-01", 1500, 300, 0, 0));

    fleet.push_back(
        new AmphibiousCraft("BOAT-01", 800, 250, 0, 0));

    fleet.push_back(
        new AmphibiousMedicalUnit(
            "HYBRID-01", 1000, 300, 0, 0));

    try {

        // Depot cargo deduction

        centralDepot -= CargoItem(
            "RationKits", 5, 2.0, 2);

        // Loading cargo into vehicles

        fleet[0]->loadCargo(
            CargoItem("TraumaKits", 10, 1.0, 3));

        fleet[1]->loadCargo(
            CargoItem("RationKits", 20, 2.0, 2));

        fleet[2]->loadCargo(
            CargoItem("WaterTanks", 5, 8.0, 2));

        fleet[3]->loadCargo(
            CargoItem("TraumaKits", 20, 1.0, 3));

        // Friend class emergency override

        IncidentCommander commander;

        commander.emergencyTriageOverride(
            *fleet[3], 100);

        // Runtime polymorphism

        fleet[0]->dispatch(
            "North Field Hospital", 100, 80);

        fleet[1]->dispatch(
            "Relief Camp A", 40, 30);

        fleet[2]->dispatch(
            "Flooded Village", 30, 20);

        fleet[3]->dispatch(
            "Emergency Medical Zone", 50, 40);

        // Binary manifest saving

        fleet[1]->getManifest().saveBinary(
            "ground_manifest.bin");

        // Binary manifest loading

        ReliefCargo restoredManifest;

        if (restoredManifest.loadBinary(
                "ground_manifest.bin")) {

            cout << "\nManifest restored from binary file:\n";
            cout << restoredManifest;
        }

        // STL unordered_map

        unordered_map<string, int> stockSummary;

        stockSummary["RationKits"] = 25;
        stockSummary["TraumaKits"] = 10;
        stockSummary["WaterTanks"] = 5;

        cout << "\nSTL stock summary:\n";

        for (unordered_map<string, int>::const_iterator it =
                 stockSummary.begin();
             it != stockSummary.end(); ++it) {

            cout << "  " << it->first
                 << ": " << it->second << '\n';
        }

        // Template priority queue

        PriorityDispatchQueue<CargoItem> urgentQueue;

        urgentQueue.push(
            CargoItem("Blankets", 20, 1.0, 1));

        urgentQueue.push(
            CargoItem("TraumaKits", 5, 1.0, 3));

        urgentQueue.push(
            CargoItem("CleanWater", 10, 1.0, 2));

        cout << "\nPriority dispatch order:\n";

        while (!urgentQueue.empty()) {

            CargoItem next = urgentQueue.pop();

            cout << "  " << next.getName()
                 << " (priority " << next.priority << ")\n";
        }

        // User-defined stream manipulators

        cout << "\nFLEET SUMMARY\n";

        tableHeader(cout);

        for (size_t i = 0; i < fleet.size(); i++)
            tableRow(cout, *fleet[i]);

    }
    catch (const exception& e) {

        cout << "System error: "
             << e.what() << '\n';
    }

    // Static variable analytics

    cout << "\nTotal successful dispatches: "
         << ReliefVehicle::getTotalDispatched() << '\n';

    cout << "Total dispatched cargo: "
         << fixed << setprecision(3)
         << ReliefVehicle::getTotalDispatchedTonnes()
         << " tonnes\n";

    // Display audit records

    showAuditLog();

    // Virtual destructor and memory cleanup

    for (size_t i = 0; i < fleet.size(); i++)
        delete fleet[i];

    fleet.clear();

    cout << "\nERL-HUB run completed.\n";

    cout << "Files created: dispatch_audit.bin "
         << "and ground_manifest.bin\n";

    return 0;
}
