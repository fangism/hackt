/**
	\file "Object/inst/value_array.hh"
	$Id: value_array.hh,v 1.4 2006/11/11 02:20:13 fang Exp $
	This fail spawned from:
	Id: value_collection.hh,v 1.19.2.1 2006/10/22 08:03:28 fang Exp
 */

#ifndef	__HAC_OBJECT_INST_VALUE_ARRAY_H__
#define	__HAC_OBJECT_INST_VALUE_ARRAY_H__

#include "Object/inst/value_collection.hh"
#include "util/multikey_map.hh"

namespace HAC {
namespace entity {
// template <class> class param_instantiation_statement;
using util::default_multikey_map;

//=============================================================================
#define VALUE_ARRAY_TEMPLATE_SIGNATURE					\
template <class Tag, size_t D>

#define	VALUE_ARRAY_CLASS						\
value_array<Tag,D>

/**
	Dimension-specific array of parameters.
	NOTE: no need to separate key from value in map because
	these value collections are not heavily replicated.
 */
VALUE_ARRAY_TEMPLATE_SIGNATURE
class value_array : public value_collection<Tag> {
private:
	typedef VALUE_ARRAY_CLASS			this_type;
	typedef	value_collection<Tag>			parent_type;
friend class value_collection<Tag>;
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::value_type	value_type;
	typedef	typename traits_type::instance_type	element_type;

	// later change this to multikey_set or not?
	/// Type for actual values, including validity and status.
private:
	typedef	default_multikey_map<D, pint_value_type, element_type>
							__helper_map_type;
	typedef	typename __helper_map_type::type	map_type;
	typedef	util::multikey_map<D, pint_value_type, element_type, map_type>
							collection_type;
public:
	typedef	typename collection_type::key_type	key_type;
	typedef	typename traits_type::const_collection_type
							const_collection_type;
	typedef	typename traits_type::value_reference_collection_type
					value_reference_collection_type;
	typedef	typename parent_type::value_placeholder_type
							value_placeholder_type;
	typedef typename parent_type::value_placeholder_ptr_type
						value_placeholder_ptr_type;
private:
	/// the collection of boolean instances
	collection_type					collection;
	// value cache is not persistent
	const_collection_type				cached_values;
	// tracking validity and density of the value cache?
public:
	value_array();

public:
	explicit
	value_array(const value_placeholder_ptr_type);

	~value_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_values(ostream& o) const;

	// update this to accept const_range_list instead
	good_bool
	instantiate_indices(const const_range_list&);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	LOOKUP_VALUE_INDEXED_PROTO;
	LOOKUP_DEFINED_INDEXED_PROTO;

	UNROLL_LVALUE_REFERENCES_PROTO;

	/// helper functor for dumping values
	struct key_value_dumper {
		ostream& os;
		key_value_dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const typename collection_type::value_type&);
	};      // end struct key_value_dumper

public:
	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(footprint&, const persistent_object_manager&, istream&);

};	// end class value_array

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_VALUE_ARRAY_H__

