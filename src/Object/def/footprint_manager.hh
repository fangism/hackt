/**
	\file "Object/def/footprint_manager.hh"
	Map of template parameters to definition footprints.  
	$Id: footprint_manager.hh,v 1.9 2009/03/09 07:30:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_MANAGER_H__
#define	__HAC_OBJECT_DEF_FOOTPRINT_MANAGER_H__

#include <iosfwd>
#include <set>
#include "util/memory/count_ptr.hh"
#include "util/persistent_fwd.hh"

namespace HAC {
namespace entity {
class footprint;
struct dump_flags;
struct expr_dump_context;
class definition_base;
class const_param_expr_list;
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

	explicit
	footprint_entry(footprint* f);
	~footprint_entry();

	bool
	operator < (const footprint_entry&) const;
};

//=============================================================================
typedef	std::set<footprint_entry>		footprint_manager_base;
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
	typedef	const_param_expr_list			key_type;
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
	footprint_manager(const size_t N, const definition_base&);

	~footprint_manager();

	size_t
	arity(void) const { return _arity; }

	void
	set_arity(const size_t, const definition_base&);

	ostream&
	dump(ostream&, const dump_flags&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	mapped_type&
	insert(const key_type& k, const definition_base&);

	mapped_type&
	insert(const count_ptr<const key_type>&, const definition_base&);

	mapped_type*
	lookup(const key_type& k) const;

	mapped_type*
	lookup(const count_ptr<const key_type>& k) const;

	using parent_type::size;
	using parent_type::empty;

	mapped_type&
	only(const definition_base&);

	const mapped_type&
	only(void) const;

// map persistence methods
private:
	using parent_type::insert;
public:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&,
		const definition_base&);

};	// end class footprint_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_FOOTPRINT_MANAGER_H__

