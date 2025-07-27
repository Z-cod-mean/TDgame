#ifndef _RESOURCES_MANAGER_H_
#define _RESOURCES_MANAGER_H_

#include "manager.h" // 管理器基类
#include <SDL_ttf.h> // 字体处理
#include <SDL_image.h> // 图像加载
#include <SDL_mixer.h> // 音频处理
#include <unordered_map> // 哈希映射容器

/**
 * @enum ResID
 * @brief 资源标识符枚举，用于唯一标识游戏中的各种资源
 *
 * 该枚举定义了游戏中所有资源的唯一ID，包括纹理、音效、音乐和字体。
 * 使用枚举可以避免在代码中硬编码字符串路径，提高可维护性。
 */
enum class ResID
{
    // ======== 纹理资源 ========
    Tex_Tileset, // 地图瓦片集

    // 玩家和防御塔纹理
    Tex_Player, // 玩家角色
    Tex_Archer, // 弓箭手塔
    Tex_Axeman, // 斧兵塔
    Tex_Gunner, // 枪手塔

    // 敌人纹理
    Tex_Slime,          // 史莱姆
    Tex_KingSlime,      // 史莱姆王
    Tex_Skeleton,       // 骷髅
    Tex_Goblin,         // 哥布林
    Tex_GoblinPriest,   // 哥布林祭司
    Tex_SlimeSketch,    // 史莱姆草图
    Tex_KingSlimeSketch,// 史莱姆王草图
    Tex_SkeletonSketch, // 骷髅草图
    Tex_GoblinSketch,   // 哥布林草图
    Tex_GoblinPriestSketch, // 哥布林祭司草图

    // 子弹纹理
    Tex_BulletArrow, // 箭矢子弹
    Tex_BulletAxe,   // 斧头子弹
    Tex_BulletShell, // 炮弹子弹

    // 游戏物品纹理
    Tex_Coin, // 金币
    Tex_Home, // 基地

    // 特效纹理
    Tex_EffectFlash_Up,    // 朝上闪光特效
    Tex_EffectFlash_Down,  // 朝下闪光特效
    Tex_EffectFlash_Left,  // 朝左闪光特效
    Tex_EffectFlash_Right, // 朝右闪光特效
    Tex_EffectImpact_Up,   // 朝上冲击特效
    Tex_EffectImpact_Down, // 朝下冲击特效
    Tex_EffectImpact_Left, // 朝左冲击特效
    Tex_EffectImpact_Right,// 朝右冲击特效
    Tex_EffectExplode,     // 爆炸特效

    // UI界面纹理
    Tex_UISelectCursor,      // 选择光标
    Tex_UIPlaceIdle,         // 放置塔空闲状态
    Tex_UIPlaceHoveredTop,   // 放置塔顶部悬停
    Tex_UIPlaceHoveredLeft,  // 放置塔左侧悬停
    Tex_UIPlaceHoveredRight, // 放置塔右侧悬停
    Tex_UIUpgradeIdle,       // 升级空闲状态
    Tex_UIUpgradeHoveredTop, // 升级顶部悬停
    Tex_UIUpgradeHoveredLeft,// 升级左侧悬停
    Tex_UIUpgradeHoveredRight,// 升级右侧悬停
    Tex_UIHomeAvatar,        // 基地头像
    Tex_UIPlayerAvatar,      // 玩家头像
    Tex_UIHeart,             // 心形图标
    Tex_UICoin,              // 金币图标
    Tex_UIGameOverBar,       // 游戏结束条
    Tex_UIWinText,           // 胜利文本
    Tex_UILossText,          // 失败文本

    // ======== 音效资源 ========
    Sound_ArrowFire_1, // 弓箭射击音效1
    Sound_ArrowFire_2, // 弓箭射击音效2
    Sound_AxeFire,     // 斧头射击音效
    Sound_ShellFire,   // 炮弹射击音效
    Sound_ArrowHit_1,  // 箭矢命中音效1
    Sound_ArrowHit_2,  // 箭矢命中音效2
    Sound_ArrowHit_3,  // 箭矢命中音效3
    Sound_AxeHit_1,    // 斧头命中音效1
    Sound_AxeHit_2,    // 斧头命中音效2
    Sound_AxeHit_3,    // 斧头命中音效3
    Sound_ShellHit,    // 炮弹命中音效

    Sound_Flash,       // 闪光音效
    Sound_Impact,      // 冲击音效

    Sound_Coin,        // 金币收集音效
    Sound_HomeHurt,    // 基地受伤音效
    Sound_PlaceTower,  // 放置塔音效
    Sound_TowerLevelUp,// 塔升级音效

    Sound_Win,         // 胜利音效
    Sound_Loss,        // 失败音效

    // ======== 音乐资源 ========
    Music_BGM, // 背景音乐

    // ======== 字体资源 ========
    Font_Main // 主字体
};

/**
 * @class ResourcesManager
 * @brief 资源管理器类，负责加载和管理游戏中的所有资源
 *
 * 该类使用单例模式实现，确保全局只有一个资源管理器实例。
 * 它管理四种类型的资源池：纹理、字体、音效和音乐。
 * 通过枚举ResID提供对资源的统一访问接口。
 */
class ResourcesManager : public Manager<ResourcesManager>
{
    friend class Manager<ResourcesManager>; // 允许管理器基类访问私有构造函数

public:
    // 资源池类型定义
    typedef std::unordered_map<ResID, TTF_Font*> FontPool;    // 字体池
    typedef std::unordered_map<ResID, Mix_Chunk*> SoundPool;  // 音效池
    typedef std::unordered_map<ResID, Mix_Music*> MusicPool;  // 音乐池
    typedef std::unordered_map<ResID, SDL_Texture*> TexturePool; // 纹理池

public:
    /**
     * @brief 从文件加载所有资源
     * @param renderer SDL渲染器指针，用于创建纹理
     * @return 加载成功返回true，任一资源加载失败返回false
     *
     * 此方法按顺序加载所有资源，如果任一资源加载失败，则立即返回false。
     * 加载过程分为四个部分：纹理、音效、音乐和字体。
     */
    bool load_from_file(SDL_Renderer* renderer)
    {
        // ======== 加载纹理资源 ========
        texture_pool[ResID::Tex_Tileset] = IMG_LoadTexture(renderer, "resources/tileset.png");

        // 玩家和防御塔纹理
        texture_pool[ResID::Tex_Player] = IMG_LoadTexture(renderer, "resources/player.png");
        texture_pool[ResID::Tex_Archer] = IMG_LoadTexture(renderer, "resources/tower_archer.png");
        texture_pool[ResID::Tex_Axeman] = IMG_LoadTexture(renderer, "resources/tower_axeman.png");
        texture_pool[ResID::Tex_Gunner] = IMG_LoadTexture(renderer, "resources/tower_gunner.png");

        // 敌人纹理
        texture_pool[ResID::Tex_Slime] = IMG_LoadTexture(renderer, "resources/enemy_slime.png");
        texture_pool[ResID::Tex_KingSlime] = IMG_LoadTexture(renderer, "resources/enemy_king_slime.png");
        texture_pool[ResID::Tex_Skeleton] = IMG_LoadTexture(renderer, "resources/enemy_skeleton.png");
        texture_pool[ResID::Tex_Goblin] = IMG_LoadTexture(renderer, "resources/enemy_goblin.png");
        texture_pool[ResID::Tex_GoblinPriest] = IMG_LoadTexture(renderer, "resources/enemy_goblin_priest.png");
        texture_pool[ResID::Tex_SlimeSketch] = IMG_LoadTexture(renderer, "resources/enemy_slime_sketch.png");
        texture_pool[ResID::Tex_KingSlimeSketch] = IMG_LoadTexture(renderer, "resources/enemy_king_slime_sketch.png");
        texture_pool[ResID::Tex_SkeletonSketch] = IMG_LoadTexture(renderer, "resources/enemy_skeleton_sketch.png");
        texture_pool[ResID::Tex_GoblinSketch] = IMG_LoadTexture(renderer, "resources/enemy_goblin_sketch.png");
        texture_pool[ResID::Tex_GoblinPriestSketch] = IMG_LoadTexture(renderer, "resources/enemy_goblin_priest_sketch.png");

        // 子弹纹理
        texture_pool[ResID::Tex_BulletArrow] = IMG_LoadTexture(renderer, "resources/bullet_arrow.png");
        texture_pool[ResID::Tex_BulletAxe] = IMG_LoadTexture(renderer, "resources/bullet_axe.png");
        texture_pool[ResID::Tex_BulletShell] = IMG_LoadTexture(renderer, "resources/bullet_shell.png");

        // 游戏物品纹理
        texture_pool[ResID::Tex_Coin] = IMG_LoadTexture(renderer, "resources/coin.png");
        texture_pool[ResID::Tex_Home] = IMG_LoadTexture(renderer, "resources/home.png");

        // 特效纹理
        texture_pool[ResID::Tex_EffectFlash_Up] = IMG_LoadTexture(renderer, "resources/effect_flash_up.png");
        texture_pool[ResID::Tex_EffectFlash_Down] = IMG_LoadTexture(renderer, "resources/effect_flash_down.png");
        texture_pool[ResID::Tex_EffectFlash_Left] = IMG_LoadTexture(renderer, "resources/effect_flash_left.png");
        texture_pool[ResID::Tex_EffectFlash_Right] = IMG_LoadTexture(renderer, "resources/effect_flash_right.png");
        texture_pool[ResID::Tex_EffectImpact_Up] = IMG_LoadTexture(renderer, "resources/effect_impact_up.png");
        texture_pool[ResID::Tex_EffectImpact_Down] = IMG_LoadTexture(renderer, "resources/effect_impact_down.png");
        texture_pool[ResID::Tex_EffectImpact_Left] = IMG_LoadTexture(renderer, "resources/effect_impact_left.png");
        texture_pool[ResID::Tex_EffectImpact_Right] = IMG_LoadTexture(renderer, "resources/effect_impact_right.png");
        texture_pool[ResID::Tex_EffectExplode] = IMG_LoadTexture(renderer, "resources/effect_explode.png");

        // UI界面纹理
        texture_pool[ResID::Tex_UISelectCursor] = IMG_LoadTexture(renderer, "resources/ui_select_cursor.png");
        texture_pool[ResID::Tex_UIPlaceIdle] = IMG_LoadTexture(renderer, "resources/ui_place_idle.png");
        texture_pool[ResID::Tex_UIPlaceHoveredTop] = IMG_LoadTexture(renderer, "resources/ui_place_hovered_top.png");
        texture_pool[ResID::Tex_UIPlaceHoveredLeft] = IMG_LoadTexture(renderer, "resources/ui_place_hovered_left.png");
        texture_pool[ResID::Tex_UIPlaceHoveredRight] = IMG_LoadTexture(renderer, "resources/ui_place_hovered_right.png");
        texture_pool[ResID::Tex_UIUpgradeIdle] = IMG_LoadTexture(renderer, "resources/ui_upgrade_idle.png");
        texture_pool[ResID::Tex_UIUpgradeHoveredTop] = IMG_LoadTexture(renderer, "resources/ui_upgrade_hovered_top.png");
        texture_pool[ResID::Tex_UIUpgradeHoveredLeft] = IMG_LoadTexture(renderer, "resources/ui_upgrade_hovered_left.png");
        texture_pool[ResID::Tex_UIUpgradeHoveredRight] = IMG_LoadTexture(renderer, "resources/ui_upgrade_hovered_right.png");
        texture_pool[ResID::Tex_UIHomeAvatar] = IMG_LoadTexture(renderer, "resources/ui_home_avatar.png");
        texture_pool[ResID::Tex_UIPlayerAvatar] = IMG_LoadTexture(renderer, "resources/ui_player_avatar.png");
        texture_pool[ResID::Tex_UIHeart] = IMG_LoadTexture(renderer, "resources/ui_heart.png");
        texture_pool[ResID::Tex_UICoin] = IMG_LoadTexture(renderer, "resources/ui_coin.png");
        texture_pool[ResID::Tex_UIGameOverBar] = IMG_LoadTexture(renderer, "resources/ui_game_over_bar.png");
        texture_pool[ResID::Tex_UIWinText] = IMG_LoadTexture(renderer, "resources/ui_win_text.png");
        texture_pool[ResID::Tex_UILossText] = IMG_LoadTexture(renderer, "resources/ui_loss_text.png");

        // 检查所有纹理是否加载成功
        for (const auto& pair : texture_pool)
            if (!pair.second) return false;

        // ======== 加载音效资源 ========
        sound_pool[ResID::Sound_ArrowFire_1] = Mix_LoadWAV("resources/sound_arrow_fire_1.mp3");
        sound_pool[ResID::Sound_ArrowFire_2] = Mix_LoadWAV("resources/sound_arrow_fire_2.mp3");
        sound_pool[ResID::Sound_AxeFire] = Mix_LoadWAV("resources/sound_axe_fire.wav");
        sound_pool[ResID::Sound_ShellFire] = Mix_LoadWAV("resources/sound_shell_fire.wav");
        sound_pool[ResID::Sound_ArrowHit_1] = Mix_LoadWAV("resources/sound_arrow_hit_1.mp3");
        sound_pool[ResID::Sound_ArrowHit_2] = Mix_LoadWAV("resources/sound_arrow_hit_2.mp3");
        sound_pool[ResID::Sound_ArrowHit_3] = Mix_LoadWAV("resources/sound_arrow_hit_3.mp3");
        sound_pool[ResID::Sound_AxeHit_1] = Mix_LoadWAV("resources/sound_axe_hit_1.mp3");
        sound_pool[ResID::Sound_AxeHit_2] = Mix_LoadWAV("resources/sound_axe_hit_2.mp3");
        sound_pool[ResID::Sound_AxeHit_3] = Mix_LoadWAV("resources/sound_axe_hit_3.mp3");
        sound_pool[ResID::Sound_ShellHit] = Mix_LoadWAV("resources/sound_shell_hit.mp3");

        sound_pool[ResID::Sound_Flash] = Mix_LoadWAV("resources/sound_flash.wav");
        sound_pool[ResID::Sound_Impact] = Mix_LoadWAV("resources/sound_impact.wav");

        sound_pool[ResID::Sound_Coin] = Mix_LoadWAV("resources/sound_coin.mp3");
        sound_pool[ResID::Sound_HomeHurt] = Mix_LoadWAV("resources/sound_home_hurt.wav");
        sound_pool[ResID::Sound_PlaceTower] = Mix_LoadWAV("resources/sound_place_tower.mp3");
        sound_pool[ResID::Sound_TowerLevelUp] = Mix_LoadWAV("resources/sound_tower_level_up.mp3");

        sound_pool[ResID::Sound_Win] = Mix_LoadWAV("resources/sound_win.wav");
        sound_pool[ResID::Sound_Loss] = Mix_LoadWAV("resources/sound_loss.mp3");

        // 检查所有音效是否加载成功
        for (const auto& pair : sound_pool)
            if (!pair.second) return false;

        // ======== 加载音乐资源 ========
        music_pool[ResID::Music_BGM] = Mix_LoadMUS("resources/music_bgm.mp3");

        // 检查所有音乐是否加载成功
        for (const auto& pair : music_pool)
            if (!pair.second) return false;

        // ======== 加载字体资源 ========
        font_pool[ResID::Font_Main] = TTF_OpenFont("resources/ipix.ttf", 25);

        // 检查所有字体是否加载成功
        for (const auto& pair : font_pool)
            if (!pair.second) return false;

        return true; // 所有资源加载成功
    }

    /**
     * @brief 获取字体资源池
     * @return 字体资源池的常量引用
     */
    const FontPool& get_font_pool()
    {
        return font_pool;
    }

    /**
     * @brief 获取音效资源池
     * @return 音效资源池的常量引用
     */
    const SoundPool& get_sound_pool()
    {
        return sound_pool;
    }

    /**
     * @brief 获取音乐资源池
     * @return 音乐资源池的常量引用
     */
    const MusicPool& get_music_pool()
    {
        return music_pool;
    }

    /**
     * @brief 获取纹理资源池
     * @return 纹理资源池的常量引用
     */
    const TexturePool& get_texture_pool()
    {
        return texture_pool;
    }

protected:
    ResourcesManager() = default; // 保护构造函数，确保单例
    ~ResourcesManager() = default; // 保护析构函数

private:
    FontPool font_pool;       // 字体资源池
    SoundPool sound_pool;     // 音效资源池
    MusicPool music_pool;     // 音乐资源池
    TexturePool texture_pool; // 纹理资源池
};

#endif // !_RESOURCES_MANAGER_H_
