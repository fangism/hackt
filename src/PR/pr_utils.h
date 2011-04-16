/**
	\file "PR/pr_utils.h"
	$Id: pr_utils.h,v 1.1.2.1 2011/04/16 01:51:54 fang Exp $
 */

#ifndef	__PR_PR_UTILS_H__
#define	__PR_PR_UTILS_H__

#include <iosfwd>
#include "util/IO_utils_fwd.h"

namespace PR {
using std::ostream;
using std::istream;
using util::write_value;
using util::read_value;

template <class T>
static
inline
void
save_array(ostream& o, const T& a) {
	typedef	typename T::const_iterator	const_iterator;
	write_value(o, a.size());
	const_iterator i(a.begin()), e(a.end());
	for ( ; i!=e; ++i) {
		i->save_checkpoint(o);
	}
}

template <class T>
static
inline
void
load_array(istream& i, T& a) {
	typedef	typename T::iterator		iterator;
	size_t s;
	read_value(i, s);
	a.resize(s);
	iterator j(a.begin()), e(a.end());
	for ( ; j!=e; ++j) {
		j->load_checkpoint(i);
	}
}

}	// end namespace PR

#endif	// __PR_PR_UTILS_H__

