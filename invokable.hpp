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
        struct is_placeholder<placeholder_template<N>> : integral_constant<int, N + 1> {};
    };

    #pragma endregion

    #pragma region Invokable Callback

    template<typename... A>
    class callback {
    private:
        size_t hash = 0;
        std::function<void(A...)> func;

        template<typename T, class _Fx, int... Is>
        void create(T* obj, _Fx&& func, int_sequence<Is...>) {
            hash = (size_t)&this->func ^ (&typeid(callback<A...>))->hash_code();
            this->func = std::function<void(A...)>(std::bind(func, obj, placeholder_template<Is>{}...));
        };

        template<class _Fx, int... Is>
        void create(_Fx&& func, int_sequence<Is...>) {
            this->func = std::function<void(A...)>(std::bind(func, placeholder_template<Is>{}...));
            hash = (size_t)&this->func ^ (&typeid(callback<A...>))->hash_code();
        };

    public:
        inline bool operator == (const callback& cb) {
            return (hash == cb.hash);
        };

        inline bool operator != (const callback& cb) {
            return (hash != cb.hash);
        };

        inline callback& operator () (A... args) {
            func(args...);
            return (*this);
        };

        template<typename T, class _Fx>
        callback(T* obj, _Fx&& func) {
            create(obj, func, make_int_sequence<sizeof...(A)> {});
        };

        template<class _Fx>
        callback(_Fx&& func) {
            create(func, make_int_sequence<sizeof...(A)> {});
        };

        void invoke(A... args) {
            func(args...);
        };

        size_t hash_code() const {
            return hash;
        };
    };

    #pragma endregion

    #pragma region Invokable Event

    template<typename... A>
    class invokable {
    private:
        std::vector<callback<A...>> callbacks;

    public:
        inline invokable& operator += (const callback<A...>& cb) {
            if (std::find(callbacks.begin(), callbacks.end(), cb) == callbacks.end())
                callbacks.push_back(cb);
            return (*this);
        };

        inline invokable& operator -= (const callback<A...>& cb) {
            std::vector<callback<A...>>::iterator it;
            it = std::find(callbacks.begin(), callbacks.end(), cb);
            
            if (it != callbacks.end())
                callbacks.erase(it);
            return (*this);
        };

        inline invokable& operator = (const callback<A...>& cb) {
            callbacks.clear();
            (*this) += cb;
            return (*this);
        };

        inline invokable& operator () (A... args) {
            invoke(args...);
            return (*this);
        };

        void hook(callback<A...> cb) {
            (*this) += cb;
        };

        void unhook(callback<A...> cb) {
            (*this) -= cb;
        };

        void invoke(A... args) {
            for (size_t i = 0; i < callbacks.size(); i++)
                callbacks[i](args...);
        };
    };

    #pragma endregion
#endif
