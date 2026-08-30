#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <windows.h>
using namespace std;

#define MAX 100
#define MIN_ACC_NO 10000
#define MAX_ACC_NO 99999
#define encrypt_key 3
#define MIN_DEPOSIT 5000
const string fileName = "database.csv";
const string cardFile = "pin.code";



struct Account{
    int accNo;
    double balance;
    string accName, birthday, contact, pin;
};

class Module{
    private:
        Account data[MAX];
        int last;
        string currentCard;
        int generateAccNo(); // pang generate ng random acc number
        void makenull();
        bool isFull();
        bool isEmpty();
        int locate(int accNo);
        void save();
        void retrieve();

        // Para sa encrytion keneme
        string encryptPIN(string pin);
        string decryptPIN(string pin);
        string generatePin();

        // Sa flash drive naman to
        bool writeCard(const Account &acc, string drivePATH);
        bool readCard(string drivePATH, int &accNo, string &pin);

    public:
        void Enrollment();
        void Balcheck();
        void Withdraw();
        void Deposit();
        void transfer();
        void changePIN();
};


// Input checker
int inputInt(string prompt);
double inputDouble(string prompt);
string inputPin(string prompt);
string inputDriveLetter();
int flashDriveDetector(string drive[]);

int inputInt(string prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a whole number!" << endl;
        }
        else {
            cin.ignore(1000, '\n');
            return value;
        }
    }
}

int Module :: generateAccNo(){
    srand(time(0));

    int x = 10000; //Start
    int y = 99999; //End
    int accountnum = x + rand() % (y - x + 1);

    return accountnum;
}

double inputDouble(string prompt) {
    double value;
    while(true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input, Please enter avalid amount!" << endl;
        }
        else {
            cin.ignore(1000, '\n');
            return value;
        }
    }
}

string inputPin(string prompt) {
    string pin;
    bool valid;
    
    do {
        cout << prompt;
        getline(cin, pin);
        valid = true;
        if (pin.length() < 4 || pin.length() > 6) {
            valid = false;
            cout << "PIN must be at least 4 to 6 digits only!" << endl;
        }
        else {
            for (int i = 9; i < (int)pin.length(); i++) {
                if (!isdigit(pin[i])) {
                    valid = false;
                    cout << "PIN must contain digits only!" << endl;
                    break;
                }
            }
        }
    } while (!valid);
    return pin;
}

string inputDriveLetter() {
    string drives[26];  //26 kasi yung letter sa alphabet
    int driveCount = flashDriveDetector(drives);

    //  check nya muna if may flash drive ka, then bibigyan ka option. 
    //  If 1 pinili mo, then sa mismong flash drive na sya mag write no need to type it manually.
    //  If 2 naman, type mo sya manually
    if (driveCount > 0) {
        cout << "Detected flash drive(s): " << endl;
        for (int i = 0; i < driveCount; i++) {
            cout << i+1 << ". " << drives[i] << endl;
        }
        cout << driveCount + 1 << ". Type a location manually" << endl;

        int  choice = inputInt("Select an option: ");
        if (choice >= 1 && choice <= driveCount) {
            return drives[choice - 1];
        }
    }
    else {
        cout << "No Flash Drive detected!" << endl;
    }

    // mapupunta lang dito if wala na detect na flash drive, meaning ikaw mismo mag t-type ng folder path
    string input;
    bool valid;
    do {
        cout << "Enter drive letter or folder path for your card" << endl;
        cout << "(e.g. C for a real flash drive, or C:\\ATM_CARD\\ for folder path): ";
        getline(cin, input);
        valid = !input.empty();
        if (!valid) {
            cout << "Input cannot be blank!" << endl;
        }
    }   while (!valid);

    if (input.length() == 1 && isalpha(input[0])) {
        return input + ":\\";
    }
    char lastLetter = input[input.length() - 1];
    if (lastLetter != '\\' && lastLetter != '/') {
        input = input + "\\";
    }
    return input;
}

int flashDriveDetector (string drive[]) {
    int count = 0;
    DWORD driveMask = GetLogicalDrives();   // yung DWORD same idea lang sa unsigned int
    for (int i = 0; i < 26; i++) {
        if (driveMask & (1 << i)) {
            char letter = (char)('A' + i);
            string path = string(1, letter) + ":\\";
            UINT type = GetDriveTypeA(path.c_str()); // para malaman anong type ng drive yun, e.g hard disk, usb drive, etc.
            if (type == DRIVE_REMOVABLE) {
                drive[count] = path;
                count++;
            }
        }
    }
    return count;
}

void Module::makenull() {
    last = -1;
}

bool Module::isFull() {
    return (last == MAX - 1);
}

bool Module::isEmpty() {
    return (last == -1);
}

int Module::locate(int accNo){
    for (int i = 0; i <= last; i++) {
        if (data[i].accNo == accNo) {
            return i;
        }
    }
    return -1;
}

void Module::save() {
    ofstream fp(fileName);
    for (int i = 0; i <= last; i++){
        fp << data[i].accName << ","
            << data[i].accNo << ","
            << data[i].birthday << ","
            << data[i].contact << ","
            << data[i].balance << ","
            << encryptPIN(data[i].pin) << endl;
    }
    fp.close();
}

void Module::retrieve() {
    ifstream fp(fileName);
    if (!fp) {
        cout << "No registered account!" << endl;
        system("pause");
        return;
    }
    string line;
    while (getline(fp, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);
        string converter; // para ma convert yung string value into number
        Account acc;

        getline(ss, acc.accName, ',');
        getline(ss, converter, ',');
        acc.accNo = atoi(converter.c_str()); //para ma convert na yung text into actual integer/number
        getline(ss, acc.birthday, ',');
        getline(ss, acc.contact, ',');
        getline(ss, converter, ',');
        acc.balance = atof(converter.c_str()); // same para ma convert naman yung text into double 
        getline(ss, converter, ',');
        acc.pin = decryptPIN(converter);

        last++;
        data[last] = acc;
    }
    fp.close();
}

// ETO NA PARA SA ENCRYPTION NATIN
string Module::encryptPIN(string pin) {
    string encrypted = pin;
    for (int i = 0; i < (int)encrypted.length(); i++) {
        int digit = encrypted[i] - '0';
        digit = (digit + encrypt_key) % 10;
        encrypted[i] = (char)(digit + '0');
    }
    return encrypted;
}

string Module::decryptPIN(string pin) {
    string decrypted = pin;
    for (int i = 0; i < (int)decrypted.length(); i++) {
        int digit = decrypted[i] - '0';
        digit = (digit - encrypt_key + 10) % 10;
        decrypted[i] = (char)(digit + '0');
    }
    return decrypted;
}

string Module::generatePin() {
    string pin = "";
    for (int i = 0; i < 6; i++)  {
        pin = pin + (char)('0' + rand() % 10);
    }
    return pin;
}

bool Module::writeCard(const Account &acc, string drivePATH) {
    ofstream card((drivePATH + cardFile).c_str());
    if (!card) {
        cout << "Unable to write to the flash drive. Please try again later!" << endl;
        return false;
    }
    card << acc.accNo << "," << encryptPIN(acc.pin) << endl;
    card.close();
    cout << "ATM card saved to drive " << drivePATH << endl;
    return true;
}

bool Module::readCard(string drivePATH, int &accNo, string &pin) {
    if (!filesystem::exists(drivePATH)) {
        cout << "Drive not found. Please try again later!" << endl;
        return false;
    }

    ifstream card((drivePATH + cardFile).c_str());
    if (!card) {
        return false;
    }

    string line;
    getline(card, line);
    card.close();
    if (line.empty()) {
        return false;
    }

    stringstream ss(line);
    string converter;
    getline(ss, converter, ',');
    accNo = atoi(converter.c_str());
    getline(ss, converter, ',');
    pin = decryptPIN(converter);

    return true;
}

void Module :: Enrollment(){
    system("cls");
    if (isFull()) {
        cout << "Registration failed. Maximum number of account has been reached!" << endl;
        system("pause");
        return;
    }

    Account newAcc;
    newAcc.accNo = generateAccNo();

    cout << "\t=== NEW ACCOUNT REGISTRATION ===" << endl;
    cout << "Assigned Account Number: " << newAcc.accNo << endl << endl;

    do {
        cout << "Enter Account Name: ";
        getline(cin, newAcc.accName);
        if (newAcc.accName.empty()) {
            cout << "Account name cannot be blank!" << endl;
        }
    } while (newAcc.accName.empty());

    do {
        cout << "Enter Birthda (MM/DD/YYYY): ";
        getline(cin, newAcc.birthday);
        if (newAcc.birthday.empty()) {
            cout << "Birthday cannot be blank!" << endl;
        }
    } while (newAcc.birthday.empty());

    do {
        cout << "Enter Contact Number: ";
        getline(cin, newAcc.contact);
        if (newAcc.contact.empty()) {
            cout << "Contact number cannot be blank!" << endl;
        }
    } while(newAcc.contact.empty());

    double deposit;
    do {
        deposit = inputDouble("Enter Initial Deposit (Min. 5000): ");
        if (deposit < MIN_DEPOSIT) {
            cout << "Initial deposit must be at least: " << MIN_DEPOSIT << endl;
        }
    } while (deposit < MIN_DEPOSIT);
    newAcc.balance = deposit;

    newAcc.pin = generatePin();
    cout << endl << "Your assigned PIN code is: " << newAcc.pin << endl;
    cout << "Please remember this PIN!" << endl << endl;

    cout << "Insert your flash drive to save it as your ATM card." << endl;
    string drivePATH = inputDriveLetter();

    if(!writeCard(newAcc, drivePATH)) {
        cout << "Registration cancelled. No account was created!" << endl;
        system("pause");
        return;
    }

    last++;
    data[last] = newAcc;
    save();
    system("cls");
    cout << endl << "========== ACCOUNT REGISTERED SUCCESSFULLY! ==========" << endl;
    system("pause");

}


void Module :: Balcheck(){

}

void Module :: Withdraw(){

}

void Module :: Deposit(){

}

void Module :: transfer(){

}

void Module :: changePIN(){

}


int menu(){
    int ch;

    cout<< "Transaction Options"<<endl<<endl;
    cout<< "1.) Balance Inquiry"<<endl;
    cout<< "2.) Withdraw"<<endl;
    cout<< "3.) Deposit"<<endl;
    cout<< "4.) Transfer"<<endl;
    cout<< "5.) Change PIN"<<endl;
    cout << "6.) EXIT" << endl;

    cout<<"Enter your Choice (1-6): ";
    cin>> ch;

    return ch;
}

int main(){

    cout << "no error" << endl;
    return 0;
}
