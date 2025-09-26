
-----

# Miniature Electronic Health Record (EHR) System in C++

Ek console-based application jo healthcare data ko manage karne ke liye core data structures (Graphs, Hash Tables, Doubly Linked Lists) ki power ko demonstrate karta hai.

-----

## 📂 Project Ka Maksad (Purpose)

Aaj ke daur mein healthcare industry mein data bahut tezi se badh raha hai. Doctor-patient ke relationships, aur har patient ki medical history ko aasaani se manage karna ek badi चुनौती (challenge) hai. Yeh project is problem ko solve karne ke liye C++ mein banaya gaya ek chhota sa EHR system hai.

Yeh project dikhata hai ki kaise fundamental data structures ka istemal karke hum ek aesa system bana sakte hain jo scalable, efficient, aur well-organized ho.

-----

## ✨ Features

  - **Doctor aur Patient Management**: Naye doctors aur patients ko system mein add karna.
  - **Doctor-Patient Linking**: Doctors aur patients ke beech many-to-many relationship ko model karna.
  - **Chronological Medical History**: Har patient ki medical visits ka record (date, symptoms, diagnosis) ek timeline mein maintain karna.
  - **Fast Record Access**: Patient ya doctor ko unki unique ID se turant search karna.
  - **Symptom-Based Search**: Aise patients ko dhundhna jinhe ek specific symptom ho.
  - **Menu-Driven Interface**: Command-line par aasan menu ke zariye system ko operate karna.

-----

## 🏗️ System Design aur Data Structures Ka Istemal (The "Why")

Is project ka core iske data structures ka smart selection hai. Har data structure ek specific problem ko solve karne ke liye chuna gaya hai.

### 1\. Hash Table (`std::unordered_map`) ⚡

  - **Kya Ho Raha Hai?**: Hum do hash tables ka istemal kar rahe hain: ek patients ke liye aur ek doctors ke liye. Yeh har unique ID (e.g., "P101") ko uss patient ya doctor ke memory location (pointer) se map karta hai.

  - **Kyu Ho Raha Hai?**: **Speed\!** Jab aapko kisi patient ki details chahiye, toh aapko poori list search karne ki zaroorat nahi hai. Hash table aapko average **O(1)** time mein, yaani instant, uss record tak pahuncha deta hai. Yeh bilkul ek book ke index ki tarah kaam karta hai.

    ```cpp
    // Fast lookup using a key (ID)
    std::unordered_map<std::string, Patient*> patients;
    std::unordered_map<std::string, Doctor*> doctors;
    ```

### 2\. Graph (Adjacency List) 🔗

  - **Kya Ho Raha Hai?**: Hum ek graph data structure ka istemal kar rahe hain jismein doctors aur patients **nodes (vertices)** hain, aur unke beech ka connection ek **edge** hai. Isse Adjacency List ka use karke implement kiya gaya hai.

  - **Kyu Ho Raha Hai?**: Real-world mein, ek patient kai doctors se mil sakta hai, aur ek doctor kai patients ko dekh sakta hai. Yeh ek **many-to-many relationship** hai. Graph is tarah ke complex networks ko represent karne ke liye sabse best data structure hai.

    ```cpp
    // 'adjList' stores the network.
    // Key: Doctor/Patient ID
    // Value: A list of connected IDs
    std::unordered_map<std::string, std::vector<std::string>> adjList;
    ```

### 3\. Doubly Linked List (`MedicalRecord`) 📜

  - **Kya Ho Raha Hai?**: Har `Patient` object ke andar uski medical history ko store karne ke liye ek doubly linked list hai. Har node ek `MedicalRecord` (ek visit) hai, jismein `next` aur `prev` pointers hote hain.

  - **Kyu Ho Raha Hai?**: Patient ki medical history **chronological** (time-based) hoti hai. Linked list is order ko maintain karne ke liye perfect hai. Naye record ko list ke aakhir mein add karna bahut efficient (**O(1)**) hota hai. Doubly linked hone ka fayda yeh hai ki aap history mein aage (latest) aur pichhe (oldest) dono taraf aasaani se jaa sakte hain.

    ```cpp
    struct Patient {
        // ... other details
        MedicalRecord *historyHead; // Points to the first visit
        MedicalRecord *historyTail; // Points to the last visit
    };
    ```

-----

## 🚀 Kaise Compile aur Run Karein

Aapko ek C++ compiler (jaise g++) ki zaroorat hogi.

1.  Code ko ek file mein save karein, jaise `main.cpp`.

2.  Apna terminal ya command prompt open karein.

3.  File jahan save ki hai, uss directory mein jaayein.

4.  Compile karne ke liye yeh command chalayein:

    ```sh
    g++ main.cpp -o ehr_system -std=c++17
    ```

5.  Program ko run karne ke liye yeh command chalayein:

    ```sh
    ./ehr_system
    ```

    Iske baad aapko screen par menu dikhega aur aap system ko use kar sakte hain.

-----

## 💻 Code Ka Structure

`main.cpp` file mein code ko teen hisso mein organize kiya gaya hai:

1.  **Data Structure Definitions**:

      - `struct MedicalRecord`: Ek visit ka data store karta hai (date, symptoms, etc.). Yeh linked list ka node hai.
      - `struct Patient`: Patient ki details aur uski medical history ki list ka head/tail pointer rakhta hai.
      - `struct Doctor`: Doctor ki details store karta hai.

2.  **EHRSystem Class**:

      - Yeh class poore system ka dimaag (brain) hai.
      - Iske andar saare data structures (hash tables, graph) aur unko manage karne wale functions (addDoctor, addMedicalRecord, etc.) hain.
      - Memory management ke liye Destructor (`~EHRSystem`) bhi ismein hai.

3.  **main() Function**:

      - Yeh program ka entry point hai.
      - Yeh user ko menu dikhata hai aur user ke input ke hisaab se `EHRSystem` class ke functions ko call karta hai.
      - Yeh User Interface (UI) ka kaam karta hai.