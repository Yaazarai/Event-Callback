#include <iostream>
#include "invokable.hpp"

class myclass {
private:
    int id = 0;
public:
    myclass(int id) {
       this->id = id;
    };
    
    void function(int x) {
        std::cout << (id * x) << std::endl;
    };
};

class someclass {
public:
    void func(int x) {
        std::cout << (x / 6) << std::endl;
    };
};

int main() {
    invokable<int> myevent;
    myclass x(2), y(1);
    someclass z;
    callback<int> callx(&x, &myclass::function);
    callback<int> cally(&y, &myclass::function);
    callback<int> callz(&z, &someclass::func);
    
    myevent += callx;
    myevent += cally;
    myevent += callz;
    myevent(12);
    myevent -= callx;
    myevent -= cally;
    myevent -= callz;
};

/*
    Output:
        24
        12
        2
*/
