#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

const string fn="database.csv";

struct Account{
    int AccNo, InitialDeposit;
    string Name, Birthday, Contact, Pin code;
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


int main(){

    return 0;
}
