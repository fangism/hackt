/**
	\file "Object/devel_switches.h"
	Module-wide preprocessor conditionals.
	Use of this file (ideally) should be confined to the Object directory.  

	The sole purpose of this file is to provide a convenient
	place to include developer switches.  
	During development, it is often desirable to switch back and forth
	between two implementations and versions, controlled by a single
	preprocessor definition.  
	However, in production code, this file should be EMPTY, 
	and NO translation unit should depend on this i.e. do not include.  
	$Id: devel_switches.h,v 1.63 2011/03/23 00:36:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEVEL_SWITCHES_HH__
#define	__HAC_OBJECT_DEVEL_SWITCHES_HH__

// if we want project-wide switches
#if 0
#include "common/devel_switches.h"
#endif

// leave this error enabled for released code
#if 0
#error	Production code should NOT include this header file.  \
	However, if you are developing, use this file as you see fit.  
#endif

//=============================================================================
// define your module-wide development switches here:

/**
#define	ENABLE_RELAXED_TEMPLATE_PARAMETERS		1
	Status: complete (as of 00-01-04-main-00-81-40-merged-template-02-28)
	Perm'd: 00-01-04-main-00-82-56
**/

/**
	Define to 0 to remove formal instance management (template, port)
	from the main body of sequential unrolling.  
	Rationale: remove duplicates
	Goal: 0
	Status: applied to templates, haven't applied to ports yet
	Priority: low (if it ain't broke...), matter of consistency
 */
#define	SEQUENTIAL_SCOPE_INCLUDES_FORMALS	0

/**
	Define to 1 to use dynamic_meta_index_list instead of
	abstract meta_index_list, again because we don't need
	compile-time static analysis anymore.
	Goal: 1
	Status: not begun
	Priority: low
	NOTE: can also replace const_param_expr_list-pointers with
	non-pointers to reduce heap-allocations.  
 */
#define	ALWAYS_USE_DYNAMIC_INDEX_LIST		0

/**
	Define to 1 to allow the parse_context class to support 
	nested outstanding definitions.  
	Useful for allowing top-level scope to be considered a
	definition, where module <= process_definition.  
	Also allows future support for nested definitions. 
	Status: perm'd (00-01-04-main-00-81-81)
#define	SUPPORT_NESTED_DEFINITIONS	1
 */

/**
	Define to 1 to introduce and use dense formal value collections.
	Not critical since they are not replicated much, 
	may help performance in lookup a bit, save a little memory
	from sparse structures.  
	Goal: 1
	Status: not begun
	Priority: low, not memory critical
 */
#define	DENSE_FORMAL_VALUE_COLLECTIONS		0

/**
	Define to 1 to have footprint own its instantiated template
	parameters rather than keep a separate key in the footprint manager.
	Rationale: make it easier to associate each footprint with
	canonical complete type.
	Goal: 1
	Status: complete, tested, perm'd
#define	FOOTPRINT_HAS_PARAMS			1
 */

/**
	Define to 1 to have every footprint include a back-reference
	to its owner.
	Q: if a footprint has no members, do we ever need to worry
		about back-references?  We might want to access
		the original definition, even if it is empty.  
	Goal: 1
	Status: complete, tested, perm'd
#define	FOOTPRINT_OWNER_DEF			1
 */

/**
	Define to 1 for final scalability rework, where each footprint
	keeps a hierarchically mapped state-manager for all
	subinstances.
	Rationale: to eliminate expensive global allocation
	Goal: 1
	Phase 1: sifting of locally allocated indices public:private [done]
	Phase 2: replace all use of global state_manager [done]
	Phase 3: emulating global allocation [in-progress]
	Phase 4: various back-ends
	Status: complete, branch merged to trunk
	Perm'd.
 */
// #define	MEMORY_MAPPED_GLOBAL_ALLOCATION		1

/**
	Define to 1 to allow the new hierarchical allocation scheme
	to handle passing processes through ports, which adds some
	complexity to the local pool mapping and partitioning.  
	Before MEMORY_MAPPED_GLOBAL_ALLOCATION, this was always allowed
	and handled correctly by the global allocation process.
	Goal: 1
	Status: 1
 */
// #define	ALLOW_PROCESS_PORTS	(1 && MEMORY_MAPPED_GLOBAL_ALLOCATION)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to remove super_instance pointers from 
	instance_collection_base, and push them down to 
	port_actual_collections only.  
	Rationale: to save some memory on structures that don't need it.  
	Goal: 1?
	Status: not begun
	Priority: low, slightly memory critical
 */
#define	SUPER_INSTANCES_IN_ACTUALS_ONLY		0

/**
	Define to 1 to pool-allocate collection pool bundles.
	Goal: 1
	Status: not begun
	Priority: low, non-critical enhancement
 */
#define	POOL_ALLOCATE_COLLECTION_POOL_BUNDLES		0

/**
	Define to 1 to pool-allocate footprints. 
	Prerequisite: Depends on footprints being heap-allocated, of course
	Rationale: faster heap allocation, of course.  
	Goal: 1
	Status: not begun
	Priority: low (enhancement, non-critical)
 */
#define	POOL_ALLOCATE_FOOTPRINTS		0

/**
	Define to 1 to use footprint sub-types, distinguishing between
	process, datastruct, and channel footprints.  
	Currently footprint is generic union of needed features, bloated.  
	Rationale: memory reduction and stronger static typing.  
	Goal: ?
	Status: there is information from footprints pointing back to
	their original definitions, and hence meta-type.
	Priority: low
 */
#define	SUBTYPE_FOOTPRINTS			0

/**
	Define to 1 to instantiate struct collections and references, 
	which as of the time of writing this, have never been well-defined
	or used.  
	Goal: ?
	Pending: well-defined semantics in language
 */
#define	ENABLE_DATASTRUCTS			0

/**
	Define to 1 to re-define global_entry<channel> using
	canonical_fundamental_channel footprints.  
	Also allocate ChannelData accordingly.  
	Goal: 1
	Status: done
	Priority: high (for chpsim)
 */
#define	BUILTIN_CHANNEL_FOOTPRINTS		1

/**
	Define 1 to support shared channels in connection checking.
	Rationale: to allow processes with mutually exclusive 
	uses of channels to connect to the same channel.  
	Defect: how do we annotate bidirectional shared?  Omit for now.
	Goal: 1
	Status: done, tested, perm later
	Priority: med-high
 */
#define ENABLE_SHARED_CHANNELS			1

/**
	Define to 1 to generalize CHP attributes in 
	intermediate representation.
	Rationale: currently CHP actions have one hard-coded delay attribute.
	Goal: 1
	Priority: low (not needed before graduation)
 */
#define	CHP_GENERAL_ATTRIBUTES			0

/**
	Define to 1 to make use of a forgiving type.  
	Rationale: With the advent of run-time bound functions, we can no longer
	make static type guarantees about functions, so we need to 
	accommdate late type bindings with the help of a forgiving
	type lattice element, the match-all-data-type.  
	For type checking purposes, this matches all.  
	Goal: 1?
	Status: done, tested
	Priority: high -- for chpsim function support
 */
#define	USE_TOP_DATA_TYPE			1

// Perm'd
// #define	CFLAT_WITH_CONDUCTANCES			1

/**
	Define to 1 to derive process_definition from namespace, 
	not just scopespace.
	Rationale: simplify interfaces
	Goal: 1
	Semantically, open-spaces and open-aliases are allowed,
		but syntactically not supported.
 */
#define	PROCESS_DEFINITION_IS_NAMESPACE		1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// features targeted for branch HACKT-00-01-04-main-00-83-29-supply-01
/**
	Define to 1 to have every process definition implicitly
	declare local !Vdd and !GND ports.  
	This will require some special handling in port_formals_manager
	so the implicit ports are hidden from the list substructure, 
	and only appear in the map.  
	Goal: 1
	Status: done, tested, perm'd
#define	IMPLICIT_SUPPLY_PORTS			1
 */

/**
	Define to 1 to allow prs bodies to override Vdd and GND.
	Note: should allow declared internal nodes, not just regular nodes.
	Goal: 1
	Status: done, tested, ready-to-perm
 */
#define	PRS_SUPPLY_OVERRIDES			1

/**
	Define to 1 to support substrate contact overriding.
	Status: begun
 */
#define	PRS_SUBSTRATE_OVERRIDES			(1 && PRS_SUPPLY_OVERRIDES)

/**
	Define to 1 enable means of overriding the implicit primary supplies
	per-instance.  
	Will require syntax extension.  
	Goal: 1
	Status: done, tested, ready-to-perm
 */
#define	INSTANCE_SUPPLY_OVERRIDES		1

/**
	Define to 1 to allow implicit supplies to be unconnected by
	passing empty arguments.  
	Goal: probably do not want (0)
	Status: experimental
 */
#define	INSTANCE_SUPPLY_DISCONNECT		(0 && INSTANCE_SUPPLY_OVERRIDES)

/**
	Define to 1 to allow the implicit ports to be declared 
	!GND before !Vdd, but override connections to use the ordering
	consistent with prs-overrides: !Vdd, !GND.
	Goal: 1
	Rationale: this lets netlists produce GND-first convention subcircuits
		while allowing the common case of overriding only !Vdd
		to be consistent between prs and instance overrides.
	Status: tested
 */
#define	REVERSE_INSTANCE_SUPPLY_OVERRIDES	(1 && INSTANCE_SUPPLY_OVERRIDES)

/**
	Define to 1 to enable nonmeta-member instance/value references.
	Goal: 1
	Rationale: without this, requires using aliases to work-around
 */
#define	NONMETA_MEMBER_REFERENCES		0

/**
	Define to 1 to lookup more than one parent at a time
	for aggregate references like a[x..y].b, *without* having
	to flatten or expand into a large number of individual references.
	Goal: 1
	Rationale: more efficient lookups
	Status: tested, massive speedup on quarantine tests, stable
#define	AGGREGATE_PARENT_REFS			1
 */

/**
	Define to 1 if each footprint contains its own context cache.
	For non-top footprints, this only benefits references to 
	private subinstance references.
	This should reduce the amount of passing around of structs.  
	Goal: 1
	Rationale: caching for speedup of reference lookups.
	Status: tested, stable, perm'd
#define	FOOTPRINT_OWNS_CONTEXT_CACHE		1
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to construct one canonical process event subgraph per
	complete type (with CHP) to make allocation easier, and faster.
	Also makes events per process contiguous in allocation, making
	reverse-mapping much easier.  
	Status: Completed and perm'd: main-00-81-68-chpsim-09 branch
#define	LOCAL_CHP_EVENT_FOOTPRINT		1
**/

// Idea: regenerate CHP event footprint upon load as an alternative to
// storing in object file?  Perhaps leave as command-line option.  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// these flags affect actions in parser/hackt-parse.yy.in
/**
	Define to 1 to treat user-defined channels like processes, 
	in that they have subinstances, can be instantiated with their
	members, and are processed through cflat.  
	Type-checking? instances? built-in channel types?
	Connection between built-in channel types and user-defined
	will NOT work yet.
	THIS COULD BE HIGHLY INVASIVE.
	Rationale: so that certain unnamed parties can use defchans
		like processes in the f'd up shadow toolchain...
	Goal: ?
	Priority: damn it
	Resolution: try to hack the parser -- success!
		Parser treats defchan decls like process, rest of middle-end
		and type-checking is unchanged.  
	Status: done, tested minimally
	Afterthought: this feels really really dirty...
	Feedback: Having a unified footprint greatly simplifies the
		middle-end and back-end tools.  
 */
#define	DEFCHAN_LIKE_PROCESS			1

/**
	Same idea as above.  
	This still feels dirty.
 */
#define	DEFTYPE_LIKE_PROCESS			1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to enable bool (node, wire)
	directionality flags and perform static connectivity checking on them.
	Goal: 1
	Priority: high
	Rationale: many back-end tools need this.
	Status: works stably for a while, perm'd
#define	BOOL_CONNECTIVITY_CHECKING		1
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to enable process (user-defined channel/datatype)
	directionality flags and perform static connectivity checking on them.
	Goal: 1
	Priority: high
	Rationale: many back-end tools need this.
	Status: done, test cases added for user-defined channels/datatypes
		perm'd
#define	PROCESS_CONNECTIVITY_CHECKING		1
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to enable implementation of private member references.
	Rationale: some spec directives wish to reference non-public
		members of processes without modifying them.
	Goal: 1
	Status: done, somewhat tested
 */
#define	PRIVATE_MEMBER_REFERENCES		1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ACT compatbility switches
/**
	Define to 1 to require definitions to be exported to 
	be usable outside their home namespace.  
	Note: does not apply to typedefs.  
	Goal: 1
	Rationale: ACT-compatibility
	Status: completed, tested
 */
#define	REQUIRE_DEFINITION_EXPORT		1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// the below flags are done, revisit and perm them later

/**
	Define to 1 recurse collection of port aliases (redundantly redundant)
	Discovered that using instance_collection_pool_manager to collect
	instance aliases results in massive redundant repetition in
	footprint's scope_aliases (which is never dumper, incidentally).
	Disabling this behavior should result in smaller footprints.  
	Goal: ?
	Status: 1 (original)
	Resolution: we keep recursion for now, and revisit this later.  
 */
#define	RECURSE_COLLECT_ALIASES			1

/**
	Simplify implementation of collecting port aliases
	by collecting over scope-aliases in a separate pass.  
	Status: perm'd
#define	COPY_IF_PORT_ALIASES			1
**/

/**
	Define to 1 to not serialize scope-alias summaries, but rather
	regenerate/recache them automatically and on-demand.  
	With this on, they are reconstructed upon footprint::load.  
	This cut the size of the object binaries, by reconstructing
		redundant (precisely regenerable) information.  
	Goal: 1? (probably)
	Priority: low-medium (memory intensive)
	Status: done, tested, but not perm'd yet.  
		Results in massive (factor of 2) savings on 
		large objects, especially top-level hierarchy.
#define	AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES		1
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to unroll port structures once (owned per footprint)
	and have subsequent instances just perform a deep-copy
	on that structure, followed by replaying internal aliases).
	Note: the pointer being deep-copied is owned by a footprint's
	instance_collection_pool_bundle, so a target owner needs
	to be passed through a context.
	Note that the 'master' copies port_actual_collections
	must be owned in-tree, so subinstance_manager (non-owning)
	doesn't suffice; we need a self-owning class variant 
	of subinstance_manager.  Call it subinstance_template.
	Problem: aliases in collections are derived from
	*non-owning* subinstance_managers.  
	Perhaps instead of owning, we just use a thin layer of indirection
	to point to already owned (local) collections.
	A shallow-copy to top-level ports -- back to subinstance_manager.
	Status: works, perm'd
#define CACHE_SUBSTRUCTURES_IN_FOOTPRINT		1
 */

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_HH__

