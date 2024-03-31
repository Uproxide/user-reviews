#pragma once

using namespace geode::prelude;

#include <Geode/ui/TextInput.hpp>

class UploadReview : public Popup<> {
protected:
	bool setup() override;

    TextInput* inp;

    void onReview(CCObject* sender);
    void reloadReviews();

public:
	static UploadReview* create();
};