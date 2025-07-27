#ifndef _WAVE_MANAGER_H_
#define _WAVE_MANAGER_H_

// ������ͷ�ļ�
#include "timer.h"           // ��ʱ������
#include "manager.h"         // ��������
#include "config_manager.h"  // ��������
#include "enemy_manager.h"   // ��������
#include "coin_manager.h"    // ��ҽ���

/**
 * @class WaveManager
 * @brief ���˵������ɹ���ϵͳ������ģʽ��
 *
 * ����ְ��
 * 1. �����ý������ɵ��˲���
 * 2. ���Ʋ��μ��ʱ��
 * 3. ��������ɽ���
 * 4. ������Ϸʤ������
 */
class WaveManager : public Manager<WaveManager>
{
    friend class Manager<WaveManager>;  // ������ģ�����˽�й���

public:
    /**
     * @brief ÿ֡���²���״̬
     * @param delta ֡���ʱ�䣨�룩
     *
     * �������̣�
     * 1. ��鵱ǰ����״̬
     * 2. ���¼�ʱ��
     * 3. �������������¼�
     * 4. ����������߼�
     */
    void on_update(double delta)
    {
        static ConfigManager* instance = ConfigManager::instance();

        if (instance->is_game_over)  // ��Ϸ����ʱֹͣ����
            return;

        // ���µ�ǰ��Ծ�ļ�ʱ��
        if (!is_wave_started)
            timer_start_wave.on_update(delta);  // ���μ����ʱ
        else
            timer_spawn_enemy.on_update(delta); // �������ɼ����ʱ

        // ��鲨�����������
        // 1. ���������һ������ 
        // 2. �������޴�����
        if (is_spawned_last_enemy && EnemyManager::instance()->check_cleared())
        {
            // ���Ų��ν������
            CoinManager::instance()->increase_coin(
                instance->wave_list[idx_wave].rawards);

            idx_wave++;  // ������һ��

            // ʤ�����������в������
            if (idx_wave >= instance->wave_list.size())
            {
                instance->is_game_win = true;
                instance->is_game_over = true;
            }
            else  // ׼����һ��
            {
                idx_spawn_event = 0;
                is_wave_started = true;
                is_spawned_last_enemy = false;

                // ������һ���ļ��ʱ��
                const Wave& wave = instance->wave_list[idx_wave];
                timer_start_wave.set_wait_time(wave.interval);
                timer_start_wave.restart();
            }
        }
    }

protected:
    /**
     * @brief ���캯������ʼ����ʱ���ص���
     *
     * ��ʼ���������ļ�ʱ����
     * 1. timer_start_wave - ���Ʋ��ο�ʼǰ�ĵȴ�
     * 2. timer_spawn_enemy - ���Ƶ������ɼ��
     */
    WaveManager()
    {
        static const std::vector<Wave>& wave_list =
            ConfigManager::instance()->wave_list;

        // ���ο�ʼ��ʱ�����ã����δ�����
        timer_start_wave.set_one_shot(true);
        timer_start_wave.set_wait_time(wave_list[0].interval);
        timer_start_wave.set_on_timeout([&]() {
            is_wave_started = true;

            // ��ʼ����һ�����������¼�
            timer_spawn_enemy.set_wait_time(
                wave_list[idx_wave].spawn_event_list[0].interval);
            timer_spawn_enemy.restart();
            });

        // �������ɼ�ʱ�����ã���ʽ������
        timer_spawn_enemy.set_one_shot(true);
        timer_spawn_enemy.set_on_timeout([&]() {
            const auto& spawn_event_list =
                wave_list[idx_wave].spawn_event_list;
            const auto& spawn_event = spawn_event_list[idx_spawn_event];

            // ����ָ�����͵ĵ���
            EnemyManager::instance()->spawn_enemy(
                spawn_event.enemy_type,
                spawn_event.spawn_point);

            idx_spawn_event++;

            // ����Ƿ�Ϊ���һ�������¼�
            if (idx_spawn_event >= spawn_event_list.size())
            {
                is_spawned_last_enemy = true;
                return;
            }

            // ������һ�������¼��ļ��
            timer_spawn_enemy.set_wait_time(
                spawn_event_list[idx_spawn_event].interval);
            timer_spawn_enemy.restart();
            });
    }

    ~WaveManager() = default;  // Ĭ������

private:
    // ��ǰ������������0��ʼ��
    int idx_wave = 0;

    // ��ǰ�����ڵĵ��������¼�����
    int idx_spawn_event = 0;

    // ���ο�ʼǰ�ĵȴ���ʱ��
    Timer timer_start_wave;

    // �������ɼ����ʱ��
    Timer timer_spawn_enemy;

    // ״̬��־
    bool is_wave_started = false;         // �Ƿ��ѿ�ʼ��ǰ����
    bool is_spawned_last_enemy = false;   // �Ƿ������ɵ�ǰ�������һ������
};

#endif // !_WAVE_MANAGER_H_