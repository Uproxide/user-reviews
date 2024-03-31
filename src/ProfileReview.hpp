#pragma once

using namespace geode::prelude;

class ProfileReview : public Popup<> {
protected:
	bool setup() override;

    bool empty = false;

    CCMenuItemSpriteExtra* reviewButton;
    CCLabelBMFont* noReviews;
    matjson::Value profileJson;
    CCScale9Sprite* background;
    LoadingCircle* loadingCircle = LoadingCircle::create();
    ScrollLayer* scroll = nullptr;

    void parseJson(std::string str);
    void onReview(CCObject* sender);
    void onGetReviewsFinished();
    

    virtual void keyBackClicked();

public:
	static ProfileReview* create();
    void getReviews();
    void onClose(CCObject*);
};