#include "stdafx.h"
#include "ResManager.h"
#include "ResourceDef.h"
#include "Star97LoadingLayer.h"
#include "Star97GameLayer.h"
#include "Star97Game.h"

namespace GameStar97
{

CStar97Game::CStar97Game(CGameDirector * pDirector)
	:m_pGameDirector(pDirector)
	,m_pResManager(NULL)
	,m_pLoadingLayer(NULL)
	,m_pMainLayer(NULL)
	,m_bInGame(false)
{
}

CStar97Game::~CStar97Game()
{
}

bool CStar97Game::Init()
{
	m_pResManager = new CResManager(m_pGameDirector);
	m_pResManager->CreateConfigHelper();
	LoadGameCfg();

	//std::string strPlist = m_pResManager->GetConfigHelper()->m_sLoadingPak+"images/pak/loading.plist";
	//std::string strTexture = m_pResManager->GetConfigHelper()->m_sLoadingPak+"images/pak/loading.png";
	//CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(strPlist.c_str(), strTexture.c_str());

	return true;
}

void CStar97Game::Uninit()
{
	if (m_pLoadingLayer)
	{
		m_pGameDirector->RemoveGameLayer(m_pLoadingLayer->GetLayerID());
		if (m_pLoadingLayer->getParent())
		{
			m_pLoadingLayer->getParent()->removeChild(m_pLoadingLayer);
		}

		m_pLoadingLayer->release();
		m_pLoadingLayer = NULL;
	}

	if (m_pResManager)
	{
		m_pResManager->Clear(false);
	}
	CC_SAFE_DELETE(m_pResManager);
}

bool CStar97Game::IsInGame()
{
	return m_bInGame;
}

CRoomSprite CStar97Game::CreateRoomSprite()
{
	CRoomSprite room;

	//房间标签背景
	CCSize titlebkSize;
	CCSprite* pTitleBK = NULL;

	if (m_pResManager->GenerateNodeByCfgID(eSpriteType_Cfg, Star97_Room_Titlebk, pTitleBK))
	{
		titlebkSize = pTitleBK->getContentSize();
		pTitleBK->setPosition(ccp(WINDOW_WIDTH / 2, 180));

		room.m_pRoomNameBK = pTitleBK;
	}

	//房间标签
	if (pTitleBK)
	{
		//标签
		CCLabelTTF *plabel = CCLabelTTF::create("", FONT_NAME, FONT_SIZE_ROOMNAME);

		if (m_pResManager->GenerateNodeByCfgID(eSpriteType_Cfg, Star97_Font_Room_Name, plabel))
		{
			plabel->setPosition(ccp(titlebkSize.width / 2, titlebkSize.height / 2 - 10));
			pTitleBK->addChild(plabel);
			room.m_pRoomNameLable = plabel;
		}
		else
		{
			ERROR_CHECK;
		}
	}

	return room;
}

CTableSprite CStar97Game::CreateTableSprite()
{
	CTableSprite tableSprite;
	CCSize szBKSize;

	if (m_pResManager->GenerateNodeByCfgID(eSpriteType_Cfg, Star97_Room_Table, tableSprite.m_pTableBK))
	{
		szBKSize = tableSprite.m_pTableBK->getContentSize();
		tableSprite.m_pTableBK->setAnchorPoint(ccp(0.5f, 0.5f));
		tableSprite.m_pTableBK->setScale(1.4f);
	}
	if (m_pResManager->GenerateNodeByCfgID(eSpriteType_Cfg, Star97_Room_Table_P, tableSprite.m_pTableP))
	{
		szBKSize = tableSprite.m_pTableP->getContentSize();
		tableSprite.m_pTableP->setAnchorPoint(ccp(0.0f, 0.0f));
		tableSprite.m_pTableP->setPosition(CCPoint(0,0));
		tableSprite.m_pTableP->setZOrder(tableSprite.m_pTableBK->getZOrder()+1);
		tableSprite.m_pTableBK->addChild(tableSprite.m_pTableP);
	}
	//桌子编号
	if (m_pResManager->GenerateNodeByCfgID(eSpriteType_Base, Star97_Room_Table_Num_Font, tableSprite.m_pTableNum))
	{
		szBKSize = tableSprite.m_pTableBK->getContentSize();
		tableSprite.m_pTableNum->setAnchorPoint(ccp(0.5, 0.5));
		tableSprite.m_pTableNum->setPosition(ccp(120.f,240.f));
		//tableSprite.m_pTableNum->setScale(0.3f);
		if (NULL != tableSprite.m_pTableBK)
		{
			tableSprite.m_pTableBK->addChild(tableSprite.m_pTableNum);
		}
	}

	//游戏桌子编号
	if (m_pResManager->GenerateNodeByCfgID(eSpriteType_Base, Star97_Room_Table_Num_Font, tableSprite.m_pTableNumP))
	{
		tableSprite.m_pTableNumP->setAnchorPoint(ccp(0.5, 0.5));
		tableSprite.m_pTableNumP->setPosition(ccp(120.f,240.f));
		//tableSprite.m_pTableNumP->setScale(0.3f);
		if (NULL != tableSprite.m_pTableP)
		{
			tableSprite.m_pTableP->addChild(tableSprite.m_pTableNumP);
		}
	}

	//座位
	for (int n = 0; n < 1; n++)
	{
		CSeatSprite seat;

		CCLabelTTF *titleButton = CCLabelTTF::create("", "Arial", 30);
		if (NULL == titleButton)
		{
			ERROR_CHECK;
			break;
		}
		titleButton->setColor(ccc3(159, 168, 176));

		CCScale9Sprite *backgroundButton = NULL;
		if (!m_pResManager->GenerateNodeByCfgID(eSpriteType_Cfg, Star97_Room_Sit, backgroundButton))
		{
			ERROR_CHECK;
			break;
		}

		CCControlButton* pButton = CMoveableButton::create(titleButton, backgroundButton);
		if (NULL == pButton)
		{
			ERROR_CHECK;
			break;
		}

		CCScale9Sprite *backgroundHighlightedButton = NULL;
		if (m_pResManager->GenerateNodeByCfgID(eSpriteType_Cfg, Star97_Room_Sit, backgroundHighlightedButton))
		{
			pButton->setBackgroundSpriteForState(backgroundHighlightedButton, CCControlStateHighlighted);
		}

		CCScale9Sprite *backgroundDisibleButton = NULL;
		if (m_pResManager->GenerateNodeByCfgID(eSpriteType_Cfg, Star97_Room_Sit_D, backgroundDisibleButton))
		{
			pButton->setBackgroundSpriteForState(backgroundDisibleButton, CCControlStateDisabled);
		}

		pButton->setPosition(CCPoint(134.0f,105.0f));
		pButton->setAdjustBackgroundImage(false);
		//pButton->setZoomOnTouchDown(false);
		pButton->setTag(n);
		if (tableSprite.m_pTableP)
		{
			pButton->setZOrder(tableSprite.m_pTableP->getZOrder()+4);
		}

		tableSprite.m_pTableBK->addChild(pButton);
		seat.m_pSeat = pButton;

		//更新名称
		CCLabelTTF* pUserName = NULL; 

		if (m_pResManager->GenerateNodeByCfgID(eSpriteType_Cfg, Star97_Font_Room_Sit_Name, pUserName))
		{

		}

		if (pUserName && tableSprite.m_pTableBK)
		{
			szBKSize = tableSprite.m_pTableBK->getContentSize();
			pUserName->setString("");
			pUserName->setPosition(ccp(szBKSize.width*0.5,0));
			pUserName->setHorizontalAlignment(kCCTextAlignmentCenter);
			pUserName->setVerticalAlignment(kCCVerticalTextAlignmentCenter);
			tableSprite.m_pTableBK->addChild(pUserName);
			seat.m_pNameLabel = pUserName;
		}

		tableSprite.m_vSeatSprites.push_back(seat);
	}

	return tableSprite;
}

bool CStar97Game::EnterGame()
{
	if (m_bInGame)
	{
		return true;
	}

	if (m_pResManager)
	{
		m_pResManager->Clear(false);
	}

	LoadGameCfg();

	CCScene *pGameScene = m_pGameDirector->GetGameScene();
	if (NULL == pGameScene)
	{
		ERROR_CHECK;
		return false;
	}

	bool bHaveTitle = m_pGameDirector->m_bUseIPC;
	//bHaveTitle = false;

#if (CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
	////标题栏层
	//if (bHaveTitle)
	//{
	//	CTitleLayer* pSubLayer = new CTitleLayer();
	//	pSubLayer->SetTypeID(eLayerType_MainTitle);
	//	pSubLayer->autorelease();
	//	pSubLayer->SetGameDirector(m_pGameDirector);
	//	pSubLayer->setContentSize(CCSizeMake(designSize.width, designSize.height));
	//	pSubLayer->SetOrginPos(ccp(0, designSize.height));
	//	pSubLayer->setZOrder(10);
	//	pSubLayer->OnLayerIn(NULL, eLayerAni_Null);
	//	m_pGameDirector->AddGameLayer(pSubLayer);
	//	pSubLayer->SetResManager(m_pResManager);
	//	pGameScene->addChild(pSubLayer);

	//	CGameLayer* pLayer = m_pGameDirector->GetGameLayerByType(eLayerType_Title);
	//	if (pLayer)
	//	{
	//		pLayer->OnLayerIn(NULL, eLayerAni_Null);
	//	}
	//}
#endif

	//主界面
	if (m_pMainLayer&&m_pMainLayer->IsInit())
	{
		m_pMainLayer->Clear();

		if (m_pMainLayer->getParent())
		{
			m_pMainLayer->getParent()->removeChild(m_pMainLayer);
		}

		m_pMainLayer->release();
		m_pMainLayer = NULL;
	}
	{
		m_pMainLayer = new CStar97GameLayer(this, m_pResManager);
		m_pMainLayer->retain();
		CGameLayer* pSubLayer = m_pMainLayer;
		pSubLayer->SetTypeID(eLayerType_Main);
		pSubLayer->autorelease();
		pSubLayer->SetResManager(m_pResManager);
		pSubLayer->SetGameDirector(m_pGameDirector);
		pSubLayer->setContentSize(CCSizeMake(WINDOW_WIDTH, CONTENTS_HEIGHT));

		if (m_pGameDirector->m_bUseIPC)
			pSubLayer->SetOrginPos(ccp(0, BORDER_BOTTOM));
		pSubLayer->setZOrder(1);
		pSubLayer->OnLayerIn(NULL, eLayerAni_Null);

		pGameScene->addChild(pSubLayer);
	}

	//加载界面
	if (m_pLoadingLayer)
	{
		if (m_pLoadingLayer->getParent())
		{
			m_pLoadingLayer->getParent()->removeChild(m_pLoadingLayer);
		}

		m_pLoadingLayer->release();
		m_pLoadingLayer = NULL;
	}

	m_pLoadingLayer = new CStar97LoadingLayer(m_pResManager);
	m_pLoadingLayer->SetGameDirector(m_pGameDirector);
	m_pLoadingLayer->retain();
	CCScene *pLoadingScene = CCScene::create();
	pLoadingScene->addChild(m_pLoadingLayer);

#if (CC_TARGET_PLATFORM==CC_PLATFORM_WIN32)
	if (bHaveTitle)
	{
		CTitleLayer* pLayer = m_pGameDirector->createTitleLayer(eLayerType_Title);
		if (pLayer)
		{
			pLoadingScene->addChild(pLayer);
			pLayer->OnLayerIn(NULL, eLayerAni_Null);
		}
	}
#endif

	if (m_pGameDirector->m_bUseIPC)
	{
		CCDirector::sharedDirector()->runWithScene(pLoadingScene);
		m_pLoadingLayer->OnLayerIn(NULL, eLayerAni_Null);
	}
	else
	{
		CCDirector::sharedDirector()->replaceScene(pLoadingScene);
	}

	m_bInGame = true;

	return true;
}

void CStar97Game::LeaveGame()
{
	m_bInGame = false;
}

void CStar97Game::OnAudioChange()
{
	if (m_pGameDirector->m_bCfgEnableBackgroundMusic)
	{
		if (m_bInGame)
		{
			SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
		}
	}
	else
	{
		SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	}
}

void CStar97Game::OnUserSit(unsigned short wChairID)
{
	//if (wChairID >= MAX_CHAIR)
	//	return;
	CCLOG("user chair ID %d sit!", wChairID);

	tagLocalUser* pUser = m_pGameDirector->GetTableUserData(wChairID);
	if (pUser == NULL)
		return;

	if (m_pMainLayer&&m_pMainLayer->IsInit())
	{
		m_pMainLayer->updateUserScore(pUser->nUserScore);
	}
}

void CStar97Game::OnUserStateChange(unsigned short wChairID, int nOldState, int nNewState)
{
	//if (m_pMainLayer&&m_pMainLayer->IsInit())
	//{
	//	m_pMainLayer->OnUserStateChange(wChairID, nNewState, false);
	//}
}

void CStar97Game::OnUserLeave(unsigned short wChairID)
{
	//if (wChairID >= MAX_CHAIR)
	//	return;

	CCLOG("user chair ID %d leave!", wChairID);

}


void CStar97Game::OnTouchOption()
{
	if (m_pMainLayer && m_pMainLayer->IsInit())
		m_pMainLayer->touchSetting();
}

void CStar97Game::OnTouchClose()
{
	m_pGameDirector->End();
}

bool CStar97Game::OnEventGameOption(BYTE cbGameStatus, bool bAllowLookon)
{
	if (m_pMainLayer)
		m_pMainLayer->SetGameStatus(cbGameStatus);

	return true;
}

bool CStar97Game::OnEventGameScene(bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	if (m_pMainLayer && m_pMainLayer->IsInit())
		return m_pMainLayer->OnEventGameScene(bLookonOther, pBuffer, wDataSize);

	return true; 
}

bool CStar97Game::OnGameMessage(WORD wMainCmd, WORD wSubCmd, const void * pBuffer, WORD wDataSize)
{
	if (wMainCmd == MDM_GF_GAME && m_pMainLayer && m_pMainLayer->IsInit())
	{
		return m_pMainLayer->OnGameMessage(wSubCmd, pBuffer, wDataSize);
	}

	return true;
}

void CStar97Game::OnGB2312Message(const char* sMsg, int nType)
{
	std::string sUTF8 = CStringHelper::GB2312ToUTF8(sMsg);
	OnUTF8Message(sUTF8.c_str(), nType);
}

void CStar97Game::OnUTF8Message(const char* sMsg, int nType)
{
	if (nType==SMT_INFO/*0 != (SMT_EJECT & nType)*/)
	{
		if (m_pMainLayer&&m_pMainLayer->IsInit())
		{
			//m_pMainLayer->ShowGameMessage(sMsg);
		}
	}
}

void CStar97Game::OnMeScoreChange(int nScore)
{
	if (!m_pMainLayer || !m_pMainLayer->IsInit())
		return ;

	tagLocalUser* pUser = m_pGameDirector->GetTableUserData(0);
	if (pUser != NULL)
	{
		m_pMainLayer->updateUserScore(pUser->nUserScore);
	}
}

///////////////////////////////////////////////////////////////////////

bool CStar97Game::LoadGameCfg()
{
	CConfigHelper* configHelper = m_pResManager->GetConfigHelper();
	if (configHelper == NULL)
	{
		ERROR_CHECK;
		return false;
	}

	if (m_pGameDirector->m_bUseIPC)
	{
		if (m_pGameDirector->m_bCfgCfgPacketLoading)
		{
			configHelper->m_sLoadingPak = "|";
			configHelper->m_sLoadingPak +=  m_pGameDirector->m_sCfgLocalLoadingFix;
			configHelper->m_sLoadingPak += ".pak|";
		}
		else
		{
			configHelper->m_sLoadingPak = m_pGameDirector->m_sCfgLocalLoadingFix;
		}

		if (m_pGameDirector->m_bCfgResPacketLoading)
		{
			configHelper->m_sResPak = "|";
			configHelper->m_sResPak +=  m_pGameDirector->m_sCfgLocalResFix;
			configHelper->m_sResPak += ".pak|";
		}
		else
		{
			configHelper->m_sResPak = m_pGameDirector->m_sCfgLocalResFix;
		}
	}
	else
	{
		for (std::list<tagKindCfg*>::iterator iter = m_pGameDirector->m_pResManager->GetConfigHelper()->m_vGameKindCfgs.begin(); iter != m_pGameDirector->m_pResManager->GetConfigHelper()->m_vGameKindCfgs.end(); iter++)
		{
			tagKindCfg* pKind = *iter;

			if (m_pGameDirector->m_nCurKindID == pKind->nID)
			{
				configHelper->m_sLoadingPak = pKind->sLoadingFix;
				configHelper->m_sResPak = pKind->sResFix;
				break;
			}
		}

	}

	{
		if (m_pGameDirector->m_bCfgNoNetworkMode)
		{
			m_pResManager->GetConfigHelper()->m_bIgnoreServerOnlyFlag = true;
			CCFileUtils::sharedFileUtils()->setPopupNotify(false);
		}
		else
		{
			CCFileUtils::sharedFileUtils()->setPopupNotify(m_pGameDirector->m_nCfgDebugMode > 0);
		}

		m_pResManager->LoadConfigFromFile();
	}

	if (m_pGameDirector->m_nCfgDebugMode > 0)
	{
		cocos2d::CCLog("start loading no network(%d), loading pak(%s), res pak(%s), debug(%d) size(%.2f, %.2f, %.2f) freq(%.2f) \r\n", 
			m_pGameDirector->m_bCfgNoNetworkMode?1:0,
			m_pResManager->GetConfigHelper()->m_sLoadingPak.c_str(),
			m_pResManager->GetConfigHelper()->m_sResPak.c_str(),
			m_pGameDirector->m_nCfgDebugMode,
			WINDOW_WIDTH, WINDOW_HEIGHT,
			m_pGameDirector->m_fCfgZoom,
			1/m_pGameDirector->m_fFreq);
	}

	return true;
}

}
