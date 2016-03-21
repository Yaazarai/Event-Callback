#ifndef INVOKABLE_EVENTS
#define INVOKABLE_EVENTS
    #include <functional>
    #include <type_traits>
    #include <vector>

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

        inline bool operator == (const callback& objX) {
            return (hash == objX.hash);
        };

        template<typename T, class _Fx, int... Is>
        callback(T* obj, _Fx&& func, int_sequence<Is...>) {
            function<void(A...)> fn = bind(func, obj, placeholder_template<Is>{}...);
            this->func = fn;
            hash = (size_t)&func ^ (&typeid(callback<A...>))->hash_code();
        };

        size_t hash_code() {
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
        inline void operator -= (const callback<A...>& objX) {
            unhook(objX);
        };

        inline void operator += (const callback<A...>& objX) {
            callbacks.push_back(objX);
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
            callbacks.push_back(cb);
        };

        void unhook(callback<A...> cb) {
            for (size_t i = 0; i < callbacks.size(); i++) {
                if (cb == callbacks.at(i)) {
                    callbacks.erase(callbacks.begin() + i);
                    break;
                }
            }
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
