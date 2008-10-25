#ifndef BOOST_UNORDERED_EXTENSION
#define BOOST_UNORDERED_EXTENSION
#include <boost/algorithm/string/predicate.hpp>
#include <boost/unordered_set.hpp>

namespace boost {
	namespace algorithm {
		struct iequal_to
					: std::binary_function<std::string, std::string, bool> {
				bool operator()(std::string const& x,
				                std::string const& y) const {
						return boost::algorithm::iequals(x, y, std::locale());
					}
			};

		struct ihash
					: std::unary_function<std::string, std::size_t> {
				std::size_t operator()(std::string const& x) const {
						std::size_t seed = 0;
						std::locale locale;

						for (std::string::const_iterator it = x.begin();
						        it != x.end(); ++it) {
								boost::hash_combine(seed, std::toupper(*it, locale));
							}

						return seed;
					}
			};

	};
	
	typedef boost::unordered_set<std::string, boost::algorithm::ihash, boost::algorithm::iequal_to> caseless_set;
};

#endif
