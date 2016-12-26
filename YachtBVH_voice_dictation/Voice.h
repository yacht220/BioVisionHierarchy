/*************************************************
FOR VOICE
*************************************************/

#include <sphelper.h>                           // Contains definitions of SAPI functions
#include "stdafx.h"
#include <string.h>

#define GID_DICTATION   0           // Dictation grammar has grammar ID 0


#pragma once
CComPtr<ISpRecoGrammar>         v_cpDictationGrammar; // Pointer to our grammar object
CComPtr<ISpRecoContext>         v_cpRecoCtxt;   // Pointer to our recognition context

/******************************************************************************
* ProcessRecoEvent *
*------------------*
*   Description:
*
******************************************************************************/
int ProcessRecoEvent( HWND hWnd)
{
	USES_CONVERSION;//调用X2X的宏的函数，要在调用前增加这个宏
    CSpEvent event;  // Event helper class
	CSpDynamicString dstrText;//识别后的语音转换成的字符，放入此数据结构中

    // Loop processing events while there are any in the queue
    if (event.GetFrom(v_cpRecoCtxt) == S_OK)
    {
        // Look at recognition event only
        if(event.eEventId==SPEI_RECOGNITION)
        {
			event.RecoResult()->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &dstrText, NULL);
			DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,dstrText.m_psz,-1,NULL,0,NULL,FALSE);
			char *p,//存储转换后字符的临时指针
				 *keyWord1="right",*keyWord2="left",*keyWord3="run",*keyWord4="walk",*keyWord5="sit",
				 *keyWord6="stand",*keyWord7="slow",*keyWord8="dance";
			p = new char[dwNum];
			WideCharToMultiByte(CP_OEMCP,NULL,dstrText.m_psz,-1,p,dwNum,NULL,FALSE);
			if(strcmp(p,keyWord1)==0)
			{
				delete p,keyWord1,keyWord2,keyWord3,keyWord4,keyWord5,keyWord6,keyWord7,keyWord8;
				return 1;//right flag
			}
			if(strcmp(p,keyWord2)==0)
			{
				delete p,keyWord1,keyWord2,keyWord3,keyWord4,keyWord5,keyWord6,keyWord7,keyWord8;
				return 2;//left flag
			}
			if(strcmp(p,keyWord3)==0)
			{
				delete p,keyWord1,keyWord2,keyWord3,keyWord4,keyWord5,keyWord6,keyWord7,keyWord8;
				return 3;//run flag
			}
			if(strcmp(p,keyWord4)==0)
			{
				delete p,keyWord1,keyWord2,keyWord3,keyWord4,keyWord5,keyWord6,keyWord7,keyWord8;
				return 4;//walk flag
			}
			if(strcmp(p,keyWord5)==0)
			{
				delete p,keyWord1,keyWord2,keyWord3,keyWord4,keyWord5,keyWord6,keyWord7,keyWord8;
				return 5;//sit flag
			}
			if(strcmp(p,keyWord6)==0)
			{
				delete p,keyWord1,keyWord2,keyWord3,keyWord4,keyWord5,keyWord6,keyWord7,keyWord8;
				return 6;//stand flag
			}
			if(strcmp(p,keyWord7)==0)
			{
				delete p,keyWord1,keyWord2,keyWord3,keyWord4,keyWord5,keyWord6,keyWord7,keyWord8;
				return 7;//slow flag
			}
		    if(strcmp(p,keyWord8)==0)
			{
				delete p,keyWord1,keyWord2,keyWord3,keyWord4,keyWord5,keyWord6,keyWord7,keyWord8;
				return 8;//dance flag
			}
		}
    }
	return FALSE;
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
    CComPtr<ISpAudio> cpAudio;

    while ( 1 )
    {
        // create a recognition engine
        hr = cpRecoEngine.CoCreateInstance(CLSID_SpInprocRecognizer);
        if ( FAILED( hr ) )
        {
            break;
        }

		// create default audio object
        hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &cpAudio);
		if ( FAILED( hr ) )
        {
            break;
        }

        // set the input for the engine
        hr = cpRecoEngine->SetInput(cpAudio, TRUE);
		if ( FAILED( hr ) )
        {
            break;
        }
        hr = cpRecoEngine->SetRecoState( SPRST_ACTIVE );
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

        // Load our grammar
        hr = v_cpRecoCtxt->CreateGrammar( GID_DICTATION, &v_cpDictationGrammar );
        if (FAILED(hr))
        {
            break;
        }
        hr = v_cpDictationGrammar->LoadDictation(NULL, SPLO_STATIC);
        if ( FAILED( hr ) )
        {
            break;
        }

        // Set rules to active, we are now listening for dictation
        hr = v_cpDictationGrammar->SetDictationState( SPRS_ACTIVE );
        if ( FAILED( hr ) )
        {
            break;
        }

        break;
    }
    return ( hr );
}



