#ifndef _WAVE_MANAGER_H_
#define _WAVE_MANAGER_H_

// 依赖的头文件
#include "timer.h"           // 计时器工具
#include "manager.h"         // 单例基类
#include "config_manager.h"  // 配置数据
#include "enemy_manager.h"   // 敌人生成
#include "coin_manager.h"    // 金币奖励

/**
 * @class WaveManager
 * @brief 波浪敌人生成管理系统（单例模式）
 *
 * 核心职责：
 * 1. 按配置节奏生成敌人波次
 * 2. 控制波次间隔时间
 * 3. 处理波次完成奖励
 * 4. 触发游戏胜利条件
 */
class WaveManager : public Manager<WaveManager>
{
    friend class Manager<WaveManager>;  // 允许单例模板访问私有构造

public:
    /**
     * @brief 每帧更新波次状态
     * @param delta 帧间隔时间（秒）
     *
     * 工作流程：
     * 1. 检查当前波次状态
     * 2. 更新计时器
     * 3. 触发敌人生成事件
     * 4. 处理波次完成逻辑
     */
    void on_update(double delta)
    {
        static ConfigManager* instance = ConfigManager::instance();

        if (instance->is_game_over)  // 游戏结束时停止更新
            return;

        // 更新当前活跃的计时器
        if (!is_wave_started)
            timer_start_wave.on_update(delta);  // 波次间隔计时
        else
            timer_spawn_enemy.on_update(delta); // 敌人生成间隔计时

        // 检查波次完成条件：
        // 1. 已生成最后一个敌人 
        // 2. 场景中无存活敌人
        if (is_spawned_last_enemy && EnemyManager::instance()->check_cleared())
        {
            // 发放波次奖励金币
            CoinManager::instance()->increase_coin(
                instance->wave_list[idx_wave].rawards);

            idx_wave++;  // 移至下一波

            // 胜利条件：所有波次完成
            if (idx_wave >= instance->wave_list.size())
            {
                instance->is_game_win = true;
                instance->is_game_over = true;
            }
            else  // 准备下一波
            {
                idx_spawn_event = 0;
                is_wave_started = true;
                is_spawned_last_enemy = false;

                // 配置下一波的间隔时间
                const Wave& wave = instance->wave_list[idx_wave];
                timer_start_wave.set_wait_time(wave.interval);
                timer_start_wave.restart();
            }
        }
    }

protected:
    /**
     * @brief 构造函数（初始化计时器回调）
     *
     * 初始化两个核心计时器：
     * 1. timer_start_wave - 控制波次开始前的等待
     * 2. timer_spawn_enemy - 控制敌人生成间隔
     */
    WaveManager()
    {
        static const std::vector<Wave>& wave_list =
            ConfigManager::instance()->wave_list;

        // 波次开始计时器配置（单次触发）
        timer_start_wave.set_one_shot(true);
        timer_start_wave.set_wait_time(wave_list[0].interval);
        timer_start_wave.set_on_timeout([&]() {
            is_wave_started = true;

            // 初始化第一个敌人生成事件
            timer_spawn_enemy.set_wait_time(
                wave_list[idx_wave].spawn_event_list[0].interval);
            timer_spawn_enemy.restart();
            });

        // 敌人生成计时器配置（链式触发）
        timer_spawn_enemy.set_one_shot(true);
        timer_spawn_enemy.set_on_timeout([&]() {
            const auto& spawn_event_list =
                wave_list[idx_wave].spawn_event_list;
            const auto& spawn_event = spawn_event_list[idx_spawn_event];

            // 生成指定类型的敌人
            EnemyManager::instance()->spawn_enemy(
                spawn_event.enemy_type,
                spawn_event.spawn_point);

            idx_spawn_event++;

            // 检查是否为最后一个生成事件
            if (idx_spawn_event >= spawn_event_list.size())
            {
                is_spawned_last_enemy = true;
                return;
            }

            // 设置下一个生成事件的间隔
            timer_spawn_enemy.set_wait_time(
                spawn_event_list[idx_spawn_event].interval);
            timer_spawn_enemy.restart();
            });
    }

    ~WaveManager() = default;  // 默认析构

private:
    // 当前波次索引（从0开始）
    int idx_wave = 0;

    // 当前波次内的敌人生成事件索引
    int idx_spawn_event = 0;

    // 波次开始前的等待计时器
    Timer timer_start_wave;

    // 敌人生成间隔计时器
    Timer timer_spawn_enemy;

    // 状态标志
    bool is_wave_started = false;         // 是否已开始当前波次
    bool is_spawned_last_enemy = false;   // 是否已生成当前波次最后一个敌人
};

#endif // !_WAVE_MANAGER_H_