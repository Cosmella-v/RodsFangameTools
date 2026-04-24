#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(MyEditLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level)) return false;

        bool copyLevelString = Mod::get()->getSettingValue<bool>("copy-level-string");

        if (copyLevelString) {
            auto menu = this->getChildByID("level-actions-menu");

            CCSprite* buttonSprite = CircleButtonSprite::create(
                CCLabelBMFont::create("Copy\nData", "bigFont.fnt", 0.f, CCTextAlignment::kCCTextAlignmentCenter)
            );
            CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(MyEditLevelLayer::copyLevel)
            );

            menu->addChild(button);
            menu->updateLayout();
        }

        return true;
    }

    void copyLevel(CCObject* sender) {
        std::string levelCopyType = Mod::get()->getSettingValue<std::string>("level-copy-type");
        std::string levelString = m_level->m_levelString;

        if (levelCopyType == "Encoded") {
            clipboard::write(levelString);
            Notification* n = Notification::create("Encoded level copied successfully!", NotificationIcon::Success);
            n->show();
        } else if (levelCopyType == "Decoded") {
            std::string decrypted = ZipUtils::decompressString(levelString, false, 1);
            clipboard::write(decrypted);
            Notification* n = Notification::create("Decoded level copied successfully!", NotificationIcon::Success);
            n->show();
        } else {
            Notification* n = Notification::create("Unknown level type", NotificationIcon::Error);
            n->show();
        }
    }
};

class $modify(MyLevelPage, LevelPage) {
    bool init(GJGameLevel* level) {
        if (!LevelPage::init(level)) return false;

        bool copyMainLevels = Mod::get()->getSettingValue<bool>("copy-main-levels");

        if (copyMainLevels) {
            CCSprite* buttonSprite = CircleButtonSprite::create(CCLabelBMFont::create("Copy\nLevel", "bigFont.fnt", 0.f, CCTextAlignment::kCCTextAlignmentCenter));
            CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(MyLevelPage::onCopyLevel)
            );

            CCMenu* menu = CCMenu::createWithItem(button);
            this->addChild(menu);
        }

        return true;
    }

    void onCopyLevel(CCObject* sender) {
        GameLevelManager* glm = GameLevelManager::sharedState();
        GameManager* gm = GameManager::sharedState();
        LocalLevelManager* llm = LocalLevelManager::sharedState();
        CCDirector* ccd = CCDirector::sharedDirector();

        if (ccd->getIsTransitioning()) return;
        this->setKeypadEnabled(false);

        // log::debug("Level string for \"{}\": {}", m_level->m_levelName, m_level->m_levelString);
        
        gm->m_sceneEnum = 2;
        GJGameLevel* level = glm->createNewLevel();
        level->copyLevelInfo(m_level);
        level->m_levelType = GJLevelType::Editor;
        level->m_levelString = llm->getMainLevelString(m_level->m_levelID);
        level->m_originalLevel = m_level->m_originalLevel;

        CCScene* scene = EditLevelLayer::scene(level);
        CCTransitionFade* transition = CCTransitionFade::create(0.5f, scene);
        ccd->replaceScene(transition);
    }
};

class $modify(MyEditorPauseLayer, EditorPauseLayer) {
    struct Fields {
        FLAlertLayer* m_popup;
    };

    bool init(LevelEditorLayer* layer) {
        if (!EditorPauseLayer::init(layer)) return false;

        auto menu = this->getChildByID("guidelines-menu");

        CCSprite* buttonSprite = CircleButtonSprite::create(CCLabelBMFont::create("Paste\nLevel", "bigFont.fnt", 0.f, CCTextAlignment::kCCTextAlignmentCenter));
        CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(MyEditorPauseLayer::onPasteLevelString)
        );

        menu->addChild(button);
        menu->updateLayout();

        return true;
    }

    void onPasteLevelString(CCObject* sender) {
        /*std::string cb = clipboard::read();
        GameManager::sharedState()->m_editorClipboard = cb;
        m_editorLayer->createObjectsFromString(cb, true, true);
        LevelSettingsObject* obj = LevelSettingsObject::objectFromString(cb);
        log::debug("BG: {}, MG: {}, G: {}", obj->m_backgroundIndex, obj->m_middleGroundIndex, obj->m_groundIndex);
        m_editorLayer->m_levelSettings = obj;
        m_editorLayer->levelSettingsUpdated();*/

        // sry [redacted] ik this is indeed ass but my game keeps crashing with the code above
        std::string levelString = clipboard::read();
        m_editorLayer->m_level->m_levelString = levelString;
        FLAlertLayer* popup = FLAlertLayer::create(
            "Level Pasted",
            "<cr>PLEASE READ THE INFO BELOW:</c>\n"
            "The level was pasted <cg>successfully</c>!\n"
            "Please make sure you <cr>close</c> the editor by clicking "
            "the \"<cp>Exit</c>\" button (NOT \"<cy>Save and Exit</c>\") "
            "and then re-open the editor as usual.",
            "OK"
        );
        m_fields->m_popup = popup;
        popup->m_buttonMenu->setVisible(false);
        popup->show();
        CCArray* actions = CCArray::create();
        actions->addObject(CCDelayTime::create(2.f));
        actions->addObject(CCCallFunc::create(this, callfunc_selector(MyEditorPauseLayer::showPopupButton)));
        popup->runAction(CCSequence::create(actions));
    }

    void showPopupButton() {
        m_fields->m_popup->m_buttonMenu->setVisible(true);
    }
};
