#ifndef _ROUTE_H_
#define _ROUTE_H_

#include "tile.h"  // ������Ƭ�ඨ��

#include <SDL.h>    // SDL������
#include <vector>   // ��������

// ·���ࣨ���ڼ�������ڵ�ͼ�ϵ��ƶ�·����
class Route
{
public:
	typedef std::vector<SDL_Point> IdxList;  // ·�������б����ͱ���

public:
	Route() = default;  // Ĭ�Ϲ��캯��

	// ���캯�������ݵ�ͼ��������·��
	Route(const TileMap& map, const SDL_Point& idx_origin)
	{
		// ��ȡ��ͼ�ߴ�
		size_t width_map = map[0].size();   // ��ͼ��ȣ�������
		size_t height_map = map.size();     // ��ͼ�߶ȣ�������
		SDL_Point idx_next = idx_origin;    // ��ǰ·��������

		// ѭ������·��
		while (true)
		{
			// �߽��飺������ͼ��Χ����ֹ
			if (idx_next.x >= width_map || idx_next.y >= height_map)
				break;

			// �ظ�������飺����·��ѭ��
			if (check_duplicate_idx(idx_next))
				break;
			else
				idx_list.push_back(idx_next);  // �����������·��

			bool is_next_dir_exist = true;  // �Ƿ������Ч�����־
			const Tile& tile = map[idx_next.y][idx_next.x];  // ��ȡ��ǰ��Ƭ

			// �����־��飺0��ʾ·���յ�
			if (tile.special_flag == 0)
				break;

			// ������Ƭ����ȷ����һ������
			switch (tile.direction)
			{
			case Tile::Direction::Up:     // �����ƶ�
				idx_next.y--;
				break;
			case Tile::Direction::Down:   // �����ƶ�
				idx_next.y++;
				break;
			case Tile::Direction::Left:   // �����ƶ�
				idx_next.x--;
				break;
			case Tile::Direction::Right:  // �����ƶ�
				idx_next.x++;
				break;
			default:  // ��Ч����
				is_next_dir_exist = false;
				break;
			}

			// ����Ч��������ֹ
			if (!is_next_dir_exist)
				break;
		}
	}

	~Route() = default;  // Ĭ����������

	// ��ȡ·�������б�
	const IdxList& get_idx_list() const
	{
		return idx_list;
	}

private:
	IdxList idx_list;  // �洢·���ϵ�������Ƭ����

private:
	// ��������Ƿ�����·���У�����ѭ����
	bool check_duplicate_idx(const SDL_Point& target_idx)
	{
		// ��������·����
		for (const SDL_Point& idx : idx_list)
		{
			// ��������Ƿ�ƥ��
			if (idx.x == target_idx.x && idx.y == target_idx.y)
				return true;
		}

		return false;  // δ�ҵ��ظ�����
	}

};

#endif // !_ROUTE_H_
