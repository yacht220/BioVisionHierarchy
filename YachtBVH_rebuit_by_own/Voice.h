/*************************************************
FOR VOICE
*************************************************/

#include <sphelper.h>                           // Contains definitions of SAPI functions
#include "stdafx.h"
#include "YachtBVHgram.h"                       // This header is created by the grammar
                                                // compiler and has our rule ids

#define GRAMMARID1      161                     // Arbitrary grammar id


#pragma once
CComPtr<ISpRecoGrammar>         v_cpCmdGrammar; // Pointer to our grammar object
CComPtr<ISpRecoContext>         v_cpRecoCtxt;   // Pointer to our recognition context

/******************************************************************************
* ExecuteCommand *
*----------------*
*   Description:
*       Called to Execute commands that have been identified by the speech engine.
*
******************************************************************************/
void ExecuteCommand(ISpPhrase *pPhrase, HWND hWnd, int *eFlag)
{
    SPPHRASE *pElements;

    // Get the phrase elements, one of which is the rule id we specified in
    // the grammar.  Switch on it to figure out which command was recognized.
    if (SUCCEEDED(pPhrase->GetPhrase(&pElements)))
    {        
        switch ( pElements->Rule.ulId )
        {
            case VID_Command:
            {
                switch( pElements->pProperties->vValue.ulVal )
                {
                    case VID_Right:
						*eFlag=1;
                        break;
					case VID_Left:
						*eFlag=2;
						break;
					case VID_Run:
						*eFlag=3;
						break;
					case VID_Walk:
						*eFlag=4;
						break;
					case VID_Fight:
						*eFlag=5;
						break;
					case VID_Stand:
						*eFlag=6;
						break;
					case VID_WalkSlowly:
						*eFlag=7;
						break;
					case VID_Dance:
						*eFlag=8;
						break;
					case VID_RunSlowly:
						*eFlag=9;
						break;

                }
            }
            break;
        }
        // Free the pElements memory which was allocated for us
        ::CoTaskMemFree(pElements);
    }

}

/******************************************************************************
* ProcessRecoEvent *
*------------------*
*   Description:
*
******************************************************************************/
void ProcessRecoEvent( HWND hWnd,int *pFlag)
{
	 CSpEvent event;  // Event helper class

    // Loop processing events while there are any in the queue
    if (event.GetFrom(v_cpRecoCtxt) == S_OK)
    {
        // Look at recognition event only
        if (event.eEventId==SPEI_RECOGNITION)
        {
                ExecuteCommand(event.RecoResult(), hWnd, pFlag);
        }
    }

}

/******************************************************************************
* InitSAPI *
*----------*
*   Description:
*       Called once to get SAPI started.
*
******************************************************************************/
HRESULT InitSAPI( HWND hWnd)
{
    HRESULT hr = S_OK;
    //CComPtr<ISpAudio> cpAudio;
	CComPtr<ISpRecognizer> cpRecoEngine;

    while ( 1 )
    {
        // create a recognition engine
        hr = cpRecoEngine.CoCreateInstance(CLSID_SpSharedRecognizer);
        if ( FAILED( hr ) )
        {
            break;
        }

        // create the dictation recognition context
        hr = cpRecoEngine->CreateRecoContext( &v_cpRecoCtxt );
        if ( FAILED( hr ) )
        {
            break;
        }

	    // Tell SR what types of events interest us.  Here we only care about dictation
        // recognition.
        hr = v_cpRecoCtxt->SetInterest( SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION) );
        if ( FAILED( hr ) )
        {
            break;
        }

        // Load our grammar, which is the compiled form of simple.xml bound into this executable as a
        // user defined ("SRGRAMMAR") resource type.
        hr = v_cpRecoCtxt->CreateGrammar( GRAMMARID1, &v_cpCmdGrammar);
        if (FAILED(hr))
        {
            break;
        }
        hr = v_cpCmdGrammar->LoadCmdFromResource(NULL, MAKEINTRESOURCEW(IDR_CMD_CFG),
                                                 L"SRGRAMMAR", MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                                                 SPLO_DYNAMIC);
        if ( FAILED( hr ) )
        {
            break;
        }

        // Set rules to active, we are now listening for commands
        hr = v_cpCmdGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE );
        if ( FAILED( hr ) )
        {
            break;
        }

        break;
    }
    return ( hr );
}



