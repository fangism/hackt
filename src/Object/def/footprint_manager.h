/**
	\file "Object/def/footprint_manager.h"
	Map of template parameters to definition footprints.  
	$Id: footprint_manager.h,v 1.8.74.2 2009/03/05 00:00:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_MANAGER_H__
#define	__HAC_OBJECT_DEF_FOOTPRINT_MANAGER_H__

#include "Object/devel_switches.h"
#include <iosfwd>
#if FOOTPRINT_HAS_PARAMS
#include <set>
#else
#include <map>
#endif
#include "Object/expr/const_param_expr_list.h"
#include "util/memory/count_ptr.h"
#include "util/persistent_fwd.h"

namespace HAC {
namespace entity {
class footprint;
struct dump_flags;
struct expr_dump_context;
using std::istream;
using std::ostream;
using util::memory::count_ptr;
using util::persistent_object_manager;

typedef	count_ptr<footprint>		footprint_entry_base;

//=============================================================================
/**
	Proxy class for exclusively owned pointer to footprint.
	Also serves as implementation privatization.  
 */
struct footprint_entry : public footprint_entry_base {
	typedef	footprint_entry_base		ptr_type;

	footprint_entry(const ptr_type&);

#if FOOTPRINT_HAS_PARAMS
	explicit
	footprint_entry(footprint* f);
#else
	footprint_entry();
#endif
	~footprint_entry();

#if FOOTPRINT_HAS_PARAMS
	bool
	operator < (const footprint_entry&) const;
#endif
};

//=============================================================================
#if FOOTPRINT_HAS_PARAMS
typedef	std::set<footprint_entry>
#else
typedef	std::map<const_param_expr_list, footprint_entry>
#endif
						footprint_manager_base;
/**
	The manager for a collection of definition footprints.  
	We use std::map instead of util::qmap because we want the
	operator[] to always return a reference, i.e. default construct
	a pair if necessary.  
	Implementation: use const_param_expr_list or pointer thereof?
 */
class footprint_manager :
		private footprint_manager_base {
private:
	typedef	footprint_manager			this_type;
	typedef	footprint_manager_base			parent_type;
public:
#if FOOTPRINT_HAS_PARAMS
	typedef	const_param_expr_list			key_type;
#else
	typedef	parent_type::key_type			key_type;
#endif
	typedef	parent_type::value_type			value_type;
	/**
		NOTE: the interface we provide makes the map look like
		the footprint is the mapped type.  
	 */
	typedef	footprint				mapped_type;
private:
	typedef	parent_type::const_iterator		const_iterator;
private:
	/**
		The number of arguments required for the key_type.
		TODO: explain what happens when comparing keys of length 0.  
	 */
	size_t						_arity;
public:
	footprint_manager();

	explicit
	footprint_manager(const size_t N);

	~footprint_manager();

	size_t
	arity(void) const { return _arity; }

	void
	set_arity(const size_t);

	ostream&
	dump(ostream&, const dump_flags&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	mapped_type&
	operator [] (const key_type& k);

	mapped_type&
	operator [] (const count_ptr<const key_type>&);

	using parent_type::size;
	using parent_type::empty;

	mapped_type&
	only(void);

	const mapped_type&
	only(void) const;

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
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FOOTPRINT_MANAGER_H__

