#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/TextInput.hpp>
#include <matjson.hpp>
#include "ProfileReview.hpp"

using namespace geode::prelude;


class IconGetter : UserInfoDelegate
protected:

    GameLevelManager* m_gmgr;
    int m_accountID;
    UserInfoDelegate m_oldUID;

public:

    IconGetter(int const type)
      : m_accountID(type) {m_gmgr = GameLevelManager::sharedState();}

    // Functions for UserInfoDelegate!
    void getUserInfoFinished(GJUserScore* score) {
        //! @todo do the stuff
    }

    void getUserInfoFailed(int) {
        //! @todo make failed function
    }

    void userInfoChanged(GJUserScore* score) {
        // do nothing
    }

        



};