/**
	\file "Object/inst/instance_placeholder.h"
	Instance placeholders are used to represent instantiated collections
	that actually reside in footprints and other allocated locations.  
	$Id: instance_placeholder.h,v 1.6 2007/10/08 01:21:15 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_H__
#define	__HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_H__

#include "Object/inst/physical_instance_placeholder.h"	// for macros
#include "Object/type/canonical_type_fwd.h"	// for conditional
#include "Object/traits/class_traits_fwd.h"
// #include "util/memory/chunk_map_pool_fwd.h"
#include "util/inttypes.h"

/**
	Define to 1 if you want instance_arrays and scalars pool-allocated.  
	Hasn't been applied to placeholders yet.
 */
#define	POOL_ALLOCATE_INSTANCE_PLACEHOLDERS		1

namespace HAC {
namespace entity {
using std::list;

class meta_instance_reference_base;
class nonmeta_instance_reference_base;
class subinstance_manager;
template <bool> class internal_aliases_policy;
template <class> class instantiation_statement;
template <class> class instance_collection_pool_bundle;

//=============================================================================
#define	INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	INSTANCE_PLACEHOLDER_CLASS					\
instance_placeholder<Tag>

/**
	Interface to collection of instance aliases.  
	This abstract base class is dimension-generic.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
class instance_placeholder :
	public class_traits<Tag>::instance_placeholder_parent_type
{
public:
	typedef	class_traits<Tag>			traits_type;
private:
	typedef	Tag					category_type;
	typedef	typename traits_type::instance_placeholder_parent_type
							parent_type;
	typedef	INSTANCE_PLACEHOLDER_CLASS		this_type;
	FRIEND_PERSISTENT_TRAITS
public:
//	typedef	typename traits_type::type_ref_type	type_ref_type;
	typedef	typename traits_type::type_ref_ptr_type	type_ref_ptr_type;
	// placeholders need not know about instance aliases
	typedef	typename traits_type::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
	typedef	typename traits_type::simple_nonmeta_instance_reference_type
					simple_nonmeta_instance_reference_type;
	typedef	typename traits_type::member_simple_meta_instance_reference_type
				member_simple_meta_instance_reference_type;
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	instance_collection_pool_bundle<Tag>
					collection_pool_bundle_type;
protected:
	typedef	typename parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	typename parent_type::member_inst_ref_ptr_type
						member_inst_ref_ptr_type;
public:
	typedef	typename traits_type::instantiation_statement_type
					initial_instantiation_statement_type;
	typedef	count_ptr<const initial_instantiation_statement_type>
				initial_instantiation_statement_ptr_type;
protected:
	/**
		All collections track the first instantiation statement,
		for the sake of deducing the type.  
		Scalars instance collections need this too because
		of the possibility of relaxed template arguments.  
	 */
	initial_instantiation_statement_ptr_type
					initial_instantiation_statement_ptr;
protected:
	instance_placeholder();

	explicit
	instance_placeholder(const size_t d) :
		parent_type(d), 
		initial_instantiation_statement_ptr(NULL) { }

	instance_placeholder(const this_type&, const footprint&);

private:
	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO;

public:
	instance_placeholder(const scopespace& o, const string& n, 
		const size_t d);

	~instance_placeholder();

	instance_collection_generic_type*
	make_collection(footprint&) const;

	ostream&
	what(ostream&) const;

	ostream&
	type_dump(ostream&) const;

	ostream&
	dump_formal(ostream&) const;

	void
	attach_initial_instantiation_statement(
		const count_ptr<const instantiation_statement_base>&);

	index_collection_item_ptr_type
	get_initial_instantiation_indices(void) const;

	// this could just return hard-coded built-in type...
	// this returns the type as given by the first instantiation statement
	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const;

	// same thing, but covariant return type
	type_ref_ptr_type
	get_unresolved_type_ref_subtype(void) const;

	bool
	must_be_collectibly_type_equivalent(const this_type&) const;

	bool
	has_relaxed_type(void) const;

	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const;

	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const;

	member_inst_ref_ptr_type
	make_member_meta_instance_reference(const inst_ref_ptr_type&) const;

	UNROLL_PORT_ONLY_PROTO;

	static
	persistent*
	construct_empty(const int);

public:
	void
	collect_transient_info(persistent_object_manager&) const;

protected:
	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class instance_placeholder

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_H__

