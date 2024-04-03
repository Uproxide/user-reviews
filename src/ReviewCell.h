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

    GameLevelManager* m_gmgr

    public:

        std::string user;

        std::string reviewsID;

        bool init(std::string username, std::string reviewText, int reviewID, GJUserScore* score) {
            if (!CCLayerColor::init())
                return false;

            user = username;
            reviewsID = std::to_string(reviewID);

            auto GAM = GJAccountManager::sharedState();
            m_gMgr = GameLevelManager::sharedState(); 

            m_gMgr->m_userInfoDelegate = this


            this->setOpacity(50);

            this->setContentSize(ccp(300, 32));

            this->setAnchorPoint(ccp(0, 0));

            auto menu = CCMenu::create();
            menu->setPosition(0,-10);

            auto userTxt = CCLabelBMFont::create(username.c_str(), "goldFont.fnt");
            userTxt->setScale(.5);

            auto usernameButton = CCMenuItemSpriteExtra::create(
                userTxt,
                this,
                menu_selector(ReviewCell::onUser)
            );


            usernameButton->setPosition(4, 35);
            usernameButton->setAnchorPoint(ccp(0, .5));
            

            menu->addChild(usernameButton);
            this->addChild(menu);


            auto revText = CCLabelBMFont::create(reviewText.c_str(), "chatFont.fnt");

            revText->setPosition(4, 10);
            revText->setScale(.65);
            revText->setAnchorPoint(ccp(0, 0.5));

            this->addChild(revText);

            auto deleteSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
            deleteSpr->setScale(0.5);

            auto deleteBtn = CCMenuItemSpriteExtra::create(
                deleteSpr,
                this,
                menu_selector(ReviewCell::onDelete)
            );

            deleteBtn->setPosition(285, 25.5);

            if (GAM->m_username == score->m_userName ||
                GAM->m_username == user) {
                menu->addChild(deleteBtn);
            }

            

            return true;
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

        static ReviewCell* create(std::string username, std::string reviewText, int reviewID, GJUserScore* score) {
            ReviewCell* pRet = new ReviewCell();
            if (pRet && pRet->init(username, reviewText, reviewID, score)) {
                pRet->autorelease();
                return pRet;
            } else {
                delete pRet;
                return nullptr;
            }
        }

};