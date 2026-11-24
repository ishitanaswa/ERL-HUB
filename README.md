ERL-HUB: Emergency Relief Logistics Management System
Course: Object-Oriented Programming Using C++ (CIC-211)
Institute: Maharaja Agrasen Institute of Technology
Language: Standard C++17
1. Project objective
ERL-HUB is a console-based prototype for managing relief supplies, distribution depots, emergency transport vehicles, dispatch priorities and incident records after an earthquake.
2. Features
Relief inventory management with depot restocking and dispatch.
Ground, aerial, amphibious and hybrid medical vehicles.
Runtime route evaluation through base-class pointers.
Priority-based mission queue.
Custom exception handling.
Binary incident-log persistence.
Deep-copy dynamic cargo manifest.
3. Concepts demonstrated
Unit I — Core foundations and call semantics
ReliefOps::Inventory and ReliefOps::Logistics namespaces.
Inline distance and payload utility functions.
Overloaded allocateSupply() functions support regular stock allocation and priority medical triage.
Overloaded +=, -=, ==, << and >> operators.
Call by reference in restock() and return by reference in stockReference().
IncidentCommander is a friend class of DistributionDepot.
Unit II — Encapsulation, dynamic memory and operator overloading
Private data members in ReliefCargo and DistributionDepot.
Static-free object state is kept private; depot access is controlled through member functions.
CargoManifest uses a parameterized dynamic constructor, deep-copy constructor, assignment operator and destructor.
Static members track total operational vehicles and total dispatched metric tonnage.
Depot restocking and dispatch use overloaded operators.
Unit III — Inheritance, containership and polymorphism
Abstract ReliefVehicle defines pure virtual functions.
AerialTransport, GroundConvoy and AmphibiousCraft derive from the base.
HybridMedicalVehicle uses virtual inheritance through aerial and ground classes to avoid duplicate ReliefVehicle base subobjects.
GPSBeacon, EmergencyLifeSupportUnit and CargoManifest are composed inside the vehicle base class.
A vector<ReliefVehicle*> demonstrates runtime polymorphism and virtual destruction.
Unit IV — Streams, persistence, exceptions and STL
Custom exceptions: CargoOverweightException, FuelDepletionException, DamagedCorridorException.
Binary incident records are written and read with fstream.
Failed writes attempt rollback using stream positioning and file resizing.
STL: vector, unordered_map, sort, for_each.
Template class PriorityDispatchQueue<T, Compare>.
Custom functor UrgencyFirst.
Custom output manipulator tableHeader.
4. Compile and run
Linux / macOS (with a C++17 compiler)
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o erl_hub
./erl_hub
Windows (MinGW)
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o erl_hub.exe
erl_hub.exe
The program creates incident_log.dat in the current working directory when it saves its first incident.
5. Suggested repository structure
ERL-HUB/
├── main.cpp
├── README.md
├── PROJECT_REPORT.md
└── .gitignore
6. Important academic note
This is an educational console prototype, not a production disaster-response platform. It uses simulated data and does not connect to real vehicles, GPS devices, emergency services or live inventory systems. Test it with your compiler and understand each demonstrated concept before submission.
