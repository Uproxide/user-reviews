#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/ProfilePage.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/Notification.hpp>
#include "ProfileReview.hpp"
#include "ReviewCell.h"
#include "IconGetter.hpp"
#include "../utils/Manager.hpp"
#include "../utils/HttpStatusCodes.h"

IconGetter* IconGetter::s_shared = nullptr;

IconGetter* IconGetter::shared() {
    if (s_shared == nullptr) {
        s_shared = new (std::nothrow) IconGetter();
        s_shared->m_allinfo = matjson::parse("{\"info\": {}}")["info"];
    }
    GameLevelManager::sharedState()->m_userInfoDelegate = s_shared;

    return s_shared;
}


bool ProfileReview::setup() {
    m_noElasticity = true;
	auto winSize = CCDirector::sharedDirector()->getWinSize();

    popup = this;

    this->loadingCircle->setParentLayer(this->m_mainLayer);
	this->loadingCircle->setPosition({ -50,-40 });
	this->loadingCircle->setScale(1.f);
	this->loadingCircle->show();

    std::string userName = score->m_userName;

    std::string title = fmt::format("{}'s Reviews", userName);

	this->setTitle(title);

    this->m_title->setScale(.95);

    getReviews();

    this->retain();

    return true;
}

void ProfileReview::parseJson(std::string str) {
    if (str == "[]") {
        empty = true;
    } else {
        try {
            profileJson = matjson::parse(str);
        } catch (const std::exception& e) {
            log::error("Failed to parse JSON: {}", e.what());
        }
    }

    onGetReviewsFinished();
}

void ProfileReview::getReviews() {
    std::string url = fmt::format("https://uproxide.xyz/api/v1/reviews/getReviews.php?id={}", score->m_accountID);
    this->m_getReviews.bind([this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->ok()) {
                log::info("Review Get Success");
                Notification::create("Successfully deleted", NotificationIcon::Success)->show();
                this->parseJson(res->string().unwrapOr("Error"));
            } else {
                log::info("Reviews Get Failed - Status Code: {} - {}", res->code(), HttpStatus::reasonPhrase(res->code()));
                Notification::create(fmt::format("Status Code: {} - {}", res->code(), HttpStatus::reasonPhrase(res->code())), NotificationIcon::Error);
            }
        } else if (web::WebProgress* p = e->getProgress()) {
            log::info("get reviews progress: {}", p->downloadProgress().value_or(0.f));
        } else if (e->isCancelled()) {
            log::info("The request was cancelled... So sad :(");
        }
    });
    web::WebRequest req = web::WebRequest();
    this->m_getReviews.setFilter(req.get(url));
}

void ProfileReview::onGetReviewsFinished() {
	if (this) {
	    this->loadingCircle->fadeAndRemove();
	
	    auto winSize = CCDirector::sharedDirector()->getWinSize();

        int revCount = 0;
	
	    scroll = ScrollLayer::create(ccp(300, 150));
	    scroll->setAnchorPoint(ccp(0, 0));
	    scroll->ignoreAnchorPointForPosition(false);
	
	    auto noReviews = CCLabelBMFont::create("No Reviews for this User.\nWant to write one?", "bigFont.fnt");
	    noReviews->setPosition(winSize / 2);
	    noReviews->setScale(0.5);

        

        menu = CCMenu::create();
        

        menu->setPosition({this->m_title->getPositionX(),25});
        this->m_mainLayer->addChild(menu);

        inp = TextInput::create(260, fmt::format("Write a Review for {}", score->m_userName), "chatFont.fnt");
        menu->addChild(inp);

        inp->setCommonFilter(CommonFilter::Any);
        inp->setMaxCharCount(45);
        
        CCSprite* reviewSpr = CCSprite::createWithSpriteFrameName("GJ_chatBtn_001.png");
        reviewSpr->setScale(0.75);

        reviewButton = CCMenuItemSpriteExtra::create(
            reviewSpr,
            this,
            menu_selector(ProfileReview::onReview)
        );

        menu->addChild(reviewButton);
        menu->setLayout(RowLayout::create());
        menu->setContentWidth(342);
        menu->updateLayout();
        menu->setTouchPriority(-510);
	
	    if (empty) {
	        this->addChild(noReviews);
	    } else {
	        int basePosY = 117;
	        background = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
	        background->setContentSize(scroll->getContentSize());
	        background->setOpacity(75);
	        background->setPosition(winSize / 2);
	        this->addChild(background);
	        background->addChild(scroll);
	
	        scroll->m_contentLayer->removeAllChildren();
	
	        if (profileJson.is_object()) {
	            for (const auto& pair : profileJson.as_object()) {
	                const auto& reviewObject = pair.second;
                    std::string userName;
                    int accountID;

                    if (reviewObject.contains("userName") && !reviewObject.contains("uAccountID")) {
	                    userName = reviewObject["userName"].as_string();
	                    accountID = 12;
                    } else if (!reviewObject.contains("userName") && reviewObject.contains("uAccountID")) {
	                    userName = "User";
	                    accountID = reviewObject["uAccountID"].as_int();
                    } else if (reviewObject.contains("userName") && reviewObject.contains("uAccountID")) {
	                    userName = reviewObject["userName"].as_string();
	                    accountID = reviewObject["uAccountID"].as_int();
                    }

	                std::string reviewText = reviewObject["reviewText"].as_string();
	                int reviewID = reviewObject["reviewID"].as_int();
	                auto cell = ReviewCell::create(userName, accountID, reviewText, reviewID, score);
	                cell->setPositionY(basePosY);
	                scroll->m_contentLayer->addChild(cell);
	                scroll->m_contentLayer->setAnchorPoint(ccp(0,1));
	
	                float height = std::max<float>(scroll->getContentSize().height, 35 * scroll->m_contentLayer->getChildrenCount());
	
	                scroll->m_contentLayer->setContentSize(ccp(scroll->m_contentLayer->getContentSize().width, height));
	
	                CCArrayExt<ReviewCell*> objects = scroll->m_contentLayer->getChildren();

                int i = 0;
	
	
				for (auto* obj : objects) {
	                i++;
					obj->setPositionY(height - (35 * i));
	
				}

                revCount = i;

	
	            scroll->moveToTop();
	        }

            std::string reviewAmnt;

            if (revCount == 1) {
                reviewAmnt = fmt::format("{} Review", revCount);
            } else {
                reviewAmnt = fmt::format("{} Reviews", revCount);
            }

            auto reviewCount = CCLabelBMFont::create(reviewAmnt.c_str(), "chatFont.fnt");
            reviewCount->setScale(0.5);
            reviewCount->setPosition(m_title->getPosition());
            reviewCount->setPositionY(reviewCount->getPositionY() - 18);

            this->m_mainLayer->addChild(reviewCount);
	    }
	
	    this->setTouchEnabled(true);
	    CCTouchDispatcher::get()->addTargetedDelegate(this, -129, true);
	    CCTouchDispatcher::get()->addTargetedDelegate(scroll, -130, true);
	    }
	}
}



void ProfileReview::keyBackClicked() 
{
    onClose(nullptr);
}

void ProfileReview::onClose(CCObject*)
{
    IconGetter::shared()->clearVector();
    this->release();
    this->removeFromParent();
}

void ProfileReview::onReview(CCObject* sender) {
    auto GAM = GJAccountManager::sharedState();

    if (inp->getString() == "")
    {
        Notification::create("You cannot have an empty review.", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
    } else {
        reviewButton->setVisible(false);
        std::string reviewText = inp->getString();
        
        for (size_t i = 0; i < reviewText.length(); ++i) {
            if (reviewText[i] == ' ') {
                reviewText.replace(i, 1, "%20");
            }
        }

        std::string url = fmt::format("https://uproxide.xyz/api/v1/reviews/uploadReview.php?userName={}&reviewText={}&accountID={}&uid={}", GAM->m_username, reviewText, score->m_accountID, GAM->m_accountID);

        this->m_uploadReview.bind([this] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (res->ok()) {
                    Notification::create("Successfully deleted", NotificationIcon::Success)->show();
                    matjson::Value json = res->json().unwrapOr("error");
                    if (json.contains("error")) {
                        std::string errorResponse = json["error"].as_string();
                        Notification::create(fmt::format("Review Upload Failed! {}", errorResponse), NotificationIcon::Error)->show();
                        reviewButton->setVisible(true);
                    } else {
                        doSomething();
                    }
                } else {
                    Notification::create(fmt::format("Status Code: {} - {}", res->code(), HttpStatus::reasonPhrase(res->code())), NotificationIcon::Error);
                }
            } else if (web::WebProgress* p = e->getProgress()) {
                log::info("upload review progress: {}", p->downloadProgress().value_or(0.f));
            } else if (e->isCancelled()) {
                log::info("The request was cancelled... So sad :(");
            }
        });
        web::WebRequest req = web::WebRequest();
        this->m_uploadReview.setFilter(req.get(url));
    }
}

void ProfileReview::doSomething() {
    Notification::create("Successfully Reviewed this User.", CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();

    popup->onClose(nullptr);
    popup = ProfileReview::create();
    popup->show();
}

ProfileReview* ProfileReview::create() {
	auto ret = new ProfileReview();
	if (ret && ret->initAnchored(342, 240, "GJ_square01.png")) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}