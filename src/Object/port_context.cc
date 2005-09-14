/**
	\file "Object/port_context.h"
	$Id: port_context.cc,v 1.1.2.3 2005/09/14 13:57:35 fang Exp $
 */

#include <iostream>
#include "Object/port_context.h"
#include "util/macros.h"
#include "util/indent.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::auto_indent;

//=============================================================================
// class port_member_context method definitions

port_member_context::port_member_context() : member_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
port_member_context::~port_member_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Debug dumping.  
 */
ostream&
port_member_context::dump(ostream& o) const {
if (!member_array.empty()) {
	o << '{' << endl;
	{
		INDENT_SECTION(o);
		size_t i = 0;
		const size_t s = size();
		for ( ; i<s; i++) {
			member_array[i].dump(
				o << auto_indent << i << ": ") << endl;
		}
	}
	return o << auto_indent << '}';
} else	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
port_member_context::size(void) const {
	return member_array.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_member_context::resize(const size_t s) {
	member_array.resize(s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
port_collection_context&
port_member_context::operator [] (const size_t i) {
	return member_array[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const port_collection_context&
port_member_context::operator [] (const size_t i) const {
	return member_array[i];
}

//=============================================================================
// class port_collection_context method definitions

port_collection_context::port_collection_context() :
		id_map(), substructure_array() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
port_collection_context::~port_collection_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_collection_context::dump(ostream& o) const {
	INVARIANT(id_map.size() == substructure_array.size());
if (id_map.size()) {
	o << '{' << endl;
	{
		INDENT_SECTION(o);
		size_t i = 0;
		const size_t s = size();
		for ( ; i<s; i++) {
			substructure_array[i].dump(
				o << auto_indent << i <<
				" -> " << id_map[i]<< ": ") << endl;
		}
	}
	return o << auto_indent << '}';
} else	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_collection_context::resize(const size_t s) {
	id_map.resize(s);
	substructure_array.resize(s);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

