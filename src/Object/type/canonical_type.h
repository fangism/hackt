/**
	\file "Object/type/canonical_type.h"
	$Id: canonical_type.h,v 1.1.2.3 2005/08/11 21:52:52 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CANONICAL_TYPE_H__
#define	__OBJECT_TYPE_CANONICAL_TYPE_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class definition_base;
class const_param_expr_list;
using std::istream;
using std::ostream;
using util::memory::count_ptr;
using util::memory::never_ptr;
using util::persistent_object_manager;

//=============================================================================

#define	CANONICAL_TYPE_TEMPLATE_SIGNATURE	template <class DefType>
#define	CANONICAL_TYPE_CLASS			canonical_type<DefType>

/**
	A canonical type references a non-typedef definition
	with only constant template parameters, if applicable.  
	SHould be implemented with same interface as 
		fundamental_type_reference.  
	Q: use const_param_expr_list or pointer?
	A: depends on whether or not this is expected to be copied much.  
		Since it will be used with unroll context actuals, 
		it should probably be a pointer.  
		This will also be used in footprint lookups.  
	TODO: import more standard interfaces from fundamental_type_reference
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
class canonical_type {
	typedef	CANONICAL_TYPE_CLASS		this_type;
public:
	/**
		This must be a non-typedef definition.  
	 */
	typedef	DefType				canonical_definition_type;
	typedef	never_ptr<const canonical_definition_type>
						canonical_definition_ptr_type;
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
						param_list_ptr_type;
private:
	canonical_definition_ptr_type		canonical_definition_ptr;
	param_list_ptr_type			param_list_ptr;
public:
	canonical_type();

	explicit
	canonical_type(const canonical_definition_ptr_type);

	canonical_type(const canonical_definition_ptr_type, 
		const param_list_ptr_type&);

	// default copy-constructor suffices

	~canonical_type();

	never_ptr<const definition_base>
	get_base_def(void) const { return this->canonical_definition_ptr; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	ostream&
	dump_template_args(ostream&) const;

	bool
	is_strict(void) const;

	bool
	is_relaxed(void) const { return !this->is_strict(); }

	operator bool () { return this->canonical_definition_ptr; }

	static
	ostream&
	type_mismatch_error(ostream&, const this_type&, const this_type&);

//	make_unroll_context

	// like fundamental_type_reference::unroll_register_complete_type()
	void
	register_definition_footprint(void) const;

public:
// object persistence
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class canonical_type

//=============================================================================
// possilbly specialize built-in data types, but require same interface

// possilbly specialize built-in channel type, but require same interface

//=============================================================================
}	// end namespace entity
}	// end namespace ART


#endif	// __OBJECT_TYPE_CANONICAL_TYPE_H__

