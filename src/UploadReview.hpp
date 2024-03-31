#pragma once

using namespace geode::prelude;

#include <Geode/ui/TextInput.hpp>

class UploadReview : public Popup<> {
protected:
	bool setup() override;

    TextInput* inp;

public:
	static UploadReview* create();
    void onReview(CCObject* sender);
    void doSomething();
};