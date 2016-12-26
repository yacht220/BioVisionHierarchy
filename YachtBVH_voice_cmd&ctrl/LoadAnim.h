///////////////////////////////////////////////////////////////////////////////
//
// LoadAnim.h 
//
// Purpose: implementation of the Motion Capture file Loader
//
// Created:
//		JL 9/5/97		
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1997 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(LoadAnim_H__INCLUDED_)
#define LoadAnim_H__INCLUDED_

#define MAX_STRINGLENGTH	8192

#include "Skeleton.h"

BOOL LoadBVH(CString name,t_Bone *root);

#endif // !defined(LoadAnim_H__INCLUDED_)
