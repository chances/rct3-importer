#ifndef PRETTYFOREACH_H_INCLUDED
#define PRETTYFOREACH_H_INCLUDED

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

template<class C, class T>
inline typename C::iterator find_in ( C& container, const T& value ) {
    return std::find(container.begin(), container.end(), value);
}

template<class C, class T>
inline typename C::const_iterator find_in ( const C& container, const T& value ) {
    return std::find(container.begin(), container.end(), value);
}

template<class C, class T>
inline bool contains ( C& container, const T& value ) {
    return std::find(container.begin(), container.end(), value) != container.end();
}

template<class C, class T>
inline bool contains ( const C& container, const T& value ) {
    return std::find(container.begin(), container.end(), value) != container.end();
}

template<class C, class T>
inline bool has ( C& container, const T& value ) {
    return container.find(value) != container.end();
}

template<class C, class T>
inline bool has ( const C& container, const T& value ) {
    return container.find(value) != container.end();
}

template<class C, class OutputIterator>
inline OutputIterator copy_all ( C& container, OutputIterator result ) {
  return std::copy(container.begin(), container.end(), result);
}

template<class C, class OutputIterator>
inline OutputIterator copy_all ( const C& container, OutputIterator result ) {
  return std::copy(container.begin(), container.end(), result);
}

template<class C, class Function>
inline Function for_all ( C& container, Function f ) {
  return std::for_each(container.begin(), container.end(), f);
}

template<class C, class Function>
inline Function for_all ( const C& container, Function f ) {
  return std::for_each(container.begin(), container.end(), f);
}

#if 0

namespace wx {
    template<class T>
    class cWrap {
        private:
            const T& t;
        public:
            typedef int iterator;
            typedef int const_iterator;

            cWrap(const T& _t): t(_t) {}
            iterator begin() { return 0; }
            iterator end() { return -1; }
    };

    template<>
    cWrap<wxControlWithItems*>::iterator cWrap<wxControlWithItems*>::end() {
        if (t) {
            return t->GetCount()-1;
        } else {
            return -1;
        }
    }

    template<class T>
    inline cWrap<T> wrap(const T& t) { return cWrap<T>(t); }

/*
    inline int range_begin( wxControlWithItems& x ) {
        return 0;
    }

    inline int range_end( wxControlWithItems& x ) {
        return x.GetCount() - 1;
    }

    inline const int range_begin( wxControlWithItems const & x ) {
        return 0;
    }

    inline const int range_end( wxControlWithItems const & x ) {
        return x.GetCount() - 1;
    }
*/
}

/*
namespace boost {
    template<>
    struct range_mutable_iterator< wx::wxControlWithItems >
    {
        typedef int type;
    };

    template<>
    struct range_const_iterator< wx::wxControlWithItems >
    {
        typedef int type;
    };
}
*/

#endif

#endif // PRETTYFOREACH_H_INCLUDED
