/* furniture.h
**
**    code for the display of the furniture 3d-models
**
**    foobillard++ started at 12/2010
**    Copyright (C) 2010/2011 Holger Schaekel (foobillardplus@go4more.de)
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
**/

#ifndef MESHES_H
#define MESHES_H

  void InitMesh(void);

  extern int sofa_id;         // glcompile-id sofa
  extern int chair_id;        // glcompile-id chair
  extern int bartable_id;     // glcompile-id bar-table
  extern int camin_id;        // glcompile-id camin
  extern int lamp_id;         // glcompile-id lamp

#endif /* MESHES_H */
