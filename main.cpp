// main.cpp
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <algorithm>

// To handle the newline character left by `cin`
void clearInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ======================================================================
// 1. DATA STRUCTURE DEFINITIONS
// ======================================================================

// Node for the Doubly Linked List (stores a single medical visit)
struct MedicalRecord {
    std::string date;
    std::string symptoms;
    std::string diagnosis;
    std::string prescription;
    MedicalRecord *next;
    MedicalRecord *prev;

    // Constructor for easy creation
    MedicalRecord(std::string dt, std::string sym, std::string dx, std::string px)
        : date(dt), symptoms(sym), diagnosis(dx), prescription(px), next(nullptr), prev(nullptr) {}
};

// Patient Structure
struct Patient {
    std::string id;
    std::string name;
    MedicalRecord *historyHead; // Head of the doubly linked list
    MedicalRecord *historyTail; // Tail for efficient appending

    Patient(std::string patientId, std::string patientName)
        : id(patientId), name(patientName), historyHead(nullptr), historyTail(nullptr) {}

    // Destructor to free the linked list memory
    ~Patient() {
        MedicalRecord *current = historyHead;
        while (current != nullptr) {
            MedicalRecord *toDelete = current;
            current = current->next;
            delete toDelete;
        }
    }
};

// Doctor Structure
struct Doctor {
    std::string id;
    std::string name;
    std::string specialization;

    Doctor(std::string docId, std::string docName, std::string spec)
        : id(docId), name(docName), specialization(spec) {}
};

// ======================================================================
// 2. MAIN EHR SYSTEM CLASS
// ======================================================================

class EHRSystem {
private:
    // Hash Tables for fast O(1) average time access
    std::unordered_map<std::string, Patient*> patients;
    std::unordered_map<std::string, Doctor*> doctors;

    // Graph (Adjacency List) to represent doctor-patient relationships
    std::unordered_map<std::string, std::vector<std::string>> adjList;

public:
    // Destructor to clean up all dynamically allocated memory
    ~EHRSystem() {
        for (auto const& [id, patientPtr] : patients) {
            delete patientPtr;
        }
        for (auto const& [id, doctorPtr] : doctors) {
            delete doctorPtr;
        }
    }

    // --- Core Functionalities ---

    void addDoctor(const std::string& id, const std::string& name, const std::string& specialization) {
        if (doctors.find(id) != doctors.end()) {
            std::cout << "Error: Doctor with ID '" << id << "' already exists.\n";
            return;
        }
        Doctor* newDoctor = new Doctor(id, name, specialization);
        doctors[id] = newDoctor;
        adjList[id] = {}; // Initialize adjacency list for the new doctor
        std::cout << "✅ Doctor '" << name << "' added successfully.\n";
    }

    void addPatient(const std::string& id, const std::string& name) {
        if (patients.find(id) != patients.end()) {
            std::cout << "Error: Patient with ID '" << id << "' already exists.\n";
            return;
        }
        Patient* newPatient = new Patient(id, name);
        patients[id] = newPatient;
        adjList[id] = {}; // Initialize adjacency list for the new patient
        std::cout << "✅ Patient '" << name << "' added successfully.\n";
    }

    void linkDoctorPatient(const std::string& docId, const std::string& patientId) {
        if (doctors.find(docId) == doctors.end()) {
            std::cout << "Error: Doctor with ID '" << docId << "' not found.\n";
            return;
        }
        if (patients.find(patientId) == patients.end()) {
            std::cout << "Error: Patient with ID '" << patientId << "' not found.\n";
            return;
        }

        // Add edge in both directions for the many-to-many relationship
        adjList[docId].push_back(patientId);
        adjList[patientId].push_back(docId);
        std::cout << "🔗 Successfully linked Dr. " << doctors[docId]->name << " and Patient " << patients[patientId]->name << ".\n";
    }

    void addMedicalRecord(const std::string& patientId, const std::string& date, const std::string& symptoms, const std::string& diagnosis, const std::string& prescription) {
        if (patients.find(patientId) == patients.end()) {
            std::cout << "Error: Patient with ID '" << patientId << "' not found.\n";
            return;
        }

        Patient* patient = patients[patientId];
        MedicalRecord* newRecord = new MedicalRecord(date, symptoms, diagnosis, prescription);

        // Append to the patient's doubly linked list
        if (patient->historyHead == nullptr) {
            // List is empty
            patient->historyHead = newRecord;
            patient->historyTail = newRecord;
        } else {
            // Append to the end
            patient->historyTail->next = newRecord;
            newRecord->prev = patient->historyTail;
            patient->historyTail = newRecord;
        }
        std::cout << "📝 Medical record added for patient '" << patient->name << "'.\n";
    }

    // --- Display and Search Functionalities ---

    void displayPatientHistory(const std::string& patientId) {
        if (patients.find(patientId) == patients.end()) {
            std::cout << "Error: Patient with ID '" << patientId << "' not found.\n";
            return;
        }

        Patient* patient = patients[patientId];
        std::cout << "\n--- Medical History for " << patient->name << " (ID: " << patient->id << ") ---\n";
        
        if (patient->historyHead == nullptr) {
            std::cout << "No medical records found.\n";
            return;
        }

        MedicalRecord* current = patient->historyHead;
        int recordCount = 1;
        while (current != nullptr) {
            std::cout << "Record " << recordCount++ << ":\n";
            std::cout << "  Date:         " << current->date << "\n";
            std::cout << "  Symptoms:     " << current->symptoms << "\n";
            std::cout << "  Diagnosis:    " << current->diagnosis << "\n";
            std::cout << "  Prescription: " << current->prescription << "\n";
            std::cout << "-------------------------------------------\n";
            current = current->next;
        }
    }
    
    void displayPatientInfo(const std::string& patientId) {
        if (patients.find(patientId) == patients.end()) {
            std::cout << "Error: Patient with ID '" << patientId << "' not found.\n";
            return;
        }
        Patient* p = patients[patientId];
        std::cout << "\n--- Patient Information ---\n";
        std::cout << "ID:   " << p->id << "\n";
        std::cout << "Name: " << p->name << "\n";
        std::cout << "Consulted Doctors:\n";
        
        bool hasDoctors = false;
        for (const auto& connectedId : adjList[patientId]) {
            if (doctors.count(connectedId)) { // Check if the connected node is a doctor
                std::cout << " - Dr. " << doctors[connectedId]->name << " (" << doctors[connectedId]->specialization << ")\n";
                hasDoctors = true;
            }
        }
        if (!hasDoctors) {
            std::cout << " - None on record.\n";
        }
        std::cout << "---------------------------\n";
    }

    void findPatientsBySymptom(const std::string& symptom) {
        std::vector<Patient*> foundPatients;
        std::string lowerSymptom = symptom;
        std::transform(lowerSymptom.begin(), lowerSymptom.end(), lowerSymptom.begin(), ::tolower);

        // Iterate through all patients (hash table)
        for (auto const& [id, patientPtr] : patients) {
            // Iterate through their medical history (linked list)
            MedicalRecord* current = patientPtr->historyHead;
            while (current != nullptr) {
                std::string lowerRecordSymptoms = current->symptoms;
                std::transform(lowerRecordSymptoms.begin(), lowerRecordSymptoms.end(), lowerRecordSymptoms.begin(), ::tolower);
                
                // Simple substring search
                if (lowerRecordSymptoms.find(lowerSymptom) != std::string::npos) {
                    foundPatients.push_back(patientPtr);
                    break; // Found a match, no need to check other records for this patient
                }
                current = current->next;
            }
        }

        std::cout << "\n--- Patients with symptom: '" << symptom << "' ---\n";
        if (foundPatients.empty()) {
            std::cout << "No patients found with this symptom.\n";
        } else {
            for (const auto& patient : foundPatients) {
                std::cout << " - " << patient->name << " (ID: " << patient->id << ")\n";
            }
        }
        std::cout << "-------------------------------------------\n";
    }
};

// ======================================================================
// 3. USER INTERFACE (MENU)
// ======================================================================

void printMenu() {
    std::cout << "\n===== Miniature EHR System Menu =====\n";
    std::cout << "1. Add Doctor\n";
    std::cout << "2. Add Patient\n";
    std::cout << "3. Link Doctor and Patient\n";
    std::cout << "4. Add Patient Medical Record\n";
    std::cout << "5. View Patient Medical History\n";
    std::cout << "6. View Patient Info and Doctors\n";
    std::cout << "7. Find Patients by Symptom\n";
    std::cout << "0. Exit\n";
    std::cout << "=====================================\n";
    std::cout << "Enter your choice: ";
}

int main() {
    EHRSystem ehr;
    int choice;

    
    ehr.addDoctor("D001", "Ronith", "Cardiologist");
    ehr.addDoctor("D002", "Harsimran", "Dermatologist");
    ehr.addPatient("P101", "Kapish");
    ehr.addPatient("P102", "Medhansh");
    ehr.linkDoctorPatient("D001", "P101");
    ehr.linkDoctorPatient("D002", "P101");
    ehr.linkDoctorPatient("D002", "P102");
    ehr.addMedicalRecord("P101", "2025-09-20", "Chest pain, dizziness", "Angina", "Aspirin");
    ehr.addMedicalRecord("P101", "2025-09-25", "Itchy rash on arm", "Eczema", "Hydrocortisone cream");
    ehr.addMedicalRecord("P102", "2025-09-24", "Dry skin, persistent rash", "Psoriasis", "Topical Steroids");
    
    std::cout << "\n--- System pre-populated with sample data. ---";

    do {
        printMenu();
        std::cin >> choice;
        clearInputBuffer(); 
        std::string id, name, spec, docId, patId, date, sym, dx, px;

        switch (choice) {
            case 1:
                std::cout << "Enter Doctor ID: ";
                std::getline(std::cin, id);
                std::cout << "Enter Doctor Name: ";
                std::getline(std::cin, name);
                std::cout << "Enter Specialization: ";
                std::getline(std::cin, spec);
                ehr.addDoctor(id, name, spec);
                break;
            case 2:
                std::cout << "Enter Patient ID: ";
                std::getline(std::cin, id);
                std::cout << "Enter Patient Name: ";
                std::getline(std::cin, name);
                ehr.addPatient(id, name);
                break;
            case 3:
                std::cout << "Enter Doctor ID: ";
                std::getline(std::cin, docId);
                std::cout << "Enter Patient ID: ";
                std::getline(std::cin, patId);
                ehr.linkDoctorPatient(docId, patId);
                break;
            case 4:
                std::cout << "Enter Patient ID: ";
                std::getline(std::cin, patId);
                std::cout << "Enter Date (YYYY-MM-DD): ";
                std::getline(std::cin, date);
                std::cout << "Enter Symptoms: ";
                std::getline(std::cin, sym);
                std::cout << "Enter Diagnosis: ";
                std::getline(std::cin, dx);
                std::cout << "Enter Prescription: ";
                std::getline(std::cin, px);
                ehr.addMedicalRecord(patId, date, sym, dx, px);
                break;
            case 5:
                std::cout << "Enter Patient ID to view history: ";
                std::getline(std::cin, patId);
                ehr.displayPatientHistory(patId);
                break;
            case 6:
                std::cout << "Enter Patient ID to view info: ";
                std::getline(std::cin, patId);
                ehr.displayPatientInfo(patId);
                break;
            case 7:
                std::cout << "Enter symptom to search for: ";
                std::getline(std::cin, sym);
                ehr.findPatientsBySymptom(sym);
                break;
            case 0:
                std::cout << "Exiting system. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }

    } while (choice != 0);

    return 0;
}