/**
	\file "Object/type/canonical_type.h"
	$Id: canonical_type.h,v 1.1.2.1 2005/08/10 20:30:56 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CANONICAL_TYPE_H__
#define	__OBJECT_TYPE_CANONICAL_TYPE_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
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
	Q: use const_param_expr_list or pointer?
	A: depends on whether or not this is expected to be copied much.  
		Since it will be used with unroll context actuals, 
		it should probably be a pointer.  
		This will also be used in footprint lookups.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
class canonical_type {
public:
	/**
		This must be a non-typedef definition.  
	 */
	typedef	DefType				canonical_definition_type;
	typedef	never_ptr<const canonical_definition_type>
						canonical_definition_ptr_type;
	typedef	const_param_expr_list		param_list_type;
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

	~canonical_type();

//	make_unroll_context

// object persistence
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class canonical_type

//=============================================================================
}	// end namespace entity
}	// end namespace ART


#endif	// __OBJECT_TYPE_CANONICAL_TYPE_H__

