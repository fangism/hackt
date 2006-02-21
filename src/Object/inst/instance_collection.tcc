/**
	\file "Object/inst/instance_collection.tcc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originally came from 
		"Object/art_object_instance_collection.tcc"
		in a previous life.  
	$Id: instance_collection.tcc,v 1.19 2006/02/21 04:48:29 fang Exp $
	TODO: trim includes
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_TCC__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overridable debug switches

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <exception>
#include <iostream>
#include <algorithm>
#include <functional>

// experimental: suppressing automatic template instantiation
#include "Object/common/extern_templates.h"

#include "Object/inst/instance_collection.h"
#include "Object/inst/alias_actuals.tcc"
#include "Object/inst/subinstance_manager.tcc"
#include "Object/inst/instance_pool.tcc"
#include "Object/inst/internal_aliases_policy.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_param_expr_list.h"		// for debug only
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/def/definition_base.h"
#include "Object/type/canonical_type.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/def/footprint.h"
#include "Object/global_entry.h"
#include "Object/port_context.h"
#include "Object/unroll/instantiation_statement.h"
#include "common/ICE.h"

#include "util/multikey_set.tcc"
#include "util/ring_node.tcc"
#include "util/packed_array.tcc"
#include "util/memory/count_ptr.tcc"

#include "util/persistent_object_manager.tcc"
#include "util/indent.h"
#include "util/what.h"
#include "util/wtf.h"
#include "util/stacktrace.h"
#include "util/static_trace.h"
#include "util/ptrs_functional.h"
#include "util/compose.h"
#include "util/binders.h"
#include "util/dereference.h"

#if ENABLE_STACKTRACE
#include <iterator>
#endif

//=============================================================================
// module-local specializations

namespace std {
using HAC::entity::instance_alias;

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
struct _Select1st<INSTANCE_ALIAS_CLASS> :
	public _Select1st<typename INSTANCE_ALIAS_CLASS::parent_type> {
};      // end struct _Select1st

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
struct _Select2nd<INSTANCE_ALIAS_CLASS> :
	public _Select2nd<typename INSTANCE_ALIAS_CLASS::parent_type> {
};	// end struct _Select2nd
}	// end namespace std

//=============================================================================

namespace HAC {
namespace entity {
using std::string;
using std::_Select1st;
#include "util/using_ostream.h"
using util::multikey_generator;
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
using std::bind2nd_argval;
using util::multikey;
using util::value_writer;
using util::value_reader;
using util::write_value;
using util::read_value;
using util::indent;
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// class instance_array member class definitions

/**
	Functor to collect transient info in the aliases.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class INSTANCE_ARRAY_CLASS::element_collector {
	persistent_object_manager& pom;
public:
	element_collector(persistent_object_manager& m) : pom(m) { }

	void
	operator () (const element_type& ) const;
};      // end struct element_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to write alias elements.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class INSTANCE_ARRAY_CLASS::element_writer {
	ostream& os;
	const persistent_object_manager& pom;
public:
	element_writer(const persistent_object_manager& m, ostream& o)
		: os(o), pom(m) { }

	void
	operator () (const element_type& ) const;
};      // end struct element_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to load alias elements.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class INSTANCE_ARRAY_CLASS::element_loader {
	istream& is;
	const persistent_object_manager& pom;
	collection_type& coll;
public:
	element_loader(const persistent_object_manager& m,
		istream& i, collection_type& c) :
		is(i), pom(m), coll(c) { }

	void
	operator () (void);
};      // end class element_loader

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to write alias connections, continuation pointers.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class INSTANCE_ARRAY_CLASS::connection_writer {
	ostream& os;
	const persistent_object_manager& pom;
public:
	connection_writer(const persistent_object_manager& m,
		ostream& o) : os(o), pom(m) { }

	void
	operator () (const element_type& ) const;
};      // end struct connection_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to load alias connections, continuation pointers.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class INSTANCE_ARRAY_CLASS::connection_loader {
	istream& is;
	const persistent_object_manager& pom;
public:
	connection_loader(const persistent_object_manager& m,
		istream& i) : is(i), pom(m) { }

	void
	operator () (const element_type& );
};      // end class connection_loader

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to dump keys and alias information.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
struct INSTANCE_ARRAY_CLASS::key_dumper {
	ostream& os;
	const dump_flags& df;

	key_dumper(ostream& o, const dump_flags& _df) : os(o), df(_df) { }

	ostream&
	operator () (const value_type&);
};      // end struct key_dumper

//=============================================================================
// class instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::instance_collection(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d), collection_type_manager_parent_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::instance_collection(const this_type& t, 
		const footprint& f) :
		parent_type(t, f),
		collection_type_manager_parent_type(t) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::~instance_collection() {
	STACKTRACE_DTOR("~instance_collection<>()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This needs to be specialized with a functor...
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
ostream&
INSTANCE_COLLECTION_CLASS::type_dump(ostream& o) const {
	typename collection_type_manager_parent_type::dumper dump_it(o);
	return dump_it(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
INSTANCE_COLLECTION_CLASS::get_type_ref(void) const {
	return initial_instantiation_statement_ptr->get_type_ref();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off from instance_collection_base::formal_size_equivalent.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
index_collection_item_ptr_type
INSTANCE_COLLECTION_CLASS::get_initial_instantiation_indices(void) const {
	NEVER_NULL(initial_instantiation_statement_ptr);
	return initial_instantiation_statement_ptr->get_indices();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
typename INSTANCE_COLLECTION_CLASS::type_ref_ptr_type
INSTANCE_COLLECTION_CLASS::get_type_ref_subtype(void) const {
	return collection_type_manager_parent_type::get_type(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
bool
INSTANCE_COLLECTION_CLASS::must_match_type(const this_type& c) const {
	return collection_type_manager_parent_type::must_match_type(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::establish_collection_type(
		const instance_collection_parameter_type& t) {
	NEVER_NULL(t);
	collection_type_manager_parent_type::commit_type_first_time(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
bool
INSTANCE_COLLECTION_CLASS::has_relaxed_type(void) const {
	return collection_type_manager_parent_type::is_relaxed_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	During unroll phase, this commits the type of the collection.  
	\param t the data integer type reference, containing width, 
		must already be resolved to a const_param_expr_list.  
	\return false on success, true on error.  
	\post the integer width is fixed for the rest of the program.  
	TODO: rename this!, doesn't commit anymore, just checks (const)
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_COLLECTION_CLASS::check_established_type(
		const instance_collection_parameter_type& t) const {
	// functor, specialized for each class
	return collection_type_manager_parent_type::check_type(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: update this description, nothing to do with context
	Create a meta instance reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<meta_instance_reference_base>
INSTANCE_COLLECTION_CLASS::make_meta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<meta_instance_reference_base>	return_type;
	return return_type(new simple_meta_instance_reference_type(
			never_ptr<const this_type>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a nonmeta instance reference.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<nonmeta_instance_reference_base>
INSTANCE_COLLECTION_CLASS::make_nonmeta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<nonmeta_instance_reference_base>	return_type;
	return return_type(new simple_nonmeta_instance_reference_type(
			never_ptr<const this_type>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
typename INSTANCE_COLLECTION_CLASS::member_inst_ref_ptr_type
INSTANCE_COLLECTION_CLASS::make_member_meta_instance_reference(
		const inst_ref_ptr_type& b) const {
	NEVER_NULL(b);
	return member_inst_ref_ptr_type(
		new member_simple_meta_instance_reference_type(
			b, never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to return a legitmate reference to a parameter list!
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
never_ptr<const const_param_expr_list>
INSTANCE_COLLECTION_CLASS::get_actual_param_list(void) const {
	STACKTRACE("instance_collection::get_actual_param_list()");
	return never_ptr<const const_param_expr_list>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS*
INSTANCE_COLLECTION_CLASS::make_array(
		const scopespace& o, const string& n, const size_t d) {
	switch(d) {
		case 0: return new instance_array<Tag,0>(o, n);
		case 1: return new instance_array<Tag,1>(o, n);
		case 2: return new instance_array<Tag,2>(o, n);
		case 3: return new instance_array<Tag,3>(o, n);
		case 4: return new instance_array<Tag,4>(o, n);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	initial_instantiation_statement_ptr is permitted to be NULL
	for instance collections that belong to footprints.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_collection<Tag>::collect_base()");
	parent_type::collect_transient_info_base(m);
	collection_type_manager_parent_type::collect_transient_info_base(m);
	if (this->initial_instantiation_statement_ptr) {
		initial_instantiation_statement_ptr->collect_transient_info(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("instance_collection<Tag>::write_base()");
	parent_type::write_object_base(m, o);
	// specialization functor parameter writer
	collection_type_manager_parent_type::write_object_base(m, o);
	m.write_pointer(o, this->initial_instantiation_statement_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_collection<Tag>::load_base()");
	parent_type::load_object_base(m, i);
	// specialization functor parameter loader
	collection_type_manager_parent_type::load_object_base(m, i);
	m.read_pointer(i, this->initial_instantiation_statement_ptr);
}

//=============================================================================
// class array method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array() : parent_type(D), collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial deep-copy constructor for footprint management.  
	NOTE: this collection should be empty, but we copy it anyhow.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array(const this_type& t, const footprint& f) :
		parent_type(t, f), collection(t.collection) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::~instance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial deep copy constructor for the footprint.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
instance_collection_base*
INSTANCE_ARRAY_CLASS::make_instance_collection_footprint_copy(
		const footprint& f) const {
	return new this_type(*this, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::dump_unrolled_instances(ostream& o,
		const dump_flags& df) const {
#if 0
	o << "[dump flags: " << (df.show_definition_owner ? "(def) " : " ") <<
		(df.show_namespace_owner ? "(ns) " : " ") <<
		(df.show_leading_scope ? "(::)]" : "]");
#endif
	for_each(this->collection.begin(), this->collection.end(),
		key_dumper(o, df));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE 
ostream&
INSTANCE_ARRAY_CLASS::key_dumper::operator () (const value_type& p) {
	os << auto_indent << _Select1st<value_type>()(p);
	NEVER_NULL(p.container);
	if (p.container->has_relaxed_type())
		p.dump_actuals(os);
	os << " = ";
	NEVER_NULL(p.get_next());
	p.get_next()->dump_hierarchical_name(os, df);
	if (p.instance_index)
		os << " (" << p.instance_index << ')';
	p.dump_ports(os << ' ', df);
	return os << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates integer parameters at the specified indices.
	\param actuals the (optional) relaxed template arguments, 
		which is only applicable to processes.  
	\param i fully-specified range of indices to instantiate.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::instantiate_indices(const const_range_list& ranges, 
		const instance_relaxed_actuals_type& actuals, 
		const unroll_context& c) {
	STACKTRACE("instance_array<Tag,D>::instantiate_indices()");
	if (!ranges.is_valid()) {
		ranges.dump(cerr << "ERROR: invalid instantiation range list: ",
			expr_dump_context::default_value) << endl;
		return good_bool(false);
	}
	// now iterate through, unrolling one at a time...
	// stop as soon as there is a conflict
	// later: factor this out into common helper class
	multikey_generator<D, pint_value_type> key_gen;
#if ENABLE_STACKTRACE
	ranges.dump(STACKTRACE_INDENT,
		expr_dump_context::default_value) << endl;
#endif
	ranges.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool err = false;
	do {
		const const_iterator iter(this->collection.find(key_gen));
		if (iter == collection.end()) {
			// then we can insert a new one
			// create with back-ref!
			const iterator
				new_elem(this->collection.insert(
					element_type(key_gen)));
			// recursive instantiation happens on construction
			// alternative to constructing and copying:
			// construct empty, then initialize the new reference.
			// establish back-link here.  
			// This is actually necessary for correctness as well.
			const_cast<instance_alias_base_type&>(
				static_cast<const instance_alias_base_type&>(
				*new_elem)).instantiate(
					never_ptr<const this_type>(this), c);
			// set its relaxed actuals!!! (if appropriate)
			if (actuals) {
			const bool attached(new_elem->attach_actuals(actuals));
			if (!attached) {
				cerr << "ERROR: attaching relaxed actuals to "
					<< this->get_qualified_name() <<
					key_gen << endl;
				err = true;
			}
			}
		} else {
			// found one that already exists!
			// more detailed message, please!
			cerr << "ERROR: Index " << key_gen << " of ";
			what(cerr) << ' ' << this->get_qualified_name() <<
				" already instantiated!" << endl;
			err = true;
		}
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return good_bool(!err);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates uniquely allocated space for aliases instances.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::create_unique_state(const const_range_list& ranges, 
		footprint& f) {
	STACKTRACE_VERBOSE;
	multikey_generator<D, pint_value_type> key_gen;
	ranges.make_multikey_generator(key_gen);
	key_gen.initialize();
	do {
		// should be iterator, not const_iterator
		const const_iterator iter(this->collection.find(key_gen));
		INVARIANT(iter != collection.end());
		if (!iter->allocate_state(f))
			return good_bool(false);
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Like create_unique_state except it operates on the entire collection.
	Called from subinstance_manager::create_state.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::allocate_state(footprint& f) {
	STACKTRACE_VERBOSE;
	iterator i(collection.begin());
	const iterator e(collection.end());
	for ( ; i!=e; i++) {
		if (!i->__allocate_state(f))
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Expands indices which may be under-specified into explicit
        indices for the implicit subslice, if it is densely packed.
        Depends on the current state of the collection.
        \param l is list of indices, which may be under-specified,
                or even empty.
        \return fully-specified index list, or empty list if there is error.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
const_index_list
INSTANCE_ARRAY_CLASS::resolve_indices(const const_index_list& l) const {
	STACKTRACE_VERBOSE;
	const size_t l_size = l.size();
	if (D == l_size) {
		// already fully specified
		return l;
	}
	// convert indices to pair of list of multikeys
	if (!l_size) {
		return const_index_list(l, this->collection.is_compact());
	}
	// else construct slice
	list<pint_value_type> lower_list, upper_list;
	transform(l.begin(), l.end(), back_inserter(lower_list),
		unary_compose(
			mem_fun_ref(&const_index::lower_bound),
			dereference<count_ptr<const const_index> >()
		)
	);
	transform(l.begin(), l.end(), back_inserter(upper_list),
		unary_compose(
			mem_fun_ref(&const_index::upper_bound),
			dereference<count_ptr<const const_index> >()
		)
	);
	return const_index_list(l,
		collection.is_compact_slice(lower_list, upper_list));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Associative lookup using native key type.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
never_ptr<typename INSTANCE_ARRAY_CLASS::element_type>
INSTANCE_ARRAY_CLASS::operator [] (const key_type& index) const {
	typedef	never_ptr<element_type>		return_type;
	const const_iterator it(this->collection.find(index));
	if (it == this->collection.end()) {
		this->type_dump(
			cerr << "ERROR: reference to uninstantiated ") <<
			" " << this->get_qualified_name() << " at index: " <<
			index << endl;
		return return_type(NULL);
	}
	const element_type& b(*it);
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return return_type(const_cast<element_type*>(&b));
	} else {
		// remove the blank we added?
		// not necessary, but could keep the collection "clean"
		this->type_dump(
			cerr << "ERROR: reference to uninstantiated ") <<
			" " << this->get_qualified_name() << " at index: " <<
			index << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
typename INSTANCE_ARRAY_CLASS::instance_alias_base_ptr_type
INSTANCE_ARRAY_CLASS::lookup_instance(const multikey_index_type& i) const {
	typedef	instance_alias_base_ptr_type	return_type;
	INVARIANT(D == i.dimensions());
	const key_type index(i);
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "i = " << i << endl;
	STACKTRACE_INDENT << "index = " << index << endl;
#endif
	return (*this)[index];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l list in which to accumulate instance references.
	\param r the ranges, must be valid, and fully resolved.
	\return false on error, e.g. if value doesn't exist or
		is uninitialized; true on success.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::lookup_instance_collection(
		typename default_list<instance_alias_base_ptr_type>::type& l,
		const const_range_list& r) const {
	INVARIANT(!r.empty());
	key_generator_type key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const const_iterator it(this->collection.find(key_gen));
		if (it == collection.end()) {
			this->type_dump(
				cerr << "FATAL: reference to uninstantiated ")
					<< " index " << key_gen << endl;
			l.push_back(instance_alias_base_ptr_type(NULL));
			ret = false;
		} else {
		const element_type& pi(*it);
		// pi MUST be valid if it belongs to an array
		if (pi.valid()) {
			l.push_back(instance_alias_base_ptr_type(
				const_cast<element_type*>(&pi)));
		} else {
			this->type_dump(
				cerr << "FATAL: reference to uninstantiated ")
					<< " index " << key_gen << endl;
			l.push_back(instance_alias_base_ptr_type(NULL));
			ret = false;
		}
		}
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Packs resolved range of aliases into a collection.  
	\pre array a must already be sized properly, do not resize here.  
	\return true on error, else false.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_ARRAY_CLASS::unroll_aliases(const multikey_index_type& l, 
		const multikey_index_type& u, alias_collection_type& a) const {
	typedef	typename alias_collection_type::key_type
						collection_key_type;
	typedef	typename alias_collection_type::iterator
						alias_collection_iterator;
	STACKTRACE_VERBOSE;
	const key_type lower(l);	// this will assert dimension match!
	const key_type upper(u);	// this will assert dimension match!
	key_generator_type key_gen(lower, upper);
	key_gen.initialize();
	bool ret = false;
	alias_collection_iterator a_iter(a.begin());
	const const_iterator collection_end(this->collection.end());
	// maybe INVARIANT(sizes == iterations)
	do {
		// really is a monotonic incremental search, 
		// don't need log(N) lookup each time, fix later...
		const const_iterator it(this->collection.find(key_gen));
		if (it == collection_end) {
			this->type_dump(
			cerr << "FATAL: reference to uninstantiated ") <<
				" index " << key_gen << endl;
			*a_iter = never_ptr<element_type>(NULL);
			ret = true;
		} else {
			const element_type& pi(*it);
			*a_iter = never_ptr<element_type>(
				const_cast<element_type*>(&pi));
		}
		a_iter++;
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	INVARIANT(a_iter == a.end());
	return bad_bool(ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands this collection into a copy for a port formal.  
	\return owned pointer to new created collection.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
count_ptr<physical_instance_collection>
INSTANCE_ARRAY_CLASS::unroll_port_only(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const count_ptr<this_type> ret(new this_type(*this));
	NEVER_NULL(ret);
	// Is this really copy-constructible?
	INVARIANT(this->initial_instantiation_statement_ptr);
	if (this->initial_instantiation_statement_ptr->
			instantiate_port(c, *ret).good) {
		return ret;
	} else 	return count_ptr<physical_instance_collection>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Merges two instance collections, assigning them the same allocated
	state.  
	Called by subinstance_manager::create_state(), 
		during create-unique phase.  
	this and t must be port subinstances, 
		and hence, must be densely packed.  
	Since collection p was type/size checked during connection, 
	we can conclude that they have the same size.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::merge_created_state(physical_instance_collection& p, 
		footprint& f) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	INVARIANT(this->collection.size() == t.collection.size());
	iterator i(this->collection.begin());
	iterator j(t.collection.begin());
	const iterator e(this->collection.end());
	for ( ; i!=e; i++, j++) {
		// unfortunately, set iterators only return const refs
		// we only intend to modify the value without modifying the key
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		element_type& jj(const_cast<element_type&>(
			AS_A(const element_type&, *j)));
		if (!ii.merge_allocate_state(jj, f).good)
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::inherit_created_state(
		const physical_instance_collection& p, const footprint& f) {
	STACKTRACE_VERBOSE;
	const this_type& t(IS_A(const this_type&, p));	// assert dynamic_cast
	INVARIANT(this->collection.size() == t.collection.size());
	iterator i(this->collection.begin());
	const_iterator j(t.collection.begin());
	const iterator e(this->collection.end());
	for ( ; i!=e; i++, j++) {
		// unfortunately, set iterators only return const refs
		// we only intend to modify the value without modifying the key
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		ii.inherit_subinstances_state(*j, f);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively synchronize relaxed actuals of ports.  
	Even though this type may not have relaxed actuals, 
	the ports (and ports-of-ports) might, so recursion is necessary.  
	This is called at create-time. 
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::synchronize_actuals(physical_instance_collection& p) {
	STACKTRACE_VERBOSE;
	const this_type& t(IS_A(const this_type&, p));	// assert dynamic_cast
	INVARIANT(this->collection.size() == t.collection.size());
	iterator i(this->collection.begin());
	iterator j(t.collection.begin());
	const iterator e(this->collection.end());
	for ( ; i!=e; i++, j++) {
		// unfortunately, set iterators only return const refs
		// we only intend to modify the value without modifying the key
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		element_type& jj(const_cast<element_type&>(
			AS_A(const element_type&, *j)));
		if (!element_type::synchronize_actuals_recursive(ii, jj).good) {
			// error message?
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Walks the entire collection and create definition footprints of
	constitutent types.  
	TODO: optimize with specialization for non-recursive types.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::create_dependent_types(void) const {
	STACKTRACE_VERBOSE;
	iterator i(this->collection.begin());
	const iterator e(this->collection.end());
if (this->has_relaxed_type()) {
	for ( ; i!=e; i++) {
		if (!element_type::create_dependent_types(*i).good)
			return good_bool(false);
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		if (!internal_alias_policy::connect(ii).good) {
			return good_bool(false);
		}
	}
} else {
	// type of container is already strict, 
	// evaluate it once and re-use it when replaying internal aliases
	const typename parent_type::instance_collection_parameter_type
		t(collection_type_manager_parent_type::get_canonical_type());
	if (!create_definition_footprint(t).good)
		return good_bool(false);
	for ( ; i!=e; i++) {
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		// shouldn't we pass in the canonical type 't'?
		if (!internal_alias_policy::connect(ii, t).good) {
			return good_bool(false);
		}
	}
}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre already called create_dependent_types.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::collect_port_aliases(port_alias_tracker& t) const {
	STACKTRACE_VERBOSE;
	const_iterator i(this->collection.begin());
	const const_iterator e(this->collection.end());
	for ( ; i!=e; i++) {
		const element_type& ii(*i);
		INVARIANT(ii.instance_index);
		// 0 is not an acceptable index
		t.template get_id_map<Tag>()[ii.instance_index]
			.push_back(never_ptr<const element_type>(&ii));
		ii.collect_port_aliases(t);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
/**
	Temporary hack.  :(
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::hack_remap_indices(footprint& f) {
	STACKTRACE_VERBOSE;
	iterator i(this->collection.begin());
	const iterator e(this->collection.end());
	for ( ; i!=e; i++) {
		element_type& ii(const_cast<element_type&>(*i));
		ii.hack_remap_indices(f);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads a key from binary stream then returns a reference to the 
	indexed instance alias.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
typename INSTANCE_ARRAY_CLASS::instance_alias_base_type&
INSTANCE_ARRAY_CLASS::load_reference(istream& i) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::load_reference()");
	key_type k;
	value_reader<key_type> read_key(i);
	read_key(k);
	const iterator it(collection.find(k));
	INVARIANT(it != this->collection.end());
	// need const cast because set only returns const references/iterators
	return const_cast<element_type&>(*it);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Going to need some sort of element_reader counterpart.
	\param e is a reference to a INSTANCE_ALIAS_CLASS.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::element_collector::operator () (
		const element_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::element_collector::operator()");
	e.collect_transient_info_base(pom);
	// postpone connection writing until next phase
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Going to need some sort of element_reader counterpart.
	\param e is a reference to a INSTANCE_ALIAS_CLASS.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::element_writer::operator () (const element_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::element_writer::operator()");
	value_writer<key_type> write_key(os);
	write_key(e.key);
	e.write_object_base(pom, os);
	// postpone connection writing until next phase
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This must perfectly complement element_writer::operator().
	construct the element locally first, then insert it into set.
	BUG FIX NOTE: 2005-07-16:
		Loading the object *after* inserting it into the collection
		is not only an enhancement (eliminating copy-overhead), 
		but absolutely necessary for correctness, because effective 
		loading of the object base requires that the parent object 
		be stable, i.e.  not have short lifetime.  
		Operating on the newly inserted reference guarantees 
		proper lifetime.  This also applies to 
		instance_collection<>::instantiate_indices.
		This is not an issue with scalar instances, because
		they have the same lifetime as their parent collection, 
		by construction.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::element_loader::operator () (void) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::element_loader::operator()");
	typedef	typename collection_type::iterator	local_iterator;
	key_type temp_key;
	value_reader<key_type> read_key(this->is);
	read_key(temp_key);
	const element_type temp_elem(temp_key);
	const local_iterator i(this->coll.insert(temp_elem));
	const_cast<element_type&>(static_cast<const element_type&>(*i))
		.load_object_base(this->pom, this->is);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::connection_writer::operator() (const element_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::connection_writer::operator()");
	const instance_alias_base_type* const next(e.get_next());
	NEVER_NULL(next);
	if (next != &e) {
		write_value<char>(os, 1);
		next->write_next_connection(pom, os);
	} else {
		write_value<char>(os, 0);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must complement connection_writer::operator().
	const_cast is an unfortunate consequence of set only
	returning const references and const iterators, where we intend
	the non-key part of the object to me mutable.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::connection_loader::operator() (const element_type& e) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::connection_loader::operator()");
	char c;
	read_value(this->is, c);
	if (c) {
		element_type& elem(const_cast<element_type&>(e));
		// lookup the instance in the collection referenced
		// and connect them
		elem.load_next_connection(this->pom, this->is);
	}
	// else just leave it pointing to itself, 
	// which was how it was constructed
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates port formal placeholders to actual global IDs.  
	\pre footprint_frame has already been constructed.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::construct_port_context(port_collection_context& pcc, 
		const footprint_frame& ff) const {
	STACKTRACE_VERBOSE;
	const_iterator i(this->collection.begin());
	const const_iterator e(this->collection.end());
	pcc.resize(this->collection.size());
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		i->construct_port_context(pcc, ff, j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Assigns...
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::assign_footprint_frame(footprint_frame& ff, 
		const port_collection_context& pcc) const {
	STACKTRACE_VERBOSE;
	INVARIANT(this->collection.size() == pcc.size());
	const_iterator i(this->collection.begin());
	const const_iterator e(this->collection.end());
	size_t j = 0;
	for ( ; i!=e; i++, j++) {
		i->assign_footprint_frame(ff, pcc, j);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints hierarchical aliases for cflat. 
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::cflat_aliases(const cflat_aliases_arg_type& c) const {
	for_each(this->collection.begin(), this->collection.end(),
		bind2nd_argval(mem_fun_ref(&element_type::cflat_aliases), c)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key, D)) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::collect_transients()");
	parent_type::collect_transient_info_base(m);
	for_each(this->collection.begin(), this->collection.end(), 
		element_collector(m)	// added 2005-07-07
	);
	// optimization for later: factor this out into a policy
	// so that collections without pointers to collect
	// may be skipped.
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::write_object()");
	parent_type::write_object_base(m, f);
	// need to know how many members to expect
	write_value(f, this->collection.size());
	for_each(this->collection.begin(), this->collection.end(),
		element_writer(m, f)
	);
	for_each(this->collection.begin(), this->collection.end(), 
		connection_writer(m, f)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::load_object()");
	parent_type::load_object_base(m, f);
	// procedure:
	// 1) load all instantiated indices *without* their connections
	//      let them start out pointing to themselves.  
	// 2) each element contains information to reconstruct, 
	//      we need temporary local storage for it.
	size_t collection_size;
	read_value(f, collection_size);
	size_t i = 0;
	element_loader load_element(m, f, this->collection);
	for ( ; i < collection_size; i++) {
		// this must perfectly complement element_writer::operator()
		// construct the element locally first, then insert it into set
		load_element();
	}
	for_each(collection.begin(), collection.end(),
		connection_loader(m, f)
	);
}

//=============================================================================
// class array method definitions (specialized)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array() : parent_type(0), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array(const scopespace& o, const string& n) :
		parent_type(o, n, 0), the_instance() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial deep-copy constructor for footprint management.  
	NOTE: this collection should be empty, but we copy it anyhow.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array(const this_type& t, const footprint& f) :
		parent_type(t, f), the_instance(t.the_instance) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::~instance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Deep (partial) copy-constructor for footprint.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
instance_collection_base*
INSTANCE_SCALAR_CLASS::make_instance_collection_footprint_copy(
		const footprint& f) const {
	return new this_type(*this, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bool
INSTANCE_SCALAR_CLASS::is_partially_unrolled(void) const {
	return this->the_instance.valid();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::dump_unrolled_instances(ostream& o,
		const dump_flags& df) const {
	// no auto-indent, continued on same line
	// see physical_instance_collection::dump for reason why
	if (this->the_instance.container->has_relaxed_type()) {
		this->the_instance.dump_actuals(o);
	}
#if 0
	o << "[dump flags: " << (df.show_definition_owner ? "(def) " : " ") <<
		(df.show_namespace_owner ? "(ns) " : " ") <<
		(df.show_leading_scope ? "(::)]" : "]");
#endif
	this->the_instance.get_next()->dump_hierarchical_name(o << " = ", df);
	if (this->the_instance.instance_index)
		o << " (" << this->the_instance.instance_index << ')';
	this->the_instance.dump_ports(o << ' ', df);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
/**
	Instantiates the_instance of integer datatype.
	Ideally, the error should never trigger because
	re-instantiation / redeclaration of a scalar instance
	is easily detected (and actually detected) during the compile phase.
	\param i indices must be NULL because this is not an array.
 */
good_bool
INSTANCE_SCALAR_CLASS::instantiate_indices(
		const const_range_list& r, 
		const instance_relaxed_actuals_type& actuals, 
		const unroll_context& c) {
	STACKTRACE("instance_array<Tag,0>::instantiate_indices()");
	INVARIANT(r.empty());
	if (this->the_instance.valid()) {
		// should never happen, but just in case...
		this->type_dump(cerr << "ERROR: Scalar ") <<
			" already instantiated!" << endl;
		return good_bool(false);
	}
	// here we need an explicit instantiation (recursive)
	this->the_instance.instantiate(never_ptr<const this_type>(this), c);
	const bool attached(actuals ?
		this->the_instance.attach_actuals(actuals) : true);
	if (!attached) {
		cerr << "ERROR: attaching relaxed actuals to scalar ";
		this->type_dump(cerr) << " " << this->get_qualified_name()
			<< endl;
	}
	return good_bool(attached);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates state for a single instance alias.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::create_unique_state(const const_range_list& ranges, 
		footprint& f) {
	STACKTRACE("instance_array<Tag,0>::create_unique_state()");
	INVARIANT(ranges.empty());
	INVARIANT(this->the_instance.valid());
	return good_bool(this->the_instance.allocate_state(f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as create_unique_state.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::allocate_state(footprint& f) {
	STACKTRACE_VERBOSE;
	INVARIANT(this->the_instance.valid());
	return good_bool(this->the_instance.__allocate_state(f) != 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This specialization isn't ever supposed to be called.
	\param l is list of indices, which may be under-specified,
		or even empty.
	\return empty index list, always.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
const_index_list
INSTANCE_SCALAR_CLASS::resolve_indices(const const_index_list& l) const {
	cerr << "WARNING: instance_array<Tag,0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_alias_base_ptr_type
INSTANCE_SCALAR_CLASS::lookup_instance(const multikey_index_type& i) const {
	typedef	typename INSTANCE_SCALAR_CLASS::instance_alias_base_ptr_type
						return_type;
	if (!this->the_instance.valid()) {
		this->type_dump(cerr << "ERROR: Reference to uninstantiated ")
			<< "!" << endl;
		return return_type(NULL);
	} else	return return_type(
		const_cast<instance_alias_base_type*>(
			&static_cast<const instance_alias_base_type&>(
				this->the_instance)));
	// ok to return non-const reference to the type, 
	// perhaps it should be declared mutable?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
	\return false to signal error.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bool
INSTANCE_SCALAR_CLASS::lookup_instance_collection(
		typename default_list<instance_alias_base_ptr_type>::type& l,
		const const_range_list& r) const {
	cerr << "WARNING: instance_array<Tag,0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error, false on success.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_SCALAR_CLASS::unroll_aliases(const multikey_index_type& l, 
		const multikey_index_type& u, alias_collection_type& a) const {
	STACKTRACE_VERBOSE;
	if (this->the_instance.valid()) {
		*(a.begin()) = instance_alias_base_ptr_type(
			const_cast<instance_alias_base_type*>(
				&static_cast<const instance_alias_base_type&>(
					this->the_instance)));
		return bad_bool(false);
	} else {
		this->type_dump(cerr << "ERROR: Reference to uninstantiated ")
			<< "!" << endl;
		return bad_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands this collection into a copy for a port formal.  
	\return owned pointer to new created collection.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
count_ptr<physical_instance_collection>
INSTANCE_SCALAR_CLASS::unroll_port_only(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const count_ptr<this_type> ret(new this_type(*this));
	NEVER_NULL(ret);
	INVARIANT(this->initial_instantiation_statement_ptr);
	if (this->initial_instantiation_statement_ptr->
			instantiate_port(c, *ret).good) {
		return ret;
	} else 	return count_ptr<physical_instance_collection>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Merges two instances, assigning them the same allocated state.  
	Called by subinstance_manager::create_state(), 
		during create-unique phase.  
	this and t must be port subinstances.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::merge_created_state(physical_instance_collection& p, 
		footprint& f) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	return this->the_instance.merge_allocate_state(t.the_instance, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::inherit_created_state(
		const physical_instance_collection& p, const footprint& f) {
	STACKTRACE_VERBOSE;
	const this_type& t(IS_A(const this_type&, p));	// assert dynamic_cast
	this->the_instance.inherit_subinstances_state(t.the_instance, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called at create-time to check that the implicit connections
	in the port hierarchy are compatible, w.r.t. relaxed actual
	parameters.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::synchronize_actuals(physical_instance_collection& p) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	return instance_type::synchronize_actuals_recursive(this->the_instance,
		t.the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively creates footprints of complete types bottom-up.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::create_dependent_types(void) const {
	STACKTRACE_VERBOSE;
if (this->has_relaxed_type()) {
	if (!instance_type::create_dependent_types(this->the_instance).good) {
		return good_bool(false);
	}
} else {
	const typename parent_type::instance_collection_parameter_type
		t(collection_type_manager_parent_type::get_canonical_type());
	if (!create_definition_footprint(t).good) {
		return good_bool(false);
	}
}
	if (!internal_alias_policy::connect(
			const_cast<instance_type&>(this->the_instance)).good) {
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::collect_port_aliases(port_alias_tracker& t) const {
	INVARIANT(this->the_instance.instance_index);
	// 0 is not an acceptable index
	t.template get_id_map<Tag>()[this->the_instance.instance_index]
		.push_back(never_ptr<const instance_type>(&this->the_instance));
	this->the_instance.collect_port_aliases(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::hack_remap_indices(footprint& f) {
	this->the_instance.hack_remap_indices(f);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_alias_base_type&
INSTANCE_SCALAR_CLASS::load_reference(istream& i) const {
	STACKTRACE_PERSISTENT("instance_scalar::load_reference()");
	// no key to read!
	// const_cast: have to modify next pointers to re-establish connection, 
	// which is semantically allowed because we allow the alias pointers
	// to be mutable.  
	return const_cast<instance_type&>(this->the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates port formal placeholders to actual global IDs.  
	\pre footprint_frame has already been constructed.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::construct_port_context(port_collection_context& pcc, 
		const footprint_frame& ff) const {
	STACKTRACE_VERBOSE;
	pcc.resize(1);
	this->the_instance.construct_port_context(pcc, ff, 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::assign_footprint_frame(footprint_frame& ff,
		const port_collection_context& pcc) const {
	STACKTRACE_VERBOSE;
	INVARIANT(pcc.size() == 1);
	this->the_instance.assign_footprint_frame(ff, pcc, 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::cflat_aliases(const cflat_aliases_arg_type& c) const {
	this->the_instance.cflat_aliases(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key, 0)) {
	STACKTRACE_PERSISTENT("instance_scalar::collect_transients()");
	parent_type::collect_transient_info_base(m);
	this->the_instance.check(this);
	this->the_instance.collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("instance_scalar::write_object()");
	parent_type::write_object_base(m, f);
	this->the_instance.write_object(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("instance_scalar::load_object()");
	parent_type::load_object_base(m, f);
	this->the_instance.load_object(m, f);		// problem?
	this->the_instance.check(this);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_TCC__

