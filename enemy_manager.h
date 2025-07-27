#ifndef _ENEMY_MANAGER_H_
#define _ENEMY_MANAGER_H_

// 依赖头文件
#include "enemy.h"                   // 敌人基类
#include "manager.h"                 // 单例基类
#include "config_manager.h"          // 配置数据
#include "home_manager.h"            // 基地管理
#include "slim_enemy.h"              // 具体敌人类型
#include "king_slim_enemy.h"         // (继承自Enemy)
#include "skeleton_enemy.h"
#include "goblin_enemy.h"
#include "goblin_priest_enemy.h"
#include "bullet_manager.h"          // 子弹系统
#include "coin_manager.h"            // 金币奖励

#include <vector>                    // STL容器
#include <SDL.h>                     // SDL基础库

/**
 * @class EnemyManager
 * @brief 敌人生命周期管理系统（单例模式）
 *
 * 核心职责：
 * 1. 敌人生成/销毁管理
 * 2. 碰撞检测（子弹/基地）
 * 3. 敌人状态更新与渲染
 * 4. 死亡奖励处理
 */
class EnemyManager : public Manager<EnemyManager> {
    friend class Manager<EnemyManager>;  // 允许单例模板访问私有构造

public:
    typedef std::vector<Enemy*> EnemyList;  // 敌人容器类型定义

public:
    /**
     * @brief 每帧更新所有敌人状态
     * @param delta 帧间隔时间（秒）
     *
     * 处理流程：
     * 1. 更新每个敌人的内部状态
     * 2. 检测与基地的碰撞
     * 3. 检测与子弹的碰撞
     * 4. 清理已标记删除的敌人
     */
    void on_update(double delta) {
        // 更新所有敌人逻辑
        for (Enemy* enemy : enemy_list)
            enemy->on_update(delta);

        // 碰撞检测
        process_home_collision();   // 基地碰撞
        process_bullet_collision(); // 子弹碰撞

        // 清理无效敌人
        remove_invalid_enemy();
    }

    /**
     * @brief 渲染所有敌人
     * @param renderer SDL渲染器上下文
     */
    void on_render(SDL_Renderer* renderer) {
        for (Enemy* enemy : enemy_list)
            enemy->on_render(renderer);
    }

    /**
     * @brief 生成指定类型的敌人
     * @param type 敌人枚举类型
     * @param idx_spawn_point 出生点索引
     *
     * 工作流程：
     * 1. 根据类型创建具体敌人实例
     * 2. 设置初始位置和移动路径
     * 3. 配置治疗技能回调
     * 4. 加入管理列表
     */
    void spawn_enemy(EnemyType type, int idx_spawn_point) {
        static Vector2 position;
        static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
        static const Map::SpawnerRoutePool& spawner_route_pool
            = ConfigManager::instance()->map.get_idx_spawner_pool();

        // 验证出生点有效性
        const auto& itor = spawner_route_pool.find(idx_spawn_point);
        if (itor == spawner_route_pool.end())
            return;

        Enemy* enemy = nullptr;

        // 工厂模式：根据类型创建具体敌人
        switch (type) {
        case EnemyType::Slim:
            enemy = new SlimEnemy();
            break;
        case EnemyType::KingSlim:
            enemy = new KingSlimeEnemy();
            break;
        case EnemyType::Skeleton:
            enemy = new SkeletonEnemy();
            break;
        case EnemyType::Goblin:
            enemy = new GoblinEnemy();
            break;
        case EnemyType::GoblinPriest:
            enemy = new GoblinPriestEnemy();
            break;
        default:  // 默认生成史莱姆
            enemy = new SlimEnemy();
            break;
        }

        // 配置治疗技能回调（观察者模式）
        enemy->set_on_skill_released([&](Enemy* enemy_src) {
            double recover_raduis = enemy_src->get_recover_radius();
            if (recover_raduis < 0) return;

            // 范围内友军回血
            const Vector2 pos_src = enemy_src->get_position();
            for (Enemy* enemy_dst : enemy_list) {
                const Vector2& pos_dst = enemy_dst->get_position();
                double distance = (pos_dst - pos_src).length();
                if (distance <= recover_raduis)
                    enemy_dst->increase_hp(enemy_src->get_recover_intensity());
            }
            });

        // 设置初始位置（出生点第一个瓦片中心）
        const Route::IdxList& idx_list = itor->second.get_idx_list();
        position.x = rect_tile_map.x + idx_list[0].x * SIZE_TILE + SIZE_TILE / 2;
        position.y = rect_tile_map.y + idx_list[0].y * SIZE_TILE + SIZE_TILE / 2;

        enemy->set_position(position);
        enemy->set_route(&itor->second);  // 绑定移动路径

        enemy_list.push_back(enemy);  // 加入管理列表
    }

    /**
     * @brief 检查场景中是否无存活敌人
     * @return 是否清空
     */
    bool check_cleared() {
        return enemy_list.empty();
    }

    /**
     * @brief 获取敌人列表引用（用于跨系统访问）
     */
    EnemyList& get_enemy_list() {
        return enemy_list;
    }

protected:
    EnemyManager() = default;  // 默认构造

    /**
     * @brief 析构时释放所有敌人内存
     */
    ~EnemyManager() {
        for (Enemy* enemy : enemy_list)
            delete enemy;
    }

private:
    EnemyList enemy_list;  // 当前存活的敌人集合

private:
    /**
     * @brief 处理敌人与基地的碰撞
     *
     * 当敌人进入基地所在瓦片时：
     * 1. 标记敌人为待删除状态
     * 2. 对基地造成伤害
     */
    void process_home_collision() {
        static const SDL_Point& idx_home = ConfigManager::instance()->map.get_idx_home();
        static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
        static const Vector2 position_home_tile = {
            (double)rect_tile_map.x + idx_home.x * SIZE_TILE,
            (double)rect_tile_map.y + idx_home.y * SIZE_TILE
        };

        for (Enemy* enemy : enemy_list) {
            if (enemy->can_remove()) continue;

            // AABB碰撞检测
            const Vector2& position = enemy->get_position();
            if (position.x >= position_home_tile.x
                && position.y >= position_home_tile.y
                && position.x <= position_home_tile.x + SIZE_TILE
                && position.y <= position_home_tile.y + SIZE_TILE) {
                enemy->make_invalid();  // 标记删除
                HomeManager::instance()->decrease_hp(enemy->get_damage());
            }
        }
    }

    /**
     * @brief 处理子弹与敌人的碰撞
     *
     * 支持两种伤害模式：
     * 1. 单体伤害：直接对目标造成伤害
     * 2. 范围伤害：影响爆炸半径内所有敌人
     */
    void process_bullet_collision() {
        static BulletManager::BulletList& bullet_list
            = BulletManager::instance()->get_bullet_list();

        for (Enemy* enemy : enemy_list) {
            if (enemy->can_remove()) continue;

            const Vector2& size_enemy = enemy->get_size();
            const Vector2& pos_enemy = enemy->get_position();

            for (Bullet* bullet : bullet_list) {
                if (!bullet->can_collide()) continue;

                // 子弹与敌人的AABB碰撞检测
                const Vector2& pos_bullet = bullet->get_position();
                if (pos_bullet.x >= pos_enemy.x - size_enemy.x / 2
                    && pos_bullet.y >= pos_enemy.y - size_enemy.y / 2
                    && pos_bullet.x <= pos_enemy.x + size_enemy.x / 2
                    && pos_bullet.y <= pos_enemy.y + size_enemy.y / 2) {

                    double damage = bullet->get_damage();
                    double damage_range = bullet->get_damage_range();

                    // 单体伤害模式
                    if (damage_range < 0) {
                        enemy->decrease_hp(damage);
                        if (enemy->can_remove())
                            try_spawn_coin_prop(pos_enemy, enemy->get_reward_ratio());
                    }
                    // 范围伤害模式
                    else {
                        for (Enemy* target_enemy : enemy_list) {
                            const Vector2& pos_target_enemy = target_enemy->get_position();
                            if ((pos_target_enemy - pos_bullet).length() <= damage_range) {
                                target_enemy->decrease_hp(damage);
                                if (target_enemy->can_remove())
                                    try_spawn_coin_prop(pos_target_enemy, enemy->get_reward_ratio());
                            }
                        }
                    }

                    bullet->on_collide(enemy);  // 触发子弹碰撞效果
                }
            }
        }
    }

    /**
     * @brief 清理标记删除的敌人
     *
     * 使用STL remove-erase惯用法高效删除元素
     */
    void remove_invalid_enemy() {
        enemy_list.erase(std::remove_if(enemy_list.begin(), enemy_list.end(),
            [](const Enemy* enemy) {
                bool deletable = enemy->can_remove();
                if (deletable) delete enemy;  // 释放内存
                return deletable;
            }), enemy_list.end());
    }

    /**
     * @brief 概率生成金币道具
     * @param position 生成位置
     * @param ratio 生成概率(0~1)
     */
    void try_spawn_coin_prop(const Vector2& position, double ratio) {
        static CoinManager* instance = CoinManager::instance();

        if ((double)(rand() % 100) / 100 <= ratio)
            instance->spawn_coin_prop(position);
    }
};

#endif // !_ENEMY_MANAGER_H_