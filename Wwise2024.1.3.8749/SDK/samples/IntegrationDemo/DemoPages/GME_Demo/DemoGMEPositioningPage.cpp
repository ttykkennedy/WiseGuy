/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/

#include "stdafx.h"

#include <math.h>
#include "Menu.h"
#include "MovableChip.h"
#include "DemoGMEPositioningPage.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine

//If you get a compiling error here, it means the file wasn't generated with the banks.  Did you generate the soundbanks before compiling?
#include "Wwise_IDs.h"		


/////////////////////////////////////////////////////////////////////
// DemoGMEPositioningPage Public Methods
/////////////////////////////////////////////////////////////////////

DemoGMEPositioningPage::DemoGMEPositioningPage(Menu& in_ParentMenu, AkUInt64 in_gameobject)
	: Page( in_ParentMenu, "Positioning")
	, m_pChip( NULL )
	, m_fGameObjectX( 0 )
	, m_fGameObjectZ( 0 )
	, m_fWidth( 0.0f )
	, m_fHeight( 0.0f )
	, m_gameObject(in_gameobject)
{
	m_szHelp = "3D positioning sound";

	char strTitle[1024];
	sprintf(strTitle, "Position of [%lld]", (unsigned long long)in_gameobject);
	SetTitle(strTitle);
}

bool DemoGMEPositioningPage::Init()
{
	return Page::Init();
}

void DemoGMEPositioningPage::Release()
{

	Page::Release();
}

#define HELICOPTER_CLONE_X_OFFSET (50)
#define POSITION_RANGE (200.0f)

float DemoGMEPositioningPage::PixelsToAKPos_X(float in_X)
{
	return ((in_X / m_fWidth) - 0.5f) * POSITION_RANGE;
}

float DemoGMEPositioningPage::PixelsToAKPos_Y(float in_y)
{
	return -((in_y / m_fHeight) - 0.5f) * POSITION_RANGE;
}

void DemoGMEPositioningPage::UpdateGameObjPos()
{
	float x, y;
	m_pChip->GetPos(x, y);
	
	// Single Helicopter.

	// Converting X-Y UI into X-Z world plan.
	AkVector position;
	m_fGameObjectX = position.X = PixelsToAKPos_X(x);
	position.Y = 0;
	m_fGameObjectZ = position.Z = PixelsToAKPos_Y(y);
	AkVector orientationFront;
	orientationFront.Z = 1;
	orientationFront.Y = orientationFront.X = 0;
	AkVector orientationTop;
	orientationTop.X = orientationTop.Z = 0;
	orientationTop.Y = 1;

	AkSoundPosition soundPos;
	soundPos.Set(position, orientationFront, orientationTop);
	AK::SoundEngine::SetPosition(m_gameObject, soundPos);
}

bool DemoGMEPositioningPage::Update()
{
	//Always update the MovableChip

	bool bMoved = false;
	UniversalInput::Iterator it;
	for ( it = m_pParentMenu->Input()->Begin(); it != m_pParentMenu->Input()->End(); it++ )
	{
		// Skip this input device if it's not connected
		if ( ! it->IsConnected() )
			continue;

		m_pChip->Update(*it);

		bMoved = true;
	}

	if ( bMoved )
	{
		UpdateGameObjPos();
	}

	return Page::Update();
}

void DemoGMEPositioningPage::Draw()
{
	Page::Draw();

	m_pChip->Draw();

	char strBuf[50];
	snprintf( strBuf, 50, "X: %.2f\nZ: %.2f", m_fGameObjectX, m_fGameObjectZ );

	static int s_nOffset = 2 * GetLineHeight( DrawStyle_Text );

	DrawTextOnScreen( strBuf, 5, m_pParentMenu->GetHeight() - s_nOffset, DrawStyle_Text );

	// Display instructions at the bottom of the page
	int iInstructionsY = m_pParentMenu->GetHeight() - 3 * GetLineHeight( DrawStyle_Text );
	DrawTextOnScreen( "(Press <<UG_BUTTON2>> To Go Back...)", m_pParentMenu->GetWidth() / 4, iInstructionsY, DrawStyle_Text );
}

bool DemoGMEPositioningPage::OnPointerEvent( PointerEventType in_eType, int in_x, int in_y )
{
	if ( in_eType == PointerEventType_Moved )
	{
		m_pChip->SetPos( (float) in_x, (float) in_y );
		UpdateGameObjPos();
	}

	return Page::OnPointerEvent( in_eType, in_x, in_y );
}

void DemoGMEPositioningPage::InitControls()
{
	m_pChip = new MovableChip(*this);
	m_pChip->SetLabel( "o" );
	m_pChip->UseJoystick(UG_STICKRIGHT);
	m_pChip->SetNonLinear();

	m_fWidth = (float)m_pParentMenu->GetWidth() - (float)m_pChip->GetRightBottomMargin();
	m_fHeight = (float)m_pParentMenu->GetHeight() - (float)m_pChip->GetRightBottomMargin();
}

