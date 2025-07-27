#ifndef _CONFIG_MANAGER_H_
#define _CONFIG_MANAGER_H_

// 引入必要的头文件
#include "map.h"         // 游戏地图类
#include "wave.h"        // 敌人生成波次类
#include "manager.h"     // 单例基类

#include <SDL.h>         // SDL库用于窗口和渲染
#include <string>        // 字符串处理
#include <cJSON.h>       // cJSON轻量级JSON解析库
#include <fstream>       // 文件输入输出流
#include <sstream>       // 字符串流
#include <iostream>      // 标准输入输出

// 配置管理器类（采用单例模式）
// 职责：
// 1. 加载和解析游戏配置文件（JSON格式）
// 2. 集中管理所有游戏配置数据
// 3. 提供全局访问接口
// 设计特点：
// - 继承自Manager模板类实现单例
// - 使用结构体分类存储配置数据
// - 支持多级配置（如防御塔的各级属性）
// - 完善的错误检查和默认值机制
class ConfigManager : public Manager<ConfigManager>
{
    friend class Manager<ConfigManager>;  // 允许Manager模板类访问私有成员

public:
    // 基础配置模板结构体
    // 包含游戏窗口和系统相关的基础设置
    struct BasicTemplate {
        std::string window_title = u8"村庄保卫战！";  // UTF-8编码的窗口标题
        int window_width = 1280;      // 窗口默认宽度（像素）
        int window_height = 720;      // 窗口默认高度（像素）
        // 注意：窗口尺寸应该与游戏设计分辨率匹配
    };

    // 玩家角色属性模板
    // 控制玩家角色的各种行为参数
    struct PlayerTemplate {
        double speed = 3;            // 移动速度（单位：格子/秒）
        double normal_attack_interval = 0.5;  // 普通攻击冷却时间（秒）
        double normal_attack_damage = 0;     // 普通攻击基础伤害值
        double skill_interval = 10;          // 技能冷却时间（秒）
        double skill_damage = 1;             // 技能基础伤害值
        // 注意：伤害值可能需要根据游戏平衡性调整
    };

    // 防御塔属性模板
    // 使用数组存储各级属性，支持升级系统
    struct TowerTemplate {
        // 数组索引对应防御塔等级（0-9）
        double interval[10] = { 1 };    // 攻击间隔（秒）
        double damage[10] = { 25 };     // 攻击伤害值
        double view_range[10] = { 5 };  // 视野范围（格子数）
        double cost[10] = { 50 };       // 建造费用（金币）
        double upgrade_cost[9] = { 75 }; // 升级费用（索引0表示0级升1级）
        // 设计说明：
        // - 升级费用数组比属性数组少1个元素
        // - 可以通过调整这些数值实现不同的成长曲线
    };

    // 敌人属性模板
    // 定义各类敌人的基础属性
    struct EnemyTemplate {
        double hp = 100;           // 基础生命值
        double speed = 1;          // 移动速度（格子/秒）
        double damage = 1;         // 攻击伤害值
        double reward_ratio = 0.5; // 击杀奖励系数（影响金币掉落）
        double recover_interval = 10;  // 生命恢复间隔（秒）
        double recover_range = 0;  // 恢复范围（0表示仅自身）
        double recover_intensity = 25; // 每次恢复的生命值
        // 平衡性说明：
        // - 高血量的敌人应该有较低的reward_ratio
        // - 快速敌人应该有较低的生命值
    };

public:
    // 游戏地图数据
    // 包含地形、路径等信息
    Map map;

    // 敌人波次配置列表
    // 每个Wave对象描述一波敌人的生成规则
    std::vector<Wave> wave_list;

    // 防御塔当前等级
    // 用于追踪玩家建造的防御塔等级
    int level_archer = 0;   // 弓箭手塔当前等级（0-9）
    int level_axeman = 0;   // 斧手塔当前等级
    int level_gunner = 0;   // 枪手塔当前等级

    // 游戏状态标志
    bool is_game_win = true;    // 游戏胜利状态
    bool is_game_over = false;  // 游戏结束标志
    SDL_Rect rect_tile_map = { 0 };  // 地图区域矩形（像素坐标）
    // 注意：rect_tile_map用于碰撞检测和渲染边界判断

    // 配置模板实例
    BasicTemplate basic_template;      // 基础配置
    PlayerTemplate player_template;    // 玩家角色配置
    // 三种防御塔配置
    TowerTemplate archer_template;     // 弓箭手塔属性
    TowerTemplate axeman_template;     // 斧手塔属性
    TowerTemplate gunner_template;     // 枪手塔属性
    // 五种敌人配置
    EnemyTemplate slim_template;       // 普通史莱姆
    EnemyTemplate king_slim_template;  // 史莱姆王（精英）
    EnemyTemplate skeleton_template;   // 骷髅兵
    EnemyTemplate goblin_template;     // 哥布林
    EnemyTemplate goblin_priest_template; // 哥布林祭司（治疗者）

    // 游戏常量（不可配置）
    const double num_initial_hp = 10;     // 玩家初始生命值
    const double num_initial_coin = 100;  // 初始金币数量
    const double num_coin_per_prop = 10;  // 每个道具奖励金币数
    // 设计考虑：
    // 这些值设为常量是因为它们属于核心规则，不应通过配置修改

public:
    // 加载关卡配置文件
    // 参数：
    //   path - 配置文件的路径（相对或绝对路径）
    // 返回值：
    //   bool - 成功返回true，失败返回false
    // 功能：
    //   解析JSON格式的关卡配置，初始化wave_list
    // 实现细节：
    // 1. 使用ifstream读取文件内容
    // 2. 使用cJSON解析JSON数据
    // 3. 验证JSON结构并提取数据
    // 4. 错误处理包括：
    //    - 文件不存在或无法读取
    //    - JSON解析错误
    //    - 数据结构不符合预期
    bool load_level_config(const std::string& path) {
        // 打开文件流
        std::ifstream file(path);
        if (!file.good()) {
            std::cerr << "Error: Failed to open level config file: " << path << std::endl;
            return false;
        }

        // 读取整个文件内容到字符串流
        std::stringstream str_stream;
        str_stream << file.rdbuf();
        file.close();

        // 解析JSON数据
        cJSON* json_root = cJSON_Parse(str_stream.str().c_str());
        if (!json_root) {
            std::cerr << "Error: Failed to parse JSON in level config" << std::endl;
            return false;
        }

        // 验证根节点是否为数组（每个元素代表一波敌人）
        if (json_root->type != cJSON_Array) {
            std::cerr << "Error: Level config root should be an array" << std::endl;
            cJSON_Delete(json_root);
            return false;
        }

        // 遍历所有波次配置
        cJSON* json_wave = nullptr;
        cJSON_ArrayForEach(json_wave, json_root) {
            // 跳过非对象元素
            if (json_wave->type != cJSON_Object)
                continue;

            // 添加新波次到列表（使用emplace_back避免拷贝）
            wave_list.emplace_back();
            Wave& wave = wave_list.back();

            // 解析波次奖励金币数
            cJSON* json_wave_rewards = cJSON_GetObjectItem(json_wave, "rewards");
            if (json_wave_rewards && json_wave_rewards->type == cJSON_Number)
                wave.rawards = json_wave_rewards->valuedouble;

            // 解析波次间隔时间（与前一波的间隔）
            cJSON* json_wave_interval = cJSON_GetObjectItem(json_wave, "interval");
            if (json_wave_interval && json_wave_interval->type == cJSON_Number)
                wave.interval = json_wave_interval->valuedouble;

            // 解析生成事件列表（该波次中的敌人生成规则）
            cJSON* json_wave_spawn_list = cJSON_GetObjectItem(json_wave, "spawn_list");
            if (json_wave_spawn_list && json_wave_spawn_list->type == cJSON_Array) {
                cJSON* json_spawn_event = nullptr;
                cJSON_ArrayForEach(json_spawn_event, json_wave_spawn_list) {
                    // 跳过非对象元素
                    if (json_spawn_event->type != cJSON_Object)
                        continue;

                    // 添加生成事件
                    wave.spawn_event_list.emplace_back();
                    Wave::SpawnEvent& spawn_event = wave.spawn_event_list.back();

                    // 解析生成间隔（秒）
                    cJSON* json_spawn_event_interval = cJSON_GetObjectItem(json_spawn_event, "interval");
                    if (json_spawn_event_interval && json_spawn_event_interval->type == cJSON_Number)
                        spawn_event.interval = json_spawn_event_interval->valuedouble;

                    // 解析生成点索引（对应地图上的生成位置）
                    cJSON* json_spawn_event_spawn_point = cJSON_GetObjectItem(json_spawn_event, "point");
                    if (json_spawn_event_spawn_point && json_spawn_event_spawn_point->type == cJSON_Number)
                        spawn_event.spawn_point = json_spawn_event_spawn_point->valueint;

                    // 解析敌人类型（字符串枚举）
                    cJSON* json_spawn_event_enemy_type = cJSON_GetObjectItem(json_spawn_event, "enemy");
                    if (json_spawn_event_enemy_type && json_spawn_event_enemy_type->type == cJSON_String) {
                        const std::string str_enemy_type = json_spawn_event_enemy_type->valuestring;
                        // 将字符串映射到 EnemyType 枚举
                        if (str_enemy_type == "Slim")
                            spawn_event.enemy_type = EnemyType::Slim;
                        else if (str_enemy_type == "KingSlim")
                            spawn_event.enemy_type = EnemyType::KingSlim;
                        else if (str_enemy_type == "Skeleton")
                            spawn_event.enemy_type = EnemyType::Skeleton;
                        else if (str_enemy_type == "Goblin")
                            spawn_event.enemy_type = EnemyType::Goblin;
                        else if (str_enemy_type == "GoblinPriest")
                            spawn_event.enemy_type = EnemyType::GoblinPriest;
                        else
                            std::cerr << "Warning: Unknown enemy type: " << str_enemy_type << std::endl;
                    }
                }

                // 如果该波次没有有效的生成事件，则移除该波次
                if (wave.spawn_event_list.empty()) {
                    wave_list.pop_back();
                    std::cerr << "Warning: Removed empty wave" << std::endl;
                }
            }
        }

        // 释放JSON内存
        cJSON_Delete(json_root);

        // 验证至少有一个有效波次
        if (wave_list.empty()) {
            std::cerr << "Error: No valid waves in level config" << std::endl;
            return false;
        }

        return true;
    }

    // 加载游戏全局配置文件
    // 参数：
    //   path - 配置文件的路径
    // 返回值：
    //   bool - 成功返回true，失败返回false
    // 功能：
    //   解析JSON格式的游戏配置，初始化各种模板
    // 实现细节：
    // 1. 读取并解析JSON文件
    // 2. 验证基本结构
    // 3. 分发到各个专门的解析方法
    bool load_game_config(const std::string& path) {
        std::ifstream file(path);
        if (!file.good()) {
            std::cerr << "Error: Failed to open game config file: " << path << std::endl;
            return false;
        }

        // 读取文件内容
        std::stringstream str_stream;
        str_stream << file.rdbuf();
        file.close();

        // 解析JSON
        cJSON* json_root = cJSON_Parse(str_stream.str().c_str());
        if (!json_root) {
            std::cerr << "Error: Failed to parse JSON in game config" << std::endl;
            return false;
        }

        // 验证根节点是否为对象
        if (json_root->type != cJSON_Object) {
            std::cerr << "Error: Game config root should be an object" << std::endl;
            cJSON_Delete(json_root);
            return false;
        }

        // 获取各配置节点
        cJSON* json_basic = cJSON_GetObjectItem(json_root, "basic");
        cJSON* json_player = cJSON_GetObjectItem(json_root, "player");
        cJSON* json_tower = cJSON_GetObjectItem(json_root, "tower");
        cJSON* json_enemy = cJSON_GetObjectItem(json_root, "enemy");

        // 验证节点存在性和类型
        if (!json_basic || !json_player || !json_tower || !json_enemy ||
            json_basic->type != cJSON_Object ||
            json_player->type != cJSON_Object ||
            json_tower->type != cJSON_Object ||
            json_enemy->type != cJSON_Object) {
            std::cerr << "Error: Invalid game config structure" << std::endl;
            cJSON_Delete(json_root);
            return false;
        }

        // 分发到各解析方法
        parse_basic_template(basic_template, json_basic);
        parse_player_template(player_template, json_player);

        // 解析三种防御塔配置
        parse_tower_template(archer_template, cJSON_GetObjectItem(json_tower, "archer"));
        parse_tower_template(axeman_template, cJSON_GetObjectItem(json_tower, "axeman"));
        parse_tower_template(gunner_template, cJSON_GetObjectItem(json_tower, "gunner"));

        // 解析五种敌人配置
        parse_enemy_template(slim_template, cJSON_GetObjectItem(json_enemy, "slim"));
        parse_enemy_template(king_slim_template, cJSON_GetObjectItem(json_enemy, "king_slim"));
        parse_enemy_template(skeleton_template, cJSON_GetObjectItem(json_enemy, "skeleton"));
        parse_enemy_template(goblin_template, cJSON_GetObjectItem(json_enemy, "goblin"));
        parse_enemy_template(goblin_priest_template, cJSON_GetObjectItem(json_enemy, "goblin_priest"));

        // 释放JSON内存
        cJSON_Delete(json_root);
        return true;
    }

protected:
    // 构造函数（protected保证只能通过instance()获取单例）
    ConfigManager() = default;
    // 析构函数
    ~ConfigManager() = default;

private:
    // 解析基础配置
    // 参数：
    //   tpl - 要填充的BasicTemplate结构体
    //   json_root - 对应的JSON节点
    void parse_basic_template(BasicTemplate& tpl, cJSON* json_root) {
        if (!json_root || json_root->type != cJSON_Object) {
            std::cerr << "Warning: Invalid basic config node" << std::endl;
            return;
        }

        // 解析窗口标题
        cJSON* json_window_title = cJSON_GetObjectItem(json_root, "window_title");
        if (json_window_title && json_window_title->type == cJSON_String)
            tpl.window_title = json_window_title->valuestring;

        // 解析窗口宽度
        cJSON* json_window_width = cJSON_GetObjectItem(json_root, "window_width");
        if (json_window_width && json_window_width->type == cJSON_Number)
            tpl.window_width = json_window_width->valueint;

        // 解析窗口高度
        cJSON* json_window_height = cJSON_GetObjectItem(json_root, "window_height");
        if (json_window_height && json_window_height->type == cJSON_Number)
            tpl.window_height = json_window_height->valueint;
    }

    // 解析玩家配置
    void parse_player_template(PlayerTemplate& tpl, cJSON* json_root) {
        if (!json_root || json_root->type != cJSON_Object) {
            std::cerr << "Warning: Invalid player config node" << std::endl;
            return;
        }

        // 解析移动速度
        cJSON* json_speed = cJSON_GetObjectItem(json_root, "speed");
        if (json_speed && json_speed->type == cJSON_Number)
            tpl.speed = json_speed->valuedouble;

        // 解析普通攻击间隔
        cJSON* json_normal_attack_interval = cJSON_GetObjectItem(json_root, "normal_attack_interval");
        if (json_normal_attack_interval && json_normal_attack_interval->type == cJSON_Number)
            tpl.normal_attack_interval = json_normal_attack_interval->valuedouble;

        // 解析普通攻击伤害
        cJSON* json_normal_attack_damage = cJSON_GetObjectItem(json_root, "normal_attack_damage");
        if (json_normal_attack_damage && json_normal_attack_damage->type == cJSON_Number)
            tpl.normal_attack_damage = json_normal_attack_damage->valuedouble;

        // 解析技能冷却时间
        cJSON* json_skill_interval = cJSON_GetObjectItem(json_root, "skill_interval");
        if (json_skill_interval && json_skill_interval->type == cJSON_Number)
            tpl.skill_interval = json_skill_interval->valuedouble;

        // 解析技能伤害
        cJSON* json_skill_damage = cJSON_GetObjectItem(json_root, "skill_damage");
        if (json_skill_damage && json_skill_damage->type == cJSON_Number)
            tpl.skill_damage = json_skill_damage->valuedouble;
    }

    // 通用方法：解析数值数组
    // 参数：
    //   ary - 目标数组指针
    //   max_len - 数组最大长度
    //   json_root - JSON数组节点
    void parse_number_array(double* ary, int max_len, cJSON* json_root) {
        if (!json_root || json_root->type != cJSON_Array) {
            std::cerr << "Warning: Expected number array" << std::endl;
            return;
        }

        int idx = -1;
        cJSON* json_element = nullptr;
        // 遍历JSON数组
        cJSON_ArrayForEach(json_element, json_root) {
            idx++;
            // 跳过非数值元素或超出数组长度的元素
            if (json_element->type != cJSON_Number || idx >= max_len)
                continue;

            // 赋值到数组
            ary[idx] = json_element->valuedouble;
        }
    }

    // 解析防御塔配置
    void parse_tower_template(TowerTemplate& tpl, cJSON* json_root) {
        if (!json_root || json_root->type != cJSON_Object) {
            std::cerr << "Warning: Invalid tower config node" << std::endl;
            return;
        }

        // 解析攻击间隔数组（各级）
        cJSON* json_interval = cJSON_GetObjectItem(json_root, "interval");
        // 解析伤害值数组（各级）
        cJSON* json_damage = cJSON_GetObjectItem(json_root, "damage");
        // 解析视野范围数组（各级）
        cJSON* json_view_range = cJSON_GetObjectItem(json_root, "view_range");
        // 解析建造费用数组（各级）
        cJSON* json_cost = cJSON_GetObjectItem(json_root, "cost");
        // 解析升级费用数组（各级）
        cJSON* json_upgrade_cost = cJSON_GetObjectItem(json_root, "upgrade_cost");

        // 使用通用方法解析各数组
        parse_number_array(tpl.interval, 10, json_interval);
        parse_number_array(tpl.damage, 10, json_damage);
        parse_number_array(tpl.view_range, 10, json_view_range);
        parse_number_array(tpl.cost, 10, json_cost);
        parse_number_array(tpl.upgrade_cost, 9, json_upgrade_cost);
    }

    // 解析敌人配置
    void parse_enemy_template(EnemyTemplate& tpl, cJSON* json_root) {
        if (!json_root || json_root->type != cJSON_Object) {
            std::cerr << "Warning: Invalid enemy config node" << std::endl;
            return;
        }

        // 解析生命值
        cJSON* json_hp = cJSON_GetObjectItem(json_root, "hp");
        if (json_hp && json_hp->type == cJSON_Number)
            tpl.hp = json_hp->valuedouble;

        // 解析移动速度
        cJSON* json_speed = cJSON_GetObjectItem(json_root, "speed");
        if (json_speed && json_speed->type == cJSON_Number)
            tpl.speed = json_speed->valuedouble;

        // 解析攻击伤害
        cJSON* json_damage = cJSON_GetObjectItem(json_root, "damage");
        if (json_damage && json_damage->type == cJSON_Number)
            tpl.damage = json_damage->valuedouble;

        // 解析奖励系数
        cJSON* json_reward_ratio = cJSON_GetObjectItem(json_root, "reward_ratio");
        if (json_reward_ratio && json_reward_ratio->type == cJSON_Number)
            tpl.reward_ratio = json_reward_ratio->valuedouble;

        // 解析恢复间隔
        cJSON* json_recover_interval = cJSON_GetObjectItem(json_root, "recover_interval");
        if (json_recover_interval && json_recover_interval->type == cJSON_Number)
            tpl.recover_interval = json_recover_interval->valuedouble;

        // 解析恢复范围
        cJSON* json_recover_range = cJSON_GetObjectItem(json_root, "recover_range");
        if (json_recover_range && json_recover_range->type == cJSON_Number)
            tpl.recover_range = json_recover_range->valuedouble;

        // 解析恢复强度
        cJSON* json_recover_intensity = cJSON_GetObjectItem(json_root, "recover_intensity");
        if (json_recover_intensity && json_recover_intensity->type == cJSON_Number)
            tpl.recover_intensity = json_recover_intensity->valuedouble;
    }
};

#endif // !_CONFIG_MANAGER_H_
