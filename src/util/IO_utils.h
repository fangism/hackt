/**
	\file "IO_utils.h"
	General-purpose handy utilities.
	$Id: IO_utils.h,v 1.4 2005/01/28 19:58:46 fang Exp $
 */

#ifndef __IO_UTILS_H__
#define __IO_UTILS_H__

#include <iosfwd>
#include "string_fwd.h"
#include "STL/pair_fwd.h"
#include "STL/valarray_fwd.h"

#define	USING_IO_UTILS							\
using util::read_value;							\
using util::write_value;						\
using util::read_string;						\
using util::write_string;						\
using util::write_sequence;						\
using util::write_array;						\
using util::read_sequence_in_place;					\
using util::read_sequence_resize;					\
using util::read_sequence_back_insert;					\
using util::write_key_value_pair;					\
using util::read_key_value_pair;					\
using util::read_map;							\
using util::write_map;

//=============================================================================
// general utility functions

namespace util {
using std::string;
using std::ostream;
using std::istream;
using std::pair;

template <class T>
void	write_value(ostream& f, const T& v);

template <class T>
void	read_value(istream& f, T& v);


template <>
void	write_value<bool>(ostream& f, const bool& v);

template <>
void	read_value<bool>(istream& f, bool& v);


template <>
void	write_value<string>(ostream& f, const string& v);

template <>
void	read_value<string>(istream& f, string& v);


extern
void	write_string(ostream& f, const string& s);

extern
void	read_string(istream& f, string& s);


/***
	Consider container read/writing policies with trait tags...
***/

template <class S>
void	write_sequence(ostream& f, const S& s);

/// does not require iterator concept, just operator [] access
// cannot partial specialize functions, drat
template <class S>
void	write_array(ostream&, const S& );

template <class S>
void	read_sequence_in_place(istream& f, S& s);

template <class S>
void	read_sequence_resize(istream& f, S& s);

template <class S>
void	read_sequence_back_insert(istream& f, S& s);


template <class K, class T>
void	write_key_value_pair(ostream& f, const pair<const K, T>& p);

template <class K, class T>
void	read_key_value_pair(istream& f, pair<K, T>& p);

template <class M>
void	write_map(ostream& f, const M& m);

template <class M>
void	read_map(istream& f, M& m);

}	// end namespace util

//=============================================================================

#endif	// __IO_UTILS_H__

