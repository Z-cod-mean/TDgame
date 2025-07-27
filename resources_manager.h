#ifndef _RESOURCES_MANAGER_H_
#define _RESOURCES_MANAGER_H_

#include "manager.h" // ����������
#include <SDL_ttf.h> // ���崦��
#include <SDL_image.h> // ͼ�����
#include <SDL_mixer.h> // ��Ƶ����
#include <unordered_map> // ��ϣӳ������

/**
 * @enum ResID
 * @brief ��Դ��ʶ��ö�٣�����Ψһ��ʶ��Ϸ�еĸ�����Դ
 *
 * ��ö�ٶ�������Ϸ��������Դ��ΨһID������������Ч�����ֺ����塣
 * ʹ��ö�ٿ��Ա����ڴ�����Ӳ�����ַ���·������߿�ά���ԡ�
 */
enum class ResID
{
    // ======== ������Դ ========
    Tex_Tileset, // ��ͼ��Ƭ��

    // ��Һͷ���������
    Tex_Player, // ��ҽ�ɫ
    Tex_Archer, // ��������
    Tex_Axeman, // ������
    Tex_Gunner, // ǹ����

    // ��������
    Tex_Slime,          // ʷ��ķ
    Tex_KingSlime,      // ʷ��ķ��
    Tex_Skeleton,       // ����
    Tex_Goblin,         // �粼��
    Tex_GoblinPriest,   // �粼�ּ�˾
    Tex_SlimeSketch,    // ʷ��ķ��ͼ
    Tex_KingSlimeSketch,// ʷ��ķ����ͼ
    Tex_SkeletonSketch, // ���ò�ͼ
    Tex_GoblinSketch,   // �粼�ֲ�ͼ
    Tex_GoblinPriestSketch, // �粼�ּ�˾��ͼ

    // �ӵ�����
    Tex_BulletArrow, // ��ʸ�ӵ�
    Tex_BulletAxe,   // ��ͷ�ӵ�
    Tex_BulletShell, // �ڵ��ӵ�

    // ��Ϸ��Ʒ����
    Tex_Coin, // ���
    Tex_Home, // ����

    // ��Ч����
    Tex_EffectFlash_Up,    // ����������Ч
    Tex_EffectFlash_Down,  // ����������Ч
    Tex_EffectFlash_Left,  // ����������Ч
    Tex_EffectFlash_Right, // ����������Ч
    Tex_EffectImpact_Up,   // ���ϳ����Ч
    Tex_EffectImpact_Down, // ���³����Ч
    Tex_EffectImpact_Left, // ��������Ч
    Tex_EffectImpact_Right,// ���ҳ����Ч
    Tex_EffectExplode,     // ��ը��Ч

    // UI��������
    Tex_UISelectCursor,      // ѡ����
    Tex_UIPlaceIdle,         // ����������״̬
    Tex_UIPlaceHoveredTop,   // ������������ͣ
    Tex_UIPlaceHoveredLeft,  // �����������ͣ
    Tex_UIPlaceHoveredRight, // �������Ҳ���ͣ
    Tex_UIUpgradeIdle,       // ��������״̬
    Tex_UIUpgradeHoveredTop, // ����������ͣ
    Tex_UIUpgradeHoveredLeft,// ���������ͣ
    Tex_UIUpgradeHoveredRight,// �����Ҳ���ͣ
    Tex_UIHomeAvatar,        // ����ͷ��
    Tex_UIPlayerAvatar,      // ���ͷ��
    Tex_UIHeart,             // ����ͼ��
    Tex_UICoin,              // ���ͼ��
    Tex_UIGameOverBar,       // ��Ϸ������
    Tex_UIWinText,           // ʤ���ı�
    Tex_UILossText,          // ʧ���ı�

    // ======== ��Ч��Դ ========
    Sound_ArrowFire_1, // ���������Ч1
    Sound_ArrowFire_2, // ���������Ч2
    Sound_AxeFire,     // ��ͷ�����Ч
    Sound_ShellFire,   // �ڵ������Ч
    Sound_ArrowHit_1,  // ��ʸ������Ч1
    Sound_ArrowHit_2,  // ��ʸ������Ч2
    Sound_ArrowHit_3,  // ��ʸ������Ч3
    Sound_AxeHit_1,    // ��ͷ������Ч1
    Sound_AxeHit_2,    // ��ͷ������Ч2
    Sound_AxeHit_3,    // ��ͷ������Ч3
    Sound_ShellHit,    // �ڵ�������Ч

    Sound_Flash,       // ������Ч
    Sound_Impact,      // �����Ч

    Sound_Coin,        // ����ռ���Ч
    Sound_HomeHurt,    // ����������Ч
    Sound_PlaceTower,  // ��������Ч
    Sound_TowerLevelUp,// ��������Ч

    Sound_Win,         // ʤ����Ч
    Sound_Loss,        // ʧ����Ч

    // ======== ������Դ ========
    Music_BGM, // ��������

    // ======== ������Դ ========
    Font_Main // ������
};

/**
 * @class ResourcesManager
 * @brief ��Դ�������࣬������غ͹�����Ϸ�е�������Դ
 *
 * ����ʹ�õ���ģʽʵ�֣�ȷ��ȫ��ֻ��һ����Դ������ʵ����
 * �������������͵���Դ�أ��������塢��Ч�����֡�
 * ͨ��ö��ResID�ṩ����Դ��ͳһ���ʽӿڡ�
 */
class ResourcesManager : public Manager<ResourcesManager>
{
    friend class Manager<ResourcesManager>; // ����������������˽�й��캯��

public:
    // ��Դ�����Ͷ���
    typedef std::unordered_map<ResID, TTF_Font*> FontPool;    // �����
    typedef std::unordered_map<ResID, Mix_Chunk*> SoundPool;  // ��Ч��
    typedef std::unordered_map<ResID, Mix_Music*> MusicPool;  // ���ֳ�
    typedef std::unordered_map<ResID, SDL_Texture*> TexturePool; // �����

public:
    /**
     * @brief ���ļ�����������Դ
     * @param renderer SDL��Ⱦ��ָ�룬���ڴ�������
     * @return ���سɹ�����true����һ��Դ����ʧ�ܷ���false
     *
     * �˷�����˳�����������Դ�������һ��Դ����ʧ�ܣ�����������false��
     * ���ع��̷�Ϊ�ĸ����֣�������Ч�����ֺ����塣
     */
    bool load_from_file(SDL_Renderer* renderer)
    {
        // ======== ����������Դ ========
        texture_pool[ResID::Tex_Tileset] = IMG_LoadTexture(renderer, "resources/tileset.png");

        // ��Һͷ���������
        texture_pool[ResID::Tex_Player] = IMG_LoadTexture(renderer, "resources/player.png");
        texture_pool[ResID::Tex_Archer] = IMG_LoadTexture(renderer, "resources/tower_archer.png");
        texture_pool[ResID::Tex_Axeman] = IMG_LoadTexture(renderer, "resources/tower_axeman.png");
        texture_pool[ResID::Tex_Gunner] = IMG_LoadTexture(renderer, "resources/tower_gunner.png");

        // ��������
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

        // �ӵ�����
        texture_pool[ResID::Tex_BulletArrow] = IMG_LoadTexture(renderer, "resources/bullet_arrow.png");
        texture_pool[ResID::Tex_BulletAxe] = IMG_LoadTexture(renderer, "resources/bullet_axe.png");
        texture_pool[ResID::Tex_BulletShell] = IMG_LoadTexture(renderer, "resources/bullet_shell.png");

        // ��Ϸ��Ʒ����
        texture_pool[ResID::Tex_Coin] = IMG_LoadTexture(renderer, "resources/coin.png");
        texture_pool[ResID::Tex_Home] = IMG_LoadTexture(renderer, "resources/home.png");

        // ��Ч����
        texture_pool[ResID::Tex_EffectFlash_Up] = IMG_LoadTexture(renderer, "resources/effect_flash_up.png");
        texture_pool[ResID::Tex_EffectFlash_Down] = IMG_LoadTexture(renderer, "resources/effect_flash_down.png");
        texture_pool[ResID::Tex_EffectFlash_Left] = IMG_LoadTexture(renderer, "resources/effect_flash_left.png");
        texture_pool[ResID::Tex_EffectFlash_Right] = IMG_LoadTexture(renderer, "resources/effect_flash_right.png");
        texture_pool[ResID::Tex_EffectImpact_Up] = IMG_LoadTexture(renderer, "resources/effect_impact_up.png");
        texture_pool[ResID::Tex_EffectImpact_Down] = IMG_LoadTexture(renderer, "resources/effect_impact_down.png");
        texture_pool[ResID::Tex_EffectImpact_Left] = IMG_LoadTexture(renderer, "resources/effect_impact_left.png");
        texture_pool[ResID::Tex_EffectImpact_Right] = IMG_LoadTexture(renderer, "resources/effect_impact_right.png");
        texture_pool[ResID::Tex_EffectExplode] = IMG_LoadTexture(renderer, "resources/effect_explode.png");

        // UI��������
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

        // ������������Ƿ���سɹ�
        for (const auto& pair : texture_pool)
            if (!pair.second) return false;

        // ======== ������Ч��Դ ========
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

        // ���������Ч�Ƿ���سɹ�
        for (const auto& pair : sound_pool)
            if (!pair.second) return false;

        // ======== ����������Դ ========
        music_pool[ResID::Music_BGM] = Mix_LoadMUS("resources/music_bgm.mp3");

        // ������������Ƿ���سɹ�
        for (const auto& pair : music_pool)
            if (!pair.second) return false;

        // ======== ����������Դ ========
        font_pool[ResID::Font_Main] = TTF_OpenFont("resources/ipix.ttf", 25);

        // ������������Ƿ���سɹ�
        for (const auto& pair : font_pool)
            if (!pair.second) return false;

        return true; // ������Դ���سɹ�
    }

    /**
     * @brief ��ȡ������Դ��
     * @return ������Դ�صĳ�������
     */
    const FontPool& get_font_pool()
    {
        return font_pool;
    }

    /**
     * @brief ��ȡ��Ч��Դ��
     * @return ��Ч��Դ�صĳ�������
     */
    const SoundPool& get_sound_pool()
    {
        return sound_pool;
    }

    /**
     * @brief ��ȡ������Դ��
     * @return ������Դ�صĳ�������
     */
    const MusicPool& get_music_pool()
    {
        return music_pool;
    }

    /**
     * @brief ��ȡ������Դ��
     * @return ������Դ�صĳ�������
     */
    const TexturePool& get_texture_pool()
    {
        return texture_pool;
    }

protected:
    ResourcesManager() = default; // �������캯����ȷ������
    ~ResourcesManager() = default; // ������������

private:
    FontPool font_pool;       // ������Դ��
    SoundPool sound_pool;     // ��Ч��Դ��
    MusicPool music_pool;     // ������Դ��
    TexturePool texture_pool; // ������Դ��
};

#endif // !_RESOURCES_MANAGER_H_
