#ifndef _TOWER_MANAGER_H_
#define _TOWER_MANAGER_H_

#include "tower.h"             // 基础塔类
#include "tower_type.h"        // 塔类型枚举
#include "manager.h"           // 管理器基类
#include "archer_tower.h"      // 弓箭手塔
#include "axeman_tower.h"      // 斧兵塔
#include "gunner_tower.h"      // 枪手塔
#include "config_manager.h"    // 配置管理器
#include "resources_manager.h" // 资源管理器

#include <vector>              // 向量容器

/**
 * @class TowerManager
 * @brief 防御塔管理器类，负责游戏中所有防御塔的创建、升级和管理
 *
 * 该类使用单例模式实现，全局唯一。主要功能包括：
 * 1. 管理所有防御塔的更新和渲染
 * 2. 计算防御塔的建造和升级成本
 * 3. 获取防御塔的攻击范围
 * 4. 放置新防御塔
 * 5. 升级现有防御塔
 */
class TowerManager : public Manager<TowerManager>
{
    friend class Manager<TowerManager>; // 允许基类访问私有构造函数

public:
    /**
     * @brief 每帧更新所有防御塔状态
     * @param delta 帧时间增量（秒）
     */
    void on_update(double delta)
    {
        // 遍历更新所有防御塔
        for (Tower* tower : tower_list)
            tower->on_update(delta);
    }

    /**
     * @brief 渲染所有防御塔
     * @param renderer SDL渲染器指针
     */
    void on_render(SDL_Renderer* renderer)
    {
        // 遍历渲染所有防御塔
        for (Tower* tower : tower_list)
            tower->on_render(renderer);
    }

    /**
     * @brief 获取建造防御塔的成本
     * @param type 防御塔类型
     * @return 建造所需金币数量
     *
     * 根据塔类型和当前等级从配置获取成本
     */
    double get_place_cost(TowerType type)
    {
        static ConfigManager* instance = ConfigManager::instance();

        switch (type)
        {
        case Archer: // 弓箭手塔
            return instance->archer_template.cost[instance->level_archer];
        case Axeman: // 斧兵塔
            return instance->axeman_template.cost[instance->level_axeman];
        case Gunner: // 枪手塔
            return instance->gunner_template.cost[instance->level_gunner];
        default:
            return 0; // 未知类型返回0
        }
    }

    /**
     * @brief 获取升级防御塔的成本
     * @param type 防御塔类型
     * @return 升级所需金币数量（满级返回-1）
     *
     * 1. 检查防御塔是否已达最高等级（9级）
     * 2. 返回当前等级的升级成本
     */
    double get_upgrade_cost(TowerType type)
    {
        static ConfigManager* instance = ConfigManager::instance();

        switch (type)
        {
        case Archer: // 弓箭手塔
            return instance->level_archer == 9 ? -1 :
                instance->archer_template.upgrade_cost[instance->level_archer];
        case Axeman: // 斧兵塔
            return instance->level_axeman == 9 ? -1 :
                instance->axeman_template.upgrade_cost[instance->level_axeman];
        case Gunner: // 枪手塔
            return instance->level_gunner == 9 ? -1 :
                instance->gunner_template.upgrade_cost[instance->level_gunner];
        default:
            return 0; // 未知类型返回0
        }
    }

    /**
     * @brief 获取防御塔的攻击范围
     * @param type 防御塔类型
     * @return 攻击范围（单位：地图格子）
     *
     * 根据塔类型和当前等级从配置获取范围
     */
    double get_damage_range(TowerType type)
    {
        static ConfigManager* instance = ConfigManager::instance();

        switch (type)
        {
        case Archer: // 弓箭手塔
            return instance->archer_template.view_range[instance->level_archer];
        case Axeman: // 斧兵塔
            return instance->axeman_template.view_range[instance->level_axeman];
        case Gunner: // 枪手塔
            return instance->gunner_template.view_range[instance->level_gunner];
        default:
            return 0; // 未知类型返回0
        }
    }

    /**
     * @brief 在指定位置放置防御塔
     * @param type 防御塔类型
     * @param idx 地图格子索引位置
     *
     * 1. 创建指定类型的防御塔
     * 2. 计算实际像素位置
     * 3. 添加到防御塔列表
     * 4. 更新地图状态
     * 5. 播放放置音效
     */
    void place_tower(TowerType type, const SDL_Point& idx)
    {
        Tower* tower = nullptr; // 新防御塔指针

        // 根据类型创建具体防御塔对象
        switch (type)
        {
        case Archer: // 弓箭手塔
            tower = new ArcherTower();
            break;
        case Axeman: // 斧兵塔
            tower = new AxemanTower();
            break;
        case Gunner: // 枪手塔
            tower = new GunnerTower();
            break;
        default:     // 默认创建弓箭手塔
            tower = new ArcherTower();
            break;
        }

        // 计算防御塔在屏幕上的像素位置
        static Vector2 position;
        static const SDL_Rect& rect = ConfigManager::instance()->rect_tile_map;

        // 格子中心点位置 = 地图起点 + 格子坐标 * 格子尺寸 + 半个格子偏移
        position.x = rect.x + idx.x * SIZE_TILE + SIZE_TILE / 2;
        position.y = rect.y + idx.y * SIZE_TILE + SIZE_TILE / 2;

        // 设置防御塔位置
        tower->set_position(position);

        // 添加到防御塔列表
        tower_list.push_back(tower);

        // 更新地图状态（标记该格子已有防御塔）
        ConfigManager::instance()->map.place_tower(idx);

        // 播放放置音效
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_PlaceTower)->second, 0);
    }

    /**
     * @brief 升级指定类型的防御塔
     * @param type 防御塔类型
     *
     * 1. 检查当前等级是否已达上限（9级）
     * 2. 提升对应防御塔类型的等级
     * 3. 播放升级音效
     *
     * 注意：这是全局升级，影响所有同类型防御塔
     */
    void upgrade_tower(TowerType type)
    {
        static ConfigManager* instance = ConfigManager::instance();

        // 根据类型升级对应防御塔
        switch (type)
        {
        case Archer: // 弓箭手塔
            instance->level_archer = (instance->level_archer >= 9) ?
                9 : instance->level_archer + 1;
            break;
        case Axeman: // 斧兵塔
            instance->level_axeman = (instance->level_axeman >= 9) ?
                9 : instance->level_axeman + 1;
            break;
        case Gunner: // 枪手塔
            instance->level_gunner = (instance->level_gunner >= 9) ?
                9 : instance->level_gunner + 1;
            break;
        }

        // 播放升级音效
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_TowerLevelUp)->second, 0);
    }

protected:
    /// 保护构造函数（单例模式）
    TowerManager() = default;

    /// 析构函数：清理所有防御塔
    ~TowerManager()
    {
        for (Tower* tower : tower_list)
            delete tower;
    }

private:
    std::vector<Tower*> tower_list; ///< 存储所有防御塔的列表
};

#endif // !_TOWER_MANAGER_H_