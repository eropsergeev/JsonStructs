#ifndef _JSON_STRUCTS_HPP
#define _JSON_STRUCTS_HPP

#include <type_traits>
#include <utility>
#include <vector>
#include <algorithm>
#include <optional>
#include <memory>

#define JSON_PROPERTY(type, name) type name; \
    template<class T> \
    struct _SerializeHelper<offsetof(_selfType, name), T> { \
        constexpr static bool _value = 1; \
        constexpr static char _name[] = #name; \
        using _type = type; \
        using _parent = _selfType; \
    } \

#define JSON_SERIALIZABLE_STRUCT(_name, ...) struct _name { \
    using _selfType = _name; \
    template<size_t x, class T> \
    struct _SerializeHelper { const static bool _value = 0; }; \
    __VA_ARGS__ \
} \

namespace json_struct {

template<class T>
struct is_json_serializable_struct {
private:
    static void detect(...);
    template<class U>
    static typename U::_SerializeHelper<0, U> detect(const U&);
public:
    static const bool value = !std::is_same<decltype(detect(std::declval<T>())), void>::value;
};

template<class T>
constexpr bool is_json_serializable_struct_v = is_json_serializable_struct<T>::value;

namespace _helpers {
    template<class T>
    constexpr bool is_std_vector_v = 0;

    template<class T, class Alloc>
    constexpr bool is_std_vector_v<std::vector<T, Alloc>> = 1;

    template<class T>
    constexpr bool is_std_shared_ptr_v = 0;

    template<class T>
    constexpr bool is_std_shared_ptr_v<std::shared_ptr<T>> = 1;

    template<class T>
    constexpr bool is_std_unique_ptr_v = 0;

    template<class T>
    constexpr bool is_std_unique_ptr_v<std::unique_ptr<T>> = 1;

    template<class T>
    constexpr bool is_std_optional_v = 0;

    template<class T>
    constexpr bool is_std_optional_v<std::optional<T>> = 1;
};

template<class T, size_t offset = 0>
inline void deserialize(T &res, const nlohmann::json &j) {
    if constexpr (!is_json_serializable_struct_v<T>) {
        if constexpr (_helpers::is_std_vector_v<T>) {
            res.resize(j.size());
            for (size_t i = 0; i < j.size(); ++i) {
                deserialize(res[i], j[i]);
            }
        } else if constexpr (
            _helpers::is_std_optional_v<T> ||
            _helpers::is_std_shared_ptr_v<T> ||
            _helpers::is_std_unique_ptr_v<T> ||
            std::is_pointer_v<T>) {
            if (!j.is_null()) {
                using ContainsType = std::remove_reference_t<decltype(*res)>;
                if constexpr (_helpers::is_std_optional_v<T>) {
                    res.emplace();
                } else if constexpr (std::is_pointer_v<T>) {
                    delete res;
                    res = new ContainsType();
                } else if constexpr (_helpers::is_std_unique_ptr_v<T>) {
                    res = std::make_unique<ContainsType>();
                } else if constexpr (_helpers::is_std_shared_ptr_v<T>) {
                    res = std::make_shared<ContainsType>();
                }
                deserialize<std::remove_reference_t<decltype(*res)>, 0>(*res, j);
            }
        } else {
            res = j.get<T>();
        }
    } else {
        using HelperType = typename T::_SerializeHelper<offset, T>;
        if constexpr (HelperType::_value) {
            auto *p = reinterpret_cast<typename HelperType::_type *>(
                reinterpret_cast<char *>(&res) + offset
            );
            auto it = j.find(HelperType::_name);
            if (it != j.end()) {
                deserialize<typename HelperType::_type, 0>(*p, *it);
            } else {
                deserialize<typename HelperType::_type, 0>(*p, nlohmann::json());
            }
        }
        if constexpr (offset < sizeof(T)) {
            deserialize<T, offset + 1>(res, j);
        }
    }
}

template<class T>
inline T deserialize(const nlohmann::json &j) {
    T res;
    deserialize(res, j);
    return res;
}

template<class T>
inline nlohmann::json serialize(const T &obj);

template<class T, size_t offset = 0>
inline void serialize(nlohmann::json &res, const T &obj) {
    if constexpr (!is_json_serializable_struct_v<T>) {
        if constexpr (_helpers::is_std_vector_v<T>) {
            res = nlohmann::json::array();
            for (size_t i = 0; i < obj.size(); ++i) {
                res.push_back(serialize(obj[i]));
            }
        } else if constexpr (
            _helpers::is_std_optional_v<T> ||
            _helpers::is_std_shared_ptr_v<T> ||
            _helpers::is_std_unique_ptr_v<T> ||
            std::is_pointer_v<T>) {
            if (obj) {
                serialize<std::remove_reference_t<decltype(*obj)>, 0>(res, *obj);
            }
        } else {
            res = obj;
        }
    } else {
        using HelperType = typename T::_SerializeHelper<offset, T>;
        if constexpr (HelperType::_value) {
            auto *p = reinterpret_cast<const typename HelperType::_type *>(
                reinterpret_cast<const char *>(&obj) + offset
            );
            serialize<typename HelperType::_type, 0>(res[HelperType::_name], *p);
        }
        if constexpr (offset < sizeof(T)) {
            serialize<T, offset + 1>(res, obj);
        }
    }
}

template<class T>
inline nlohmann::json serialize(const T &obj) {
    nlohmann::json res;
    serialize(res, obj);
    return res;
}

};

#endif