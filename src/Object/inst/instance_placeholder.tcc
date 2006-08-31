/**
	\file "Object/inst/instance_placeholder.tcc"
	$Id: instance_placeholder.tcc,v 1.1.2.2 2006/08/31 07:28:40 fang Exp $
	TODO: trim includes
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_TCC__

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

#include "Object/inst/instance_placeholder.h"
#include "Object/inst/instance_collection.h"
#if 0
#include "Object/inst/alias_actuals.tcc"
#include "Object/inst/subinstance_manager.tcc"
#include "Object/inst/instance_pool.tcc"
#include "Object/inst/internal_aliases_policy.h"
#include "Object/inst/port_alias_tracker.h"
#endif
#include "Object/expr/expr_dump_context.h"
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
#if 0
#include "Object/def/footprint.h"
#include "Object/global_entry.h"
#include "Object/port_context.h"
#endif
#include "Object/unroll/instantiation_statement.h"
#include "common/ICE.h"

#if 0
#include "util/multikey_set.tcc"
#include "util/ring_node.tcc"
#include "util/packed_array.tcc"
#endif
#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"

#include "util/persistent_object_manager.tcc"
#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/static_trace.h"
#include "util/compose.h"
#include "util/binders.h"
#include "util/dereference.h"

#if ENABLE_STACKTRACE
#include <iterator>
#endif

//=============================================================================
// module-local specializations

#if 0
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
#endif

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
using util::bind2nd_argval;
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

#if 0
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
#endif

//=============================================================================
// class instance_placeholder method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
INSTANCE_PLACEHOLDER_CLASS::instance_placeholder(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d), 
#if 0
		collection_type_manager_parent_type(), 
#endif
		initial_instantiation_statement_ptr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// trying to do away with this copy-ctor
#if 0
/**
	Note: we don't bother copying the initial_instantiation_statement_ptr.
	Or should we? we need it to get_type_ref(), sometimes for diagnostics
		after deep-copying collection to footprint.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
INSTANCE_PLACEHOLDER_CLASS::instance_placeholder(const this_type& t, 
		const footprint& f) :
		parent_type(t, f),
		collection_type_manager_parent_type(t), 
		initial_instantiation_statement_ptr(
			t.initial_instantiation_statement_ptr) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
INSTANCE_PLACEHOLDER_CLASS::~instance_placeholder() {
	STACKTRACE_DTOR("~instance_placeholder<>()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This needs to be specialized with a functor...
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
INSTANCE_PLACEHOLDER_CLASS::type_dump(ostream& o) const {
#if 0
	typename collection_type_manager_parent_type::dumper dump_it(o);
	return dump_it(*this);
#else
	o << "(TODO: infer type from inst-stmt)";
	return o;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: for port collections with relaxed types, might want to expand
	relaxed actuals at some point for diagnostics.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
INSTANCE_PLACEHOLDER_CLASS::dump_formal(ostream& o) const {
//	this->dump_base(o);
#if 0
	this->dump_collection_only(o);
#endif
	expr_dump_context dc(expr_dump_context::default_value);
	dc.enclosing_scope = this->owner;
	if (this->dimensions) {
		const index_collection_item_ptr_type
			i(this->get_initial_instantiation_indices());
		NEVER_NULL(i);
		i->dump(o, dc);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the type-reference given by the first instantiation
	statement (which may be predicated).  
	This is not guaranteed to be the *final* type of the collection.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
INSTANCE_PLACEHOLDER_CLASS::get_type_ref(void) const {
	NEVER_NULL(this->initial_instantiation_statement_ptr);
	return this->initial_instantiation_statement_ptr->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off from instance_placeholder_base::formal_size_equivalent.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
index_collection_item_ptr_type
INSTANCE_PLACEHOLDER_CLASS::get_initial_instantiation_indices(void) const {
	NEVER_NULL(this->initial_instantiation_statement_ptr);
	return this->initial_instantiation_statement_ptr->get_indices();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// what to do with these?
#if 0
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
typename INSTANCE_PLACEHOLDER_CLASS::type_ref_ptr_type
INSTANCE_PLACEHOLDER_CLASS::get_type_ref_subtype(void) const {
	return collection_type_manager_parent_type::get_type(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must match exact type, i.e. be connectibly type equivalent.  
	Includes relaxed parameters, if applicable.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
bool
INSTANCE_PLACEHOLDER_CLASS::must_be_collectibly_type_equivalent(
		const this_type& c) const {
	return collection_type_manager_parent_type::
		must_be_collectibly_type_equivalent(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the type of the collection during the first instantiation
	of any of its members.  
	Called from instantiation_statement.
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
good_bool
INSTANCE_PLACEHOLDER_CLASS::establish_collection_type(
		const instance_placeholder_parameter_type& t) {
	return collection_type_manager_parent_type::commit_type_first_time(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
bool
INSTANCE_PLACEHOLDER_CLASS::has_relaxed_type(void) const {
	return collection_type_manager_parent_type::is_relaxed_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	During unroll phase, this commits the type of the collection.  
	\param t the data integer type reference, containing width, 
		must already be resolved to a const_param_expr_list.  
	\return false on success, true on error.  
	\post the integer width is fixed for the rest of the program.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_PLACEHOLDER_CLASS::check_established_type(
		const instance_placeholder_parameter_type& t) const {
	// functor, specialized for each class
	return collection_type_manager_parent_type::check_type(t);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just creates a simple instance_reference to this collection.  
	If indexed, the index is set by the caller.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<meta_instance_reference_base>
INSTANCE_PLACEHOLDER_CLASS::make_meta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<meta_instance_reference_base>	ptr_return_type;
	return ptr_return_type(new simple_meta_instance_reference_type(
			never_ptr<const this_type>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a nonmeta instance reference.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<nonmeta_instance_reference_base>
INSTANCE_PLACEHOLDER_CLASS::make_nonmeta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<nonmeta_instance_reference_base>	ptr_return_type;
	return ptr_return_type(new simple_nonmeta_instance_reference_type(
			never_ptr<const this_type>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does exactly what it says.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
typename INSTANCE_PLACEHOLDER_CLASS::member_inst_ref_ptr_type
INSTANCE_PLACEHOLDER_CLASS::make_member_meta_instance_reference(
		const inst_ref_ptr_type& b) const {
	NEVER_NULL(b);
	return member_inst_ref_ptr_type(
		new member_simple_meta_instance_reference_type(
			b, never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// inappropriate for placeholders
#if 0
/**
	Need to return a legitmate reference to a parameter list!
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
never_ptr<const const_param_expr_list>
INSTANCE_PLACEHOLDER_CLASS::get_actual_param_list(void) const {
	STACKTRACE("instance_placeholder::get_actual_param_list()");
	return never_ptr<const const_param_expr_list>(NULL);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// re-write
/**
	\return newly constructed d-dimensional array.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
typename INSTANCE_PLACEHOLDER_CLASS::instance_collection_generic_type*
INSTANCE_PLACEHOLDER_CLASS::make_array(
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// re-write, need persistent type-key
/**
	initial_instantiation_statement_ptr is permitted to be NULL
	for instance collections that belong to footprints.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
INSTANCE_PLACEHOLDER_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_placeholder<Tag>::collect_base()");
	parent_type::collect_transient_info_base(m);
#if 0
	collection_type_manager_parent_type::collect_transient_info_base(m);
#endif
	if (this->initial_instantiation_statement_ptr) {
		initial_instantiation_statement_ptr->collect_transient_info(m);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
INSTANCE_PLACEHOLDER_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("instance_placeholder<Tag>::write_base()");
	parent_type::write_object_base(m, o);
#if 0
	collection_type_manager_parent_type::write_object_base(m, o);
#endif
	m.write_pointer(o, this->initial_instantiation_statement_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
INSTANCE_PLACEHOLDER_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_placeholder<Tag>::load_base()");
	parent_type::load_object_base(m, i);
#if 0
	collection_type_manager_parent_type::load_object_base(m, i);
#endif
	m.read_pointer(i, this->initial_instantiation_statement_ptr);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_TCC__

