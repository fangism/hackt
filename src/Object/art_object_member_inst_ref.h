/**
	\file "art_object_inst_ref_base.h"
	Base class family for instance references in ART.  
	$Id: art_object_member_inst_ref.h,v 1.2 2005/02/27 22:54:17 fang Exp $
 */

#ifndef	__ART_OBJECT_MEMBER_INST_REF_H__
#define	__ART_OBJECT_MEMBER_INST_REF_H__

#include "art_object_inst_ref_base.h"

namespace ART {
namespace entity {

using std::ostream;
using std::istream;
USING_LIST
using namespace util::memory;

//=============================================================================
#define	MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	MEMBER_INSTANCE_REFERENCE_CLASS					\
member_instance_reference<Tag>

/**
	Re-usable type-specific member_instance_reference class template.  
	This class is final, nothing else derives from it, 
	no need for virtual functions.  
	\param InstRef must be a type derived from simple_instance_reference.
	Consider moving this class definition to "art_object_inst_ref.h"?
 */
MEMBER_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class member_instance_reference :
	public class_traits<Tag>::instance_reference_type {
private:
	typedef	MEMBER_INSTANCE_REFERENCE_CLASS		this_type;
public:
	/// the underlying type of the member instance referenced
	typedef	typename class_traits<Tag>::instance_reference_type
							parent_type;
	typedef	typename class_traits<Tag>::instance_collection_generic_type
						instance_collection_generic_type;
	typedef	typename class_traits<Tag>::alias_collection_type
						alias_collection_type;
	typedef	never_ptr<const instance_collection_generic_type>
						instance_collection_ptr_type;
	/// the containing type, whose member is referenced
	typedef	instance_reference_base			base_inst_type;
	// should be kept consistent with
	//	instance_collection_base::inst_ref_ptr_type
	typedef	count_ptr<const base_inst_type>		base_inst_ptr_type;
protected:
	/**
		The owning base instance reference, 
		must have dimension-0, scalar... for now
		Is type limited to simple? or can it be nested member?
	 */
	const base_inst_ptr_type			base_inst_ref;
private:
	member_instance_reference();
public:
	member_instance_reference(const base_inst_ptr_type& b, 
		const instance_collection_ptr_type m);

	~member_instance_reference();

	ostream&
	what(ostream&) const;

// already implicit
//	using parent_type::make_aliases_connection_private;

	// overrides parent's implementation.  
	bool
	unroll_references(unroll_context&, alias_collection_type&) const;

public:
	// final, non-virtual
	PERSISTENT_METHODS_DECLARATIONS
};	// end class member_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_MEMBER_INST_REF_H__

