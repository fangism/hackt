/**
	\file "util/tree_cache.tcc"
	$Id: tree_cache.tcc,v 1.1.2.2 2010/03/17 02:11:47 fang Exp $
 */

#ifndef	__UTIL_TREE_CACHE_TCC__
#define	__UTIL_TREE_CACHE_TCC__

#include <iostream>
#include "util/tree_cache.h"
#include "util/macros.h"
#include "util/indent.h"
#include "util/stacktrace.h"
#include "util/value_saver.h"

namespace util {
#include "util/using_ostream.h"
using util::auto_indent;
using util::value_saver;

//-----------------------------------------------------------------------------
// class tree_cache method definitions
enum { INITIAL_TREE_CACHE_WEIGHT = 1 };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TREE_CACHE_TEMPLATE_SIGNATURE
TREE_CACHE_CLASS::tree_cache(const key_type& k) :
		parent(NULL), _key(k), children_cache(), 
		local_weight(INITIAL_TREE_CACHE_WEIGHT),
		children_weight(0), value() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TREE_CACHE_TEMPLATE_SIGNATURE
TREE_CACHE_CLASS::tree_cache(const key_type& k, const value_type& v) :
		parent(NULL), _key(k), children_cache(), 
		local_weight(INITIAL_TREE_CACHE_WEIGHT),
		children_weight(0), value(v) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only copy the key.
	Do NOT copy the parent pointer.
 */
TREE_CACHE_TEMPLATE_SIGNATURE
TREE_CACHE_CLASS::tree_cache(const this_type& t) :
		parent(NULL), _key(t._key), children_cache(), 
		local_weight(INITIAL_TREE_CACHE_WEIGHT),
		children_weight(0), value() {
	INVARIANT(!t.parent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Upon destruction, if any weight remained, notify parents of loss.  
 */
TREE_CACHE_TEMPLATE_SIGNATURE
TREE_CACHE_CLASS::~tree_cache() {
//	STACKTRACE_VERBOSE;
	STACKTRACE("tree_cache::~tree_cache()");
	const weight_type loss = this->weight();
{
	// temporarily disconnect parent pointer
	// aggregate weight loss without propagating all the way
	// to super parents
	const value_saver<this_type*> tmp(this->parent, NULL);
	STACKTRACE_INDENT_PRINT("children: " << children_cache.size() <<
		", total_weight = " << children_weight << endl);
	children_cache.clear();	// will zero-out children_weight
	STACKTRACE_INDENT_PRINT("end-weight = " << children_weight << endl);
	INVARIANT(!children_weight);
}
if (loss) {
	// at the end, propagate cumulative loss up the parent chain
	this_type* p = parent;	// subtract up the parent chain
	while (p) {
		STACKTRACE_INDENT_PRINT("reduce parent by " << loss << endl);
		p->children_weight -= loss;
		p = p->parent;
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Upon connecting to parent, increase parents' weights.  
	Treated as const because key is not modified.
 */
TREE_CACHE_TEMPLATE_SIGNATURE
void
TREE_CACHE_CLASS::connect_parent(this_type& r) {
	STACKTRACE("tree_cache::connect_parent()");
	INVARIANT(!parent);
	parent = &r;
	const weight_type gain = this->weight();
if (gain) {
	this_type* p = parent;	// subtract up the parent chain
	do {
		STACKTRACE_INDENT_PRINT("increase parent by " << gain << endl);
		p->children_weight += gain;
		p = p->parent;
	} while (p);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Count cache-hit: increments weight up the chain of parents.  
 */
TREE_CACHE_TEMPLATE_SIGNATURE
void
TREE_CACHE_CLASS::count_hit(void) {
	++this->local_weight;
	this_type* p = this->parent;	// subtract up the parent chain
	while (p) {
		STACKTRACE_INDENT_PRINT("increment parent" << endl);
		++p->children_weight;
		p = p->parent;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Performs lookup-insertion, must like std::map::operator[].
	Returns an existing cached child entry or creates a new one.
	\return iterator-bool pair.  Bool is true if cache missed, 
	and a new entry was created, false if cache hit and previous entry
	was found.  Is up to caller to initialize value in the event of
	a cache-miss.
 */
TREE_CACHE_TEMPLATE_SIGNATURE
std::pair<typename TREE_CACHE_CLASS::child_iterator, bool>
TREE_CACHE_CLASS::insert_find(const key_type& k) {
//	STACKTRACE_VERBOSE;
	STACKTRACE("tree_cache::insert_find()");
	const std::pair<child_iterator, bool>
		ret(children_cache.insert(this_type(k)));
	if (ret.second) {
		// then this was a new entry
		STACKTRACE_INDENT_PRINT("cache miss @" << k <<
			", new entry" << endl);
		const_cast<this_type&>(*ret.first).connect_parent(*this);
	} else {
		// this hit an existing entry, bump its counter
		STACKTRACE_INDENT_PRINT("cache hit @" << k << endl);
		const_cast<this_type&>(*ret.first).count_hit();
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively reduces the weights of nodes in the cache.
	Nodes that reach weight 0 are expired and evicted from the cache.
	\param shave optional value to subtract after halving weights.
	\return the aggregate weight lost including self and children.
	This method is not truly const, in that it can result in
	loss of cached entries, but such modification is const-enough
	w.r.t. set membership of sorted elements.  
 */
TREE_CACHE_TEMPLATE_SIGNATURE
typename TREE_CACHE_CLASS::weight_type
TREE_CACHE_CLASS::halve(const weight_type shave) const {
	STACKTRACE_VERBOSE;
	// bottom-up, children first
	child_iterator i(children_cache.begin()), e(children_cache.end());
	weight_type child_diff = 0;
	for ( ; i!=e; ) {
		const weight_type cw = i->weight();
		const weight_type diff = i->halve(shave);
		child_diff += diff;
		INVARIANT(cw >= diff);
		if (cw == diff) {
			child_iterator j(i);
			++i;
			STACKTRACE_INDENT_PRINT("evicting sub-cache "
				<< j->key() << endl);
			children_cache.erase(j);	// evict
		} else {
			++i;
		}
	}
	INVARIANT(children_weight >= child_diff);
	children_weight -= child_diff;
	const weight_type old_local_weight = this->local_weight;
	local_weight >>= 1;	// halve
	if (local_weight > shave) {
		local_weight -= shave;
	} else {
		local_weight = 0;
	}
	const weight_type ldiff = old_local_weight -local_weight;
	const weight_type ret = ldiff +child_diff;
	STACKTRACE_INDENT_PRINT("weight lost: " << ret << endl);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return Count total number of entries in tree cache.
 */
TREE_CACHE_TEMPLATE_SIGNATURE
size_t
TREE_CACHE_CLASS::size(void) const {
	size_t ret = 1;		// self
	const_child_iterator i(children_cache.begin()), e(children_cache.end());
	for ( ; i!=e; ++i) {
		ret += i->size();
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively print contents of cache, revealing weights.
 */
TREE_CACHE_TEMPLATE_SIGNATURE
ostream&
TREE_CACHE_CLASS::dump(ostream& o,
		void (*value_dumper)(ostream&, const value_type&)) const {
	o << auto_indent << _key << ": (" << this->local_weight << "+"
		<< this->children_weight << ") ";
	NEVER_NULL(value_dumper);
	(*value_dumper)(o, this->value);
	o << auto_indent << "{" << endl;
	// now iterate over children
{
	INDENT_SECTION(o);
	const_child_iterator i(children_cache.begin()), e(children_cache.end());
	for ( ; i!=e; ++i) {
		i->dump(o, value_dumper) << endl;
	}
}
	o << auto_indent << "}";
	return o;
}

//-----------------------------------------------------------------------------
}	// end namespace util

#endif	// __UTIL_TREE_CACHE_TCC__

