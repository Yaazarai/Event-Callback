#pragma once
#ifndef INVOKABLE_CALLBACKS
#define INVOKABLE_CALLBACKS
    #include <functional>
    #include <type_traits>
    #include <vector>
    #include <algorithm>
    #include <mutex>

    /// This generates the expansion for vargs into the invokable templated types.
    #pragma region Placeholder Generator
    template<int>
    struct placeholder_template {};

    namespace std {
        template<int N>
        struct is_placeholder<placeholder_template<N>> : integral_constant<int, N + 1> {};
    }
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
        }

        // Creates and binds the function callback for a static/lambda/global function.
        template<class _Fx, std::size_t... Is>
        void create(_Fx&& func, std::integer_sequence<std::size_t, Is...>) {
            this->func = std::function<void(A...)>(std::bind(func, placeholder_template<Is>()...));
        }
    public:
        /// Compares equality of callbacks.
        inline bool operator == (const callback& cb) { return (hash == cb.hash); }
        /// Compares not-equality of callbacks.
        inline bool operator != (const callback& cb) { return (hash != cb.hash); }
        /// Executes this callback with arguments.
        inline callback& operator () (A... args) { func(args...); return (*this); }

        /// Construct a callback with a template T reference for instance/member functions.
        template<typename T, class _Fx>
        callback(T* obj, _Fx&& func) {
            hash = reinterpret_cast<size_t>(&this->func) ^ reinterpret_cast<size_t>(obj) ^ (&typeid(callback<A...>))->hash_code();
            create(obj, func, std::make_integer_sequence<std::size_t, sizeof...(A)> {});
        }

        /// Construct a callback with template _Fx for static/lambda/global functions.
        template<class _Fx>
        callback(_Fx&& func) {
            hash = reinterpret_cast<size_t>(&this->func) ^ (&typeid(callback<A...>))->hash_code();
            create(func, std::make_integer_sequence<std::size_t, sizeof...(A)> {});
        }

        /// Executes this callback with arguments.
        callback& invoke(A... args) { func(args...); return (*this); }

        /// Returns this callbacks hash code.
        constexpr size_t hash_code() const throw() { return hash; }
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
        inline invokable<A...>& operator += (const callback<A...>& cb) { hook(cb); return (*this); }
        /// Removes a callback from this event.
        inline invokable<A...>& operator -= (const callback<A...>& cb) { unhook(cb); return (*this); }
        /// Removes all registered callbacks and adds a new callback.
        inline invokable<A...>& operator = (const callback<A...>& cb) { hook_unhook(cb); return (*this); }
        /// Execute all registered callbacks.
        inline invokable<A...>& operator () (A... args) { invoke(args...); return (*this); }

        /// Adds a callback to this event, operator +=
        invokable<A...>& hook(callback<A...> cb) {
            std::lock_guard<std::mutex> g(safety_lock);

            if (std::find(callbacks.begin(), callbacks.end(), cb) == callbacks.end())
                callbacks.push_back(cb);

            return (*this);
        }

        /// Removes a callback from this event, operator -=
        invokable<A...>& unhook(callback<A...> cb) {
            std::lock_guard<std::mutex> g(safety_lock);
            std::erase_if(callbacks, [cb](callback<A...> i) { return cb == i; });
            return (*this);
        }

        /// Removes all registered callbacks and adds a new callback, operator =
        invokable<A...>& hook_unhook(callback<A...> cb) {
            std::lock_guard<std::mutex> g(safety_lock);
            callbacks.clear();
            callbacks.push_back(cb);
            return (*this);
        }

        /// Removes all registered callbacks.
        invokable<A...>& unhook_all() {
            std::lock_guard<std::mutex> g(safety_lock);
            callbacks.clear();
            return (*this);
        }

        /// Execute all registered callbacks, operator ()
        invokable<A...>& invoke(A... args) {
            std::lock_guard<std::mutex> g(safety_lock);
            for (callback<A...>& cb : callbacks) cb.invoke(args...);
            return (*this);
        }
    };
    #pragma endregion
#endif
