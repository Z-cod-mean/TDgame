#ifndef _PLACE_PANEL_H_
#define _PLACE_PANEL_H_

#include "panel.h"               // ���������
#include "coin_manager.h"        // ��ҹ���ϵͳ
#include "tower_manager.h"       // ����������ϵͳ
#include "resources_manager.h"   // ��Դ����ϵͳ

#include <SDL2_gfxPrimitives.h>  // SDL2ͼ�λ�����չ��

// ���÷������Ĳ�������ࣨ�̳���Panel��
class PlacePanel : public Panel
{
public:
	PlacePanel()
	{
		// ����Դ�������л�ȡ��������������Դ
		const ResourcesManager::TexturePool& texture_pool
			= ResourcesManager::instance()->get_texture_pool();

		// ���ò�ͬ״̬�µ��������
		tex_idle = texture_pool.find(ResID::Tex_UIPlaceIdle)->second;           // Ĭ��״̬����
		tex_hovered_top = texture_pool.find(ResID::Tex_UIPlaceHoveredTop)->second;   // ������ͣ����
		tex_hovered_left = texture_pool.find(ResID::Tex_UIPlaceHoveredLeft)->second;  // �����ͣ����
		tex_hovered_right = texture_pool.find(ResID::Tex_UIPlaceHoveredRight)->second;// �Ҳ���ͣ����
	}

	~PlacePanel() = default;  // Ĭ����������

	// �������״̬��ÿ֡���ã�
	void on_update(SDL_Renderer* renderer) override
	{
		static TowerManager* instance = TowerManager::instance();

		// ��ȡ���ַ������Ľ������
		val_top = (int)instance->get_place_cost(TowerType::Axeman);   // ����������
		val_left = (int)instance->get_place_cost(TowerType::Archer);  // ������������
		val_right = (int)instance->get_place_cost(TowerType::Gunner); // ǹ��������

		// �������ַ������Ĺ�����Χ��ת��Ϊ���أ�
		reg_top = (int)instance->get_damage_range(TowerType::Axeman) * SIZE_TILE;   // ��������Χ
		reg_left = (int)instance->get_damage_range(TowerType::Archer) * SIZE_TILE;  // ����������Χ
		reg_right = (int)instance->get_damage_range(TowerType::Gunner) * SIZE_TILE; // ǹ������Χ

		// ���û�����·����������ı���Ⱦ��
		Panel::on_update(renderer);
	}

	// ��Ⱦ��壨ÿ֡���ã�
	void on_render(SDL_Renderer* renderer) override
	{
		if (!visible) return;  // ��岻�ɼ�ʱ������Ⱦ

		// ������ͣ״̬��ȡ��Ӧ�������Ĺ�����Χ
		int reg = 0;
		switch (hovered_target)
		{
		case Panel::HoveredTarget::Top:
			reg = reg_top;    // ����������ͣ����������
			break;
		case Panel::HoveredTarget::Left:
			reg = reg_left;   // ���������ͣ������������
			break;
		case Panel::HoveredTarget::Right:
			reg = reg_right;  // �Ҳ�������ͣ��ǹ������
			break;
		}

		// ���Ʒ�����������Χָʾ��
		if (reg > 0)
		{
			// ���ư�͸�����Բ����������
			filledCircleRGBA(renderer, center_pos.x, center_pos.y, reg,
				color_region_content.r, color_region_content.g, color_region_content.b, color_region_content.a);

			// ����Բ�α߿򣨷�Χ�߽磩
			aacircleRGBA(renderer, center_pos.x, center_pos.y, reg,
				color_region_frame.r, color_region_frame.g, color_region_frame.b, color_region_frame.a);
		}

		// ���û�����Ⱦ�������������������ı���
		Panel::on_render(renderer);
	}

protected:
	// �����������¼������ø�������
	void on_click_top_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// ������Ƿ��㹻
		if (val_top <= instance->get_current_coin_num())
		{
			// ���ø�����
			TowerManager::instance()->place_tower(TowerType::Axeman, idx_tile_selected);
			// �۳���Ӧ���
			instance->decrease_coin(val_top);
		}
	}

	// ����������¼������ù���������
	void on_click_left_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// ������Ƿ��㹻
		if (val_left <= instance->get_current_coin_num())
		{
			// ���ù�������
			TowerManager::instance()->place_tower(TowerType::Archer, idx_tile_selected);
			// �۳���Ӧ���
			instance->decrease_coin(val_left);
		}
	}

	// �Ҳ��������¼�������ǹ������
	void on_click_right_area() override
	{
		CoinManager* instance = CoinManager::instance();

		// ������Ƿ��㹻
		if (val_right <= instance->get_current_coin_num())
		{
			// ����ǹ����
			TowerManager::instance()->place_tower(TowerType::Gunner, idx_tile_selected);
			// �۳���Ӧ���
			instance->decrease_coin(val_right);
		}
	}

private:
	// ��Χָʾ����ɫ����
	const SDL_Color color_region_frame = { 30, 80, 162, 175 };    // ��ɫ�߿�RGBA:30,80,162,175��
	const SDL_Color color_region_content = { 0, 149, 217, 75 };   // ǳ����䣨RGBA:0,149,217,75��

private:
	// ���ַ������Ĺ�����Χ�����ص�λ��
	int reg_top = 0;     // ������������Χ
	int reg_left = 0;    // ��������������Χ
	int reg_right = 0;   // ǹ����������Χ
};

#endif // !_PLACE_PANEL_H_
