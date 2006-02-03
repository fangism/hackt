/**
	\file "Object/lang/SPEC.cc"
	$Id: SPEC.cc,v 1.1.2.1 2006/02/03 05:42:03 fang Exp $
 */

#include <iostream>
#include <algorithm>
#include "Object/lang/SPEC.h"
#include "Object/lang/PRS.h"	// for PRS::literal
#include "Object/persistent_type_hash.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/indent.h"
#include "util/what.h"

namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::SPEC::directive, "SPEC::directive")
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::SPEC::directive, SPEC_DIRECTIVE_TYPE_KEY, 0)
}

namespace HAC {
namespace entity {
namespace SPEC {
using std::for_each;
using util::auto_indent;
#include "util/using_ostream.h"
using PRS::rule_dump_context;

//=============================================================================
// class directive method definitions

directive::directive() : name(), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive::directive(const string& n) : name(n), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive::~directive() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive::push_back(const_reference r) {
	nodes.push_back(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(directive)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off PRS::macro::dump().
 */
ostream&
directive::dump(ostream& o, const PRS::rule_dump_context& c) const {
	o << name << '(';
	typedef args_type::const_iterator      const_iterator;
	INVARIANT(nodes.size());
	const_iterator i(nodes.begin());
	const const_iterator e(nodes.end());
	NEVER_NULL(*i);
	(*i)->dump(o, c);
	for (++i; i!=e; ++i) {
		NEVER_NULL(*i);
		(*i)->dump(o << ',', c);
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(nodes);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive::write_object(const persistent_object_manager& m, ostream& o) const {
	m.write_pointer_list(o, nodes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive::load_object(const persistent_object_manager& m, istream& i) {
	m.read_pointer_list(i, nodes);
}

//=============================================================================
// helper class definitions

/**
	ostream binding functor.  
 */
struct directive::dumper {
	ostream&				os;
	const rule_dump_context&		rdc;

	dumper(ostream& o, const rule_dump_context& r) : os(o), rdc(r) { }

	template <class P>
	void
	operator () (const P& p) {
		NEVER_NULL(p);
		p->dump(os << auto_indent, rdc) << endl;
	}
};	// end struct directive::dumper

//=============================================================================
// class directives_set method definitions

directives_set::directives_set() : directives_set_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directives_set::~directives_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directives_set::dump(ostream& o, const rule_dump_context& rdc) const {
	for_each(begin(), end(), directive::dumper(o, rdc));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_set::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(AS_A(const parent_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_set::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	m.write_pointer_list(o, AS_A(const parent_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directives_set::load_object_base(const persistent_object_manager& m,
		istream& i) {
	m.read_pointer_list(i, AS_A(parent_type&, *this));
}

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

