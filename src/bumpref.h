/* bumpref.h
**
**    bummp-reflection-mapping using
**    NVIDIA vertex-shaders register-combiners and texture-programs
**    Copyright (C) 2002  Florian Berger
**    Email: harpin_floh@yahoo.de, florian.berger@jk.uni-linz.ac.at
**
**    Updated Version foobillard++ started at 12/2010
**    Copyright (C) 2010 - 2013 Holger Schaekel (foobillardplus@go4more.de)
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License Version 2 as
**    published by the Free Software Foundation;
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
*/

#ifndef BUMPREF_H
#define BUMPREF_H

#include "vmath.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        int init;
        int bumpref_list;
        int cubemap_bind;
        int normalmap_bind;
        GLfloat bump_light[3];
        GLfloat bump_diff_col[4];
        GLfloat bump_spec_col[4];
    } BumpRefType;


    /* bump reflections */

    BumpRefType bumpref_setup_vp_ts_rc(
                                       char * map_name, VMfloat strength,
                                       char * posx_name,
                                       char * posy_name,
                                       char * posz_name,
                                       char * negx_name,
                                       char * negy_name,
                                       char * negz_name,
                                       VMfloat zoffs
                                      );

    void bumpref_use( BumpRefType * bumpref );
    void bumpref_restore();


    /* bump only */

    BumpRefType bump_setup_vp_rc( char * map_name, VMfloat strength, int texgen );

    void bump_set_light( BumpRefType * bumpref, VMfloat x, VMfloat y, VMfloat z );
    void bump_set_diff( BumpRefType * bumpref, VMfloat r, VMfloat g, VMfloat b );
    void bump_set_spec( BumpRefType * bumpref, VMfloat r, VMfloat g, VMfloat b );

    void bump_use( BumpRefType * bump );
    void bump_restore(void);

#ifdef __cplusplus
}
#endif

#endif  /* BUMPREF_H */
