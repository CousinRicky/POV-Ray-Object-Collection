/* scarycoronavirus.inc 1.0B
 * Persistence of Vision Raytracer include file
 * POV-Ray Object Collection
 *
 * A Gorgon-like virion based on the SARS-CoV-2 agent that causes COVID-19.
 * The protein spikes have demonic-looking faces.
 *
 * References:
 *   https://phil.cdc.gov/Details.aspx?pid=23313
 *   https://science.sciencemag.org/content/367/6483/1260
 *   https://commons.wikimedia.org/wiki/File:6VSB_spike_protein_SARS-CoV-2_monomer_in_homotrimer.png
 * The size of the spikes in nanometers is extrapolated from the benzene rings
 * in the latter illustration (C-C bond at 0.139 nm).
 *
 * Disclaimer: I am not a microbiologist.
 *
 * Implementation note: due to POV-Ray namespace scope leakage, local variables
 * and function arguments (other than x, y, and z) *must* be prefixed.
 *
 * Copyright (C) 2020 - 2021 Richard Callwood III.  Some rights reserved.
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
 * Vers  Date         Notes
 * ----  ----         -----
 *       2020-Mar-28  Started.
 * 1.0   2020-Apr-15  Completed.
 * 1.0A  2020-May-09  Adapted for the POV-Ray Object Collection.
 * 1.0B  2021-Aug-15  The license text is updated.
 */
#ifndef (ScaryCoronavirus_Inc_Temp)
#declare ScaryCoronavirus_Inc_Temp = version;
#declare ScaryCoronavirus_Version = 1.0;
#version max (3.5, min (3.8, version)); // Bracket the POV version.
#ifdef (View_POV_Include_Stack)
  #debug "#include \"scarycoronavirus.inc\" //version 1.0B\n"
#end

#include "roundedge.inc"

//================== PUBLIC PARAMETERS AND REFERENCE VALUES ====================

#ifndef (scov_Debug) #declare scov_Debug = 0; #end
#ifndef (scov_Glow) #declare scov_Glow = 1; #end
#ifndef (scov_Seed) #declare scov_Seed = 574647; #end

#declare SCOV_DRAFT_QUALITY = 0;
#declare SCOV_BLOB_QUALITY = 1;
#declare SCOV_UNION_QUALITY = 2;
#declare SCOV_MERGE_QUALITY = 3;
#declare SCOV_HSPIKE = 12; // a guess; from spike top to envelope surface

//======================== PRIVATE/PROTECTED FEATURES ==========================
// These tools should not be referenced directly by the user.

//-------------------- SPIKE PROTEIN -----------------------

#local SCOV_LSPIKE = 16.7; // total spike length (see header comment)
#local SCOV_RSTALK = 3.25;
#local SCOV_RSPIKE = 5.85;
#local SCOV_RRBD = 1.25;
#local SCOV_RBUMP = 2.2;
#local SCOV_YBUMP = -7;
#local SCOV_BLOB = 1;
#local SCOV_RBD_BLOB = 1;
#local SCOV_BUMP_BLOB = 1.5;
#local scov_zBump = (SCOV_RSPIKE - SCOV_RBUMP) / cos (radians (30));
#local scov_Bump_strength =
  RE_fn_Blob_strength (SCOV_RBUMP, SCOV_RBUMP + SCOV_BUMP_BLOB);
#local scov_rTop = scov_zBump * sin (radians (30)) + SCOV_RBUMP;

#declare SCov__Spike = blob
{ cylinder
  { -SCOV_RRBD * y, -(SCOV_RRBD + 2) * y,
    SCOV_RRBD + SCOV_RBD_BLOB,
    RE_fn_Blob_strength (SCOV_RRBD, SCOV_RRBD + SCOV_RBD_BLOB)
    translate (scov_rTop - SCOV_RRBD) * z
  }
  #local scov_Az = 0;
  #while (scov_Az < 360)
    cylinder
    { -5.05 * y, <0, SCOV_YBUMP, 1>, SCOV_RBUMP + SCOV_BUMP_BLOB,
      scov_Bump_strength
      translate -scov_zBump * z
      rotate scov_Az * y
    }
    #local scov_Az = scov_Az + 120;
  #end
  sphere
  { -(scov_rTop + 1.1) * y, scov_rTop + SCOV_BLOB,
    RE_fn_Blob_strength (scov_rTop, scov_rTop + SCOV_BLOB)
  }
  cylinder
  { SCOV_YBUMP * y, (SCOV_RSTALK - SCOV_LSPIKE) * y, SCOV_RSTALK + SCOV_BLOB,
    RE_fn_Blob_strength (SCOV_RSTALK, SCOV_RSTALK + SCOV_BLOB)
  }
}

#local SCov_f_Glow = finish
{ #if (version < 3.7)
    ambient scov_Glow diffuse 0
  #else
    ambient 0 diffuse 0 emission scov_Glow
  #end
}

#local SCov_Eye = difference
{ prism
  { bezier_spline -4, 0, 8,
    <1.30, 1.45>, <2.30, 1.45>, <3.80, 2.20>, <3.80, 2.20>,
    <3.80, 2.20>, <3.55, 0.20>, <1.55, -0.05>, <1.30, 1.45>
  }
  object { SCov__Spike translate -0.001 * y }
  texture
  { pigment { rgb <1, 0.75, 0> }
    finish { SCov_f_Glow }
  }
}

#local SCov_Mouth = prism
{ bezier_spline -6, 0, 28,
  <-1.70, -3.90>, <-1.50, -1.90>, <-1.35, -2.80>, <-0.95, -2.00>,
  <-0.95, -2.00>, <-0.55, -1.20>, <-0.20, -1.60>, <0.00, -1.60>,
  <0.00, -1.60>, <0.20, -1.60>, <0.55, -1.20>, <0.95, -2.00>,
  <0.95, -2.00>, <1.35, -2.80>, <1.50, -1.90>, <1.70, -3.90>,
  <1.70, -3.90>, <1.10, -3.48>, <0.87, -3.90>, <0.42, -3.60>,
  <0.42, -3.60>, <0.18, -3.44>, <-0.18, -3.44>, <-0.42, -3.60>,
  <-0.42, -3.60>, <-0.87, -3.90>, <-1.10, -3.48>, <-1.70, -3.90>
  texture
  { pigment { rgb <0.1, 0, 0> }
    finish { SCov_f_Glow }
  }
}

#declare SCov__Spike = difference
{ object { SCov__Spike }
  object { SCov_Eye }
  object { SCov_Eye scale <-1, 1, 1> }
  object { SCov_Mouth }
}

//---------------------- M PROTEIN -------------------------

#local SCOV_RMPROTEIN = 1.45;
#local SCOV_MHEAD_BLOB = SCOV_RMPROTEIN * 1.5;
#local SCOV_RMSTALK = SCOV_RMPROTEIN / 2;
#local SCOV_MSTALK_BLOB = (SCOV_RMSTALK + SCOV_RMPROTEIN / 2);

#declare SCov__MProtein = blob
{ sphere
  { (SCOV_RMPROTEIN - SCOV_HSPIKE) * y, SCOV_MHEAD_BLOB,
    RE_fn_Blob_strength (SCOV_RMPROTEIN, SCOV_MHEAD_BLOB)
  }
  cylinder
  { (SCOV_RMPROTEIN - SCOV_HSPIKE - SCOV_RMSTALK) * y,
    (SCOV_RMSTALK - SCOV_LSPIKE) * y,
    SCOV_MSTALK_BLOB, RE_fn_Blob_strength (SCOV_RMSTALK, SCOV_MSTALK_BLOB)
  }
}

//---------------------- E PROTEIN -------------------------

#local SCOV_REPROTEIN = 1.7;
#local SCOV_REWALL = SCOV_REPROTEIN * 0.42;
#local SCOV_REFILL = SCOV_REWALL * 0.5;
#local SCOV_EBLOB = 1.5;
#local scov_Eblob_strength = RE_fn_Blob_strength (1, SCOV_EBLOB);
#declare SCov__EProtein = blob
{ #local scov_I = 0;
  #while (scov_I < 359)
    cylinder
    { -SCOV_HSPIKE * y, (SCOV_REWALL - SCOV_LSPIKE) * y,
      SCOV_REWALL * SCOV_EBLOB, scov_Eblob_strength
      translate (SCOV_REPROTEIN - SCOV_REWALL) * z
      rotate scov_I * y
    }
    cylinder
    { -SCOV_HSPIKE * y, (SCOV_REWALL - SCOV_LSPIKE) * y,
      SCOV_REFILL * SCOV_EBLOB, scov_Eblob_strength
      translate (SCOV_REPROTEIN - SCOV_REFILL) * z
      rotate (scov_I + 45) * y
    }
    #local scov_I = scov_I + 90;
  #end
}

//---------------------- ASSEMBLAGE ------------------------

#declare SCOV__GOLDEN = (sqrt (5) - 1) / 2;
#declare scov__Rand = seed (scov_Seed);
// For the envelope; for reproducibility between Quality levels,
// this stream must be kept separate from the main stream:
#declare scov__ERand = seed (scov_Seed);

#declare SCOV__MINSIZE = 50; // minimum allowed diameter
#declare SCOV__RLUMP = 1.1; // lumpiness of envelope
#declare SCOV__LUMP_BLOB = SCOV__RLUMP * 0.35;
#declare SCOV__LAYER = 2; // half of envelope membrane thickness
#declare SCOV__SJITTER = 0.5; // amount of irregularity in spike placement
#declare SCOV__LUMP_JITTER = 1; // amount of irregularity in envelope lumps
#declare SCOV__BAND = 1 + SCOV__SJITTER; // collision avoidance optimization
#declare SCOV__NTRIES = 20; // number of avoidance tries, less 1
#declare SCOV__MSPACING = SCOV_RMPROTEIN * 3;

// Rounds to the nearest integer:
#declare SCov__fn_Round = function (x) { floor (x + 0.5) }
// Returns latitudinal angles that space points evenly over the surface of a
// sphere, given x's of a flat distribution:
#declare SCov__fn_Distribute = function (x) { degrees (acos (1 - x * 2)) }
// Returns the angular jitter space allotted for each of x points:
#declare SCov__fn_Jitter = function (x) { degrees (pi / sqrt (pi * x)) }
// Returns how far back or forward we should check, among previously placed 
// protein studs, for collision avoidance:
//  P0_R     - radius of envelope
//  P1_Angle - angular space, above and below, to consider
//  p2_N     - total number of protein studs to avoid
#declare SCov__fn_Band = function (scov_P0_R, scov_P1_Angle, scov_P2_N)
{ ceil
  (   pi * scov_P0_R * sin (radians (scov_P1_Angle))
    / SCov__fn_Jitter (scov_P2_N) * SCOV__BAND
  )
}

// Creates a blob envelope:
#macro SCov__Blob_envelope (R, N, Use_big_sphere)
  blob
  { #local scov_Strength =
      RE_fn_Blob_strength (SCOV__RLUMP, SCOV__RLUMP + SCOV__LUMP_BLOB);
    #local scov_rJitter = SCov__fn_Jitter (N) * SCOV__LUMP_JITTER;
    #local scov_I = 0;
    #while (scov_I < N)
      sphere
      { R * y, SCOV__RLUMP + SCOV__LUMP_BLOB, scov_Strength
        rotate <rand (scov__ERand) * scov_rJitter, rand (scov__ERand) * 360, 0>
        rotate
        < SCov__fn_Distribute ((scov_I + 0.5) / N),
          scov_I * SCOV__GOLDEN * 360,
          0
        >
      }
      #local scov_I = scov_I + 1;
    #end
    #if (Use_big_sphere)
      sphere
      { 0, R + SCOV__LUMP_BLOB, RE_fn_Blob_strength (R, R + SCOV__LUMP_BLOB)
      }
    #end
  }
#end

//=============================== PUBLIC MACRO =================================

#macro ScaryCoronavirus
( Diameter, t_Envelope, t_Spike, t_MProtein, t_EProtein, Quality
)
  #local scov_This =
    concat ("ScaryCoronavirus (", str (Diameter, 0, 6), ", ...)")
  #if (Diameter < SCOV__MINSIZE)
    #error concat
    ( scov_This, ": Diameter must be ", str (SCOV__MINSIZE, 0, 0),
      " or greater."
    )
  #end
  #local scov_rEnvelope = Diameter / 2 - SCOV_HSPIKE;
  //#local scov_rMidEnv = scov_rEnvelope - SCOV__LAYER;
  #local scov_rLumps = scov_rEnvelope - SCOV__RLUMP;
 // These numbers are all eyeballed estimates.  The integral
 // ratios of the divisors made testing of different methods
 // of collision avoidance easier, though such ratios are not
 // necessary for the algorithm that was ultimately selected.
  #local scov_nLumps = SCov__fn_Round (pow (scov_rEnvelope, 2) * 5);
  #local scov_nSs = SCov__fn_Round (scov_nLumps / 110);
  #local scov_nMs = SCov__fn_Round (scov_nLumps / 220);
  #local scov_nEs = ceil (scov_nLumps / 1980);
  #if (scov_Debug)
    #debug concat (scov_This, ":\n")
    #debug concat ("  scov_rEnvelope = ", str (scov_rEnvelope, 0, 6), "\n")
    #debug concat ("  scov_nLumps = ", str (scov_nLumps, 0, 0), "\n")
    #debug concat ("  scov_nEs = ", str (scov_nEs, 0, 0), "\n")
    #debug concat ("  scov_nMs = ", str (scov_nMs, 0, 0), "\n")
    #debug concat ("  scov_nSs = ", str (scov_nSs, 0, 0), "\n")
  #end
  union
  {//--------------------- envelope ------------------------
    #switch (Quality)
      #case (SCOV_BLOB_QUALITY)
        object
        { SCov__Blob_envelope (scov_rLumps, scov_nLumps, yes)
          texture { t_Envelope }
        }
        #break
      #case (SCOV_UNION_QUALITY)
        union
        { SCov__Blob_envelope (scov_rLumps, scov_nLumps, no)
          sphere { 0, scov_rLumps }
          texture { t_Envelope }
        }
        #break
      #case (SCOV_MERGE_QUALITY)
        merge
        { SCov__Blob_envelope (scov_rLumps, scov_nLumps, no)
          sphere { 0, scov_rLumps }
          texture { t_Envelope }
        }
        #break
      #else
        sphere
        { 0, scov_rEnvelope
          texture { t_Envelope }
        }
    #end
   //---------------------- spikes -------------------------
    union
    { #local scov_aSSpace = SCov__fn_Jitter (scov_nSs);
      #local scov_rJitter = scov_aSSpace * SCOV__SJITTER;
      #local scov_I = 0;
      #local scov_pv_SPosns = array[scov_nSs]
      #while (scov_I < scov_nSs)
        #local scov_x_Posn = transform
        { rotate <rand (scov__Rand) * scov_rJitter, rand (scov__Rand) * 360, 0>
          rotate
          < SCov__fn_Distribute ((scov_I + 0.5) / scov_nSs),
            scov_I * SCOV__GOLDEN * 360,
            0
          >
        }
        #local scov_pv_SPosns[scov_I] =
          vtransform (scov_rEnvelope * y, scov_x_Posn);
        object
        { SCov__Spike
          translate Diameter / 2 * y
          transform { scov_x_Posn }
        }
        #local scov_I = scov_I + 1;
      #end
      texture
      { t_Spike
        #if (Quality != SCOV_DRAFT_QUALITY) normal { bumps 1 } #end
      }
    }
   //-------------------- M proteins -----------------------
    union
    { #local scov_Angle = degrees (SCOV__MSPACING / scov_rEnvelope / 2);
      #local SCov_Pair = union
      { object
        { SCov__MProtein translate Diameter / 2 * y rotate scov_Angle * x
        }
        object
        { SCov__MProtein translate Diameter / 2 * y rotate -scov_Angle * x
        }
      }
      #local scov_MinSBand =
        SCov__fn_Band (scov_rEnvelope, scov_aSSpace * 2 * SCOV__BAND, scov_nSs);
      #local scov_MinSpace = radians (scov_aSSpace) * scov_rEnvelope;
      #local scov_pv_MPosns = array[scov_nMs]
      #local scov_I = 0;
      #while (scov_I < scov_nMs)
        #local scov_MFract = (scov_I + 0.5) / scov_nMs;
        #local scov_MAngle = SCov__fn_Distribute (scov_MFract);
       // Limit the set of spikes to compare:
        #local scov_SxCtr = SCov__fn_Round (scov_MFract * scov_nSs);
        #local scov_SDBand = max
        ( SCov__fn_Band (scov_rEnvelope, scov_MAngle, scov_nSs),
          scov_MinSBand
        );
        #local scov_SxMin = max (scov_SxCtr - scov_SDBand, 0);
        #local scov_SxMax = min (scov_SxCtr + scov_SDBand, scov_nSs - 1);
        // to unadjusted M position
        #local scov_rv_M = <scov_MAngle, scov_I * SCOV__GOLDEN * 360, 0>;
       // Adjust to avoid collisions with spikes:
        #local scov_Avoided = no;
        #local scov_J = 0;
        #local scov_rv_Furthest = <0, 0, 0>; // rotation of most avoidant try
        #local scov_Max_dist = 0; // distance of most avoidant try
        #while (scov_J <= SCOV__NTRIES & !scov_Avoided)
          #local scov_Shortest = scov_rEnvelope; // closest dist. for this try
          #local scov_rv_Avoid =
          < scov_aSSpace * 2 * scov_J / SCOV__NTRIES,
            scov_J * SCOV__GOLDEN * 360,
            0
          >;
          #local scov_pv_MPosn = vtransform
          ( scov_rEnvelope * y,
            transform { rotate scov_rv_Avoid rotate scov_rv_M }
          );
          #local scov_Collision = no;
          #local scov_K = scov_SxMin;
          #while (scov_K <= scov_SxMax)
            #local scov_Dist = vlength (scov_pv_SPosns[scov_K] - scov_pv_MPosn);
            #if (scov_Dist < scov_MinSpace)
              #local scov_Collision = yes;
              #if (scov_Dist < scov_Shortest)
                #local scov_Shortest = scov_Dist;
              #end
            #end
            #local scov_K = scov_K + 1;
          #end
          #if (scov_Shortest > scov_Max_dist)
            #local scov_Max_dist = scov_Shortest;
            #local scov_rv_Furthest = scov_rv_Avoid;
          #end
          #if (!scov_Collision) #local scov_Avoided = yes; #end
          #local scov_J = scov_J + 1;
        #end // while (scov_J <= SCOV__NTRIES...)
        #if (!scov_Avoided & scov_Debug) #warning "Collision not avoided!" #end
        #local scov_pv_MPosns[scov_I] = scov_pv_MPosn;
        object
        { SCov_Pair
          rotate rand (scov__Rand) * 360 * y
          rotate scov_rv_Furthest
          rotate scov_rv_M
        }
        #local scov_I = scov_I + 1;
      #end
      texture
      { t_MProtein
        #if (Quality != SCOV_DRAFT_QUALITY) normal { bumps 1 } #end
      }
    }
   //-------------------- E proteins -----------------------
    #if (1 = scov_nEs) object #else union #end
    { #local scov_MinMBand =
        SCov__fn_Band (scov_rEnvelope, scov_aSSpace * 2 * SCOV__BAND, scov_nMs);
      #local scov_I = 0;
      #while (scov_I < scov_nEs)
        #local scov_EFract = (scov_I + 0.5) / scov_nEs;
        #local scov_EAngle = SCov__fn_Distribute (scov_EFract);
       // Limit the set of spikes and M proteins to compare:
        #local scov_SxCtr = SCov__fn_Round (scov_EFract * scov_nSs);
        #local scov_SDBand = max
        ( SCov__fn_Band (scov_rEnvelope, scov_EAngle, scov_nSs),
          scov_MinSBand
        );
        #local scov_SxMin = max (scov_SxCtr - scov_SDBand, 0);
        #local scov_SxMax = min (scov_SxCtr + scov_SDBand, scov_nSs - 1);
        #local scov_MxCtr = SCov__fn_Round (scov_EFract * scov_nMs);
        #local scov_MDBand = max
        ( SCov__fn_Band (scov_rEnvelope, scov_EAngle, scov_nMs),
          scov_MinMBand
        );
        #local scov_MxMin = max (scov_MxCtr - scov_MDBand, 0);
        #local scov_MxMax = min (scov_MxCtr + scov_MDBand, scov_nMs - 1);
        // to unadjusted E position
        #local scov_rv_E = <scov_EAngle, scov_I * SCOV__GOLDEN * 360, 0>;
       // Adjust to avoid collisions with spikes or M proteins:
        #local scov_Avoided = no;
        #local scov_J = 0;
        #local scov_rv_Furthest = <0, 0, 0>; // rotation of most avoidant try
        #local scov_Max_dist = 0; // distance of most avoidant try
        #while (scov_J <= SCOV__NTRIES & !scov_Avoided)
          #local scov_Shortest = scov_rEnvelope; // closest dist. for this try
          #local scov_rv_Avoid =
          < scov_aSSpace * 2 * scov_J / SCOV__NTRIES,
            scov_J * SCOV__GOLDEN * 360,
            0
          >;
          #local scov_v_EPosn = vtransform
          ( scov_rEnvelope * y,
            transform { rotate scov_rv_Avoid rotate scov_rv_E }
          );
          #local scov_Collision = no;
          #local scov_K = scov_SxMin;
          #while (scov_K <= scov_SxMax)
            #local scov_Dist = vlength (scov_pv_SPosns[scov_K] - scov_v_EPosn);
            #if (scov_Dist < scov_MinSpace)
              #local scov_Collision = yes;
              #if (scov_Dist < scov_Shortest)
                #local scov_Shortest = scov_Dist;
              #end
            #end
            #local scov_K = scov_K + 1;
          #end
          #local scov_K = scov_MxMin;
          #while (scov_K <= scov_MxMax)
            #local scov_Dist = vlength (scov_pv_MPosns[scov_K] - scov_v_EPosn);
            #if (scov_Dist < scov_MinSpace)
              #local scov_Collision = yes;
              #if (scov_Dist < scov_Shortest)
                #local scov_Shortest = scov_Dist;
              #end
            #end
            #local scov_K = scov_K + 1;
          #end
          #if (scov_Shortest > scov_Max_dist)
            #local scov_Max_dist = scov_Shortest;
            #local scov_rv_Furthest = scov_rv_Avoid;
          #end
          #if (!scov_Collision) #local scov_Avoided = yes; #end
          #local scov_J = scov_J + 1;
        #end // while (scov_J <= SCOV__NTRIES...)
        #if (!scov_Avoided & scov_Debug) #warning "Collision not avoided!" #end
        object
        { SCov__EProtein
          translate Diameter / 2 * y
          rotate scov_rv_Furthest
          rotate scov_rv_E
        }
        #local scov_I = scov_I + 1;
      #end
      texture { t_EProtein }
    }
  }
#end

#version ScaryCoronavirus_Inc_Temp;
#end
// end of scarycoronavirus.inc
