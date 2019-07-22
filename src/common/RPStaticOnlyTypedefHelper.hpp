
#ifndef RPStaticOnlyTypedefHelper_hpp
#define RPStaticOnlyTypedefHelper_hpp

///////////////////////////////////////////////////////
/// Strong typedefs as per source: https://stackoverflow.com/a/34288050/11385910
/// with modifications for uint32_t
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

#endif // RPStaticOnlyTypedefHelper_hpp

/// End code from stackoverflow source.
////////////////////////////////////////////////////
