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

#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine
#include "GMEDefines.h"

#include "Platform.h"
#include "Wwise_IDs.h"		// IDs generated by Wwise
#include "Menu.h"
#include "DemoGMEVoiceChatPage.h"
#include "ButtonControl.h"
#include "DemoGMEPositioningPage.h"
#include "DemoGMERoutePage.h"
#include "DemoGMEServerAudioRoutePage.h"
#include <random>

//The game object ID used for this demo, it can be anything (except 0 and 1, as stated in the RegisterGameObj() doc)
#define MICRO_GAME_OBJECT_EFFECT        11
#define MICRO_GAME_OBJECT_MUSIC         12
#define MICRO_GAME_OBJECT_SOURCE        21
#define MICRO_GAME_OBJECT_SEND_MUSIC    51
#define MICRO_GAME_OBJECT_RECEIVEALL    66666666

namespace
{
	template<int Length>
	void GetIdStringFromControl(void* in_pSender, char (&out_String)[Length])
	{
		NumericControl* sender = (NumericControl*)in_pSender;
		snprintf(out_String, Length, "%u", (AkUInt32)sender->GetValue());
		out_String[Length - 1] = 0;
	}
}

#ifdef INTDEMO_GME
DemoGMEBasePage::DemoGMEBasePage(Menu& in_ParentMenu) : Page(in_ParentMenu, GMEWWisePlugin_GetVersion())
#else
DemoGMEBasePage::DemoGMEBasePage(Menu& in_ParentMenu) : Page(in_ParentMenu, "-")
#endif // INTDEMO_GME
{
}

void DemoGMEBasePage::AddIdControls()
{
	enum RoomIdValues
	{
		RoomIdValue_MinValue = 10000,
		RoomIdValue_MaxValue = 10000000,
		RoomIdValue_DefaultValue = 60000,
		RoomIdValue_Increment = 100,
	};

	NumericControl* roomID = new NumericControl(*this);
	roomID->SetLabel("Room ID:");
	roomID->SetMinValue(RoomIdValue_MinValue);
	roomID->SetMaxValue(RoomIdValue_MaxValue);
	roomID->SetIncrement(RoomIdValue_Increment);
	roomID->SetValue(RoomIdValue_DefaultValue);
	roomID->SetInitialIncrement(RoomIdValue_Increment);
	roomID->SetDelegate((PageMFP)&DemoGMEBasePage::RoomID_ValueChanged);
	roomID->CallDelegate(nullptr); // Force the RTPC to the correct initial value
	m_Controls.push_back(roomID);

	enum UserIdValues
	{
		UserIdValue_MinValue = 10000,
		UserIdValue_MaxValue = 10000000,
		UserIdValue_Increment = 100,
	};

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type>
		dist(UserIdValue_MinValue / UserIdValue_Increment, UserIdValue_MaxValue / UserIdValue_Increment);

	NumericControl* openID = new NumericControl(*this);
	openID->SetLabel("User ID:");
	openID->SetMinValue(UserIdValue_MinValue);
	openID->SetMaxValue(UserIdValue_MaxValue);
	openID->SetIncrement(UserIdValue_Increment);
	openID->SetValue(dist(rng) * UserIdValue_Increment);
	openID->SetInitialIncrement(UserIdValue_Increment);
	openID->SetDelegate((PageMFP)&DemoGMEBasePage::OpenID_ValueChanged);
	openID->CallDelegate(nullptr); // Force the RTPC to the correct initial value
	m_Controls.push_back(openID);
}

void DemoGMEBasePage::RoomID_ValueChanged(void* in_pSender, ControlEvent*)
{
	char roomID[128];
	::GetIdStringFromControl(in_pSender, roomID);
#ifdef INTDEMO_GME
	GMEWWisePlugin_SetRoomID(roomID);
#endif // INTDEMO_GME
}

void DemoGMEBasePage::OpenID_ValueChanged(void* in_pSender, ControlEvent*)
{
	char openID[128];
	::GetIdStringFromControl(in_pSender, openID);
#ifdef INTDEMO_GME
	GMEWWisePlugin_SetUserID(openID);
#endif // INTDEMO_GME
}


DemoGMEVoiceChatPage::DemoGMEVoiceChatPage( Menu& in_ParentMenu ): DemoGMEBasePage(in_ParentMenu)
{
	m_nLocalEffectStatus = 0;
	m_nLocalMusicStatus = 0;
	m_nSendStatus = 0;
	m_nSendMusicStatus = 0;
	m_nReceiveAllStatus = 0;
	m_nReceive1Status = 0;
	m_nReceive2Status = 0;

	m_recv1ID = 0;
	m_recv2ID = 0;

	m_szHelp  = "This page allows you to test sending and receiving sound through GME with the "
				"provided demo project.\n\n"
				"Refer to the documentation available in the Wwise Launcher.\n"
				"On the Plug-ins page, open the Help (?) menu and select \"Documentation\"\n";
}

bool DemoGMEVoiceChatPage::Init()
{
	// Load the sound bank
	AkBankID bankID; // Not used
	if (AK::SoundEngine::LoadBank("GME.bnk", bankID) != AK_Success)
	{
		SetLoadFileErrorMessage("GME.bnk");
		return false;
	}

	// Register the "Micro" game object on which the sound recorded from the micro will be played
	AK::SoundEngine::RegisterGameObj(MICRO_GAME_OBJECT_EFFECT, "LocalEffect");
	AK::SoundEngine::RegisterGameObj(MICRO_GAME_OBJECT_MUSIC, "LocalMusic");
	AK::SoundEngine::RegisterGameObj(MICRO_GAME_OBJECT_SOURCE, "Source");
	AK::SoundEngine::RegisterGameObj(MICRO_GAME_OBJECT_RECEIVEALL, "ReceiveAll");
	AK::SoundEngine::RegisterGameObj(MICRO_GAME_OBJECT_SEND_MUSIC, "MixMusic");

	// Initialize the page
	return Page::Init();
}

void DemoGMEVoiceChatPage::Release()
{
	AK::SoundEngine::PostEvent("Stop_Shotgun", MICRO_GAME_OBJECT_EFFECT);
	AK::SoundEngine::PostEvent("Stop_original", MICRO_GAME_OBJECT_MUSIC);
	AK::SoundEngine::PostEvent("Stop_gme_send", MICRO_GAME_OBJECT_SOURCE);
	AK::SoundEngine::PostEvent("Stop_gme_receive", MICRO_GAME_OBJECT_RECEIVEALL);
	AK::SoundEngine::PostEvent("Stop_gme_receive_3d", m_recv1ID);
	AK::SoundEngine::PostEvent("Stop_gme_receive_3d", m_recv2ID);
	AK::SoundEngine::PostEvent("Stop_gme_blend", MICRO_GAME_OBJECT_SEND_MUSIC);

	AK::SoundEngine::UnregisterGameObj(MICRO_GAME_OBJECT_EFFECT);
	AK::SoundEngine::UnregisterGameObj(MICRO_GAME_OBJECT_MUSIC);
	AK::SoundEngine::UnregisterGameObj(MICRO_GAME_OBJECT_SOURCE);
	AK::SoundEngine::UnregisterGameObj(MICRO_GAME_OBJECT_RECEIVEALL);
	AK::SoundEngine::UnregisterGameObj(m_recv1ID);
	AK::SoundEngine::UnregisterGameObj(m_recv2ID);

	AK::SoundEngine::UnloadBank("GME.bnk", nullptr);

	Page::Release();
}

void DemoGMEVoiceChatPage::InitControls()
{
	DemoGMEBasePage::AddIdControls();

	ToggleControl* mStreamProfile = new ToggleControl(*this);
	mStreamProfile = new ToggleControl(*this);
	mStreamProfile->SetLabel("Stream Profile: ");
	mStreamProfile->AddOption("Low Latency");
	mStreamProfile->AddOption("Standard");
	mStreamProfile->AddOption("HD");
	mStreamProfile->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::StreamProfile_ValueChanged);
	mStreamProfile->CallDelegate(NULL); // Force the RTPC to the correct initial value
	m_Controls.push_back(mStreamProfile);

	ButtonControl* newBtn = nullptr;

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Local Effect: {Stop}");
	newBtn->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::PlayLocalEffectButton_Pressed);
	m_Controls.push_back(newBtn);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Local Music: {Stop}");
	newBtn->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::PlayLocalMusicButton_Pressed);
	m_Controls.push_back(newBtn);

	ToggleControl* newToggle = new ToggleControl(*this);
	newToggle->SetLabel("Send: {Stop}");
	newToggle->AddOption("Original");
	newToggle->AddOption("Chipmunk");
	newToggle->AddOption("Monster");
	newToggle->AddOption("Vibrato1");
	newToggle->AddOption("Vibrato2");
	newToggle->AddOption("Bathroom");
	newToggle->AddOption("Church");
	newToggle->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::GMESendButton_Pressed);
	m_Controls.push_back(newToggle);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Send Music: {Stop}");
	newBtn->SetDelegate( (PageMFP)&DemoGMEVoiceChatPage::PlaySendMusicButton_Pressed);
	m_Controls.push_back(newBtn);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("Receive All: {Stop}");
	newBtn->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::GMERecAllButton_Pressed);
	m_Controls.push_back(newBtn);

	NumericControl* mRec1ID = new NumericControl(*this);
	mRec1ID->SetLabel("Receive 1: {Stop}...");
	mRec1ID->SetMinValue(10000);
	mRec1ID->SetMaxValue(10000000);
	mRec1ID->SetIncrement(100);
	mRec1ID->SetValue(50000);
	mRec1ID->SetInitialIncrement(100);
	mRec1ID->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::GMERec1Button_Pressed);
	mRec1ID->CallDelegate(nullptr); // Force the RTPC to the correct initial value
	m_Controls.push_back(mRec1ID);

	NumericControl* mRec2ID = new NumericControl(*this);
	mRec2ID->SetLabel("Receive 2: {Stop}...");
	mRec2ID->SetMinValue(10000);
	mRec2ID->SetMaxValue(10000000);
	mRec2ID->SetIncrement(100);
	mRec2ID->SetValue(50200);
	mRec2ID->SetInitialIncrement(100);
	mRec2ID->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::GMERec2Button_Pressed);
	mRec2ID->CallDelegate(nullptr); // Force the RTPC to the correct initial value
	m_Controls.push_back(mRec2ID);

	NumericControl* mTeamID = new NumericControl(*this);
	mTeamID->SetLabel("Team ID:");
	mTeamID->SetMinValue(0);
	mTeamID->SetMaxValue(10000000);
	mTeamID->SetIncrement(100);
	mTeamID->SetValue(0);
	mTeamID->SetInitialIncrement(100);
	mTeamID->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::TeamID_ValueChanged);
	mTeamID->CallDelegate(nullptr); // Force the RTPC to the correct initial value
	m_Controls.push_back(mTeamID);

	ToggleControl* mTeamMode = new ToggleControl(*this);
	mTeamMode->SetLabel("Team Mode:");
	mTeamMode->AddOption("Global");
	mTeamMode->AddOption("Team");
	mTeamMode->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::TeamMode_ValueChanged);
	mTeamMode->CallDelegate(nullptr); // Force the RTPC to the correct initial value
	m_Controls.push_back(mTeamMode);

	NumericControl* mRecvRange = new NumericControl(*this);
	mRecvRange->SetLabel("Recv Range:");
	mRecvRange->SetMinValue(0);
	mRecvRange->SetMaxValue(10000000);
	mRecvRange->SetIncrement(100);
	mRecvRange->SetValue(0);
	mRecvRange->SetInitialIncrement(100);
	mRecvRange->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::RecvRange_ValueChanged);
	mRecvRange->CallDelegate(nullptr); // Force the RTPC to the correct initial value
	m_Controls.push_back(mRecvRange);

	NumericControl* mPositionX = new NumericControl(*this);
	mPositionX->SetLabel("PositionX:");
	mPositionX->SetMinValue(-10000);
	mPositionX->SetMaxValue(10000);
	mPositionX->SetIncrement(30);
	mPositionX->SetValue(0);
	mPositionX->SetInitialIncrement(30);
	mPositionX->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::PositionX_ValueChanged);
	mPositionX->CallDelegate(nullptr); // Force the RTPC to the correct initial value
	m_Controls.push_back(mPositionX);

	newBtn = new ButtonControl(*this);
	newBtn->SetLabel("SetServerAudioRoute");
	newBtn->SetDelegate((PageMFP)&DemoGMEVoiceChatPage::GMESetServerAudioRouteButton_Pressed);
	m_Controls.push_back(newBtn);
}

void DemoGMEVoiceChatPage::StreamProfile_ValueChanged(void* in_pSender, ControlEvent*)
{
#ifdef INTDEMO_GME
	ToggleControl* sender = (ToggleControl*)in_pSender;
	string szStreamProfile = sender->SelectedText();
	if (!szStreamProfile.compare("Low Latency"))
	{
		GMEWWisePlugin_SetAudioStreamProfile(STREAMPROFILE_LOW_LATENCY);
	}
	else if (!szStreamProfile.compare("Standard"))
	{
		GMEWWisePlugin_SetAudioStreamProfile(STREAMPROFILE_STANDARD);
	}
	else if (!szStreamProfile.compare("HD"))
	{
		GMEWWisePlugin_SetAudioStreamProfile(STREAMPROFILE_HD);
	}
#endif // INTDEMO_GME
}

void DemoGMEVoiceChatPage::TeamID_ValueChanged(void* in_pSender, ControlEvent*)
{
#ifdef INTDEMO_GME
	NumericControl* sender = (NumericControl*)in_pSender;
	GMEWWisePlugin_SetRangeAudioTeamID((AkInt32)sender->GetValue());
#endif // INTDEMO_GME
}

void DemoGMEVoiceChatPage::TeamMode_ValueChanged(void* in_pSender, ControlEvent* in_pEvent)
{
#ifdef INTDEMO_GME
	ToggleControl* sender = (ToggleControl*)in_pSender;
	string szTeamMode = sender->SelectedText();
	if (!szTeamMode.compare("Global"))
	{
		GMEWWisePlugin_SetRangeAudioTeamMode(TEAMMODE_GLOBAL);
	}
	else if (!szTeamMode.compare("Team"))
	{
		GMEWWisePlugin_SetRangeAudioTeamMode(TEAMMODE_TEAM);
	}
#endif // INTDEMO_GME
}

void DemoGMEVoiceChatPage::RecvRange_ValueChanged(void* in_pSender, ControlEvent* in_pEvent)
{
#ifdef INTDEMO_GME
	NumericControl* sender = (NumericControl*)in_pSender;
	GMEWWisePlugin_SetRangeAudioRecvRange((AkInt32)sender->GetValue());
#endif // INTDEMO_GME
}

void DemoGMEVoiceChatPage::PositionX_ValueChanged(void* in_pSender, ControlEvent* in_pEvent)
{
#ifdef INTDEMO_GME
	NumericControl* sender = (NumericControl*)in_pSender;
	GMEWWisePlugin_SetSelfPosition((AkInt32)sender->GetValue(), 0, 0);
#endif // INTDEMO_GME
}

void DemoGMEVoiceChatPage::PlayLocalEffectButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	ButtonControl* sender = (ButtonControl*)in_pSender;
	int& status = m_nLocalEffectStatus;
	if (in_pEvent->iButton == UG_BUTTON3)
	{
		if (status == 0)	// Stopped
		{
			sender->SetLabel("Local Effect: {Play}");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PLAY_SHOTGUN*/"Play_Shotgun", MICRO_GAME_OBJECT_EFFECT);
		}
		else
		{
			sender->SetLabel("Local Effect: {Stop}");
			status = 0;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::STOP_SHOTGUN*/"Stop_Shotgun", MICRO_GAME_OBJECT_EFFECT);
		}
	}
	if (in_pEvent->iButton == UG_BUTTON4)
	{
		if (status == 1)	// Playing
		{
			sender->SetLabel("Local Effect: {Pause}");
			status = 2;
			AK::SoundEngine::PostEvent( /*AK::EVENTS::PAUSE_SHOTGUN*/"Pause_Shotgun", MICRO_GAME_OBJECT_EFFECT);
		}
		else if (status == 2)	// Paused
		{
			sender->SetLabel("Local Effect: {Play}");
			status = 1;
			AK::SoundEngine::PostEvent(/*"AK::EVENTS::RESUME_SHOTGUN*/"Resume_Shotgun", MICRO_GAME_OBJECT_EFFECT);
		}
	}
}


void DemoGMEVoiceChatPage::PlayLocalMusicButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	ButtonControl* sender = (ButtonControl*)in_pSender;
	int& status = m_nLocalMusicStatus;
	if (in_pEvent->iButton == UG_BUTTON3)
	{
		if (status == 0)	// Stopped
		{
			sender->SetLabel("Local Music: {Play}");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PLAY_ORIGINAL*/"Play_original", MICRO_GAME_OBJECT_MUSIC);
		}
		else
		{
			sender->SetLabel("Local Music: {Stop}");
			status = 0;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::STOP_ORIGINAL*/"Stop_original", MICRO_GAME_OBJECT_MUSIC);
		}
	}
	if (in_pEvent->iButton == UG_BUTTON4)
	{
		if (status == 1)	// Playing
		{
			sender->SetLabel("Local Music: {Pause}");
			status = 2;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PAUSE_ORIGINAL*/"Pause_original", MICRO_GAME_OBJECT_MUSIC);
		}
		else if (status == 2)	// Paused
		{
			sender->SetLabel("Local Music: {Play}");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::RESUME_ORIGINAL*/"Resume_original", MICRO_GAME_OBJECT_MUSIC);
		}
	}
}

void DemoGMEVoiceChatPage::GMESendButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
		ToggleControl* sender = (ToggleControl*)in_pSender;
		AkUniqueID effectID = AK_INVALID_UNIQUE_ID;
		string szLang = sender->SelectedText();
		if (!szLang.compare("Original"))
			effectID = AK_INVALID_UNIQUE_ID;
		else if (!szLang.compare("Chipmunk"))
			effectID = 654807232;
		else if (!szLang.compare("Monster"))
			effectID = 1431265797;
		else if (!szLang.compare("Vibrato1"))
			effectID = 109045632;
		else if (!szLang.compare("Vibrato2"))
			effectID = 1291433344;
		else if (!szLang.compare("Bathroom"))
			effectID = 535667444;
		else if (!szLang.compare("Church"))
			effectID = 2662085913;
		else
			effectID = AK_INVALID_UNIQUE_ID;

		int& status = m_nSendStatus;
		if (in_pEvent->iButton == UG_BUTTON1)
		{
#ifdef INTDEMO_GME
			RoutePage* pg = new RoutePage(*m_pParentMenu, MICRO_GAME_OBJECT_SOURCE);
			m_pParentMenu->StackPage(pg);
#endif // INTDEMO_GME
		}
		if (in_pEvent->iButton == UG_DPAD_LEFT || in_pEvent->iButton == UG_DPAD_RIGHT)
		{
			AK::SoundEngine::SetActorMixerEffect(985826208, 0, effectID);
		}

		if (in_pEvent->iButton == UG_BUTTON3)
		{
			if (status == 0)    // Stopped
			{
				sender->SetLabel("Send: {Play}");
				status = 1;
				AK::SoundEngine::SetActorMixerEffect(985826208, 0, effectID);
				AK::SoundEngine::PostEvent(/*AK::EVENTS::PLAY_GME_SEND*/"Play_gme_send", MICRO_GAME_OBJECT_SOURCE);
			}
			else
			{
				sender->SetLabel("Send: {Stop}");
				status = 0;
				AK::SoundEngine::PostEvent(/*AK::EVENTS::STOP_GME_SEND*/"Stop_gme_send", MICRO_GAME_OBJECT_SOURCE);
			}
		}
		if (in_pEvent->iButton == UG_BUTTON4)
		{
			if (status == 1)    // Playing
			{
				sender->SetLabel("Send: {Pause}");
				status = 2;
				AK::SoundEngine::PostEvent(/*AK::EVENTS::PAUSE_GME_SEND*/"Pause_gme_send", MICRO_GAME_OBJECT_SOURCE);
			}
			else if (status == 2)    // Paused
			{
				sender->SetLabel("Send: {Play}");
				status = 1;
				AK::SoundEngine::PostEvent(/*AK::EVENTS::RESUME_GME_SEND*/"Resume_gme_send", MICRO_GAME_OBJECT_SOURCE);
			}
		}
}

void DemoGMEVoiceChatPage::GMESetServerAudioRouteButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
#ifdef INTDEMO_GME
	ButtonControl* sender = (ButtonControl*)in_pSender;
	int& status = m_nSendMusicStatus;
	if (in_pEvent->iButton == UG_BUTTON1) {
		AudioRoutePage* pg = new AudioRoutePage(*m_pParentMenu);
		m_pParentMenu->StackPage(pg);
	}
#endif // INTDEMO_GME
}


void DemoGMEVoiceChatPage::PlaySendMusicButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	ButtonControl* sender = (ButtonControl*)in_pSender;
	int& status = m_nSendMusicStatus;
	if (in_pEvent->iButton == UG_BUTTON1)
	{
#ifdef INTDEMO_GME
		RoutePage* pg = new RoutePage(*m_pParentMenu, MICRO_GAME_OBJECT_SEND_MUSIC);
		m_pParentMenu->StackPage(pg);
#endif // INTDEMO_GME
	}
	else if (in_pEvent->iButton == UG_BUTTON3)
	{
		if (status == 0)    // Stopped
		{
			sender->SetLabel("Send Music: {Play}");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PLAY_GME_BLEND*/"Play_gme_blend", MICRO_GAME_OBJECT_SEND_MUSIC);
		}
		else
		{
			sender->SetLabel("Send Music: {Stop}");
			status = 0;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::STOP_GME_BLEND*/"Stop_gme_blend", MICRO_GAME_OBJECT_SEND_MUSIC);
		}
	}
	else if (in_pEvent->iButton == UG_BUTTON4)
	{
		if (status == 1)    // Playing
		{
			sender->SetLabel("Send Music: {Pause}");
			status = 2;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PAUSE_GME_BLEND*/"Pause_gme_blend", MICRO_GAME_OBJECT_SEND_MUSIC);
		}
		else if (status == 2)    // Paused
		{
			sender->SetLabel("Send Music: {Play}");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::RESUME_GME_BLEND*/"Resume_gme_blend", MICRO_GAME_OBJECT_SEND_MUSIC);
		}
	}
}

void DemoGMEVoiceChatPage::GMERecAllButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	ButtonControl* sender = (ButtonControl*)in_pSender;
	int& status = m_nReceiveAllStatus;
	if (in_pEvent->iButton == UG_BUTTON3)
	{
		if (status == 0)	// Stopped
		{
			sender->SetLabel("Receive All: {Play}");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PLAY_GME_RECEIVE*/"Play_gme_receive", MICRO_GAME_OBJECT_RECEIVEALL);
		}
		else
		{
			sender->SetLabel("Receive All: {Stop}");
			status = 0;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::STOP_GME_RECEIVE*/"Stop_gme_receive", MICRO_GAME_OBJECT_RECEIVEALL);
		}
	}
	if (in_pEvent->iButton == UG_BUTTON4)
	{
		if (status == 1)	// Playing
		{
			sender->SetLabel("Receive All: {Pause}");
			status = 2;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PAUSE_GME_RECEIVE*/"Pause_gme_receive", MICRO_GAME_OBJECT_RECEIVEALL);
		}
		else if (status == 2)	// Paused
		{
			sender->SetLabel("Receive All: {Play}");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::RESUME_GME_RECEIVE*/"Resume_gme_receive", MICRO_GAME_OBJECT_RECEIVEALL);
		}
	}
}

void DemoGMEVoiceChatPage::GMERec1Button_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	NumericControl* sender = (NumericControl*)in_pSender;
	int& status = m_nReceive1Status;
	if (in_pEvent == nullptr)  // First Try
	{
		NumericControl* sender = (NumericControl*)in_pSender;
		m_recv1ID = (AkUInt64)sender->GetValue();
		AK::SoundEngine::RegisterGameObj(m_recv1ID, "Receive1");

		return;
	}

	if (in_pEvent->iButton == UG_DPAD_LEFT || in_pEvent->iButton == UG_DPAD_RIGHT)
	{
		if (status == 0)    // Stopped
		{
			AkUInt64 recvID = (AkUInt64)sender->GetValue();
			if (recvID == m_recv2ID)
			{
				if (in_pEvent->iButton == UG_DPAD_LEFT)
				{
					recvID -= (AkUInt64)sender->GetIncrement();
				}
				else
				{
					recvID += (AkUInt64)sender->GetIncrement();
				}
				sender->SetValue((AkReal64)recvID);
			}

			AK::SoundEngine::UnregisterGameObj(m_recv1ID);
			m_recv1ID = (AkUInt64)sender->GetValue();
			AK::SoundEngine::RegisterGameObj(m_recv1ID, "Receive1");
		}
		else
		{
			sender->SetValue((AkReal64)m_recv1ID);
		}
	}

	if (in_pEvent->iButton == UG_BUTTON1)
	{
		DemoGMEPositioningPage* pg = new DemoGMEPositioningPage(*m_pParentMenu, m_recv1ID);
		m_pParentMenu->StackPage(pg);
	}

	if (in_pEvent->iButton == UG_BUTTON3)
	{
		if (status == 0)	// Stopped
		{
			char recvID[128];
			sprintf(recvID, "%llu", (unsigned long long)m_recv1ID);
#ifdef INTDEMO_GME
			GMEWWisePlugin_ReceivePlugin_SetReceiveOpenIDWithGameObjectID(m_recv1ID, recvID);
#endif // INTDEMO_GME

			sender->SetLabel("Receive 1: {Play}...");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PLAY_GME_RECEIVE_3D*/"Play_gme_receive_3d", m_recv1ID);
		}
		else
		{
			sender->SetLabel("Receive 1: {Stop}...");
			status = 0;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::STOP_GME_RECEIVE_3D*/"Stop_gme_receive_3d", m_recv1ID);
		}
	}
	if (in_pEvent->iButton == UG_BUTTON4)
	{
		if (status == 1)	// Playing
		{
			sender->SetLabel("Receive 1: {Pause}...");
			status = 2;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PAUSE_GME_RECEIVE_3D*/"Pause_gme_receive_3d", m_recv1ID);
		}
		else if (status == 2)	// Paused
		{
			sender->SetLabel("Receive 1: {Play}...");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::RESUME_GME_RECEIVE_3D*/"Resume_gme_receive_3d", m_recv1ID);
		}
	}
}

void DemoGMEVoiceChatPage::GMERec2Button_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	NumericControl* sender = (NumericControl*)in_pSender;
	int& status = m_nReceive2Status;

	if (in_pEvent == nullptr)  // First Try
	{
		NumericControl* sender = (NumericControl*)in_pSender;
		m_recv2ID = (AkUInt64)sender->GetValue();
		AK::SoundEngine::RegisterGameObj(m_recv2ID, "Receive2");

		return;
	}

	if (in_pEvent->iButton == UG_DPAD_LEFT || in_pEvent->iButton == UG_DPAD_RIGHT)
	{
		if (status == 0)    // Stopped
		{
			AkUInt64 recvID = (AkUInt64)sender->GetValue();
			if (recvID == m_recv1ID)
			{
				if (in_pEvent->iButton == UG_DPAD_LEFT)
				{
					recvID -= (AkUInt64)sender->GetIncrement();
				}
				else
				{
					recvID += (AkUInt64)sender->GetIncrement();
				}
				sender->SetValue((AkReal64)recvID);
			}


			AK::SoundEngine::UnregisterGameObj(m_recv2ID);
			m_recv2ID = (AkUInt64)sender->GetValue();
			AK::SoundEngine::RegisterGameObj(m_recv2ID, "Receive2");
		}
		else
		{
			sender->SetValue((AkReal64)m_recv2ID);
		}
	}

	if (in_pEvent->iButton == UG_BUTTON1)
	{
		DemoGMEPositioningPage* pg = new DemoGMEPositioningPage(*m_pParentMenu, m_recv2ID);
		m_pParentMenu->StackPage(pg);
	}
	if (in_pEvent->iButton == UG_BUTTON3)
	{
		if (status == 0)	// Stopped
		{
			char recvID[128];
			sprintf(recvID, "%llu", (unsigned long long)m_recv2ID);
#ifdef INTDEMO_GME
			GMEWWisePlugin_ReceivePlugin_SetReceiveOpenIDWithGameObjectID(m_recv2ID, recvID);
#endif // INTDEMO_GME

			sender->SetLabel("Receive 2: {Play}...");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PLAY_GME_RECEIVE_3D*/"Play_gme_receive_3d", m_recv2ID);
		}
		else
		{
			sender->SetLabel("Receive 2: {Stop}...");
			status = 0;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::STOP_GME_RECEIVE_3D*/"Stop_gme_receive_3d", m_recv2ID);
		}
	}
	if (in_pEvent->iButton == UG_BUTTON4)
	{
		if (status == 1)    // Playing
		{
			sender->SetLabel("Receive 2: {Pause}...");
			status = 2;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::PAUSE_GME_RECEIVE_3D*/"Pause_gme_receive_3d", m_recv2ID);
		}
		else if (status == 2)	// Paused
		{
			sender->SetLabel("Receive 2: {Play}...");
			status = 1;
			AK::SoundEngine::PostEvent(/*AK::EVENTS::RESUME_GME_RECEIVE_3D*/"Resume_gme_receive_3d", m_recv2ID);
		}
	}
}

void DemoGMEVoiceChatPage::GMESFXVolumeButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	NumericControl* sender = (NumericControl*)in_pSender;

	AK::SoundEngine::SetRTPCValue(/*AK::GAME_PARAMETERS::SFXVOLUME*/"SFXVolume", (AkRtpcValue)sender->GetValue());
}

void DemoGMEVoiceChatPage::GMEReceiveVolumeButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	NumericControl* sender = (NumericControl*)in_pSender;

	AK::SoundEngine::SetRTPCValue(/*AK::GAME_PARAMETERS::RECEIVEVOLUME*/"ReceiveVolume", (AkRtpcValue)sender->GetValue());
}

void DemoGMEVoiceChatPage::GMESendVolumeButton_Pressed(void* in_pSender, ControlEvent* in_pEvent)
{
	NumericControl* sender = (NumericControl*)in_pSender;

	AK::SoundEngine::SetRTPCValue(/*AK::GAME_PARAMETERS::SENDVOLUME*/"SendVolume", (AkRtpcValue)sender->GetValue());
}
