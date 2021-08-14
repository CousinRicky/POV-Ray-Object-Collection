/* roundedge.inc version 2.0A
 * Persistence of Vision Raytracer include file
 * POV-Ray Object Collection
 *
 * Macros and functions to facilitate rounded CSG joints and edges.
 * Based on various functions and macros that I wrote, dating back to 2004.
 *
 * Copyright (C) 2008 - 2021 Richard Callwood III.  Some rights reserved.
 * This file is licensed under the terms of the CC-LGPL
 * a.k.a. the GNU Lesser General Public License version 2.1.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  Please
 * visit https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html for
 * the text of the GNU Lesser General Public License version 2.1.
 *
 * Vers.  Date         Comments
 * -----  ----         --------
 *        2008-Jul-12  Rounded edge macros and functions are gathered from old
 *                     unpublished include files.
 *        2008-Aug-24  Adaptation for the Object Collection is complete.  A
 *                     crude function for isosurface oblong toroids is omitted.
 *        2008-Sep-02  Local variable names are brought into compliance.
 * 1.0    2008-Sep-04  Uploaded.
 *        2012-Jun-07  RE_Round_inside_join() is added.
 *        2012-Jun-08  RE_Corner_join() is added.
 * 1.1    2012-Jun-09  Uploaded (incomplete upload).
 *        2012-Jul-17  The module is tested for 3.5 compatibility.
 *        2012-Sep-03  A version identifier is added.
 *        2013-Feb-27  Elliptical toroid macros are added.
 * 1.2    2013-Mar-01  Uploaded (incomplete upload).
 * 1.3    2013-Jun-15  Some blob functions are renamed.
 *        2015-Jan-23  Additional error checking is added to elliptical toroids.
 *    ca. 2015-Jan-23  Some error messages are reworded.
 *        2015-Feb-07  An artifact in merged rounded cylinders, when the edge
 *                     radius is too large, is corrected.
 * 1.3.1  2016-Jan-23  Uploaded.
 *        2019-Jul-09  View_POV_Include_Stack is utilized.
 *        2019-Jul-10  An isosurface evaluate wrapper is added.
 *        2020-Apr-17  An anticipatory warning is added to RE_Box() to avoid
 *                     user confusion over Round_Box() warnings.
 * 2.0    2020-Apr-29  Boxes with unequal edge radii are added.
 * 2.0A   2021-Aug-14  The license text is updated.
 *
 * Implementation note: due to POV-Ray namespace scope leakage, local variables
 * and function arguments (other than x, y, and z) *must* be prefixed.
 */
#ifndef (RoundEdge_Inc_Temp) #declare RoundEdge_Inc_Temp = version;
#version 3.5;
#declare ROUNDEDGE_VERSION = 2.0;
#ifdef (View_POV_Include_Stack)
  #debug "#include \"roundedge.inc\" //version 2.0A\n"
#end

#include "functions.inc"
#include "math.inc"
#include "shapes.inc"
#include "transforms.inc"

//========================== INTERNAL UTILITY TOOLS ============================
// These tools should not be referenced directly by the user.

/* Scalar-to-vector automatic promotion:
 *   These functions and macros are safe:
 *     transforms.inc::Point_At_Trans()
 *     vlength()
 *     vnormalize()
 *     vrotate()
 *   These macros are NOT safe:
 *     math.inc::VEq()
 *     transforms.inc::vinv_transform()
 */

#ifndef (RE__SIG) #declare RE__SIG = 4; #end

// An optionally clipped cylinder.  Degenerate cylinders are handled gracefully.
// Warning: the result may be no object.
#macro RE__clipped_cylinder (A, B, R, Condition, Clip)
  #if (!VEq (<0,0,0> + A, <0,0,0> + B) & R != 0)
    #if (Condition)
      intersection
      { cylinder { A, B, R }
        object { Clip }
      }
    #else
      cylinder { A, B, R }
    #end
  #end
#end

// Returns the number of non-zero components in a 3-D vector:
#macro RE__count_axes (V)
  #local re_V = <0,0,0> + V; // force V to be a vector.
  ((re_V.x != 0) + (re_V.y != 0) + (re_V.z != 0))
#end

// A cylinder, for use in situations where a degenerate cylinder can be
// silently omitted.
// Warning: the result may be no object.
#macro RE__cylinder (A, B, R)
  #if (!VEq (A, B) & R != 0)
    cylinder { A, B, R }
  #end
#end

// Converts a number to a string of a fixed number of /significant/ digits,
// in contrast to str(), which returns a requested number of /decimal/ digits.
// Returns: the string.
#macro RE__str (Number)
  #if (Number < 0 | Number > 0) //This circumlocution avoids the epsilon.
    str (Number, 1, max (RE__SIG - floor(log(abs(Number))) - 1, 0))
  #else
    str (0, 1, RE__SIG - 1)
  #end
#end

// A merge of torus and cylinder has an artifact if the major radius is less
// than half the minor radius.  This construction avoids that problem.
#macro RE__torus (RMajor, rMinor, Use_merge)
  #if (RMajor * 2 < rMinor & Use_merge)
    #local re_R = sqrt (rMinor * rMinor - RMajor * RMajor);
    merge
    { difference
      { torus { RMajor, rMinor }
        sphere { 0, re_R }
      }
      sphere { 0, (re_R + rMinor) / 2 }
    }
  #else
    torus { RMajor, rMinor }
  #end
#end

// Returns the inverse rotation of a vector.
#macro RE__unrotate (V, Rotn)
  //vrotate (vrotate (vrotate (V, -Rotn * z), -Rotn * y), -Rotn * x)
  vinv_transform (<0,0,0> + V, transform { rotate Rotn })
#end

// Returns a vector of the absolute values of the components of the input
// vector.
#macro RE__vabs (V)
  #local re_V = <0,0,0> + V; // force V to be a vector.
  <abs (re_V.x), abs (re_V.y), abs (re_V.z)>
#end

// Returns a vector of the signs of the components of the input vector.
#macro RE__vsgn (V)
  #local re_V = <0,0,0> + V; // force V to be a vector.
  <sgn (re_V.x), sgn (re_V.y), sgn (re_V.z)>
#end

// Converts a vector to a string with a fixed number of /significant/ digits,
// in contrast to vstr(), which returns a requested number of /decimal/ digits.
// Returns: the string.
#macro RE__vstr (V)
  #local re_V = <0,0,0> + V; // force V to be a vector.
  concat
  ( "<", RE__str (re_V.x), ", ", RE__str (re_V.y), ", ", RE__str (re_V.z), ">"
  )
#end

//============================== ERROR HANDLING ================================
// These tools should not be referenced directly by the user.

// N.B. In my personal experience, the error checking is *necessary*.

// Throws a fatal error, reporting an illegal zero value.
// This macro is called when construction of an object would require a
// cylinder with coincident endpoints.
#macro RE__error0 (Caller, Side)
  #error concat (Caller, "(): Zero ", Side, ".")
#end

// Calculates the length of a cylinder's axis, throwing a fatal error if it is
// zero, and returning it otherwise.
#macro RE__check_cyl (Caller, A, B)
( #local re_Axis = vlength (A - B);
  #if (re_Axis = 0)
    #error concat
    ( Caller, "(): The endpoints of the cylinder have the same value."
    )
  #else
    re_Axis
  #end
)
#end

// Warns if a value is zero.
#macro RE__check0 (Caller, Desc, Side)
  #if (Side = 0) #warning concat (Caller, "(): Zero ", Desc, ".") #end
#end

// Warns if a value is negative.
#macro RE__check_neg (Caller, Desc, Edge)
  #if (Edge < 0)
    #warning concat
    ( Caller, "(): The ", Desc, " (", RE__str(Edge),
      ") is negative. The resulting shape is undefined."
    )
  #end
#end

// Warns if a value is non-positive.
#macro RE__check_neg0 (Caller, Desc, Edge)
  #if (Edge <= 0)
    #warning concat
    ( Caller, "(): The ", Desc, " (", RE__str(Edge),
      ") is not positive. The resulting shape is undefined."
    )
  #end
#end

// Warns if the edge radius is negative.
#macro RE__check_r (Caller, rEdge)
  RE__check_neg (Caller, "edge radius", rEdge)
#end

// Warns if the edge radius is too large compared to another value.
#macro RE__check_r_gt (Caller, Side, Dim1, Dim2, rEdge, Constraint)
  #local re_Dim = abs (Dim2 - Dim1);
  #if (re_Dim < rEdge * Constraint)
    #warning concat
    ( Caller, "(): The edge radius (", RE__str(rEdge), ") is larger than ",
      #if (Constraint = 2) "half ", #end
      "the ", Side, " (", RE__str(re_Dim),
      "). The resulting shape is undefined."
    )
  #end
#end

// Warns if a value is less than the blob radius.
#macro RE__check_lt_rblob (Caller, Side, Dim1, Dim2, rEdge)
  #local re_Dim = abs (Dim2 - Dim1);
  #if (re_Dim < rEdge)
    #warning concat
    ( Caller, "(): The ", Side, " (", RE__str(re_Dim),
      ") is less than the blob radius (", RE__str(rEdge),
      "). The resulting shape is undefined."
    )
  #end
#end

// Warns if the parameters don't make sense compared to each other.
#macro RE__check3 (Caller, RInner, ROuter, rEdge, Constraint)
  #if (RInner > ROuter)
    #warning concat
    (Caller, "(): The outer radius (", RE__str(ROuter),
      ") is less than the inner radius (", RE__str(RInner),
      "). The resulting shape is undefined."
    )
  #else #if (ROuter - RInner < rEdge * Constraint)
    #warning concat
    ( Caller, "(): The difference between the outer radius (", RE__str(ROuter),
      ") and the inner radius (", RE__str(RInner), ")"
    )
    #warning concat
    ( "is less than ",
      #if (Constraint = 2) "twice ", #end
      "the edge radius (", RE__str(rEdge),
      "). The resulting shape is undefined."
    )
  #end
  #end
#end

// Warns if a value is too small.
// Min should be an integer.
// Returns: the value, corrected if necessary.
#macro RE__check_res (Caller, Value_name, Given, Min)
( #local re_Min = ceil (Min);
  #if (Given < re_Min)
    #warning concat
    ( Caller, "(): ", Value_name, " (", RE__str(Given), ") is less than ",
      str(re_Min,0,0), "; ", str(re_Min,0,0), " is substituted."
    )
    re_Min
  #else
    Given
  #end
)
#end

#ifndef (re__Debug_internal) #declare re__Debug_internal = off; #end

//================================ PARAMETERS ==================================

#ifndef (RE_MORE) #declare RE_MORE = 1.001; #end
#ifndef (RE_LESS) #declare RE_LESS = 0.999; #end
#ifndef (RE_ABIT) #declare RE_ABIT = 0.001; #end
#ifndef (RE_Split_union) #declare RE_Split_union = off; #end
#ifndef (RE_Debug) #declare RE_Debug = off; #end

//------------------- Suggested Values ---------------------

#declare RE_MIN_FACTOR = 0.6;
#declare RE_ATTENUATION = 0.7;

//=============================== JOINT MACROS =================================

#macro RE_Corner_join (rBlob)
  RE__check_neg ("RE_Corner_join", "blob radius", rBlob)
  difference
  { box { -RE_ABIT, rBlob }
    sphere { rBlob, rBlob }
  }
#end

#macro RE_Round_join (RPost, rBlob)
  RE__check_neg ("RE_Round_join", "post radius", RPost)
  RE__check_neg ("RE_Round_join", "blob radius", rBlob)
  difference
  { cylinder { -RE_ABIT * y, rBlob * y, RPost + rBlob }
    torus { RPost + rBlob, rBlob translate rBlob * y }
  }
#end

#macro RE_Round_inside_join (RHollow, rBlob)
  #local re_This = "RE_Round_inside_join"
  RE__check_neg (re_This, "radius of the hollow", RHollow)
  RE__check_neg (re_This, "blob radius", rBlob)
  RE__check_lt_rblob (re_This, "radius of the hollow", 0, RHollow, rBlob)
  difference
  { cylinder { -RE_ABIT * y, max (0, rBlob) * y, RHollow + RE_ABIT }
    #if (rBlob = RHollow)
      sphere { y, 1 scale rBlob }
    #else
      torus { RHollow - rBlob, rBlob translate rBlob * y }
      #if (rBlob * 1.5 > RHollow) //delete inner part of torus
        sphere { y, 1 scale <2*rBlob - RHollow, rBlob, 2*rBlob - RHollow> }
      #end
      #if (rBlob < RHollow)
        cylinder { -2*RE_ABIT * y, (rBlob + RE_ABIT) * y, RHollow - rBlob }
      #end
    #end
  }
  // Note: the max() and the last #if directive are there to avoid a potential
  // degenerate cylinder error.  These precautions won't affect the output if
  // the arguments are legit.
#end

#macro RE_Straight_join_x (pv_Start, End_x, rBlob, Dir)
  #local re_This = "RE_Straight_join_x"
  #local re_Scale = (End_x - pv_Start) * x + <0, rBlob, rBlob>;
  RE__check_neg (re_This, "blob radius", rBlob)
  RE__check0 (re_This, "width", re_Scale.x)
  difference
  { box { -<0, RE_ABIT, RE_ABIT>, 1 }
    cylinder { <-RE_ABIT, 1, 1>, <RE_MORE, 1, 1>, 1 }
    rotate Dir * x
    scale re_Scale
    translate pv_Start
  }
#end

#macro RE_Straight_join_y (pv_Start, End_y, rBlob, Dir)
  #local re_This = "RE_Straight_join_y"
  #local re_Scale = (End_y - pv_Start) * y + <rBlob, 0, rBlob>;
  RE__check_neg (re_This, "blob radius", rBlob)
  RE__check0 (re_This, "height", re_Scale.y)
  difference
  { box { -<RE_ABIT, 0, RE_ABIT>, 1 }
    cylinder { <1, -RE_ABIT, 1>, <1, RE_MORE, 1>, 1 }
    rotate Dir * y
    scale re_Scale
    translate pv_Start
  }
#end

#macro RE_Straight_join_z (pv_Start, End_z, rBlob, Dir)
  #local re_This = "RE_Straight_join_z"
  #local re_Scale = (End_z - pv_Start) * z + <rBlob, rBlob, 0>;
  RE__check_neg (re_This, "blob radius", rBlob)
  RE__check0 (re_This, "depth", re_Scale.z)
  difference
  { box { -<RE_ABIT, RE_ABIT, 0>, 1 }
    cylinder { <1, 1, -RE_ABIT>, <1, 1, RE_MORE>, 1 }
    rotate Dir * z
    scale re_Scale
    translate pv_Start
  }
#end

//=============================== EDGE MACROS ==================================

#macro RE_Parabolic_torus (H, R)
  RE__check_r ("RE_Parabolic_torus", R)
  quartic
  { < H*H, 0, 0, 0, 0, 0, -2*H, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
      0, 0, 1, 0, -R*R
    >
    sturm
  }
#end

//------------------ Elliptical Toroids --------------------

// This auxilliary macro should not be called by the user.
#macro RE__elliptorus_point (RxMajor, RzMajor, rMinor, Theta, Phi)
  #local re_Radj = rMinor * cos(Phi);
  < (RxMajor + re_Radj) * cos(Theta),
    rMinor * sin(Phi),
    (RzMajor + re_Radj) * sin(Theta)
  >
#end

// This auxilliary macro should not be called by the user.
#macro RE__elliptorus_normal (RxMajor, RzMajor, rMinor, Theta, Phi)
( #local RE_NEAR = 0.001;
  #local re_N =
    RE__elliptorus_point (RxMajor, RzMajor, rMinor, Theta, Phi + RE_NEAR);
  #local re_S =
    RE__elliptorus_point (RxMajor, RzMajor, rMinor, Theta, Phi - RE_NEAR);
  #local re_E =
    RE__elliptorus_point (RxMajor, RzMajor, rMinor, Theta + RE_NEAR, Phi);
  #local re_W =
    RE__elliptorus_point (RxMajor, RzMajor, rMinor, Theta - RE_NEAR, Phi);
  vnormalize (vcross (re_E - re_N, re_W - re_N))
+ vnormalize (vcross (re_N - re_W, re_S - re_W))
+ vnormalize (vcross (re_W - re_S, re_E - re_S))
+ vnormalize (vcross (re_S - re_E, re_N - re_E))
)
#end

#macro RE_Elliptorus_mesh (RxMajor, RzMajor, rMinor, ThetaRes, PhiRes)
  #local re_This = "RE_Elliptorus_mesh"
  RE__check_neg (re_This, "major radius in x", RxMajor)
  RE__check_neg (re_This, "major radius in z", RzMajor)
  RE__check_neg (re_This, "minor radius", rMinor)
  #local re_Ntheta = RE__check_res (re_This, "ThetaRes", ThetaRes, 4);
  #local re_Nphi = RE__check_res (re_This, "PhiRes", PhiRes, 4);
  mesh2
  { vertex_vectors
    { re_Ntheta * re_Nphi,
      #local re_V = 0;
      #while (re_V < re_Nphi)
        #local re_U = 0;
        #while (re_U < re_Ntheta)
          RE__elliptorus_point
          ( RxMajor, RzMajor, rMinor,
            re_U/re_Ntheta * 2*pi, re_V/re_Nphi * 2*pi
          )
          #local re_U = re_U + 1;
        #end
        #local re_V = re_V + 1;
      #end
    }
    #if (rMinor != 0)
      normal_vectors
      { re_Ntheta * re_Nphi,
        #local re_V = 0;
        #while (re_V < re_Nphi)
          #local re_U = 0;
          #while (re_U < re_Ntheta)
            RE__elliptorus_normal
            ( RxMajor, RzMajor, rMinor,
              re_U/re_Ntheta * 2*pi, re_V/re_Nphi * 2*pi
            )
            #local re_U = re_U + 1;
          #end
          #local re_V = re_V + 1;
        #end
      }
    #end
    face_indices
    { re_Ntheta * re_Nphi * 2,
      #local re_V = 0;
      #while (re_V < re_Nphi)
        #local re_Base = re_V * re_Ntheta;
        #local re_Next = mod (re_V+1, re_Nphi) * re_Ntheta;
        #local re_U = 0;
        #while (re_U < re_Ntheta)
          #local re_U1 = mod (re_U+1, re_Ntheta);
          <re_Base + re_U, re_Base + re_U1, re_Next + re_U1>
          <re_Base + re_U, re_Next + re_U1, re_Next + re_U>
          #local re_U = re_U + 1;
        #end
        #local re_V = re_V + 1;
      #end
    }
   // Advice has been given to avoid inside_vector parallel to any face (which
   // makes sense).  I have not been able to force adverse consequences during
   // testing, but I'm not taking the chance.
    inside_vector vtransform
    ( x,
      transform
      { rotate degrees (1/re_Nphi) * z
        rotate degrees (1/re_Ntheta) * y
        scale <1, 1, RzMajor/RxMajor>
      }
    )
  }
#end

#macro RE_Elliptorus_octant (RxMajor, RzMajor, rMinor, ThetaRes, PhiRes)
  #local re_This = "RE_Elliptorus_octant"
  RE__check_neg (re_This, "major radius in x", RxMajor)
  RE__check_neg (re_This, "major radius in z", RzMajor)
  RE__check_neg (re_This, "minor radius", rMinor)
  #local re_thRes = RE__check_res (re_This, "ThetaRes", ThetaRes, 1);
  #local re_phRes = RE__check_res (re_This, "PhiRes", PhiRes, 2);
  #local re_Ntheta = re_thRes+1;
  #local re_Nphi = re_phRes+2;
  mesh2
  { vertex_vectors
    { re_Ntheta * re_Nphi,
      #local re_V = 0;
      #while (re_V <= re_phRes)
        #local re_U = 0;
        #while (re_U <= re_thRes)
          RE__elliptorus_point
          ( RxMajor, RzMajor, rMinor,
            re_U * pi / (2 * re_thRes), re_V * pi / re_phRes
          )
          #local re_U = re_U + 1;
        #end
        #local re_V = re_V + 1;
      #end
      #local re_U = 0;
      #while (re_U <= re_thRes)
        #local re_Theta = re_U * pi / (2 * re_thRes);
        <cos(re_Theta) * RxMajor, 0, sin(re_Theta) * RzMajor> - RE_ABIT
        #local re_U = re_U + 1;
      #end
    }
    #if (rMinor != 0 & !re__Debug_internal)
      normal_vectors
      { re_Ntheta * re_Nphi,
        #local re_V = 0;
        #while (re_V <= re_phRes)
          #local re_U = 0;
          #while (re_U <= re_thRes)
            RE__elliptorus_normal
            ( RxMajor, RzMajor, rMinor,
              re_U * pi / (2 * re_thRes), re_V * pi / re_phRes
            )
            #local re_U = re_U + 1;
          #end
          #local re_V = re_V + 1;
        #end
        #local re_U = 0;
        #while (re_U <= re_thRes)
          , -y
          #local re_U = re_U + 1;
        #end
      }
    #end
    face_indices
    { re_thRes * re_Nphi * 2 + re_phRes * 2,
      #local re_V = 0;
      #while (re_V < re_Nphi)
        #local re_Base = re_V * re_Ntheta;
        #local re_Next = mod (re_V+1, re_Nphi) * re_Ntheta;
        #local re_U = 0;
        #while (re_U < re_thRes)
          <re_Base + re_U, re_Base + re_U+1, re_Next + re_U+1>
          <re_Base + re_U, re_Next + re_U+1, re_Next + re_U>
          #local re_U = re_U + 1;
        #end
        #local re_V = re_V + 1;
      #end
      #local re_V = 0;
      #while (re_V < re_phRes)
        <re_Ntheta * (re_Nphi-1), re_V * re_Ntheta, (re_V+1) * re_Ntheta>
        #local re_V = re_V + 1;
        <re_Ntheta * re_Nphi, re_V * re_Ntheta, (re_V+1) * re_Ntheta> - 1
      #end
    }
   // Advice has been given to avoid inside_vector parallel to any face (which
   // makes sense).  I have not been able to force adverse consequences during
   // testing, but I'm not taking the chance.
    inside_vector
      vrotate (x, degrees ((floor (re_Nphi / 2) * pi + 1) / re_Nphi) * z)
  }
#end

//============================ ROUNDED BOX MACROS ==============================

// Implementation note:  The many conditional (#if) directives in the
// rounded box macros handle the boundary cases.  Much of the clipping
// in the partially rounded boxes is for near-boundary cases.

//----------- Rounded on All Edges and Corners -------------

#macro RE_Box (A, B, rEdge, Use_merge)
  RE__check_r ("RE_Box", rEdge)
  #local re_Dim = (A - B) / <2,2,2>;
 //Round_Box() gives more detailed warnings.
  #if (min (abs (re_Dim.x), abs (re_Dim.y), abs (re_Dim.z)) < rEdge)
    #warning "RE_Box(): Round_Box() warning:"
  #end
  object
  { Round_Box (<0,0,0> + A, <0,0,0> + B, rEdge, Use_merge)
    #if (!Use_merge & !RE_Split_union) split_union off #end
  }
#end

//---------------- Rounded along One Axis ------------------

// Internal macro: flat top and bottom.
// This macro should not be called by the user.
#macro RE__box_axis (vA, vB, rEdge, Use_merge)

  #if (Use_merge)
    #local re_Fuzz = vlength (vB - vA) * RE_ABIT;
    #local re_Base = merge
  #else
    #local re_Fuzz = 0;
    union
  #end
  { #local re_AOut =
      <min (vA.x, vB.x), min (vA.y, vB.y) - re_Fuzz, min (vA.z, vB.z)>;
    #local re_BOut =
      <max (vA.x, vB.x), max (vA.y, vB.y) + re_Fuzz, max (vA.z, vB.z)>;
    #local re_AIn = re_AOut + rEdge;
    #local re_BIn = re_BOut - rEdge;
    #local re_Corner = cylinder { re_AOut * y, re_BOut * y, rEdge }
    #if (re_AOut.z != re_BOut.z)
      box { <re_AIn.x, re_AOut.y, re_AOut.z>, <re_BIn.x, re_BOut.y, re_BOut.z> }
    #end
    #if (re_AOut.x != re_BOut.x)
      box { <re_AOut.x, re_AOut.y, re_AIn.z>, <re_BOut.x, re_BOut.y, re_BIn.z> }
    #end
    #if (re_AIn.x != re_BIn.x & re_AIn.z != re_BIn.z)
      object { re_Corner translate <re_AIn.x, 0, re_AIn.z> }
    #end
    #if (re_AIn.x != re_BIn.x)
      object { re_Corner translate <re_AIn.x, 0, re_BIn.z> }
    #end
    #if (re_AIn.z != re_BIn.z)
      object { re_Corner translate <re_BIn.x, 0, re_AIn.z> }
    #end
    object { re_Corner translate <re_BIn.x, 0, re_BIn.z> }
    #if (!Use_merge & !RE_Split_union) split_union off #end
  }
  #if (Use_merge) //shear off the coincident surfaces
    intersection
    { object { re_Base }
      box
      { <re_AOut.x, re_BOut.y, re_AOut.z> - re_Fuzz,
        <re_BOut.x, re_AOut.y, re_BOut.z> + re_Fuzz
      }
    }
  #end
#end

//----- Public Macros -----

#macro RE_Box_x (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_x"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 2)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 2)
  #if (re_vA.x = re_vB.x) RE__error0 (re_This, "width") #end
  object
  { RE__box_axis
    ( <re_vA.y, -re_vA.x, re_vA.z>, <re_vB.y, -re_vB.x, re_vB.z>, rEdge,
      Use_merge
    )
    rotate 90 * z
  }
#end

#macro RE_Box_y (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_y"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 2)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 2)
  #if (re_vA.y = re_vB.y) RE__error0 (re_This, "height") #end
  RE__box_axis (re_vA, re_vB, rEdge, Use_merge)
#end

#macro RE_Box_z (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_z"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 2)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 2)
  #if (re_vA.z = re_vB.z) RE__error0 (re_This, "depth") #end
  object
  { RE__box_axis
    ( <re_vA.x, re_vA.z, -re_vA.y>, <re_vB.x, re_vB.z, -re_vB.y>, rEdge,
      Use_merge
    )
    rotate 90 * x
  }
#end

//------------------ Rounded on One End --------------------

// Internal macro: rounded top, flat bottom.
// This macro should not be called by the user.
#macro RE__box_end (A, B, rEdge, Use_merge)

  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_Base = #if (Use_merge) merge #else union #end
  { #local re_AOut =
      <min (re_vA.x, re_vB.x), min (re_vA.y, re_vB.y), min (re_vA.z, re_vB.z)>;
    #local re_BOut =
      <max (re_vA.x, re_vB.x), max (re_vA.y, re_vB.y), max (re_vA.z, re_vB.z)>;
    #local re_BIn = re_BOut - rEdge;
    #local re_AIn =
    < re_AOut.x + rEdge,
      re_AOut.y - (Use_merge? rEdge * RE_ABIT: 0),
      re_AOut.z + rEdge
    >;

   //rounded top
    sphere { <re_BIn.x, re_BIn.y, re_BIn.z>, rEdge }
    #if (re_AIn.x != re_BIn.x)
      sphere { <re_AIn.x, re_BIn.y, re_BIn.z>, rEdge }
      cylinder
      { <re_AIn.x, re_BIn.y, re_BIn.z>, <re_BIn.x, re_BIn.y, re_BIn.z>, rEdge
      }
    #end
    #if (re_AIn.z != re_BIn.z)
      sphere { <re_BIn.x, re_BIn.y, re_AIn.z>, rEdge }
      cylinder
      { <re_BIn.x, re_BIn.y, re_AIn.z>, <re_BIn.x, re_BIn.y, re_BIn.z>, rEdge
      }
    #end
    #if (re_AIn.z != re_BIn.z & re_AIn.x != re_BIn.x)
      sphere { <re_AIn.x, re_BIn.y, re_AIn.z>, rEdge }
      cylinder
      { <re_AIn.x, re_BIn.y, re_AIn.z>, <re_AIn.x, re_BIn.y, re_BIn.z>, rEdge
      }
      cylinder
      { <re_AIn.x, re_BIn.y, re_AIn.z>, <re_BIn.x, re_BIn.y, re_AIn.z>, rEdge
      }
      box { <re_AIn.x, re_BIn.y, re_AIn.z>, <re_BIn.x, re_BOut.y, re_BIn.z> }
    #end
   //flat bottom
    #if (re_AOut.y < re_BIn.y)
      cylinder
      { <re_BIn.x, re_BIn.y, re_BIn.z>, <re_BIn.x, re_AIn.y, re_BIn.z>, rEdge
      }
    #end
    #if (re_AOut.y < re_BIn.y & re_AIn.x != re_BIn.x)
      cylinder
      { <re_AIn.x, re_BIn.y, re_BIn.z>, <re_AIn.x, re_AIn.y, re_BIn.z>, rEdge
      }
      box { <re_AIn.x, re_AIn.y, re_AOut.z>, <re_BIn.x, re_BIn.y, re_BOut.z> }
    #end
    #if (re_AOut.y < re_BIn.y & re_AIn.z != re_BIn.z)
      cylinder
      { <re_BIn.x, re_BIn.y, re_AIn.z>, <re_BIn.x, re_AIn.y, re_AIn.z>, rEdge
      }
      box { <re_AOut.x, re_AIn.y, re_AIn.z>, <re_BOut.x, re_BIn.y, re_BIn.z> }
    #end
    #if (re_AOut.y < re_BIn.y & re_AIn.z != re_BIn.z & re_AIn.x != re_BIn.x)
      cylinder
      { <re_AIn.x, re_BIn.y, re_AIn.z>, <re_AIn.x, re_AIn.y, re_AIn.z>, rEdge
      }
    #end
    #if (!Use_merge & !RE_Split_union) split_union off #end
  }
  #if (Use_merge //then shear off the coincident surface
     | re_AOut.y > re_BIn.y - rEdge) //then crop protruding cylinders & spheres
    difference { object { re_Base } plane { y, re_AOut.y } }
  #else
    object { re_Base }
  #end
#end

//----- Public Macros -----

#macro RE_Box_up (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_up"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 2)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 1)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 2)
  RE__box_end (re_vA, re_vB, rEdge, Use_merge)
#end

#macro RE_Box_down (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_down"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 2)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 1)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 2)
  object
  { RE__box_end
    ( <re_vA.x, -re_vA.y, re_vA.z>, <re_vB.x, -re_vB.y, re_vB.z>, rEdge,
      Use_merge
    )
    scale <1, -1, 1>
  }
#end

#macro RE_Box_right (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_right"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 1)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 2)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 2)
  object
  { RE__box_end
    ( <-re_vA.y, re_vA.x, re_vA.z>, <-re_vB.y, re_vB.x, re_vB.z>, rEdge,
      Use_merge
    )
    rotate -90 * z
  }
#end

#macro RE_Box_left (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_left"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 1)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 2)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 2)
  object
  { RE__box_end
    ( <re_vA.y, -re_vA.x, re_vA.z>, <re_vB.y, -re_vB.x, re_vB.z>, rEdge,
      Use_merge
    )
    rotate 90 * z
  }
#end

#macro RE_Box_near (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_near"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 2)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 2)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 1)
  object
  { RE__box_end
    ( <re_vA.x, -re_vA.z, re_vA.y>, <re_vB.x, -re_vB.z, re_vB.y>, rEdge,
      Use_merge
    )
    rotate -90 * x
  }
#end

#macro RE_Box_far (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_far"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 2)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 2)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 1)
  object
  { RE__box_end
    (<re_vA.x, re_vA.z, -re_vA.y>, <re_vB.x, re_vB.z, -re_vB.y>, rEdge,
      Use_merge
    )
    rotate 90 * x
  }
#end

//---------- Rounded on One Side along One Axis ------------

// Internal macro: the two top edges in the x-direction rounded.
// This macro should not be called by the user.
#macro RE__box_side (vA, vB, rEdge, Use_merge)

  #if (Use_merge)
    #local re_Fuzz = rEdge * RE_ABIT;
    #local re_Base = merge
  #else
    #local re_Fuzz = 0;
    #local re_Base = union
  #end
  { #local re_AOut = <min (vA.x, vB.x), min (vA.y, vB.y), min (vA.z, vB.z)>;
    #local re_BOut = <max (vA.x, vB.x), max (vA.y, vB.y), max (vA.z, vB.z)>;
    #local re_BIn =
      <re_BOut.x + re_Fuzz, re_BOut.y - rEdge, re_BOut.z - rEdge>;
    #local re_AIn =
      <re_AOut.x - re_Fuzz, re_BIn.y - re_Fuzz, re_AOut.z + rEdge>;

    #if (re_AOut.y < re_BIn.y)
      box
      { <re_AIn.x, min (re_AOut.y, re_BIn.y - rEdge), re_AOut.z>,
        <re_BIn.x, re_BIn.y, re_BOut.z>
      }
    #end
    #if (re_AIn.z != re_BIn.z)
      box { re_AIn, <re_BIn.x, re_BOut.y, re_BIn.z> }
      cylinder
      { <re_AIn.x, re_BIn.y, re_AIn.z>, <re_BIn.x, re_BIn.y, re_AIn.z>, rEdge
      }
    #end
    cylinder { <re_AIn.x, re_BIn.y, re_BIn.z>, re_BIn, rEdge }

    #if (!Use_merge & !RE_Split_union) split_union off #end
  }
  #if (Use_merge) //shear off the coincident surface
    intersection
    { object { re_Base }
      box { re_AOut - <0, re_Fuzz, re_Fuzz>, re_BOut + <0, re_Fuzz, re_Fuzz> }
    }
  #else #if (re_AOut.y > re_BIn.y - rEdge) //crop protruding cylinders
    difference
    { object { re_Base }
      plane { y, re_AOut.y }
    }
  #else
    object { re_Base }
  #end
  #end
#end

//----- Public Macros -----

#macro RE_Box_x_up (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_x_up"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 1)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 2)
  #if (re_vA.x = re_vB.x) RE__error0 (re_This, "width") #end
  RE__box_side (re_vA, re_vB, rEdge, Use_merge)
#end

#macro RE_Box_x_down (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_x_down"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 1)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 2)
  #if (re_vA.x = re_vB.x) RE__error0 (re_This, "width") #end
  object
  { RE__box_side
    ( <re_vA.x, -re_vA.y, re_vA.z>, <re_vB.x, -re_vB.y, re_vB.z>, rEdge,
      Use_merge
    )
    scale <1, -1, 1>
  }
#end

#macro RE_Box_x_near (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_x_near"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 2)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 1)
  #if (re_vA.x = re_vB.x) RE__error0 (re_This, "width") #end
  object
  { RE__box_side
    ( <re_vA.x, -re_vA.z, re_vA.y>, <re_vB.x, -re_vB.z, re_vB.y>, rEdge,
      Use_merge
    )
    rotate -90 * x
  }
#end

#macro RE_Box_x_far (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_x_far"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 2)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 1)
  #if (re_vA.x = re_vB.x) RE__error0 (re_This, "width") #end
  object
  { RE__box_side
    ( <re_vA.x, re_vA.z, -re_vA.y>, <re_vB.x, re_vB.z, -re_vB.y>, rEdge,
      Use_merge
    )
    rotate 90 * x
  }
#end

#macro RE_Box_y_right (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_y_right"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 1)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 2)
  #if (re_vA.y = re_vB.y) RE__error0 (re_This, "height") #end
  object
  { RE__box_side
    ( <-re_vA.y, re_vA.x, re_vA.z>, <-re_vB.y, re_vB.x, re_vB.z>, rEdge,
      Use_merge
    )
    rotate -90 * z
  }
#end

#macro RE_Box_y_left (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_y_left"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 1)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 2)
  #if (re_vA.y = re_vB.y) RE__error0 (re_This, "height") #end
  object
  { RE__box_side
    ( <re_vA.y, -re_vA.x, re_vA.z>, <re_vB.y, -re_vB.x, re_vB.z>, rEdge,
      Use_merge
    )
    rotate 90 * z
  }
#end

#macro RE_Box_y_near (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_y_near"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 2)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 1)
  #if (re_vA.y = re_vB.y) RE__error0 (re_This, "height") #end
  object
  { RE__box_side
    ( <re_vA.y, -re_vA.z, -re_vA.x>, <re_vB.y, -re_vB.z, -re_vB.x>, rEdge,
      Use_merge
    )
    rotate <-90, 0, 90>
  }
#end

#macro RE_Box_y_far (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_y_far"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 2)
  RE__check_r_gt (re_This, "depth", re_vA.z, re_vB.z, rEdge, 1)
  #if (re_vA.y = re_vB.y) RE__error0 (re_This, "height") #end
  object
  { RE__box_side
    ( <re_vA.y, re_vA.z, re_vA.x>, <re_vB.y, re_vB.z, re_vB.x>, rEdge,
      Use_merge
    )
    rotate <90, 0, 90>
  }
#end

#macro RE_Box_z_up (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_z_up"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 2)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 1)
  #if (re_vA.z = re_vB.z) RE__error0 (re_This, "depth") #end
  object
  { RE__box_side
    ( <-re_vA.z, re_vA.y, re_vA.x>, <-re_vB.z, re_vB.y, re_vB.x>, rEdge,
      Use_merge
    )
    rotate 90 * y
  }
#end

#macro RE_Box_z_down (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_z_down"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 2)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 1)
  #if (re_vA.z = re_vB.z) RE__error0 (re_This, "depth") #end
  object
  { RE__box_side
    ( <re_vA.z, -re_vA.y, re_vA.x>, <re_vB.z, -re_vB.y, re_vB.x>, rEdge,
      Use_merge
    )
    rotate <0, -90, 180>
  }
#end

#macro RE_Box_z_right (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_z_right"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 1)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 2)
  #if (re_vA.z = re_vB.z) RE__error0 (re_This, "depth") #end
  object
  { RE__box_side
    ( <re_vA.z, re_vA.x, re_vA.y>, <re_vB.z, re_vB.x, re_vB.y>, rEdge,
      Use_merge
    )
    rotate <-90, -90, 0>
  }
#end

#macro RE_Box_z_left (A, B, rEdge, Use_merge)
  #local re_vA = <0,0,0> + A;
  #local re_vB = <0,0,0> + B;
  #local re_This = "RE_Box_z_left"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "width", re_vA.x, re_vB.x, rEdge, 1)
  RE__check_r_gt (re_This, "height", re_vA.y, re_vB.y, rEdge, 2)
  #if (re_vA.z = re_vB.z) RE__error0 (re_This, "depth") #end
  object
  { RE__box_side
    ( <-re_vA.z, -re_vA.x, re_vA.y>, <-re_vB.z, -re_vB.x, re_vB.y>, rEdge,
      Use_merge
    )
    rotate <-90, 90, 0>
  }
#end

//===================== UNEQUAL-EDGE ROUNDED BOX MACROS ========================

//------------- Unequal Edge Internal Utilities ----------------
// These tools should not be used directly by the user.

// Returns +/- 1, 2, or 3 for axis +/- x, y, or z, respectively.
// Throws a fatal error if the vector is not a Cartesian axis or its opposite.
#macro RE__check_axis (Caller, Desc, v_Axis)
( #if (RE__count_axes (v_Axis) != 1)
    #error concat
    ( Caller, "(): The ", Desc, " (", RE__vstr (v_Axis),
      ") must have one and only on non-zero component."
    )
  #end
 // There should be no parse errors past this point.  If v_Axis was a scalar,
 // then it has already bombed out on the previous test; if it has no .z
 // component, then the #if construction will necessarily conclude at .x or .y.
  #if (v_Axis.x) select (v_Axis.x, -1, 1)
  #else #if (v_Axis.y) select (v_Axis.y, -2, 2)
  #else select (v_Axis.z, -3, 3)
  #end
  #end
)
#end

// Warns if unequal edge radii do not make sense.
#macro RE__check_unequal (Caller, rSide, rEnd)
  RE__check_neg (Caller, "side edge radius", rSide)
  RE__check_neg (Caller, "end edge radius", rEnd)
  #if (rEnd > rSide & rSide >= 0 & rEnd >= 0)
    #warning concat
    ( Caller, "(): The end edge radius (", RE__str (rEnd),
      ") is larger than the side edge radius (", RE__str (rSide),
      "). The resulting shape is undefined."
    )
  #end
#end

// Warns if an unequal edge radius is too large compared to an adjacent face.
#macro RE__check_unequal_gt (Caller, Desc, Dim, rEdge, Constraint)
 // This roundabout test avoids FP glitches at boundary cases.
  #if (!(Dim >= rEdge * Constraint))
    #warning concat
    ( Caller, "(): The ", Desc, " edge radius (", RE__str (rEdge),
      ") is larger than ",
      #if (Constraint = 2) "half of ", #end
      "an adjacent dimension (", RE__str (Dim),
      "). The resulting shape is undefined."
    )
    #if (re__Debug_internal)
      #warning concat ("difference = ", RE__str (Dim - rEdge * Constraint))
    #end
  #end
#end

// An edge cylinder that is clipped if the flat bottom of its unioned object
// encroaches.  Merged objects are not clipped at this stage.  The cylinder's
// axis is assumed to be parallel to the x-z plane.
#macro RE__clip_edge (A, B, R, Y, Use_merge)
  #if (Y < R & !Use_merge)
    intersection
    { cylinder { A, B, R }
      plane { -y, Y }
    }
    #if (re__Debug_internal)
      #debug "RE__clip_edge\n"
      #debug concat ("( ", RE__vstr (A), ", ", RE__vstr (B), ",\n")
      #debug concat
      ( "  ", RE__str (R), ", ", RE__str (Y), ", ", str (Use_merge, 0, 0), "\n"
      )
      #debug "): clipped\n"
    #end
  #else
    cylinder { A, B, R }
  #end
#end

// Indexes a combination of 2 axes, for use with re__Unequals3.
#macro RE__index_axes (v_Axes)
  #local re_Coded = RE__vsgn (v_Axes) + 1;
  re__Axes_indices [re_Coded.x] [re_Coded.y] [re_Coded.z]
#end

//----- rotations -----
#declare RE__NA = <0, 0, 0>; // "not applicable" place holder

#declare re__Unequals1 = array[7] // RE_Unequal and RE_Unequal_end
{ -90 * x, 180 * x, 90 * z, RE__NA, -90 * z, <0, 0, 0>, 90 * x,
}
#declare re__Unequals2 = array[7][7] // RE_Unequal_side and RE_Unequal_side_end
{ { RE__NA, <270, 0, 270>, <270, 0, 180>, RE__NA,
    <270, 0, 0>, <270, 0, 90>, RE__NA
  },
  { <180, 90, 0>, RE__NA, <180, 180, 0>, RE__NA,
    <180, 0, 0>, RE__NA, <180, 270, 0>
  },
  { <270, 90, 0>, <180, 0, 270>, RE__NA, RE__NA,
    RE__NA, <0, 0, 90>, <90, 270, 0>
  },
  { RE__NA, RE__NA, RE__NA, RE__NA, RE__NA, RE__NA, RE__NA },
  { <90, 90, 0>, <0, 0, 270>, RE__NA, RE__NA,
    RE__NA, <180, 0, 90>, <270, 270, 0>
  },
  { <0, 90, 0>, RE__NA, <0, 180, 0>, RE__NA,
    <0, 0, 0>, RE__NA, <0, 270, 0>
  },
  { RE__NA, <90, 0, 270>, <90, 0, 180>, RE__NA,
    <90, 0, 0>, <90, 0, 90>, RE__NA
  },
}
#declare re__Unequals3 = array[12][7] // RE_Unequal_edge and RE_Unequal_corner
{ { RE__NA, RE__NA, <0, -90, 90>, RE__NA, <0, 180, -90>, RE__NA, RE__NA },
  { <-90, 0, 90>, RE__NA, RE__NA, RE__NA, RE__NA, RE__NA, <90, 0, 0> },
  { RE__NA, RE__NA, <0, 0, 90>, RE__NA, <0, 90, -90> RE__NA, RE__NA },
  { <-90, 0, 180>, RE__NA, RE__NA, RE__NA, RE__NA, RE__NA, <90, 0, 90> },
  { RE__NA, <180, 0, 0>, RE__NA, RE__NA, RE__NA, <0, -90, 0>, RE__NA },
  { RE__NA, <180, 90, 0>, RE__NA, RE__NA, RE__NA, <0, 0, 0>, RE__NA },
  { RE__NA, <180, 180, 0>, RE__NA, RE__NA, RE__NA, <0, 90, 0>, RE__NA },
  { RE__NA, <180, -90, 0>, RE__NA, RE__NA, RE__NA, <0, 180, 0>, RE__NA },
  { RE__NA, RE__NA, <0, 180, 90>, RE__NA, <0, -90, -90>, RE__NA, RE__NA },
  { <-90, 0, 0>, RE__NA, RE__NA, RE__NA, RE__NA, RE__NA, <90, 0, -90> },
  { RE__NA, RE__NA, <0, 90, 90>, RE__NA, <0, 0, -90>, RE__NA, RE__NA },
  { <-90, 0, -90>, RE__NA, RE__NA, RE__NA, RE__NA, RE__NA, <90, 0, 180> },
}
#declare re__Axes_indices = array[3][3][3]
{ { { -1, 11, -1 }, {  6, -1,  7 }, { -1,  3, -1 } },
  { { 10, -1,  8 }, { -1, -1, -1 }, {  2, -1,  0 } },
  { { -1,  9, -1 }, {  5, -1,  4 }, { -1,  1, -1 } },
}

//---------------- Unequal Edge Public Macros ------------------

// Implementation note:  The code for the macros in this section can
// indeed be greatly simplified, by using hierarchical unions or merges.
// However, testing revealed this to increase render times.

// Implementation note:  Clipping and coincident surface mitigation vary
// depending on whether the object is a union or a merge; thus Use_merge
// is tested in otherwise unexpected places.

// Implementation note:  The many conditional (#if) directives in the unequal
// edge macros are for handling the boundary cases.  Much of the clipping in
// the partially rounded boxes is for boundary or near-boundary cases.

// Usage note:  Attempts have been made to insure that scalar arguments passed
// in for any vector parameters are promoted to vectors, whether implicitly by
// operations within the unequal macros, or explicitly by the utility macros
// that they call.  Implementation-wise, this means that there should be no
// parse errors when these macros refer to .x, .y, or .z.  In practice, scalars
// may logically be passed in for A, B, and v_Corner; however, promotion of
// v_Edge, v_End, v_Orientation, or v_Side results in invalid vectors in their
// respective contexts, so a parse error will be issued for these anyway.

#macro RE_Unequal (A, B, rSide, rEnd, Use_merge, v_Orientation)
  #local re_This = "RE_Unequal"
  RE__check_unequal (re_This, rSide, rEnd)
  #local re_Axis = RE__check_axis (re_This, "orientation", v_Orientation);
  #local re_Rotn = re__Unequals1 [re_Axis + 3];
  #local re_Place = transform { rotate re_Rotn translate (A + B) / 2 }
  #local re_v_Size = RE__vabs (vrotate (A - B, re_Rotn));
  RE__check_unequal_gt
  ( re_This, "side", min (re_v_Size.x, re_v_Size.z), rSide, 2
  )
  RE__check_unequal_gt (re_This, "end", VMin (re_v_Size), rEnd, 2)
  #local re_v_Out = re_v_Size / 2; //corner of containing box
  #local re_v_In = re_v_Out - <rSide, rEnd, rSide>; //center of corner torus
  #local re_RCorner = rSide - rEnd;
  #local re_xEdge = re_v_Out.x - rEnd;
  #local re_zEdge = re_v_Out.z - rEnd;
  #local re_Fuzz = (Use_merge? RE_ABIT: 0); //to avoid coincident surfaces
  #local re_Corner =
    #if (rSide = rEnd) sphere { re_v_In, rEnd } //degenerate torus
    #else object { RE__torus (rSide - rEnd, rEnd, Use_merge) translate re_v_In }
    #end
  #local re_Base = #if (Use_merge) merge #else union #end
  { box
    { -<re_xEdge, re_v_Out.y + re_Fuzz, re_v_In.z>,
      <re_xEdge, re_v_Out.y + re_Fuzz, re_v_In.z>
    }
    box
    { -<re_v_In.x, re_v_Out.y + re_Fuzz, re_zEdge>,
      <re_v_In.x, re_v_Out.y + re_Fuzz, re_zEdge>
    }
    object { re_Corner }
    RE__cylinder
    ( <re_v_In.x, re_v_Out.y + re_Fuzz, re_v_In.z>,
      <re_v_In.x, -re_v_Out.y - re_Fuzz, re_v_In.z>, re_RCorner
    )
    #if (re_v_In.x != 0)
      object { re_Corner scale <-1, 1, 1> }
      cylinder
      { <re_v_In.x, re_v_In.y, re_zEdge>,
        <-re_v_In.x, re_v_In.y, re_zEdge>, rEnd
      }
      #if (re_zEdge != 0)
        cylinder
        { <re_v_In.x, re_v_In.y, -re_zEdge>,
          <-re_v_In.x, re_v_In.y, -re_zEdge>, rEnd
        }
      #end
      RE__cylinder
      ( <-re_v_In.x, re_v_Out.y + re_Fuzz, re_v_In.z>,
        <-re_v_In.x, -re_v_Out.y - re_Fuzz, re_v_In.z>, re_RCorner
      )
    #end
    #if (re_v_In.z != 0)
      object { re_Corner scale <1, 1, -1> }
      cylinder
      { <re_xEdge, re_v_In.y, re_v_In.z>,
        <re_xEdge, re_v_In.y, -re_v_In.z>, rEnd
      }
      #if (re_xEdge != 0)
        cylinder
        { <-re_xEdge, re_v_In.y, re_v_In.z>,
          <-re_xEdge, re_v_In.y, -re_v_In.z>, rEnd
        }
      #end
      RE__cylinder
      ( <re_v_In.x, re_v_Out.y + re_Fuzz, -re_v_In.z>,
        <re_v_In.x, -re_v_Out.y - re_Fuzz, -re_v_In.z>, re_RCorner
      )
    #end
    #if (re_v_In.x != 0 & re_v_In.z != 0)
      object { re_Corner scale <-1, 1, -1> }
      RE__cylinder
      ( <-re_v_In.x, re_v_Out.y + re_Fuzz, -re_v_In.z>,
        <-re_v_In.x, -re_v_Out.y - re_Fuzz, -re_v_In.z>, re_RCorner
      )
    #end
    #if (re_v_In.y != 0)
      box
      { -<re_v_In.x, re_v_In.y, re_v_Out.z>,
        <re_v_In.x, re_v_In.y, re_v_Out.z>
      }
      box
      { -<re_v_Out.x, re_v_In.y, re_v_In.z>,
        <re_v_Out.x, re_v_In.y, re_v_In.z>
      }
      object { re_Corner scale <1, -1, 1> }
      cylinder { re_v_In, re_v_In * <1, -1, 1>, rSide }
      #if (re_v_In.x != 0)
        object { re_Corner scale <-1, -1, 1> }
        cylinder { re_v_In * <-1, 1, 1>, re_v_In * <-1, -1, 1>, rSide }
        cylinder
        { <re_v_In.x, -re_v_In.y, re_zEdge>,
          <-re_v_In.x, -re_v_In.y, re_zEdge>, rEnd
        }
        #if (re_zEdge != 0)
          cylinder
          { <re_v_In.x, -re_v_In.y, -re_zEdge>,
            <-re_v_In.x, -re_v_In.y, -re_zEdge>, rEnd
          }
        #end
      #end
      #if (re_v_In.z != 0)
        object { re_Corner scale <1, -1, -1> }
        cylinder { re_v_In * <1, 1, -1>, re_v_In * <1, -1, -1>, rSide }
        cylinder
        { <re_xEdge, -re_v_In.y, re_v_In.z>,
          <re_xEdge, -re_v_In.y, -re_v_In.z>, rEnd
        }
        #if (re_xEdge != 0)
          cylinder
          { <-re_xEdge, -re_v_In.y, re_v_In.z>,
            <-re_xEdge, -re_v_In.y, -re_v_In.z>, rEnd
          }
        #end
      #end
      #if (re_v_In.x != 0 & re_v_In.z != 0)
        object { re_Corner scale <-1, -1, -1> }
        cylinder { re_v_In * <-1, 1, -1>, re_v_In * <-1, -1, -1>, rSide }
      #end
    #end
  }
  #if (Use_merge) //shear off the coincident surfaces
    intersection
    { object { re_Base }
      box
      { -<re_v_Out.x + re_Fuzz, re_v_Out.y, re_v_Out.z + re_Fuzz>,
        <re_v_Out.x + re_Fuzz, re_v_Out.y, re_v_Out.z + re_Fuzz>
      }
      transform { re_Place }
    }
  #else
    object
    { re_Base
      transform { re_Place }
      #if (!RE_Split_union) split_union off #end
    }
  #end
#end

//----------------------------------------------------------

#macro RE_Unequal_end (A, B, rSide, rEnd, Use_merge, v_End)
  #local re_This = "RE_Unequal_end"
  RE__check_unequal (re_This, rSide, rEnd)
  #local re_Axis = RE__check_axis (re_This, "end", v_End);
  #local re_Rotn = re__Unequals1 [re_Axis + 3];
  #local re_Place = transform { rotate re_Rotn translate (A + B) / 2 }
  #local re_v_Size = RE__vabs (vrotate (A - B, re_Rotn));
  RE__check_unequal_gt
  ( re_This, "side", min (re_v_Size.x, re_v_Size.z), rSide, 2
  )
  RE__check_unequal_gt
  ( re_This, "end", min (re_v_Size.x, re_v_Size.z), rEnd, 2
  )
  RE__check_unequal_gt (re_This, "end", re_v_Size.y, rEnd, 1)
  #local re_v_Out = re_v_Size / 2; //corner of containing box
  #local re_v_In = re_v_Out - <rSide, rEnd, rSide>; //center of corner torus
  #local re_RCorner = rSide - rEnd;
  #local re_xEdge = re_v_Out.x - rEnd;
  #local re_zEdge = re_v_Out.z - rEnd;
  #local re_Fuzz = (Use_merge? RE_ABIT: 0); //to avoid coincident surfaces
  #local re_Corner =
    #if (rSide = rEnd) sphere { re_v_In, rEnd } //degenerate torus
    #else object { RE__torus (rSide - rEnd, rEnd, Use_merge) translate re_v_In }
    #end
  #if (re_v_Out.y < rEnd & !Use_merge)
    #local re_Corner = intersection
    { object { re_Corner }
      plane { -y, re_v_Out.y }
    }
    #end
  #local re_Base = #if (Use_merge) merge #else union #end
  { box
    { -<re_xEdge, re_v_Out.y + re_Fuzz, re_v_In.z>,
      <re_xEdge, re_v_Out.y + re_Fuzz, re_v_In.z>
    }
    box
    { -<re_v_In.x, re_v_Out.y + re_Fuzz, re_zEdge>,
      <re_v_In.x, re_v_Out.y + re_Fuzz, re_zEdge>
    }
    object { re_Corner }
    RE__cylinder
    ( <re_v_In.x, re_v_Out.y + re_Fuzz, re_v_In.z>,
      <re_v_In.x, -re_v_Out.y - re_Fuzz, re_v_In.z>, re_RCorner
    )
    #if (re_v_In.x != 0)
      object { re_Corner scale <-1, 1, 1> }
      RE__clip_edge
      ( <re_v_In.x, re_v_In.y, re_zEdge>,
        <-re_v_In.x, re_v_In.y, re_zEdge>, rEnd, re_v_Out.y, Use_merge
      )
      #if (re_zEdge != 0)
        RE__clip_edge
        ( <re_v_In.x, re_v_In.y, -re_zEdge>,
          <-re_v_In.x, re_v_In.y, -re_zEdge>, rEnd, re_v_Out.y, Use_merge
        )
      #end
      RE__cylinder
      ( <-re_v_In.x, re_v_Out.y + re_Fuzz, re_v_In.z>,
        <-re_v_In.x, -re_v_Out.y - re_Fuzz, re_v_In.z>, re_RCorner
      )
    #end
    #if (re_v_In.z != 0)
      object { re_Corner scale <1, 1, -1> }
      RE__clip_edge
      ( <re_xEdge, re_v_In.y, re_v_In.z>,
        <re_xEdge, re_v_In.y, -re_v_In.z>, rEnd, re_v_Out.y, Use_merge
      )
      #if (re_xEdge != 0)
        RE__clip_edge
        ( <-re_xEdge, re_v_In.y, re_v_In.z>,
          <-re_xEdge, re_v_In.y, -re_v_In.z>, rEnd, re_v_Out.y, Use_merge
        )
      #end
      RE__cylinder
      ( <re_v_In.x, re_v_Out.y + re_Fuzz, -re_v_In.z>,
        <re_v_In.x, -re_v_Out.y - re_Fuzz, -re_v_In.z>, re_RCorner
      )
    #end
    #if (re_v_In.x != 0 & re_v_In.z != 0)
      object { re_Corner scale <-1, 1, -1> }
      RE__cylinder
      ( <-re_v_In.x, re_v_Out.y + re_Fuzz, -re_v_In.z>,
        <-re_v_In.x, -re_v_Out.y - re_Fuzz, -re_v_In.z>, re_RCorner
      )
    #end
    #if (re_v_Size.y > rEnd)
      #if (re_v_In.x > 0)
        box
        { -<re_v_In.x, re_v_Out.y + re_Fuzz, re_v_Out.z>,
          <re_v_In.x, re_v_In.y, re_v_Out.z>
        }
      #end
      #if (re_v_In.z > 0)
        box
        { -<re_v_Out.x, re_v_Out.y + re_Fuzz, re_v_In.z>,
          <re_v_Out.x, re_v_In.y, re_v_In.z>
        }
      #end
      RE__cylinder
      ( re_v_In,
        <re_v_In.x, -re_v_Out.y - re_Fuzz, re_v_In.z>, rSide
      )
      #if (re_v_In.x != 0)
        RE__cylinder
        ( re_v_In * <-1, 1, 1>,
          <-re_v_In.x, -re_v_Out.y - re_Fuzz, re_v_In.z>, rSide
        )
      #end
      #if (re_v_In.z != 0)
        RE__cylinder
        ( re_v_In * <1, 1, -1>,
          <re_v_In.x, -re_v_Out.y - re_Fuzz, -re_v_In.z>, rSide
        )
      #end
      #if (re_v_In.x != 0 & re_v_In.z != 0)
        RE__cylinder
        ( re_v_In * <-1, 1, -1>,
          <-re_v_In.x, -re_v_Out.y - re_Fuzz, -re_v_In.z>, rSide
        )
      #end
    #end
  }
  #if (Use_merge) //shear off the coincident surfaces
    intersection
    { object { re_Base }
      box
      { -<re_v_Out.x + re_Fuzz, re_v_Out.y, re_v_Out.z + re_Fuzz>,
        <re_v_Out.x + re_Fuzz, re_v_Out.y, re_v_Out.z + re_Fuzz>
      }
      transform { re_Place }
    }
  #else
    object
    { re_Base
      transform { re_Place }
      #if (!RE_Split_union) split_union off #end
    }
  #end
#end

//----------------------------------------------------------

#macro RE_Unequal_side (A, B, rSide, rEnd, Use_merge, v_Orientation, v_Side)
  #local re_This = "RE_Unequal_side"
  RE__check_unequal (re_This, rSide, rEnd)
  #local re_Axis = RE__check_axis (re_This, "orientation", v_Orientation);
  #local re_Side = RE__check_axis (re_This, "side", v_Side);
  #if (abs (re_Axis) = abs (re_Side))
    #error concat
    ( re_This, "(): The orientation (", RE__vstr (v_Orientation),
      ") and the side (", RE__vstr (v_Side), ") cannot be along the same axis."
    )
  #end
  #local re_Rotn = re__Unequals2 [re_Axis + 3] [re_Side + 3];
  #local re_Place = transform { rotate re_Rotn translate (A + B) / 2 }
  #local re_v_Size = RE__vabs (RE__unrotate (A - B, re_Rotn));
  RE__check_unequal_gt (re_This, "side", re_v_Size.x, rSide, 1)
  RE__check_unequal_gt (re_This, "side", re_v_Size.z, rSide, 2)
  RE__check_unequal_gt (re_This, "end", re_v_Size.x, rEnd, 1)
  RE__check_unequal_gt
  ( re_This, "end", min (re_v_Size.y, re_v_Size.z), rEnd, 2
  )
  #local re_v_Out = re_v_Size / 2; //corner of containing box
  #local re_v_In = re_v_Out - <rSide, rEnd, rSide>; //center of corner torus
  #local re_RCorner = rSide - rEnd;
  #local re_xEdge = re_v_Out.x - rEnd;
  #local re_zEdge = re_v_Out.z - rEnd;
  #local re_Fuzz = (Use_merge? RE_ABIT: 0); //to avoid coincident surfaces
  #local re_Short = (re_v_Out.x < rSide & !Use_merge); // clipping necessary?
  #local re_Clip = plane { -x, re_v_Out.x }
  #local re_Corner =
    #if (rSide = rEnd) sphere { re_v_In, rEnd } //degenerate torus
    #else object { RE__torus (rSide - rEnd, rEnd, Use_merge) translate re_v_In }
    #end
  #if (re_Short)
    #local re_Corner = intersection
    { object { re_Corner }
      object { re_Clip }
    }
  #end
  #local re_Base = #if (Use_merge) merge #else union #end
  { box
    { -<re_v_Out.x + re_Fuzz, re_v_Out.y + re_Fuzz, re_zEdge>,
      <re_v_In.x, re_v_Out.y + re_Fuzz, re_zEdge>
    }
    #if (re_v_In.z > 0 & re_xEdge > -re_v_Out.x)
      #local re_Left = re_v_In.x - RE_ABIT;
      box
      { < (re_Left > -re_v_Out.x? re_Left: -re_v_Out.x - re_Fuzz),
          -re_v_Out.y - re_Fuzz,
          -re_v_In.z
        >,
        <re_xEdge, re_v_Out.y + re_Fuzz, re_v_In.z>
      }
    #end
    object { re_Corner }
    RE__clipped_cylinder
    ( <re_v_In.x, re_v_Out.y + re_Fuzz, re_v_In.z>,
      <re_v_In.x, -re_v_Out.y - re_Fuzz, re_v_In.z>, re_RCorner,
      re_Short, re_Clip
    )
    #if (re_v_In.z != 0)
      object { re_Corner scale <1, 1, -1> }
      RE__clipped_cylinder
      ( <re_xEdge, re_v_In.y, re_v_In.z>,
        <re_xEdge, re_v_In.y, -re_v_In.z>, rEnd, re_Short, re_Clip
      )
      RE__clipped_cylinder
      ( <re_v_In.x, re_v_Out.y + re_Fuzz, -re_v_In.z>,
        <re_v_In.x, -re_v_Out.y - re_Fuzz, -re_v_In.z>, re_RCorner,
        re_Short, re_Clip
      )
    #end
    #if (re_v_Size.x > rSide)
      cylinder
      { <re_v_In.x, re_v_In.y, re_zEdge>,
        <-re_v_Out.x - re_Fuzz, re_v_In.y, re_zEdge>, rEnd
      }
      #if (re_zEdge != 0)
        cylinder
        { <re_v_In.x, re_v_In.y, -re_zEdge>,
          <-re_v_Out.x - re_Fuzz, re_v_In.y, -re_zEdge>, rEnd
        }
      #end
    #end
    #if (re_v_In.y != 0)
      box
      { -<re_v_Out.x + re_Fuzz, re_v_In.y, re_v_Out.z>,
        <re_v_In.x, re_v_In.y, re_v_Out.z>
      }
      box
      { -<re_v_Out.x + re_Fuzz, re_v_In.y, re_v_In.z>,
        <re_v_Out.x, re_v_In.y, re_v_In.z>
      }
      object { re_Corner scale <1, -1, 1> }
      RE__clipped_cylinder
      ( re_v_In, re_v_In * <1, -1, 1>, rSide, re_Short, re_Clip
      )
      #if (re_v_In.z != 0)
        object { re_Corner scale <1, -1, -1> }
        RE__clipped_cylinder
        ( re_v_In * <1, 1, -1>, re_v_In * <1, -1, -1>, rSide, re_Short, re_Clip
        )
        RE__clipped_cylinder
        ( <re_xEdge, -re_v_In.y, re_v_In.z>,
          <re_xEdge, -re_v_In.y, -re_v_In.z>, rEnd, re_Short, re_Clip
        )
      #end
      #if (re_v_Size.x > rSide)
        cylinder
        { <re_v_In.x, -re_v_In.y, re_zEdge>,
          <-re_v_Out.x - re_Fuzz, -re_v_In.y, re_zEdge>, rEnd
        }
        #if (re_zEdge != 0)
          cylinder
          { <re_v_In.x, -re_v_In.y, -re_zEdge>,
            <-re_v_Out.x - re_Fuzz, -re_v_In.y, -re_zEdge>, rEnd
          }
        #end
      #end
    #end
  }
  #if (Use_merge) //shear off the coincident surfaces
    intersection
    { object { re_Base }
      box
      { -<re_v_Out.x, re_v_Out.y, re_v_Out.z + re_Fuzz>,
        <re_v_Out.x + re_Fuzz, re_v_Out.y, re_v_Out.z + re_Fuzz>
      }
      transform { re_Place }
    }
  #else
    object
    { re_Base
      transform { re_Place }
      #if (!RE_Split_union) split_union off #end
    }
  #end
#end

//----------------------------------------------------------

#macro RE_Unequal_side_end (A, B, rSide, rEnd, Use_merge, v_End, v_Side)
  #local re_This = "RE_Unequal_side_end"
  RE__check_unequal (re_This, rSide, rEnd)
  #local re_Axis = RE__check_axis (re_This, "end", v_End);
  #local re_Side = RE__check_axis (re_This, "side", v_Side);
  #if (abs (re_Axis) = abs (re_Side))
    #error concat
    ( re_This, "(): The end (", RE__vstr (v_End),
      ") and the side (", RE__vstr (v_Side), ") cannot be along the same axis."
    )
  #end
  #local re_Rotn = re__Unequals2 [re_Axis + 3] [re_Side + 3];
  #local re_Place = transform { rotate re_Rotn translate (A + B) / 2 }
  #local re_v_Size = RE__vabs (RE__unrotate (A - B, re_Rotn));
  RE__check_unequal_gt (re_This, "side", re_v_Size.x, rSide, 1)
  RE__check_unequal_gt (re_This, "side", re_v_Size.z, rSide, 2)
  RE__check_unequal_gt
  ( re_This, "end", min (re_v_Size.x, re_v_Size.y), rEnd, 1
  )
  RE__check_unequal_gt (re_This, "end", re_v_Size.z, rEnd, 2)
  #local re_v_Out = re_v_Size / 2; //corner of containing box
  #local re_v_In = re_v_Out - <rSide, rEnd, rSide>; //center of corner torus
  #local re_RCorner = rSide - rEnd;
  #local re_xEdge = re_v_Out.x - rEnd;
  #local re_zEdge = re_v_Out.z - rEnd;
  #local re_Fuzz = (Use_merge? RE_ABIT: 0); //to avoid coincident surfaces
  //Is clipping necessary?
  #local re_Short = ((re_v_Out.x < rSide | re_v_Out.y < rEnd) & !Use_merge);
  #local re_Clip = box
  { -re_v_Out - <0, 0, RE_ABIT>, re_v_Out + RE_ABIT
  }
  #local re_Corner =
    #if (rSide = rEnd) sphere { re_v_In, rEnd } //degenerate torus
    #else object { RE__torus (rSide - rEnd, rEnd, Use_merge) translate re_v_In }
    #end
  #if (re_Short)
    #local re_Corner = intersection
    { object { re_Corner }
      object { re_Clip }
    }
  #end
  #local re_Base = #if (Use_merge) merge #else union #end
  { box
    { -<re_v_Out.x + re_Fuzz, re_v_Out.y - re_Fuzz, re_zEdge>,
      <re_v_In.x, re_v_Out.y + re_Fuzz, re_zEdge>
    }
    #if (re_v_In.z > 0 & re_xEdge > -re_v_Out.x)
      #local re_Left = re_v_In.x - RE_ABIT;
      box
      { < (re_Left > -re_v_Out.x? re_Left: -re_v_Out.x - re_Fuzz),
          -re_v_Out.y - re_Fuzz,
          -re_v_In.z
        >,
        <re_xEdge, re_v_Out.y + re_Fuzz, re_v_In.z>
      }
    #end
    object { re_Corner }
    RE__clipped_cylinder
    ( <re_v_In.x, re_v_Out.y + re_Fuzz, re_v_In.z>,
      <re_v_In.x, -re_v_Out.y - RE_ABIT, re_v_In.z>, re_RCorner,
      !Use_merge, re_Clip
    )
    #if (re_v_In.z != 0)
      object { re_Corner scale <1, 1, -1> }
      RE__clipped_cylinder
      ( <re_xEdge, re_v_In.y, re_v_In.z>,
        <re_xEdge, re_v_In.y, -re_v_In.z>, rEnd, re_Short, re_Clip
      )
      RE__clipped_cylinder
      ( <re_v_In.x, re_v_Out.y + re_Fuzz, -re_v_In.z>,
        <re_v_In.x, -re_v_Out.y - RE_ABIT, -re_v_In.z>, re_RCorner,
        !Use_merge, re_Clip
      )
    #end
    #if (re_v_Size.x > rSide)
      RE__clipped_cylinder
      ( <re_v_In.x, re_v_In.y, re_zEdge>,
        <-re_v_Out.x - RE_ABIT, re_v_In.y, re_zEdge>, rEnd,
        !Use_merge, re_Clip
      )
      #if (re_zEdge != 0)
        RE__clipped_cylinder
        ( <re_v_In.x, re_v_In.y, -re_zEdge>,
          <-re_v_Out.x - RE_ABIT, re_v_In.y, -re_zEdge>, rEnd,
          !Use_merge, re_Clip
        )
      #end
    #end
    #if (re_v_In.y != 0)
      box
      { -<re_v_Out.x + re_Fuzz, re_v_Out.y + re_Fuzz, re_v_Out.z>,
        <re_v_In.x, re_v_In.y, re_v_Out.z>
      }
      box
      { -<re_v_Out.x + re_Fuzz, re_v_Out.y + re_Fuzz, re_v_In.z>,
        <re_v_Out.x, re_v_In.y, re_v_In.z>
      }
      RE__clipped_cylinder
      ( re_v_In, <re_v_In.x, -re_v_Out.y - RE_ABIT, re_v_In.z>,
        rSide, !Use_merge, re_Clip
      )
      #if (re_v_In.z != 0)
        RE__clipped_cylinder
        ( re_v_In * <1, 1, -1>, <re_v_In.x, -re_v_Out.y - RE_ABIT, -re_v_In.z>,
          rSide, !Use_merge, re_Clip
        )
      #end
    #end
  }
  #if (Use_merge) //shear off the coincident surfaces
    intersection
    { object { re_Base }
      box
      { -<re_v_Out.x, re_v_Out.y, re_v_Out.z + re_Fuzz>,
        <re_v_Out.x + re_Fuzz, re_v_Out.y, re_v_Out.z + re_Fuzz>
      }
      transform { re_Place }
    }
  #else
    object
    { re_Base
      transform { re_Place }
      #if (!RE_Split_union) split_union off #end
    }
  #end
#end

//----------------------------------------------------------

#macro RE_Unequal_edge (A, B, rSide, rEnd, Use_merge, v_Orientation, v_Edge)
  #local re_This = "RE_Unequal_edge"
  RE__check_unequal (re_This, rSide, rEnd)
  #local re_Axis = RE__check_axis (re_This, "orientation", v_Orientation);
  #if (RE__count_axes (v_Edge) != 2)
    #error concat
    ( re_This, "(): The edge (", RE__vstr (v_Edge),
      ") must have exactly 2 non-zero axes."
    )
  #end
  #if (!VZero (v_Orientation * v_Edge))
    #error concat
    ( re_This, "(): The orientation (", RE__vstr (v_Orientation),
      ") must not be one of the edge axes (",
      RE__vstr (v_Edge), ")."
    )
  #end
  #local re_Rotn = re__Unequals3 [RE__index_axes (v_Edge)] [re_Axis + 3];
  #local re_Place = transform { rotate re_Rotn translate (A + B) / 2 }
  #local re_v_Size = RE__vabs (RE__unrotate (A - B, re_Rotn));
  RE__check_unequal_gt
  ( re_This, "side", min (re_v_Size.x, re_v_Size.z), rSide, 1
  )
  RE__check_unequal_gt
  ( re_This, "end", min (re_v_Size.x, re_v_Size.z), rEnd, 1
  )
  RE__check_unequal_gt (re_This, "end", re_v_Size.y, rEnd, 2)
  #local re_v_Out = re_v_Size / 2; //corner of containing box
  #local re_v_In = re_v_Out - <rSide, rEnd, rSide>; //center of corner torus
  #local re_RCorner = rSide - rEnd;
  #local re_xEdge = re_v_Out.x - rEnd;
  #local re_zEdge = re_v_Out.z - rEnd;
  #local re_Fuzz = (Use_merge? RE_ABIT: 0); //to avoid coincident surfaces
  // Is clipping necessary?
  #local re_Short = ((re_v_Out.x < rSide | re_v_Out.z < rSide) & !Use_merge);
  #local re_Clip = box
  { -re_v_Out - <0, 1, 1> * RE_ABIT, re_v_Out + <1, 1, 0> * RE_ABIT
  }
  #local re_xClip = box
  { -re_v_Out - <0, 1, 1> * RE_ABIT, re_v_Out + RE_ABIT
  }
  #local re_zClip = box
  { -re_v_Out - RE_ABIT, re_v_Out + <1, 1, 0> * RE_ABIT
  }
  #local re_Corner =
    #if (rSide = rEnd)
      sphere { re_v_In * <1, 1, -1>, rEnd } //degenerate torus
    #else
      object
      { RE__torus (rSide - rEnd, rEnd, Use_merge)
        translate re_v_In * <1, 1, -1>
      }
    #end
  #if (re_Short)
    #local re_Corner = intersection
    { object { re_Corner }
      object { re_Clip }
    }
  #end
  #local re_Base = #if (Use_merge) merge #else union #end
  { #if (rSide < re_v_Out.x * 2)
      box
      { -<re_v_Out.x + re_Fuzz, re_v_Out.y + re_Fuzz, re_zEdge>,
        <re_v_In.x, re_v_Out.y + re_Fuzz, re_v_Out.z + re_Fuzz>
      }
      box
      { -<re_v_Out.x + re_Fuzz, re_v_In.y, re_v_Out.z>,
        <re_v_In.x, re_v_In.y, re_v_Out.z + re_Fuzz>
      }
      RE__clipped_cylinder
      ( <re_v_In.x, re_v_In.y, -re_zEdge>,
        <-re_v_Out.x - re_Fuzz, re_v_In.y, -re_zEdge>, rEnd, re_Short, re_zClip
      )
      #if (re_v_In.y != 0)
        RE__clipped_cylinder
        ( <re_v_In.x, -re_v_In.y, -re_zEdge>,
          <-re_v_Out.x - re_Fuzz, -re_v_In.y, -re_zEdge>,
          rEnd, re_Short, re_zClip
        )
      #end
    #end
    #if (rSide < re_v_Out.z * 2)
      #local re_Left = re_v_In.x - RE_ABIT;
      box
      { < (re_Left > -re_v_Out.x? re_Left: -re_v_Out.x - re_Fuzz),
          -re_v_Out.y - re_Fuzz,
          -re_v_In.z
        >,
        <re_xEdge, re_v_Out.y + re_Fuzz, re_v_Out.z + re_Fuzz>
      }
      box
      { -<re_v_Out.x + re_Fuzz, re_v_In.y, re_v_In.z>,
        <re_v_Out.x, re_v_In.y, re_v_Out.z + re_Fuzz>
      }
      RE__clipped_cylinder
      ( <re_xEdge, re_v_In.y, re_v_Out.z + re_Fuzz>,
        <re_xEdge, re_v_In.y, -re_v_In.z>, rEnd, re_Short, re_xClip
      )
      #if (re_v_In.y != 0)
        RE__clipped_cylinder
        ( <re_xEdge, -re_v_In.y, re_v_Out.z + re_Fuzz>,
          <re_xEdge, -re_v_In.y, -re_v_In.z>, rEnd, re_Short, re_xClip
        )
      #end
    #end
    #if (re_v_In.y != 0)
      RE__clipped_cylinder
      ( re_v_In * <1, 1, -1>, re_v_In * <1, -1, -1>, rSide, re_Short, re_Clip
      )
      object { re_Corner scale <1, -1, 1> }
    #end
    RE__clipped_cylinder
    ( <re_v_In.x, re_v_Out.y + re_Fuzz, -re_v_In.z>,
      <re_v_In.x, -re_v_Out.y - re_Fuzz, -re_v_In.z>, re_RCorner,
      re_Short, re_Clip
    )
    object { re_Corner }
  }
  #if (Use_merge) //shear off the coincident surfaces
    intersection
    { object { re_Base }
      box { -re_v_Out - re_Fuzz * z, re_v_Out + re_Fuzz * x }
      transform { re_Place }
    }
  #else
    object
    { re_Base
      transform { re_Place }
      #if (!RE_Split_union) split_union off #end
    }
  #end
#end

//----------------------------------------------------------

#macro RE_Unequal_corner (A, B, rSide, rEnd, Use_merge, v_End, v_Corner)
  #local re_This = "RE_Unequal_corner"
  RE__check_unequal (re_This, rSide, rEnd)
  #local re_Axis = RE__check_axis (re_This, "end", v_End);
  #if (RE__count_axes (v_Corner) != 3)
    #error concat
    ( re_This, "(): The corner (", RE__vstr (v_Corner),
      ") must be non-zero for all three coordinates."
    )
  #end
  #local re_Match = v_End * v_Corner;
  #if (re_Match.x < 0 | re_Match.y < 0 | re_Match.z < 0)
    #error concat
    ( re_This, "(): The end (", RE__vstr (v_End),
      ") must match the corresponding axis of the corner (",
      RE__vstr (v_Corner), ")."
    )
  #end
  #local re_Edge = RE__vsgn (v_Corner) - RE__vsgn (v_End);
  #local re_Rotn = re__Unequals3 [RE__index_axes (re_Edge)] [re_Axis + 3];
  #local re_Place = transform { rotate re_Rotn translate (A + B) / 2 }
  #local re_v_Size = RE__vabs (RE__unrotate (A - B, re_Rotn));
  RE__check_unequal_gt
  ( re_This, "side", min (re_v_Size.x, re_v_Size.z), rSide, 1
  )
  RE__check_unequal_gt (re_This, "end", VMin (re_v_Size), rEnd, 1)
  #local re_v_Out = re_v_Size / 2; //corner of containing box
  #local re_v_In = re_v_Out - <rSide, rEnd, rSide>; //center of corner torus
  #local re_RCorner = rSide - rEnd;
  #local re_xEdge = re_v_Out.x - rEnd;
  #local re_zEdge = re_v_Out.z - rEnd;
  #local re_Fuzz = (Use_merge? RE_ABIT: 0); //to avoid coincident surfaces
  #local re_Short = // Is clipping necessary?
  ( (re_v_Out.x < rSide | re_v_Out.y < rEnd | re_v_Out.z < rSide) &
    !Use_merge
  );
  #local re_Clip = box
  { -re_v_Out - <0, 0, 1> * RE_ABIT, re_v_Out + <1, 1, 0> * RE_ABIT
  }
  #local re_xyClip = box
  { -re_v_Out - <0, 0, 1> * RE_ABIT, re_v_Out + <1, 1, 1> * RE_ABIT
  }
  #local re_yzClip = box
  { -re_v_Out - <1, 0, 1> * RE_ABIT, re_v_Out + <1, 1, 0> * RE_ABIT
  }
  #local re_xzClip = box
  { -re_v_Out - <0, 1, 1> * RE_ABIT, re_v_Out + <1, 1, 0> * RE_ABIT
  }
  #local re_Corner =
    #if (rSide = rEnd)
      sphere { re_v_In * <1, 1, -1>, rEnd } //degenerate torus
    #else
      object
      { RE__torus (rSide - rEnd, rEnd, Use_merge)
        translate re_v_In * <1, 1, -1>
      }
    #end
  #if (re_Short)
    #local re_Corner = intersection
    { object { re_Corner }
      object { re_Clip }
    }
  #end
  #local re_Base = #if (Use_merge) merge #else union #end
  { #if (rSide < re_v_Out.x * 2)
      box
      { -<re_v_Out.x + re_Fuzz, re_v_Out.y + re_Fuzz, re_zEdge>,
        <re_v_In.x, re_v_Out.y + re_Fuzz, re_v_Out.z + re_Fuzz>
      }
      box
      { -<re_v_Out.x + re_Fuzz, re_v_Out.y + re_Fuzz, re_v_Out.z>,
        <re_v_In.x, re_v_In.y, re_Fuzz - re_zEdge>
      }
      RE__clipped_cylinder
      ( <re_v_In.x, re_v_In.y, -re_zEdge>,
        <-re_v_Out.x - re_Fuzz, re_v_In.y, -re_zEdge>, rEnd, re_Short, re_yzClip
      )
    #end
    #if (rSide < re_v_Out.z * 2)
      #local re_Left = re_v_In.x - RE_ABIT;
      box
      { < (re_Left > -re_v_Out.x? re_Left: -re_v_Out.x - re_Fuzz),
          -re_v_Out.y - re_Fuzz,
          -re_v_In.z
        >,
        <re_xEdge, re_v_Out.y + re_Fuzz, re_v_Out.z + re_Fuzz>
      }
      box
      { <re_xEdge.x - re_Fuzz, -re_v_Out.y - re_Fuzz, -re_v_In.z>,
        <re_v_Out.x, re_v_In.y, re_v_Out.z + re_Fuzz>
      }
      RE__clipped_cylinder
      ( <re_xEdge, re_v_In.y, re_v_Out.z + re_Fuzz>,
        <re_xEdge, re_v_In.y, -re_v_In.z>, rEnd, re_Short, re_xyClip
      )
    #end
    RE__clipped_cylinder
    ( <re_v_In.x, re_v_In.y, -re_v_In.z>,
      <re_v_In.x, -re_v_Out.y - re_Fuzz, -re_v_In.z>, rSide,
      re_Short, re_xzClip
    )
    RE__clipped_cylinder
    ( <re_v_In.x, re_v_Out.y + re_Fuzz, -re_v_In.z>,
      <re_v_In.x, re_v_In.y - re_Fuzz, -re_v_In.z>, re_RCorner,
      re_Short, re_Clip
    )
    object { re_Corner }
  }
  #if (Use_merge) //shear off the coincident surfaces
    intersection
    { object { re_Base }
      box { -re_v_Out - re_Fuzz * <0, 0, 1>, re_v_Out + re_Fuzz * <1, 0, 0> }
      transform { re_Place }
    }
  #else
    object
    { re_Base
      transform { re_Place }
      #if (!RE_Split_union) split_union off #end
    }
  #end
#end

//========================= ROUNDED CYLINDER MACROS ============================

#macro RE_Cylinder (A, B, ROuter, rEdge, Use_merge)
  #local re_This = "RE_Cylinder"
  #local re_Axis = RE__check_cyl (re_This, A, B);
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "axis length", 0, re_Axis, rEdge, 2)
 //boundary cases:
  #if (2 * rEdge = re_Axis & rEdge = ROuter)
    sphere { (A + B) / 2, rEdge }
  #else #if (2 * rEdge = re_Axis & rEdge < ROuter)
    #if (Use_merge) merge #else union #end
    { cylinder { A, B, ROuter - rEdge }
      object
      { RE__torus (ROuter - rEdge, rEdge, Use_merge)
        Point_At_Trans (B - A) translate (A + B) / 2
      }
      #if (!Use_merge & !RE_Split_union) split_union off #end
    }
  #else #if (2 * rEdge != re_Axis & rEdge = ROuter)
    #if (Use_merge) merge #else union #end
    { sphere { rEdge * y, rEdge }
      sphere { (re_Axis - rEdge) * y, rEdge }
      cylinder { rEdge * y, (re_Axis - rEdge) * y, ROuter }
      Point_At_Trans (B - A) translate A
      #if (!Use_merge & !RE_Split_union) split_union off #end
    }
 //general case:
  #else
    RE__check_r_gt (re_This, "outer radius", 0, ROuter, rEdge, 1)
    #if (Use_merge) merge #else union #end
    { cylinder { rEdge * y, (re_Axis - rEdge) * y, ROuter }
      cylinder { 0, re_Axis * y, ROuter - rEdge }
      object
      { RE__torus (ROuter - rEdge, rEdge, Use_merge)
        translate rEdge * y
      }
      object
      { RE__torus (ROuter - rEdge, rEdge, Use_merge)
        translate (re_Axis - rEdge) * y
      }
      Point_At_Trans (B - A) translate A
      #if (!Use_merge & !RE_Split_union) split_union off #end
    }
  #end
  #end
  #end
#end

//----------------------------------------------------------

#macro RE_Cylinder_end (A, B, ROuter, rEdge, Use_merge)

  #local re_This = "RE_Cylinder_end"
  #local re_Axis = RE__check_cyl (re_This, A, B);
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "axis length", 0, re_Axis, rEdge, 1)
  RE__check_r_gt (re_This, "radius", 0, ROuter, rEdge, 1)

  #if (re_Axis >= 2 * rEdge)
    #if (Use_merge) merge #else union #end
    { cylinder { rEdge * y, re_Axis * y, ROuter }
      #if (rEdge = ROuter)
        sphere { y, 1 scale rEdge } //degenerate torus
      #else
        cylinder { 0, rEdge * RE_MORE * y, ROuter - rEdge }
        object
        { RE__torus (ROuter - rEdge, rEdge, Use_merge) translate rEdge * y
        }
      #end
      Point_At_Trans (B - A) translate A
      #if (!Use_merge & !RE_Split_union) split_union off #end
    }
  #else
    intersection
    { #if (re_Axis <= rEdge & rEdge = ROuter) object
      #else #if (Use_merge) merge
      #else union
      #end
      #end
      { #if (re_Axis > rEdge) cylinder { rEdge * y, re_Axis * y, ROuter } #end
        #if (rEdge = ROuter)
          sphere { y, 1 scale rEdge } //degenerate torus
        #else
          cylinder { 0, rEdge * 2 * y, ROuter - rEdge }
          object
          { RE__torus (ROuter - rEdge, rEdge, Use_merge) translate rEdge * y
          }
        #end
      }
      plane { y, re_Axis }
      Point_At_Trans (B - A) translate A
    }
  #end
#end

//----------------------------------------------------------

// This auxilliary macro should not be called by the user.
#macro RE__hole (A, B, ROuter, RInner, rEdge, Use_merge)
  #local re_Axis = vlength (A - B);
  #local re_Base = #if (Use_merge) merge #else union #end
  { difference
    { cylinder { 0, re_Axis * y, max (ROuter, RInner + 2*rEdge) }
      cylinder { -RE_ABIT * y, re_Axis * RE_MORE * y, RInner + rEdge }
    }
    #if (re_Axis != 2*rEdge)
      difference
      { cylinder { rEdge * y, (re_Axis - rEdge) * y, RInner + rEdge * RE_MORE }
        cylinder
        { rEdge * RE_LESS * y, (re_Axis - rEdge * RE_LESS) * y, RInner
        }
      }
      torus { RInner + rEdge, rEdge translate (re_Axis - rEdge) * y }
    #end
    torus { RInner + rEdge, rEdge translate rEdge * y }
    #if (!Use_merge & !RE_Split_union) split_union off #end
  }
  #if (ROuter < RInner + 2*rEdge)
    #local re_Base = intersection
    { object { re_Base }
      cylinder { -RE_ABIT * y, re_Axis * RE_MORE * y, ROuter }
    }
  #end
  object { re_Base Point_At_Trans (B - A) translate A }
#end

//----- Public Macros -----

#macro RE_Hole (A, B, ROuter, RInner, rEdge, Use_merge)
  #local re_Axis = vlength (A - B);
  #local re_This = "RE_Hole"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "axis length", 0, re_Axis, rEdge, 2)
  RE__check3 (re_This, RInner, ROuter, rEdge, 1)
  #if (re_Axis = 0) RE__error0 (re_This, "axis length") #end
  RE__hole (A, B, ROuter, RInner, rEdge, Use_merge)
#end

#macro RE_Hole_minimal (A, B, RInner, rEdge, Use_merge)
  #local re_Axis = vlength (A - B);
  #local re_This = "RE_Hole_minimal"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "axis length", 0, re_Axis, rEdge, 2)
  #if (re_Axis = 0) RE__error0 (re_This, "axis length") #end
  RE__hole (A, B, (RInner + rEdge) * sqrt(2), RInner, rEdge, Use_merge)
#end

//----------------------------------------------------------

// This auxilliary macro should not be called by the user.
#macro RE__hole_end (A, B, ROuter, RInner, rEdge, Use_merge)
  #local re_Axis = vlength (A - B);
  #local re_Extent = (ROuter < RInner + 2*rEdge | Use_merge?
                      re_Axis * RE_MORE: max (re_Axis, 2*rEdge));
  #local re_Base = #if (Use_merge) merge #else union #end
  { difference
    { cylinder { 0, re_Extent * y, max (ROuter, RInner + 2*rEdge) }
      cylinder { -RE_ABIT * y, re_Extent * RE_MORE * y, RInner + rEdge }
    }
    #if (re_Axis > rEdge)
      difference
      { cylinder { rEdge * y, re_Extent * y, RInner + rEdge * RE_MORE }
        cylinder { rEdge * RE_LESS * y, re_Extent * RE_MORE * y, RInner }
      }
    #end
    torus { RInner + rEdge, rEdge translate rEdge * y }
    #if (!Use_merge & !RE_Split_union) split_union off #end
  }
  #if (ROuter < RInner + 2*rEdge)
    #local re_Base = intersection
    { object { re_Base }
      cylinder { -RE_ABIT * y, re_Axis * y, ROuter }
    }
  #else #if (re_Axis < 2*rEdge | Use_merge)
    #local re_Base = intersection { object { re_Base } plane { y, re_Axis } }
  #end
  #end
  object { re_Base Point_At_Trans (B - A) translate A }
#end

//----- Public Macros -----

#macro RE_Hole_end (A, B, ROuter, RInner, rEdge, Use_merge)
  #local re_Axis = vlength (A - B);
  #local re_This = "RE_Hole_end"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "axis length", 0, re_Axis, rEdge, 1)
  RE__check3 (re_This, RInner, ROuter, rEdge, 1)
  #if (re_Axis = 0) RE__error0 (re_This, "axis length") #end
  RE__hole_end (A, B, ROuter, RInner, rEdge, Use_merge)
#end

#macro RE_Hole_end_minimal (A, B, RInner, rEdge, Use_merge)
  #local re_Axis = vlength (A - B);
  #local re_This = "RE_Hole_end_minimal"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "axis length", 0, re_Axis, rEdge, 1)
  #if (re_Axis = 0) RE__error0 (re_This, "axis length") #end
  RE__hole_end (A, B, (RInner + rEdge) * sqrt(2), RInner, rEdge, Use_merge)
#end

//----------------------------------------------------------

#macro RE_Washer (A, B, ROuter, RInner, rEdge, Use_merge)

  #local re_Axis = vlength (A - B);
  #local re_This = "RE_Washer"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "axis length", 0, re_Axis, rEdge, 2)
  RE__check3 (re_This, RInner, ROuter, rEdge, 2)
  #if (re_Axis = 0) RE__error0 (re_This, "axis length") #end

  #if (Use_merge) merge #else union #end
  { #if (re_Axis != 2*rEdge)
      difference
      { cylinder { rEdge * y, (re_Axis - rEdge) * y, ROuter }
        cylinder
        { rEdge * RE_LESS * y, (re_Axis - rEdge * RE_LESS) * y, RInner
        }
      }
      torus { ROuter - rEdge, rEdge translate (re_Axis - rEdge) * y }
    #end
    #if (ROuter != RInner + 2*rEdge)
      difference
      { cylinder { 0, re_Axis * y, ROuter - rEdge }
        cylinder { -RE_ABIT * y, re_Axis * RE_MORE * y, RInner + rEdge }
      }
      torus { RInner + rEdge, rEdge translate rEdge * y }
    #end
    #if (re_Axis != 2*rEdge & ROuter != RInner + 2*rEdge)
      torus { RInner + rEdge, rEdge translate (re_Axis - rEdge) * y }
    #end
    torus { ROuter - rEdge, rEdge translate rEdge * y }
    Point_At_Trans (B - A)
    translate A
    #if (!Use_merge & !RE_Split_union) split_union off #end
  }
#end

//----------------------------------------------------------

#macro RE_Washer_end (A, B, ROuter, RInner, rEdge, Use_merge)

  #local re_Axis = vlength (A - B);
  #local re_Extent = max (re_Axis, 2*rEdge);
  #local re_This = "RE_Washer_end"
  RE__check_r (re_This, rEdge)
  RE__check_r_gt (re_This, "axis length", 0, re_Axis, rEdge, 1)
  RE__check3 (re_This, RInner, ROuter, rEdge, 2)
  #if (re_Axis = 0) RE__error0 (re_This, "axis length") #end

  #local re_Base = #if (Use_merge) merge #else union #end
  { #if (re_Axis > rEdge)
    difference
    { cylinder { rEdge * y, re_Extent * y, ROuter }
      cylinder { rEdge * RE_LESS * y, re_Extent * RE_MORE * y, RInner }
    }
    #end
    #if (ROuter != RInner + 2*rEdge)
      difference
      { cylinder { 0, RE_MORE * rEdge * y, ROuter - rEdge }
        cylinder { -RE_ABIT * y, RE_MORE * RE_MORE * rEdge * y, RInner + rEdge }
      }
      torus { RInner + rEdge, rEdge translate rEdge * y }
    #end
    torus { ROuter - rEdge, rEdge translate rEdge * y }
    Point_At_Trans (B - A)
    translate A
    #if (!Use_merge & !RE_Split_union) split_union off #end
  }
  #if (re_Axis < 2*rEdge)
    intersection
    { object { re_Base }
      plane { y, re_Axis }
    }
  #else
    object { re_Base }
  #end
#end

//=========================== ISOSURFACE FUNCTIONS =============================

#declare RE_fn_Blob = function (RE_P0_Value, RE_P1_Blob)
{ select
  ( RE_P1_Blob - RE_P0_Value, 0,
    pow (1 + (1 - pow (RE_P0_Value + 1, 2)) / (pow (RE_P1_Blob + 1, 2) - 1), 2)
  )
}

#declare RE_fn_Blob2 = function (RE_P0_Value, RE_P1_Blob)
{ select
  ( RE_P1_Blob - RE_P0_Value, 0,
    pow ((RE_P1_Blob - RE_P0_Value) / RE_P1_Blob, 2)
  )
}

#declare RE_fn_Hole = function (x, y, z, RE_P0_RMajor, RE_P1_rMinor)
{ select
  ( pow(x,2) + pow(z,2) - pow(RE_P0_RMajor,2),
    f_torus (x, y, z, RE_P0_RMajor, RE_P1_rMinor),
    max (y - RE_P1_rMinor, -RE_P1_rMinor - y)
  )
}

#declare RE_fn_Parabolic_torus = function (x, y, z, RE_P0_H, RE_P1_R)
{ sqrt
  ( pow(z,2) + pow(y,2) - 2*RE_P0_H * y * pow(x,2) + pow (RE_P0_H * x*x, 2)
  ) - RE_P1_R
}

#declare RE_fn_Wheel = function (x, y, z, RE_P0_RMajor, RE_P1_rMinor)
{ select
  ( pow(x,2) + pow(z,2) - pow(RE_P0_RMajor,2),
    max (y - RE_P1_rMinor, -RE_P1_rMinor - y),
    f_torus (x, y, z, RE_P0_RMajor, RE_P1_rMinor)
  )
}

//============================= ISOSURFACE MACRO ===============================

#macro RE_Evaluate (Max_gradient, Min_factor, Attenuation)
  #local re_This = "RE_Evaluate"
  RE__check_neg0 (re_This, "Max_gradient", Max_gradient)
  RE__check_neg0 (re_This, "Min_factor", Min_factor)
  RE__check_neg0 (re_This, "Attenuation", Attenuation)
  #local re_MinMaxG = Max_gradient * Min_factor;
  #local re_Overest = sqrt (abs (Max_gradient / (Max_gradient * Min_factor)));
  #if (RE_Debug)
    #debug concat
    ( re_This, " (", RE__str (Max_gradient), " ", RE__str (Min_factor), " ",
      RE__str (Attenuation), "):\n"
    )
    #debug concat
    ( "  evaluate ", RE__str (re_MinMaxG), " ", RE__str (re_Overest), " ",
      RE__str (Attenuation), "\n"
    )
  #end
  evaluate re_MinMaxG, re_Overest, Attenuation
#end

//========================== BLOB ASSIST FUNCTIONS =============================

#declare RE_fn_Blob_surface_radius = function (RE_P0_RField, RE_P1_Strength)
{ RE_P0_RField * sqrt (1 - 1 / sqrt (RE_P1_Strength))
}

#declare RE_fn_Blob_field_radius = function (RE_P0_RSurface, RE_P1_Strength)
{ RE_P0_RSurface / sqrt (1 - 1 / sqrt (RE_P1_Strength))
}

#declare RE_fn_Blob_strength = function (RE_P0_RSurface, RE_P1_RField)
{ 1 / pow (1 - pow (RE_P0_RSurface / RE_P1_RField, 2), 2)
}

//--------------------- GRANDFATHER ------------------------

#declare RE_fn_Blob_distance = function (RE_P0_RField, RE_P1_Strength)
{ RE_fn_Blob_surface_radius (RE_P0_RField, RE_P1_Strength)
}

#declare RE_fn_Blob_radius = function (RE_P0_RSurface, RE_P1_Strength)
{ RE_fn_Blob_field_radius (RE_P0_RSurface, RE_P1_Strength)
}

//==============================================================================

#version RoundEdge_Inc_Temp;
#end
// end of roundedge.inc
