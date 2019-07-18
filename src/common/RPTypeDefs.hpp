#ifndef RPTypeDefs_hpp
#define RPTypeDefs_hpp

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>

	// TODO: Define a type for tracking data such as counts, degrees, etc.
	// TODO: Make sure that we have safety checks for unsigned integer overflow.

	// source: https://stackoverflow.com/questions/34287842/c-strongly-typed-using-and-typedef

	////////////////////////////////////////////

	// define some tags to create uniqueness
	struct portal_tag {};
	struct cake_tag {};

	// a string-like identifier that is typed on a tag type
	template<class Tag>
	struct string_id
	{
		using tag_type = Tag;

		// needs to be default-constuctable because of use in map[] below
		string_id(std::string s) : _value(std::move(s)) {}
		string_id() : _value() {}

		// provide access to the underlying string value
		const std::string& value() const { return _value; }
	private:
		std::string _value;

		// will only compare against same type of id.
		friend bool operator < (const string_id& l, const string_id& r) {
			return l._value < r._value;
		}

		friend bool operator == (const string_id& l, const string_id& r) {
			return l._value == r._value;
		}

		// and let's go ahead and provide expected free functions
		friend
			auto to_string(const string_id& r)
			-> const std::string &
		{
			return r._value;
		}

		friend
			auto operator << (std::ostream& os, const string_id& sid)
			-> std::ostream &
		{
			return os << sid.value();
		}

		friend
			std::size_t hash_code(const string_id& sid)
		{
			std::size_t seed = typeid(tag_type).hash_code();
			seed ^= std::hash<std::string>()(sid._value);
			return seed;
		}

	};

	// let's make it hashable

	namespace std {
		template<class Tag>
		struct hash<string_id<Tag>>
		{
			using argument_type = string_id<Tag>;
			using result_type = std::size_t;

			result_type operator()(const argument_type& arg) const {
				return hash_code(arg);
			}
		};
	}


namespace RPGraph {
	// create some type aliases for ease of use
	using PortalId = string_id<portal_tag>;
	using CakeId = string_id<cake_tag>;

	using namespace std;

	// confirm that requirements are met
	auto main() -> int
	{
		PortalId portal_id("2");
		CakeId cake_id("is a lie");
		std::map<CakeId, PortalId> p_to_cake; // OK

		p_to_cake[cake_id] = portal_id; // OK
		//    p_to_cake[portal_id] = cake_id;   // COMPILER ERROR

		//    portal_id = cake_id;        // COMPILER ERROR
		//    portal_id = "1.0";          // COMPILER ERROR
		portal_id = PortalId("42"); // OK

		// extra checks

		std::unordered_map<CakeId, PortalId> hashed_ptocake;
		hashed_ptocake.emplace(CakeId("foo"), PortalId("bar"));
		hashed_ptocake.emplace(CakeId("baz"), PortalId("bar2"));

		for (const auto& entry : hashed_ptocake) {
			cout << entry.first << " = " << entry.second << '\n';

			// exercise string conversion
			auto s = to_string(entry.first) + " maps to " + to_string(entry.second);
			cout << s << '\n';
		}

		// if I really want to copy the values of dissimilar types I can express it:

		const CakeId cake1("a cake ident");
		auto convert = PortalId(to_string(cake1));

		cout << "this portal is called '" << convert << "', just like the cake called '" << cake1 << "'\n";


		return 0;
	}

	// Container for mapping contiguous_nid_t to comm_ids
	// typedef std::unordered_map<RPGraph::contiguous_nid_t, RPGraph::comm_id_t> nid_comm_map_t;

	namespace Enums {
		// Method to use to display.
		enum OutputMethod { PNG_WRITER };

		// Community algos available.
		enum CommAlgo { SCoDA_e }; /// Added `_e` to name of enum just for assurance during name lookup resolution.  

		// TODO: Could use an Enum and bitset to define options to GraphViewer without changing the function prototype if options are added. https://www.learncpp.com/cpp-tutorial/3-8a-bit-flags-and-bit-masks/
	} // namespace Enums

} // namespace RPGraph

#endif // RPTypeDefs_hpp 
