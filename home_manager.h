#ifndef _HOME_MANAGER_H_
#define _HOME_MANAGER_H_

#include "manager.h"         // 管理器基类
#include "config_manager.h"  // 配置管理器
#include "resources_manager.h" // 资源管理器

/**
 * @class HomeManager
 * @brief 基地管理器类，负责管理玩家基地的生命值状态
 *
 * 该类使用单例模式实现，全局唯一。主要功能包括：
 * 1. 管理基地的当前生命值
 * 2. 处理基地受到的伤害
 * 3. 播放基地受伤音效
 * 4. 提供生命值查询接口
 */
class HomeManager : public Manager<HomeManager>
{
    friend class Manager<HomeManager>; // 允许基类访问私有构造函数

public:
    /**
     * @brief 获取基地当前生命值
     * @return 基地当前的生命值
     */
    double get_current_hp_num()
    {
        return num_hp;
    }

    /**
     * @brief 减少基地生命值
     * @param val 减少的生命值量
     *
     * 1. 减少基地生命值
     * 2. 确保生命值不低于0
     * 3. 播放基地受伤音效
     */
    void decrease_hp(double val)
    {
        // 减少生命值
        num_hp -= val;

        // 确保生命值不低于0
        if (num_hp < 0)
            num_hp = 0;

        // 获取音效资源池
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();

        // 播放基地受伤音效
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_HomeHurt)->second, 0);
    }

protected:
    /**
     * @brief 构造函数：初始化基地生命值
     *
     * 从配置管理器获取初始生命值
     */
    HomeManager()
    {
        // 从配置获取初始生命值
        num_hp = ConfigManager::instance()->num_initial_hp;
    }

    /// 默认析构函数
    ~HomeManager() = default;

private:
    double num_hp = 0; ///< 基地当前的生命值
};

#endif // !_HOME_MANAGER_H_
