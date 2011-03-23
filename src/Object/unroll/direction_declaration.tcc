/**
	\file "Object/unroll/direction_declaration.tcc"
	Implementation of generic attribute statements.  
	$Id: direction_declaration.tcc,v 1.2 2011/03/23 00:36:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_DIRECTION_DECLARATION_TCC__
#define	__HAC_OBJECT_UNROLL_DIRECTION_DECLARATION_TCC__

#include <vector>
#include "Object/unroll/direction_declaration.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/type/channel_type_reference_base.h"
#include "util/persistent_object_manager.tcc"
#include "util/what.tcc"
#include "util/stacktrace.h"
#include "common/TODO.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::persistent_traits;

//=============================================================================
// class direction_declaration method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIRECTION_DECLARATION_TEMPLATE_SIGNATURE
DIRECTION_DECLARATION_CLASS::direction_declaration() :
		instance_management_base(), ref(), dir() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIRECTION_DECLARATION_TEMPLATE_SIGNATURE
DIRECTION_DECLARATION_CLASS::direction_declaration(const reference_ptr_type& r, 
		const direction_type a) :
		instance_management_base(), ref(r), dir(a) {
	NEVER_NULL(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIRECTION_DECLARATION_TEMPLATE_SIGNATURE
DIRECTION_DECLARATION_CLASS::~direction_declaration() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIRECTION_DECLARATION_TEMPLATE_SIGNATURE
ostream&
DIRECTION_DECLARATION_CLASS::what(ostream& o) const {
	// needs specialization
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIRECTION_DECLARATION_TEMPLATE_SIGNATURE
ostream&
DIRECTION_DECLARATION_CLASS::dump(ostream& o, const expr_dump_context& c) const {
	ref->dump(o, c);
	return channel_type_reference_base::dump_direction(o, dir);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Use template_type_completion::unroll as a reference...
 */
DIRECTION_DECLARATION_TEMPLATE_SIGNATURE
good_bool
DIRECTION_DECLARATION_CLASS::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	typedef	typename reference_type::alias_collection_type
					alias_collection_type;
	alias_collection_type aliases;
	if (ref->unroll_references_packed(c, aliases).bad) {
		cerr << "Error resolving references of direction declaration."
			<< endl;
		return good_bool(false);
	}
	// apply to all aliases
	typedef	typename alias_collection_type::const_iterator
					alias_iterator;
	alias_iterator ai(aliases.begin()), ae(aliases.end());
	bool good = true;
	for ( ; ai!=ae; ++ai) {
		NEVER_NULL(*ai);
		// apply attribute to *canonical* alias
		if (!(*ai)->find()->declare_direction(dir).good) {
			// have error message
			good = false;
		}
	}
	return good_bool(good);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIRECTION_DECLARATION_TEMPLATE_SIGNATURE
void
DIRECTION_DECLARATION_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	ref->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIRECTION_DECLARATION_TEMPLATE_SIGNATURE
void
DIRECTION_DECLARATION_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, ref);
	const char d = dir;
	util::write_value(o, d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIRECTION_DECLARATION_TEMPLATE_SIGNATURE
void
DIRECTION_DECLARATION_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, ref);
	char d;
	util::read_value(i, d);
	dir = direction_type(d);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_DIRECTION_DECLARATION_TCC__

