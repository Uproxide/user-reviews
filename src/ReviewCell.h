#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include <cctype>
#include <algorithm>
#include "ProfileReview.hpp"
#include "UploadReview.hpp"
#include "IconGetter.hpp"

using namespace geode::prelude;

class ReviewCell : public CCLayerColor {
    protected:

    GameLevelManager* m_gmgr;
    IconGetter* m_icongetter;

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

            this->setOpacity(50);

            this->setContentSize(ccp(300, 32));

            this->setAnchorPoint(ccp(0, 0));

            // auto background = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
	        // background->setContentSize(this->getContentSize());
	        // background->setOpacity(100);
	        // background->setPosition({(this->getContentSize().width / 2), (this->getContentSize().height / 2) - 3});
	        // this->addChild(background);

            auto menu = CCMenu::create();
            menu->setPosition(0,-10);

            auto playerBundle = CCMenu::create();

            playerBundle->setLayout(
                RowLayout::create()
                ->setGap(18)
                 ->setGrowCrossAxis(true)
                 ->setCrossAxisReverse(true)
                 ->setAutoScale(false)
                 ->setAxisAlignment(AxisAlignment::Start)
                );

            auto userTxt = CCLabelBMFont::create(username.c_str(), "goldFont.fnt");
            
            userTxt->setScale(.5);
            userTxt->setID("player-name");

            auto playerIcon = SimplePlayer::create(0);
            
            playerIcon->setScale(.475);
            playerIcon->setID("player-icon");
            
            

            CCMenuItemSpriteExtra* usernameButton;

            if(accid == 0) {
                usernameButton = CCMenuItemSpriteExtra::create(
                    userTxt,
                    this,
                    menu_selector(ReviewCell::onUserNoAccID)
                );
            } else if(accid != 0) {
                usernameButton = CCMenuItemSpriteExtra::create(
                    userTxt,
                    this,
                    menu_selector(ReviewCell::onUser)
                );
            }

            if (Mod::get()->getSettingValue<bool>("hide-icon") == false) {
                playerBundle->addChild(playerIcon);
            }
            playerBundle->addChild(usernameButton);

            playerBundle->setPosition(3, 35);
            playerBundle->setAnchorPoint(ccp(0, 0.5));
            playerBundle->setContentWidth(150);

            // usernameButton->setAnchorPoint(ccp(0, 0.5));

            /*
                usernameButton->setPosition(3, 35);
                usernameButton->setContentWidth(150);
            */
            
            userTxt->setAnchorPoint(ccp(0, 0.5));
            playerBundle->updateLayout();
            static_cast<CCSprite*>(playerIcon->getChildren()->objectAtIndex(0))->setAnchorPoint(ccp(0, 0.6));

            menu->addChild(playerBundle);
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
            
            auto gamgr = GameManager::sharedState();
            playerIcon->updatePlayerFrame(1, IconType::Cube);
            playerIcon->setColor(gamgr->colorForIdx(1));
            playerIcon->setSecondColor(gamgr->colorForIdx(3));
            playerIcon->disableGlowOutline();

            if (accid != 0 && Mod::get()->getSettingValue<bool>("hide-icon") == false) {
                IconGetter::shared()->setStuff(accid, playerBundle);
            }

            std::string lowerUsername = GAM->m_username;
            std::transform(lowerUsername.begin(), lowerUsername.end(), lowerUsername.begin(),
                        [](unsigned char c) { return std::tolower(c); });

            std::string lowerScoreName = score->m_userName;
            std::transform(lowerScoreName.begin(), lowerScoreName.end(), lowerScoreName.begin(),
                        [](unsigned char c) { return std::tolower(c); });

            std::string lowerUser = user;
            std::transform(lowerUser.begin(), lowerUser.end(), lowerUser.begin(),
               [](unsigned char c) { return std::tolower(c); });

            if (lowerUsername == lowerScoreName || lowerUsername == lowerUser) {
                menu->addChild(deleteBtn);
            }

            


            return true;
        }

        void onUserNoAccID(CCObject* sender) {
            GJSearchObject* obj = GJSearchObject::create(SearchType::Users, user);
            auto scene = CCTransitionFade::create(0.5f, LevelBrowserLayer::scene(obj));
            CCDirector::sharedDirector()->replaceScene(scene);
        }
        void onUser(CCObject* sender) {
            ProfilePage::create(accountId, GJAccountManager::sharedState()->m_accountID == accountId)->show();
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