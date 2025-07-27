#ifndef _SHELL_BULLET_H_
#define _SHELL_BULLET_H_

#include "bullet.h"
#include "resources_manager.h"

/**
 * @class ShellBullet
 * @brief 炮弹子弹类，继承自基础子弹类
 *
 * 该类实现炮弹子弹的特定行为，包括爆炸效果和范围伤害。
 * 炮弹子弹通常由枪手塔发射，击中目标后会产生爆炸动画并对周围敌人造成伤害。
 */
class ShellBullet : public Bullet
{
public:
    /**
     * @brief 构造函数：初始化炮弹子弹的属性和动画
     *
     * 1. 加载炮弹和爆炸纹理资源
     * 2. 配置飞行和爆炸动画
     * 3. 设置爆炸范围和尺寸
     */
    ShellBullet()
    {
        // 获取资源管理器中的炮弹和爆炸纹理
        static SDL_Texture* tex_shell = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_BulletShell)->second;
        static SDL_Texture* tex_explode = ResourcesManager::instance()
            ->get_texture_pool().find(ResID::Tex_EffectExplode)->second;

        // 定义动画帧序列
        static const std::vector<int> idx_list = { 0, 1 };         // 炮弹飞行帧
        static const std::vector<int> idx_explode_list = { 0, 1, 2, 3, 4 }; // 爆炸效果帧

        // ===================== 设置炮弹飞行动画 =====================
        animation.set_loop(true);             // 循环播放
        animation.set_interval(0.1);          // 每帧0.1秒
        animation.set_frame_data(tex_shell, 2, 1, idx_list); // 纹理, 2列1行, 帧序列

        // ===================== 设置爆炸效果动画 =====================
        animation_explode.set_loop(false);    // 不循环（单次播放）
        animation_explode.set_interval(0.1);   // 每帧0.1秒
        animation_explode.set_frame_data(tex_explode, 5, 1, idx_explode_list); // 纹理, 5列1行, 帧序列

        // 爆炸动画结束时的回调：标记子弹无效
        animation_explode.set_on_finished([&]() { make_invalid(); });

        // 设置爆炸范围（96像素半径）
        damage_range = 96;

        // 设置子弹尺寸
        size.x = 48, size.y = 48;
    }

    /// 默认析构函数
    ~ShellBullet() = default;

    /**
     * @brief 每帧更新子弹状态
     * @param delta 帧时间增量
     *
     * 1. 如果子弹仍可碰撞（飞行中），调用基类更新
     * 2. 否则（已爆炸），更新爆炸动画
     */
    void on_update(double delta) override
    {
        if (can_collide()) // 仍在飞行中
        {
            Bullet::on_update(delta); // 基类更新（移动位置）
            return;
        }

        // 已爆炸状态：更新爆炸动画
        animation_explode.on_update(delta);
    }

    /**
     * @brief 渲染子弹
     * @param renderer SDL渲染器指针
     *
     * 1. 如果子弹仍可碰撞（飞行中），渲染炮弹
     * 2. 否则（已爆炸），渲染爆炸动画
     */
    void on_render(SDL_Renderer* renderer) override
    {
        if (can_collide()) // 仍在飞行中
        {
            Bullet::on_render(renderer); // 基类渲染（炮弹）
            return;
        }

        // 爆炸效果渲染
        static SDL_Point point;

        // 计算爆炸中心位置（爆炸效果比炮弹大）
        point.x = (int)(position.x - 96 / 2); // 96为爆炸效果尺寸
        point.y = (int)(position.y - 96 / 2);

        // 渲染爆炸动画
        animation_explode.on_render(renderer, point);
    }

    /**
     * @brief 碰撞敌人时的处理
     * @param enemy 被击中的敌人指针
     *
     * 1. 播放炮弹命中音效
     * 2. 禁用碰撞检测（进入爆炸状态）
     * 3. 注意：不调用基类方法，因为需要显示爆炸动画
     */
    void on_collide(Enemy* enemy) override
    {
        // 获取音效资源池
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();

        // 播放炮弹命中音效
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_ShellHit)->second, 0);

        // 禁用碰撞检测（进入爆炸状态）
        disable_collide();

        // 注意：不调用基类的on_collide，因为我们需要显示爆炸动画
        // 而不是立即标记为无效
    }

private:
    Animation animation_explode; ///< 爆炸特效动画
};

#endif // !_SHELL_BULLET_H_
