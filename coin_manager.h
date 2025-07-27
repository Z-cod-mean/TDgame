#ifndef _COIN_MANAGER_H_
#define _COIN_MANAGER_H_

#include "coin_prop.h"      // 金币道具类
#include "manager.h"        // 管理器基类
#include "config_manager.h" // 配置管理器

#include <vector>           // 向量容器

/**
 * @class CoinManager
 * @brief 金币管理器类，负责游戏中金币系统的管理
 *
 * 该类使用单例模式实现，全局唯一。主要功能包括：
 * 1. 管理玩家拥有的金币数量
 * 2. 管理地图上掉落的金币道具
 * 3. 处理金币的增减逻辑
 * 4. 更新和渲染金币道具
 */
class CoinManager : public Manager<CoinManager>
{
    friend class Manager<CoinManager>; // 允许基类访问私有构造函数

public:
    typedef std::vector<CoinProp*> CoinPropList; ///< 金币道具列表类型

public:
    /**
     * @brief 增加玩家金币数量
     * @param val 增加的金币数量
     */
    void increase_coin(double val)
    {
        num_coin += val; // 直接增加金币数量
    }

    /**
     * @brief 减少玩家金币数量
     * @param val 减少的金币数量
     *
     * 如果减少后金币数量小于0，则重置为0
     */
    void decrease_coin(double val)
    {
        num_coin -= val; // 减少金币数量

        // 防止金币数量变为负数
        if (num_coin < 0)
            num_coin = 0;
    }

    /**
     * @brief 每帧更新金币道具状态
     * @param delta 帧时间增量（秒）
     *
     * 1. 遍历所有金币道具并更新状态
     * 2. 清理标记为可移除的道具
     */
    void on_update(double delta)
    {
        // 更新所有金币道具
        for (CoinProp* coin_prop : coin_prop_list)
            coin_prop->on_update(delta);

        // 清理可移除的金币道具（使用remove-erase惯用法）
        coin_prop_list.erase(std::remove_if(coin_prop_list.begin(), coin_prop_list.end(),
            [](CoinProp* coin_prop) {
                bool deletable = coin_prop->can_remove(); // 检查是否可移除
                if (deletable) delete coin_prop;          // 释放内存
                return deletable;
            }),
            coin_prop_list.end());
    }

    /**
     * @brief 渲染所有金币道具
     * @param renderer SDL渲染器指针
     */
    void on_render(SDL_Renderer* renderer)
    {
        for (CoinProp* coin_prop : coin_prop_list)
            coin_prop->on_render(renderer);
    }

    /**
     * @brief 获取当前玩家金币数量
     * @return 玩家当前拥有的金币数量
     */
    double get_current_coin_num()
    {
        return num_coin;
    }

    /**
     * @brief 获取金币道具列表引用
     * @return 当前所有活跃金币道具的列表
     */
    CoinPropList& get_coin_prop_list()
    {
        return coin_prop_list;
    }

    /**
     * @brief 生成金币道具
     * @param position 金币生成位置（二维向量）
     *
     * 在指定位置创建一个新的金币道具，并添加到管理列表
     */
    void spawn_coin_prop(const Vector2& position)
    {
        CoinProp* coin_prop = new CoinProp(); // 创建新金币道具
        coin_prop->set_position(position);    // 设置位置

        // 添加到金币道具列表
        coin_prop_list.push_back(coin_prop);
    }

protected:
    /**
     * @brief 构造函数：初始化金币数量
     *
     * 从配置管理器获取初始金币数量
     */
    CoinManager()
    {
        // 从配置获取初始金币数量
        num_coin = ConfigManager::instance()->num_initial_coin;
    }

    /**
     * @brief 析构函数：清理所有金币道具
     */
    ~CoinManager()
    {
        // 释放所有金币道具内存
        for (CoinProp* coin_prop : coin_prop_list)
            delete coin_prop;
    }

private:
    double num_coin = 0;       ///< 玩家当前拥有的金币数量
    CoinPropList coin_prop_list; ///< 存储所有活跃金币道具的列表
};

#endif // !_COIN_MANAGER_H_
