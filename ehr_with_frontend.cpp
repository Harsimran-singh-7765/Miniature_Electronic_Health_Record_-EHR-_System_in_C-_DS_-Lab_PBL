#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/fl_message.H>

// ======================================================================
// 1. BACKEND: EHR System (Core Logic)
// ======================================================================

struct MedicalRecord {
    std::string date;
    std::string symptoms;
    std::string diagnosis;
    std::string prescription;
    MedicalRecord *next;
    MedicalRecord *prev;

    MedicalRecord(std::string dt, std::string sym, std::string dx, std::string px)
        : date(dt), symptoms(sym), diagnosis(dx), prescription(px), next(nullptr), prev(nullptr) {}
};

struct Patient {
    std::string id;
    std::string name;
    MedicalRecord *historyHead;
    MedicalRecord *historyTail;

    Patient(std::string patientId, std::string patientName)
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

struct Doctor {
    std::string id;
    std::string name;
    std::string specialization;

    Doctor(std::string docId, std::string docName, std::string spec)
        : id(docId), name(docName), specialization(spec) {}
};

// ----------------------------------------------------------------------

class EHRSystem {
private:
    std::unordered_map<std::string, Patient*> patients;
    std::unordered_map<std::string, Doctor*> doctors;
    std::unordered_map<std::string, std::vector<std::string>> adjList;

public:
    ~EHRSystem() {
        for (auto& [id, p] : patients) delete p;
        for (auto& [id, d] : doctors) delete d;
    }

    void addDoctor(const std::string& id, const std::string& name, const std::string& spec) {
        if (doctors.count(id)) {
            std::string msg = "Doctor ID " + id + " already exists.";
            fl_message("%s", msg.c_str());
            return;
        }
        Doctor* newDoctor = new Doctor(id, name, spec);
        doctors[id] = newDoctor;
        adjList[id] = {};
        std::string msg = "Doctor " + name + " added successfully.";
        fl_message("%s", msg.c_str());
    }

    void addPatient(const std::string& id, const std::string& name) {
        if (patients.count(id)) {
            std::string msg = "Patient ID " + id + " already exists.";
            fl_message("%s", msg.c_str());
            return;
        }
        Patient* newPatient = new Patient(id, name);
        patients[id] = newPatient;
        adjList[id] = {};
        std::string msg = "Patient " + name + " added successfully.";
        fl_message("%s", msg.c_str());
    }

    void linkDoctorPatient(const std::string& docId, const std::string& patId) {
        if (!doctors.count(docId) || !patients.count(patId)) {
            fl_message("Invalid Doctor or Patient ID.");
            return;
        }
        adjList[docId].push_back(patId);
        adjList[patId].push_back(docId);
        std::string msg = "Linked Doctor " + docId + " with Patient " + patId;
        fl_message("%s", msg.c_str());
    }

    void addMedicalRecord(const std::string& patId, const std::string& date,
                          const std::string& sym, const std::string& dx, const std::string& px) {
        if (!patients.count(patId)) {
            fl_message("Patient not found.");
            return;
        }

        Patient* patient = patients[patId];
        MedicalRecord* newRec = new MedicalRecord(date, sym, dx, px);

        if (!patient->historyHead) {
            patient->historyHead = newRec;
            patient->historyTail = newRec;
        } else {
            patient->historyTail->next = newRec;
            newRec->prev = patient->historyTail;
            patient->historyTail = newRec;
        }

        std::string msg = "Medical record added for patient " + patient->name;
        fl_message("%s", msg.c_str());
    }

    std::string getPatientHistory(const std::string& patId) {
        std::ostringstream oss;

        if (!patients.count(patId))
            return "Patient not found.";

        Patient* p = patients[patId];
        if (!p->historyHead)
            return "No medical records for " + p->name;

        oss << "Medical History for " << p->name << " (" << p->id << ")\n";
        oss << "-------------------------------------------\n";
        MedicalRecord* cur = p->historyHead;
        int count = 1;
        while (cur) {
            oss << "Record " << count++ << ":\n";
            oss << "Date: " << cur->date << "\n";
            oss << "Symptoms: " << cur->symptoms << "\n";
            oss << "Diagnosis: " << cur->diagnosis << "\n";
            oss << "Prescription: " << cur->prescription << "\n";
            oss << "-------------------------------------------\n";
            cur = cur->next;
        }
        return oss.str();
    }

    std::string findPatientsBySymptom(const std::string& symptom) {
        std::ostringstream oss;
        std::string lowerSym = symptom;
        std::transform(lowerSym.begin(), lowerSym.end(), lowerSym.begin(), ::tolower);

        bool found = false;
        for (auto& [id, p] : patients) {
            MedicalRecord* cur = p->historyHead;
            while (cur) {
                std::string sym = cur->symptoms;
                std::transform(sym.begin(), sym.end(), sym.begin(), ::tolower);
                if (sym.find(lowerSym) != std::string::npos) {
                    oss << "- " << p->name << " (ID: " << p->id << ")\n";
                    found = true;
                    break;
                }
                cur = cur->next;
            }
        }

        if (!found)
            return "No patients found with symptom: " + symptom;
        return "Patients with symptom '" + symptom + "':\n" + oss.str();
    }
};

// ======================================================================
// 2. FRONTEND: FLTK GUI
// ======================================================================

EHRSystem ehr;

// Callbacks
void addRecordCallback(Fl_Widget*, void* data) {
    Fl_Input** in = (Fl_Input**)data;
    ehr.addMedicalRecord(in[0]->value(), in[1]->value(), in[2]->value(), in[3]->value(), in[4]->value());
}

void viewHistoryCallback(Fl_Widget*, void* data) {
    Fl_Input* in = (Fl_Input*)data;
    std::string result = ehr.getPatientHistory(in->value());
    fl_message("%s", result.c_str());
}

void findSymptomCallback(Fl_Widget*, void* data) {
    Fl_Input* in = (Fl_Input*)data;
    std::string result = ehr.findPatientsBySymptom(in->value());
    fl_message("%s", result.c_str());
}

// ======================================================================
// 3. MAIN FUNCTION: GUI Setup
// ======================================================================

int main() {
    Fl_Window *win = new Fl_Window(700, 850, "Mini EHR System (by Kapish)");

    // --- Add Doctor ---
    Fl_Box *doctorLabel = new Fl_Box(20, 20, 200, 25, "Add Doctor");
    Fl_Input *docId = new Fl_Input(100, 50, 150, 25, "ID:");
    Fl_Input *docName = new Fl_Input(100, 80, 150, 25, "Name:");
    Fl_Input *docSpec = new Fl_Input(100, 110, 150, 25, "Specialization:");
    Fl_Button *addDocBtn = new Fl_Button(100, 140, 150, 30, "Add Doctor");
    Fl_Input* doctorInputs[3] = {docId, docName, docSpec};
    addDocBtn->callback([](Fl_Widget*, void* data){
        Fl_Input** in = (Fl_Input**)data;
        ehr.addDoctor(in[0]->value(), in[1]->value(), in[2]->value());
    }, doctorInputs);

    // --- Add Patient ---
    Fl_Box *patientLabel = new Fl_Box(20, 190, 200, 25, "Add Patient");
    Fl_Input *patId = new Fl_Input(100, 220, 150, 25, "ID:");
    Fl_Input *patName = new Fl_Input(100, 250, 150, 25, "Name:");
    Fl_Button *addPatBtn = new Fl_Button(100, 280, 150, 30, "Add Patient");
    Fl_Input* patientInputs[2] = {patId, patName};
    addPatBtn->callback([](Fl_Widget*, void* data){
        Fl_Input** in = (Fl_Input**)data;
        ehr.addPatient(in[0]->value(), in[1]->value());
    }, patientInputs);

    // --- Link Doctor & Patient ---
    Fl_Box *linkLabel = new Fl_Box(300, 20, 200, 25, "Link Doctor & Patient");
    Fl_Input *linkDocId = new Fl_Input(380, 50, 120, 25, "Doc ID:");
    Fl_Input *linkPatId = new Fl_Input(380, 80, 120, 25, "Pat ID:");
    Fl_Button *linkBtn = new Fl_Button(380, 110, 120, 30, "Link");
    Fl_Input* linkInputs[2] = {linkDocId, linkPatId};
    linkBtn->callback([](Fl_Widget*, void* data){
        Fl_Input** in = (Fl_Input**)data;
        ehr.linkDoctorPatient(in[0]->value(), in[1]->value());
    }, linkInputs);

    // --- Add Medical Record ---
    Fl_Box *recLabel = new Fl_Box(300, 190, 200, 25, "Add Medical Record");
    Fl_Input *recPatId = new Fl_Input(380, 220, 120, 25, "Pat ID:");
    Fl_Input *recDate = new Fl_Input(380, 250, 120, 25, "Date:");
    Fl_Input *recSym = new Fl_Input(380, 280, 120, 25, "Symptoms:");
    Fl_Input *recDx = new Fl_Input(380, 310, 120, 25, "Diagnosis:");
    Fl_Input *recPx = new Fl_Input(380, 340, 120, 25, "Prescription:");
    Fl_Button *recBtn = new Fl_Button(380, 370, 120, 30, "Add Record");
    Fl_Input* recInputs[5] = {recPatId, recDate, recSym, recDx, recPx};
    recBtn->callback(addRecordCallback, recInputs);

    // --- View History ---
    Fl_Input *viewPatId = new Fl_Input(100, 360, 150, 25, "Pat ID:");
    Fl_Button *viewBtn = new Fl_Button(100, 390, 150, 30, "View History");
    viewBtn->callback(viewHistoryCallback, viewPatId);

    // --- Find by Symptom ---
    Fl_Input *symInput = new Fl_Input(100, 440, 150, 25, "Symptom:");
    Fl_Button *symBtn = new Fl_Button(100, 470, 150, 30, "Find Patients");
    symBtn->callback(findSymptomCallback, symInput);

    win->end();
    win->show();
    return Fl::run();
}
