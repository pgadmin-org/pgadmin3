//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Portions Copyright (C) 1998 - 2011, Julian Smart
// Portions Copyright (C) 2011 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ogl.h - OGL main include
//
//////////////////////////////////////////////////////////////////////////

#ifndef _OGL_OGL_H_
#define _OGL_OGL_H_

#include "ogl/basic.h"      // Basic shapes
#include "ogl/basicp.h"
#include "ogl/lines.h"      // Lines and splines
#include "ogl/linesp.h"
#include "ogl/divided.h"    // Vertically-divided rectangle
#include "ogl/composit.h"   // Composite images
#include "ogl/canvas.h"     // wxShapeCanvas for displaying objects
#include "ogl/ogldiag.h"    // wxDiagram

#include "ogl/bmpshape.h"
#include "ogl/constrnt.h"
#include "ogl/drawn.h"
#include "ogl/drawnp.h"
#include "ogl/mfutils.h"
#include "ogl/misc.h"

// TODO: replace with wxModule implementation
extern void wxOGLInitialize();
extern void wxOGLCleanUp();

#endif
// _OGL_OGL_H_
