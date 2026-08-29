#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
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
        void writeCard(const Account &acc, string drivePATH);
        bool readCard(string drivePATH, int &accNo, string &pin);

    public:
        void Enrollment();
        void Balcheck();
        void Withdraw();
        void Deposit();
        void transfer();
        void changePIN();
};

int Module :: generateAccNo(){
    srand(time(0));

    x = 10000 //Start
    y = 99999 //End
    accNo = x + rand() % (y - x + 1);

    return accNo;
}

void Module :: Enrollment(){

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

void Module :: save(){

}

void Module :: retrieve(){

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

    return 0;
}
