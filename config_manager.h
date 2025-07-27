#ifndef _CONFIG_MANAGER_H_
#define _CONFIG_MANAGER_H_

// �����Ҫ��ͷ�ļ�
#include "map.h"         // ��Ϸ��ͼ��
#include "wave.h"        // �������ɲ�����
#include "manager.h"     // ��������

#include <SDL.h>         // SDL�����ڴ��ں���Ⱦ
#include <string>        // �ַ�������
#include <cJSON.h>       // cJSON������JSON������
#include <fstream>       // �ļ����������
#include <sstream>       // �ַ�����
#include <iostream>      // ��׼�������

// ���ù������ࣨ���õ���ģʽ��
// ְ��
// 1. ���غͽ�����Ϸ�����ļ���JSON��ʽ��
// 2. ���й���������Ϸ��������
// 3. �ṩȫ�ַ��ʽӿ�
// ����ص㣺
// - �̳���Managerģ����ʵ�ֵ���
// - ʹ�ýṹ�����洢��������
// - ֧�ֶ༶���ã���������ĸ������ԣ�
// - ���ƵĴ������Ĭ��ֵ����
class ConfigManager : public Manager<ConfigManager>
{
    friend class Manager<ConfigManager>;  // ����Managerģ�������˽�г�Ա

public:
    // ��������ģ��ṹ��
    // ������Ϸ���ں�ϵͳ��صĻ�������
    struct BasicTemplate {
        std::string window_title = u8"��ׯ����ս��";  // UTF-8����Ĵ��ڱ���
        int window_width = 1280;      // ����Ĭ�Ͽ�ȣ����أ�
        int window_height = 720;      // ����Ĭ�ϸ߶ȣ����أ�
        // ע�⣺���ڳߴ�Ӧ������Ϸ��Ʒֱ���ƥ��
    };

    // ��ҽ�ɫ����ģ��
    // ������ҽ�ɫ�ĸ�����Ϊ����
    struct PlayerTemplate {
        double speed = 3;            // �ƶ��ٶȣ���λ������/�룩
        double normal_attack_interval = 0.5;  // ��ͨ������ȴʱ�䣨�룩
        double normal_attack_damage = 0;     // ��ͨ���������˺�ֵ
        double skill_interval = 10;          // ������ȴʱ�䣨�룩
        double skill_damage = 1;             // ���ܻ����˺�ֵ
        // ע�⣺�˺�ֵ������Ҫ������Ϸƽ���Ե���
    };

    // ����������ģ��
    // ʹ������洢�������ԣ�֧������ϵͳ
    struct TowerTemplate {
        // ����������Ӧ�������ȼ���0-9��
        double interval[10] = { 1 };    // ����������룩
        double damage[10] = { 25 };     // �����˺�ֵ
        double view_range[10] = { 5 };  // ��Ұ��Χ����������
        double cost[10] = { 50 };       // ������ã���ң�
        double upgrade_cost[9] = { 75 }; // �������ã�����0��ʾ0����1����
        // ���˵����
        // - �����������������������1��Ԫ��
        // - ����ͨ��������Щ��ֵʵ�ֲ�ͬ�ĳɳ�����
    };

    // ��������ģ��
    // ���������˵Ļ�������
    struct EnemyTemplate {
        double hp = 100;           // ��������ֵ
        double speed = 1;          // �ƶ��ٶȣ�����/�룩
        double damage = 1;         // �����˺�ֵ
        double reward_ratio = 0.5; // ��ɱ����ϵ����Ӱ���ҵ��䣩
        double recover_interval = 10;  // �����ָ�������룩
        double recover_range = 0;  // �ָ���Χ��0��ʾ������
        double recover_intensity = 25; // ÿ�λָ�������ֵ
        // ƽ����˵����
        // - ��Ѫ���ĵ���Ӧ���нϵ͵�reward_ratio
        // - ���ٵ���Ӧ���нϵ͵�����ֵ
    };

public:
    // ��Ϸ��ͼ����
    // �������Ρ�·������Ϣ
    Map map;

    // ���˲��������б�
    // ÿ��Wave��������һ�����˵����ɹ���
    std::vector<Wave> wave_list;

    // ��������ǰ�ȼ�
    // ����׷����ҽ���ķ������ȼ�
    int level_archer = 0;   // ����������ǰ�ȼ���0-9��
    int level_axeman = 0;   // ��������ǰ�ȼ�
    int level_gunner = 0;   // ǹ������ǰ�ȼ�

    // ��Ϸ״̬��־
    bool is_game_win = true;    // ��Ϸʤ��״̬
    bool is_game_over = false;  // ��Ϸ������־
    SDL_Rect rect_tile_map = { 0 };  // ��ͼ������Σ��������꣩
    // ע�⣺rect_tile_map������ײ������Ⱦ�߽��ж�

    // ����ģ��ʵ��
    BasicTemplate basic_template;      // ��������
    PlayerTemplate player_template;    // ��ҽ�ɫ����
    // ���ַ���������
    TowerTemplate archer_template;     // ������������
    TowerTemplate axeman_template;     // ����������
    TowerTemplate gunner_template;     // ǹ��������
    // ���ֵ�������
    EnemyTemplate slim_template;       // ��ͨʷ��ķ
    EnemyTemplate king_slim_template;  // ʷ��ķ������Ӣ��
    EnemyTemplate skeleton_template;   // ���ñ�
    EnemyTemplate goblin_template;     // �粼��
    EnemyTemplate goblin_priest_template; // �粼�ּ�˾�������ߣ�

    // ��Ϸ�������������ã�
    const double num_initial_hp = 10;     // ��ҳ�ʼ����ֵ
    const double num_initial_coin = 100;  // ��ʼ�������
    const double num_coin_per_prop = 10;  // ÿ�����߽��������
    // ��ƿ��ǣ�
    // ��Щֵ��Ϊ��������Ϊ�������ں��Ĺ��򣬲�Ӧͨ�������޸�

public:
    // ���عؿ������ļ�
    // ������
    //   path - �����ļ���·������Ի����·����
    // ����ֵ��
    //   bool - �ɹ�����true��ʧ�ܷ���false
    // ���ܣ�
    //   ����JSON��ʽ�Ĺؿ����ã���ʼ��wave_list
    // ʵ��ϸ�ڣ�
    // 1. ʹ��ifstream��ȡ�ļ�����
    // 2. ʹ��cJSON����JSON����
    // 3. ��֤JSON�ṹ����ȡ����
    // 4. �����������
    //    - �ļ������ڻ��޷���ȡ
    //    - JSON��������
    //    - ���ݽṹ������Ԥ��
    bool load_level_config(const std::string& path) {
        // ���ļ���
        std::ifstream file(path);
        if (!file.good()) {
            std::cerr << "Error: Failed to open level config file: " << path << std::endl;
            return false;
        }

        // ��ȡ�����ļ����ݵ��ַ�����
        std::stringstream str_stream;
        str_stream << file.rdbuf();
        file.close();

        // ����JSON����
        cJSON* json_root = cJSON_Parse(str_stream.str().c_str());
        if (!json_root) {
            std::cerr << "Error: Failed to parse JSON in level config" << std::endl;
            return false;
        }

        // ��֤���ڵ��Ƿ�Ϊ���飨ÿ��Ԫ�ش���һ�����ˣ�
        if (json_root->type != cJSON_Array) {
            std::cerr << "Error: Level config root should be an array" << std::endl;
            cJSON_Delete(json_root);
            return false;
        }

        // �������в�������
        cJSON* json_wave = nullptr;
        cJSON_ArrayForEach(json_wave, json_root) {
            // �����Ƕ���Ԫ��
            if (json_wave->type != cJSON_Object)
                continue;

            // ����²��ε��б�ʹ��emplace_back���⿽����
            wave_list.emplace_back();
            Wave& wave = wave_list.back();

            // �������ν��������
            cJSON* json_wave_rewards = cJSON_GetObjectItem(json_wave, "rewards");
            if (json_wave_rewards && json_wave_rewards->type == cJSON_Number)
                wave.rawards = json_wave_rewards->valuedouble;

            // �������μ��ʱ�䣨��ǰһ���ļ����
            cJSON* json_wave_interval = cJSON_GetObjectItem(json_wave, "interval");
            if (json_wave_interval && json_wave_interval->type == cJSON_Number)
                wave.interval = json_wave_interval->valuedouble;

            // ���������¼��б��ò����еĵ������ɹ���
            cJSON* json_wave_spawn_list = cJSON_GetObjectItem(json_wave, "spawn_list");
            if (json_wave_spawn_list && json_wave_spawn_list->type == cJSON_Array) {
                cJSON* json_spawn_event = nullptr;
                cJSON_ArrayForEach(json_spawn_event, json_wave_spawn_list) {
                    // �����Ƕ���Ԫ��
                    if (json_spawn_event->type != cJSON_Object)
                        continue;

                    // ��������¼�
                    wave.spawn_event_list.emplace_back();
                    Wave::SpawnEvent& spawn_event = wave.spawn_event_list.back();

                    // �������ɼ�����룩
                    cJSON* json_spawn_event_interval = cJSON_GetObjectItem(json_spawn_event, "interval");
                    if (json_spawn_event_interval && json_spawn_event_interval->type == cJSON_Number)
                        spawn_event.interval = json_spawn_event_interval->valuedouble;

                    // �������ɵ���������Ӧ��ͼ�ϵ�����λ�ã�
                    cJSON* json_spawn_event_spawn_point = cJSON_GetObjectItem(json_spawn_event, "point");
                    if (json_spawn_event_spawn_point && json_spawn_event_spawn_point->type == cJSON_Number)
                        spawn_event.spawn_point = json_spawn_event_spawn_point->valueint;

                    // �����������ͣ��ַ���ö�٣�
                    cJSON* json_spawn_event_enemy_type = cJSON_GetObjectItem(json_spawn_event, "enemy");
                    if (json_spawn_event_enemy_type && json_spawn_event_enemy_type->type == cJSON_String) {
                        const std::string str_enemy_type = json_spawn_event_enemy_type->valuestring;
                        // ���ַ���ӳ�䵽 EnemyType ö��
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

                // ����ò���û����Ч�������¼������Ƴ��ò���
                if (wave.spawn_event_list.empty()) {
                    wave_list.pop_back();
                    std::cerr << "Warning: Removed empty wave" << std::endl;
                }
            }
        }

        // �ͷ�JSON�ڴ�
        cJSON_Delete(json_root);

        // ��֤������һ����Ч����
        if (wave_list.empty()) {
            std::cerr << "Error: No valid waves in level config" << std::endl;
            return false;
        }

        return true;
    }

    // ������Ϸȫ�������ļ�
    // ������
    //   path - �����ļ���·��
    // ����ֵ��
    //   bool - �ɹ�����true��ʧ�ܷ���false
    // ���ܣ�
    //   ����JSON��ʽ����Ϸ���ã���ʼ������ģ��
    // ʵ��ϸ�ڣ�
    // 1. ��ȡ������JSON�ļ�
    // 2. ��֤�����ṹ
    // 3. �ַ�������ר�ŵĽ�������
    bool load_game_config(const std::string& path) {
        std::ifstream file(path);
        if (!file.good()) {
            std::cerr << "Error: Failed to open game config file: " << path << std::endl;
            return false;
        }

        // ��ȡ�ļ�����
        std::stringstream str_stream;
        str_stream << file.rdbuf();
        file.close();

        // ����JSON
        cJSON* json_root = cJSON_Parse(str_stream.str().c_str());
        if (!json_root) {
            std::cerr << "Error: Failed to parse JSON in game config" << std::endl;
            return false;
        }

        // ��֤���ڵ��Ƿ�Ϊ����
        if (json_root->type != cJSON_Object) {
            std::cerr << "Error: Game config root should be an object" << std::endl;
            cJSON_Delete(json_root);
            return false;
        }

        // ��ȡ�����ýڵ�
        cJSON* json_basic = cJSON_GetObjectItem(json_root, "basic");
        cJSON* json_player = cJSON_GetObjectItem(json_root, "player");
        cJSON* json_tower = cJSON_GetObjectItem(json_root, "tower");
        cJSON* json_enemy = cJSON_GetObjectItem(json_root, "enemy");

        // ��֤�ڵ�����Ժ�����
        if (!json_basic || !json_player || !json_tower || !json_enemy ||
            json_basic->type != cJSON_Object ||
            json_player->type != cJSON_Object ||
            json_tower->type != cJSON_Object ||
            json_enemy->type != cJSON_Object) {
            std::cerr << "Error: Invalid game config structure" << std::endl;
            cJSON_Delete(json_root);
            return false;
        }

        // �ַ�������������
        parse_basic_template(basic_template, json_basic);
        parse_player_template(player_template, json_player);

        // �������ַ���������
        parse_tower_template(archer_template, cJSON_GetObjectItem(json_tower, "archer"));
        parse_tower_template(axeman_template, cJSON_GetObjectItem(json_tower, "axeman"));
        parse_tower_template(gunner_template, cJSON_GetObjectItem(json_tower, "gunner"));

        // �������ֵ�������
        parse_enemy_template(slim_template, cJSON_GetObjectItem(json_enemy, "slim"));
        parse_enemy_template(king_slim_template, cJSON_GetObjectItem(json_enemy, "king_slim"));
        parse_enemy_template(skeleton_template, cJSON_GetObjectItem(json_enemy, "skeleton"));
        parse_enemy_template(goblin_template, cJSON_GetObjectItem(json_enemy, "goblin"));
        parse_enemy_template(goblin_priest_template, cJSON_GetObjectItem(json_enemy, "goblin_priest"));

        // �ͷ�JSON�ڴ�
        cJSON_Delete(json_root);
        return true;
    }

protected:
    // ���캯����protected��ֻ֤��ͨ��instance()��ȡ������
    ConfigManager() = default;
    // ��������
    ~ConfigManager() = default;

private:
    // ������������
    // ������
    //   tpl - Ҫ����BasicTemplate�ṹ��
    //   json_root - ��Ӧ��JSON�ڵ�
    void parse_basic_template(BasicTemplate& tpl, cJSON* json_root) {
        if (!json_root || json_root->type != cJSON_Object) {
            std::cerr << "Warning: Invalid basic config node" << std::endl;
            return;
        }

        // �������ڱ���
        cJSON* json_window_title = cJSON_GetObjectItem(json_root, "window_title");
        if (json_window_title && json_window_title->type == cJSON_String)
            tpl.window_title = json_window_title->valuestring;

        // �������ڿ��
        cJSON* json_window_width = cJSON_GetObjectItem(json_root, "window_width");
        if (json_window_width && json_window_width->type == cJSON_Number)
            tpl.window_width = json_window_width->valueint;

        // �������ڸ߶�
        cJSON* json_window_height = cJSON_GetObjectItem(json_root, "window_height");
        if (json_window_height && json_window_height->type == cJSON_Number)
            tpl.window_height = json_window_height->valueint;
    }

    // �����������
    void parse_player_template(PlayerTemplate& tpl, cJSON* json_root) {
        if (!json_root || json_root->type != cJSON_Object) {
            std::cerr << "Warning: Invalid player config node" << std::endl;
            return;
        }

        // �����ƶ��ٶ�
        cJSON* json_speed = cJSON_GetObjectItem(json_root, "speed");
        if (json_speed && json_speed->type == cJSON_Number)
            tpl.speed = json_speed->valuedouble;

        // ������ͨ�������
        cJSON* json_normal_attack_interval = cJSON_GetObjectItem(json_root, "normal_attack_interval");
        if (json_normal_attack_interval && json_normal_attack_interval->type == cJSON_Number)
            tpl.normal_attack_interval = json_normal_attack_interval->valuedouble;

        // ������ͨ�����˺�
        cJSON* json_normal_attack_damage = cJSON_GetObjectItem(json_root, "normal_attack_damage");
        if (json_normal_attack_damage && json_normal_attack_damage->type == cJSON_Number)
            tpl.normal_attack_damage = json_normal_attack_damage->valuedouble;

        // ����������ȴʱ��
        cJSON* json_skill_interval = cJSON_GetObjectItem(json_root, "skill_interval");
        if (json_skill_interval && json_skill_interval->type == cJSON_Number)
            tpl.skill_interval = json_skill_interval->valuedouble;

        // ���������˺�
        cJSON* json_skill_damage = cJSON_GetObjectItem(json_root, "skill_damage");
        if (json_skill_damage && json_skill_damage->type == cJSON_Number)
            tpl.skill_damage = json_skill_damage->valuedouble;
    }

    // ͨ�÷�����������ֵ����
    // ������
    //   ary - Ŀ������ָ��
    //   max_len - ������󳤶�
    //   json_root - JSON����ڵ�
    void parse_number_array(double* ary, int max_len, cJSON* json_root) {
        if (!json_root || json_root->type != cJSON_Array) {
            std::cerr << "Warning: Expected number array" << std::endl;
            return;
        }

        int idx = -1;
        cJSON* json_element = nullptr;
        // ����JSON����
        cJSON_ArrayForEach(json_element, json_root) {
            idx++;
            // ��������ֵԪ�ػ򳬳����鳤�ȵ�Ԫ��
            if (json_element->type != cJSON_Number || idx >= max_len)
                continue;

            // ��ֵ������
            ary[idx] = json_element->valuedouble;
        }
    }

    // ��������������
    void parse_tower_template(TowerTemplate& tpl, cJSON* json_root) {
        if (!json_root || json_root->type != cJSON_Object) {
            std::cerr << "Warning: Invalid tower config node" << std::endl;
            return;
        }

        // ��������������飨������
        cJSON* json_interval = cJSON_GetObjectItem(json_root, "interval");
        // �����˺�ֵ���飨������
        cJSON* json_damage = cJSON_GetObjectItem(json_root, "damage");
        // ������Ұ��Χ���飨������
        cJSON* json_view_range = cJSON_GetObjectItem(json_root, "view_range");
        // ��������������飨������
        cJSON* json_cost = cJSON_GetObjectItem(json_root, "cost");
        // ���������������飨������
        cJSON* json_upgrade_cost = cJSON_GetObjectItem(json_root, "upgrade_cost");

        // ʹ��ͨ�÷�������������
        parse_number_array(tpl.interval, 10, json_interval);
        parse_number_array(tpl.damage, 10, json_damage);
        parse_number_array(tpl.view_range, 10, json_view_range);
        parse_number_array(tpl.cost, 10, json_cost);
        parse_number_array(tpl.upgrade_cost, 9, json_upgrade_cost);
    }

    // ������������
    void parse_enemy_template(EnemyTemplate& tpl, cJSON* json_root) {
        if (!json_root || json_root->type != cJSON_Object) {
            std::cerr << "Warning: Invalid enemy config node" << std::endl;
            return;
        }

        // ��������ֵ
        cJSON* json_hp = cJSON_GetObjectItem(json_root, "hp");
        if (json_hp && json_hp->type == cJSON_Number)
            tpl.hp = json_hp->valuedouble;

        // �����ƶ��ٶ�
        cJSON* json_speed = cJSON_GetObjectItem(json_root, "speed");
        if (json_speed && json_speed->type == cJSON_Number)
            tpl.speed = json_speed->valuedouble;

        // ���������˺�
        cJSON* json_damage = cJSON_GetObjectItem(json_root, "damage");
        if (json_damage && json_damage->type == cJSON_Number)
            tpl.damage = json_damage->valuedouble;

        // ��������ϵ��
        cJSON* json_reward_ratio = cJSON_GetObjectItem(json_root, "reward_ratio");
        if (json_reward_ratio && json_reward_ratio->type == cJSON_Number)
            tpl.reward_ratio = json_reward_ratio->valuedouble;

        // �����ָ����
        cJSON* json_recover_interval = cJSON_GetObjectItem(json_root, "recover_interval");
        if (json_recover_interval && json_recover_interval->type == cJSON_Number)
            tpl.recover_interval = json_recover_interval->valuedouble;

        // �����ָ���Χ
        cJSON* json_recover_range = cJSON_GetObjectItem(json_root, "recover_range");
        if (json_recover_range && json_recover_range->type == cJSON_Number)
            tpl.recover_range = json_recover_range->valuedouble;

        // �����ָ�ǿ��
        cJSON* json_recover_intensity = cJSON_GetObjectItem(json_root, "recover_intensity");
        if (json_recover_intensity && json_recover_intensity->type == cJSON_Number)
            tpl.recover_intensity = json_recover_intensity->valuedouble;
    }
};

#endif // !_CONFIG_MANAGER_H_
