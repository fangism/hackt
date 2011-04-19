/**
	\file "PR/placement_engine.cc"
	$Id: placement_engine.cc,v 1.1.2.4 2011/04/19 01:08:42 fang Exp $
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
#include "util/IO_utils.tcc"
#include "util/iterator_more.h"
#include "util/iomanip.h"
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
using namespace util::vector_ops;		// for many operator overloads
#include "util/using_ostream.h"

//=============================================================================
static
const real_type __default_lower_corner[] = {0.0, 0.0, -50.0};
static
const real_type __default_upper_corner[] = {100.0, 100.0, 50.0};

//=============================================================================
// class placement_engine method definitions

placement_engine::placement_engine(const size_t d) :
		state_base(), 
		object_types(),
		channel_types(),
		temperature(0.0),	// brrrr-r-r-r!!!!
		viscous_damping(0.1),	// gooey
		proximity_radius(0.0),	// for collision scanning
		repulsion_coeff(1.0),
		lower_corner(__default_lower_corner),
		upper_corner(__default_upper_corner),
		time_step(1e-3),
		pos_tol(1e-3),
		vel_tol(1e-3),
//		accel_tol(1e-3),
		precision(4),
		watch_objects(false),
		space(d),
		autosave_name() {
	initialize_default_types();
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
	proximity_radius = space.auto_proximity_radius();
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
	// automatically update proximity radius
	const real_type rad =
		space.objects.back().properties.maximum_dimension();
	if (rad > proximity_radius)
		proximity_radius = rad;
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
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::pin_object(const size_t i) {
	CHECK_OBJECT_INDEX(i)
	space.objects[i].fix();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::unpin_object(const size_t i) {
	CHECK_OBJECT_INDEX(i)
	space.objects[i].unfix();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enforce bounds on position.
 */
void
placement_engine::clamp_position(real_vector& v) const {
	min_clamp_elements(v, lower_corner);
	max_clamp_elements(v, upper_corner);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::place_object(const size_t i, const real_vector& v) {
	CHECK_OBJECT_INDEX(i)
	real_vector p(v);
	clamp_position(p);
	space.objects[i].place(p);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::parse_parameter(const string& s) {
	return parse_parameter(optparse(s));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: list keys from option_map
 */
ostream&
placement_engine::list_parameters(ostream& o) {
	o << "parameters:\n"
"  damping\n"
"  repulsion_coeff\n"
"  position_tolerance\n"
"  velocity_tolerance\n"
"  precision\n"
"  temperature\n"
"  time_step";
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_parameters(ostream& o) const {
	const save_precision p(o, precision);
	o << "parameters:\n";
	o << "  bounds=" << lower_corner << ','  << upper_corner << endl;
	o << "  damping=" << viscous_damping << endl;
	o << "  temperature=" << temperature << endl;
	o << "  proximity_radius=" << proximity_radius << endl;
	o << "  repulsion_coeff=" << repulsion_coeff << endl;
	o << "  time_step=" << time_step << endl;
	o << "  precision=" << precision << endl;
	o << "  position_tolerance=" << pos_tol << endl;
	o << "  velocity_tolerance=" << vel_tol << endl;
//	o << "  acceleration_tolerance=" << accel_tol << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::parse_parameter(const option_value& o) {
	if (o.key == "damping") {
		if (o.values.empty()) {
			cerr << "physics.property.parse: missing value"
				<< endl;
			// or print current value
			return true;
		}
		if (string_to_num(o.values.front(), viscous_damping)) {
			cerr << "physics.property.parse: bad value"
				<< endl;
			return true;
		}
		return false;
	}
	else if (o.key == "precision") {
		if (o.values.empty()) {
			cerr << "physics.property.parse: missing value"
				<< endl;
			// or print current value
			return true;
		}
		if (string_to_num(o.values.front(), precision)) {
			cerr << "physics.property.parse: bad value"
				<< endl;
			return true;
		}
		return false;
	}
	else if (o.key == "repulsion_coeff") {
		if (o.values.empty()) {
			cerr << "physics.property.parse: missing value"
				<< endl;
			// or print current value
			return true;
		}
		if (string_to_num(o.values.front(), repulsion_coeff)) {
			cerr << "physics.property.parse: bad value"
				<< endl;
			return true;
		}
		return false;
	}
	else if (o.key == "temperature") {
		if (o.values.empty()) {
			cerr << "physics.property.parse: missing value"
				<< endl;
			// or print current value
			return true;
		}
		if (string_to_num(o.values.front(), temperature)) {
			cerr << "physics.property.parse: bad value"
				<< endl;
			return true;
		}
		return false;
	}
	else if (o.key == "time_step") {
		if (o.values.empty()) {
			cerr << "physics.property.parse: missing value"
				<< endl;
			// or print current value
			return true;
		}
		if (string_to_num(o.values.front(), time_step)) {
			cerr << "physics.property.parse: bad value"
				<< endl;
			return true;
		}
		return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Place all free objects somewhere randomly within the bounding box.
 */
void
placement_engine::scatter(void) {
	STACKTRACE_VERBOSE;
	const real_vector box_size(upper_corner -lower_corner);
	typedef	rand48<double>			random_generator;
	random_generator g;
	vector<tile_instance>::iterator
		i(space.objects.begin()), e(space.objects.end());
	for ( ; i!=e; ++i) {
	if (!i->is_fixed()) {
		real_vector r;
		// fixed for 3D
		r[0] = g();
		r[1] = g();
		r[2] = g();
		r *= box_size;
		r += lower_corner;
		i->place(r);
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::zero_forces(void) {
	for_each(space.objects.begin(), space.objects.end(), 
		std::mem_fun_ref(&tile_instance::zero_force));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pair-wise attractive-only force.
	Only need channel_instance to be modifiable for the sake of
	updating tension.  
 */
void
placement_engine::apply_attraction_forces(
		tile_instance& sobj, tile_instance& dobj,
#if PR_CHANNEL_TENSION
		const channel_properties& cp
#else
		channel_instance& ch
#endif
		) {
	STACKTRACE_VERBOSE;
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
if (!(sf && df)) {
	// at least one end not fixed
	// TODO: optimize away number of mathematical operations
	// TODO: use delta - sizeof(objects)
	//	or distance between ellipsoids
	const position_type delta(dobj.position -sobj.position);
#if PR_CHANNEL_TENSION
	const force_type force_vec(delta * cp.spring_coeff);
	const real_type& dthresh(cp.equilibrium_distance);
#else
	const force_type force_vec(delta * ch.properties.spring_coeff);
	const real_type& dthresh(ch.properties.equilibrium_distance);
#endif
	const real_type dist = norm(delta);
	// TODO: use rectilinear distance as an option
	if (dist > dthresh) {
#if PR_TILE_MASS
		const tile_type& sp(sobj.properties);
		const tile_type& dp(dobj.properties);
#endif
		const real_type ffrac = (dist -dthresh) / dist;
#if PR_CHANNEL_TENSION
		const real_type tension = norm(force_vec) * ffrac;
		ch.tension = tension;
#endif
		if (sf) {
			// only source is fixed
			dobj.acceleration -= force_vec *
#if PR_TILE_MASS
				(ffrac / dp.mass);
#else
				ffrac;
#endif
		} else if (df) {
			// only dest is fixed
			sobj.acceleration += force_vec *
#if PR_TILE_MASS
				(ffrac / sp.mass);
#else
				ffrac;
#endif
		} else {
			// neither fixed, account for mass ratio
#if PR_TILE_MASS
			const real_type massfrac = dp.mass / (dp.mass +sp.mass);
#else
			static const real_type massfrac = 0.5;
			const real_type hf = ffrac * massfrac;
#endif
			sobj.acceleration +=
#if PR_TILE_MASS
				force_vec * (ffrac * massfrac / sp.mass);
#else
				force_vec * hf;
#endif
			dobj.acceleration -=
#if PR_TILE_MASS
				force_vec * (ffrac * (1.0 -massfrac) / dp.mass);
#else
				force_vec * hf;
#endif
		}
	}	// else objects too close to attract
	// let repulsion forces be computed in different phase
}	// else don't bother computing if both ends are fixed
}	// end placement_engine::apply_object_forces()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pair-wise repulsion-only force.
 */
void
placement_engine::apply_repulsion_forces(
		tile_instance& sobj, tile_instance& dobj,
		const channel_properties& cp
		) {
	STACKTRACE_VERBOSE;
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
if (!(sf && df)) {
	// at least one end not fixed
	// TODO: optimize away number of mathematical operations
	// TODO: use delta - sizeof(objects)
	//	or distance between ellipsoids
	const position_type delta(dobj.position -sobj.position);
	const force_type force_vec(delta * cp.spring_coeff);
	const real_type& dthresh(cp.equilibrium_distance);
	const real_type dist = norm(delta);
	// TODO: use rectilinear distance as an option
	if (dist < dthresh) {
#if PR_TILE_MASS
		const tile_type& sp(sobj.properties);
		const tile_type& dp(dobj.properties);
#endif
		const real_type ffrac = (dist -dthresh) / dist;
		if (sf) {
			// only source is fixed
			dobj.acceleration -= force_vec *
#if PR_TILE_MASS
				(ffrac / dp.mass);
#else
				ffrac;
#endif
		} else if (df) {
			// only dest is fixed
			sobj.acceleration += force_vec *
#if PR_TILE_MASS
				(ffrac / sp.mass);
#else
				ffrac;
#endif
		} else {
			// neither fixed, account for mass ratio
#if PR_TILE_MASS
			const real_type massfrac = dp.mass / (dp.mass +sp.mass);
#else
			static const real_type massfrac = 0.5;
			const real_type hf = ffrac * massfrac;
#endif
			sobj.acceleration +=
#if PR_TILE_MASS
				force_vec * (ffrac * massfrac / sp.mass);
#else
				force_vec * hf;
#endif
			dobj.acceleration -=
#if PR_TILE_MASS
				force_vec * (ffrac * (1.0 -massfrac) / dp.mass);
#else
				force_vec * hf;
#endif
		}
	}	// else objects too close to attract
	// let repulsion forces be computed in different phase
}	// else don't bother computing if both ends are fixed
}	// end placement_engine::apply_object_forces()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should return some information about this step for 
	determining convergence.
	TODO: compute effective force vector, accumulate per object
	TODO: support different spring types
 */
void
placement_engine::compute_spring_forces(void) {
	STACKTRACE_VERBOSE;
	// compute spring tensions (attraction)
	typedef	vector<channel_instance>::iterator	iterator;
	iterator i(space.springs.begin()), e(space.springs.end());
	for ( ; i!=e; ++i) {
		const int_type& si(i->source);
		const int_type& di(i->destination);
		tile_instance& sobj(space.objects[si]);
		tile_instance& dobj(space.objects[di]);
		apply_attraction_forces(sobj, dobj, *i);
	}	// end for each spring
}	// end compute_spring_forces

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D>
static
bool
dim_less(const vector<tile_instance>::const_iterator l,
		const vector<tile_instance>::const_iterator r) {
	return l->position[D] < r->position[D];
}

template <size_t D>
static
bool
dim_comp(const vector<tile_instance>::const_iterator l, const real_type& r) {
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
	for_each(space.objects.begin(), space.objects.end(),
		std::mem_fun_ref(&tile_instance::clear_proximity_cache));
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
	typedef	vector<tile_instance>::iterator		iterator;
	vector<iterator> obj_x;
	obj_x.reserve(space.objects.size());
	iterator i(space.objects.begin()), e(space.objects.end());
	for ( ; i!=e; ++i) {
		obj_x.push_back(i);
	}
	// perform in-place sort on each
	// first, sorted by x-coordinates only
	sort(obj_x.begin(), obj_x.end(), &dim_less<0>);
//	sort(obj_z.begin(), obj_z.end(), &dim_less<2>);
	// use proximity_radius to find sets of objects in the same window
	vector<iterator>::iterator xb(obj_x.begin()), xe(obj_x.end());
	// Q: is binary search worth it? linear-incremental may suffice
	// A: linear! b/c overall cost O(N) vs. O(N lg N)
	vector<iterator>::iterator xu(xb);
//		xu(lower_bound(xb, xe, x+proximity_radius, &dim_comp<0>));
	// [xb, xu] define a sliding window along the x-dimension
	const array_offset<iterator> vo(space.objects.begin());
	const size_t i1 = vo(*xb);
#if 0
	std::set<size_t> window_set_x;	// a queue is good enough?
	transform(xb, xu, util::set_inserter(window_set_x), vo);
 	INVARIANT(window_set_x.size() >= 1);
#endif
	// x-sweep
for ( ; xb!=xe; ++xb) {
	const real_type x = (*xb)->position[0];
	// linear scan overall costs less than repeated (lg N) binary searches
	while (xu!=xe && (*xu)->position[0] < x+proximity_radius) {
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
		yl(lower_bound(yb, ye, y_ref-proximity_radius, &dim_comp<1>)),
		yu(lower_bound(yl, ye, y_ref+proximity_radius, &dim_comp<1>));
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
			zl(lower_bound(zb, ze, z_ref-proximity_radius,
				&dim_comp<2>)),
			zu(lower_bound(zl, ze, z_ref+proximity_radius,
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
				STACKTRACE_INDENT_PRINT(
					"caching collision candidate pair ("
					<< i1 << ',' << i2 << ")." << endl);
				// debug print
				space.objects[i1].proximity_cache.insert(i2);
				space.objects[i2].proximity_cache.insert(i1);
			}
			}
		}
	}
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
	typedef	vector<tile_instance>::iterator		iterator;
	iterator i(space.objects.begin()), e(space.objects.end());
	const array_offset<iterator> vo(i);
	for ( ; i!=e; ++i) {
		const size_t j1 = vo(i);
		set<int_type>::const_iterator
			ci(i->proximity_cache.begin()),
			ce(i->proximity_cache.end());
		for ( ; ci!=ce; ++ci) {
			const size_t j2 = *ci;
			// avoid double counting
			if (j1 < j2) {
				STACKTRACE_INDENT_PRINT("repelling objects " <<
					j1 << " and " << j2 << endl);
				tile_instance& o1(space.objects[j1]);
				tile_instance& o2(space.objects[j2]);
				channel_properties dummy;
				// TODO: configure later
				dummy.spring_coeff = repulsion_coeff;
				dummy.equilibrium_distance =
					(o2.properties.maximum_dimension()
					+o1.properties.maximum_dimension()) *0.5;
				apply_repulsion_forces(o1, o2, dummy);
			}
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: return maximum delta for chekcing for convergence.
 */
void
placement_engine::update_velocity_and_position(void) {
	STACKTRACE_VERBOSE;
	typedef	vector<tile_instance>::iterator		iterator;
	iterator i(space.objects.begin()), e(space.objects.end());
	for ( ; i!=e; ++i) {
	if (!i->is_fixed()) {
		i->update(time_step);
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::iterate(void) {
	STACKTRACE_VERBOSE;
	zero_forces();		// reset forces
	compute_spring_forces();
	// compute proximity repulsions on 'close' objects
	refresh_proximity_cache();
	compute_collision_forces();
	update_velocity_and_position();
	if (watch_objects) {
		dump_objects(cout);
	}
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
__dump_array(ostream& o, const T& a) {
	typedef	typename T::const_iterator	const_iterator;
	INDENT_SECTION(o);
	const_iterator i(a.begin()), e(a.end());
	size_t j;
	for (j=0; i!=e; ++i, ++j) {
		i->dump(o << auto_indent << '[' << j << "]: ") << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_object_types(ostream& o) const {
	const save_precision p(o, precision);
	o << "object types:" << endl;
	return __dump_array(o, object_types);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_channel_types(ostream& o) const {
	const save_precision p(o, precision);
	o << "channel types:" << endl;
	return __dump_array(o, channel_types);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_objects(ostream& o) const {
	const save_precision p(o, precision);
	o << "objects:" << endl;
	return __dump_array(o, space.objects);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_channels(ostream& o) const {
	const save_precision p(o, precision);
	o << "channels:" << endl;
	return __dump_array(o, space.springs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump(ostream& o) const {
	const save_precision p(o, precision);
	dump_parameters(o);
	dump_object_types(o);
	dump_channel_types(o);
	dump_objects(o);
	dump_channels(o);
	return o;
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
// write global parameters
	write_value(o, temperature);
	write_value(o, viscous_damping);
	write_value(o, proximity_radius);
	write_value(o, repulsion_coeff);
	write_value(o, lower_corner);
	write_value(o, upper_corner);
	write_value(o, time_step);
	write_value(o, pos_tol);
	write_value(o, vel_tol);
//	write_value(o, accel_tol);
// write object types
	save_array(o, object_types);
// write channel types
	save_array(o, channel_types);
// write objects and channels
	space.save_checkpoint(o);

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
// read global parameters
	read_value(i, temperature);
	read_value(i, viscous_damping);
	read_value(i, proximity_radius);
	read_value(i, repulsion_coeff);
	read_value(i, lower_corner);
	read_value(i, upper_corner);
	read_value(i, time_step);
	read_value(i, pos_tol);
	read_value(i, vel_tol);
//	read_value(i, accel_tol);
// read object types
	load_array(i, object_types);
// read channel types
	load_array(i, channel_types);
// read objects and channels
	space.load_checkpoint(i);

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

