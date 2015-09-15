///////////////////////////////////////////////////////////////////////////////
//
// MainFrm.h : interface of the CMainFrame class
//
// Purpose:	Implementation of Main Window of Hierarchical Animation System
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

#if !defined(AFX_MAINFRM_H__4B0629BD_2696_11D1_83A0_004005308EB5__INCLUDED_)
#define AFX_MAINFRM_H__4B0629BD_2696_11D1_83A0_004005308EB5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OGLView.h"
#include "Skeleton.h"

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CString m_ClassName;
	HCURSOR m_HArrow;
	COGLView  m_OGLView;
	int m_Animation_Direction;		// USED FOR FORWARD/BACK PLAYBACK
	int m_AnimSpeed;				// SPEED OF PLAYBACK IN FPS
	BOOL m_Animating;
	// for elapsed timing calculations
	DWORD		m_StartTime,m_ElapsedTime,m_previousElapsedTime;

// Operations
public:
	CMainFrame();

	// The Timing Member Functions
	// The following are used in timing calulations
	DWORD	ElapsedTime( void )
		{ return( m_ElapsedTime - m_StartTime );}
	DWORD	ElapsedTimeRender( void )
		{ return( m_ElapsedTime-m_previousElapsedTime);}

// Implementation
public:
	virtual ~CMainFrame();

protected:  // control bar embedded members
	t_Bone		m_Skeleton;
	CToolBar    m_wndToolBar;

	void InitializeSkeleton();

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBackFrame();
	afx_msg void OnPlayBack();
	afx_msg void OnStop();
	afx_msg void OnPlayForward();
	afx_msg void OnForwardFrame();
	afx_msg void OnFileLoadanim();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

/// Local Dialog Headers ///////////////////////////////////////////////////////
//
// MFC LIKES TO PUT THESE IN A SEPARATE FILE, BUT IT LIKE THEM IN CODE THAT
// USES THEM
//
///////////////////////////////////////////////////////////////////////////////		

#endif // !defined(AFX_MAINFRM_H__4B0629BD_2696_11D1_83A0_004005308EB5__INCLUDED_)

