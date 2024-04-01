#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/ProfilePage.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/Notification.hpp>
#include "ProfileReview.hpp"
#include "UploadReview.hpp"
#include "Test.h"

GJUserScore* score;
ProfileReview* popup;

bool UploadReview::setup() {
    m_noElasticity = true;
	auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->setTitle("Upload Review");

    inp = TextInput::create(180, "Review");

    inp->setPosition(winSize / 2);

    inp->setMaxCharCount(45);

    inp->setPositionY(inp->getPositionY() + 5);

    inp->setCommonFilter(CommonFilter::Any);

    this->addChild(inp);

    auto btn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Ok"),
        this,
        menu_selector(UploadReview::onReview)
    );

    auto menu = CCMenu::create();

    menu->setContentSize(btn->getContentSize());

    menu->addChild(btn);

    menu->setPosition(winSize / 2);

    menu->setPositionY(menu->getPositionY() - 35);

    menu->setLayout(RowLayout::create());


    this->addChild(menu);

    return true;
}


bool ProfileReview::setup() {
    m_noElasticity = true;
	auto winSize = CCDirector::sharedDirector()->getWinSize();

    popup = this;

    this->loadingCircle->setParentLayer(this->m_mainLayer);
	this->loadingCircle->setPosition({ -70,-40 });
	this->loadingCircle->setScale(1.f);
	this->loadingCircle->show();

    std::string userName = score->m_userName;

    std::string title = fmt::format("{}'s Reviews", userName);

	this->setTitle(title);

    this->m_title->setScale(.95);

    CCSprite* reviewSpr = CCSprite::createWithSpriteFrameName("GJ_chatBtn_001.png");
    reviewSpr->setScale(0.8);

    reviewButton = CCMenuItemSpriteExtra::create(
        reviewSpr,
        this,
        menu_selector(ProfileReview::onReview)
    );

    getReviews();

	CCMenu* menu = CCMenu::create();
	menu->setPosition({this->m_mainLayer->getContentWidth()-28,30});
	menu->addChild(reviewButton);
	this->m_mainLayer->addChild(menu);

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
    this->retain();
    std::string url = fmt::format("https://uproxide.xyz/api/v1/reviews/getReviews.php?id={}", score->m_accountID);
    
    web::AsyncWebRequest()
        .fetch(url)
        .text()
        .then([this](std::string const& json) {
            parseJson(json);
	    this->release();
        })
        .expect([this](std::string const& json) {
            log::error("something went wrong :3");
	    this->release();
        });
}

void ProfileReview::onGetReviewsFinished() {
    this->loadingCircle->fadeAndRemove();

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    scroll = ScrollLayer::create(ccp(300, 150));
    scroll->setAnchorPoint(ccp(0, 0));
    scroll->ignoreAnchorPointForPosition(false);

    auto noReviews = CCLabelBMFont::create("No Reviews for this User.\nWant to write one?", "bigFont.fnt");
    noReviews->setPosition(winSize / 2);
    noReviews->setScale(0.5);

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
                std::string userName = reviewObject["userName"].as_string();
                std::string reviewText = reviewObject["reviewText"].as_string();
                int reviewID = reviewObject["reviewID"].as_int();
                auto cell = ReviewCell::create(userName, reviewText, reviewID, score);
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

            scroll->moveToTop();
        }
    }

    this->setTouchEnabled(true);
    CCTouchDispatcher::get()->addTargetedDelegate(this, -129, true);
    CCTouchDispatcher::get()->addTargetedDelegate(scroll, -130, true);
    }   
}


void ProfileReview::onReview(CCObject* sender) {
    UploadReview::create()->show();
}

void ProfileReview::keyBackClicked() 
{
    onClose(nullptr);
}

void ProfileReview::onClose(CCObject*)
{
    this->removeFromParent();
}

void UploadReview::onReview(CCObject* sender) {
    auto GAM = GJAccountManager::sharedState();

    if (inp->getString() == "")
    {
        Notification::create("You cannot have an empty review.", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
    } else {
        std::string reviewText = inp->getString();
        
        for (size_t i = 0; i < reviewText.length(); ++i) {
            if (reviewText[i] == ' ') {
                reviewText.replace(i, 1, "%20");
            }
        }

        std::string url = fmt::format("https://uproxide.xyz/api/v1/reviews/uploadReview.php?userName={}&reviewText={}&accountID={}", GAM->m_username, reviewText, score->m_accountID);
    
        web::AsyncWebRequest()
            .fetch(url)
            .json()
            .then([this](auto const& json) {
                if (json.contains("bw"))
                {
                    Notification::create("Review contains banned words.", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
                } else if (json.contains("error")) {
                    Notification::create("Something went wrong.", CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"))->show();
                } else {
                    doSomething();
                }
                
            })
            .expect([this](auto const& json) {
                log::error("something went wrong :3");
            });
    }
}

void UploadReview::doSomething() {
    this->keyBackClicked();
    Notification::create("Successfully Reviewed this User.", CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png"))->show();

    popup->onClose(nullptr);
    popup = ProfileReview::create();
    popup->show();
}

ProfileReview* ProfileReview::create() {
	auto ret = new ProfileReview();
	if (ret && ret->initAnchored(420, 240, "GJ_square01.png")) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

UploadReview* UploadReview::create() {
	auto ret = new UploadReview();
	if (ret && ret->initAnchored(220, 120, "GJ_square01.png")) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

class $modify(PP, ProfilePage) {
    void onReviews(CCObject*sender) {
        score = this->m_score;
		ProfileReview::create()->show();
    }

    void loadPageFromUserInfo(GJUserScore* p0) {
        ProfilePage::loadPageFromUserInfo(p0);
	auto image = CCSprite::createWithSpriteFrameName("GJ_bigStar_noShadow_001.png");
        auto sprite = CircleButtonSprite::create(image, CircleBaseColor::Blue);
        sprite->setScale(0.8);
		auto button = CCMenuItemSpriteExtra::create(
			sprite, this, menu_selector(PP::onReviews)
		);
		button->setID("review-btn"_spr);

		if (auto menu = this->getChildByIDRecursive("left-menu")) {
			if (auto btnAlready = menu->getChildByID("review-btn"_spr)) {
				btnAlready->removeFromParent();
			}
			menu->addChild(button);
			menu->updateLayout();
		}
    }
};
