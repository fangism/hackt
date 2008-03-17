/**
	\file "Object/unroll/meta_loop.h"
	$Id: meta_loop.h,v 1.2 2008/03/17 23:02:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_LOOP_H__
#define	__HAC_OBJECT_UNROLL_META_LOOP_H__

#include <iosfwd>
#include "Object/lang/SPEC_fwd.h"
#include "Object/unroll/meta_loop_base.h"
#include "util/boolean_types.h"

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
	dump(const T&, ostream&, const C&, const char);

	template <class F>
	static
	good_bool
	unroll(const T&, const unroll_context& c, const node_pool_type&, F&, 
		const char*);

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

