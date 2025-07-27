#ifndef _UPGRADE_PANEL_H_
#define _UPGRADE_PANEL_H_

#include "panel.h"               // ���������
#include "tower_manager.h"       // ����������ϵͳ
#include "resources_manager.h"   // ��Դ����ϵͳ

// ������������������ࣨ�̳���Panel��
class UpgradePanel : public Panel
{
public:
	UpgradePanel()
	{
		// ����Դ�������л�ȡ������������������Դ
		const ResourcesManager::TexturePool& texture_pool
			= ResourcesManager::instance()->get_texture_pool();

		// ���ò�ͬ״̬�µ������������
		tex_idle = texture_pool.find(ResID::Tex_UIUpgradeIdle)->second;           // Ĭ��״̬����
		tex_hovered_top = texture_pool.find(ResID::Tex_UIUpgradeHoveredTop)->second;   // ������ͣ����
		tex_hovered_left = texture_pool.find(ResID::Tex_UIUpgradeHoveredLeft)->second;  // �����ͣ����
		tex_hovered_right = texture_pool.find(ResID::Tex_UIUpgradeHoveredRight)->second;// �Ҳ���ͣ����
	}

	~UpgradePanel() = default;  // Ĭ����������

	// �������״̬��ÿ֡���ã�
	void on_update(SDL_Renderer* renderer)
	{
		static TowerManager* instance = TowerManager::instance();

		// ��ȡ���ַ���������������
		val_top = (int)instance->get_upgrade_cost(TowerType::Axeman);   // ��������������
		val_left = (int)instance->get_upgrade_cost(TowerType::Archer);  // ����������������
		val_right = (int)instance->get_upgrade_cost(TowerType::Gunner); // ǹ������������

		// ���û�����·����������ı���Ⱦ��
		Panel::on_update(renderer);
	}

protected:
	// �����������¼���������������
	void on_click_top_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// ������Ƿ��㹻������������Ч
		if (val_top > 0 && val_top <= instance->get_current_coin_num())
		{
			// ����������
			TowerManager::instance()->upgrade_tower(TowerType::Axeman);
			// �۳���������
			instance->decrease_coin(val_top);
		}
	}

	// ����������¼�����������������
	void on_click_left_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// ������Ƿ��㹻������������Ч
		if (val_left > 0 && val_left <= instance->get_current_coin_num())
		{
			// ������������
			TowerManager::instance()->upgrade_tower(TowerType::Axeman);
			// �۳���������
			instance->decrease_coin(val_left);
		}
	}

	// �Ҳ��������¼�������ǹ������
	void on_click_right_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// ������Ƿ��㹻������������Ч
		if (val_right > 0 && val_right <= instance->get_current_coin_num())
		{
			// ����ǹ����
			TowerManager::instance()->upgrade_tower(TowerType::Axeman);
			// �۳���������
			instance->decrease_coin(val_right);
		}
	}

};

#endif // !_UPGRADE_PANEL_H_
