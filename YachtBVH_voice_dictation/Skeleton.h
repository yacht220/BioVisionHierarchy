///////////////////////////////////////////////////////////////////////////////
//
// Skeleton.h : Animation System structure definition file
//
// Purpose:	Structure Definition of Hierarchical Animation System
//
// I DIDN'T PUT THESE IN A C++ CLASS FOR CROSS PLATFORM COMPATIBILITY
// SINCE THE ENGINE MAY BE IMPLEMENTED ON CONSOLES AND OTHER SYSTEMS
//
// Created:
//		JL 9/1/97		
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1997 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(SKELETON_H__INCLUDED_)
#define SKELETON_H__INCLUDED_

/// Bone Definitions /////////////////////////////////////////////////////////
#define BONE_ID_ROOT				1		// ROOT BONE
///////////////////////////////////////////////////////////////////////////////

/// Channel Definitions ///////////////////////////////////////////////////////
#define CHANNEL_TYPE_NONE			0		// NO CHANNEL APPLIED
#define CHANNEL_TYPE_SRT			1		// SCALE ROTATION AND TRANSLATION
#define CHANNEL_TYPE_TRANS			2		// CHANNEL HAS TRANSLATION (X Y Z) ORDER
#define CHANNEL_TYPE_RXYZ			4		// ROTATION (RX RY RZ) ORDER
#define CHANNEL_TYPE_RZXY			8		// ROTATION (RZ RX RY) ORDER
#define CHANNEL_TYPE_RYZX			16		// ROTATION (RY RZ RX) ORDER
#define CHANNEL_TYPE_RZYX			32		// ROTATION (RZ RY RX) ORDER
#define CHANNEL_TYPE_RXZY			64		// ROTATION (RX RZ RY) ORDER
#define CHANNEL_TYPE_RYXZ			128		// ROTATION (RY RX RZ) ORDER
#define CHANNEL_TYPE_S				256		// SCALE ONLY
#define CHANNEL_TYPE_T				512		// TRANSLATION ONLY (X Y Z) ORDER
#define CHANNEL_TYPE_INTERLEAVED	1024	// THIS DATA STREAM HAS MULTIPLE CHANNELS
#define CHANNEL_TYPE_BVH			2048	// SPECIAL FORMAT FOR BVH

///////////////////////////////////////////////////////////////////////////////
typedef struct
{
	float x,y,z;
} tVector;
/// Structure Definitions ///////////////////////////////////////////////////////

// THIS STRUCTURE DEFINES A BONE IN THE ANIMATION SYSTEM
// A BONE IS ACTUALLY AN ABSTRACT REPRESENTATION OF A OBJECT
// IN THE 3D WORLD.  A CHARACTER COULD BE MADE OF ONE BONE
// WITH MULTIPLE VISUALS OF ANIMATION ATTACHED.  THIS WOULD
// BE SIMILAR TO A QUAKE CHARACTER.  BY MAKING IT HAVE LEVELS
// OF HIERARCHY AND CHANNELS OF ANIMATION IT IS JUST MORE FLEXIBLE
struct t_Bone
{
	// HIERARCHY INFO//层次信息
	t_Bone	*parent;					// POINTER TO PARENT BONE//指向父亲节点的指针
	int 	childCnt;					// COUNT OF CHILD BONES//孩子节点的个数
	t_Bone	*children;					// POINTER TO CHILDREN//孩子节点的指针
	tVector	rot;						// CURRENT ROTATION FACTORS//当前旋转因子
	tVector	trans;						// CURRENT TRANSLATION FACTORS//当前转换因子

	// ANIMATION INFO//动画信息
	DWORD	primChanType;				// WHAT TYPE OF PRIMARY CHANNEL IS ATTACHED//第一个描绘渠道的类型
	float	*primChannel;				// POINTER TO PRIMARY CHANNEL OF ANIMATION//动画第一个描绘渠道的指针
	float 	primFrameCount;				// FRAMES IN PRIMARY CHANNEL//第一个渠道的帧数
	float	primSpeed;					// CURRENT PLAYBACK SPEED//当前播放速度
	float	primCurFrame;				// CURRENT FRAME NUMBER IN CHANNEL//渠道中当前帧数
	DWORD	secChanType;				// WHAT TYPE OF SECONDARY CHAN IS ATTACHED//第二个渠道的类型
	float	*secChannel;				// POINTER TO SECONDARY CHANNEL OF ANIMATION//动画第二个描绘渠道的指针
	float	secFrameCount;				// FRAMES IN SECONDARY CHANNEL//第二个渠道的帧数
};

///////////////////////////////////////////////////////////////////////////////

/// Support Function Definitions //////////////////////////////////////////////

void DestroySkeleton(t_Bone *root);
void ResetBone(t_Bone *bone,t_Bone *parent);
void BoneAdvanceFrame(t_Bone *bone,int direction);

///////////////////////////////////////////////////////////////////////////////

#endif // !defined(SKELETON_H__INCLUDED_)
