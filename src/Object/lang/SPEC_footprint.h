/**
	\file "Object/lang/SPEC_footprint.h"
	$Id: SPEC_footprint.h,v 1.3.140.1 2010/02/10 06:43:06 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_FOOTPRINT_H__
#define	__HAC_OBJECT_LANG_SPEC_FOOTPRINT_H__

#include "Object/lang/directive_base.h"
#include "Object/lang/SPEC_fwd.h"
#include "Object/devel_switches.h"
// #include "Object/lang/cflat_visitee.h"
#include "util/persistent_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class footprint;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
struct global_entry_context;
#endif
namespace SPEC {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
typedef	global_entry_context		spec_visitor;
#else
typedef	PRS::cflat_visitor		spec_visitor;
#endif

//=============================================================================
/**
	A create-time resolved spec directive.  
	Modeled after PRS::footprint_macro.
 */
class footprint_directive : 
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		public PRS::cflat_visitee, 
#endif
		public directive_base {
public:
	footprint_directive() : directive_base() { }

	explicit
	footprint_directive(const string& k) : directive_base(k) { }

	// everything else inherited from directive_base

	void
	accept(spec_visitor&) const;

};	// end class footprint directive.

//=============================================================================
// could use list_vector? not anticipated to be huge...
typedef	std::vector<footprint_directive>	footprint_base_type;
//=============================================================================
/**
	This spec footprint is a signature of a complete type
	whose references are resolved.  
 */
class footprint : private footprint_base_type
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		, public PRS::cflat_visitee
#endif
		{
public:
	footprint();
	~footprint();

private:
	static
	ostream&
	dump_directive(const footprint_directive&, ostream&,
		const node_pool_type&);
public:

	ostream&
	dump(ostream&, const entity::footprint&) const;

	using footprint_base_type::begin;
	using footprint_base_type::end;

	footprint_directive&
	push_back_directive(const string&);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	void
	accept(spec_visitor&) const;

};	// end class footprint

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_FOOTPRINT_H__

