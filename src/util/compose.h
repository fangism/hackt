/**
	\file "compose.h"
	Unary and binary functor compositors.  
	This file is taken from Sean Mauch's Algorithms and 
	Data Structures class.  
	From California Institute of Technology, 2003-2004.  
	We've kept the original namespace to attribute credit to its source.  
	The original source can be found at:
	http://www.acm.caltech.edu/~seanm/projects/ads/html/compose_8h-source.html
	$Id: compose.h,v 1.6 2005/02/27 22:54:20 fang Exp $
 */

#ifndef	__UTIL_COMPOSE_H__
#define	__UTIL_COMPOSE_H__

#include <functional>
// note: <ext/functional> contains SGI extensions for some of these...

#define	USING_UTIL_COMPOSE						\
using ADS::unary_compose;						\
using ADS::unary_compose_void;						\
using ADS::binary_compose;

namespace ADS {
//=============================================================================
using std::unary_function;
using std::binary_function;

//=============================================================================
/**
	Composes f(g(x).
 */
template <class F, class G>
class unary_compose_unary_unary
	: public unary_function< typename G::argument_type,
					typename F::result_type > {
private:
	typedef unary_function< typename G::argument_type,
			       typename F::result_type > base_type;
protected:
	F _f;
	G _g;

public:
	typedef typename base_type::argument_type argument_type;
	typedef typename base_type::result_type result_type;

	unary_compose_unary_unary() : _f(), _g() { }
	unary_compose_unary_unary( const F& f, const G& g )
		: _f( f ), _g( g ) { }

	result_type
	operator() (const argument_type& x ) const { return _f( _g( x ) ); }
};	// end class unary_compose_unary_unary

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Composes f(g(x), but without any return value.
 */
template <class F, class G>
class unary_compose_unary_unary_void
	: public unary_function< typename G::argument_type, void > {
private:
	typedef unary_function< typename G::argument_type, void > base_type;
protected:
	F _f;
	G _g;

public:
	typedef typename base_type::argument_type argument_type;
	typedef typename base_type::result_type	result_type;

	unary_compose_unary_unary_void() : _f(), _g() { }
	unary_compose_unary_unary_void( const F& f, const G& g )
		: _f( f ), _g( g ) { }

	void
	operator() (const argument_type& x ) const { _f( _g( x ) ); }
};	// end class unary_compose_unary_unary_void

//-----------------------------------------------------------------------------
// helper wrapper function
template <class F, class G>
inline
unary_compose_unary_unary<F, G>
unary_compose(F f, G g) {
	return unary_compose_unary_unary<F, G>(f, g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class F, class G>
inline
unary_compose_unary_unary_void<F, G>
unary_compose_void(F f, G g) {
	return unary_compose_unary_unary_void<F, G>(f, g);
}

//=============================================================================
/**
	Composes f(g(x,y)).
 */
template <class F, class G>
class binary_compose_unary_binary
	: public binary_function< typename G::first_argument_type,
		 typename G::second_argument_type, typename F::result_type > {
private:
	typedef binary_function< typename G::first_argument_type,
		typename G::second_argument_type, typename F::result_type >
						base_type;
protected:
	F _f;
	G _g;

public:
	typedef typename base_type::first_argument_type first_argument_type;
	typedef typename base_type::second_argument_type second_argument_type;
	typedef typename base_type::result_type result_type;

	binary_compose_unary_binary() : _f(), _g() { }

	binary_compose_unary_binary( const F& f, const G& g )
		: _f( f ), _g( g ) { }

	result_type
	operator() (const first_argument_type& x,
			const second_argument_type& y ) const {
		return _f( _g( x, y ) );
	}
};	// end class binary_compose_unary_binary

//-----------------------------------------------------------------------------
// helper wrapper function
template <class F, class G>
inline
binary_compose_unary_binary<F, G>
binary_compose(F f, G g) {
	return binary_compose_unary_binary<F, G>(f, g);
}

//=============================================================================
/**
	Composes f(g(x), h(x)).
 */
template <class F, class G, class H>
class unary_compose_binary_unary
	: public unary_function< typename G::argument_type,
					typename F::result_type > {
private:
	typedef unary_function< typename G::argument_type,
			       typename F::result_type >
						base_type;
protected:
	F _f;
	G _g;
	H _h;

public:
	typedef typename base_type::argument_type argument_type;
	typedef typename base_type::result_type result_type;

	unary_compose_binary_unary() : _f(), _g(), _h() { }

	unary_compose_binary_unary( const F& f, const G& g, const H& h ) :
		_f( f ), _g( g ), _h( h ) { }

	result_type
	operator() (const argument_type& x ) const {
		return _f( _g( x ), _h( x ) );
	}
};	// end class unary_compose_binary_unary

//-----------------------------------------------------------------------------
// helper wrapper function
template <class F, class G, class H>
inline
unary_compose_binary_unary<F, G, H>
unary_compose(F f, G g, H h) {
	return unary_compose_binary_unary<F, G, H>(f, g, h);
}

//=============================================================================
/**
	Composes f(g(x), h(y)).  
 */
template <class F, class G, class H>
class binary_compose_binary_unary
	: public binary_function< typename G::argument_type,
		 typename H::argument_type, typename F::result_type > {
private:
	typedef binary_function< typename G::argument_type,
		typename H::argument_type, typename F::result_type >
						base_type;
protected:
	F _f;
	G _g;
	H _h;

public:
	typedef typename base_type::first_argument_type first_argument_type;
	typedef typename base_type::second_argument_type second_argument_type;
	typedef typename base_type::result_type result_type;

	binary_compose_binary_unary() : _f(), _g(), _h() { }

	binary_compose_binary_unary( const F& f, const G& g, const H& h )
		: _f( f ), _g( g ), _h( h ) { }

	result_type
	operator() (const first_argument_type& x,
		const second_argument_type& y ) const {
		return _f( _g( x ), _h( y ) );
	}
};	// end class binary_compose_binary_unary

//-----------------------------------------------------------------------------
// helper wrapper function
template <class F, class G, class H>
inline
binary_compose_binary_unary<F, G, H>
binary_compose(F f, G g, H h) {
	return binary_compose_binary_unary<F, G, H>(f, g, h);
}

//=============================================================================

}	// end namespace ADS

#endif	// __UTIL_COMPOSE_H__

