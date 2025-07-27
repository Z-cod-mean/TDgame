#ifndef _AXE_BULLET_H_
#define _AXE_BULLET_H_

#include "bullet.h"
#include "resources_manager.h"

/**
 * @class AxeBullet
 * @brief 斧头子弹类，继承自基础子弹类
 *
 * 该类实现斧头子弹的特定行为，包括旋转动画和特殊碰撞效果。
 * 斧头子弹通常由斧兵塔发射，具有使敌人减速的特殊效果。
 */
class AxeBullet : public Bullet
{
public:
    /**
     * @brief 构造函数：初始化斧头子弹的属性和动画
     *
     * 1. 加载斧头纹理资源
     * 2. 配置动画帧序列（9帧旋转动画）
     * 3. 设置子弹尺寸
     */
    AxeBullet()
    {
        // 获取资源管理器中的斧头纹理
        static SDL_Texture* tex_axe = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_BulletAxe)->second;

        // 定义动画帧序列（斧头旋转动画的9个帧）
        static const std::vector<int> idx_list = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

        // 设置动画属性
        animation.set_loop(true);      // 循环播放动画
        animation.set_interval(0.1);   // 每帧0.1秒（10 FPS）
        animation.set_frame_data(tex_axe, 4, 2, idx_list); // 纹理, 4列2行, 帧序列

        // 设置子弹尺寸
        size.x = 48;          // 子弹宽度
        size.y = 48;          // 子弹高度
    }

    /// 默认析构函数
    ~AxeBullet() = default;

    /**
     * @brief 碰撞敌人时的处理（重写基类方法）
     * @param enemy 被击中的敌人指针
     *
     * 1. 随机播放三种斧头命中音效之一
     * 2. 对敌人施加减速效果
     * 3. 调用基类的碰撞处理逻辑
     */
    void on_collide(Enemy* enemy) override
    {
        // 获取音效资源池
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();

        // 随机选择三种命中音效之一播放
        switch (rand() % 3)
        {
        case 0:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_1)->second, 0);
            break;
        case 1:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_2)->second, 0);
            break;
        case 2:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_AxeHit_3)->second, 0);
            break;
        }

        // 对敌人施加减速效果
        enemy->slow_down();

        // 调用基类的碰撞处理（标记子弹为无效等）
        Bullet::on_collide(enemy);
    }
};

#endif // !_AXE_BULLET_H_
