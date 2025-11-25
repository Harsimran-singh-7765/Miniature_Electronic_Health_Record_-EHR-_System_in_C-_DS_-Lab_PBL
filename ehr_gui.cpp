#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <iomanip> 
#include <queue> 
#include <limits> 
#include <set>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/fl_message.H>
#include <FL/fl_draw.H> 
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

using namespace std;

/*
 * PROJECT: ADVANCED EHR SYSTEM v3.0 (DIJKSTRA EDITION)
 * ---------------------------------------------------------
 * DEVELOPER CONTRIBUTIONS:
 * [1] Harsimran (Lead): Core Architecture & Algorithms
 * [2] Kapish: Patient Data Structures 
 * [3] Medhansh: Doctor Registry & Network Linkage 
 * [4] Harsimran: Advanced Analytics (Dijkstra Implementation)
 * [5] Ronith: Frontend GUI & Event Handling
 * ---------------------------------------------------------
 */

// ======================================================================
// MODULE 1: PATIENT DATA STRUCTURES (Kapish)
// ======================================================================

struct MedicalRecord {
    string date, symptoms, diagnosis, prescription, doctorId;
    MedicalRecord *next;
    MedicalRecord *prev;
    MedicalRecord(string dt, string sym, string dx, string px, string docId)
        : date(dt), symptoms(sym), diagnosis(dx), prescription(px), doctorId(docId), next(nullptr), prev(nullptr) {}
};

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

// ======================================================================
// MODULE 2: PROVIDER STRUCTURES (Medhansh)
// ======================================================================

struct Doctor {
    string id, name, specialization;
    Doctor(string docId, string docName, string spec)
        : id(docId), name(docName), specialization(spec) {}
};

// ======================================================================
// MODULE 3: CORE SYSTEM ARCHITECTURE (Harsimran & Aryan)
// ======================================================================

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
        for (auto& pair : patients) delete pair.second;
        for (auto& pair : doctors) delete pair.second;
    }
    
    void addDoctor(const string& id, const string& name, const string& spec) {
        if (doctors.count(id)) { fl_message("Error: Doctor ID %s already exists.", id.c_str()); return; }
        doctors[id] = new Doctor(id, name, spec);
        adjList[id] = {};
        fl_message("Success: Doctor %s registered.", name.c_str());
    }

    void addPatient(const string& id, const string& name) {
        if (patients.count(id)) { fl_message("Error: Patient ID %s already exists.", id.c_str()); return; }
        patients[id] = new Patient(id, name);
        adjList[id] = {};
        fl_message("Success: Patient %s registered.", name.c_str());
    }

    void linkDoctorPatient(const string& docId, const string& patId) {
        if (!doctors.count(docId) || !patients.count(patId)) { fl_message("Error: Invalid IDs."); return; }
        adjList[docId].push_back(patId);
        adjList[patId].push_back(docId);
        fl_message("Network: Linked %s with %s", docId.c_str(), patId.c_str());
    }

    void addMedicalRecord(const string& patId, const string& date,
                          const string& sym, const string& dx, const string& px,
                          const string& docId) {
        if (!patients.count(patId)) { fl_message("Error: Patient not found."); return; }
        Patient* patient = patients.at(patId);
        MedicalRecord* newRec = new MedicalRecord(date, sym, dx, px, docId);

        if (!patient->historyHead) {
            patient->historyHead = newRec;
            patient->historyTail = newRec;
        } else {
            patient->historyTail->next = newRec;
            newRec->prev = patient->historyTail;
            patient->historyTail = newRec;
        }
        fl_message("Clinical Note: Record added for %s", patient->name.c_str());
    }

    string getPatientHistory(const string& patId) {
        ostringstream oss;
        if (!patients.count(patId)) return "System: Patient not found.";
        Patient* p = patients.at(patId);
        if (!p->historyHead) return "System: No medical records found.";

        oss << "CLINICAL HISTORY REPORT: " << p->name << " (ID: " << p->id << ")\n";
        oss << "========================================================\n";
        MedicalRecord* cur = p->historyHead;
        int count = 1;
        while (cur) {
            oss << "RECORD #" << count++ << "  [Date: " << cur->date << "]\n";
            oss << "  Attending Physician ID : " << cur->doctorId << "\n";
            oss << "  Presented Symptoms     : " << cur->symptoms << "\n";
            oss << "  Clinical Diagnosis     : " << cur->diagnosis << "\n";
            oss << "  Prescribed Treatment   : " << cur->prescription << "\n";
            oss << "--------------------------------------------------------\n";
            cur = cur->next;
        }
        return oss.str();
    }

    string findPatientsByKeyword(const string& keyword) {
        ostringstream oss;
        if (keyword.empty()) return "System: Please enter a search term.";
        oss << "SEARCH RESULTS FOR: '" << keyword << "'\n";
        oss << "==========================================\n";
        bool found = false;
        for (auto& pair : patients) {
            Patient* p = pair.second;
            MedicalRecord* cur = p->historyHead;
            while (cur) {
                if (smartSearch(cur->symptoms, keyword) || smartSearch(cur->diagnosis, keyword)) {
                    oss << "[MATCH] Patient: " << p->name << " (ID: " << p->id << ")\n";
                    oss << "        Date: " << cur->date << " | Dx: " << cur->diagnosis << "\n";
                    found = true;
                    break; 
                }
                cur = cur->next;
            }
        }
        return found ? oss.str() : "System: No records found.";
    }

    string getAllDataInTable() {
        ostringstream oss;
        oss << left;
        oss << "=== REGISTERED PHYSICIANS ===\n";
        oss << setw(12) << "ID" << setw(25) << "Name" << "Specialization\n";
        oss << "------------------------------------------------------------\n";
        for (const auto& pair : doctors) oss << setw(12) << pair.second->id << setw(25) << pair.second->name << pair.second->specialization << "\n";
        
        oss << "\n=== REGISTERED PATIENTS ===\n";
        oss << setw(12) << "ID" << "Name\n";
        oss << "------------------------------------------------------------\n";
        for (const auto& pair : patients) oss << setw(12) << pair.second->id << pair.second->name << "\n";
        return oss.str();
    }
    
    string getLinkTree() {
        ostringstream oss;
        oss << "--- NETWORK LINKAGE TREE ---\n";
        for (const auto& pair : doctors) {
            if (!adjList.at(pair.first).empty()) {
                oss << "\n[DR] " << pair.second->name << "\n";
                for (const auto& pid : adjList.at(pair.first)) {
                    oss << "  |--> [PAT] " << patients.at(pid)->name << "\n";
                }
            }
        }
        return oss.str();
    }

    // --- DIJKSTRA'S ALGORITHM IMPLEMENTATION (Harsimran) ---
   
    string findShortestPath(const string& startId, const string& endId) {
        if (adjList.find(startId) == adjList.end() || adjList.find(endId) == adjList.end()) {
            return "Error: Start or End ID does not exist in the network.";
        }

        // Priority Queue for Dijkstra: {distance, nodeId}
        // Using set to act as Min-Priority Queue for simplicity in C++
        set<pair<int, string>> pq;
        unordered_map<string, int> dist;
        unordered_map<string, string> parent;

        // Initialize distances
        for (auto const& [key, val] : adjList) dist[key] = numeric_limits<int>::max();
        
        dist[startId] = 0;
        pq.insert({0, startId});

        while (!pq.empty()) {
            string u = pq.begin()->second;
            pq.erase(pq.begin());

            if (u == endId) break; // Found target

            for (const string& v : adjList[u]) {
                if (dist[u] + 1 < dist[v]) {
                    pq.erase({dist[v], v});
                    dist[v] = dist[u] + 1;
                    parent[v] = u;
                    pq.insert({dist[v], v});
                }
            }
        }

        if (dist[endId] == numeric_limits<int>::max()) 
            return "No connection found between " + startId + " and " + endId;

        // Reconstruct Path
        vector<string> path;
        string curr = endId;
        while (curr != startId) {
            path.push_back(curr);
            curr = parent[curr];
        }
        path.push_back(startId);
        reverse(path.begin(), path.end());

        // Format Output
        ostringstream oss;
        oss << "SHORTEST REFERRAL CHAIN (" << dist[endId] << " hops):\n";
        oss << "------------------------------------------\n";
        for (size_t i = 0; i < path.size(); ++i) {
            string id = path[i];
            string name = (doctors.count(id)) ? doctors[id]->name : patients[id]->name;
            string role = (doctors.count(id)) ? "[Doctor]" : "[Patient]";
            
            if (i > 0) oss << "   |\n   v\n";
            oss << role << " " << name << " (" << id << ")\n";
        }
        return oss.str();
    }
};


// FRONTEND UTILITIES (Ronith)

EHRSystem ehr;

void createReportWindow(const char* title, const string& content) {
    Fl_Window* win = new Fl_Window(550, 450, title);
    Fl_Text_Buffer* buff = new Fl_Text_Buffer();
    buff->text(content.c_str());
    Fl_Text_Display* disp = new Fl_Text_Display(10, 10, 530, 430);
    disp->buffer(buff);
    disp->textfont(FL_COURIER);
    disp->textsize(13);
    win->resizable(disp);
    win->end();
    win->show();
}

void addRecordCallback(Fl_Widget*, void* data) {
    Fl_Input** in = (Fl_Input**)data;
    // Map inputs: PatID, Date, Sym, Dx, Rx, DocID
    ehr.addMedicalRecord(in[0]->value(), in[2]->value(), in[3]->value(), in[4]->value(), in[5]->value(), in[1]->value());
}

void viewHistoryCallback(Fl_Widget*, void* data) {
    Fl_Input* in = (Fl_Input*)data;
    createReportWindow("Medical History Report", ehr.getPatientHistory(in->value()));
}

void smartSearchCallback(Fl_Widget*, void* data) {
    Fl_Input* in = (Fl_Input*)data;
    createReportWindow("Symptom Search Results", ehr.findPatientsByKeyword(in->value()));
}

void showAllDataCallback(Fl_Widget*, void*) {
    createReportWindow("Master Database View", ehr.getAllDataInTable());
}

void showLinkTreeCallback(Fl_Widget*, void*) {
    createReportWindow("Physician-Patient Network", ehr.getLinkTree());
}

void findPathCallback(Fl_Widget*, void* data) {
    Fl_Input** in = (Fl_Input**)data;
    string start = in[0]->value();
    string end = in[1]->value();
    createReportWindow("Referral Path Analysis", ehr.findShortestPath(start, end));
}

// MAIN  LOOP (Harsimran)

int main() {
    Fl::scheme("gtk+"); 
    Fl::set_color(FL_BACKGROUND_COLOR, 0xF2F2F200);

    Fl_Window *win = new Fl_Window(700, 750, "Integrated EHR Management System v3.0");

    const int PADDING = 20;
    const int WIDGET_H = 28;
    const int BUTTON_H = 38;
    const int LABEL_W = 140; 
    const int INPUT_W = 200;
    const int COL_GAP = 60;
    int x_left = PADDING; 
    int y = PADDING;

    auto createInput = [&](const char* lbl, int& y_pos) {
        Fl_Input* inp = new Fl_Input(x_left + LABEL_W, y_pos, INPUT_W, WIDGET_H, lbl);
        inp->textsize(13);
        y_pos += WIDGET_H + 8; 
        return inp;
    };
    
    // --- COLUMN 1: ADMINISTRATIVE ACTIONS ---

    // 1. Doctor Registration
    Fl_Box* h1 = new Fl_Box(FL_NO_BOX, x_left, y, 200, 25, "Physician Registration"); 
    h1->labelfont(FL_BOLD); h1->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE); y+=30;
    
    Fl_Input* d1 = createInput("Doctor ID:", y);
    Fl_Input* d2 = createInput("Full Name:", y);
    Fl_Input* d3 = createInput("Specialization:", y);
    
    Fl_Button* b1 = new Fl_Button(x_left, y, LABEL_W+INPUT_W, BUTTON_H, "Register Doctor");
    b1->color(FL_DARK_CYAN); b1->labelcolor(FL_WHITE); y+=BUTTON_H+15;
    
    static Fl_Input* dIn[] = {d1,d2,d3};
    b1->callback([](Fl_Widget*,void*){ ehr.addDoctor(dIn[0]->value(), dIn[1]->value(), dIn[2]->value()); });

    // 2. Patient Registration
    Fl_Box* h2 = new Fl_Box(FL_NO_BOX, x_left, y, 200, 25, "Patient Registration"); 
    h2->labelfont(FL_BOLD); h2->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE); y+=30;
    
    Fl_Input* p1 = createInput("Patient ID:", y);
    Fl_Input* p2 = createInput("Full Name:", y);
    
    Fl_Button* b2 = new Fl_Button(x_left, y, LABEL_W+INPUT_W, BUTTON_H, "Register Patient");
    b2->color(FL_DARK_CYAN); b2->labelcolor(FL_WHITE); y+=BUTTON_H+15;
    
    static Fl_Input* pIn[] = {p1,p2};
    b2->callback([](Fl_Widget*,void*){ ehr.addPatient(pIn[0]->value(), pIn[1]->value()); });

    // 3. Network Linkage
    Fl_Box* h3 = new Fl_Box(FL_NO_BOX, x_left, y, 200, 25, "Network Assignment"); 
    h3->labelfont(FL_BOLD); h3->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE); y+=30;
    
    Fl_Input* l1 = createInput("Doctor ID:", y);
    Fl_Input* l2 = createInput("Patient ID:", y);
    
    Fl_Button* b3 = new Fl_Button(x_left, y, LABEL_W+INPUT_W, BUTTON_H, "Assign Doctor to Patient");
    b3->color(FL_GRAY); y+=BUTTON_H+20;
    
    static Fl_Input* lIn[] = {l1,l2};
    b3->callback([](Fl_Widget*,void*){ ehr.linkDoctorPatient(lIn[0]->value(), lIn[1]->value()); });

    // Global Admin Buttons
    Fl_Button* bAll = new Fl_Button(x_left, y, 165, BUTTON_H, "Full Database");
    bAll->color(FL_DARK_YELLOW); bAll->callback(showAllDataCallback);
    
    Fl_Button* bTree = new Fl_Button(x_left + 175, y, 165, BUTTON_H, "Network Tree");
    bTree->color(FL_DARK_GREEN); bTree->labelcolor(FL_WHITE); bTree->callback(showLinkTreeCallback);
    
    int max_y_left = y + BUTTON_H;

    // --- COLUMN 2: CLINICAL ACTIONS ---
    int x_right = x_left + LABEL_W + INPUT_W + COL_GAP; 
    y = PADDING;

    // 4. Clinical Entry
    Fl_Box* h4 = new Fl_Box(FL_NO_BOX, x_right, y, 250, 25, "Clinical Record Entry"); 
    h4->labelfont(FL_BOLD); h4->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE); y+=30;
    
    Fl_Input* r1 = new Fl_Input(x_right+LABEL_W, y, INPUT_W, WIDGET_H, "Patient ID:"); y+=WIDGET_H+8;
    Fl_Input* r2 = new Fl_Input(x_right+LABEL_W, y, INPUT_W, WIDGET_H, "Doctor ID:"); y+=WIDGET_H+8;
    Fl_Input* r3 = new Fl_Input(x_right+LABEL_W, y, INPUT_W, WIDGET_H, "Date (YYYY-MM-DD):"); y+=WIDGET_H+8;
    Fl_Input* r4 = new Fl_Input(x_right+LABEL_W, y, INPUT_W, WIDGET_H, "Symptoms:"); y+=WIDGET_H+8;
    Fl_Input* r5 = new Fl_Input(x_right+LABEL_W, y, INPUT_W, WIDGET_H, "Diagnosis:"); y+=WIDGET_H+8;
    Fl_Input* r6 = new Fl_Input(x_right+LABEL_W, y, INPUT_W, WIDGET_H, "Prescription:"); y+=WIDGET_H+8;
    
    Fl_Button* bRec = new Fl_Button(x_right, y, LABEL_W+INPUT_W, BUTTON_H, "Submit Clinical Record");
    bRec->color(FL_DARK_BLUE); bRec->labelcolor(FL_WHITE); y+=BUTTON_H+25;
    
    static Fl_Input* rIn[] = {r1,r2,r3,r4,r5,r6};
    bRec->callback(addRecordCallback, rIn);

    // Separator
    Fl_Box* line = new Fl_Box(FL_FLAT_BOX, x_right, y, LABEL_W+INPUT_W, 2, ""); 
    line->color(FL_GRAY); y+=15;

    // 5. Search & Analytics
    Fl_Box* h5 = new Fl_Box(FL_NO_BOX, x_right, y, 250, 25, "Search & Analytics"); 
    h5->labelfont(FL_BOLD); h5->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE); y+=30;

    Fl_Input* q1 = new Fl_Input(x_right+LABEL_W, y, INPUT_W, WIDGET_H, "Patient ID:"); y+=WIDGET_H+8;
    Fl_Button* bHist = new Fl_Button(x_right, y, LABEL_W+INPUT_W, BUTTON_H, "View Medical History");
    bHist->callback(viewHistoryCallback, q1); y+=BUTTON_H+15;
    
    Fl_Input* q2 = new Fl_Input(x_right+LABEL_W, y, INPUT_W, WIDGET_H, "Symptom Keyword:"); y+=WIDGET_H+8;
    Fl_Button* bSearch = new Fl_Button(x_right, y, LABEL_W+INPUT_W, BUTTON_H, "Smart Symptom Search");
    bSearch->color(FL_DARK_MAGENTA); bSearch->labelcolor(FL_WHITE);
    bSearch->callback(smartSearchCallback, q2); y+=BUTTON_H;

    // 6. Network Path Finder (Dijkstra)
    Fl_Box* h6 = new Fl_Box(FL_NO_BOX, x_right, y, 250, 25, "Referral Path Finder (Dijkstra)"); 
    h6->labelfont(FL_BOLD); h6->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE); y+=30;

    Fl_Input* pathStart = new Fl_Input(x_right+LABEL_W, y, INPUT_W, WIDGET_H, "Source ID:"); y+=WIDGET_H+8;
    Fl_Input* pathEnd = new Fl_Input(x_right+LABEL_W, y, INPUT_W, WIDGET_H, "Target ID:"); y+=WIDGET_H+8;
    
    Fl_Button* bPath = new Fl_Button(x_right, y, LABEL_W+INPUT_W, BUTTON_H, "Find Shortest Path");
    bPath->color(FL_DARK_RED); bPath->labelcolor(FL_WHITE);
    
    static Fl_Input* pathIn[] = {pathStart, pathEnd};
    bPath->callback(findPathCallback, pathIn);
    y+=BUTTON_H;

    // Window Setup
    int final_h = max(max_y_left, y) + PADDING;
    int final_w = x_right + LABEL_W + INPUT_W + PADDING;
    
    win->size(final_w, final_h);
    win->resizable(0); 

    win->end();
    win->show();



    ehr.addDoctor("D001", "Dr. Ronith", "Cardiologist");
    ehr.addDoctor("D002", "Dr. Harsimran", "Dermatologist");
    ehr.addDoctor("D003", "Dr. Aryan", "Neurologist");
    ehr.addDoctor("D004", "Dr. Stranger", "Surgeon");
    
   
    ehr.addPatient("P101", "Kapish S.");
    ehr.addPatient("P102", "Medhansh G.");
    ehr.addPatient("P103", "John Doe");


    ehr.linkDoctorPatient("D001", "P101"); 
    
    ehr.linkDoctorPatient("D002", "P101"); 
    
   
    ehr.linkDoctorPatient("D002", "P102");
   
    ehr.linkDoctorPatient("D003", "P102");
    
    ehr.linkDoctorPatient("D003", "P103");
   
    ehr.linkDoctorPatient("D004", "P103");
    
    
    ehr.addMedicalRecord("P101", "2025-09-20", "Chest Pain", "Angina", "Aspirin", "D001");
    ehr.addMedicalRecord("P101", "2025-09-25", "Rash", "Eczema", "Cream", "D002");
    return Fl::run();
}