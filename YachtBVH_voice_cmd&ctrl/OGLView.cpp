///////////////////////////////////////////////////////////////////////////////
//
// OGLView.cpp : implementation file
//
// Purpose:	Implementation of OpenGL Window of Hierarchical Animation System
//
// Created:
//		JL 9/1/97		
// Versions:
//		1.01	12/20/97	Fix perspective in OpenGL Resize Code
//		1.02	1/10/97		Change to display code to handle skeletal hierarchy
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
#include "YachtBVH.h"
#include "OGLView.h"
#include "Voice.h"
#include "LoadAnim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ROTATE_SPEED		1.0		// SPEED OF ROTATION
///////////////////////////////////////////////////////////////////////////////
// These values need to be available globally
    // Light values and coordinates
    GLfloat  ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat  diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat	 lightPos[] = { -50.0f, 50.0f, 50.0f, 0.0f };

	// Transformation matrix to project shadow
    M3DMatrix44f shadowMat;

	// Rotation amounts
    //static GLfloat yRot = 0.0f;

	bool rFlag = FALSE, lFlag = FALSE;//标识语音控制中的左右转动

	int rLoop = 0, lLoop = 0;//标识因语音控制引起的转动次数
///////////////////////////////////////////////////////////////////////////////
// COGLView

COGLView::COGLView()
{
	// INITIALIZE THE MODE KEYS
	m_Skeleton = NULL;

	//Voice////////////////////////////////////////////////
	//要使用CoInitialize()或者CoInitializeEx()这两个函数的其中一个来创建COM或者DCOM对象
	::CoInitialize(NULL);
	/////////////////////////////////////////////////////
}

COGLView::~COGLView()
{
    //Voice////////////////////////////////////////////////
	//要使用对应的UnInitialize()或者UnInitializeEx()来释放COM或者DCOM接口
	::CoUninitialize();
	/////////////////////////////////////////////////////
}

BOOL COGLView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, t_Bone *skeleton, CCreateContext* pContext) 
{
	m_Skeleton = skeleton;
	m_SelectedBone = skeleton;
	/////////////////////////////////////////////////////////
	///Voice///////////////////////////////////////////////
	InitSAPI(NULL);
	///////////////////////////////////////////////////////
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BEGIN_MESSAGE_MAP(COGLView, CWnd)
	//{{AFX_MSG_MAP(COGLView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COGLView message handlers

BOOL COGLView::SetupPixelFormat(HDC hdc)
{
/// Local Variables ///////////////////////////////////////////////////////////
    PIXELFORMATDESCRIPTOR pfd, *ppfd;
    int pixelformat;
///////////////////////////////////////////////////////////////////////////////
    ppfd = &pfd;

    ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
    ppfd->nVersion = 1;
    ppfd->dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    ppfd->dwLayerMask = PFD_MAIN_PLANE;
    ppfd->iPixelType = PFD_TYPE_RGBA;
    ppfd->cColorBits = 16;
    ppfd->cDepthBits = 16;
    ppfd->cAccumBits = 0;
    ppfd->cStencilBits = 0;

    pixelformat = ChoosePixelFormat(hdc, ppfd);

    if ((pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0) {
        MessageBox("ChoosePixelFormat failed", "Error", MB_OK);
        return FALSE;
    }

    if (pfd.dwFlags & PFD_NEED_PALETTE) {
        MessageBox("Needs palette", "Error", MB_OK);
        return FALSE;
    }

    if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) {
        MessageBox("SetPixelFormat failed", "Error", MB_OK);
        return FALSE;
    }

    return TRUE;
}

int COGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) //***************************************
{
/// Local Variables ///////////////////////////////////////////////////////////
	RECT rect;
///////////////////////////////////////////////////////////////////////////////
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
    m_hDC = ::GetDC(m_hWnd);
    if (!SetupPixelFormat(m_hDC))
		PostQuitMessage (0);
	
    m_hRC = wglCreateContext(m_hDC);
    wglMakeCurrent(m_hDC, m_hRC);
    GetClientRect(&rect);
    initializeGL(rect.right, rect.bottom);
	drawScene();
	return 0;
}

/* OpenGL code */

///////////////////////////////////////////////////////////////////////////////
// Function:	resize
// Purpose:		This code handles the windows resize for OpenGL
// Arguments:	Width and heights of the view window
////////////////////n//////////////////////////////////////////////////////////
GLvoid COGLView::resize( GLsizei width, GLsizei height )
{
// Local Variables /////////////////////////////////////////////////////////
    GLfloat aspect;
///////////////////////////////'/////////////////////////////////////////////

    glViewport(0, 0, width, height);

    aspect = (GLfloat)width/(GLfloat)height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(30.0, aspect,1, 2000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glTranslatef(0,-5,-500);
	glRotatef(30,1,0,0);	
}
//// resize /////////////////////////////////////////////////////////////////

GLvoid COGLView::initializeGL(GLsizei width, GLsizei height)
{
/// Local Variables ///////////////////////////////////////////////////////////
    //GLfloat aspect;

	// Any three points on the ground (counter clockwise order)
    M3DVector3f points[3] = {{ -30.0f, 0.0f, -20.0f },
                            { -30.0f, 0.0f, 20.0f },
                            { 40.0f, 0.0f, 20.0f }};
///////////////////////////////////////////////////////////////////////////////
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

     // Setup light parameters
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientLight);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	
	// Mostly use material tracking
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

	// Get the plane equation from three points on the ground
    M3DVector4f vPlaneEquation;
    m3dGetPlaneEquation(vPlaneEquation, points[0], points[1], points[2]);

    // Calculate projection matrix to draw shadow on the ground
    m3dMakePlanarShadowMatrix(shadowMat, vPlaneEquation, lightPos);
}


///////////////////////////////////////////////////////////////////////////////
// Function:	DrawBone
// Purpose:		Draws the Bone model
// Arguments:	None
///////////////////////////////////////////////////////////////////////////////
void COGLView::DrawBone(void)
{
		glutSolidSphere(3.0f, 10, 10);
}
//// DrawBone /////////////////////////////////////////////////////////////////


/////////'/////////////////////////////////////////////////////////////////////
// Function:	drawSkeleton
// Purpose:		Actually draws the Skeleton it is recurqive
// Arguments:	None
///////////////////////////////.///////////////m////////////////+//////////////
GLvoid COGLView::drawSkeleton(t_Bone *rootBone,int nShadow)
{
/// Local Variables //////////////////////////////////////////////////////////
    int loop;
	t_Bone *curBone;
////////////////////////////////////////+//////////////////////////////////////
	if(nShadow == 0)
		glColor3f(1,0,0);
	else
		glColor3f(0,0,0);


	curBone = rootBone->children;//同一个父骨骼下可以有多个子骨骼，curBone++相当于让指针curBone指向另一个子骨骼
	for (loop = 0; loop < rootBone->childCnt; loop++)
	{
		glPushMatrix();

		glTranslatef(curBone->trans.x, curBone->trans.y, curBone->trans.z);

		// Set observer's orientation and position
		glRotatef(curBone->rot.z, 0.0f, 0.0f, 1.0f);
		glRotatef(curBone->rot.x, 1.0f, 0.0f, 0.0f);
		glRotatef(curBone->rot.y, 0.0f, 1.0f, 0.0f);

		DrawBone();

		// CHECK IF THIS BONE HAS CHILDREN, IF SO RECURSIVE CALL/这样才能保证所有的子骨骼都画完
		if (curBone->childCnt > 0)
			drawSkeleton(curBone,nShadow); 
		//跳出递归，回到上一级继续执行，依次类推

		glPopMatrix();

		curBone++;
	}
}
//// drawSkeleton /////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
// Draw a ground
GLvoid COGLView::DrawGround(GLvoid)
    {
		glBegin(GL_QUADS);
		  glNormal3f(0.0f, 1.0f, 0.0f);
		  glColor3f(0,1,0);
          glVertex3f(-150.0f, 0.0f, -150.0f);
          glVertex3f(150.0f, 0.0f, -150.0f);
		  glVertex3f(150.0f, 0.0f, 150.0f);
		  glVertex3f(-150.0f, 0.0f, 150.0f);
        glEnd();
    }


///////////////////////////////////////////////////////////////////////////////
// Function:	drawScene
// Purpose:		Actually draw the OpenGL Scene
// Arguments:	None
///////////////////////////////////////////////////////////////////////////////
GLvoid COGLView::drawScene(GLvoid)
{
/// Local Variables ///////////////////////////////////////////////////////////
		int flag=0;//标识不同的语音命令
///////////////////////////////////////////////////////////////////////////////

	if (m_Skeleton->rot.y  > 360.0f) m_Skeleton->rot.y  -= 360.0f;
    if (m_Skeleton->rot.x   > 360.0f) m_Skeleton->rot.x   -= 360.0f;
    if (m_Skeleton->rot.z > 360.0f) m_Skeleton->rot.z -= 360.0f;

	/////////////////////////////////////////////////////////////////
	///////Voice/////////////////////////////////////////////////
	ProcessRecoEvent(NULL,&flag);
	switch(flag)
	{
		case 1://right flag
            rFlag = TRUE;
			break;
		case 2://left flag
            lFlag = TRUE;
			break;
		case 3://run flag
			DestroySkeleton(m_Skeleton);//清空骨骼存储空间
			LoadBVH("..\\YachtBVH_voice_cmd&ctrl\\BVH\\Run.bvh",m_Skeleton);//加载bvh数据文件
			break;
		case 4://walk flag
			DestroySkeleton(m_Skeleton);//清空骨骼存储空间
			LoadBVH("..\\YachtBVH_voice_cmd&ctrl\\BVH\\Walk-Normal.bvh",m_Skeleton);//加载bvh数据文件
			break;
		case 5://fight flag
			DestroySkeleton(m_Skeleton);//清空骨骼存储空间
			LoadBVH("..\\YachtBVH_voice_cmd&ctrl\\BVH\\DOUBLE PUNCH 1.bvh",m_Skeleton);//加载bvh数据文件
			break;
		case 6://stand flag
			DestroySkeleton(m_Skeleton);//清空骨骼存储空间
			LoadBVH("..\\YachtBVH_voice_cmd&ctrl\\BVH\\T.bvh",m_Skeleton);//加载bvh数据文件
			break;
		case 7://walk slowly flag
			DestroySkeleton(m_Skeleton);//清空骨骼存储空间
			LoadBVH("..\\YachtBVH_voice_cmd&ctrl\\BVH\\Walk-Slow.bvh",m_Skeleton);//加载bvh数据文件
			break;
		case 8://dance flag
			DestroySkeleton(m_Skeleton);//清空骨骼存储空间
			LoadBVH("..\\YachtBVH_voice_cmd&ctrl\\BVH\\ballet1.bvh",m_Skeleton);//加载bvh数据文件
			break;
		case 9://run slowly flag
			DestroySkeleton(m_Skeleton);//清空骨骼存储空间
			LoadBVH("..\\YachtBVH_voice_cmd&ctrl\\BVH\\Jog.bvh",m_Skeleton);//加载bvh数据文件
			break;
	}
	///////////////////////////////////////////////////////////////


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

	// Do the rotation for viewing
	/*glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	yRot+=0.5;
	if(yRot>360.0f)
		yRot=0.0;*/

	//Set light position
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

	//Draw a ground///////////////
    DrawGround();

	//语音控制左右命令引起的逐步转动

	if(rLoop == 45)
	{
		rFlag = FALSE;
		rLoop = 0;
	}
	if(lLoop == 45)
	{
		lFlag = FALSE;
		lLoop = 0;
	}	
	
	if(rFlag == TRUE)
	{		
		m_Skeleton->rot.y -=2.0f;
		rLoop++;
	}
	
	if(lFlag == TRUE)
	{
		m_Skeleton->rot.y += 2.0f;
		lLoop++;
	}

    // Get ready to draw the shadow and the ground
    // First disable lighting and save the projection state
    glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
    glPushMatrix();

	// Multiply by shadow projection matrix
    glMultMatrixf((GLfloat *)shadowMat);

	glTranslatef(m_Skeleton->trans.x, m_Skeleton->trans.y, m_Skeleton->trans.z);

	// Set root skeleton's orientation and position
    glRotatef(m_Skeleton->rot.z, 0.0f, 0.0f, 1.0f);
	glRotatef(m_Skeleton->rot.x, 1.0f, 0.0f, 0.0f);
    glRotatef(m_Skeleton->rot.y, 0.0f, 1.0f, 0.0f);

	drawSkeleton(m_Skeleton,1);

	// Restore the projection to normal
    glPopMatrix();
	// Restore lighting state variables
    glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

    glPushMatrix();

	glTranslatef(m_Skeleton->trans.x, m_Skeleton->trans.y, m_Skeleton->trans.z);

	// Set root skeleton's orientation and position
    glRotatef(m_Skeleton->rot.z, 0.0f, 0.0f, 1.0f);
	glRotatef(m_Skeleton->rot.x, 1.0f, 0.0f, 0.0f);
    glRotatef(m_Skeleton->rot.y, 0.0f, 1.0f, 0.0f);

	drawSkeleton(m_Skeleton,0);

    glPopMatrix();

	glPopMatrix();

    glFinish();

    SwapBuffers(m_hDC);
}
//// drawScene //////////////////////////////////////////////////////



void COGLView::OnDestroy() 
{
	CWnd::OnDestroy();
	if (m_hRC)
		wglDeleteContext(m_hRC);
    if (m_hDC)
		::ReleaseDC(m_hWnd,m_hDC);
    m_hRC = 0;
    m_hDC = 0;	
}

void COGLView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	drawScene();
	// Do not call CWnd::OnPaint() for painting messages
}

void COGLView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// STORE OFF THE KIT POINT AND SETTINGS FOR THE MOVEMENT LATER
	m_mousepos = point;
	m_Grab_Rot_X = 	m_SelectedBone->rot.x;
	m_Grab_Rot_Y = 	m_SelectedBone->rot.y;
	m_Grab_Rot_Z = 	m_SelectedBone->rot.z;
	m_Grab_Trans_X = 	m_SelectedBone->trans.x;
	m_Grab_Trans_Y = 	m_SelectedBone->trans.y;
	m_Grab_Trans_Z = 	m_SelectedBone->trans.z;
	CWnd::OnLButtonDown(nFlags, point);
}

void COGLView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// STORE OFF THE KIT POINT AND SETTINGS FOR THE MOVEMENT LATER
	m_mousepos = point;
	m_Grab_Rot_X = 	m_SelectedBone->rot.x;
	m_Grab_Rot_Y = 	m_SelectedBone->rot.y;
	m_Grab_Rot_Z = 	m_SelectedBone->rot.z;
	m_Grab_Trans_X = 	m_SelectedBone->trans.x;
	m_Grab_Trans_Y = 	m_SelectedBone->trans.y;
	m_Grab_Trans_Z = 	m_SelectedBone->trans.z;
	CWnd::OnRButtonDown(nFlags, point);
}


///////////////////////////////////////////////////////////////////////////////
// Function:	OnMouseMove
// Purpose:		Handler for the mouse.  Handles movement when pressed
// Arguments:	Flags for key masks and point
///////////////////////////////////////////////////////////////////////////////
void COGLView::OnMouseMove(UINT nFlags, CPoint point) 
{
//	UpdateStatusBar(0);
	if (nFlags & MK_LBUTTON > 0)
	{
		// IF I AM HOLDING THE 'CTRL' BUTTON TRANSLATE
		if ((nFlags & MK_CONTROL) > 0)
		{
//			UpdateStatusBar(2);
			if ((point.x - m_mousepos.x) != 0)
			{
				m_SelectedBone->trans.x = m_Grab_Trans_X + (.05f * (point.x - m_mousepos.x));
				drawScene();
			}
			if ((point.y - m_mousepos.y) != 0)
			{
				m_SelectedBone->trans.y = m_Grab_Trans_Y - (.05f * (point.y - m_mousepos.y));
				drawScene();
			}
		}	
		// ELSE "SHIFT" ROTATE THE ROOT
		else if ((nFlags & MK_SHIFT) > 0)
		{
//			UpdateStatusBar(1);
			if ((point.x - m_mousepos.x) != 0)
			{
				m_SelectedBone->rot.y = m_Grab_Rot_Y + ((float)ROTATE_SPEED * (point.x - m_mousepos.x));
				drawScene();
			}
			if ((point.y - m_mousepos.y) != 0)
			{
				m_SelectedBone->rot.x = m_Grab_Rot_X + ((float)ROTATE_SPEED * (point.y - m_mousepos.y));
				drawScene();
			}
		}
	}
	else if ((nFlags & MK_RBUTTON) == MK_RBUTTON)
	{
		if ((nFlags & MK_CONTROL) > 0)
		{
//			UpdateStatusBar(2);
			if ((point.x - m_mousepos.x) != 0)
			{
				m_SelectedBone->trans.z = m_Grab_Trans_Z + (.1f * (point.x - m_mousepos.x));
				drawScene();
			}
		}
		else if ((nFlags & MK_SHIFT) > 0)
		{
//			UpdateStatusBar(1);
			if ((point.x - m_mousepos.x) != 0)
			{
				m_SelectedBone->rot.z = m_Grab_Rot_Z + ((float)ROTATE_SPEED * (point.x - m_mousepos.x));
				drawScene();
			}
		}
	}
	
	CWnd::OnMouseMove(nFlags, point);
}
//// OnMouseMove //////////////////////////////////////////////////////