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
// Function:	ParseString�����ַ�
// Purpose:		Actually breaks a string of words into individual piecesʵ���ϰ�һ���ַ��ָ�Ϊ�����Ĳ���
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
// Function:	LoadBVH														  ����.bvh�ļ�
// Purpose:		Actually load a BVH file into the system
// Arguments:	Name of the file to open and root skeleton to put it in
///////////////////////////////////////////////////////////////////////////////
BOOL LoadBVH(CString name,t_Bone *root)//����*root������ָ�����ROOT������������һ��t_Bone����struct�壬
//�������п�ʼ��ָ��*curBoneָ��*root��ָ���t_Bone����struct�壬���Ĳ����������ָ��*curBone���еġ�
//����������Ϻ�*root��ָ������t_Bone����struct����ӹ���ָ��ROOT�������������ƽ��������������ӡ�
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
	curBone = root;// root��ŵĵ�ַ����curBone������*curBoneָ��*root��ָ��Ķ���
	// OPEN THE BVH FILE//��BVH�ļ�
	if (fp = fopen((LPCTSTR)name,"r")) 
	{///////////////////////////////////////////////////////////////////////////
	 /////HIERARCHY
		while (!feof(fp))
		{
			fgets(buffer,MAX_STRINGLENGTH,fp);//Only read one line once.
			ParseString(buffer,&words,&cnt);
			/////////////////////////////////////////////////////////////////
			////�˴�if��乹��ROOT��JOINT��END����������else��乹��OFFSET��CHANNELS
			if (words.GetAt(0) == "ROOT" || words.GetAt(0) == "JOINT" || words.GetAt(0) == "End")
			{
				//��ָ��*tempBonesָ��һ���Ѿ�����õ����ݿռ�
				tempBones = (t_Bone *)malloc((curBone->childCnt + 1) * sizeof(t_Bone));
				//����if�����ҪĿ���ǵ����Ѿ����ӹ�����curBone������ӹ���ʱ����ԭ�������õĵ������ӹ������Ƶ�
				//������½����Ľṹ��buffer�У�������������Ա����trans.x,trans.y,trans.z(����ROOT����),
				//rot.x,rot.y,rot.z���µ�ַһһ��Ӧ�ظ��Ƹ�*channelData��buffer�����һ��ṹ�嵱ǰΪ�գ����ڴ��
				//�¹���
				if (curBone->childCnt > 0 && curBone->children != NULL)
				{
					//��ԭ���������ӹ����ṹ�帴�Ƶ��´����Ľṹ���У���ԭ�г�Ա����ֵ���䣨��ַ�б仯��
					memcpy(tempBones,curBone->children,curBone->childCnt * sizeof(t_Bone));
					t_Bone *bone1,*bone2;//��ʱ��������ʱ����¾��ӹ���
					int i,j;//For loop
					for (i = 0; i < curBone->childCnt; i++)//ѭ��ÿһ����ǰ�ѽ������ӹ���
					{
						bone1 = &curBone->children[i];
						bone2 = &tempBones[i];
						for (j = 0; j < channelCnt; j++)//ѭ����ǰ�Ѿ���õ�channel
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
					free(curBone->children);//�ͷ�ָ��ԭ�ӹ����ṹ�����ڴ�Ŀռ䣬������佫����childrenָ��
					                        //ָ���µ��ӹ����ṹ��
				}
				parent = curBone;//ȷ����ǰ������
				parent->children = tempBones;//�����������ӹ���
				curBone = &tempBones[parent->childCnt];//��ָ��*curBoneָ���������ӵ��ӹ���
				parent->childCnt++;//��ǰ��������Ա������������������1
				ResetBone(curBone, parent);		// SETUP INITIAL BONE SETTINGS.�ӹ��������丸����

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
			}//OFFSET����
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
			}//CHANNELS����
			else if(words.GetAt(0)=="}")
			{
				curBone = curBone->parent;//ָ��*curBone������������
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
				////��ȡ�����ٶ�
				/*if(words.GetAt(0)=="Frame"&&words.GetAt(1)=="Time:")
					root->primSpeed=atof(words.GetAt(2))*1000;*/

				words.RemoveAll();		// CLEAR WORD BUFFER

				int i,j;//For loop
				////////////////////////////////////////////////////////////////
			    ////��MOTION���ֵ�����֡��CHANNELֵ����*motionData
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
		//������ڵ�ǰloop��motionData[loop]��ʵ�����ݸ�����ǰloop�Ĵ��ʵ�����ݵ�*channelData[loop]�У�����Ϊ
		//��Ӧ��channelData[loop]����Ÿ�����trans.x,trans.y,trans.z(����ROOT����),rot.x,rot.y,rot.z�ĵ�ַ��
		//���ԣ���*channelData[loop]��ֵ��Ҳ�͸ı��˸�����trans.x,trans.y,trans.z(����ROOT����),rot.x,rot.y,rot.z
		//��ֵ��
		for (int i = 0; i < channelCnt; i++)
			*channelData[i] = motionData[i];
		

		// SET THE CHANNEL INFO INTO BONE
		root->primChannel = motionData; //��motion�ε����ж������ݵ�ַ����root->primChannel��
		                                //��*root->primChannelָ��motion�ε����ж�������
		root->primChanType = CHANNEL_TYPE_BVH;
		root->primFrameCount = (float)frameCnt;
		root->primCurFrame = 0;
		root->secChannel = (float *)channelData; //��������trans.x,trans.y,trans.z(����ROOT����),rot.x,rot.y,rot.z��
		                                         //��ַ����root->secChannel
		root->secFrameCount = (float)channelCnt; //��channel��������root->secFrameCount

		fclose(fp);
		return TRUE;
	}	
	return FALSE;
}
//// LoadBVH //////////////////////////////////////////////////////////////
//��BOOL LoadBVH(CString name,t_Bone *root)�����������ʱ��֮ǰ�����Ĳ���t_Bone *root��ָ���t_Bone struct���
//�ӹ���ָ������BVH�ļ���������ROOT��������ROOT�����Ѿ���BVH�ļ������������й����໥֮�佨���˸������ӣ�������
//�ṹָ��*root�е���Ӧ�ĳ�ԱҲ��BVH���������������ݣ���������Ա����trans.x,trans.y,trans.z(��ӦOFFSET��ROOT��
//������������Ա��������֡�仯���仯)Ҳʼ�մ����Ӧ��OFFSETֵ���Ժ��������Ĳ������ͽ����ָ��*root���в�����