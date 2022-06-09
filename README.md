# C++ Event Callback (requires C++20)
Library that mimics the C# style event handling system via subscribing(***hooking***) / unsubscribing(***unhooking***) member functions to events. Create a callback, subscribe that callback to an event, invoke the event, unscribe the callback when it's no longer needed.

Sources: [Function\<T\>](http://stackoverflow.com/a/9568485) and [Placeholder Generator](http://stackoverflow.com/a/21664270/4988255).

### Reference Invokable / Callback

Creating an invokable event with input parameters:
```C++
// With parameters (int, int)
invokable<int, int> event;

// Without parameters
invokable<> event;
```
Callback creation:

- ***Quick Clarification: The template arguments are the function parameters of the function you want to create a callback for. The actual arguments to the callback constructor are the references to the function and instance the function is attached to itself. This could be somewhat confusing if you usually correlate template parameters with the leading constructor parameters for other projects.***
```C++
class myclass {
public:
    void F(int x, int y) {...};
};

myclass inst;
callback<int, int> call(&inst, &myclass::G);
```
Hooking and unhooking a callback to/from an event:
```C++
// Hoook
event += call;
// OR
event.hook(call);

// Unhook
event -= call;
// OR
event.unhook(call);

// Hook Event & Unhook All Other Hooked events
event = call;
// OR
event.hook_unhook(call);

// Clone invokable A events into invokable B (replacing all events in B).
invokable<int, int> eventA;
eventA += call1;
eventA += call2;
eventA += call3;
eventB.clone(eventA);
// Now eventB has all of eventA's event calls.
```
Unhooking all hooked events:
```C++
event.unhook_all();
```

Invoking an event--notifies subscribed callbacks:
```C++
event(10, 10);
// OR
event.invoke(10, 10);
```
Lambda & Static Class Methods:
```C++
class myclass {
public:
    static void function() {
       cout << "Static Call" << endl;
    }
};

invokable<> event;
callback<> static_call(&myclass::function);
callback<> lambda_call([](){cout << "Lambda Call" << endl;});
event += static_call;
event += lambda_call;
```
Finally the function calls can be method-chained, example:
```
event.hook(static_call).hook(lambda_call);
```
