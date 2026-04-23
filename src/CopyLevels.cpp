#include "Geode/utils/general.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelPage.hpp>

using namespace geode::prelude;

class $modify(MyEditLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level)) return false;

        bool copyLevelString = Mod::get()->getSettingValue<bool>("copy-level-string");

        if (copyLevelString) {
            auto menu = this->getChildByID("level-actions-menu");

            CCSprite* buttonSprite = CircleButtonSprite::create(
                CCLabelBMFont::create("Copy\nData", "bigFont.fnt")
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
            Notification* n = Notification::create("Encrypted level copied successfully!", NotificationIcon::Success);
            n->show();
        } else if (levelCopyType == "Decoded") {
            std::string decrypted = ZipUtils::decompressString(levelString, false, 1);
            clipboard::write(decrypted);
            Notification* n = Notification::create("Decrypted level copied successfully!", NotificationIcon::Success);
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

        CCSprite* buttonSprite = CircleButtonSprite::create(CCLabelBMFont::create("Copy\nLevel", "bigFont.fnt"));
        CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(MyLevelPage::onCopyLevel)
        );

        CCMenu* menu = CCMenu::createWithItem(button);
        this->addChild(menu);

        return true;
    }

    void onCopyLevel(CCObject* sender) {
        GameLevelManager* glm = GameLevelManager::sharedState();
        GameManager* gm = GameManager::sharedState();
        LocalLevelManager* llm = LocalLevelManager::sharedState();
        CCDirector* ccd = CCDirector::sharedDirector();

        if (ccd->getIsTransitioning()) return;
        this->setKeypadEnabled(false);

        log::debug("Level string for \"{}\": {}", m_level->m_levelName, m_level->m_levelString);
        
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
