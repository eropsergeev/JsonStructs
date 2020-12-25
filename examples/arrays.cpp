#include <iostream>
#include <vector>
#include <nlohmann/json.hpp> // you can use any other path to json.hpp file
#include "../jsonStructs.hpp"

using namespace std;
using nlohmann::json;
using namespace json_struct;

JSON_SERIALIZABLE_STRUCT(Foo,
    JSON_PROPERTY(vector<string>, a);
    JSON_SERIALIZABLE_STRUCT(Bar,
        JSON_PROPERTY(int, a);
        JSON_PROPERTY(int, b);
    );
    JSON_PROPERTY(vector<Bar>, b);
    JSON_PROPERTY(vector<vector<int>>, c);
);

int main() {
    json j = json::parse("{\"a\": [\"Hello\", \"world\"], \"b\": [{\"a\": 1, \"b\": 2}, {\"a\": 10, \"b\": 20}, {\"a\": 42, \"b\": 42}], \"c\": [[1, 2], [], [3, 4, 5, 6]]}");
    auto foo = deserialize<Foo>(j);
    for (auto x : foo.a) {
        cout << x << " ";
    }
    cout << "\n";
    for (auto x : foo.b) {
        cout << x.a << "," << x.b << " ";
    }
    cout << "\n";
    for (auto &x : foo.c) {
        for (auto y : x) {
            cout << y << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}