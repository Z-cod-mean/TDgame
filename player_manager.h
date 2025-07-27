#ifndef _PLAYER_MANAGER_H_
#define _PLAYER_MANAGER_H_

#include "tile.h"
#include "facing.h"
#include "vector2.h"
#include "manager.h"
#include "animation.h"
#include "coin_manager.h"
#include "enemy_manager.h"
#include "resources_manager.h"

#include <SDL.h>

// 玩家管理类，继承自Manager模板类
// 负责处理玩家角色相关的所有逻辑：输入、移动、攻击、动画等
class PlayerManager : public Manager<PlayerManager>
{
    friend class Manager<PlayerManager>;  // 声明友元，允许Manager模板类访问私有成员

public:
    // 处理输入事件
    // 参数：event - SDL事件对象
    void on_input(const SDL_Event& event)
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:  // 按键按下事件
            switch (event.key.keysym.sym)
            {
            case SDLK_a:    // A键 - 向左移动
                is_move_left = true;
                break;
            case SDLK_d:    // D键 - 向右移动
                is_move_right = true;
                break;
            case SDLK_w:    // W键 - 向上移动
                is_move_up = true;
                break;
            case SDLK_s:    // S键 - 向下移动
                is_move_down = true;
                break;
            case SDLK_j:    // J键 - 释放闪光技能
                on_release_flash();
                break;
            case SDLK_k:    // K键 - 释放冲击技能
                on_release_impact();
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:    // 按键释放事件
            switch (event.key.keysym.sym)
            {
            case SDLK_a:    // 停止向左移动
                is_move_left = false;
                break;
            case SDLK_d:    // 停止向右移动
                is_move_right = false;
                break;
            case SDLK_w:    // 停止向上移动
                is_move_up = false;
                break;
            case SDLK_s:    // 停止向下移动
                is_move_down = false;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    // 更新游戏逻辑
    // 参数：delta - 距离上一帧的时间间隔(秒)
    void on_update(double delta)
    {
        // 更新计时器
        timer_auto_increase_mp.on_update(delta);       // MP自动恢复计时器
        timer_release_flash_cd.on_update(delta);        // 闪光技能冷却计时器

        // 计算移动方向向量并归一化
        Vector2 direction =
            Vector2(is_move_right - is_move_left,
                is_move_down - is_move_up).normalize();
        // 计算实际速度(像素/秒)
        velocity = direction * speed * SIZE_TILE;

        // 如果不是正在释放技能，则处理移动逻辑
        if (!is_releasing_flash && !is_releasing_impact)
        {
            // 更新位置
            position += velocity * delta;

            // 确保玩家不会移出地图边界
            const SDL_Rect& rect_map = ConfigManager::instance()->rect_tile_map;
            if (position.x < rect_map.x) position.x = rect_map.x;
            if (position.x > rect_map.x + rect_map.w) position.x = rect_map.x + rect_map.w;
            if (position.y < rect_map.y) position.y = rect_map.y;
            if (position.y > rect_map.y + rect_map.h) position.y = rect_map.y + rect_map.h;

            // 根据移动方向更新角色朝向
            if (velocity.y > 0) facing = Facing::Down;
            if (velocity.y < 0) facing = Facing::Up;
            if (velocity.x > 0) facing = Facing::Right;
            if (velocity.x < 0) facing = Facing::Left;

            // 根据朝向设置对应的站立动画
            switch (facing)
            {
            case Facing::Left:
                anim_current = &anim_idle_left;
                break;
            case Facing::Right:
                anim_current = &anim_idle_right;
                break;
            case Facing::Up:
                anim_current = &anim_idle_up;
                break;
            case Facing::Down:
                anim_current = &anim_idle_down;
                break;
            }
        }
        else
        {
            // 如果正在释放技能，则设置攻击动画
            switch (facing)
            {
            case Facing::Left:
                anim_current = &anim_attack_left;
                break;
            case Facing::Right:
                anim_current = &anim_attack_right;
                break;
            case Facing::Up:
                anim_current = &anim_attack_up;
                break;
            case Facing::Down:
                anim_current = &anim_attack_down;
                break;
            }
        }

        // 更新当前动画
        anim_current->on_update(delta);

        // 处理闪光技能效果
        if (is_releasing_flash)
        {
            // 更新闪光特效动画
            anim_effect_flash_current->on_update(delta);

            // 获取敌人列表并检测碰撞
            EnemyManager::EnemyList& enemy_list
                = EnemyManager::instance()->get_enemy_list();
            for (Enemy* enemy : enemy_list)
            {
                if (enemy->can_remove())  // 跳过已标记移除的敌人
                    continue;

                // 检查敌人是否在闪光技能的攻击范围内
                const Vector2& position = enemy->get_position();
                if (position.x >= rect_hitbox_flash.x
                    && position.x <= rect_hitbox_flash.x + rect_hitbox_flash.w
                    && position.y >= rect_hitbox_flash.y
                    && position.y <= rect_hitbox_flash.y + rect_hitbox_flash.h)
                {
                    // 对敌人造成伤害(伤害值随时间累计)
                    enemy->decrease_hp(ConfigManager::instance()->player_template.normal_attack_damage * delta);
                }
            }
        }

        // 处理冲击技能效果
        if (is_releasing_impact)
        {
            // 更新冲击特效动画
            anim_effect_impact_current->on_update(delta);

            // 获取敌人列表并检测碰撞
            EnemyManager::EnemyList& enemy_list
                = EnemyManager::instance()->get_enemy_list();
            for (Enemy* enemy : enemy_list)
            {
                if (enemy->can_remove())  // 跳过已标记移除的敌人
                    continue;

                // 检查敌人是否在冲击技能的攻击范围内
                const Vector2& size = enemy->get_size();
                const Vector2& position = enemy->get_position();
                if (position.x >= rect_hitbox_impact.x
                    && position.x <= rect_hitbox_impact.x + rect_hitbox_impact.w
                    && position.y >= rect_hitbox_impact.y
                    && position.y <= rect_hitbox_impact.y + rect_hitbox_impact.h)
                {
                    // 对敌人造成伤害并减速
                    enemy->decrease_hp(ConfigManager::instance()->player_template.skill_damage * delta);
                    enemy->slow_down();
                }
            }
        }

        // 检测与金币道具的碰撞
        CoinManager::CoinPropList& coin_prop_list = CoinManager::instance()->get_coin_prop_list();
        static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::instance()->get_sound_pool();

        for (CoinProp* coin_prop : coin_prop_list)
        {
            if (coin_prop->can_remove())  // 跳过已收集的金币
                continue;

            // 检查玩家是否与金币碰撞
            const Vector2& pos_coin_prop = coin_prop->get_position();
            if (pos_coin_prop.x >= position.x - size.x / 2
                && pos_coin_prop.x <= position.x + size.x / 2
                && pos_coin_prop.y >= position.y - size.y / 2
                && pos_coin_prop.y <= position.y + size.y / 2)
            {
                // 收集金币
                coin_prop->make_invalid();
                CoinManager::instance()->increase_coin(10);

                // 播放收集音效
                Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_Coin)->second, 0);
            }
        }
    }

    // 渲染玩家角色和技能特效
    // 参数：renderer - SDL渲染器
    void on_render(SDL_Renderer* renderer)
    {
        static SDL_Point point;  // 用于存储渲染位置

        // 渲染玩家角色
        point.x = (int)(position.x - size.x / 2);
        point.y = (int)(position.y - size.y / 2);
        anim_current->on_render(renderer, point);

        // 如果正在释放闪光技能，渲染闪光特效
        if (is_releasing_flash)
        {
            point.x = rect_hitbox_flash.x;
            point.y = rect_hitbox_flash.y;
            anim_effect_flash_current->on_render(renderer, point);
        }

        // 如果正在释放冲击技能，渲染冲击特效
        if (is_releasing_impact)
        {
            point.x = rect_hitbox_impact.x;
            point.y = rect_hitbox_impact.y;
            anim_effect_impact_current->on_render(renderer, point);
        }
    }

    // 获取当前MP值
    double get_current_mp() const
    {
        return mp;
    }

protected:
    // 构造函数(protected，因为这是单例类)
    PlayerManager()
    {
        // 设置MP自动恢复计时器
        timer_auto_increase_mp.set_one_shot(false);  // 循环计时器
        timer_auto_increase_mp.set_wait_time(0.1);   // 每0.1秒触发一次
        timer_auto_increase_mp.set_on_timeout(
            [&]()  // 计时器回调函数
            {
                // 按照配置的恢复速度增加MP
                double interval = ConfigManager::instance()->player_template.skill_interval;
                mp = std::min(mp + 100 / (interval / 0.1), 100.0);  // 确保不超过最大值100
            });

        // 设置闪光技能冷却计时器
        timer_release_flash_cd.set_one_shot(true);  // 单次计时器
        timer_release_flash_cd.set_wait_time(
            ConfigManager::instance()->player_template.normal_attack_interval);  // 从配置读取冷却时间
        timer_release_flash_cd.set_on_timeout(
            [&]()  // 计时器回调函数
            {
                can_release_flash = true;  // 冷却结束后可以再次释放技能
            });

        // 从资源管理器获取纹理
        const ResourcesManager::TexturePool& tex_pool
            = ResourcesManager::instance()->get_texture_pool();

        SDL_Texture* tex_player = tex_pool.find(ResID::Tex_Player)->second;

        // 初始化站立动画(上、下、左、右)
        anim_idle_up.set_loop(true); anim_idle_up.set_interval(0.1);
        anim_idle_up.set_frame_data(tex_player, 4, 8, { 4, 5, 6, 7 });
        anim_idle_down.set_loop(true); anim_idle_down.set_interval(0.1);
        anim_idle_down.set_frame_data(tex_player, 4, 8, { 0, 1, 2, 3 });
        anim_idle_left.set_loop(true); anim_idle_left.set_interval(0.1);
        anim_idle_left.set_frame_data(tex_player, 4, 8, { 8, 9, 10, 11 });
        anim_idle_right.set_loop(true); anim_idle_right.set_interval(0.1);
        anim_idle_right.set_frame_data(tex_player, 4, 8, { 12, 13, 14, 15 });

        // 初始化攻击动画(上、下、左、右)
        anim_attack_up.set_loop(true); anim_attack_up.set_interval(0.1);
        anim_attack_up.set_frame_data(tex_player, 4, 8, { 20, 21 });
        anim_attack_down.set_loop(true); anim_attack_down.set_interval(0.1);
        anim_attack_down.set_frame_data(tex_player, 4, 8, { 16, 17 });
        anim_attack_left.set_loop(true); anim_attack_left.set_interval(0.1);
        anim_attack_left.set_frame_data(tex_player, 4, 8, { 24, 25 });
        anim_attack_right.set_loop(true); anim_attack_right.set_interval(0.1);
        anim_attack_right.set_frame_data(tex_player, 4, 8, { 28, 29 });

        // 初始化闪光技能特效动画(上、下、左、右)
        anim_effect_flash_up.set_loop(false); anim_effect_flash_up.set_interval(0.1);
        anim_effect_flash_up.set_frame_data(tex_pool.find(ResID::Tex_EffectFlash_Up)->second, 5, 1, { 0, 1, 2, 3, 4 });
        anim_effect_flash_up.set_on_finished([&]() { is_releasing_flash = false; });  // 动画结束时标记技能结束
        anim_effect_flash_down.set_loop(false); anim_effect_flash_down.set_interval(0.1);
        anim_effect_flash_down.set_frame_data(tex_pool.find(ResID::Tex_EffectFlash_Down)->second, 5, 1, { 4, 3, 2, 1, 0 });
        anim_effect_flash_down.set_on_finished([&]() { is_releasing_flash = false; });
        anim_effect_flash_left.set_loop(false); anim_effect_flash_left.set_interval(0.1);
        anim_effect_flash_left.set_frame_data(tex_pool.find(ResID::Tex_EffectFlash_Left)->second, 1, 5, { 4, 3, 2, 1, 0 });
        anim_effect_flash_left.set_on_finished([&]() { is_releasing_flash = false; });
        anim_effect_flash_right.set_loop(false); anim_effect_flash_right.set_interval(0.1);
        anim_effect_flash_right.set_frame_data(tex_pool.find(ResID::Tex_EffectFlash_Right)->second, 1, 5, { 0, 1, 2, 3, 4 });
        anim_effect_flash_right.set_on_finished([&]() { is_releasing_flash = false; });

        // 初始化冲击技能特效动画(上、下、左、右)
        anim_effect_impact_up.set_loop(false); anim_effect_impact_up.set_interval(0.1);
        anim_effect_impact_up.set_frame_data(tex_pool.find(ResID::Tex_EffectImpact_Up)->second, 5, 1, { 0, 1, 2, 3, 4 });
        anim_effect_impact_up.set_on_finished([&]() { is_releasing_impact = false; });  // 动画结束时标记技能结束
        anim_effect_impact_down.set_loop(false); anim_effect_impact_down.set_interval(0.1);
        anim_effect_impact_down.set_frame_data(tex_pool.find(ResID::Tex_EffectImpact_Down)->second, 5, 1, { 4, 3, 2, 1, 0 });
        anim_effect_impact_down.set_on_finished([&]() { is_releasing_impact = false; });
        anim_effect_impact_left.set_loop(false); anim_effect_impact_left.set_interval(0.1);
        anim_effect_impact_left.set_frame_data(tex_pool.find(ResID::Tex_EffectImpact_Left)->second, 1, 5, { 4, 3, 2, 1, 0 });
        anim_effect_impact_left.set_on_finished([&]() { is_releasing_impact = false; });
        anim_effect_impact_right.set_loop(false); anim_effect_impact_right.set_interval(0.1);
        anim_effect_impact_right.set_frame_data(tex_pool.find(ResID::Tex_EffectImpact_Right)->second, 1, 5, { 0, 1, 2, 3, 4 });
        anim_effect_impact_right.set_on_finished([&]() { is_releasing_impact = false; });

        // 初始化玩家位置(地图中心)
        const SDL_Rect& rect_map = ConfigManager::instance()->rect_tile_map;
        position.x = rect_map.x + rect_map.w / 2;
        position.y = rect_map.y + rect_map.h / 2;

        // 从配置读取玩家移动速度
        speed = ConfigManager::instance()->player_template.speed;

        // 设置玩家角色大小
        size.x = 96, size.y = 96;
    }

    // 析构函数
    ~PlayerManager() = default;

private:
    // 玩家属性
    Vector2 size;        // 玩家尺寸(像素)
    Vector2 position;    // 玩家位置(世界坐标)
    Vector2 velocity;    // 玩家速度(像素/秒)

    // 技能攻击范围
    SDL_Rect rect_hitbox_flash = { 0 };    // 闪光技能攻击范围
    SDL_Rect rect_hitbox_impact = { 0 };   // 冲击技能攻击范围

    double mp = 100;     // 魔法值(0-100)
    double speed = 0;    // 移动速度

    // 技能状态标志
    bool can_release_flash = true;    // 是否可以释放闪光技能
    bool is_releasing_flash = false;  // 是否正在释放闪光技能
    bool is_releasing_impact = false; // 是否正在释放冲击技能

    // 移动状态标志
    bool is_move_up = false;    // 是否向上移动
    bool is_move_down = false;  // 是否向下移动
    bool is_move_left = false;  // 是否向左移动
    bool is_move_right = false; // 是否向右移动

    // 动画资源
    Animation anim_idle_up;      // 站立(上)动画
    Animation anim_idle_down;    // 站立(下)动画
    Animation anim_idle_left;    // 站立(左)动画
    Animation anim_idle_right;   // 站立(右)动画
    Animation anim_attack_up;    // 攻击(上)动画
    Animation anim_attack_down;  // 攻击(下)动画
    Animation anim_attack_left;  // 攻击(左)动画
    Animation anim_attack_right; // 攻击(右)动画
    Animation* anim_current = &anim_idle_right;  // 当前播放的动画

    // 技能特效动画
    Animation anim_effect_flash_up;     // 闪光特效(上)
    Animation anim_effect_flash_down;   // 闪光特效(下)
    Animation anim_effect_flash_left;   // 闪光特效(左)
    Animation anim_effect_flash_right;  // 闪光特效(右)
    Animation* anim_effect_flash_current = nullptr;  // 当前闪光特效动画

    Animation anim_effect_impact_up;    // 冲击特效(上)
    Animation anim_effect_impact_down;  // 冲击特效(下)
    Animation anim_effect_impact_left;  // 冲击特效(左)
    Animation anim_effect_impact_right; // 冲击特效(右)
    Animation* anim_effect_impact_current = nullptr;  // 当前冲击特效动画

    // 计时器
    Timer timer_release_flash_cd;    // 闪光技能冷却计时器
    Timer timer_auto_increase_mp;   // MP自动恢复计时器

    Facing facing = Facing::Left;   // 玩家当前朝向(默认为左)

private:
    // 释放闪光技能
    void on_release_flash()
    {
        // 检查是否可以释放技能
        if (!can_release_flash || is_releasing_flash)
            return;

        // 根据玩家朝向设置不同的攻击范围和动画
        switch (facing)
        {
        case Facing::Left:  // 向左攻击
            anim_effect_flash_current = &anim_effect_flash_left;
            rect_hitbox_flash.x = (int)(position.x - size.x / 2 - 300);  // 攻击范围在左侧300像素
            rect_hitbox_flash.y = (int)(position.y - 68 / 2);           // 高度68像素
            rect_hitbox_flash.w = 300, rect_hitbox_flash.h = 68;
            break;
        case Facing::Right:  // 向右攻击
            anim_effect_flash_current = &anim_effect_flash_right;
            rect_hitbox_flash.x = (int)(position.x + size.x / 2);  // 攻击范围在右侧300像素
            rect_hitbox_flash.y = (int)(position.y - 68 / 2);
            rect_hitbox_flash.w = 300, rect_hitbox_flash.h = 68;
            break;
        case Facing::Up:  // 向上攻击
            anim_effect_flash_current = &anim_effect_flash_up;
            rect_hitbox_flash.x = (int)(position.x - 68 / 2);  // 宽度68像素
            rect_hitbox_flash.y = (int)(position.y - size.x / 2 - 300);  // 攻击范围在上方300像素
            rect_hitbox_flash.w = 68, rect_hitbox_flash.h = 300;
            break;
        case Facing::Down:  // 向下攻击
            anim_effect_flash_current = &anim_effect_flash_down;
            rect_hitbox_flash.x = (int)(position.x - 68 / 2);
            rect_hitbox_flash.y = (int)(position.y + size.x / 2);  // 攻击范围在下方300像素
            rect_hitbox_flash.w = 68, rect_hitbox_flash.h = 300;
            break;
        }

        // 设置技能状态
        is_releasing_flash = true;
        anim_effect_flash_current->reset();  // 重置动画
        timer_release_flash_cd.restart();    // 开始冷却计时
        can_release_flash = false;           // 进入冷却状态

        // 播放技能音效
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_Flash)->second, 0);
    }

    // 释放冲击技能(消耗全部MP)
    void on_release_impact()
    {
        // 检查MP是否足够且没有正在释放技能
        if (mp < 100 || is_releasing_impact)
            return;

        // 根据玩家朝向设置不同的攻击范围和动画
        switch (facing)
        {
        case Facing::Left:  // 向左攻击
            anim_effect_impact_current = &anim_effect_impact_left;
            rect_hitbox_impact.x = (int)(position.x - size.x / 2 - 60);  // 攻击范围在左侧60像素
            rect_hitbox_impact.y = (int)(position.y - 140 / 2);         // 高度140像素
            rect_hitbox_impact.w = 60, rect_hitbox_impact.h = 140;
            break;
        case Facing::Right:  // 向右攻击
            anim_effect_impact_current = &anim_effect_impact_right;
            rect_hitbox_impact.x = (int)(position.x + size.x / 2);  // 攻击范围在右侧60像素
            rect_hitbox_impact.y = (int)(position.y - 140 / 2);
            rect_hitbox_impact.w = 60, rect_hitbox_impact.h = 140;
            break;
        case Facing::Up:  // 向上攻击
            anim_effect_impact_current = &anim_effect_impact_up;
            rect_hitbox_impact.x = (int)(position.x - 140 / 2);  // 宽度140像素
            rect_hitbox_impact.y = (int)(position.y - size.x / 2 - 60);  // 攻击范围在上方60像素
            rect_hitbox_impact.w = 140, rect_hitbox_impact.h = 60;
            break;
        case Facing::Down:  // 向下攻击
            anim_effect_impact_current = &anim_effect_impact_down;
            rect_hitbox_impact.x = (int)(position.x - 140 / 2);
            rect_hitbox_impact.y = (int)(position.y + size.x / 2);  // 攻击范围在下方60像素
            rect_hitbox_impact.w = 140, rect_hitbox_impact.h = 60;
            break;
        }

        // 消耗MP并设置技能状态
        mp = 0;
        is_releasing_impact = true;
        anim_effect_impact_current->reset();  // 重置动画

        // 播放技能音效
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_Impact)->second, 0);
    }
};

#endif // !_PLAYER_MANAGER_H_
