/**
	\file "util/boolean_types.h"
	Boolean-like classes with different semantics.  
	The purpose of these classes is to provide self-documenting
	boolean values, which prevent user errors.  

	$Id: boolean_types.h,v 1.5 2006/07/04 07:26:24 fang Exp $
	"When I'm good, I'm good, but when I'm bad, I'm better." -- Mae West
 */

#ifndef	__UTIL_BOOLEAN_TYPES_H__
#define	__UTIL_BOOLEAN_TYPES_H__

namespace util {

struct good_bool;
struct bad_bool;

//=============================================================================
/**
	Is interpreted as "good" when the value is true.  
	Useful for indicating the sense of a return value.
 */
struct good_bool {
	/**
		Public value member, indicating status.  
	 */
	bool	good;

	/**
		Default constructor.  
		Value is "good" until set otherwise.  
	 */
	good_bool() : good(true) { }

	/**
		No implicit conversion from plain bools, only explicit.
	 */
	explicit
	good_bool(const bool b) : good(b) { }

	/**
		Implicit conversion of from opposite type of bool.  
	 */
	good_bool(const bad_bool&);

	// default copy construction
	// default assignment

	/// default destructor
	~good_bool() { }

	// no implicit conversion operator

	// is this obfuscating?
	good_bool
	operator && (const good_bool& b) const {
		return good_bool(this->good && b.good);
	}

	good_bool&
	operator &= (const good_bool& c) {
		this->good &= c.good;
		return *this;
	}

};	// end struct good_bool

//=============================================================================
/**
	Is interpreted as "bad" when the value is true.
	Useful for indicating the sense of a return value.
 */
struct bad_bool {
	/**
		Public value member, indicating status.  
	 */
	bool	bad;

	/**
		Default constructor.
		Value is "good" until set otherwise.
	 */
	bad_bool() : bad(false) { }

	/**
		No implicit conversion from plain bools, only explicit.
	 */
	explicit
	bad_bool(const bool b) : bad(b) { }

	/**
		Implicit conversion of from opposite type of bool.  
	 */
	bad_bool(const good_bool&);

	// default copy construction
	// default assignment

	/// default destructor
	~bad_bool() { }

	// no implicit conversion operator

	bad_bool
	operator || (const bad_bool& b) const {
		return bad_bool(this->bad || b.bad);
	}

	bad_bool&
	operator |= (const bad_bool& c) {
		this->bad |= c.bad;
		return *this;
	}

};	// end struct bad_bool

//-----------------------------------------------------------------------------

inline
good_bool::good_bool(const bad_bool& b) : good(!b.bad) { }

inline
bad_bool::bad_bool(const good_bool& b) : bad(!b.good) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// some handy overloaded functions
// so user doesn't need to remember a priori the return type

static
inline
bool
is_good(const good_bool& g) { return g.good; }

static
inline
bool
is_good(const bad_bool& b) { return !b.bad; }

static
inline
bool
is_bad(const good_bool& g) { return !g.good; }

static
inline
bool
is_bad(const bad_bool& b) { return b.bad; }

//=============================================================================

}	// end namespace util

#endif	// __UTIL_BOOLEAN_TYPES_H__

