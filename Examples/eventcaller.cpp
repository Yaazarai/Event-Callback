/*
  Simple example that creates an event caller and listener. A function
  from the listener is subscribed to the caller, then the caller invokes
  the event. When the event is invoked the listener is notified.
*/
#include <iostream>
#include "invokable.hpp"

class EventCaller {
public:
    invokable<> button_press;
};

class EventListener {
public:
    void notifier() {
        cout << "Button Pressed" << endl;
    };
};

int main() {
    EventCaller caller;
    EventListener listener;
    
    callback<> call = caller.button_press.create(&listener, &EventListener::notifier);
    caller += call;
    caller.button_press();
    caller -= call;
};

/*
    Output:
        Button Pressed
*/
