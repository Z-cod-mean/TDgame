#ifndef _GUNNER_TOWER_H_
#define _GUNNER_TOWER_H_

#include "tower.h"
#include "resources_manager.h"

/**
 * @class GunnerTower
 * @brief ǹ�����࣬�̳��Ի������࣬ʵ��ǹ�������ض����Ժ���Ϊ
 */
class GunnerTower : public Tower
{
public:
	/**
	 * @brief ���캯������ʼ��ǹ��������������֡������
	 *
	 * ����Դ��������ȡǹ�����������ø����򶯻�֡������
	 * �������ĳߴ硢���͡��ӵ��ٶȺ��ӵ�����
	 */
	GunnerTower()
	{
		// ����Դ��������ȡǹ������������̬���������ظ���ȡ��
		static SDL_Texture* tex_gunner = ResourcesManager::instance()
			->get_texture_pool().find(ResID::Tex_Gunner)->second;

		// ��������򶯻���֡�����б�������е�λ��������
		static const std::vector<int> idx_list_idle_up = { 4, 5 };     // �������ö���֡
		static const std::vector<int> idx_list_idle_down = { 0, 1 };   // �������ö���֡
		static const std::vector<int> idx_list_idle_left = { 12, 13 }; // �������ö���֡
		static const std::vector<int> idx_list_idle_right = { 8, 9 }; // �������ö���֡
		static const std::vector<int> idx_list_fire_up = { 20, 21, 22, 23 };    // ���Ϲ�������֡
		static const std::vector<int> idx_list_fire_down = { 16, 17, 18, 19 };  // ���¹�������֡
		static const std::vector<int> idx_list_fire_left = { 28, 29, 30, 31 };  // ���󹥻�����֡
		static const std::vector<int> idx_list_fire_right = { 24, 25, 26, 27 }; // ���ҹ�������֡

		// ���ø����򶯻�֡���ݣ�������������֡�����б�
		anim_idle_up.set_frame_data(tex_gunner, 4, 8, idx_list_idle_up);     // 4��8�еľ����
		anim_idle_down.set_frame_data(tex_gunner, 4, 8, idx_list_idle_down);
		anim_idle_left.set_frame_data(tex_gunner, 4, 8, idx_list_idle_left);
		anim_idle_right.set_frame_data(tex_gunner, 4, 8, idx_list_idle_right);
		anim_fire_up.set_frame_data(tex_gunner, 4, 8, idx_list_fire_up);
		anim_fire_down.set_frame_data(tex_gunner, 4, 8, idx_list_fire_down);
		anim_fire_left.set_frame_data(tex_gunner, 4, 8, idx_list_fire_left);
		anim_fire_right.set_frame_data(tex_gunner, 4, 8, idx_list_fire_right);

		// �������ĳߴ磨���أ�
		size.x = 48, size.y = 48;

		// ����������Ϊǹ��
		tower_type = TowerType::Gunner;

		// �����ӵ����ԣ��ٶȺ����ͣ�
		fire_speed = 6;              // �ӵ������ٶ�
		bullet_type = BulletType::Shell; // ʹ���ڵ��ӵ�����
	}

	/// Ĭ����������
	~GunnerTower() = default;
};

#endif // !_GUNNER_TOWER_H_
