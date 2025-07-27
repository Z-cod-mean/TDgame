#ifndef _BULLET_MANAGER_H_
#define _BULLET_MANAGER_H_

#include "bullet.h"
#include "manager.h"          // 管理器基类
#include "arrow_bullet.h"     // 箭矢子弹
#include "axe_bullet.h"       // 斧头子弹
#include "shell_bullet.h"     // 炮弹子弹
#include "bullet_type.h"      // 子弹类型枚举

#include <vector>             // 向量容器

/**
 * @class BulletManager
 * @brief 子弹管理器类，负责游戏中所有子弹的生命周期管理
 *
 * 该类使用单例模式实现，全局唯一。主要功能包括：
 * 1. 创建和存储所有子弹对象
 * 2. 每帧更新子弹状态
 * 3. 每帧渲染子弹
 * 4. 自动清理可移除的子弹
 */
class BulletManager : public Manager<BulletManager>
{
    friend class Manager<BulletManager>; // 允许基类访问私有构造函数

public:
    typedef std::vector<Bullet*> BulletList; ///< 子弹列表类型

public:
    /**
     * @brief 每帧更新所有子弹状态
     * @param delta 帧时间增量（秒）
     *
     * 1. 遍历所有子弹并更新状态
     * 2. 清理标记为可移除的子弹
     */
    void on_update(double delta)
    {
        // 更新所有子弹
        for (Bullet* bullet : bullet_list)
            bullet->on_update(delta);

        // 清理可移除的子弹（使用remove-erase惯用法）
        bullet_list.erase(std::remove_if(
            bullet_list.begin(),
            bullet_list.end(),
            [](const Bullet* bullet) {
                bool deletable = bullet->can_remove();
                if (deletable) delete bullet; // 释放内存
                return deletable;
            }),
            bullet_list.end());
    }

    /**
     * @brief 渲染所有子弹
     * @param renderer SDL渲染器指针
     */
    void on_render(SDL_Renderer* renderer)
    {
        for (Bullet* bullet : bullet_list)
            bullet->on_render(renderer);
    }

    /**
     * @brief 获取子弹列表引用
     * @return 当前所有活跃子弹的列表
     */
    BulletList& get_bullet_list()
    {
        return bullet_list;
    }

    /**
     * @brief 发射新子弹
     * @param type 子弹类型（箭头/斧头/炮弹）
     * @param position 发射位置（二维向量）
     * @param velocity 子弹速度（二维向量）
     * @param damage 子弹伤害值
     *
     * 根据子弹类型创建对应的子弹对象，设置初始属性，并添加到管理列表
     */
    void fire_bullet(BulletType type, const Vector2& position, const Vector2& velocity, double damage)
    {
        Bullet* bullet = nullptr; // 新子弹指针

        // 根据子弹类型创建具体子弹对象
        switch (type)
        {
        case Arrow: // 箭矢子弹
            bullet = new ArrowBullet();
            break;
        case Axe:   // 斧头子弹
            bullet = new AxeBullet();
            break;
        case Shell: // 炮弹子弹
            bullet = new ShellBullet();
            break;
        default:    // 默认使用箭矢子弹
            bullet = new ArrowBullet();
            break;
        }

        // 设置子弹初始属性
        bullet->set_position(position); // 位置
        bullet->set_velocity(velocity); // 速度
        bullet->set_damage(damage);     // 伤害值

        // 添加到子弹列表
        bullet_list.push_back(bullet);
    }

protected:
    /// 保护构造函数（单例模式）
    BulletManager() = default;

    /// 析构函数：清理所有剩余子弹
    ~BulletManager()
    {
        for (Bullet* bullet : bullet_list)
            delete bullet; // 释放内存
    }

private:
    BulletList bullet_list; ///< 存储所有活跃子弹的列表
};

#endif // !_BULLET_MANAGER_H_
