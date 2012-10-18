/**
	\file "main/options_modifier.tcc"
	Implementation details of options modifier manager.  
	$Id: options_modifier.tcc,v 1.3 2006/04/28 03:20:14 fang Exp $
 */

#ifndef	__HAC_MAIN_OPTIONS_MODIFIER_TCC__
#define	__HAC_MAIN_OPTIONS_MODIFIER_TCC__

#include "util/static_trace.hh"
#include "main/options_modifier.hh"
#include <string>
#include <map>
#include <iostream>

namespace HAC {
using std::string;
#include "util/using_ostream.hh"
//=============================================================================
/**
	TODO: specialize for different return types. 
 */
OPTIONS_MODIFIER_POLICY_TEMPLATE_SIGNATURE
class OPTIONS_MODIFIER_POLICY_CLASS::options_modifier_info {
	modifier_type				op;
	string					brief;
public:
	options_modifier_info() : op(NULL), brief() { }

	options_modifier_info(const modifier_type o, const char* b) :
		op(o), brief(b) { }

	options_modifier_info(const modifier_type o, const string& b) :
		op(o), brief(b) { }

	operator bool () const { return op; }

	const string&
	get_brief(void) const { return brief; }

	void
	operator () (options_type& c) const {
		NEVER_NULL(op);
		(*op)(c);
	}

};	// end class options_modifier_info

//=============================================================================
/**
	\param T the host class, that contains a static member
		of the options_modifier_map_type named options_modifier_map.
 */
OPTIONS_MODIFIER_POLICY_TEMPLATE_SIGNATURE
class OPTIONS_MODIFIER_POLICY_CLASS::register_options_modifier_base {
public:
	register_options_modifier_base(const string& Mode,
			const modifier_type COM,
			const string& b) {
		options_modifier_map_type&
			omm(const_cast<options_modifier_map_type&>(
				options_modifier_map));
		NEVER_NULL(COM);
		options_modifier_info& i(omm[Mode]);
		INVARIANT(!i);
		// i.op = COM; i.brief = b;
		i = options_modifier_info(COM, b);
	}
};      // end class register_options_modifier

//=============================================================================
#if 0
OPTIONS_MODIFIER_POLICY_TEMPLATE_SIGNATURE
string
OPTIONS_MODIFIER_POLICY_CLASS::default_options_brief;
#endif

STATIC_TRACE_HERE("before options_modifier_map")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initializer for modifier registry.  
	NOTE: this needs to be instantiated explicitly, rather than
	implicitly before it is used.  
 */
OPTIONS_MODIFIER_POLICY_TEMPLATE_SIGNATURE
const typename OPTIONS_MODIFIER_POLICY_CLASS::options_modifier_map_type
OPTIONS_MODIFIER_POLICY_CLASS::options_modifier_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OPTIONS_MODIFIER_POLICY_TEMPLATE_SIGNATURE
ostream&
OPTIONS_MODIFIER_POLICY_CLASS::dump_options_briefs(ostream& o) {
	typedef typename options_modifier_map_type::const_iterator
						const_iterator;
	const_iterator i(options_modifier_map.begin());
	const const_iterator e(options_modifier_map.end());
	for ( ; i!=e; i++) {
		cerr << '\t' << i->first << " : " <<
			i->second.get_brief() << endl;
	}
	return o;
}

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_OPTIONS_MODIFIER_TCC__

