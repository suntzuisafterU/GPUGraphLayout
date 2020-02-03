#include <iostream>
#include <string>
#include <map>
#include <unordered_map>

// a string-like identifier that is typed on a tag type
template<class Tag>
struct uint_id
{
    using tag_type = Tag;

    // needs to be default-constuctable because of use in map[] below
    uint_id(std::uint32_t s) : _value(std::move(s)) {}
    uint_id() : _value() {}

    // provide access to the underlying string value
    const std::uint32_t& value() const { return _value; }
private:
    std::uint32_t _value;

    // will only compare against same type of id.
    friend bool operator < (const uint_id& l, const uint_id& r) {
        return l._value < r._value;
    }

    friend bool operator == (const uint_id& l, const uint_id& r) {
        return l._value == r._value;
    }

    // and let's go ahead and provide expected free functions
    friend
    auto to_string(const uint_id& r)
    -> const std::uint32_t&
    {
        return r._value;
    }

    friend
    auto operator << (std::ostream& os, const uint_id& sid)
    -> std::ostream&
    {
        return os << sid.value();
    }

    friend
    std::size_t hash_code(const uint_id& sid)
    {
        std::size_t seed = typeid(tag_type).hash_code();
        seed ^= std::hash<std::uint32_t>()(sid._value);
        return seed;
    }

};

// let's make it hashable

namespace std {
    template<class Tag>
    struct hash<uint_id<Tag>>
    {
        using argument_type = uint_id<Tag>;
        using result_type = std::size_t;

        result_type operator()(const argument_type& arg) const {
            return hash_code(arg);
        }
    };
}

// define some tags to create uniqueness
struct portal_tag {};
struct cake_tag {};

// create some type aliases for ease of use
using PortalId = uint_id<portal_tag>;
using CakeId = uint_id<cake_tag>;

using namespace std;

// confirm that requirements are met
auto main() -> int
{
    PortalId portal_id(1);
    CakeId cake_id(2);
    std::map<CakeId, PortalId> p_to_cake; // OK

    p_to_cake[cake_id]   = portal_id; // OK
    //    p_to_cake[portal_id] = cake_id;   // COMPILER ERROR

    //    portal_id = cake_id;        // COMPILER ERROR
    //    portal_id = "1.0";          // COMPILER ERROR
    portal_id = PortalId(3); // OK

    // extra checks

    std::unordered_map<CakeId, PortalId> hashed_ptocake;
    hashed_ptocake.emplace(CakeId(4), PortalId(5));
    hashed_ptocake.emplace(CakeId(6), PortalId(7));

    for(const auto& entry : hashed_ptocake) {
        cout << entry.first << " = " << entry.second << '\n';

        // exercise string conversion
        auto s = to_string(entry.first) + " maps to " + to_string(entry.second);
        cout << s << '\n';
    }

    // if I really want to copy the values of dissimilar types I can express it:

    const CakeId cake1(9);

    return 0;
}