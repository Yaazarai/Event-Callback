/// Source: https://stackoverflow.com/questions/9568150/what-is-a-c-delegate/9568485#9568485
/// Source: https://en.cppreference.com/w/cpp/utility/functional/invoke
#pragma once
#ifndef INVOKABLE_CALLBACKS
#define INVOKABLE_CALLBACKS
    #include <functional>
    #include <vector>
    #include <mutex>
    #include <utility>
    #include <type_traits>

    template<typename... A>
    class callback {
    protected:
        /// Unique identifying hash code.
        size_t hash;
        /// The function bound to this callback.
        std::function<void(A...)> bound;

    public:
        callback(std::function<void(A...)> func) {
            bound = [func = std::move(func)](A... args) { std::invoke(func, args...); };
            hash = bound.target_type().hash_code();
        }

        /// Compares the underlying hash_code of the callback function(s).
        bool operator == (const callback<A...>& cb) { return hash == cb.hash; }
        
        /// Inequality Compares the underlying hash_code of the callback function(s).
        bool operator != (const callback<A...>& cb) { return hash != cb.hash; }
        
        /// Returns the unique hash code for this callback function.
        constexpr size_t hash_code() const throw() { return hash; }
        
        /// Invoke this callback with required arguments.
        callback<A...>& invoke(A... args) { bound(args...); return (*this); }

        /// Operator() invoke this callback with required arguments.
        void operator()(A... args) { bound(args...); }
    };

    template<typename... A>
    class invokable {
    protected:
        /// Resource lock for thread-safe accessibility.
        std::mutex safety_lock;
        /// Record of stored callbacks to invoke.
        std::vector<callback<A...>> callbacks;

    public:
        /// Adds a callback to this event, operator +=
        invokable<A...>& hook(const callback<A...> cb) {
            std::lock_guard<std::mutex> g(safety_lock);
            if (std::find(callbacks.begin(), callbacks.end(), cb) == callbacks.end())
                callbacks.push_back(cb);
            return (*this);
        }

        /// Removes a callback from this event, operator -=
        invokable<A...>& unhook(const callback<A...> cb) {
            std::lock_guard<std::mutex> g(safety_lock);
            std::erase_if(callbacks, [cb](callback<A...> c){ return cb.hash_code() == c.hash_code(); });
            return (*this);
        }

        /// Removes all registered callbacks and adds a new callback, operator =
        invokable<A...>& rehook(const callback<A...> cb) {
            std::lock_guard<std::mutex> g(safety_lock);
            callbacks.clear();
            callbacks.push_back(cb);
            return (*this);
        }

        /// Removes all registered callbacks.
        invokable<A...>& empty() {
            std::lock_guard<std::mutex> g(safety_lock);
            callbacks.clear();
            return (*this);
        }

        /// Execute all registered callbacks, operator ()
        invokable<A...>& invoke(A... args) {
            std::lock_guard<std::mutex> g(safety_lock);
            for(callback<A...> cb : callbacks) cb.invoke(args...);
            return (*this);
        }
    };

#endif
