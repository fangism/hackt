/**
	\file "Object/def/footprint_manager.h"
	Map of template parameters to definition footprints.  
	$Id: footprint_manager.h,v 1.1.2.1 2005/08/10 20:30:54 fang Exp $
 */

#ifndef	__OBJECT_DEF_FOOTPRINT_MANAGER_H__
#define	__OBJECT_DEF_FOOTPRINT_MANAGER_H__

#include <iosfwd>
#include <map>
#include "Object/expr/const_param_expr_list.h"
#include "Object/def/footprint.h"
#include "util/persistent_fwd.h"

namespace ART {
namespace entity {
using std::istream;
using std::ostream;
using util::persistent_object_manager;
//=============================================================================
/**
	The manager for a collection of definition footprints.  
	We use std::map instead of util::qmap because we want the
	operator[] to always return a reference, i.e. default construct
	a pair if necessary.  
 */
class footprint_manager :
		private std::map<const_param_expr_list, footprint> {
private:
	typedef	footprint_manager			this_type;
	typedef	std::map<const_param_expr_list, footprint>	parent_type;
public:
	typedef	parent_type::key_type			key_type;
	typedef	parent_type::value_type			value_type;
	typedef	parent_type::mapped_type		mapped_type;
private:
	typedef	parent_type::const_iterator		const_iterator;
private:
	/**
		The number of arguments required for the key_type.
	 */
	size_t						_arity;
public:
	footprint_manager();

	explicit
	footprint_manager(const size_t N);

	~footprint_manager();

	size_t
	arity(void) const { return _arity; }

	ostream&
	dump(ostream&) const;

	mapped_type&
	operator [] (const key_type& k);

// map persistence methods
public:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class footprint_manager

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_DEF_FOOTPRINT_MANAGER_H__

