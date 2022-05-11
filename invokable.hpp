#pragma once
#ifndef INVOKABLE_EVENTS
#define INVOKABLE_EVENTS
    #include <functional>
    #include <type_traits>
    #include <vector>
    #include <algorithm>
    #include <mutex>
    
    /// by "dyp" at http://stackoverflow.com/a/21664270
    /// This generates the expansion for vargs into the invokable templated types.
    #pragma region Placeholder Generator
    template<int>
    struct placeholder_template {};

    namespace std {
        template<int N>
        struct is_placeholder<placeholder_template<N>> : integral_constant<int, N + 1> {};
    };
    #pragma endregion

    #pragma region Invokable Callback
    /// Function binder for dynamic callbacks.
    template<typename... A>
    class callback {
    private:
        // Unique hash ID.
        size_t hash;
        // Bound function.
        std::function<void(A...)> func;
        
        // Creates and binds the function callback with a class instance for member functions.
        template<typename T, class _Fx, std::size_t... Is>
        void create(T* obj, _Fx&& func, std::integer_sequence<std::size_t, Is...>) {
            this->func = std::function<void(A...)>(std::bind(func, obj, placeholder_template<Is>()...));
        };

        // Creates and binds the function callback for a static/lambda/global function.
        template<class _Fx, std::size_t... Is>
        void create(_Fx&& func, std::integer_sequence<std::size_t, Is...>) {
            this->func = std::function<void(A...)>(std::bind(func, placeholder_template<Is>()...));
        };

    public:
        /// Compares equality of callbacks.
        const inline bool operator == (const callback& cb) { return (hash == cb.hash); };
        
        /// Compares not-equality of callbacks.
        const inline bool operator != (const callback& cb) { return (hash != cb.hash); };
        
        /// Executes this callback with arguments.
        const inline callback& operator () (A... args) { func(args...); return (*this); };
        
        /// Construct a callback with a template T reference for instance/member functions.
        template<typename T, class _Fx>
        callback(T* obj, _Fx&& func) {
            hash = reinterpret_cast<size_t>(&this->func) ^ (&typeid(callback<A...>))->hash_code();
            create(obj, func, std::make_integer_sequence<std::size_t, sizeof...(A)> {});
        };

        /// Construct a callback with template _Fx for static/lambda/global functions.
        template<class _Fx>
        callback(_Fx&& func) {
            hash = reinterpret_cast<size_t>(&this->func) ^ (&typeid(callback<A...>))->hash_code();
            create(func, std::make_integer_sequence<std::size_t, sizeof...(A)> {});
        };
        
        /// Executes this callback with arguments.
        callback& invoke(A... args) { func(args...); return (*this); };

        /// Returns this callbacks hash code.
        constexpr size_t hash_code() const throw() {
            return hash;
        };
    };
    #pragma endregion

    #pragma region Invokable Event
    /// Thread-safe event handler for callbacks.
    template<typename... A>
    class invokable {
    private:
        /// Shared resource to manage multi-thread resource locking.
        std::mutex safety_lock;
        /// Vector list of function callbacks associated with this invokable event.
        std::vector<callback<A...>> callbacks;

    public:
        /// Adds a callback to this event.
        const inline invokable& operator += (const callback<A...>& cb) {
            if (cb != nullptr) hook(cb);
            return (*this);
        };
        
        /// Removes a callback from this event.
        const inline invokable& operator -= (const callback<A...>& cb) { unhook(cb); return (*this); };
        
        /// Removes all registered callbacks and adds a new callback, unless defaulted then all callbacks are unhooked.
        const inline invokable& operator = (const callback<A...>& cb) { hook_unhook(cb); return (*this); };
        
        /// Execute all registered callbacks.
        const inline invokable& operator () (A... args) { invoke(args...); return (*this); };
        
        /// Adds a callback to this event, operator +=
        invokable& hook(callback<A...> cb) {
            std::lock_guard<std::mutex> g(safety_lock);

            if (std::find(callbacks.begin(), callbacks.end(), cb) == callbacks.end())
                callbacks.push_back(cb);

            return (*this);
        };
        
        /// Removes a callback from this event, operator -=
        invokable& unhook(callback<A...> cb) {
            std::lock_guard<std::mutex> g(safety_lock);

            typename std::vector<callback<A...>>::iterator it;
            it = std::find(callbacks.begin(), callbacks.end(), cb);

            if (it != callbacks.end())
                callbacks.erase(it);

            return (*this);
        };
        
        /// Removes all registered callbacks and adds a new callback, operator =
        invokable& hook_unhook(callback<A...> cb) {
            std::lock_guard<std::mutex> g(safety_lock);

            callbacks.clear();
            (*this) += cb;

            return (*this);
        };
        
        /// Removes all registered callbacks.
        invokable& unhook_all() {
            std::lock_guard<std::mutex> g(safety_lock);

            callbacks.clear();

            return (*this);
        };
        
        /// Execute all registered callbacks, operator ()
        invokable& invoke(A... args) {
            std::lock_guard<std::mutex> g(safety_lock);

            for (size_t i = 0; i < callbacks.size(); i++)
                callbacks[i](args...);

            return (*this);
        };
    };
    #pragma endregion
#endif
