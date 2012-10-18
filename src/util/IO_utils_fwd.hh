/**
	\file "util/IO_utils_fwd.hh"
	General-purpose handy utilities for binary I/O.
	$Id: IO_utils_fwd.hh,v 1.3 2005/05/10 04:51:22 fang Exp $
 */

#ifndef __UTIL_IO_UTILS_FWD_H__
#define __UTIL_IO_UTILS_FWD_H__

#include <iosfwd>

//=============================================================================
// general utility functions

namespace util {
using std::ostream;
using std::istream;

template <class T>
void	write_value(ostream& f, const T& v);

template <class T>
void	read_value(istream& f, T& v);

/**
	Generic functor for value-writing.  
	This is partially specializable, 
	whereas the function templates are not.  
 */
template <class>
struct value_writer;

/**
	Generic functor for value-reading.  
	Partially specializable.  
 */
template <class>
struct value_reader;

}	// end namespace util

//=============================================================================

#endif	// __UTIL_IO_UTILS_FWD_H__

