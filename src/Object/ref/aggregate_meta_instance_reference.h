/**
	\file "Object/ref/aggregate_meta_instance_reference.h"
	This is going to be exciting...
	$Id: aggregate_meta_instance_reference.h,v 1.5 2006/08/08 05:46:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_H__
#define	__HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_H__

#include <iosfwd>
#include <vector>
#include "util/memory/count_ptr.h"
#include "Object/ref/aggregate_meta_instance_reference_base.h"
#include "Object/ref/meta_instance_reference_subtypes.h"

namespace HAC {
namespace entity {
using std::istream;
using std::ostream;
using util::memory::count_ptr;

#define	AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE		\
template <class Tag>

#define	AGGREGATE_META_INSTANCE_REFERENCE_CLASS				\
aggregate_meta_instance_reference<Tag>

//=============================================================================
/**
	Concatenation or construction? or both! 
	Use a boolean flag to determine.
	NOTE: probably no other class will derive from this.  
	TODO: Will also need aggregate_meta_value_reference counterpart.  
 */
AGGREGATE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class aggregate_meta_instance_reference : 
	public aggregate_meta_instance_reference_base, 
	public meta_instance_reference<Tag> {
	typedef	AGGREGATE_META_INSTANCE_REFERENCE_CLASS		this_type;
	typedef	meta_instance_reference<Tag>			parent_type;
	typedef	class_traits<Tag>			traits_type;
public:
	typedef	typename traits_type::alias_collection_type
							alias_collection_type;
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	count_ptr<const parent_type>		subreference_ptr_type;
	typedef	std::vector<subreference_ptr_type>	subreference_array_type;
private:
	typedef	typename subreference_array_type::const_iterator
							const_iterator;
private:
	subreference_array_type			subreferences;
public:
	aggregate_meta_instance_reference();

	explicit
	aggregate_meta_instance_reference(const bool);

	~aggregate_meta_instance_reference();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	ostream&
	dump_type_size(ostream&) const;

	size_t
	dimensions(void) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	UNROLL_SCALAR_SUBSTRUCTURE_REFERENCE_PROTO;

	LOOKUP_FOOTPRINT_FRAME_PROTO;

	bad_bool
	unroll_references_packed(const unroll_context&, alias_collection_type&) const;

	good_bool
	append_meta_instance_reference(
		const count_ptr<const meta_instance_reference_base>&);

	good_bool
	append_meta_instance_reference(
		const count_ptr<const parent_type>&);
private:
	using parent_type::unroll_references_packed_helper;
	using parent_type::unroll_references_packed_helper_no_lookup;

private:
	excl_ptr<port_connection_base>
	make_port_connection_private(
		const count_ptr<const meta_instance_reference_base>&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS

};	// end class aggregate_meta_instance_reference
//=============================================================================

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_INSTANCE_REFERENCE_H__

