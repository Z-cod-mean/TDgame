#ifndef _ARROW_BULLET_H_
#define _ARROW_BULLET_H_

#include "bullet.h"
#include "resources_manager.h"

/**
 * @class ArrowBullet
 * @brief 箭矢子弹类，继承自基础子弹类
 *
 * 该类实现箭矢子弹的特定行为，包括旋转效果和碰撞音效。
 * 箭矢子弹通常由弓箭手塔发射，具有穿透效果或特殊音效。
 */
class ArrowBullet : public Bullet
{
public:
    /**
     * @brief 构造函数：初始化箭矢子弹的属性和动画
     *
     * 1. 加载箭矢纹理资源
     * 2. 配置动画帧序列
     * 3. 设置子弹属性（可旋转、尺寸等）
     */
    ArrowBullet()
    {
        // 获取资源管理器中的箭矢纹理
        static SDL_Texture* tex_arrow = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_BulletArrow)->second;

        // 定义动画帧序列（精灵表中的帧索引）
        static const std::vector<int> idx_list = { 0, 1 }; // 使用前两帧

        // 设置动画属性
        animation.set_loop(true);      // 循环播放动画
        animation.set_interval(0.1);   // 每帧0.1秒（10 FPS）
        animation.set_frame_data(tex_arrow, 2, 1, idx_list); // 纹理, 2列1行, 帧序列

        // 设置子弹特性
        can_rotated = true;   // 允许子弹根据飞行方向旋转
        size.x = 48;          // 子弹宽度
        size.y = 48;          // 子弹高度
    }

    /// 默认析构函数
    ~ArrowBullet() = default;

    /**
     * @brief 碰撞敌人时的处理（重写基类方法）
     * @param enemy 被击中的敌人指针
     *
     * 1. 随机播放三种箭矢命中音效之一
     * 2. 调用基类的碰撞处理逻辑
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
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_1)->second, 0);
            break;
        case 1:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_2)->second, 0);
            break;
        case 2:
            Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ArrowHit_3)->second, 0);
            break;
        }

        // 调用基类的碰撞处理（标记子弹为无效等）
        Bullet::on_collide(enemy);
    }
};

#endif // !_ARROW_BULLET_H_
