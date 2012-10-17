/**
	\file "Object/unroll/meta_loop_base.hh"
	$Id: meta_loop_base.hh,v 1.8 2008/03/17 23:02:40 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_LOOP_BASE_H__
#define	__HAC_OBJECT_UNROLL_META_LOOP_BASE_H__

#include <iosfwd>
#include "util/persistent_fwd.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
class meta_range_expr;
struct pint_tag;
template <class> class value_placeholder;
class footprint;
template <class, size_t> class value_array;
using std::ostream;
using std::istream;
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::persistent_object_manager;

template <class T> struct meta_loop;

//=============================================================================
/**
	Base structure for meta-language loop construct.  
	May need to derive from scopespace after introducing placeholders
		to distinguish from collections and make footprint lookup 
		consistent in the unroll_context.  
 */
class meta_loop_base {
public:
	typedef	value_placeholder<pint_tag>		pint_placeholder_type;
	typedef	count_ptr<pint_placeholder_type>	ind_var_ptr_type;
protected:
	typedef	value_array<pint_tag, 0>		pint_scalar;
public:
	typedef	count_ptr<const meta_range_expr>	range_ptr_type;

protected:
	ind_var_ptr_type			ind_var;
	range_ptr_type				range;

	meta_loop_base();
	meta_loop_base(const ind_var_ptr_type&, const range_ptr_type&);
	~meta_loop_base();

	// instantiates an actual variable into the footprint
	never_ptr<pint_scalar>
	initialize_footprint(footprint&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
};

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_META_LOOP_BASE_H__

