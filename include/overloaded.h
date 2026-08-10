#ifndef REDIS_CLONE_OVERLOADED_H_
#define REDIS_CLONE_OVERLOADED_H_

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

#endif
