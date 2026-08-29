#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

const string fn="database.csv";

struct Account{
    int accNo, initialDeposit;
    string name, birthday, contact, pinCode;
};

class Module{
    private:
        void encryption();
        void save();
        void retrieve();
    public:
        void Enrollment();
        void Balcheck();
        void Withdraw();
        void Deposit();
        void transfer();
        void changePIN();
};

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

void Module :: encryption(){

}

void Module :: save(){

}

void Module :: retrieve(){

}

int menu(){
    int ch;

    cout<<"Transaction Options"<<endl<<endl;
    cout<<"1.) Balance Inquiry"<<endl;
    cout<<"2.) Withdraw"<<endl;
    cout<<"3.) Deposit"<<endl;
    cout<<"4.) Transfer"<<endl;
    cout<<"5.) Change PIN"<<endl<<endl;

    cout<<"Enter your Choice (1-5): ";
    cin>> ch;

    return ch;
}

int main(){

    return 0;
}
