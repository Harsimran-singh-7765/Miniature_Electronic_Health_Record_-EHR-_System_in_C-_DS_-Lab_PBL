#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <set>
#include <sstream>
#include <iomanip>

using namespace std;

// Developer: Kapish
struct MedicalRecord {
    string date, symptoms, diagnosis, prescription, doctorId;
    MedicalRecord *next;
    MedicalRecord *prev;

    MedicalRecord(string dt, string sym, string dx, string px, string docId)
        : date(dt), symptoms(sym), diagnosis(dx), prescription(px), doctorId(docId), next(nullptr), prev(nullptr) {}
};

// Developer: Kapish
struct Patient {
    string id, name;
    MedicalRecord *historyHead;
    MedicalRecord *historyTail;

    Patient(string patientId, string patientName)
        : id(patientId), name(patientName), historyHead(nullptr), historyTail(nullptr) {}

    ~Patient() {
        MedicalRecord *current = historyHead;
        while (current != nullptr) {
            MedicalRecord *toDelete = current;
            current = current->next;
            delete toDelete;
        }
    }
};

// Developer: Medhansh
struct Doctor {
    string id, name, specialization;

    Doctor(string docId, string docName, string spec)
        : id(docId), name(docName), specialization(spec) {}
};

class EHRSystem {
private:
    unordered_map<string, Patient*> patients;
    unordered_map<string, Doctor*> doctors;
    unordered_map<string, vector<string>> adjList;

    bool smartSearch(const string& text, const string& query) {
        string lowerText = text; string lowerQuery = query;
        transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
        transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        return lowerText.find(lowerQuery) != string::npos;
    }

public:
    ~EHRSystem() {
        for (auto const& [id, ptr] : patients) delete ptr;
        for (auto const& [id, ptr] : doctors) delete ptr;
    }

    void addDoctor(const string& id, const string& name, const string& spec) {
        if (doctors.count(id)) { cout << "Error: Doctor ID exists.\n"; return; }
        doctors[id] = new Doctor(id, name, spec);
        adjList[id] = {};
        cout << "Success: Doctor registered.\n";
    }

    void addPatient(const string& id, const string& name) {
        if (patients.count(id)) { cout << "Error: Patient ID exists.\n"; return; }
        patients[id] = new Patient(id, name);
        adjList[id] = {};
        cout << "Success: Patient registered.\n";
    }

    void linkDoctorPatient(const string& docId, const string& patId) {
        if (!doctors.count(docId) || !patients.count(patId)) { cout << "Error: Invalid IDs.\n"; return; }
        adjList[docId].push_back(patId);
        adjList[patId].push_back(docId);
        cout << "Network: Linked Doctor and Patient.\n";
    }

    void addMedicalRecord(const string& patId, const string& docId, const string& date, const string& sym, const string& dx, const string& px) {
        if (!patients.count(patId)) { cout << "Error: Patient not found.\n"; return; }
        Patient* p = patients[patId];
        MedicalRecord* newRec = new MedicalRecord(date, sym, dx, px, docId);

        if (!p->historyHead) {
            p->historyHead = newRec;
            p->historyTail = newRec;
        } else {
            p->historyTail->next = newRec;
            newRec->prev = p->historyTail;
            p->historyTail = newRec;
        }
        cout << "Record added to history.\n";
    }

    void displayPatientHistory(const string& patId) {
        if (!patients.count(patId)) { cout << "Patient not found.\n"; return; }
        Patient* p = patients[patId];
        cout << "\n--- History: " << p->name << " ---\n";
        MedicalRecord* cur = p->historyHead;
        while (cur) {
            cout << "Date: " << cur->date << " | Doc: " << cur->doctorId << "\n";
            cout << "Sym: " << cur->symptoms << " | Dx: " << cur->diagnosis << " | Rx: " << cur->prescription << "\n";
            cout << "--------------------------------\n";
            cur = cur->next;
        }
    }

    void searchBySymptom(const string& keyword) {
        cout << "\n--- Search Results: " << keyword << " ---\n";
        bool found = false;
        for (auto const& [id, p] : patients) {
            MedicalRecord* cur = p->historyHead;
            while (cur) {
                if (smartSearch(cur->symptoms, keyword)) {
                    cout << "Match: " << p->name << " (ID: " << p->id << ") - " << cur->symptoms << "\n";
                    found = true;
                    break; 
                }
                cur = cur->next;
            }
        }
        if (!found) cout << "No matches found.\n";
    }

    void showDatabase() {
        cout << "\n--- Doctors ---\n";
        for (auto const& [id, d] : doctors) cout << id << ": " << d->name << " (" << d->specialization << ")\n";
        cout << "\n--- Patients ---\n";
        for (auto const& [id, p] : patients) cout << id << ": " << p->name << "\n";
    }

    // Developer: Harsimran (Dijkstra's Algorithm)
    void findShortestReferralPath(const string& startId, const string& endId) {
        if (adjList.find(startId) == adjList.end() || adjList.find(endId) == adjList.end()) {
            cout << "Error: IDs not found in network.\n";
            return;
        }

        set<pair<int, string>> pq;
        unordered_map<string, int> dist;
        unordered_map<string, string> parent;

        for (auto const& [key, val] : adjList) dist[key] = numeric_limits<int>::max();

        dist[startId] = 0;
        pq.insert({0, startId});

        while (!pq.empty()) {
            string u = pq.begin()->second;
            pq.erase(pq.begin());

            if (u == endId) break;

            for (const string& v : adjList[u]) {
                if (dist[u] + 1 < dist[v]) {
                    pq.erase({dist[v], v});
                    dist[v] = dist[u] + 1;
                    parent[v] = u;
                    pq.insert({dist[v], v});
                }
            }
        }

        if (dist[endId] == numeric_limits<int>::max()) {
            cout << "No connection exists between these two.\n";
            return;
        }

        vector<string> path;
        string curr = endId;
        while (curr != startId) {
            path.push_back(curr);
            curr = parent[curr];
        }
        path.push_back(startId);
        reverse(path.begin(), path.end());

        cout << "\n--- Shortest Network Path (Dijkstra) ---\n";
        cout << "Hops: " << dist[endId] << "\n";
        for (size_t i = 0; i < path.size(); ++i) {
            string name = doctors.count(path[i]) ? doctors[path[i]]->name : patients[path[i]]->name;
            string role = doctors.count(path[i]) ? "[Dr]" : "[Pat]";
            cout << (i==0 ? "" : " -> ") << role << " " << name;
        }
        cout << "\n";
    }
};

void clearBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    EHRSystem ehr;
    
    // Sample Data
    ehr.addDoctor("D001", "Ronith", "Cardiologist");
    ehr.addDoctor("D002", "Harsimran", "Dermatologist");
    ehr.addDoctor("D003", "Aryan", "Neurologist");
    ehr.addPatient("P101", "Kapish");
    ehr.addPatient("P102", "Medhansh");
    ehr.addPatient("P103", "John");

    ehr.linkDoctorPatient("D001", "P101"); // Ronith - Kapish
    ehr.linkDoctorPatient("D002", "P101"); // Harsimran - Kapish
    ehr.linkDoctorPatient("D002", "P102"); // Harsimran - Medhansh
    ehr.linkDoctorPatient("D003", "P102"); // Aryan - Medhansh
    ehr.linkDoctorPatient("D003", "P103"); // Aryan - John
    
    ehr.addMedicalRecord("P101", "D001", "2025-10-20", "Chest Pain", "Angina", "Aspirin");

    int choice;
    do {
        cout << "\n=== EHR Console System ===\n";
        cout << "1. Add Doctor\n2. Add Patient\n3. Link Network\n4. Add Record\n";
        cout << "5. View History\n6. Search Symptoms\n7. Show Database\n";
        cout << "8. Referral Path Finder (Dijkstra)\n0. Exit\nChoice: ";
        cin >> choice;
        clearBuffer();

        string id, name, spec, doc, pat, dt, sym, dx, rx;

        switch(choice) {
            case 1:
                cout << "ID: "; getline(cin, id);
                cout << "Name: "; getline(cin, name);
                cout << "Spec: "; getline(cin, spec);
                ehr.addDoctor(id, name, spec);
                break;
            case 2:
                cout << "ID: "; getline(cin, id);
                cout << "Name: "; getline(cin, name);
                ehr.addPatient(id, name);
                break;
            case 3:
                cout << "Doc ID: "; getline(cin, doc);
                cout << "Pat ID: "; getline(cin, pat);
                ehr.linkDoctorPatient(doc, pat);
                break;
            case 4:
                cout << "Pat ID: "; getline(cin, pat);
                cout << "Doc ID: "; getline(cin, doc);
                cout << "Date: "; getline(cin, dt);
                cout << "Sym: "; getline(cin, sym);
                cout << "Dx: "; getline(cin, dx);
                cout << "Rx: "; getline(cin, rx);
                ehr.addMedicalRecord(pat, doc, dt, sym, dx, rx);
                break;
            case 5:
                cout << "Pat ID: "; getline(cin, pat);
                ehr.displayPatientHistory(pat);
                break;
            case 6:
                cout << "Keyword: "; getline(cin, sym);
                ehr.searchBySymptom(sym);
                break;
            case 7:
                ehr.showDatabase();
                break;
            case 8: // Dijkstra Feature
                cout << "Start ID: "; getline(cin, doc);
                cout << "End ID: "; getline(cin, pat);
                ehr.findShortestReferralPath(doc, pat);
                break;
            case 0: cout << "Exiting...\n"; break;
        }
    } while (choice != 0);

    return 0;
}