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

- ***Quick Clarification: The template arguments are the function parameters of the function you want to create a callback for. While the constructor accepts `std::function` using those parameters.***

Example usage of callback creation using lambdas:
```C++
class myclass {
public:
    void F(int x, int y) {...};
    static inline void FF(int x, int y) {...};
};

class inst;
auto lambda1 = [&inst](int x, int y) { inst.F(x, y); };
callback<int, int> call1(lambda1);

auto lambda2 = [](int x, int y) { myclass::FF(x, y); };
callback<int, int> call2(lambda2);
```
Hooking and unhooking a callback to/from an event:
```C++
// Hoook
event.hook(call);

// Unhook
event.unhook(call);

// Hook Event & Unhook All Other Hooked events
event.rehook(call);

// Clone invokable A events into invokable B (replacing all events in B).
invokable<int, int> eventA;
eventA.hook(call1);
eventA.hook(call2);
eventA.hook(call3);
```
Unhooking all hooked events:
```C++
event.empty();
```
Invoking an event--notifies subscribed callbacks:
```C++
event.invoke(10, 10);
```
Finally the function calls can be method-chained, example:
```
event.hook(static_call).hook(lambda_call);
```
Each callback also has a semi-unique hash code. The hash code is not unique to the callback itself, but unique to the underlying function or member-function bound to the callback. This keeps you from generating duplicate bindings for the same callback function to `invokable<A...>`. You can still create duplicate `callbacks<A...>` with the same function or member-function, but they cannot all be bound to `invokable<A...>`.
```
callback<> static_call(&myclass::function);
size_t hash = static_call.hash_code();
```
