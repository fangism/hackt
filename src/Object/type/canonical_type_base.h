/**
	\file "Object/type/canonical_type_base.h"
	$Id: canonical_type_base.h,v 1.2 2005/09/04 21:14:58 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CANONICAL_TYPE_BASE_H__
#define	__OBJECT_TYPE_CANONICAL_TYPE_BASE_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class template_actuals;
class const_param_expr_list;
using std::istream;
using std::ostream;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================

/**
	Base class with just const_param_list.
	Not polymorphic.  
	This is actually useful for process_definition::unroll_complete_type.
 */
class canonical_type_base {
	typedef	canonical_type_base		this_type;
public:
	typedef	const_param_expr_list		param_list_type;
	/**
		Consider: template actuals are split into strict
		and relaxed parameters.  
		Should this list unify them into one list?
		Unifying into one list makes it easier to 
			compare and sort the keys in the footprint_manager.  
		However will be a little less convenient when
			the lists are to be split up.  
	 */
	typedef	count_ptr<const param_list_type>
						const_param_list_ptr_type;
	typedef	count_ptr<param_list_type>
						param_list_ptr_type;
protected:
	const_param_list_ptr_type			param_list_ptr;
public:
	canonical_type_base();

	explicit
	canonical_type_base(const const_param_list_ptr_type&);

	// default copy-constructor suffices

	~canonical_type_base();

	const const_param_list_ptr_type&
	get_raw_template_params(void) const { return param_list_ptr; }

// protected:
// grant public access and use
	template_actuals
	get_template_params(const size_t) const;

	ostream&
	dump_template_args(ostream&, const size_t) const;

protected:
	void
	combine_relaxed_actuals(const const_param_list_ptr_type&);

	bool
	match_relaxed_actuals(const const_param_list_ptr_type&) const;

#if 0
// object persistence
// let child call directly
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
#endif

};	// end class canonical_type

//=============================================================================
// possilbly specialize built-in data types, but require same interface

// possilbly specialize built-in channel type, but require same interface

//=============================================================================
}	// end namespace entity
}	// end namespace ART


#endif	// __OBJECT_TYPE_CANONICAL_TYPE_BASE_H__

