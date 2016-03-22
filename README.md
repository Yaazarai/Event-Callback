# C++ Event Callback
Library that mimics the C# style event handling system via subscribing(***hooking***) / unsubscribing(***unhooking***) member functions to events. Create a callback, subscribe that callback to an event, invoke the event, unscribe the callback when it's not longer needed.

Sources: [Function\<T\>](http://stackoverflow.com/a/9568485) and [Placeholder Generator](http://stackoverflow.com/a/21664270/4988255).

####Reference Invokable / Callback

Creating an invokable event with input parameters:
```
// With parameters (int, int)
invokable<int, int> event;

// Without parameters
invokable<> event;
```
Explicit callback creation:
```
class myclass {
public:
    void F(int x, int y) {...};
};

myclass inst;
callback<int, int> call = event.create(&inst, &myclass::G);
```
Hooking and unhooking a callback to/from an event:
```
// Hoook
event += call;
// OR
event.hook(call);

// Unhook
event -= call;
// OR
event.unhook(call);
```
Invoking an event--notifies subscribed callbacks:
```
event(10, 10);
// OR
event.invoke(10, 10);
```


####Example
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
