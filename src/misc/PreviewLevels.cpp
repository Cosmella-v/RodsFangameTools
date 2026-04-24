#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

using namespace geode::prelude;

// (modified) Code from Eclipse - Read LICENSE.EPL

class $modify(LevelEditPLHook, PauseLayer) {
    void customSetup() override {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto layer = scene->getChildByType<PlayLayer>(0);
        
        if (layer) {
            auto level = layer->m_level;
            auto levelType = level->m_levelType;
            level->m_levelType = GJLevelType::Editor;
            PauseLayer::customSetup();
            level->m_levelType = levelType;
        } else {
            PauseLayer::customSetup();
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

#if GEODE_COMP_GD_VERSION > 22070
class $modify(LevelEditEUIHook, EditorUI) {
    void onSettings(CCObject* sender) {
        auto level = this->m_editorLayer->m_level;
        auto levelType = level->m_levelType;
        level->m_levelType = GJLevelType::Editor;
        EditorUI::onSettings(sender);
        level->m_levelType = levelType;
    }
};
class $modify(LevelEditEPLHook, EditorPauseLayer) {
    void customSetup() override {
        auto level = this->m_editorLayer->m_level;
        auto levelType = level->m_levelType;
        level->m_levelType = GJLevelType::Editor;
        EditorPauseLayer::customSetup();
        level->m_levelType = levelType;
    }
};
#endif
