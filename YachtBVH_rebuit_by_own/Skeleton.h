/////////////////////////////////////////////////////
//与骨骼相关的功能函数///////////////////////////////
/////////////////////////////////////////////////////

#ifndef SKELETON_H
#define SKELETON_H

#define CHANNEL_TYPE_BVH			2048	// SPECIAL FORMAT FOR BVH
#define CHANNEL_TYPE_NONE			0		// NO CHANNEL APPLIED

//定义骨骼的x,y,z坐标
struct tVector
{
	float x,y,z;
};


//构建骨骼
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
		bone->primCurFrame = 0;//所有帧播放完毕后还原
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
#endif //#ifndef SKELETON_H