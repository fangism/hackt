/**
	\file "Object/unroll/meta_loop.hh"
	$Id: meta_loop.hh,v 1.4 2010/07/09 02:14:14 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_LOOP_H__
#define	__HAC_OBJECT_UNROLL_META_LOOP_H__

#include <iosfwd>
#include "Object/lang/SPEC_fwd.hh"
#include "Object/unroll/meta_loop_base.hh"
#include "util/boolean_types.hh"

namespace HAC {
namespace entity {
using std::ostream;
using SPEC::node_pool_type;
using util::good_bool;

//=============================================================================
/**
	Wrapper class of just static helper methods.  
 */
template <class T>
struct meta_loop {
	// underlying list type
	typedef	typename T::implementation_type		implementation_type;

	template <class C>
	static
	ostream&
	dump(const T&, ostream&, const C&, const char, 
		ostream& (T::implementation_type::*)(ostream&, const C&) const
			= &T::implementation_type::dump);

	static
	good_bool
	unroll(const T&, const unroll_context& c, const char*);

	static
	void
	collect_transient_info(const T&, persistent_object_manager&);

	static
	void
	write_object(const T&, const persistent_object_manager&, ostream&);

	static
	void
	load_object(T& mc, const persistent_object_manager&, istream&);

};	// end struct meta_loop

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_META_LOOP_H__

