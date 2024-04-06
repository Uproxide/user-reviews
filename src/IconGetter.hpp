#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include <matjson.hpp>
#include "ProfileReview.hpp"
#include <vector>

using namespace geode::prelude;


class IconGetter : public UserInfoDelegate {
protected:

    static IconGetter* s_shared;

    GameLevelManager* m_glmgr;
    int m_accountID;
    //UserInfoDelegate* const m_oldUID;
    CCMenu* m_playerbundle;
    SimplePlayer* m_playericon;
    CCLabelBMFont* m_playername;
    matjson::Value m_allinfo;
    std::vector<CCMenu*> m_allmenus;
    
    int getIcon(GJUserScore* score) {
        IconType type = score->m_iconType;
        switch(type) {
            default: return score->m_playerCube;
            case IconType::Cube: return score->m_playerCube;
            case IconType::Ship: return score->m_playerShip;
            case IconType::Ball: return score->m_playerBall;
            case IconType::Ufo: return score->m_playerUfo;
            case IconType::Wave: return score->m_playerWave;
            case IconType::Robot: return score->m_playerRobot;
            case IconType::Spider: return score->m_playerSpider;
            case IconType::Swing: return score->m_playerSwing; 
            case IconType::Jetpack: return score->m_playerJetpack;
    }
}

public:

    IconGetter() {log::debug("make shared thing of icongetter");}
    ~IconGetter() {}

    static IconGetter* shared();
    

    void setStuff(int accid, CCMenu* pbun) {
        m_allmenus.insert(m_allmenus.end(), pbun);
        m_allinfo[std::to_string(accid)] = (m_allmenus.size() - 1);
        log::debug("{}", m_allinfo.dump());


        // m_accountID = accid;
        // m_oldUID = olduid;
        // log::info("call le setStuff");
        // m_playerbundle = pbun;
        m_glmgr = GameLevelManager::sharedState();
        // m_playericon = static_cast<SimplePlayer*>(pbun->getChildByID("playericon"));
        // m_playername = static_cast<CCLabelBMFont*>(pbun->getChildByID("playername"));
        m_glmgr->getGJUserInfo(accid);
    }
    

    // Functions for UserInfoDelegate!
    void getUserInfoFinished(GJUserScore* score) {
        log::debug("{}", m_allinfo[std::to_string(score->m_accountID)].as_int());
        CCMenu* playerbundle = m_allmenus[m_allinfo[std::to_string(score->m_accountID)].as_int()];
        SimplePlayer* playericon = static_cast<SimplePlayer*>(playerbundle->getChildByID("playericon"));
        CCLabelBMFont* playername = static_cast<CCLabelBMFont*>(playerbundle->getChildByID("playername"));
        auto gmgr = GameManager::sharedState();
        playericon->updatePlayerFrame(getIcon(score), static_cast<IconType>(score->m_iconType));
        playericon->setColor(gmgr->colorForIdx(score->m_color1));
		playericon->setSecondColor(gmgr->colorForIdx(score->m_color2));
		playericon->setGlowOutline(gmgr->colorForIdx(score->m_color3));
		playericon->enableCustomGlowColor(gmgr->colorForIdx(score->m_color3));
        if(!score->m_glowEnabled) playericon->disableGlowOutline();
    }

    void getUserInfoFailed(int) {
        // don't do anything
    }


    void userInfoChanged(GJUserScore* score) {
        CCMenu* playerbundle = m_allmenus[m_allinfo[score->m_accountID].as_int()];
        SimplePlayer* playericon = static_cast<SimplePlayer*>(playerbundle->getChildByID("playericon"));
        CCLabelBMFont* playername = static_cast<CCLabelBMFont*>(playerbundle->getChildByID("playername"));
        auto gmgr = GameManager::sharedState();
        playericon->updatePlayerFrame(getIcon(score), static_cast<IconType>(score->m_iconType));
        playericon->setColor(gmgr->colorForIdx(score->m_color1));
		playericon->setSecondColor(gmgr->colorForIdx(score->m_color2));
		playericon->setGlowOutline(gmgr->colorForIdx(score->m_color3));
		playericon->enableCustomGlowColor(gmgr->colorForIdx(score->m_color3));
        if(!score->m_glowEnabled) playericon->disableGlowOutline();
    }
};