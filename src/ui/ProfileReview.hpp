#pragma once

#include <Geode/utils/web.hpp>
using namespace geode::prelude;

class ProfileReview : public Popup<> {
protected:
	bool setup() override;

    bool empty = false;
    bool firstLoad = true;

    CCMenuItemSpriteExtra* reviewButton;
    CCLabelBMFont* noReviews;
    matjson::Value profileJson;
    CCScale9Sprite* background;
    LoadingCircle* loadingCircle = LoadingCircle::create();
    ScrollLayer* scroll = nullptr;
    CCLayer* layer2;
    TextInput* inp;
    CCMenu* menu;
    EventListener<web::WebTask> m_getReviews;
    EventListener<web::WebTask> m_uploadReview;

    void parseJson(std::string str);
    void onReview(CCObject* sender);
    void onGetReviewsFinished();
    

    virtual void keyBackClicked();

public:
	static ProfileReview* create();
    void getReviews();
    void onClose(CCObject*);
    void loadCells();
    void reloadReviews();
    void doSomething();
};