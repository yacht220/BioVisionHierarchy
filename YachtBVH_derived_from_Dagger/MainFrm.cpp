///////////////////////////////////////////////////////////////////////////////
//
// MainFrm.cpp : implementation of the CMainFrame class
//
// Purpose:	Implementation of Main Window of Hierarchical Animation System
//
// Created:
//		JL 9/1/97		
// Versions:
//		1.01	12/20/97	Added OpenGL Resize Code, Moved window size to defines
//		1.02	1/10/97		Merge in of ASF Hierarchy read code
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
#include "mmsystem.h"		// NEED THIS FOR THE TIMEGETTIME
#include "YachtBVH.h"
#include "LoadAnim.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_BACK_FRAME, OnBackFrame)
	ON_COMMAND(ID_PLAY_BACK, OnPlayBack)
	ON_COMMAND(ID_STOP, OnStop)
	ON_COMMAND(ID_PLAY_FORWARD, OnPlayForward)
	ON_COMMAND(ID_FORWARD_FRAME, OnForwardFrame)
	ON_COMMAND(ID_FILE_LOADANIM, OnFileLoadanim)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_Animating = FALSE;
	m_ElapsedTime = ~(DWORD)0; // initialize to a big number
	m_Animation_Direction = 1;
	m_AnimSpeed = 30;			// DEFAULT PLAYBACK SPEED AS FRAME DELAY
	m_HArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);	

	InitializeSkeleton();
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)//********************************
{
/// Local Variables ///////////////////////////////////////////////////////////
	RECT rect;
///////////////////////////////////////////////////////////////////////////////
	GetClientRect(&rect); 

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

    //将类CMainFrame的成员变量m_Skelton的地址赋给COGLView的指针成员变量*m_Skeleton
	m_OGLView.Create(NULL,"Render Window",WS_CHILD | WS_VISIBLE,rect,this,104,&m_Skeleton);

	m_OGLView.ShowWindow(TRUE);

	m_OGLView.Invalidate(TRUE);

	
	// Now we initialize the animation code

	m_StartTime = ::timeGetTime(); // get time in ms

	// need a previous time if we start off animated
	if ( m_Animating )
		{
		m_previousElapsedTime = m_StartTime;
		}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	OnPaint
// Purpose:		This routine grabs the message loop if I am animating and
//				handles the messages.  This way I can play back as fast
//              as possible
// Reference:	OpenGL Programming for Windows 95 by Ron Fosner
//				Sort of a variation on that code
///////////////////////////////////////////////////////////////////////////////		
void CMainFrame::OnPaint()    //******************************************************************************
{
	MSG msg;
	CPaintDC dc(this); // device context for painting

	// IF I AM IN ANIMATING MODE
	while ( m_Animating == 1)
	{
		//m_AnimSpeed=(int)m_Skeleton.primSpeed;
		while (::PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			if (msg.message == WM_QUIT)//若收到消息未退出
			{
				m_Animating = FALSE;
				::PostQuitMessage(0);
			}

			// Dispatch any messages as needed
			if (!AfxGetApp()->PreTranslateMessage(&msg))//Nonzero if the message was fully processed in
				                                        //PreTranslateMessage and should not be processed
														//further. Zero if the message should be processed
														//in the normal way.
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			
			// Give the Idle system some time
			AfxGetApp()->OnIdle(0);
			AfxGetApp()->OnIdle(1);

		}
		m_ElapsedTime = ::timeGetTime(); // get current time

		// CHECK IF ENOUGH TIME HAS GONE BY TO DRAW THE NEXT FRAME
		if (ElapsedTimeRender() > (unsigned int)(1000/m_AnimSpeed)/*毫秒/帧*/ )//每帧之间的间隔
		{
			// ADVANCE THE ANIMATION
			//将下一帧的数据调出
			BoneAdvanceFrame(&m_Skeleton,m_Animation_Direction);
			// REDRAW THE OGL WINDOW
			m_OGLView.drawScene();	
			// RESET THE TIME COUNTER
			m_previousElapsedTime = m_ElapsedTime;
		}
	}
    m_OGLView.drawScene();	
}
/// OnPaint ////////////////////////////////////////////////////////////

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	// RESET THE m_OGLView WINDOW SIZE
	m_OGLView.SetWindowPos( &wndTopMost, 0, 0, cx , cy, SWP_NOZORDER );
	// RESET THE ACTUAL OPENGL WINDOW SIZE
    m_OGLView.resize( cx, cy);
	CFrameWnd::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
// Hierarchy Manipulation Functions

void CMainFrame::InitializeSkeleton()
{
	// INITIALIZE SOME OF THE SKELETON VARIABLES//初始化一些骨骼变量
	ResetBone(&m_Skeleton, NULL);
	m_Skeleton.rot.x = 0.0f;
	m_Skeleton.rot.y = 0.0f;
	m_Skeleton.rot.z = 0.0f;
	m_Skeleton.trans.y = 0.0f;
	m_Skeleton.trans.z = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
// Animation Control Functions

void CMainFrame::OnPlayBack() 
{
	m_Animating = TRUE;
	m_Animation_Direction = -1;
	Invalidate(TRUE);
}

void CMainFrame::OnBackFrame() 
{
	m_Animating = FALSE;
	BoneAdvanceFrame(&m_Skeleton,-1);
	m_OGLView.drawScene();
}

void CMainFrame::OnStop() 
{
	m_Animating = FALSE;
}

void CMainFrame::OnForwardFrame() 
{
	m_Animating = FALSE;
	BoneAdvanceFrame(&m_Skeleton,1);
	m_OGLView.drawScene();
}

void CMainFrame::OnPlayForward() 
{
	m_Animation_Direction = 1;
	m_Animating = TRUE;
	Invalidate(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Animation File Functions

///////////////////////////////////////////////////////////////////////////////
// Procedure:	OnFileLoadanim
// Purpose:		Create dialog to load an animation file of various formats
//				Response to Menu Message
///////////////////////////////////////////////////////////////////////////////		
void CMainFrame::OnFileLoadanim() //**************************************************************************
{
/// Local Variables ///////////////////////////////////////////////////////////
	char BASED_CODE szFilter[] = "Biovision Heirarchy BVH (*.bvh)|*.bvh|"; 
	CFileDialog	*dialog;
	CString exten;
///////////////////////////////////////////////////////////////////////////////

	dialog = new CFileDialog(TRUE,"bvh",NULL, NULL,szFilter);
	if (dialog->DoModal() == IDOK)
	{
		exten = dialog->GetFileExt();//取得文件后缀名
		exten.MakeUpper();//把取得的后缀名字符全部大写 
		// HANDLE THE BIOVISION HEIRARCHY
		if (exten == "BVH")
		{
			DestroySkeleton(&m_Skeleton);//清空骨骼存储空间
			InitializeSkeleton();//初始化骨骼
			LoadBVH(dialog->GetPathName(),&m_Skeleton);//加载bvh数据文件
			m_OGLView.drawScene();//显示骨骼
		}
	}
	delete dialog;
}
/// OnFileLoadanim ////////////////////////////////////////////////////////////