#ifndef INVOKABLE_EVENTS
#define INVOKABLE_EVENTS
    #include <functional>
    #include <type_traits>
    #include <vector>
    #include <algorithm>

    #pragma region Placeholder Generator

    /// by "dyp" at http://stackoverflow.com/a/21664270
    template<int...>
    struct int_sequence {};

    template<int N, int... Is>
    struct make_int_sequence : make_int_sequence<N - 1, N - 1, Is...> {};

    template<int... Is>
    struct make_int_sequence<0, Is...> : int_sequence<Is...> {};

    template<int>
    struct placeholder_template {};

    namespace std {
        template<int N>
        struct is_placeholder< placeholder_template<N> > : integral_constant<int, N + 1> {};
    };

    #pragma endregion

    #pragma region Invokable Callback

    template<typename... A>
    class callback {
    private:
        size_t hash = 0;
    public:
        std::function<void(A...)> func;

        inline bool operator == (const callback& cb) {
            return (hash == cb.hash);
        };

        template<typename T, class _Fx, int... Is>
        callback(T* obj, _Fx&& func, int_sequence<Is...>) {
            function<void(A...)> fn = bind(func, obj, placeholder_template<Is>{}...);
            this->func = fn;
            hash = (size_t)&func ^ (&typeid(callback<A...>))->hash_code();
        };

        size_t hash_code() const {
            return hash;
        };
    };

    #pragma endregion

    #pragma region Invokable event

    template<typename... A>
    class invokable {
    private:
        std::vector<callback<A...>> callbacks;

    public:
        inline void operator += (const callback<A...>& cb) {
            if (find(callbacks.begin(), callbacks.end(), cb) == callbacks.end())
                callbacks.push_back(cb);
        };
        
        inline void operator -= (const callback<A...>& cb) {
            std::vector<callback<A...>>::iterator it;
            it = std::find(callbacks.begin(), callbacks.end(), cb);
            if (it != callbacks.end())
                callbacks.erase(it);
        };

        inline void operator () (A... args) {
            invoke(args...);
        };

        template<typename T, class _Fx>
        callback<A...> create(T* obj, _Fx&& func) {
            callback<A...> cb(obj, func, make_int_sequence<sizeof...(A)>{});
            return cb;
        };

        void hook(callback<A...> cb) {
            (*this) += cb;
        };

        void unhook(callback<A...> cb) {
            (*this) -= cb;
        };

        template<typename... Ar>
        void invoke(Ar... args) {
            for (size_t i = 0; i < callbacks.size(); i++) {
                callback<A...> cb = callbacks[i];
                cb.func(args...);
            }
        };
    };

    #pragma endregion
#endif
