/**
	\file "Object/inst/state_instance.tcc"
	Class implementation for instance state.  
	$Id: state_instance.tcc,v 1.2.4.1 2005/08/11 00:20:20 fang Exp $
 */

#ifndef	__OBJECT_INST_STATE_INSTANCE_TCC__
#define	__OBJECT_INST_STATE_INSTANCE_TCC__

#include <iostream>
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"
// #include "util/persistent_object_manager.h"
#include "util/stacktrace.h"
// #include "util/IO_utils.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
#if 0
using util::write_value;
using util::read_value;
#endif

//=============================================================================
// class state_instance method definitions

/**
	State instance pool, currently with hard-coded pool chunk size.  
	This is the static initialization.  
	TODO: use class_traits to give each pool different chunk size.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
typename STATE_INSTANCE_CLASS::pool_type
STATE_INSTANCE_CLASS::pool(class_traits<Tag>::instance_pool_chunk_size);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
STATE_INSTANCE_CLASS::state_instance() :
		state_instance_base(), back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
STATE_INSTANCE_CLASS::state_instance(const alias_info_type& a) :
		state_instance_base(), back_ref(&a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
STATE_INSTANCE_CLASS::~state_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now, just prints the canonical back-link.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
ostream&
STATE_INSTANCE_CLASS::dump(ostream& o) const {
	NEVER_NULL(back_ref);
	back_ref->dump_alias(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't actually register the back-reference because it is
	not directly managed by the persistent object manager.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::collect_transient_info_base(
		persistent_object_manager&) const {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves the canonical alias back-reference.
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	NEVER_NULL(back_ref);
	back_ref->write_next_connection(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores the canonical alias back-reference.
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::load_object_base(const persistent_object_manager& m,
		istream& i) {
	this->back_ref = never_ptr<const alias_info_type>(
		&alias_info_type::load_alias_reference(m, i));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Dumps the state of the entire pool of instances.  
	Displays the unique ID number and each instance's state information.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
ostream&
STATE_INSTANCE_CLASS::dump_pool_state(ostream& o) {
	if (pool.size() > 1) {
		o << class_traits<Tag>::tag_name << " instance pool:" << endl;
		// skip first element (NULL)
		const_pool_iterator i(++pool.begin());
		const const_pool_iterator e(pool.end());
		size_t j = 1;
		for ( ; i!=e; i++, j++) {
			i->dump(o << j << '\t') << endl;
		}
	}
	// else pool is empty
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::collect_pool_state(persistent_object_manager& m) {
	const_pool_iterator i(++pool.begin());	// skip first element (NULL)
	const const_pool_iterator e(pool.end());
	for ( ; i!=e; i++) {
		i->collect_transient_info_base(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::write_pool_state(const persistent_object_manager& m, 
		ostream& o) {
	const_pool_iterator i(++pool.begin());	// skip first element (NULL)
	const const_pool_iterator e(pool.end());
	const size_t s = pool.size();
	INVARIANT(s);
	write_value(o, s-1);
	size_t j = 1;
	for ( ; i!=e; i++, j++) {
		i->write_object_base(m, o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: pre-reserving memory is not necessary, because the 
	instance_pool implementation (list_vector) already reserves
	chunks in advance as necessary.  
	\pre the pool is already globally statically initialized.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
void
STATE_INSTANCE_CLASS::load_pool_state(const persistent_object_manager& m, 
		istream& i) {
	size_t s;
	read_value(i, s);
	size_t j=0;
	for ( ; j<s; j++) {
		this_type temp;
		temp.load_object_base(m, i);
		pool.allocate(temp);
		// works because this_type is copy-constructible
	}
}
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_STATE_INSTANCE_TCC__

