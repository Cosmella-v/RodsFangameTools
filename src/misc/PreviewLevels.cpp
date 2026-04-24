#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

// (modified) Code from Eclipse - Read LICENSE.EPL

class $modify(LevelEditPLHook, PauseLayer) {
    void customSetup() override {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto layer = scene->getChildByType<PlayLayer>(0);
        PauseLayer::customSetup();
        if (layer) {
            auto level = layer->m_level;
            auto levelType = level->m_levelType;
            level->m_levelType = GJLevelType::Editor;
        }
    }

    void onTryEdit(CCObject* sender) {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto layer = scene->getChildByType<PlayLayer>(0);
        auto level = layer->m_level;
        auto levelType = level->m_levelType;
        level->m_levelType = GJLevelType::Editor;
        PauseLayer::onEdit(sender);
        level->m_levelType = levelType;
    }
};
