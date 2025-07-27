#ifndef _TOWER_MANAGER_H_
#define _TOWER_MANAGER_H_

#include "tower.h"             // ��������
#include "tower_type.h"        // ������ö��
#include "manager.h"           // ����������
#include "archer_tower.h"      // ��������
#include "axeman_tower.h"      // ������
#include "gunner_tower.h"      // ǹ����
#include "config_manager.h"    // ���ù�����
#include "resources_manager.h" // ��Դ������

#include <vector>              // ��������

/**
 * @class TowerManager
 * @brief �������������࣬������Ϸ�����з������Ĵ����������͹���
 *
 * ����ʹ�õ���ģʽʵ�֣�ȫ��Ψһ����Ҫ���ܰ�����
 * 1. �������з������ĸ��º���Ⱦ
 * 2. ����������Ľ���������ɱ�
 * 3. ��ȡ�������Ĺ�����Χ
 * 4. �����·�����
 * 5. �������з�����
 */
class TowerManager : public Manager<TowerManager>
{
    friend class Manager<TowerManager>; // ����������˽�й��캯��

public:
    /**
     * @brief ÿ֡�������з�����״̬
     * @param delta ֡ʱ���������룩
     */
    void on_update(double delta)
    {
        // �����������з�����
        for (Tower* tower : tower_list)
            tower->on_update(delta);
    }

    /**
     * @brief ��Ⱦ���з�����
     * @param renderer SDL��Ⱦ��ָ��
     */
    void on_render(SDL_Renderer* renderer)
    {
        // ������Ⱦ���з�����
        for (Tower* tower : tower_list)
            tower->on_render(renderer);
    }

    /**
     * @brief ��ȡ����������ĳɱ�
     * @param type ����������
     * @return ��������������
     *
     * ���������ͺ͵�ǰ�ȼ������û�ȡ�ɱ�
     */
    double get_place_cost(TowerType type)
    {
        static ConfigManager* instance = ConfigManager::instance();

        switch (type)
        {
        case Archer: // ��������
            return instance->archer_template.cost[instance->level_archer];
        case Axeman: // ������
            return instance->axeman_template.cost[instance->level_axeman];
        case Gunner: // ǹ����
            return instance->gunner_template.cost[instance->level_gunner];
        default:
            return 0; // δ֪���ͷ���0
        }
    }

    /**
     * @brief ��ȡ�����������ĳɱ�
     * @param type ����������
     * @return ������������������������-1��
     *
     * 1. ���������Ƿ��Ѵ���ߵȼ���9����
     * 2. ���ص�ǰ�ȼ��������ɱ�
     */
    double get_upgrade_cost(TowerType type)
    {
        static ConfigManager* instance = ConfigManager::instance();

        switch (type)
        {
        case Archer: // ��������
            return instance->level_archer == 9 ? -1 :
                instance->archer_template.upgrade_cost[instance->level_archer];
        case Axeman: // ������
            return instance->level_axeman == 9 ? -1 :
                instance->axeman_template.upgrade_cost[instance->level_axeman];
        case Gunner: // ǹ����
            return instance->level_gunner == 9 ? -1 :
                instance->gunner_template.upgrade_cost[instance->level_gunner];
        default:
            return 0; // δ֪���ͷ���0
        }
    }

    /**
     * @brief ��ȡ�������Ĺ�����Χ
     * @param type ����������
     * @return ������Χ����λ����ͼ���ӣ�
     *
     * ���������ͺ͵�ǰ�ȼ������û�ȡ��Χ
     */
    double get_damage_range(TowerType type)
    {
        static ConfigManager* instance = ConfigManager::instance();

        switch (type)
        {
        case Archer: // ��������
            return instance->archer_template.view_range[instance->level_archer];
        case Axeman: // ������
            return instance->axeman_template.view_range[instance->level_axeman];
        case Gunner: // ǹ����
            return instance->gunner_template.view_range[instance->level_gunner];
        default:
            return 0; // δ֪���ͷ���0
        }
    }

    /**
     * @brief ��ָ��λ�÷��÷�����
     * @param type ����������
     * @param idx ��ͼ��������λ��
     *
     * 1. ����ָ�����͵ķ�����
     * 2. ����ʵ������λ��
     * 3. ��ӵ��������б�
     * 4. ���µ�ͼ״̬
     * 5. ���ŷ�����Ч
     */
    void place_tower(TowerType type, const SDL_Point& idx)
    {
        Tower* tower = nullptr; // �·�����ָ��

        // �������ʹ����������������
        switch (type)
        {
        case Archer: // ��������
            tower = new ArcherTower();
            break;
        case Axeman: // ������
            tower = new AxemanTower();
            break;
        case Gunner: // ǹ����
            tower = new GunnerTower();
            break;
        default:     // Ĭ�ϴ�����������
            tower = new ArcherTower();
            break;
        }

        // �������������Ļ�ϵ�����λ��
        static Vector2 position;
        static const SDL_Rect& rect = ConfigManager::instance()->rect_tile_map;

        // �������ĵ�λ�� = ��ͼ��� + �������� * ���ӳߴ� + �������ƫ��
        position.x = rect.x + idx.x * SIZE_TILE + SIZE_TILE / 2;
        position.y = rect.y + idx.y * SIZE_TILE + SIZE_TILE / 2;

        // ���÷�����λ��
        tower->set_position(position);

        // ��ӵ��������б�
        tower_list.push_back(tower);

        // ���µ�ͼ״̬����Ǹø������з�������
        ConfigManager::instance()->map.place_tower(idx);

        // ���ŷ�����Ч
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_PlaceTower)->second, 0);
    }

    /**
     * @brief ����ָ�����͵ķ�����
     * @param type ����������
     *
     * 1. ��鵱ǰ�ȼ��Ƿ��Ѵ����ޣ�9����
     * 2. ������Ӧ���������͵ĵȼ�
     * 3. ����������Ч
     *
     * ע�⣺����ȫ��������Ӱ������ͬ���ͷ�����
     */
    void upgrade_tower(TowerType type)
    {
        static ConfigManager* instance = ConfigManager::instance();

        // ��������������Ӧ������
        switch (type)
        {
        case Archer: // ��������
            instance->level_archer = (instance->level_archer >= 9) ?
                9 : instance->level_archer + 1;
            break;
        case Axeman: // ������
            instance->level_axeman = (instance->level_axeman >= 9) ?
                9 : instance->level_axeman + 1;
            break;
        case Gunner: // ǹ����
            instance->level_gunner = (instance->level_gunner >= 9) ?
                9 : instance->level_gunner + 1;
            break;
        }

        // ����������Ч
        static const ResourcesManager::SoundPool& sound_pool
            = ResourcesManager::instance()->get_sound_pool();
        Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_TowerLevelUp)->second, 0);
    }

protected:
    /// �������캯��������ģʽ��
    TowerManager() = default;

    /// �����������������з�����
    ~TowerManager()
    {
        for (Tower* tower : tower_list)
            delete tower;
    }

private:
    std::vector<Tower*> tower_list; ///< �洢���з��������б�
};

#endif // !_TOWER_MANAGER_H_