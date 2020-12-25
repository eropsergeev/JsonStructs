#include <iostream>
#include <nlohmann/json.hpp> // you can use any other path to json.hpp file
#include "../jsonStructs.hpp"

using namespace std;
using nlohmann::json;
using namespace json_struct;

JSON_SERIALIZABLE_STRUCT(Foo,
    JSON_PROPERTY(int, i);
    JSON_PROPERTY(string, str);
    JSON_PROPERTY(float, fl);
    JSON_SERIALIZABLE_STRUCT(Bar,
        JSON_PROPERTY(int, a);
        JSON_PROPERTY(int, b);
    );
    JSON_PROPERTY(Bar, bar);
);

int main() {
    json j = json::parse("{\"i\": 42, \"fl\": 3.14, \"str\": \"abacaba\", \"bar\": {\"a\": 1, \"b\": 2}}");
    auto foo = deserialize<Foo>(j);
    cout << foo.i << "\n";
    cout << foo.str << "\n";
    cout << foo.fl << "\n";
    cout << foo.bar.a << "\n";
    cout << foo.bar.b << "\n";
    Foo foo2;
    foo2.i = 34;
    foo2.str = "Hello!";
    foo2.fl = -0.5;
    foo2.bar = {.a = 10, .b = 20};
    cout << serialize(foo2).dump() << "\n";
}