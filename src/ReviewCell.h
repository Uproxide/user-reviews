#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include "ProfileReview.hpp"
#include "UploadReview.hpp"
#include "IconGetter.hpp"

using namespace geode::prelude;

class ReviewCell : public CCLayerColor {
    protected:

    GameLevelManager* m_gmgr;

    public:

        std::string user;

        int accountId;

        std::string reviewsID;

        bool init(std::string username, int accid, std::string reviewText, int reviewID, GJUserScore* score) {
            if (!CCLayerColor::init())
                return false;

            user = username;
            accountId = accid;
            reviewsID = std::to_string(reviewID);

            auto GAM = GJAccountManager::sharedState();
            m_gmgr = GameLevelManager::sharedState(); 

            //this->setOpacity(50);
            this->setOpacity(0);

            this->setContentSize(ccp(295, 39));

            this->setAnchorPoint(ccp(0, 0));

            auto background = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
	        background->setContentSize(this->getContentSize());
	        background->setOpacity(100);
	        background->setPosition({(this->getContentSize().width / 2), (this->getContentSize().height / 2) - 3});
	        this->addChild(background);

            auto menu = CCMenu::create();
            menu->setPosition(0,-10);

            auto playerBundle = CCMenu::create();

            playerBundle->setLayout(
                RowLayout::create()
                ->setGap(10)
                 ->setGrowCrossAxis(true)
                 ->setCrossAxisReverse(true)
                 ->setAutoScale(false)
                 ->setAxisAlignment(AxisAlignment::Start)
                );

            auto userTxt = CCLabelBMFont::create(username.c_str(), "goldFont.fnt");
            
            userTxt->setScale(.5);
            userTxt->setAnchorPoint(ccp(0, 0.5));
            userTxt->setID("playername");

            auto playerIcon = SimplePlayer::create(0);
            
            playerIcon->setScale(.5);
            playerIcon->setID("playericon");
            
            playerBundle->addChild(playerIcon);
            playerBundle->addChild(userTxt);
            playerBundle->updateLayout();

            CCMenuItemSpriteExtra* usernameButton;

            if(accid == 0) {
                usernameButton = CCMenuItemSpriteExtra::create(
                    playerBundle,
                    this,
                    menu_selector(ReviewCell::onUserNoAccID)
                );
            } else if(accid != 0) {
                usernameButton = CCMenuItemSpriteExtra::create(
                    playerBundle,
                    this,
                    menu_selector(ReviewCell::onUser)
                );
            }

            usernameButton->setPosition(6, 35);
            usernameButton->setAnchorPoint(ccp(0, 0.5));

            menu->addChild(usernameButton);
            background->addChild(menu);


            auto revText = CCLabelBMFont::create(reviewText.c_str(), "chatFont.fnt");

            revText->setPosition(4, 10);
            revText->setScale(.65);
            revText->setAnchorPoint(ccp(0, 0.5));

            background->addChild(revText);

            auto deleteSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
            deleteSpr->setScale(0.5);

            auto deleteBtn = CCMenuItemSpriteExtra::create(
                deleteSpr,
                this,
                menu_selector(ReviewCell::onDelete)
            );

            deleteBtn->setPosition(285, 25.5);
            

            IconGetter ig = IconGetter(0, m_gmgr->m_userInfoDelegate, playerBundle);

            m_gmgr->m_userInfoDelegate = &ig;

            if (GAM->m_username == score->m_userName ||
                GAM->m_username == user) {
                menu->addChild(deleteBtn);
            }

            
            m_gmgr->getGJUserInfo(accid);

            return true;
        }

        void onUserNoAccID(CCObject* sender) {
            GJSearchObject* obj = GJSearchObject::create(SearchType::Users, user);
            auto scene = CCTransitionFade::create(0.5f, LevelBrowserLayer::scene(obj));
            CCDirector::sharedDirector()->replaceScene(scene);
        }
        void onUser(CCObject* sender) {
            GJSearchObject* obj = GJSearchObject::create(SearchType::Users, user);
            auto scene = CCTransitionFade::create(0.5f, LevelBrowserLayer::scene(obj));
            CCDirector::sharedDirector()->replaceScene(scene);
        }

        void onDelete(CCObject* sender) {
           std::string url = fmt::format("https://uproxide.xyz/api/v1/reviews/deleteReview.php?id={}", reviewsID);
    
            web::AsyncWebRequest()
                .fetch(url)
                .text()
                .then([this](std::string const& json) {
                    Notification::create("Successfully deleted");
                    this->removeFromParent();
                })
                .expect([this](std::string const& json) {
                    log::error("something went wrong :3");
                });
        }

        static ReviewCell* create(std::string username, int aid, std::string reviewText, int reviewID, GJUserScore* score) {
            ReviewCell* pRet = new ReviewCell();
            if (pRet && pRet->init(username, aid, reviewText, reviewID, score)) {
                pRet->autorelease();
                return pRet;
            } else {
                delete pRet;
                return nullptr;
            }
        }

};