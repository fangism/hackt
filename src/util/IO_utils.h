/**
	\file "IO_utils.h"
	General-purpose handy utilities.
	$Id: IO_utils.h,v 1.2 2004/12/16 01:08:53 fang Exp $
 */

#ifndef __IO_UTILS_H__
#define __IO_UTILS_H__

#include <iosfwd>
#include "string_fwd.h"
#include "STL/pair_fwd.h"

#define	USING_IO_UTILS							\
using util::read_value;							\
using util::write_value;						\
using util::read_string;						\
using util::write_string;						\
using util::write_sequence;						\
using util::read_sequence_in_place;					\
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
void	write_value<string>(ostream& f, const string& v);

template <>
void	read_value<string>(istream& f, string& v);


extern
void	write_string(ostream& f, const string& s);

extern
void	read_string(istream& f, string& s);


template <template <class> class S, class T>
void	write_sequence(ostream& f, const S<T>& s);

template <template <class> class S, class T>
void	read_sequence_in_place(istream& f, S<T>& s);

template <template <class> class S, class T>
void	read_sequence_back_insert(istream& f, S<T>& s);


template <class K, class T>
void	write_key_value_pair(ostream& f, const pair<const K, T>& p);

template <class K, class T>
void	read_key_value_pair(istream& f, pair<K, T>& p);

template <template <class, class> class M, class K, class T>
void	write_map(ostream& f, const M<K,T>& m);

template <template <class, class> class M, class K, class T>
void	read_map(istream& f, M<K,T>& m);

}	// end namespace util

//=============================================================================

#endif	// __IO_UTILS_H__

