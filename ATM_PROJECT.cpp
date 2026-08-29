#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
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
double inputDouble(string prompt);
string inputDriveLetter();

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

string inputDriveLetter() {
    string input;
    bool valid;

    do {
        cout << "Enter drive letter or folder path for your card" << endl;
        cout << ("(e.g C for a real flash drive, or C:\\ATMCard\\ if no flash drive): ");
        getline(cin, input);
        valid = !input.empty();
        if (!valid) {
            cout << "This cannot be blank!" << endl;
        }
    } while (!valid);
    
    // yung single letter C ay = E:\  .
    // then pag hindi na sya single letter, treated na sya as folder path
    if(input.length() == 1 && isalpha(input[0])) {
        return input + ":\\";
    }

    char lastleter = input[input.length() - 1];
    if (lastleter != '\\' && lastleter != '/') {
        input = input + "\\";
    }
    return input;
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
