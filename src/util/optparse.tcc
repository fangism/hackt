/**
	\file "util/optparse.tcc"
	$Id: optparse.tcc,v 1.2 2010/05/13 00:32:04 fang Exp $
 */

#ifndef	__UTIL_OPTPARSE_TCC__
#define	__UTIL_OPTPARSE_TCC__

#include <iostream>
#include <iterator>
#include <algorithm>
#include "util/optparse.h"
#include "util/tokenize.h"
#include "util/string.h"

namespace util {
using std::copy;
using std::ostream_iterator;
using util::strings::string_to_num;
// currently hardcoded diagnostics to stderr
using std::cerr;
using std::endl;

//=============================================================================
// class options_map_impl method definitions

template <class T>
const T
options_map_impl<T>::default_value;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets a value of a structure member according to first value
	associated with option.  
	Such functions should be re-usable in util library.  
	\param V is value type, can be deduced from arguments!  
	\param opt key=values option value association.  
	\param mem is a pointer-to-member of type V.
	\return true on error
 */
template <class T>
template <class V>
bool
options_map_impl<T>::set_member_single_numeric_value(const option_value& opt,
		options_struct_type& t, V options_struct_type::*mem) {
	const size_t s = opt.values.size();
	if (s >= 1) {
		if (s > 1) {
		cerr << "Warning: extra arguments passed to \'" << opt.key
			<< "\' option ignored." << endl;
		}
		const string& arg(opt.values.front());
		const bool ret = string_to_num(arg, t.*mem);
		if (ret) {
			cerr << "Error: processing argument of \'" << opt.key
				<< "\' option, expecting numeric value, "
				"but got: " << arg << endl;
		}
		return ret;
	} else {
		cerr << "Warning: blank argument passed to \'" << opt.key
			<< "\' option where one is expected.  Ignoring."
			<< endl;
		return false;
	}

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If the string is blank, clear out the member string too.
	\return true on error
 */
template <class T>
bool
options_map_impl<T>::set_member_single_string(const option_value& opt,
                options_struct_type& t,
		string options_struct_type::*mem) {
	const size_t s = opt.values.size();
	if (s >= 1) {
		if (s > 1) {
		cerr << "Warning: extra arguments passed to \'" << opt.key
			<< "\' option ignored." << endl;
		}
		t.*mem = opt.values.front();
	} else {
		// if blank value, erase the string
		(t.*mem).clear();
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error
 */
template <class T>
template <class V, V K>
bool
options_map_impl<T>::set_member_constant(const option_value& opt,
		options_struct_type& t, V options_struct_type::*mem) {
	if (opt.values.size()) {
		cerr << "Warning: arguments passed to \'" << opt.key
			<< "\' option ignored." << endl;
	}
	t.*mem = K;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set a member of a member.
 */
template <class T>
template <class V1, class V2, V2 K>
bool
options_map_impl<T>::set_member_member_constant(const option_value& opt,
	options_struct_type& t, V1 options_struct_type::*mem1, 
	V2 V1::*mem2) {
	if (opt.values.size()) {
		cerr << "Warning: arguments passed to \'" << opt.key
			<< "\' option ignored." << endl;
	}
	t.*mem1.*mem2 = K;
	return false;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
template <class V>
ostream&
options_map_impl<T>::print_member_default(ostream& o,
		const options_struct_type& t, V options_struct_type::*mem) {
	return o << t.*mem;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default sequence printer.
 */
template <class T>
template <class VC>
ostream&
options_map_impl<T>::print_member_sequence(ostream& o,
		const options_struct_type& t, VC options_struct_type::*mem) {
	const VC& s(t.*mem);
	if (!s.empty()) {
		typedef typename VC::const_iterator      const_iterator;
		typedef typename VC::value_type  value_type;
		const_iterator i(s.begin()), l(--s.end());
		copy(i, l, std::ostream_iterator<value_type>(o, ","));
		// might want different delimiter at times...
		o << *l;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
ostream&
options_map_impl<T>::dump(ostream& o, const options_struct_type& t) const {
	typedef	typename opt_map_type::const_iterator    map_iterator;
	map_iterator i(options_map.begin());
	const map_iterator e(options_map.end());
	for ( ; i!=e; ++i) {
		const opt_entry& s(i->second);
	if (s.printer) {
		o << i->first << '=';
		(*s.printer)(o, t) << endl;
	}       // else is a meta option
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Print brief description of each configuration option.
	\param t whether or not to show type
	\param d whether or not to show default value
 */
template <class T>
ostream&
options_map_impl<T>::help(ostream& o, const bool t, const bool d) const {
	typedef	typename opt_map_type::const_iterator    map_iterator;
	map_iterator i(options_map.begin());
	const map_iterator e(options_map.end());
	for ( ; i!=e; ++i) {
		const opt_entry& s(i->second);
		o << i->first << ' ';
	if (t) {
		o << '(';
		if (s.type) {
			o << *s.type;
		}
		o << ')';
	}
		o << ": " << s.help;
	if (d) {
		if (s.printer) {
			o << " [";
			(*s.printer)(o, default_value);
			o << ']';
		} else o << " <pseudo-option>";
	}
		o << endl;
	}
	return o;
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_OPTPARSE_TCC__

