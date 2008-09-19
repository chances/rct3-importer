#ifndef PRETTYFOREACH_H_INCLUDED
#define PRETTYFOREACH_H_INCLUDED

#include <algorithm>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

namespace pretty {
/*
template<class T>
struct reverse_view {
    typedef typename T::reverse_iterator iterator;
    typedef typename T::const_reverse_iterator const_iterator;
    T& sequence;

    reverse_view(T& t): sequence(t) {}

    iterator begin() { return sequence.rbegin(); }
    const_iterator begin() const { return sequence.rbegin(); }
    iterator end() { return sequence.rend(); }
    const_iterator end() const { return sequence.rend(); }
};
*/
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

template<class C, class P>
inline typename C::iterator find_in_if ( C& container, const P& predicate ) {
    return std::find_if(container.begin(), container.end(), predicate);
}

template<class C, class P>
inline typename C::const_iterator find_in_if ( const C& container, const P& predicate ) {
    return std::find_if(container.begin(), container.end(), predicate);
}

template<class C, class P>
inline bool contains_if ( C& container, const P& predicate ) {
    return std::find_if(container.begin(), container.end(), predicate) != container.end();
}

template<class C, class P>
inline bool contains_if ( const C& container, const P& predicate ) {
    return std::find_if(container.begin(), container.end(), predicate) != container.end();
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
