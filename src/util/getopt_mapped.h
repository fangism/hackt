/**
	\file "util/getopt_mapped.h"
	Using a map-like interface (and implementation) of
	getopt features.  
	$Id: getopt_mapped.h,v 1.1 2006/07/30 05:50:14 fang Exp $
 */

#ifndef	__UTIL_GETOPT_MAPPED_H__
#define	__UTIL_GETOPT_MAPPED_H__

#include <iosfwd>
#include <map>
#include <string>

namespace util {
//=============================================================================
/**
	Thrown to indicate option processing error.  
	Keep all methods inlined.  
 */
struct getopt_exception {
	int				exit_status;
	std::string			message;

	explicit
	getopt_exception(const int e) : exit_status(e), message() { }

	getopt_exception(const int e, const std::string& m)
			: exit_status(e), message(m) { }

	// default destructor and copy-constructors

};	// end struct getopt_exception

//=============================================================================
extern
void
unknown_option(std::ostream&, const int);

//=============================================================================
/**
	Uses an expandable map-lookup instead of fixed switch-case.  
	Consider templating by CharType as well.  
	TODO: augment mapped entry with brief help string.  
	TODO: store and update local copies of getopt state variables.  
 */
template <class O>
class getopt_map {
public:
	/// the type of the object modified by the option processor
	typedef	O			option_type;
	typedef	int			getopt_char_type;
	/// function prototype type of option modifier
	typedef		void function_type(option_type&);
	typedef		void function2_type(option_type&, const char*);
	typedef	function_type*		function_ptr_type;
	typedef	function2_type*		function2_ptr_type;
private:
	/// whether or not to conform to POSIX getopt behavior (default true)
	bool				_is_posix;
	/**
		All mutator methods invalidate the cache.
		Cache is automagically updated when class const
		methods are used.  
	 */
	mutable bool			_cache_valid;
	/// the option string (automatically generated)
	mutable std::string		optstring;

	/**
		Could use sparse map/tree or even dense fixed size array.
	 */
	typedef	std::map<getopt_char_type, function_ptr_type>	map_type;
	typedef	std::map<getopt_char_type, function2_ptr_type>	map2_type;
	/// the option map
	map_type			option_map;
	map2_type			option2_map;

#if 0
	static	function_type		_unknown_option;
	static	function_type		_missing_argument;
#endif
public:
	getopt_map();
	~getopt_map();

	/**
		Overloaded method knows which map to classify
		the option under.  
	 */
	void
	add_option(const getopt_char_type, const function_ptr_type);

	/**
		Overloaded method knows which map to classify
		the option under.  
	 */
	void
	add_option(const getopt_char_type, const function2_ptr_type);

	void
	clear_option(const getopt_char_type);

	void
	update(void) const;

	void
	set_posix(const bool b) {
		if (this->_is_posix != b) {
			this->_is_posix = b;
			this->_cache_valid = false;
		}
	}

	int
	operator () (const int, char* const[], option_type&) const;

};	// end class getopt_map

//=============================================================================
}	// end namespace util

#endif	// __UTIL_GETOPT_MAPPED_H__

