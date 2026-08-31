#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <windows.h>
using namespace std;

#define MAX 100
#define encrypt_key 3
#define MIN_DEPOSIT 5000
const string fileName = "database.csv";
const string cardFile = "pin.txt";

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

        // Para sa encrytion keneme
        string encryptPIN(string pin);
        string decryptPIN(string pin);
        string generatePin();

    public:
        // Initialze last to -1
        Module() {makenull();}
        // Account Modules
        int login(int accNo);
        void Enrollment(string drivePATH);
        // Transaction Modules
        void Balcheck(int accNo);
        void Withdraw(int accNo);
        void Deposit(int accNo);
        void transfer(int accNo);
        void changePIN(int accNo);
        // Input checker
        int inputInt(string prompt);
        double inputDouble(string prompt);
        string inputPin(string prompt);
        string inputDriveLetter();
        int flashDriveDetector(string drive[]);
        // File Handling
        void save();
        void retrieve();
        // Sa flash drive naman to
        bool writeCard(const Account &acc, string drivePATH);
        bool readCard(string drivePATH, int &accNo, string &pin);
};

int Module :: generateAccNo(){
    srand(time(0));

    int x = 10000; //Start
    int y = 99999; //End
    int accountnum = x + rand() % (y - x + 1);

    return accountnum;
}

int Module :: inputInt(string prompt) {
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

double Module ::  inputDouble(string prompt) {
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

string Module :: inputPin(string prompt) {
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

string Module :: inputDriveLetter() {
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

int Module :: flashDriveDetector (string drive[]) {
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
    if (!fp){
        cout << "There are no registered accounts!" << endl;
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

int Module :: login(int accNo){
    system("cls");
    string PIN;
   int position = locate(accNo);
        if (position == -1){
            cout<<"Card is invalid! No matching data found!"<<endl;
            system("pause");
            return -1;
        }

        string PINinput = inputPin("Enter your PIN: ");

        if(data[position].pin == PINinput){
            return accNo;
        }
        return -1;
}

void Module :: Enrollment(string drivePATH){
    system("cls");

    if (isFull()) {
        cout << "Registration failed. Maximum number of account has been reached!" << endl;
        system("pause");
        return;
    }

    Account newAcc;

    do{
    newAcc.accNo = generateAccNo();
    }while(locate(newAcc.accNo)!= -1);

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
        cout << "Enter Birthday (MM/DD/YYYY): ";
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
    system("pause");

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

void Module :: Balcheck(int accNo){
    int position;

    position = locate(accNo);

    cout<<"BALANCE INQUIRY"<<endl<<endl;
    cout<<"Your current balance is: "<<fixed<<setprecision(2)<<data[position].balance<<endl<<endl;
    system("pause");
}

void Module :: Withdraw(int accNo){
    int position;
    double amount;

    position = locate(accNo);

    cout<<"WITHDRAW"<<endl<<endl;
    amount = inputDouble("Enter Amount to withdraw: ");


    if(amount>data[position].balance){
        cout<<"Insufficient Balance!"<<endl;
        system("pause");
    }else{
     data[position].balance -= amount;
     cout<<"Withdraw Successful!"<<endl;
     system("pause");
    }
}

void Module :: Deposit(int accNo){
    int position;
    double amount;

    position = locate(accNo);

    cout<<"DEPOSIT"<<endl<<endl;
    amount = inputDouble("Enter Amount to deposit: ");

    data[position].balance += amount;

    cout<<"Successfully Deposited!"<<endl;
    system("pause");
}

void Module :: transfer(int accNo){
    int position,psttrans, transferee;
    double amount;

    cout<<"TRANSFER"<<endl<<endl;
    cout<<"Input Account number to transfer to: ";
    cin>>transferee;

    amount = inputDouble("Enter amount to transfer: ");

    position = locate(accNo);
    psttrans = locate(transferee);

    if(data[position].balance<amount){
        cout<<"Insufficient Balance!"<<endl;
        system("pause");
    }

    if(psttrans == -1){
        cout<<"Account not found!"<<endl<<"Please input a valid account number."<<endl;
        system("pause");
    }else{
        data[position].balance -= amount;
        data[psttrans].balance += amount;
        cout<<"Successfully Transferred!"<<endl;
        system("pause");
    }
}

void Module :: changePIN(int accNo){
    int position;
    string newPIN, currPIN;

    position = locate(accNo);

    cout<<"CHANGE PIN"<<endl<<endl;
    cout<<"Enter Current PIN: ";
    cin>> currPIN;
    cin.ignore(1000, '\n');

    if(currPIN!=data[position].pin){
        cout<<"Invalid PIN!"<<endl<<"Please Try Again!"<<endl<<endl;
        system("pause");
    }else{
        newPIN = inputPin("Enter Your New PIN: ");
        data[position].pin = newPIN;
    }
}

int menu(){
    system("cls");
    int ch;

    cout<< "TRANSACTION OPTIONS"<<endl<<endl;
    cout<< "1.) BALANCE INQUIRY"<<endl;
    cout<< "2.) WITHDRAW"<<endl;
    cout<< "3.) DEPOSIT"<<endl;
    cout<< "4.) TRANSFER"<<endl;
    cout<< "5.) CHANGE PIN"<<endl;
    cout<< "6.) EXIT" <<endl;

    cout<<"Enter your Choice (1-6): ";
    cin>> ch;

    return ch;
}

int main(){
    Module M;
    M.retrieve();
    system("cls");

    cout<<"Please insert your card."<<endl;
    string drivePATH = M.inputDriveLetter();

    int accNo;
    string cardPIN;
    bool dataexists = M.readCard(drivePATH, accNo, cardPIN);
    int loggedinAcc = -1;

    if(!dataexists){
        system("cls");
        char ch;
        cout<<"Card is not enrolled!"<<endl;
        cout<<"Would you like to enroll your card? (y/n): ";
        cin>>ch;
        cin.ignore(1000, '\n');
        if(ch == 'y' || ch == 'Y'){
            cout<<"Proceeding to enrollment...."<<endl<<endl;
            system("pause");
            M.Enrollment(drivePATH);
            dataexists = M.readCard(drivePATH, accNo, cardPIN);
        }else{
            cout<<"Thank you for using this machine!";
            system("pause");
            exit(0);
        }
    }

        int attempts = 0;

        while(attempts < 3){
            loggedinAcc = M.login(accNo);

            if(loggedinAcc != -1){
                cout<<"Log-in Successful!"<<endl;
                system("pause");
                break;
            }

            attempts++;
            cout<<"Invalid PIN! "<<(3-attempts)<<" Attempts Remaining!"<<endl<<endl;
            system("pause");
        }

        if(loggedinAcc == -1){
            cout<<"Too many failed attempts! Returning card..."<<endl;
            system("pause");
            return 0;
        }

        while(1){
        switch(menu()){

        case 1:
            system("cls");
            M.Balcheck(loggedinAcc);
            M.save();
            break;

        case 2:
            system("cls");
            M.Withdraw(loggedinAcc);
            M.save();
            break;

        case 3:
            system("cls");
            M.Deposit(loggedinAcc);
            M.save();
            break;

        case 4:
            system("cls");
            M.transfer(loggedinAcc);
            M.save();
            break;

        case 5:
            system("cls");
            M.changePIN(loggedinAcc);
            M.save();
            break;

        case 6:
            cout<<"Thank you for Banking with us!"<<endl;
            cout<<"Please remove your card!"<<endl<<endl;
            system("pause");
            exit(0);
            break;

        default:
            cout<<"Invalid Choice! Please choose again."<<endl<<endl;
            system("pause");
            break;
        }
    }
    return 0;
}
