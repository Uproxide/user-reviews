#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include "ProfileReview.hpp"

using namespace geode::prelude;

class ReviewCell : public CCLayerColor
{
    public:

        std::string user;

        bool init(std::string username, std::string reviewText)
        {
            if (!CCLayerColor::init())
                return false;

            user = username;


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

           

            return true;
        }

        void onUser(CCObject* sender) {
            GJSearchObject* obj = GJSearchObject::create(SearchType::Users, user);
            auto scene = CCTransitionFade::create(0.5f, LevelBrowserLayer::scene(obj));
            CCDirector::sharedDirector()->replaceScene(scene);
        }

        static ReviewCell* create(std::string username, std::string reviewText)
        {
            ReviewCell* pRet = new ReviewCell();
            if (pRet && pRet->init(username, reviewText)) {
                pRet->autorelease();
                return pRet;
            } else {
                delete pRet;
                return nullptr;
            }
        }
};