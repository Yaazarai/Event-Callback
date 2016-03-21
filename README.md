# CPP-Event-Callback
Similar C# style event system for C++.

####Example Usage of Invokable / Callback

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
    void F(int x, int y) {};
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
