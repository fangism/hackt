/**
	\file "Object/state_manager.cc"
	This module has been obsoleted by the introduction of
		the footprint class in "Object/def/footprint.h".
	$Id: state_manager.cc,v 1.16.8.3 2007/01/12 00:39:52 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <functional>
#include "Object/state_manager.tcc"
#include "Object/global_entry.tcc"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#endif
#include "Object/traits/proc_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/inst/channel_instance_collection.h"
#include "Object/entry_collection.h"
#include "main/cflat_options.h"
#include "util/stacktrace.h"
#include "util/list_vector.tcc"
#include "util/memory/index_pool.tcc"
#include "util/IO_utils.h"

namespace HAC {
namespace entity {
using util::write_value;
using util::read_value;
#include "util/using_ostream.h"
//=============================================================================
// class global_entry_pool method definitions

template <class Tag>
global_entry_pool<Tag>::global_entry_pool() :
		pool_type() {
	this->set_chunk_size(class_traits<Tag>::instance_pool_chunk_size);
	this->allocate();	// reserve the 0-index as NULL
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
global_entry_pool<Tag>::~global_entry_pool() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param topfp the top-level footprint of the module.  
 */
template <class Tag>
ostream&
global_entry_pool<Tag>::dump(ostream& o, const footprint& topfp) const {
if (this->size() > 1) {
	const state_manager& sm(AS_A(const state_manager&, *this));
	global_entry_dumper ged(o, sm, topfp);
	o << "[global " << class_traits<Tag>::tag_name << " entries]" << endl;
	ged.index = 1;
	const_iterator i(++this->begin());
	const const_iterator e(this->end());
	for ( ; i!=e; i++, ++ged.index) {
		i->dump(ged) << endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry_pool<Tag>::collect_transient_info_base(
		persistent_object_manager& m, 
		const footprint& f) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	const_iterator i(++this->begin());
	const const_iterator e(this->end());
	size_t j = 1;
	for ( ; i!=e; ++i, ++j) {
		i->collect_transient_info_base(m, j, f, 
			AS_A(const state_manager&, *this));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry_pool<Tag>::write_object_base(const persistent_object_manager& m, 
		ostream& o, const footprint& f) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	write_value(o, this->size() -1);
	const_iterator i(++this->begin());
	const const_iterator e(this->end());
	size_t j = 1;
	for ( ; i!=e; i++, j++) {
		STACKTRACE_INDENT_PRINT("writing entry " << j << endl);
		i->write_object_base(m, o, j, f,
			AS_A(const state_manager&, *this));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry_pool<Tag>::load_object_base(const persistent_object_manager& m, 
		istream& i, const footprint& f) {
	STACKTRACE_PERSISTENT_VERBOSE;
	size_t s;
	read_value(i, s);
	// consider setting chunk size to s+1 for optimization
	size_t j = 1;
	for ( ; j<=s; j++) {
		STACKTRACE_INDENT_PRINT("loading entry " << j << endl);
		(*this)[this->allocate()].load_object_base(m, i, j, f, 
			AS_A(const state_manager&, *this));
	}
}

//=============================================================================
// class state_manager method definitions

state_manager::state_manager() :
		process_pool_type(), channel_pool_type(), 
#if ENABLE_DATASTRUCTS
		struct_pool_type(), 
#endif
		enum_pool_type(), 
		int_pool_type(), bool_pool_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
state_manager::~state_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param topfp the top-level footprint of the module.  
 */
ostream&
state_manager::dump(ostream& o, const footprint& topfp) const {
	o << "globID\tsuper\t\tlocalID\tcanonical\tfootprint-frame" << endl;
	global_entry_pool<process_tag>::dump(o, topfp);
	global_entry_pool<channel_tag>::dump(o, topfp);
#if ENABLE_DATASTRUCTS
	global_entry_pool<datastruct_tag>::dump(o, topfp);
#endif
	global_entry_pool<enum_tag>::dump(o, topfp);
	global_entry_pool<int_tag>::dump(o, topfp);
	global_entry_pool<bool_tag>::dump(o, topfp);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Prototype cflat -- strictly for backwards compatibility.  
	Connections should come after production rules.  (CAST tools)
	Check of ordering matters for various tools.
	TODO: move cf-conditionals to caller, module.  
 */
good_bool
state_manager::cflat_prs(ostream& o, const footprint& topfp,
		const cflat_options& cf) const {
if (cf.include_prs) {
	if (cf.dsim_prs)	o << "dsim {" << endl;
	// dump prs
	// for each process entry
	size_t pid = 1;		// 0-indexed, but 0th entry is null
	const global_entry_pool<process_tag>& proc_entry_pool(*this);
	const size_t plim = proc_entry_pool.size();
	for ( ; pid < plim; pid++) {
		production_rule_substructure::cflat_prs(o, 
			proc_entry_pool[pid], topfp, cf, *this);
	}
	if (cf.dsim_prs)	o << "}" << endl;
}
	return good_bool(true);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A cflat visitor only visits processes -- nothing else can accept them.
	Should we generalize this to include nodes as well?
	We need another visitor hierarchy for named instances 
	(see cflat_aliases). 
 */
void
state_manager::accept(PRS::cflat_visitor& v) const {
#if 0
	size_t pid = 1;		// 0-indexed, but 0th entry is null
	const global_entry_pool<process_tag>& proc_entry_pool(*this);
	// Could re-write in terms of begin() and end() iterators.  
	const size_t plim = proc_entry_pool.size();
	for ( ; pid < plim; pid++) {
		production_rule_substructure::accept(proc_entry_pool[pid], v);
	}
#else
	v.visit(*this);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, just written to force instantiation.  
 */
void
state_manager::allocate_test(void) {
	__allocate_test<process_tag>();
	__allocate_test<channel_tag>();
#if ENABLE_DATASTRUCTS
	__allocate_test<datastruct_tag>();
#endif
	__allocate_test<enum_tag>();
	__allocate_test<int_tag>();
	__allocate_test<bool_tag>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is just nonsense code for the sake of instantiating template
	methods.  
 */
void
state_manager::__collect_subentries_test(void) const {
	entry_collection foo;
	collect_subentries<process_tag>(foo, 1);
	collect_subentries<channel_tag>(foo, 1);
#if ENABLE_DATASTRUCTS
	collect_subentries<datastruct_tag>(foo, 1);
#endif
	collect_subentries<enum_tag>(foo, 1);
	collect_subentries<int_tag>(foo, 1);
	collect_subentries<bool_tag>(foo, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_manager::collect_transient_info_base(persistent_object_manager& m, 
		const footprint& f) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	global_entry_pool<process_tag>::collect_transient_info_base(m, f);
#if ENABLE_DATASTRUCTS
	global_entry_pool<datastruct_tag>::collect_transient_info_base(m, f);
#endif
	global_entry_pool<channel_tag>::collect_transient_info_base(m, f);
#if 0
	// these cannot contain pointers... yet
	global_entry_pool<enum_tag>::collect_transient_info_base(m, f);
	global_entry_pool<int_tag>::collect_transient_info_base(m, f);
	global_entry_pool<bool_tag>::collect_transient_info_base(m, f);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_manager::write_object_base(const persistent_object_manager& m, 
		ostream& o, const footprint& f) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	global_entry_pool<process_tag>::write_object_base(m, o, f);
#if ENABLE_DATASTRUCTS
	global_entry_pool<datastruct_tag>::write_object_base(m, o, f);
#endif
	global_entry_pool<channel_tag>::write_object_base(m, o, f);
	global_entry_pool<enum_tag>::write_object_base(m, o, f);
	global_entry_pool<int_tag>::write_object_base(m, o, f);
	global_entry_pool<bool_tag>::write_object_base(m, o, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_manager::load_object_base(const persistent_object_manager& m, 
		istream& i, const footprint& f) {
	STACKTRACE_PERSISTENT_VERBOSE;
	global_entry_pool<process_tag>::load_object_base(m, i, f);
#if ENABLE_DATASTRUCTS
	global_entry_pool<datastruct_tag>::load_object_base(m, i, f);
#endif
	global_entry_pool<channel_tag>::load_object_base(m, i, f);
	global_entry_pool<enum_tag>::load_object_base(m, i, f);
	global_entry_pool<int_tag>::load_object_base(m, i, f);
	global_entry_pool<bool_tag>::load_object_base(m, i, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

