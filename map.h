#ifndef _MAP_H_
#define _MAP_H_

#include "tile.h"       // ���� Tile ���Ͷ���
#include "route.h"      // ����·�� Route ���߼������ڵ���Ѱ·��

#include <SDL.h>        // SDL �⣨���� SDL_Point ���ͣ�
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

// ��ͼ�ࣺ������ص�ͼ���洢��ͼ��Ƭ��Ϣ������·����Ϣ��
class Map
{
public:
	typedef std::unordered_map<int, Route> SpawnerRoutePool;  // ���ɵ��� -> ·�� ��ӳ��

public:
	Map() = default;       // Ĭ�Ϲ��캯��
	~Map() = default;      // Ĭ����������

	// ���ص�ͼ�ļ�
	bool load(const std::string& path)
	{
		std::ifstream file(path);           // �򿪵�ͼ�ļ�
		if (!file.good()) return false;     // �ļ��򲻿�����ʧ��

		TileMap tile_map_temp;              // ��ʱ��ͼ�������ڹ��������滻

		int idx_x = -1, idx_y = -1;         // ��ǰ tile ����������

		std::string str_line;
		while (std::getline(file, str_line)) // ���ж�ȡ��ͼ�ı�
		{
			str_line = trim_str(str_line);  // ȥ����ǰ��ո�
			if (str_line.empty())
				continue;                   // ��������

			idx_x = -1, idx_y++;            // ÿ�д���ǰ�����������㣬��������һ
			tile_map_temp.emplace_back();   // ��ͼ�������

			std::string str_tile;
			std::stringstream str_stream(str_line);
			while (std::getline(str_stream, str_tile, ',')) // �ö��ŷָ�ÿ�� tile ����
			{
				idx_x++;
				tile_map_temp[idx_y].emplace_back();           // ��ǰ������� tile
				Tile& tile = tile_map_temp[idx_y].back();      // ��ȡ����
				load_tile_from_string(tile, str_tile);         // ���ַ���תΪ tile ����
			}
		}

		file.close();  // �ر��ļ�

		// �����ͼΪ�ջ���Ϊ�գ�����ʧ��
		if (tile_map_temp.empty() || tile_map_temp[0].empty())
			return false;

		tile_map = tile_map_temp;     // ������ʽ��ͼ����

		generate_map_cache();         // ���ݵ�ͼ���������ǵ�·�����棨��/�����㣩

		return true;
	}

	// ��ȡ��ͼ��ȣ�������
	size_t get_width() const
	{
		if (tile_map.empty())
			return 0;

		return tile_map[0].size();
	}

	// ��ȡ��ͼ�߶ȣ�������
	size_t get_height() const
	{
		return tile_map.size();
	}

	// ��ȡ���� tile_map ���ã�������Ⱦ�ȣ�
	const TileMap& get_tile_map() const
	{
		return tile_map;
	}

	// ��ȡ����λ��������home��
	const SDL_Point& get_idx_home() const
	{
		return idx_home;
	}

	// ��ȡ���е������ɵ��·����
	const SpawnerRoutePool& get_idx_spawner_pool() const
	{
		return spwaner_route_pool;
	}

	// ��ָ��λ�÷�����
	void place_tower(const SDL_Point& idx_tile)
	{
		tile_map[idx_tile.y][idx_tile.x].has_tower = true;
	}

private:
	TileMap tile_map;                          // �洢������ͼ�Ķ�ά��Ƭ��Ϣ
	SDL_Point idx_home = { 0 };                // ��һ���λ�ã�ͨ��������ʶ��
	SpawnerRoutePool spwaner_route_pool;       // �������ɵ��ż���Ӧ·��

private:
	// ȥ���ַ���ǰ��ո��� tab
	std::string trim_str(const std::string& str)
	{
		size_t begin_idx = str.find_first_not_of(" \t");
		if (begin_idx == std::string::npos)
			return "";
		size_t end_idx = str.find_last_not_of(" \t");
		size_t idx_range = end_idx - begin_idx + 1;

		return str.substr(begin_idx, idx_range);
	}

	// ������ tile ���ַ�����Ϣ����Ϊ Tile ����
	void load_tile_from_string(Tile& tile, const std::string& str)
	{
		std::string str_tidy = trim_str(str);   // ����ǰ��ո�

		std::string str_value;
		std::vector<int> values;                // ���ڴ洢��б�ָܷ�������ֵ
		std::stringstream str_stream(str_tidy);

		while (std::getline(str_stream, str_value, '\\')) // �� '\' �ָ�
		{
			int value;
			try
			{
				value = std::stoi(str_value);   // ����ת��Ϊ����
			}
			catch (const std::invalid_argument&)
			{
				value = -1;                      // ʧ������Ϊ -1
			}
			values.push_back(value);
		}

		// ���� tile ��������ԣ����Ǳ߽磩
		tile.terrian = (values.size() < 1 || values[0] < 0) ? 0 : values[0];
		tile.decoration = (values.size() < 2) ? -1 : values[1];
		tile.direction = (Tile::Direction)((values.size() < 3 || values[2] < 0) ? 0 : values[2]);
		tile.special_flag = (values.size() <= 3) ? -1 : values[3];
	}

	// �ӵ�ͼ�з�������㣨���ء������㣩��������·�� Route ����
	void generate_map_cache()
	{
		for (int y = 0; y < get_height(); y++)
		{
			for (int x = 0; x < get_width(); x++)
			{
				const Tile& tile = tile_map[y][x];
				if (tile.special_flag < 0)
					continue;

				if (tile.special_flag == 0)         // 0 ��ʾ����
				{
					idx_home.x = x;
					idx_home.y = y;
				}
				else                                // ���������������������
				{
					spwaner_route_pool[tile.special_flag] = Route(tile_map, { x, y });
				}
			}
		}
	}
};

#endif // !_MAP_H_
