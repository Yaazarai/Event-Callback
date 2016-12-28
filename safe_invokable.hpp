#ifndef INVOKABLE_EVENTS
#define INVOKABLE_EVENTS
    #include <functional>
    #include <type_traits>
    #include <vector>
    #include <algorithm>
    #include <mutex>

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
        void create(_Fx&& func, std::integer_sequence<std::size_t, Is...>) {
            this->func = std::function<void(A...)>(std::bind(func, placeholder_template<Is>()...));
        };
    public:
        const inline bool operator == (const callback& cb) {
            return (hash == cb.hash);
        };

        const inline bool operator != (const callback& cb) {
            return (hash != cb.hash);
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

        const inline callback& operator () (A... args) {
            func(args...);
            return (*this);
        };

        void invoke(A... args) {
            func(args...);
        };

        constexpr size_t hash_code() const throw() {
            return hash;
        };

        callback<A...> clone() {
            return callback<A...>(func);
        }
    };

    template<typename... A>
    class invokable {
    private:
        std::mutex safety_lock;
        std::vector<callback<A...>> callbacks;

    public:
        const inline invokable& operator += (const callback<A...>& cb) {
            hook(cb);
            return (*this);
        };

        const inline invokable& operator -= (const callback<A...>& cb) {
            unhook();
            return (*this);
        };

        const inline invokable& operator = (const callback<A...>& cb) {
            hook_unhook(cb);
            return (*this);
        };

        const inline invokable& operator () (A... args) {
            invoke(args...);
            return (*this);
        };

        void hook(callback<A...> cb) {
            safety_lock.lock();

            if (std::find(callbacks.begin(), callbacks.end(), cb) == callbacks.end())
                callbacks.push_back(cb);

            safety_lock.unlock();
        };

        void unhook(callback<A...> cb) {
            safety_lock.lock();

            typename std::vector<callback<A...>>::iterator it;
            it = std::find(callbacks.begin(), callbacks.end(), cb);

            if (it != callbacks.end())
                callbacks.erase(it);

            safety_lock.unlock();
        };

        void hook_unhook(callback<A...> cb) {
            safety_lock.lock();

            callbacks.clear();
            (*this) += cb;

            safety_lock.unlock();
        };

        void unhook_all() {
            safety_lock.lock();

            callbacks.clear();

            safety_lock.unlock();
        };

        void invoke(A... args) {
            safety_lock.lock();

            for (size_t i = 0; i < callbacks.size(); i++)
                callbacks[i](args...);

            safety_lock.unlock();
        };
    };
#endif
