#pragma once

using namespace geode::prelude;

class ReviewCell : public CCLayerColor {
protected:
	bool init(std::string username, std::string reviewText);
    CCSprite* spr = CCSprite::createWithSpriteFrameName("square02_small.png");
    std::string userName; 
    std::string reviewTextYea;
public:
	static ReviewCell* create(std::string username, std::string reviewText);
};