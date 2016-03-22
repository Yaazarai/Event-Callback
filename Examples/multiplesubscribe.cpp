```
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
        cout << (id * x) << endl;
    };
};

class someclass {
public:
    void func(int x) {
        cout << x << endl;
    };
};

int main() {
    invokable<int> event;
    myclass x(3), y(1);
    someclass z;
    callback<int> callx = event.create(&x, &myclass::function);
    callback<int> cally = event.create(&y, &myclass::function);
    callback<int> callz = event.create(&z, &someclass::func);
    
    event += callx;
    event += cally;
    event += callz;
    event(12);
    event -= callx;
    event -= cally;
    event -= callz;
};

/*
    Output:
        24
        12
        2
*/
```
