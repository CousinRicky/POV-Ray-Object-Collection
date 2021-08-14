/* rc3metal.inc version 1.3A
 * Persistence of Vision Raytracer include file
 * POV-Ray Object Collection
 *
 * Metallic texture macros and colors.
 *
 * Copyright (C) 2013 - 2021 Richard Callwood III.  Some rights reserved.
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
 *        2011-Jul-24  Basic colors and texture macros are gathered from various
 *                     SDL code that I wrote from 2004 to 2008.
 * 1.0    2013-Mar-09  The initial version is finalized and uploaded.
 * 1.1    2013-Apr-23  RC3Metal_Set_functions() is added to define functions.
 *        2013-May-07  Debug output for tiny roughness values is improved.
 * 1.2    2013-Sep-06  RC3Metal_Set_highlight() is added to coordinate highlight
 *                     and brilliance.
 * 1.2.1  2016-Mar-11  Redundant shadow and (for POV-Ray 3.7) radiosity are
 *                     removed from blurred texture objects.
 *        2018-Jan-19  The default ambient is set to 0 for POV-Ray >= 3.8.
 *        2019-Aug-25  If indicated, RC3Metal_Set_highlight() warns that
 *                     RC3M_Albedo is turned off.
 *        2019-Aug-25  View_POV_Include_Stack is utilized.
 *        2019-Aug-30  The roughness function has a larger minimum value.
 *        2019-Sep-02  A galvanized macro with a turbulence argument is added.
 *        2019-Sep-02  International spellings of the galvanized macros are
 *                     added.
 *        2019-Sep-04  A warning is given if RC3M_Albedo is true with POV < 3.7.
 *        2019-Sep-04  If POV >= 3.7, specular albedo is always used.
 *        2019-Sep-12  The #version is constrained, rather than just set.
 * 1.3    2019-Sep-13  Bug fix: photons were broken by the 1.2.1 change to
 *                     blurred objects; this is now fixed.
 * 1.3A   2021-Aug-14  The license text is updated.
 *
 * Implementation note: due to POV-Ray namespace scope leakage, local variables
 * and function parameters (other than x, y, and z) *must* be prefixed.
 */
#ifndef (RC3Metal_Inc_Temp) #declare RC3Metal_Inc_Temp = version;
// Ambient behavior has changed over some POV-Ray versions, so set the version
// to prevent user surprises.
#version max (3.5, min (3.8, version));

#declare RC3METAL_VERSION = 1.3;
#ifdef (View_POV_Include_Stack)
  #debug "#include \"rc3metal.inc\" //version 1.3A\n"
  #debug concat ("  #version ", str(version,0,3), ";\n")
#end

#include "functions.inc"

//================================ PARAMETERS ==================================

#ifndef (RC3M_Diffuse) #declare RC3M_Diffuse = 0.6; #end
#ifndef (RC3M_Ambient)
  #declare RC3M_Ambient = rgb (RC3Metal_Inc_Temp >= 3.8? 0: 0.1);
#end
#ifndef (RC3M_Debug) #declare RC3M_Debug = off; #end
#ifndef (RC3M_Seed) #declare RC3M_Seed = 0; #end
#ifndef (RC3M_Albedo) #declare RC3M_Albedo = yes; #end

//========================== REFERENCE IDENTIFIERS =============================

// Sample colors.  See documentation for notes.

#declare RC3M_C_ALUMINUM = rgb <0.89, 0.91, 0.91>;
#declare RC3M_C_ALUMINIUM = RC3M_C_ALUMINUM;
#declare RC3M_C_BRASS_COOL = rgb <0.80, 0.72, 0.32>;
#declare RC3M_C_BRASS_LIGHT = rgb <0.80, 0.64, 0.32>;
#declare RC3M_C_BRASS = rgb <0.80, 0.57, 0.23>;
#declare RC3M_C_BRASS_WARM = rgb <0.80, 0.52, 0.20>;
#declare RC3M_C_BRONZE_NEW = rgb <0.80, 0.50, 0.25>;
#declare RC3M_C_BRONZE_COOL = rgb <0.36, 0.30, 0.12>;
#declare RC3M_C_BRONZE = rgb <0.36, 0.26, 0.12>;
#declare RC3M_C_BRONZE_WARM = rgb <0.36, 0.22, 0.12>;
#declare RC3M_C_CHROME = rgb <0.61, 0.61, 0.64>;
#declare RC3M_C_COPPER = rgb <0.80, 0.38, 0.24>;
#declare RC3M_C_COPPER_LIGHT = rgb <0.80, 0.44, 0.32>;
#declare RC3M_C_GOLD = rgb <0.95, 0.75, 0.40>;
#declare RC3M_C_GOLD_LIGHT = rgb <0.95, 0.80, 0.40>;
#declare RC3M_C_NICKEL = rgb <0.68, 0.64, 0.55>;
#declare RC3M_C_PENNY = rgb <0.80, 0.45, 0.25>;
#declare RC3M_C_SILVER = rgb <0.96, 0.96, 0.92>;
#declare RC3M_C_SILVER_COIN = rgb <0.66, 0.63, 0.57>;
#declare RC3M_C_STAINLESS = rgb <0.49, 0.49, 0.49>;
#declare RC3M_C_STAINLESS_WARM = rgb <0.55, 0.55, 0.51>;
#declare RC3M_C_STEEL = rgb <0.61, 0.59, 0.54>;
#declare RC3M_C_ZINC = rgb <0.66, 0.72, 0.72>;
#declare RC3M_C_ZINC_COOL = rgb <0.66, 0.72, 0.78>;
/* Metals I would like to see:
#declare RC3M_C_LEAD = rgb <?, ?, ?>;
#declare RC3M_C_NIOBIUM = rgb <?, ?, ?>;   //used for hypoallergenic body-piercing jewelry
#declare RC3M_C_PEWTER = rgb <?, ?, ?>;
#declare RC3M_C_PLATINUM = rgb <?, ?, ?>;
#declare RC3M_C_RHODIUM = rgb <?, ?, ?>;   //used for white gold plating
#declare RC3M_C_RUTHENIUM = rgb <?, ?, ?>; //used for dark plating for (junk?) jewelry
#declare RC3M_C_SPECULUM = rgb <?, ?, ?>;
#declare RC3M_C_TIN = rgb <?, ?, ?>;
#declare RC3M_C_TITANIUM = rgb <?, ?, ?>;
#declare RC3M_C_TITANIUM_NITRIDE = rgb <?, ?, ?>; //pruners, drill bits
*/

//============================= PRIVATE FEATURES ===============================
// These features should not be referenced directly by the user.

// Initialization:
#declare rc3m__Stochasm = seed (RC3M_Seed);
#declare rc3m__Warned_albedo = no;

// https://wiki.povray.org/content/Knowledgebase:Language_Questions_and_Tips#Topic_13
#macro rc3m__Blur_sample_n (Blurriness, Scale)
  bumps Blurriness
  translate
    <rand (rc3m__Stochasm), rand (rc3m__Stochasm), rand (rc3m__Stochasm)> * 100
  scale Scale
#end

// Converts a number to a string of so many significant digits:
#macro rc3m__Str (Number, Sig)
  #if (Number < 0 | Number > 0) //This circumlocution avoids the epsilon.
    str (Number, 1, max (Sig - floor(log(abs(Number))) - 1, 0))
  #else
    str (0, 1, Sig - 1)
  #end
#end

//============================== PUBLIC MACROS =================================

//--------------------- Basic textures ---------------------

#macro RC3Metal_Finish (Specularity, Diffuse_part)

  #if (RC3M_Albedo & RC3Metal_Inc_Temp < 3.7 & !rc3m__Warned_albedo)
    #warning concat
    ( "RC3Metal_Finish(): POV-Ray ", str (RC3Metal_Inc_Temp, 0, 2),
      ": RC3M_Albedo is ignored."
    )
    #declare rc3m__Warned_albedo = yes;
  #end
  #local rc3m_Albedo_kwd = (version >= 3.7 & RC3M_Albedo);
  #local rc3m_Rough = RC3M_fn_Roughness (Specularity);
  #local rc3m_Hilite = RC3M_fn_Specular (Specularity);
 //If albedo keyword is not used, then adjust the specular level here.
 //See Parser::Parse_Finish() in parstxtr.cpp in the POV-Ray 3.7 source.
  #if (version < 3.7)
    #local rc3m_Hilite = rc3m_Hilite
      * (1 / rc3m_Rough + 2) / (4 * (2 - pow (2, -0.5 / rc3m_Rough)));
  #end
 //Reduce the diffuse to complement the specular value:
  #local rc3m_Actual_diffuse = (1 - Specularity) * Diffuse_part;
 //Preserve the scene's ambient:diffuse ratio:
 // (5-D color must be used to prevent misleading debugging output)
  #local rc3m_Actual_ambient =
    color rc3m_Actual_diffuse * RC3M_Ambient / RC3M_Diffuse
    filter 0 transmit 0;

  #if (RC3M_Debug)
    #debug concat
    ( "RC3Metal_Finish (", str(Specularity,0,6), ", ", str(Diffuse_part,0,6),
      "):\n"
    )
    #debug "  finish\n"
    #debug concat ("  { reflection { ", str (Specularity,0,6), " metallic }\n")
    #debug concat
    ( "    diffuse ",
      #if (rc3m_Albedo_kwd) "albedo ", #end
      str (rc3m_Actual_diffuse, 0, 6), "\n"
    )
    #debug concat
      ("    ambient <", vstr (5, rc3m_Actual_ambient, ", ", 0, 6), ">\n")
    #debug concat
      ("    brilliance ", str (RC3M_fn_Brilliance (Specularity), 0, 6), "\n")
    #debug concat
    ( "    specular ",
      #if (version >= 3.7) "albedo ", #end
      rc3m__Str (rc3m_Hilite, 6), " metallic\n"
    )
    #debug concat ("    roughness ", rc3m__Str (rc3m_Rough, 6), "\n")
    #debug "  }\n"
  #end

  finish
  { reflection { Specularity metallic }
    diffuse #if (rc3m_Albedo_kwd) albedo #end rc3m_Actual_diffuse
    ambient rc3m_Actual_ambient
    brilliance RC3M_fn_Brilliance (Specularity)
    specular #if (version >= 3.7) albedo #end rc3m_Hilite metallic
    roughness rc3m_Rough
  }
#end

#macro RC3Metal_Pigment (c_Color, Specularity)
  #local rc3m_Color = color c_Color;
  #local rc3m_Quick_color = rc3m_Color / max (1 - Specularity, 0.001);

  #if (RC3M_Debug)
    #debug concat
    ( "RC3Metal_Pigment (<", vstr (5, rc3m_Color, ", ", 0, 6), ">, ",
      str(Specularity,0,6), "):\n"
    )
    #debug "  pigment\n"
    #debug concat ("  { <", vstr (5, rc3m_Color, ", ", 0, 6), ">\n")
    #debug concat
      ("    quick_color <", vstr (5, rc3m_Quick_color, ", ", 0, 6), ">\n")
    #debug "  }\n"
  #end

  pigment { rc3m_Color quick_color rc3m_Quick_color }
#end

#macro RC3Metal (c_Color, Specularity, Diffuse_part)
  #if (RC3M_Debug) #debug "RC3Metal()...\n" #end
  RC3Metal_Pigment (c_Color, Specularity)
  RC3Metal_Finish (Specularity, Diffuse_part)
#end

#macro RC3Metal_Texture (c_Color, Specularity, Diffuse_part)
  #if (RC3M_Debug) #debug "RC3Metal_Texture()...\n" #end
  texture { RC3Metal (c_Color, Specularity, Diffuse_part) }
#end

//------------------ Blurred reflection --------------------
// https://wiki.povray.org/content/Knowledgebase:Language_Questions_and_Tips#Topic_13

#macro RC3Metal_Blur_t (t_Base, Blurriness, Scale, Samples)
  texture
  { average texture_map
    { #local rc3m_I = 0;
      #while (rc3m_I < Samples)
        [1 t_Base normal { rc3m__Blur_sample_n (Blurriness, Scale) }]
        #local rc3m_I = rc3m_I + 1;
      #end
    }
  }
#end

#macro RC3Metal_Normal_blur_t (t_Base, Blurriness, Scale, Samples, n_Normal)
  texture
  { average texture_map
    { #local rc3m_I = 0;
      #while (rc3m_I < Samples)
        [1 t_Base
          normal
          { average normal_map
            { [1 rc3m__Blur_sample_n (Blurriness, Scale)]
              [1 n_Normal]
            }
          }
        ]
        #local rc3m_I = rc3m_I + 1;
      #end
    }
  }
#end

#macro RC3Metal_Blur (o_Object, t_Base, Blurriness, Scale, Samples)
  union
  { object
    { o_Object
      RC3Metal_Blur_t (t_Base, Blurriness, Scale, Samples)
      no_reflection
      #if (version >= 3.7) no_radiosity #end
    }
    object
    { o_Object
      texture { t_Base }
      no_image
      no_shadow
    }
  }
#end

#macro RC3Metal_Normal_blur (o_Object, t_Base, Blurriness, Scale, Samples, n_Normal)
  union
  { object
    { o_Object
      RC3Metal_Normal_blur_t (t_Base, Blurriness, Scale, Samples, n_Normal)
      no_reflection
      #if (version >= 3.7) no_radiosity #end
    }
    object
    { o_Object
      texture { t_Base normal { n_Normal } }
      no_image
      no_shadow
    }
  }
#end

//----------------- Miscellaneous macros -------------------

#macro RC3Metal_Galvanized_t (c_Dull, c_Shiny, Dull, Shiny, Dark, Light, Shine)
  #if (RC3M_Debug) #debug "RC3Metal_Galvanized_t()...\n" #end
  texture
  { crackle solid texture_map
    { [0 RC3Metal (c_Shiny, Shiny, Light)]
      [Shine RC3Metal (c_Dull, Dull, Light)]
      [1 RC3Metal (c_Dull, Dull, Dark)]
    }
    turbulence 0.25
  }
#end

#macro RC3Metal_Galvanized2_t
( c_Dull, c_Shiny, Dull, Shiny, Dark, Light, Shine, Turbulence
)
  #if (RC3M_Debug) #debug "RC3Metal_Galvanized_t2()...\n" #end
  texture
  { crackle solid texture_map
    { [0 RC3Metal (c_Shiny, Shiny, Light)]
      [Shine RC3Metal (c_Dull, Dull, Light)]
      [1 RC3Metal (c_Dull, Dull, Dark)]
    }
    turbulence Turbulence
    #if (RC3M_Debug)
      #debug concat ("turbulence ", rc3m__Str (Turbulence, 3), "\n")
    #end
  }
#end

#macro RC3Metal_Galvanised_t (c_Dull, c_Shiny, Dull, Shiny, Dark, Light, Shine)
  RC3Metal_Galvanized_t (c_Dull, c_Shiny, Dull, Shiny, Dark, Light, Shine)
#end

#macro RC3Metal_Galvanised2_t
( c_Dull, c_Shiny, Dull, Shiny, Dark, Light, Shine, Turbulence
)
  RC3Metal_Galvanized2_t
  ( c_Dull, c_Shiny, Dull, Shiny, Dark, Light, Shine, Turbulence
  )
#end

#macro RC3M_Uncolor_v (c_Color)
  #local rc3m_C = color c_Color;
  <rc3m_C.red, rc3m_C.green, rc3m_C.blue>
#end

#macro RC3M_Uncolour_v (c_Color)
  RC3M_Uncolor_v (c_Color)
#end

//================== FUNCTIONS AND FUNCTION-RELATED MACROS =====================
// Notes:  The roughness and brilliance functions are based on looks, not
// physical theory.  RC3Metal_Set_highlight() coodinates the specular and
// brilliance functions to maintain consistent albedo.

#macro RC3Metal_Set_functions (Hilite, Brilliance)
  #ifdef (RC3M_fn_Brilliance) #undef RC3M_fn_Brilliance #end
  #declare RC3M_fn_Brilliance = function (x) { Brilliance }
  #ifdef (RC3M_fn_Specular) #undef RC3M_fn_Specular #end
  #declare RC3M_fn_Specular = function (x) { (1 - Hilite) * x + Hilite }
#end

#macro RC3Metal_Set_highlight (Hilite)
  #local rc3m_This = "RC3Metal_Set_highlight"
  #if (Hilite < 0 | !(Hilite < 1))
    #warning concat
    ( rc3m_This, "(): the Hilite argument should be a non-negative",
      " number less than one, but ", rc3m__Str (Hilite, 6), " was passed."
    )
  #end
  #if (RC3M_Albedo & version >= 3.7)
    #warning concat (rc3m_This, "() is called; RC3M_Albedo is set to false.")
  #end
  #declare RC3M_Albedo = false; // side effect!
  RC3Metal_Set_functions (Hilite, 2 / (1 - Hilite) - 1)
#end

#declare RC3M_fn_Roughness = function (x)
{ #local rc3m_ROUGH = 400;
  #local rc3m_EPSILON = 1e-6;
  max
  ( rc3m_EPSILON,
    0.06 * (1 / pow (rc3m_ROUGH, x) - 1/rc3m_ROUGH)
         * rc3m_ROUGH / (rc3m_ROUGH - 1)
  )
}

//Grandfather the RC3Metal 1.0 settings:
RC3Metal_Set_functions ((version >= 3.7 & RC3M_Albedo? 0.2: 0.4), 2.5)

//================================== NORMAL ====================================

#declare RC3M_n_Brushed = normal
{ function { f_noise3d (x, y * 1000, z * 1000) } - 0.15
}

#version RC3Metal_Inc_Temp;
#end
// end of rc3metal.inc
