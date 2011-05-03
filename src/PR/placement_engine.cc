/**
	\file "PR/placement_engine.cc"
	$Id: placement_engine.cc,v 1.1.2.21 2011/05/03 01:19:29 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include "PR/placement_engine.h"
#include "PR/tile_type.h"
#include "PR/pr_utils.h"
#include "common/TODO.h"
#include "util/vector_ops.h"
#include "util/array.tcc"
#include "util/indent.h"
#include "util/string.h"
#include "util/numeric/random.h"
#include "util/numeric/abs.h"
#include "util/IO_utils.tcc"
#include "util/iterator_more.h"
#include "util/iomanip.h"
#include "util/value_saver.h"
#include "util/stacktrace.h"

namespace PR {
using std::for_each;
using std::transform;
using std::lower_bound;
using std::upper_bound;
using std::sort;
using std::distance;
using util::auto_indent;
using util::save_precision;
using util::strings::string_to_num;
using util::numeric::rand48;
using util::write_value;
using util::read_value;
using util::value_saver;
using namespace util::vector_ops;		// for many operator overloads
using util::numeric::abs;
#include "util/using_ostream.h"

//=============================================================================
// class placement_engine method definitions

placement_engine::placement_engine(const size_t d) :
		state_base(), 
		object_types(),
		channel_types(),
		opt(),
		space(d),
#if ENABLE_GRAVITY_WELLS
		x_wells(), 
		y_wells(), 
		z_wells(),
#endif
		proximity_cache(),
		elapsed_time(0.0), 
		proximity_potential_energy(0.0),
		gravity_potential_energy(0.0),
		autosave_name(),
		need_force_recalc(true) {
	initialize_default_types();
	proximity_cache.reserve(64);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
placement_engine::~placement_engine() {
if (autosave_name.size()) {
	std::ofstream o(autosave_name.c_str());
	if (o) {
	try {
		save_checkpoint(o);
	} catch (...) {
		cerr << "Fatal error during checkpoint save." << endl;
	}
	} else {
		cerr << "Error opening \'" << autosave_name <<
			"\' for saving checkpoint." << endl;
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::initialize_default_types(void) {
	object_types.clear();
	object_types.reserve(64);
	object_types.push_back(tile_type());
	channel_types.clear();
	channel_types.reserve(64);
	channel_types.push_back(channel_type());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::auto_proximity_radius(void) {
	opt.proximity_radius = space.auto_proximity_radius();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::add_object_type(const tile_type& t) {
	object_types.push_back(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::add_channel_type(const channel_type& t) {
	channel_types.push_back(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::add_object(const tile_instance& o) {
	space.objects.push_back(o);
	space.current.push_back(object_state());
	// automatically update proximity radius
	const real_type rad =
		space.objects.back().properties.maximum_dimension();
	if (rad > opt.proximity_radius) {
		opt.proximity_radius = rad;
	}
	need_force_recalc = true;
}

#define	CHECK_OBJECT_INDEX(i)						\
	if (i >= space.objects.size()) {				\
		cerr << "Error: object index out-of-bounds." << endl;	\
		return true;						\
	}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check that object indices are in-bounds.
	\return true on error
 */
bool
placement_engine::add_channel(const channel_instance& c) {
	CHECK_OBJECT_INDEX(c.source)
	CHECK_OBJECT_INDEX(c.destination)
	const tile_instance& s(space.objects[c.source]);
	const tile_instance& d(space.objects[c.destination]);
	// uses spherical approximation of objects for now,
	// could be ellipsoid vector...
	space.springs.push_back(c);
	space.springs.back().properties.equilibrium_distance =
		(s.properties.maximum_dimension() +
			d.properties.maximum_dimension()) * 0.5;
	need_force_recalc = true;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::pin_object(const size_t i) {
	CHECK_OBJECT_INDEX(i)
	space.objects[i].fix();
	need_force_recalc = true;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::unpin_object(const size_t i) {
	CHECK_OBJECT_INDEX(i)
	space.objects[i].unfix();
	need_force_recalc = true;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::place_object(const size_t i, const real_vector& v) {
	CHECK_OBJECT_INDEX(i)
	real_vector p(v);
	opt.clamp_position(p);
	space.current[i].place(p);
	need_force_recalc = true;
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_parameters(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "parameters:\n";
	INDENT_SECTION(o);
	o << auto_indent << "@time=" << elapsed_time << endl;
	opt.dump_parameters(o);
#if 0
	o << auto_indent << "max_delta_position=" << max_delta_position << endl;
	o << auto_indent << "max_delta_velocity=" << max_delta_velocity << endl;
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Place all free objects somewhere randomly within the bounding box.
 */
void
placement_engine::scatter(void) {
	STACKTRACE_VERBOSE;
	const real_vector box_size(opt.upper_corner -opt.lower_corner);
	typedef	rand48<double>			random_generator;
	const random_generator g;
	vector<tile_instance>::const_iterator
		i(space.objects.begin()), e(space.objects.end());
	vector<object_state>::iterator
		j(space.current.begin());
	for ( ; i!=e; ++i, ++j) {
	if (!i->is_fixed()) {
		real_vector r;
		// fixed for 3D
		// uniformly random values in each dimension
		r[0] = g();
		r[1] = g();
		r[2] = g();
		r *= box_size;
		r += opt.lower_corner;
		j->place(r);
	}
	}
	need_force_recalc = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::zero_forces(void) {
	for_each(space.current.begin(), space.current.end(), 
		std::mem_fun_ref(&object_state::zero_force));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	vector<object_state>::const_iterator		position_iterator;

template <size_t D>
static
bool
dim_less(const position_iterator l, const position_iterator r) {
	return l->position[D] < r->position[D];
}

template <size_t D>
static
bool
dim_comp(const position_iterator l, const real_type& r) {
	return l->position[D] < r;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This could go to util lib.
	\param T is an iterator type, preferably random_access.
 */
template <class T>
struct array_offset {
	typedef	T		iterator_type;
	const iterator_type		begin;

	explicit
	array_offset(const T i) : begin(i) { }

	size_t
	operator () (const T i) const {
		return distance(begin, i);
	}

};	// end struct array_offset

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really want to avoid this.
 */
void
placement_engine::clear_proximity_cache(void) {
	proximity_cache.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Populate array of iterators (pointers) to objects.
	Initially they are sorted by index.  
 */
void
placement_engine::create_object_iterator_array(object_iterator_array& v) const {
	v.clear();
	v.reserve(space.objects.size());
	object_iterator i(space.current.begin()), e(space.current.end());
	for ( ; i!=e; ++i) {
		v.push_back(i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Update dynamic graph of near-neighbors.  
	Use a sliding-window cube in each dimension.
	TODO: maintain order as object positions are updated
		and only *incrementally* sort.
	Or do sector binning, near constant time!
 */
void
placement_engine::refresh_proximity_cache(void) {
	STACKTRACE_VERBOSE;
	clear_proximity_cache();		// wipe before recomputing
	// sort by each dimension/index
	typedef	vector<object_state>::const_iterator	iterator;
	vector<iterator> obj_x;
	create_object_iterator_array(obj_x);
//	cout << "SPACE size = " << space.objects.size() << endl;

	// perform in-place sort on each
	// first, sorted by x-coordinates only
	sort(obj_x.begin(), obj_x.end(), &dim_less<0>);
	// use proximity_radius to find sets of objects in the same window
	vector<iterator>::iterator xb(obj_x.begin()), xe(obj_x.end());
	// Q: is binary search worth it? linear-incremental may suffice
	// A: linear! b/c overall cost O(N) vs. O(N lg N)
	vector<iterator>::iterator xu(xb);
	const array_offset<iterator> vo(space.current.begin());
	const size_t i1 = vo(*xb);
	const tile_instance& o1(space.objects[i1]);
#if 0
	std::set<size_t> window_set_x;	// a queue is good enough?
	transform(xb, xu, util::set_inserter(window_set_x), vo);
 	INVARIANT(window_set_x.size() >= 1);
#endif
	// x-sweep
for ( ; xb!=xe; ++xb) {
	const real_type x = (*xb)->position[0];
	// [xb, xu] defines a sliding window along the x-dimension
	// linear scan overall costs less than repeated (lg N) binary searches
	while (xu!=xe && (*xu)->position[0] < x+opt.proximity_radius) {
			// ->current.position
		++xu;
	}
const size_t xw_size = distance(xb, xu);
if (xw_size > 1) {
	STACKTRACE_INDENT_PRINT("have " << xw_size <<
		" x-collision candidates." << endl);
	// recurse in y-dimension, z-dimensions
	// reduce size of window_set in each dimension
	// copy range of iterators, and re-sort by y-dimension
	vector<iterator> obj_y(xb, xu);
	sort(obj_y.begin(), obj_y.end(), &dim_less<1>);
	const real_type& y_ref((*xb)->position[1]);
	const vector<iterator>::iterator
		yb(obj_y.begin()), ye(obj_y.end());
	const vector<iterator>::iterator
		yl(lower_bound(yb, ye, y_ref-opt.proximity_radius, &dim_comp<1>)),
		yu(lower_bound(yl, ye, y_ref+opt.proximity_radius, &dim_comp<1>));
	const size_t yw_size = distance(yl, yu);
	if (yw_size > 1) {
		STACKTRACE_INDENT_PRINT("have " << yw_size <<
			" xy-collision candidates." << endl);
		// copy range of iterators, re-sort by z-dimension
		vector<iterator> obj_z(yl, yu);
		sort(obj_z.begin(), obj_z.end(), &dim_less<2>);
		const real_type& z_ref((*xb)->position[2]);
		const vector<iterator>::iterator
			zb(obj_y.begin()), ze(obj_y.end());
		const vector<iterator>::iterator
			zl(lower_bound(zb, ze, z_ref-opt.proximity_radius,
				&dim_comp<2>)),
			zu(lower_bound(zl, ze, z_ref+opt.proximity_radius,
				&dim_comp<2>));
		const size_t zw_size = distance(zl, zu);
		if (zw_size > 1) {
			STACKTRACE_INDENT_PRINT("have " << zw_size <<
				" xyz-collision candidates." << endl);
			// then we are within proximity
			vector<iterator>::iterator zi(zl);
			for ( ; zi!=zu; ++zi) {
			if (*zi != *xb) {
				const size_t i2 = vo(*zi);
				const tile_instance& o2(space.objects[i2]);
				STACKTRACE_INDENT_PRINT(
					"caching collision candidate pair ("
					<< i1 << ',' << i2 << ")." << endl);
				INVARIANT(i1 != i2);
				// avoid double counting with index ordering
				if (i1 < i2) {
					proximity_cache.push_back(
						proximity_edge(i1, i2));
				} else {
					proximity_cache.push_back(
						proximity_edge(i2, i1));
				}
				channel_instance& pe(proximity_cache.back());
				pe.properties.spring_coeff =
					opt.repulsion_coeff;
				pe.properties.equilibrium_distance = 
					(o2.properties.maximum_dimension()
					+o1.properties.maximum_dimension())
					*0.5;
			}
			}	// end for xyz-slice candidates
		}	// end if z-window
	}	// end if y-window
}	// else no collision candidates
}	// end for each x-ordered object
}	// end placement_engine::refresh_proximity_cache

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compute repulsive forces between near-neighbors.  
 */
void
placement_engine::compute_collision_forces(void) {
	STACKTRACE_VERBOSE;
	proximity_potential_energy = 0.0;
	typedef	vector<tile_instance>::iterator		iterator;
	iterator i(space.objects.begin()), e(space.objects.end());
	const array_offset<iterator> vo(i);
	vector<proximity_edge>::iterator
		pi(proximity_cache.begin()), pe(proximity_cache.end());
	for ( ; pi!=pe; ++pi) {
		const size_t& j1(pi->source);
		const size_t& j2(pi->destination);
			const tile_instance& o1(space.objects[j1]);
			object_state& s1(space.current[j1]);
			// avoid double counting
			INVARIANT(j1 < j2);
			STACKTRACE_INDENT_PRINT("repelling objects " <<
				j1 << " and " << j2 << endl);
			const tile_instance& o2(space.objects[j2]);
			object_state& s2(space.current[j2]);
			pi->potential_energy =
				tile_instance::apply_repulsion_forces(
					o1, o2, pi->properties, 
					opt.repulsion_constant, s1, s2);
			proximity_potential_energy += pi->potential_energy;
	}	// end for each proximity_edge
	proximity_potential_energy *= 0.5;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const real_type&
placement_engine::update_proximity_potential_energy(void) {
	STACKTRACE_VERBOSE;
	proximity_potential_energy = 0.0;
	typedef	vector<tile_instance>::iterator		iterator;
	iterator i(space.objects.begin()), e(space.objects.end());
	const array_offset<iterator> vo(i);
	vector<proximity_edge>::iterator
		pi(proximity_cache.begin()), pe(proximity_cache.end());
	for ( ; pi!=pe; ++pi) {
		const size_t& j1(pi->source);
		const size_t& j2(pi->destination);
		// avoid double counting
		INVARIANT(j1 < j2);
		const tile_instance& o1(space.objects[j1]);
		const tile_instance& o2(space.objects[j2]);
		const object_state& s1(space.current[j1]);
		const object_state& s2(space.current[j2]);
		STACKTRACE_INDENT_PRINT("repelling objects " <<
			j1 << " and " << j2 << endl);
		pi->potential_energy =
			tile_instance::current_repulsion_potential_energy(
				o1, o2, pi->properties, 
				opt.repulsion_constant, s1, s2);
		proximity_potential_energy += pi->potential_energy;
	}	// end for each proximity_edge
	proximity_potential_energy *= 0.5;
	return proximity_potential_energy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Aligns object to nearest well in each hyperplane.
	\param obj pre-loaded array iterators to all objects
	\param N dimension to sort (key) and attract
	\return potential energy
 */
template <size_t N>
void
placement_engine::__snap_to_gravity_wells(const gravity_map_type& wm,
		object_iterator_array& obj) {
	STACKTRACE_VERBOSE;
	// perform in-place sort on each
	sort(obj.begin(), obj.end(), &dim_less<N>);
	typedef	object_iterator_array::iterator		iter_iter;
	iter_iter xb(obj.begin()), xe(obj.end());
	const array_offset<object_iterator> vo(space.current.begin());
	// apply x-forces
	typedef	gravity_map_type::const_iterator	g_iterator;
	g_iterator wi(wm.begin()), we(wm.end());
	typedef	std::numeric_limits<real_type>		limits_type;
	// compute gravity region bounds on the fly
	real_type lb = -limits_type::max();
	xb = lower_bound(xb, xe, lb, &dim_comp<N>);	// should just be xb
	for (; wi!=we; ++wi) {
		const real_type& node(*wi);		// node of attraction
		g_iterator wj(wi);
		++wj;
		const real_type ub((wj!=we) ? (*wj +*wi) *0.5 : limits_type::max());
		// attract to node at wi->second
		STACKTRACE_INDENT_PRINT(N << "-well region: (" << lb <<
			", " << ub << ") -> " << node << endl);
		// TODO: use linear instead of binary search?
		const iter_iter xu(lower_bound(xb, xe, ub, &dim_comp<N>));
		STACKTRACE_INDENT_PRINT("contains " << distance(xb, xu) <<
			" objects" << endl);
		for ( ; xb!=xu; ++xb) {
			const size_t oi(vo(*xb));
			const tile_instance& t(space.objects[oi]);
			if (!t.is_fixed()) {
				object_state& o(space.current[oi]);
				o.position[N] = node;
			}
		}
		INVARIANT(xb == xu);
		lb = ub;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param N dimension to sort (key) and attract
	\param obj pre-loaded array iterators to all objects
	\param g1 spring strength of gravity (like spring coeff).
	\param g0 constant force
	\return potential energy
 */
template <size_t N>
real_type
placement_engine::__compute_gravity_forces(const gravity_map_type& wm,
		const real_type& g1, const real_type& g0, 
		object_iterator_array& obj) {
	real_type grav_energy = 0.0;
if (g1 > 0.0 || g0 > 0.0) {
	STACKTRACE_VERBOSE;
	// perform in-place sort on each
	sort(obj.begin(), obj.end(), &dim_less<N>);
	typedef	object_iterator_array::iterator		iter_iter;
	iter_iter xb(obj.begin()), xe(obj.end());
	const array_offset<object_iterator> vo(space.current.begin());
	// apply x-forces
	typedef	gravity_map_type::const_iterator	g_iterator;
	g_iterator wi(wm.begin()), we(wm.end());
	typedef	std::numeric_limits<real_type>		limits_type;
	// compute gravity region bounds on the fly
	real_type lb = -limits_type::max();
	xb = lower_bound(xb, xe, lb, &dim_comp<N>);	// should just be xb
	for (; wi!=we; ++wi) {
		const real_type& node(*wi);		// node of attraction
		g_iterator wj(wi);
		++wj;
		const real_type ub((wj!=we) ? (*wj +*wi) *0.5 : limits_type::max());
		// attract to node at wi->second
		STACKTRACE_INDENT_PRINT(N << "-well region: (" << lb <<
			", " << ub << ") -> " << node << endl);
		// TODO: use linear instead of binary search?
		const iter_iter xu(lower_bound(xb, xe, ub, &dim_comp<N>));
		STACKTRACE_INDENT_PRINT("contains " << distance(xb, xu) <<
			" objects" << endl);
		for ( ; xb!=xu; ++xb) {
			const size_t oi(vo(*xb));
			const tile_instance& t(space.objects[oi]);
			if (!t.is_fixed()) {
				object_state& o(space.current[oi]);
				grav_energy +=
				tile_instance::attract_to_dimension_well<N>(
					t, node, g1, g0, o);
			}
		}
		INVARIANT(xb == xu);
		lb = ub;
	}
}
	return grav_energy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param N dimension to sort (key) and attract
	\param g1 spring strength of gravity (like spring coeff).
	\param g0 constant force
	\param obj pre-loaded array iterators to all objects
	\return potential energy
 */
template <size_t N>
real_type
placement_engine::__compute_gravity_energy(const gravity_map_type& wm,
		const real_type& g1, const real_type& g0,
		object_iterator_array& obj) {
	STACKTRACE_VERBOSE;
	real_type grav_energy = 0.0;
if (g1 > 0.0 || g0 > 0.0) {
	// perform in-place sort on each
	sort(obj.begin(), obj.end(), &dim_less<N>);
	typedef	object_iterator_array::iterator		iter_iter;
	iter_iter xb(obj.begin()), xe(obj.end());
	const array_offset<object_iterator> vo(space.current.begin());
	// apply x-forces
	typedef	std::numeric_limits<real_type>		limits_type;
	typedef	gravity_map_type::const_iterator	g_iterator;
	g_iterator wi(wm.begin()), we(wm.end());
	real_type lb = -limits_type::max();
	xb = lower_bound(xb, xe, lb, &dim_comp<N>);	// should just be xb
	for (; wi!=we; ++wi)
	{
		const real_type& node(*wi);		// node of attraction
		g_iterator wj(wi);
		++wj;
		const real_type ub((wj!=we) ? (*wj +*wi) *0.5 : limits_type::max());
		// TODO: use linear instead of binary search?
		const iter_iter xu(lower_bound(xb, xe, ub, &dim_comp<N>));
		for ( ; xb!=xu; ++xb) {
			const size_t oi(vo(*xb));
			const tile_instance& t(space.objects[oi]);
			if (!t.is_fixed()) {
				const object_state& o(space.current[oi]);
				grav_energy +=
				tile_instance::dimension_well_potential_energy<N>(
					t, node, g1, g0, o);
			}
		}
		INVARIANT(xb == xu);
		lb = ub;
	}
}
	return grav_energy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Attracts objects to their nearest gravity node in each dimension.
	Also updates potential energy.
 */
void
placement_engine::compute_gravity_forces(void) {
	STACKTRACE_VERBOSE;
	gravity_potential_energy = 0.0;
#if ENABLE_GRAVITY_WELLS
	const size_t xs = x_wells.size();
	const size_t ys = y_wells.size();
	const size_t zs = z_wells.size();
if (xs +ys +zs) {
	vector<object_iterator> obj;
	create_object_iterator_array(obj);
if (xs) {
	gravity_potential_energy +=
		__compute_gravity_forces<0>(x_wells.nodes,
			opt.x_gravity_coeff, opt.x_gravity_constant, obj);
}
if (ys) {
	gravity_potential_energy +=
		__compute_gravity_forces<1>(y_wells.nodes,
			opt.y_gravity_coeff, opt.y_gravity_constant, obj);
}
if (zs) {
	gravity_potential_energy +=
		__compute_gravity_forces<2>(z_wells.nodes,
			opt.z_gravity_coeff, opt.z_gravity_constant, obj);
}
}	// else no wells!
#endif
	gravity_potential_energy *= 0.5;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Computes energy due to gravity wells.
 */
const real_type&
placement_engine::update_gravity_potential_energy(void) {
	STACKTRACE_VERBOSE;
	gravity_potential_energy = 0.0;
#if ENABLE_GRAVITY_WELLS
	const size_t xs = x_wells.size();
	const size_t ys = y_wells.size();
	const size_t zs = z_wells.size();
if (xs +ys +zs) {
	vector<object_iterator> obj;
	create_object_iterator_array(obj);
if (xs) {
	gravity_potential_energy +=
		__compute_gravity_energy<0>(x_wells.nodes,
			opt.x_gravity_coeff, opt.x_gravity_constant, obj);
}
if (ys) {
	gravity_potential_energy +=
		__compute_gravity_energy<1>(y_wells.nodes,
			opt.y_gravity_coeff, opt.y_gravity_constant, obj);
}
if (zs) {
	gravity_potential_energy +=
		__compute_gravity_energy<2>(z_wells.nodes,
			opt.z_gravity_coeff, opt.z_gravity_constant, obj);
}
}	// else no wells!
	gravity_potential_energy *= 0.5;
#endif
	return gravity_potential_energy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Force objects to the nearest gravity wells.  
 */
void
placement_engine::snap_to_gravity_wells(void) {
#if ENABLE_GRAVITY_WELLS
	const size_t xs = x_wells.size();
	const size_t ys = y_wells.size();
	const size_t zs = z_wells.size();
if (xs +ys +zs) {
	need_force_recalc = true;	// positions update invalidates states
	vector<object_iterator> obj;
	create_object_iterator_array(obj);
if (xs) {
	__snap_to_gravity_wells<0>(x_wells.nodes, obj);
}
if (ys) {
	__snap_to_gravity_wells<1>(y_wells.nodes, obj);
}
if (zs) {
	__snap_to_gravity_wells<2>(z_wells.nodes, obj);
}
	kill_momentum();
}	// else no wells!
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// inline
void
placement_engine::kill_momentum(void) {
	STACKTRACE_VERBOSE;
	space.kill_momentum();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pertubs all objects by a random distance bound by x.
 */
void
placement_engine::shake(const real_type& x) {
	need_force_recalc = true;
	space.shake(x);
	if (opt.watch_objects) {
		dump_positions(cout);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Update acceleration of objects without moving them yet.
	Also computes current potential energy at the same time.  
 */
void
placement_engine::calculate_forces(void) {
	STACKTRACE_VERBOSE;
	zero_forces();		// reset forces
	// attractive forces
	space.compute_spring_forces();
	// compute proximity repulsions on 'close' objects
	refresh_proximity_cache();	// depends on positions only
	compute_collision_forces();
	// TODO: share iterator x-sort results
	compute_gravity_forces();
	need_force_recalc = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Jump starts the simulation by computing the spring forces
	in advance.  This is only needed for the very first time.  
	This also computes potential energy at the same time.
 */
void
placement_engine::bootstrap_forces(void) {
	STACKTRACE_VERBOSE;
	if (need_force_recalc) {
		// very first time, spring forces not pre-computed
		calculate_forces();
		watch_iterate(cout);
	}
}

void
placement_engine::bootstrap_forces_silent(void) {
	STACKTRACE_VERBOSE;
	if (need_force_recalc) {
		// very first time, spring forces not pre-computed
		calculate_forces();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	After updating object position/state, indicate that 
	forces and potential energy needs to be recalculated.
 */
delta_type
placement_engine::update_positions(void) {
	STACKTRACE_VERBOSE;
	const delta_type ret(space.update_objects(opt));
	need_force_recalc = true;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Single iteration of a simulation-based approach.
 */
delta_type
placement_engine::iterate(void) {
	STACKTRACE_VERBOSE;
	bootstrap_forces();	// after first iteration, this becomes no-op
	// after all forces applied, update position
	const delta_type ret(update_positions());
	// TODO: enforce bounds on object positions: clamp_position
#if 0
	watch_iterate(cout);
#else
	// pre-compute for next iteration
	calculate_forces();
#endif
	elapsed_time += opt.time_step;	// increment time
	watch_iterate(cout);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::watch_iterate(ostream& o) const {
	if (opt.watch_anything()) {
		o << endl;
		o << "@time=" << elapsed_time << endl;
	}
	if (opt.watch_objects) {
		dump_positions(o);
	}
	if (opt.watch_deltas) {
#if 0
		o << "delta-position=" << max_delta_position <<
			", delta-velocity=" << max_delta_velocity << endl;
#endif
	}
	if (opt.watch_energy) {
		const save_precision p(o, opt.precision);
		const real_type T = kinetic_energy();
		const real_type V = potential_energy();
		o << "T=" << T << ", V=" << V <<
			", E=" << T+V << ", L=" << T-V << endl;
		// total energy, Lagrangian
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is like iterate except that forces are computed only once
	and then a linear search is performed along the same direction
	for local potential energy minimum.  
	This ignores viscous damping.
 */
real_type
placement_engine::__gradient_slide(void) {
	STACKTRACE_VERBOSE;
	const value_saver<real_type> _vd_(opt.viscous_damping, 0.0);
// start with one iteration
	size_t it_count = 1;
	const time_type start_time = elapsed_time;
	kill_momentum();
//	watch_iterate(cout);
	vector<object_state> best(space.current);		// backup copy
	bootstrap_forces();		// calculate forces and potential energy
	real_type oldV = potential_energy();
	do {
		update_positions();
		// update potential energy *without* re-applying force
		const real_type newV = update_potential_energy();
		elapsed_time += opt.time_step;	// increment time
		if (newV > oldV) {
			// rollback to best state
			best.swap(space.current);
			elapsed_time -= opt.time_step;	// increment time
			break;
		}
		++it_count;
		watch_iterate(cout);
		best = space.current;
		oldV = newV;
	} while (1);

	if (opt.report_iterations) {
		const save_precision p(cout, opt.precision);
		cout << "gradient-slide: ran " << it_count <<
			" iterations from time " << start_time <<
			" to " << elapsed_time <<
			" (V=" << oldV << ')' << endl;
	}
	return oldV;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Maintaining a constant time_step, 
	simulate/iterate until one of the following is true:
	1) change in position/velocity falls below convergence thresholds.
	2) no forward progess has been made for some number of iterations.
	NOTE: this does not always improve/reduce potential energy, 
	it only terminates when deltas are sufficiently small. 
	Uses delta position/velocity to determine convergence.
 */
void
placement_engine::simple_converge(void) {
	STACKTRACE_VERBOSE;
	size_t it_count = 0;
	const time_type start_time = elapsed_time;
//	size_t repcount = 0;
#if 0
	real_type oldV = potential_energy();
	real_type oldT = kinetic_energy();
	real_type oldE = oldV +oldT;
#else
	delta_type diff;
#endif
	do {
		diff = iterate();
		++it_count;
#if 0
		const real_type newV = potential_energy();
		const real_type newT = kinetic_energy();
		const real_type newE = newV +newT;
		// check progress
		// absolute change
		const real_type dV = abs(newV -oldV);
		const real_type dE = abs(newE -oldE);
#if 0
		// relative change (fractional)
		const real_type rdV = dV/oldV;
		const real_type rdE = dE/oldE;
#endif
		oldT = newT;
		oldV = newV;
		oldE = newE;
		const bool reset =
			(dV > opt.energy_tol || dE > opt.energy_tol);
#else
		const bool reset =
			(diff.first > opt.pos_tol || diff.second > opt.vel_tol);
#endif
		// somehow time_step should be accounted for...
#if 0
		if (reset) {
			repcount = 0;
		} else {
			++repcount;
		}
#else
		if (!reset)
			break;
#endif
	} while (1);	// (repcount < opt.min_iterations);
	if (opt.report_iterations) {
		const save_precision p(cout, opt.precision);
		cout << "simple-converge: ran " << it_count <<
			" iterations from time " << start_time <<
			" to " << elapsed_time <<
			" (V=" << potential_energy() << ')' << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Mathematical, but non-physics-based approach.
	Compute instantaneous forces on every object to construct
	direction of best descent (gradient).
	Search along that direction for minimum potential energy.
	Collisions may make the search non-linear.
	Search can be by bissection or golden section.
	Change resolution with option.
 */
void
placement_engine::gradient_search(void) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Iterate while potential energy monotonically decreases.
	Basically stops at the first encountered local minimum.
	Then user may elect to "kill-momentum" and restart descent.
 */
real_type
placement_engine::__descend_potential_energy(void) {
	STACKTRACE_VERBOSE;
	size_t it_count = 1;
	const time_type start_time = elapsed_time;
	kill_momentum();
	vector<object_state> best(space.current);		// backup copy
	bootstrap_forces();	// first iteration: initial potential energy
	real_type oldV = potential_energy();
	do {
		iterate();
		++it_count;
		const real_type newV = potential_energy();
		if (newV >= oldV) {
			// rollback to best state
			best.swap(space.current);
			elapsed_time -= opt.time_step;	// increment time
			break;
		}
		// else is improving
		best = space.current;	// copy over best state (expensive?)
		oldV = newV;
	} while (1);
	if (opt.report_iterations) {
		const save_precision p(cout, opt.precision);
		cout << "descend-potential: ran " << it_count <<
			" iterations from time " << start_time <<
			" to " << elapsed_time <<
			" (V=" << oldV << ')' << endl;
	}
	return oldV;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Repeat until energy no longer changes by threshold.
 */
template <real_type (placement_engine::*MF)(void)>
void
placement_engine::__repeat_until_converge(
		const char* caller, const char* callee) {
	STACKTRACE_VERBOSE;
	size_t it_count = 0;
	const time_type start_time = elapsed_time;
//	pcanvas best(space);		// backup copy
	bootstrap_forces();
	real_type oldV = potential_energy();
	do {
		const real_type newV = (this->*MF)();
		++it_count;
		const real_type dV = abs(newV -oldV);
		oldV = newV;
		if (dV <= opt.energy_tol) {
			break;
		}
//		best = space;
	} while (1);
	// restore the previous best
//	space = best;
	if (opt.report_iterations) {
		const save_precision p(cout, opt.precision);
		cout << caller << ": ran " << it_count <<
			" " << callee << " from time " << start_time <<
			" to " << elapsed_time <<
			" (V=" << oldV << ')' << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::repeat_gradient_slide(void) {
	STACKTRACE_VERBOSE;
	__repeat_until_converge<&this_type::__gradient_slide>(
		"gradient-slide-converge", "gradient-slide");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Repeat until energy no longer changes by threshold.
 */
void
placement_engine::repeat_descend_potential_energy(void) {
	STACKTRACE_VERBOSE;
	__repeat_until_converge<&this_type::__descend_potential_energy>(
		"descend-potential-converge", "descend-potential");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	\param dt time step size
	\param ptol position tolerance, below which is considered converged
	\param ftol force tolerance, below which is considered converged
	\return the number of iterations advanced until convergence.
 */
int_type
placement_engine::solve(void) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
static
ostream&
__dump_array(ostream& o, const T& a, ostream& (T::value_type::*mf)(ostream&) const) {
	typedef	typename T::const_iterator	const_iterator;
	INDENT_SECTION(o);
	const_iterator i(a.begin()), e(a.end());
	size_t j;
	for (j=0; i!=e; ++i, ++j) {
		(*i.*mf)(o << auto_indent << '[' << j << "]: ") << endl;
	}
	return o;
}

template <class T>
static
ostream&
__dump_array(ostream& o, const T& a) {
	return __dump_array(o, a, &T::value_type::dump);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dump same element of parallel arrays.
 */
template <class T1, class T2>
static
ostream&
__dump_array2(ostream& o, const T1& a, const T2& b,
		ostream& (T1::value_type::*mf)(ostream&) const,
		ostream& (T2::value_type::*mf2)(ostream&) const) {
	typedef	typename T1::const_iterator	const_iterator;
	typedef	typename T2::const_iterator	const_iterator2;
	INDENT_SECTION(o);
	const_iterator i(a.begin()), e(a.end());
	const_iterator2 i2(b.begin()), e2(b.end());
	size_t j;
	for (j=0; i!=e; ++i, ++i2, ++j) {
		o << auto_indent << '[' << j << "]: ";
		(*i.*mf)(o);
		(*i2.*mf2)(o) << endl;
	}
	return o;
}

template <class T1, class T2>
static
ostream&
__dump_array2(ostream& o, const T1& a, const T2& b) {
	return __dump_array2(o, a, b,
		&T1::value_type::dump, &T2::value_type::dump);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_object_types(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "object types:" << endl;
	return __dump_array(o, object_types);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_channel_types(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "channel types:" << endl;
	return __dump_array(o, channel_types);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_objects(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "objects:" << endl;
	return __dump_array2(o, space.current, space.objects);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_positions(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "object-positions:" << endl;
	return __dump_array(o, space.current, &object_state::dump);
//	return __dump_array(o, space.current, &object_state::dump_position);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_channels(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "channels:" << endl;
	return __dump_array(o, space.springs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_wells(ostream& o) const {
#if ENABLE_GRAVITY_WELLS
	if (x_wells.size()) x_wells.dump(o << "x-wells: ");
	if (y_wells.size()) y_wells.dump(o << "y-wells: ");
	if (z_wells.size()) z_wells.dump(o << "z-wells: ");
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	updates energy calcultion if necessary, prints report.
 */
ostream&
placement_engine::dump_energy(ostream& o) {
	bootstrap_forces_silent();	// update potential energy if needed
	const save_precision p(o, opt.precision);
	const real_type T = kinetic_energy();
	const real_type V = potential_energy();
	o << "T=" << T << ", V=" << V <<
		", V(attract)=" << space.potential_energy() <<
		", V(repel)=" << proximity_potential_energy <<
		", V(gravity)=" << gravity_potential_energy <<
		", E=T+V=" << T+V << ", L=T-V=" << T-V << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump(ostream& o) const {
	const save_precision p(o, opt.precision);
	dump_parameters(o);
	dump_object_types(o);
	dump_channel_types(o);
	dump_objects(o);
	dump_channels(o);
	dump_wells(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: scale factor
 */
ostream&
placement_engine::emit_dot(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "digraph PL {\n";
	o << "# vi: syntax=dot" << endl;
	opt.emit_dot_bb(o << "graph [bb=\"") << "\"];" << endl;	// bounding box
	o << "# default node attributes" << endl;
	o << "node [shape=box, pin=true];" << endl;
	o << "# default edge attributes" << endl;
	space.emit_dot(o, opt);
	return o << "}" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// identifier string for checkpoints
static
const string magic_string("hackt-ipple-ckpt");

// bump this whenever file format is updated
static
const size_t	checkpoint_version = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves entire session to be restored later (checkpoint).
 */
bool
placement_engine::save_checkpoint(ostream& o) const {
	write_value(o, magic_string);
	write_value(o, checkpoint_version);
	util::numeric::write_seed48(o);
	opt.save_checkpoint(o);
	write_value(o, elapsed_time);
// write object types
	save_array(o, object_types);
// write channel types
	save_array(o, channel_types);
// write objects and channels
	space.save_checkpoint(o);
#if ENABLE_GRAVITY_WELLS
	x_wells.save_checkpoint(o);
	y_wells.save_checkpoint(o);
	z_wells.save_checkpoint(o);
#endif
#if 0
	write_value(o, max_delta_position);
	write_value(o, max_delta_velocity);
#endif

	write_value(o, magic_string);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::load_checkpoint(istream& i) {
	static const char bad_ckpt[] =
		"ERROR: not a valid ipple checkpoint file.";
try {
	string header_check;
	read_value(i, header_check);
	if (header_check != magic_string) {
		cerr << bad_ckpt << endl;
		return true;
	}
	size_t version_check;
	read_value(i, version_check);
	if (version_check != checkpoint_version) {
		cerr << "Expecting checkpoint compatibility version " <<
			checkpoint_version << ", but got " <<
			version_check << "." << endl;
		return true;
	}
} catch (...) {
	cerr << bad_ckpt << endl;
	return true;
}
	util::numeric::read_seed48(i);
	opt.load_checkpoint(i);
	read_value(i, elapsed_time);
// read object types
	load_array(i, object_types);
// read channel types
	load_array(i, channel_types);
// read objects and channels
	space.load_checkpoint(i);
#if ENABLE_GRAVITY_WELLS
	x_wells.load_checkpoint(i);
	y_wells.load_checkpoint(i);
	z_wells.load_checkpoint(i);
#endif
#if 0
	read_value(i, max_delta_position);
	read_value(i, max_delta_velocity);
#endif
{
	string temp;
	read_value(i, temp);
	if (temp != magic_string) {
		cerr << "ERROR: detected checkpoint misalignment!" << endl;
		return true;
	}
}
	return !i;
}

// TODO: dump_checkpoint

//=============================================================================
}	// end namespace PR

