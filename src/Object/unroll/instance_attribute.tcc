/**
	\file "Object/unroll/instance_attribute.tcc"
	Implementation of generic attribute statements.  
	$Id: instance_attribute.tcc,v 1.3 2008/10/11 22:49:11 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTE_TCC__
#define	__HAC_OBJECT_UNROLL_INSTANCE_ATTRIBUTE_TCC__

#include "Object/unroll/instance_attribute.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/expr/const_param_expr_list.h"
#include "util/persistent_object_manager.tcc"
#include "util/what.tcc"
#include "util/stacktrace.h"
#include "common/TODO.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::persistent_traits;

//=============================================================================
// class instance_attribute method definitions

INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
bool
INSTANCE_ATTRIBUTE_CLASS::attribute_exists(const string& k) {
	return attribute_registry.find(k) != attribute_registry.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
	Use template_type_completion::unroll as a reference...
	For now, error out on unknown attributes.  
 */
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ATTRIBUTE_CLASS::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	typedef	typename reference_type::alias_collection_type
					alias_collection_type;
	alias_collection_type aliases;
	if (ref->unroll_references_packed(c, aliases).bad) {
		cerr << "Error resolving references of instance attribute."
			<< endl;
		return good_bool(false);
	}
	// for each alias, for each attribute... does loop order matter?
	typedef	generic_attribute_list_type::const_iterator
					attribute_iterator;
	attribute_iterator ti(attrs.begin()), te(attrs.end());
for ( ; ti!=te; ++ti) {
	const string& k(ti->get_key());
	const count_ptr<const const_param_expr_list>
		v(ti->unroll_values(c));
	if (!v) {
		cerr << "Error resolving attribute values of \'" << k
			<< "\'.  " << endl;
		return good_bool(false);
	}
	typename attribute_registry_type::const_iterator
		cf(attribute_registry.find(k));
	if (cf == attribute_registry.end()) {
		cerr << "Error: unknown instance attribute \'" << k
			<< "\'.  " << endl;
		return good_bool(false);
	}
	// check values separately?
	if (!cf->second.check_values(*v).good) {
		// already have error message
		return good_bool(false);
	}
	// apply to all aliases
	typedef	typename alias_collection_type::const_iterator
					alias_iterator;
	alias_iterator ai(aliases.begin()), ae(aliases.end());
	for ( ; ai!=ae; ++ai) {
		NEVER_NULL(*ai);
		// apply attribute to *canonical* alias
		cf->second.main(*(*ai)->find(), *v);
	}
}
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

