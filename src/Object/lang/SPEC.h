/**
	\file "Object/lang/SPEC.h"
	$Id: SPEC.h,v 1.1.2.2 2006/02/04 01:33:11 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_H__
#define	__HAC_OBJECT_LANG_SPEC_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "Object/lang/SPEC_fwd.h"
#include "Object/object_fwd.h"
#include "Object/lang/bool_literal.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
namespace SPEC {
using std::ostream;
using std::istream;
using std::string;
using util::good_bool;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Directive placeholder.  
	TODO: support parameters later. 
 */
class directive : public util::persistent {
	typedef	directive				this_type;
public:
	typedef	std::vector<literal_ptr_type>		args_type;
	typedef	args_type::const_reference		const_reference;
	struct dumper;
	struct unroller;
private:
	string						name;
	args_type					nodes;
public:
	directive();

	explicit
	directive(const string&);

	~directive();

#define	SPEC_UNROLL_DIRECTIVE_PROTO					\
	good_bool							\
	unroll(const unroll_context&, const node_pool_type&, 		\
		SPEC::footprint&) const

	SPEC_UNROLL_DIRECTIVE_PROTO;

	void
	push_back(const_reference);

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

	PERSISTENT_METHODS_DECLARATIONS
	// CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class directive

//=============================================================================
typedef	std::vector<count_ptr<const directive> >	directives_set_base;
/**
	A set of spec directives.  
	Analogous to PRS::rule_set.  
 */
class directives_set : private directives_set_base {
	typedef	directives_set_base			parent_type;
public:
	typedef	parent_type::const_reference		const_reference;
public:
	directives_set();
	~directives_set();

	using parent_type::push_back;

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

	SPEC_UNROLL_DIRECTIVE_PROTO;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class spec_set

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_H__

