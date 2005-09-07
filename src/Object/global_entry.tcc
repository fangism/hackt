/**
	\file "Object/global_entry.tcc"
	$Id: global_entry.tcc,v 1.1.2.2 2005/09/07 19:21:03 fang Exp $
 */

#ifndef	__OBJECT_GLOBAL_ENTRY_TCC__
#define	__OBJECT_GLOBAL_ENTRY_TCC__

#include <iostream>
#include "util/macros.h"
#include "Object/global_entry.h"
#include "util/IO_utils.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::read_value;
using util::write_value;

//=============================================================================
// class global_entry method definitions

template <class Tag>
global_entry<Tag>::global_entry() : parent_type(), parent_tag_value(0), 
		parent_id(0), local_offset(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
global_entry<Tag>::~global_entry() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
global_entry<Tag>::dump(ostream& o) const {
	switch(parent_tag_value) {
	case NONE:
		break;
	case PROCESS:
		o << "process ";
		break;
	case CHANNEL:
		o << "channel ";
		break;
	case STRUCT:
		o << "struct  ";
		break;
	default:
		THROW_EXIT;
	}
	if (parent_tag_value) {
		o << parent_id << '\t';
	}
	o << local_offset << '\t';
	return parent_type::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry<Tag>::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	parent_type::write_object_base(m, o);
	write_value(o, parent_tag_value);
	write_value(o, parent_id);
	write_value(o, local_offset);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry<Tag>::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	parent_type::load_object_base(m, i);
	read_value(i, parent_tag_value);
	read_value(i, parent_id);
	read_value(i, local_offset);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_GLOBAL_ENTRY_TCC__

