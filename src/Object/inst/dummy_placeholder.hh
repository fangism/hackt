/**
	\file "Object/inst/dummy_placeholder.hh"
	Instance placeholders are used to represent instantiated collections
	that actually reside in footprints and other allocated locations.  
	$Id: dummy_placeholder.hh,v 1.3 2011/02/25 23:19:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_DUMMY_PLACEHOLDER_H__
#define	__HAC_OBJECT_INST_DUMMY_PLACEHOLDER_H__

#include "Object/inst/physical_instance_placeholder.hh"	// for macros
#include "Object/type/canonical_type_fwd.hh"	// for conditional
#include "Object/traits/class_traits_fwd.hh"
// #include "util/memory/chunk_map_pool_fwd.hh"
#include "util/inttypes.h"

namespace HAC {
namespace entity {
using std::list;

#if 0
class meta_instance_reference_base;
class nonmeta_instance_reference_base;
class subinstance_manager;
template <bool> class internal_aliases_policy;
template <class> class instantiation_statement;
template <class> struct instance_collection_pool_bundle;
#endif

//=============================================================================
#define	DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	DUMMY_PLACEHOLDER_CLASS					\
dummy_placeholder<Tag>

/**
	Interface to collection of instance aliases.  
	This abstract base class is dimension-generic.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
class dummy_placeholder :
	public class_traits<Tag>::instance_placeholder_parent_type
{
public:
	typedef	class_traits<Tag>			traits_type;
private:
	typedef	Tag					category_type;
	typedef	typename traits_type::instance_placeholder_parent_type
							parent_type;
	typedef	DUMMY_PLACEHOLDER_CLASS		this_type;
	FRIEND_PERSISTENT_TRAITS
//	typedef	typename traits_type::type_ref_ptr_type	type_ref_ptr_type;
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
public:
	// placeholders need not know about instance aliases
	typedef	typename traits_type::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
#if 0
	typedef	typename traits_type::simple_nonmeta_instance_reference_type
					simple_nonmeta_instance_reference_type;
	typedef	typename traits_type::member_simple_meta_instance_reference_type
				member_simple_meta_instance_reference_type;
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	instance_collection_pool_bundle<Tag>
					collection_pool_bundle_type;
protected:
#endif
	typedef	typename parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	typename parent_type::member_inst_ref_ptr_type
						member_inst_ref_ptr_type;
#if NONMETA_MEMBER_REFERENCES
	typedef	typename parent_type::nonmeta_ref_ptr_type
						nonmeta_ref_ptr_type;
	typedef	typename parent_type::member_nonmeta_ref_ptr_type
						member_nonmeta_ref_ptr_type;
#endif
#if 0
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
#endif
protected:
	dummy_placeholder();

	explicit
	dummy_placeholder(const size_t d) :
		parent_type(d) { }

	dummy_placeholder(const this_type&, const footprint&);

private:
	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO;

public:
	dummy_placeholder(const typename parent_type::owner_ptr_raw_type& o,
		const string& n, const size_t d);

	~dummy_placeholder();

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

#if 0
	// same thing, but covariant return type
	type_ref_ptr_type
	get_unresolved_type_ref_subtype(void) const;
#endif

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

#if NONMETA_MEMBER_REFERENCES
	member_nonmeta_ref_ptr_type
	make_member_nonmeta_instance_reference(
		const nonmeta_ref_ptr_type&) const;
#endif

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

};	// end class dummy_placeholder

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_DUMMY_PLACEHOLDER_H__

