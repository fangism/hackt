/**
	\file "Object/common/dump_flags.hh"
	Dump attributes class.  
	$Id: dump_flags.hh,v 1.8 2010/08/24 18:08:36 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_DUMP_FLAGS_H__
#define	__HAC_OBJECT_COMMON_DUMP_FLAGS_H__

#include <iosfwd>
#include <string>

namespace HAC {
namespace entity {
using std::string;
using std::ostream;
using std::istream;

/**
	Common dump flags used to tweak output formatting and style.  
 */
struct dump_flags {
	/**
		Whether or not to print the scope to which a particuluar
		hierarchical instance belongs.  
		This SHOULD print namespace owners, but NOT definition owners.  
		Default value: true.
		Will want to be false when piecing together hierarchical
		canonical names, see global_entry::dump_canonical_name().  
		See use in instance_collection_base::dump_hierarchical_name.
	 */
	bool	show_definition_owner;
	/**
		Whether or not to show namespace owners.  
	 */
	bool	show_namespace_owner;
	/**
		Whether or not the a scoped identifier should print 
		its leading "::" from the global namespace.  
		This is typically off for cflat/PRS outputs.  
	 */
	bool	show_leading_scope;
	/**
		The character to use to separate process instance members, 
		default is '.'.
		Used in instance_collection_base::dump_hierarchical_name.
	 */
	string	process_member_separator;
	/**
		The character to use to separate channel or datatype members, 
		default is '.'.
		Used in instance_collection_base::dump_hierarchical_name.
	 */
	string	struct_member_separator;

	/// default constructor
	dump_flags();
private:
	explicit
	dump_flags(const bool _show_def_owner,
		const bool _show_ns_owner, 
		const bool _show_leading);
public:
	~dump_flags() { }

	/**
		Static const global default values.  
		(Shouldn't be used before the end of global initialization.)
		None of these should be referenced until main begins.  
	 */
	static const dump_flags			default_value;
	/**
		Another common value for printing.  
		Useful for context which can only refer to local names.  
		Good for value/parameter references.
	 */
	static const dump_flags			no_definition_owner;
	/**
		Another common value for printing.  
		Useful for context which can only refer to local names.  
		Good for physical instance references (local only).
	 */
	static const dump_flags			no_definition_owner_no_ns;
	/**
		When show_namespace_owner or show_definition_owner is true, 
		this disables show_leading_scope false, 
		appropriate for PRS names.
	 */
	static const dump_flags			no_leading_scope;

	/**
		Neither definition nor namespace owners.  
	 */
	static const dump_flags			no_owners;

	/**
		Show everything verbosely.  
	 */
	static const dump_flags			verbose;

	ostream&
	dump_brief(ostream&) const;

	ostream&
	dump(ostream&) const;

	// because we want to save/restore flags in some cases...
	void
	write_object(ostream&) const;

	void
	load_object(istream&);

};	// end class dump_flags

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_DUMP_FLAGS_H__

