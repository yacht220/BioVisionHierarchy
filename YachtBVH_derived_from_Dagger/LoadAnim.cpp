///////////////////////////////////////////////////////////////////////////////
//
// LoadAnim.cpp 
//
// Purpose: implementation of the Motion Capture file Loader
//
// Created:
//		JL 9/5/97		
// Versions:
//		1.0		12/2/97	Initial Merge of the BVA Code
//		1.02	1/10/97 Merge in of ASF Hierarchy read code
//
// Todo:
//		Still trying to catch up from the holidays
//		Need to merge AMC portion of Acclaim loader for animation
//		Merge of BVH reader code
//
//		I WILL PROBABLY PULL OUT THE CHANNELS INTO A NEW STRUCT
//		ADD SPEED OF PLAYBACK VARIABLE TO CHANNEL STRUCT
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
#include "LoadAnim.h"

///////////////////////////////////////////////////////////////////////////////
// Function:	ParseString解析字符
// Purpose:		Actually breaks a string of words into individual pieces实际上把一串字符分割为单个的部分
// Arguments:	Source string in, array to put the words and the count
///////////////////////////////////////////////////////////////////////////////
void ParseString(char *buffer,CStringArray *words,int *cnt)
{
/// Local Variables ///////////////////////////////////////////////////////////
	CString in = buffer, temp;
///////////////////////////////////////////////////////////////////////////////
	
	in.TrimLeft();
	in.TrimRight();
	*cnt = 0;
	do 
	{
		temp = in.SpanExcluding(" \t");		// GET UP TO THE NEXT SPACE OR TAB
		words->Add(temp);
		if (temp == in) break;
		in = in.Right(in.GetLength() - temp.GetLength());
		in.TrimLeft();
		*cnt = *cnt + 1;			
	} while (1);
	*cnt = *cnt + 1;
}
//// ParseString //////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Function:	LoadBVH														  加载.bvh文件
// Purpose:		Actually load a BVH file into the system
// Arguments:	Name of the file to open and root skeleton to put it in
///////////////////////////////////////////////////////////////////////////////
BOOL LoadBVH(CString name,t_Bone *root)//参数*root本身并非指向骨骼ROOT本身，而是另外一个t_Bone类型struct体，
//函数运行开始，指针*curBone指向*root所指向的t_Bone类型struct体，随后的操作都是针对指针*curBone进行的。
//函数运行完毕后，*root所指向的这个t_Bone类型struct体的子骨骼指向ROOT骨骼，依此类推建立最后的数据连接。
{
    ///////////////////////////////////////////////////////////////////////////
	/////Definition
	t_Bone *parent,//Locate parent bone
		   *curBone,//Locate current bone
		   *tempBones;//For purpose of temporarily locating bones
	float **channelData=(float **)malloc(sizeof(float *) * 255),//Store channel information
		  *motionData=NULL;//Store motion information
	char *buffer = (char *)malloc(MAX_STRINGLENGTH);//Store opened file data
	int frameCnt=0,channelCnt=0;//For counting
	FILE *fp;//To point to a opened file;
    ///////////////////////////////////////////////////////////////////////////
	/////For ParseString Function
	int cnt;
	CStringArray words;
	////////////////////////////////////////////////////////////////////////////
	/////Implementation
	curBone = root;// root存放的地址赋给curBone，即让*curBone指向*root所指向的对象。
	// OPEN THE BVH FILE//打开BVH文件
	if (fp = fopen((LPCTSTR)name,"r")) 
	{///////////////////////////////////////////////////////////////////////////
	 /////HIERARCHY
		while (!feof(fp))
		{
			fgets(buffer,MAX_STRINGLENGTH,fp);//Only read one line once.
			ParseString(buffer,&words,&cnt);
			/////////////////////////////////////////////////////////////////
			////此处if语句构建ROOT、JOINT和END骨骼，其后的else语句构建OFFSET和CHANNELS
			if (words.GetAt(0) == "ROOT" || words.GetAt(0) == "JOINT" || words.GetAt(0) == "End")
			{
				//让指针*tempBones指向一个已经分配好的数据空间
				tempBones = (t_Bone *)malloc((curBone->childCnt + 1) * sizeof(t_Bone));
				//下面if语句主要目的是当对已经有子骨骼的curBone添加新子骨骼时，将原来建立好的的所有子骨骼复制到
				//上面的新建立的结构体buffer中，并将各骨骼成员变量trans.x,trans.y,trans.z(仅对ROOT骨骼),
				//rot.x,rot.y,rot.z的新地址一一对应地复制给*channelData，buffer的最后一组结构体当前为空，用于存放
				//新骨骼
				if (curBone->childCnt > 0 && curBone->children != NULL)
				{
					//将原来的所有子骨骼结构体复制到新创建的结构体中，其原有成员变量值不变（地址有变化）
					memcpy(tempBones,curBone->children,curBone->childCnt * sizeof(t_Bone));
					t_Bone *bone1,*bone2;//临时变量，临时存放新旧子骨骼
					int i,j;//For loop
					for (i = 0; i < curBone->childCnt; i++)//循环每一个当前已建立的子骨骼
					{
						bone1 = &curBone->children[i];
						bone2 = &tempBones[i];
						for (j = 0; j < channelCnt; j++)//循环当前已经获得的channel
						{
							if(channelData[j]==&bone1->trans.x)
								channelData[j] = &bone2->trans.x;
							if(channelData[j]==&bone1->trans.y)
								channelData[j] = &bone2->trans.y;
							if(channelData[j]==&bone1->trans.z)
								channelData[j] = &bone2->trans.z;
							if(channelData[j]==&bone1->rot.x)
								channelData[j] = &bone2->rot.x;
							if(channelData[j]==&bone1->rot.y)
								channelData[j] = &bone2->rot.y;
							if(channelData[j]==&bone1->rot.z)
								channelData[j] = &bone2->rot.z;
						}
					}
					free(curBone->children);//释放指向原子骨骼结构体在内存的空间，随后的语句将让其children指针
					                        //指向新的子骨骼结构体
				}
				parent = curBone;//确定当前父骨骼
				parent->children = tempBones;//父骨骼连接子骨骼
				curBone = &tempBones[parent->childCnt];//让指针*curBone指向最新增加的子骨骼
				parent->childCnt++;//当前父骨骼成员变量孩子数计数器加1
				ResetBone(curBone, parent);		// SETUP INITIAL BONE SETTINGS.子骨骼连接其父骨骼

				words.RemoveAll();		// CLEAR WORD BUFFER
                
				//GET THE NEXT LINE
				fgets(buffer,MAX_STRINGLENGTH,fp);
				ParseString(buffer,&words,&cnt);
				if (words.GetAt(0) != "{")
				{
					::MessageBox(NULL,"Error parsing BVH File: No opening Braces","ERROR",MB_OK);
				}
			}
			else if(words.GetAt(0)=="OFFSET")
			{
				curBone->trans.x = atof(words.GetAt(1));
				curBone->trans.y = atof(words.GetAt(2));
				curBone->trans.z = atof(words.GetAt(3));
			}//OFFSET数据
			else if(words.GetAt(0)=="CHANNELS")
			{
				if(words.GetAt(1)=="6")
				{
					channelData[channelCnt++] = &curBone->trans.x;
					channelData[channelCnt++] = &curBone->trans.y;
					channelData[channelCnt++] = &curBone->trans.z;
					channelData[channelCnt++] = &curBone->rot.z;
					channelData[channelCnt++] = &curBone->rot.x;
					channelData[channelCnt++] = &curBone->rot.y;
				}
				else
				{
					channelData[channelCnt++] = &curBone->rot.z;
					channelData[channelCnt++] = &curBone->rot.x;
					channelData[channelCnt++] = &curBone->rot.y;
				}
			}//CHANNELS数据
			else if(words.GetAt(0)=="}")
			{
				curBone = curBone->parent;//指针*curBone返回至父骨骼
			}
			else if(words.GetAt(0)=="MOTION")
			{
				words.RemoveAll();		// CLEAR WORD BUFFER
				break;
			}
			words.RemoveAll();		// CLEAR WORD BUFFER
		}
		//////////////////////////////////////////////////////////////////////
		//////MOTION
		while (!feof(fp))
		{   // NEXT GET THE FRAMECOUNT
			fgets(buffer,MAX_STRINGLENGTH,fp);
			ParseString(buffer,&words,&cnt);
			if (words.GetAt(0) == "Frames:")
			{
				frameCnt = atoi(words.GetAt(1));
				motionData = (float *)malloc(sizeof(float) * channelCnt * frameCnt);
 				words.RemoveAll();		// CLEAR WORD BUFFER
			
				fgets(buffer,MAX_STRINGLENGTH,fp);
				ParseString(buffer,&words,&cnt);
				////////////////////////////////////////////////////////////////
				////读取播放速度
				/*if(words.GetAt(0)=="Frame"&&words.GetAt(1)=="Time:")
					root->primSpeed=atof(words.GetAt(2))*1000;*/

				words.RemoveAll();		// CLEAR WORD BUFFER

				int i,j;//For loop
				////////////////////////////////////////////////////////////////
			    ////将MOTION部分的所有帧的CHANNEL值存在*motionData
				for (i = 0; i < frameCnt; i++)
				{
					fgets(buffer,MAX_STRINGLENGTH,fp);
					ParseString(buffer,&words,&cnt);
					for (int j = 0; j < channelCnt; j++)
					{
						motionData[i * channelCnt + j] = atof(words.GetAt(j));
					}
			
					words.RemoveAll();		// CLEAR WORD BUFFER
				}

				
			}

			words.RemoveAll();		// CLEAR WORD BUFFER
		}

		// Set the First Frame
		//将存放在当前loop的motionData[loop]的实际数据赋给当前loop的存放实际数据的*channelData[loop]中，又因为
		//对应的channelData[loop]存放着各骨骼trans.x,trans.y,trans.z(仅对ROOT骨骼),rot.x,rot.y,rot.z的地址，
		//所以，对*channelData[loop]赋值，也就改变了各骨骼trans.x,trans.y,trans.z(仅对ROOT骨骼),rot.x,rot.y,rot.z
		//的值。
		for (int i = 0; i < channelCnt; i++)
			*channelData[i] = motionData[i];
		

		// SET THE CHANNEL INFO INTO BONE
		root->primChannel = motionData; //将motion段的所有动画数据地址赋给root->primChannel，
		                                //即*root->primChannel指向motion段的所有动画数据
		root->primChanType = CHANNEL_TYPE_BVH;
		root->primFrameCount = (float)frameCnt;
		root->primCurFrame = 0;
		root->secChannel = (float *)channelData; //将各骨骼trans.x,trans.y,trans.z(仅对ROOT骨骼),rot.x,rot.y,rot.z的
		                                         //地址赋给root->secChannel
		root->secFrameCount = (float)channelCnt; //将channel总数赋给root->secFrameCount

		fclose(fp);
		return TRUE;
	}	
	return FALSE;
}
//// LoadBVH //////////////////////////////////////////////////////////////
//当BOOL LoadBVH(CString name,t_Bone *root)函数运行完毕时，之前带进的参数t_Bone *root所指向的t_Bone struct体的
//子骨骼指向了是BVH文件中描述的ROOT骨骼，且ROOT骨骼已经与BVH文件所描述的所有骨骼相互之间建立了父子连接，且数据
//结构指针*root中的相应的成员也有BVH中描述的所有数据，各骨骼成员变量trans.x,trans.y,trans.z(对应OFFSET，ROOT骨
//骼的这三个成员变量会随帧变化而变化)也始终存放相应的OFFSET值，以后对其骨骼的操作，就仅需对指针*root进行操作。