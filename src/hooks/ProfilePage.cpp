#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/ProfilePage.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/Notification.hpp>
#include "../utils/Manager.hpp"
#include "../ui/ProfileReview.hpp"

class $modify(PP, ProfilePage) {
    void onReviews(CCObject*sender) {
        score = this->m_score;
		ProfileReview::create()->show();
    }

    void loadPageFromUserInfo(GJUserScore* p0) {
        ProfilePage::loadPageFromUserInfo(p0);
	    auto image = CCSprite::createWithSpriteFrameName("URStarIcon.png"_spr);
        auto sprite = CircleButtonSprite::create(image, CircleBaseColor::Blue);
        sprite->setScale(0.7);
        image->setPositionY(25.5);
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