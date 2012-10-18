/**
	\file "PR/pr_utils.hh"
	$Id: pr_utils.hh,v 1.2 2011/05/03 19:20:49 fang Exp $
 */

#ifndef	__PR_PR_UTILS_H__
#define	__PR_PR_UTILS_H__

#include <iosfwd>
#include "util/IO_utils_fwd.hh"

namespace HAC {
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
}	// end namespace HAC

#endif	// __PR_PR_UTILS_H__

