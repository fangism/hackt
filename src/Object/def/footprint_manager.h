/**
	\file "Object/def/footprint_manager.h"
	Map of template parameters to definition footprints.  
	$Id: footprint_manager.h,v 1.7.4.3 2006/11/06 21:45:47 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_FOOTPRINT_MANAGER_H__
#define	__HAC_OBJECT_DEF_FOOTPRINT_MANAGER_H__

#include <iosfwd>
#include <map>
#include "Object/expr/const_param_expr_list.h"
#include "util/memory/excl_ptr.h"
#include "util/persistent_fwd.h"

namespace HAC {
namespace entity {
class footprint;
struct dump_flags;
struct expr_dump_context;
using std::istream;
using std::ostream;
using util::memory::excl_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Proxy class for exclusively owned pointer to footprint.
	Also serves as implementation privatization.  
 */
struct footprint_entry : public excl_ptr<footprint> {
	typedef	excl_ptr<footprint>		ptr_type;

	footprint_entry();
	footprint_entry(const footprint_entry&);
	~footprint_entry();

	footprint_entry&
	operator = (ptr_type&);
};

//=============================================================================
/**
	The manager for a collection of definition footprints.  
	We use std::map instead of util::qmap because we want the
	operator[] to always return a reference, i.e. default construct
	a pair if necessary.  
	Implementation: use const_param_expr_list or pointer thereof?
 */
class footprint_manager :
		private std::map<const_param_expr_list, footprint_entry> {
private:
	typedef	footprint_manager			this_type;
	typedef	std::map<const_param_expr_list, footprint_entry>
							parent_type;
public:
	typedef	parent_type::key_type			key_type;
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

