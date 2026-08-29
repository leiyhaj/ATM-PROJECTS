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
    cout << "Hello world";
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


int main(){

    return 0;
}
