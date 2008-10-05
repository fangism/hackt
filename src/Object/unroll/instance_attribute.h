/**
	\file "Object/unroll/instance_attribute.h"
	Definition of generic attribute statements.  
	$Id: instance_attribute.h,v 1.1 2008/10/05 23:00:34 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTES_H__
#define	__HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTES_H__

#include <iosfwd>
#include "Object/unroll/instance_management_base.h"
#include "Object/lang/generic_attribute.h"
#include "Object/ref/references_fwd.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
template <class> class instance_alias_info;
using util::memory::count_ptr;
using util::persistent_object_manager;
using std::istream;

//=============================================================================
#define	INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE		template <class Tag>
#define	INSTANCE_ATTRIBUTE_CLASS		instance_attribute<Tag>

//=============================================================================
/**
	Meta-type specific instance attribute statement.
	TODO: register attributes!
 */
template <class Tag>
class instance_attribute : public instance_management_base {
	typedef	instance_attribute<Tag>			this_type;
public:
	typedef	class_traits<Tag>			traits_type;
	/**
		Should aggregate references be allowed? member references?
		Yes, eventually.  
	 */
	typedef	meta_instance_reference<Tag>		reference_type;
//	typedef	simple_meta_instance_reference<Tag>	reference_type;
	typedef	count_ptr<const reference_type>	reference_ptr_type;
	typedef	instance_alias_info<Tag>		alias_type;
private:
	/**
		The instance(s) to which relaxed parameters should be 
		attached.  
	 */
	reference_ptr_type				ref;
	/**
		The relaxed template parameters to associate with 
		each referenced instance.  
	 */
	generic_attribute_list_type			attrs;
public:
	instance_attribute();
	instance_attribute(const reference_ptr_type&, 
		const generic_attribute_list_type&);
	// default copy-ctor
	~instance_attribute();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	good_bool
	unroll(const unroll_context&) const;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class template_type_completion

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTES_H__

