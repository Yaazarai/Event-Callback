#ifndef INVOKABLE_EVENTS
#define INVOKABLE_EVENTS
    #include <functional>
    #include <type_traits>
    #include <vector>
    #include <algorithm>

    #pragma region Placeholder Generator

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
        size_t hash;
        std::function<void(A...)> func;

        template<typename T, class _Fx, std::size_t... Is>
        void create(T* obj, _Fx&& func, std::integer_sequence<std::size_t, Is...>) {
            this->func = std::function<void(A...)>(std::bind(func, obj, placeholder_template<Is>()...));
        };

        template<class _Fx, std::size_t... Is>
        void create(_Fx&& func, std::index_sequence<Is...>) {
            this->func = std::function<void(A...)>(std::bind(func, placeholder_template<Is>()...));
        };

    public:
        const inline bool operator == (const callback& cb) {
            return (hash == cb.hash);
        };

        const inline bool operator != (const callback& cb) {
            return (hash != cb.hash);
        };

        const inline callback& operator () (A... args) {
            func(args...);
            return (*this);
        };

        template<typename T, class _Fx>
        callback(T* obj, _Fx&& func) {
            hash = reinterpret_cast<size_t>(&this->func) ^ (&typeid(callback<A...>))->hash_code();
            create(obj, func, std::make_integer_sequence<std::size_t, sizeof...(A)> {});
        };

        template<class _Fx>
        callback(_Fx&& func) {
            hash = reinterpret_cast<size_t>(&this->func) ^ (&typeid(callback<A...>))->hash_code();
            create(func, std::make_integer_sequence<std::size_t, sizeof...(A)> {});
        };

        void invoke(A... args) {
            func(args...);
        };

        constexpr size_t hash_code() const throw() {
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
        const inline invokable& operator += (const callback<A...>& cb) {
            if (std::find(callbacks.begin(), callbacks.end(), cb) == callbacks.end())
                callbacks.push_back(cb);
            return (*this);
        };

        const inline invokable& operator -= (const callback<A...>& cb) {
            typename std::vector<callback<A...>>::iterator it;
            it = std::find(callbacks.begin(), callbacks.end(), cb);

            if (it != callbacks.end())
                callbacks.erase(it);
            return (*this);
        };

        const inline invokable& operator = (const callback<A...>& cb) {
            callbacks.clear();
            (*this) += cb;
            return (*this);
        };

        const inline invokable& operator () (A... args) {
            invoke(args...);
            return (*this);
        };

        void hook(callback<A...> cb) {
            (*this) += cb;
        };

        void unhook(callback<A...> cb) {
            (*this) -= cb;
        };

        void hook_unhook(callback<A...> cb) {
            (*this) = cb;
        };

        void unhook_all() {
            callbacks.clear();
        };

        void invoke(A... args) {
            for (size_t i = 0; i < callbacks.size(); i++)
                callbacks[i](args...);
        };
    };

    #pragma endregion
#endif
