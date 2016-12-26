///////////////////////////////////////////////////////////////////////////////
//
// Skeleton.cpp : Animation System Skeleton supprt file
//
// Purpose:	Structure Supprt routines for Hierarchical Animation System
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
#include "stdafx.h"
#include "skeleton.h"

///////////////////////////////////////////////////////////////////////////////
// Function:	DestroySkeleton 销毁骨头
// Purpose:		Clear memory for a skeletal system 为骨骼体系清空存储空间
// Arguments:	Pointer to bone system
///////////////////////////////////////////////////////////////////////////////
void DestroySkeleton(t_Bone *root)
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop;
	t_Bone *child;
///////////////////////////////////////////////////////////////////////////////
	// NEED TO RECURSIVELY GO THROUGH THE CHILDREN//需要递归搜索孩子节点/先找到最末端的bone，然后从最末端的bone
	//朝parent方向清空
	if (root->childCnt > 0)//如果孩子节点数目大于0
	{
		child = root->children;
		for (loop = 0; loop < root->childCnt; loop++,child++)
		{
			if (child->childCnt > 0)
				DestroySkeleton(child);
			if (child->primChannel > NULL)
			{
				free(child->primChannel);
				child->primChannel = NULL;
			}
		}
		free(root->children);//释放孩子节点指针
	}
//清空骨骼的所有信息
	root->primChanType = CHANNEL_TYPE_NONE;//无应用的渠道
	root->secChanType = CHANNEL_TYPE_NONE;
	root->primFrameCount = 0;
	root->secFrameCount = 0;
	root->primCurFrame = 0;
	root->primChannel = NULL;
	root->secChannel = NULL;

	
	root->childCnt = 0;						// COUNT OF ATTACHED BONE ELEMENTS
	root->children = NULL;					// POINTER TO CHILDREN
}//清空骨骼的所有信息
//// DestroySkeleton //////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	ResetBone
// Purpose:		Reset the bone system and set the parent bone
// Arguments:	Pointer to bone system, and parent bone (could be null)
///////////////////////////////////////////////////////////////////////////////
void ResetBone(t_Bone *bone,t_Bone *parent)
{
	bone->rot.x =
	bone->rot.y =
	bone->rot.z = 0.0;

	bone->trans.x =
	bone->trans.y =
	bone->trans.z = 0.0;

	bone->primChanType = CHANNEL_TYPE_NONE;
	bone->secChanType = CHANNEL_TYPE_NONE;
	bone->primFrameCount = 0;
	bone->secFrameCount = 0;
	bone->primCurFrame = 0;
	bone->primChannel = NULL;
	bone->secChannel = NULL;

	bone->childCnt = 0;						// COUNT OF ATTACHED BONE ELEMENTS
	bone->children = NULL;					// POINTER TO CHILDREN
	bone->parent = parent;                  // 与parent建立连接
}

//// ResetBone /////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Function:	BoneAdvanceFrame
// Purpose:		Increment the animation stream for a bone and possible the
//				children attached to that bone1
// Arguments:	Pointer to bone system, Delta frame value to move, if it is recursive
///////////////////////////////////////////////////////////////////////////////
void BoneAdvanceFrame(t_Bone *bone,int direction)//****************************************
{
/// Local Variables ///////////////////////////////////////////////////////////
	int loop;
	float **channelData,*motionData;
///////////////////////////////////////////////////////////////////////////////
	bone->primCurFrame += direction;
	if (bone->primCurFrame >= bone->primFrameCount)
		bone->primCurFrame = 0;
	if (bone->primCurFrame < 0)
		bone->primCurFrame += bone->primFrameCount;
	channelData = (float **)bone->secChannel;//将各骨骼的每一个channel数据地址赋给channelData指针，
		                                     //*channelData存放地址，需将float *型转换成float **型
	motionData = (float *)bone->primChannel;//bone->primChannel存放着BVH文件中motion段的所有动画数据地址
	motionData = &motionData[(int)(bone->primCurFrame * bone->secFrameCount)];//将指针motionData指向当
	                                                                          //前帧的起始地址
	for (loop = 0; loop < bone->secFrameCount; loop++)
		*channelData[loop] = motionData[loop];
}
//// BoneAdvanceFrame /////////////////////////////////////////////////////////////////
