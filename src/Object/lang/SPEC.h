/**
	\file "Object/lang/SPEC.h"
	$Id: SPEC.h,v 1.1.2.1 2006/02/03 05:42:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_H__
#define	__HAC_OBJECT_LANG_SPEC_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "Object/object_fwd.h"
#include "Object/lang/bool_literal.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"

namespace HAC {
namespace entity {
namespace PRS {
	class literal;
	class rule_dump_context;
}
namespace SPEC {
using std::ostream;
using std::istream;
using std::string;
using util::memory::count_ptr;
using util::persistent_object_manager;

typedef	count_ptr<PRS::literal>				literal_ptr_type;

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
private:
	string						name;
	args_type					nodes;
public:
	directive();

	explicit
	directive(const string&);

	~directive();

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
	directives_set();
	~directives_set();

	ostream&
	dump(ostream&, const PRS::rule_dump_context&) const;

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

