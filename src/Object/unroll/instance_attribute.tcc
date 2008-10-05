/**
	\file "Object/unroll/instance_attribute.tcc"
	Implementation of generic attribute statements.  
	$Id: instance_attribute.tcc,v 1.1 2008/10/05 23:00:35 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTE_TCC__
#define	__HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTE_TCC__

#include "Object/unroll/instance_attribute.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "util/persistent_object_manager.tcc"
#include "util/what.tcc"
#include "common/TODO.h"

namespace HAC {
namespace entity {
using util::persistent_traits;

//=============================================================================
// class instance_attribute method definitions

INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
INSTANCE_ATTRIBUTE_CLASS::instance_attribute() :
		instance_management_base(), ref(), attrs() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
INSTANCE_ATTRIBUTE_CLASS::instance_attribute(const reference_ptr_type& r, 
		const generic_attribute_list_type& a) :
		instance_management_base(), ref(r), attrs(a) {
	NEVER_NULL(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
INSTANCE_ATTRIBUTE_CLASS::~instance_attribute() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ATTRIBUTE_CLASS::what(ostream& o) const {
	// needs specialization
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ATTRIBUTE_CLASS::dump(ostream& o, const expr_dump_context& c) const {
	ref->dump(o, c);
	return attrs.dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: lookup map of functions to apply to alias
	Use template_type_completion::unroll as a reference...
 */
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ATTRIBUTE_CLASS::unroll(const unroll_context& c) const {
	FINISH_ME(Fang);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
void
INSTANCE_ATTRIBUTE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	ref->collect_transient_info(m);
	attrs.collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
void
INSTANCE_ATTRIBUTE_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, ref);
	attrs.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
void
INSTANCE_ATTRIBUTE_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, ref);
	attrs.load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTE_TCC__

