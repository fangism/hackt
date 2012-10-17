/**
	\file "util/IO_utils.hh"
	General-purpose handy utilities.
	$Id: IO_utils.hh,v 1.11 2009/09/28 06:56:56 fang Exp $
 */

#ifndef __UTIL_IO_UTILS_H__
#define __UTIL_IO_UTILS_H__

#include "util/IO_utils_fwd.hh"
#include "util/string_fwd.hh"
#include "util/STL/pair_fwd.hh"
#include "util/STL/valarray_fwd.hh"

#define	USING_IO_UTILS							\
using util::value_writer;						\
using util::value_reader;						\
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
struct value_writer {
	ostream& os;

	value_writer(ostream& o) : os(o) { }

	void
	operator () (const T& t);
};      // end struct value_writer

template <class T>
struct value_reader {
	istream& is;

	value_reader(istream& i) : is(i) { }

	void
	operator () (T& t);
};	// end struct value_reader

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

template <class Iter>
void	write_range(ostream& f, Iter, const Iter);

/// does not require iterator concept, just operator [] access
// cannot partial specialize functions, drat
template <class S>
void	write_array(ostream&, const S& );

template <class Iter>
void	read_range(istream&, Iter, const Iter);

template <class S>
void	read_sequence_in_place(istream& f, S& s);

template <class S>
void	read_sequence_prealloc(istream& f, S& s);

template <class S>
void	read_sequence_resize(istream& f, S& s);

template <class S>
void	read_sequence_back_insert(istream& f, S& s);

template <class S>
void	read_sequence_set_insert(istream& f, S& s);


template <class K, class T>
void	write_pair(ostream& f, const pair<K, T>& p);

template <class K, class T>
void	read_pair(istream& f, pair<K, T>& p);

template <class K, class T>
void	write_key_value_pair(ostream& f, const pair<const K, T>& p) {
	write_pair(f, p);
}

template <class K, class T>
void	read_key_value_pair(istream& f, pair<K, T>& p) {
	read_pair(f, p);
}

template <class K, class T>
inline
void	write_value(ostream& f, const pair<K, T>& p) {
	write_pair(f, p);
}

template <class K, class T>
inline
void	read_value(istream& f, pair<K, T>& p) {
	read_pair(f, p);
}

// TODO: support writing/reading of tuples for std: c++0x

template <class M>
void	write_map(ostream& f, const M& m);

template <class M>
void	read_map(istream& f, M& m);

}	// end namespace util

//=============================================================================

#endif	// __UTIL_IO_UTILS_H__

