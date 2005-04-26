/**
	\file "art_object_expr_gcc41death.h"
	Classes related to program expressions, symbolic and parameters.  
	$Id: art_object_expr_gcc41death.h,v 1.1.2.2 2005/04/26 00:13:08 fang Exp $
 */

#ifndef __ART_OBJECT_EXPR_GCC41DEATH_H__
#define __ART_OBJECT_EXPR_GCC41DEATH_H__

#include "persistent_gcc41death.h"
#include "memory/count_ptr_gcc41death.h"

//=============================================================================
// note: need some way of hashing expression? 
//	using string of fully-qualified name?

/***
	Instead of making two flavors of queries, use an integer return
	value instead of a bool to enumerate:
		definitely no, don't know (maybe), definitely yes.  
		do we need may be yes, may be no?
	May be bit fields?
***/

//=============================================================================
namespace ART {
namespace entity {

using std::istream;
using std::ostream;
using util::persistent_object_manager;
using util::memory::count_ptr;
using util::persistent;

class object {
public:
virtual ~object() { }
};

//-----------------------------------------------------------------------------
class index_expr : public object, public persistent {
	// need inheritance to ICE, need not be virtual parents
protected:
        index_expr();

public:
virtual ~index_expr();

};      // end class index_expr

//-----------------------------------------------------------------------------
/**
	Elements of this index list are no necessarily static constants.  
 */
class dynamic_index_list : public persistent {
public:
	dynamic_index_list();
	~dynamic_index_list();

	ostream&
	what(ostream& o) const;

public:
#if 0
        void
        write_object(const persistent_object_manager&, ostream&) const;
#endif
        void
        load_object(const persistent_object_manager&, istream&);
#if 0
        void
        collect_transient_info(persistent_object_manager&) const;
#endif

};	// end class dynamic_index_list

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __ART_OBJECT_EXPR_GCC41DEATH_H__

