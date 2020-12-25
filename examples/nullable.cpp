#include <iostream>
#include <vector>
#include <optional>
#include <memory>
#include <cassert>
#include <nlohmann/json.hpp> // you can use any other path to json.hpp file
#include "../jsonStructs.hpp"

using namespace std;
using nlohmann::json;
using namespace json_struct;

JSON_SERIALIZABLE_STRUCT(Foo,
    JSON_PROPERTY(int, mandatory);
    JSON_PROPERTY(optional<int>, opt1);
    JSON_PROPERTY(unique_ptr<int>, opt2);
    JSON_PROPERTY(shared_ptr<int>, opt3);
    JSON_PROPERTY(int*, opt4); // Will not be deleted on deserialization!
);

int main() {
    Foo foo = {.mandatory = 42, .opt1 = 37, .opt2 = nullptr, .opt3 = nullptr, .opt4 = nullptr};
    cout << serialize(foo).dump() << "\n";
    foo.opt1 = optional<int>();
    foo.opt2 = make_unique<int>(11);
    cout << serialize(foo).dump() << "\n";
    json valid = json::parse("{\"mandatory\": 42, \"opt1\": 1, \"opt2\": 2}");
    foo = deserialize<Foo>(valid);
    assert(foo.opt1);
    assert(foo.opt2);
    assert(!foo.opt3);
    assert(!foo.opt4);
    json invalid = json::parse("{\"opt1\": 1, \"opt2\": 2}");
    // foo = deserialize<Foo>(invalid); // error
}