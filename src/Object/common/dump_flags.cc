/**
	\file "Object/common/dump_flags.cc"
	Global static initializer for default.  
	$Id: dump_flags.cc,v 1.7 2010/08/24 18:08:35 fang Exp $
 */

#include <iostream>
#include "Object/common/dump_flags.hh"
#include "util/IO_utils.hh"
#include "Object/devel_switches.hh"

namespace HAC {
namespace entity {
using util::write_value;
using util::read_value;
#include "util/using_ostream.hh"

//=============================================================================
// class dump_flags method and static member definitions

const dump_flags
dump_flags::default_value = dump_flags();

const dump_flags
dump_flags::no_definition_owner_no_ns = dump_flags(false, false, true);

const dump_flags
dump_flags::no_definition_owner = dump_flags(false, true, true);

const dump_flags
dump_flags::no_owners = dump_flags(false, false, false);

/**
	This is appropriate for cflat use.  
 */
const dump_flags
dump_flags::no_leading_scope = dump_flags(false, false, false);

/**
	Verbose dumping flags for diagnostics.  
 */
const dump_flags
dump_flags::verbose = dump_flags(true, true, true);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dump_flags::dump_flags() :
		show_definition_owner(true),
		show_namespace_owner(true),
		show_leading_scope(true),
		process_member_separator("."),
		struct_member_separator(".")
{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
dump_flags::dump_flags(const bool _show_def_owner, 
		const bool _show_ns_owner, 
		const bool _show_leading_scope) :
		show_definition_owner(_show_def_owner),
		show_namespace_owner(_show_ns_owner),
		show_leading_scope(_show_leading_scope),
		process_member_separator("."),
		struct_member_separator(".")
{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
dump_flags::dump_brief(ostream& o) const {
        o << "[dump flags:" << (show_definition_owner ? "(def)" : "") <<
                (show_namespace_owner ? "(ns)" : "") <<
                (show_leading_scope ? "(::)]" : "]");
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
dump_flags::dump(ostream& o) const {
	o << (show_definition_owner ? "show" : "hide") <<
		" definition owner" << endl;
	o << (show_namespace_owner ? "show" : "hide") <<
		" namespace owner" << endl;
	o << (show_leading_scope ? "show" : "hide") <<
		" leading scope" << endl;
	o << "process member separatorr: " << process_member_separator << endl;
	o << "struct member separatorr: " << struct_member_separator << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
dump_flags::write_object(ostream& o) const {
	write_value(o, show_definition_owner);
	write_value(o, show_namespace_owner);
	write_value(o, show_leading_scope);
	write_value(o, process_member_separator);
	write_value(o, struct_member_separator);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
dump_flags::load_object(istream& i) {
	read_value(i, show_definition_owner);
	read_value(i, show_namespace_owner);
	read_value(i, show_leading_scope);
	read_value(i, process_member_separator);
	read_value(i, struct_member_separator);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

