#ifndef _MAP_H_
#define _MAP_H_

#include "tile.h"       // 引入 Tile 类型定义
#include "route.h"      // 引入路径 Route 的逻辑（用于敌人寻路）

#include <SDL.h>        // SDL 库（用于 SDL_Point 类型）
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

// 地图类：负责加载地图、存储地图瓦片信息、生成路径信息等
class Map
{
public:
	typedef std::unordered_map<int, Route> SpawnerRoutePool;  // 生成点编号 -> 路径 的映射

public:
	Map() = default;       // 默认构造函数
	~Map() = default;      // 默认析构函数

	// 加载地图文件
	bool load(const std::string& path)
	{
		std::ifstream file(path);           // 打开地图文件
		if (!file.good()) return false;     // 文件打不开返回失败

		TileMap tile_map_temp;              // 临时地图对象，用于构建完再替换

		int idx_x = -1, idx_y = -1;         // 当前 tile 的坐标索引

		std::string str_line;
		while (std::getline(file, str_line)) // 按行读取地图文本
		{
			str_line = trim_str(str_line);  // 去除行前后空格
			if (str_line.empty())
				continue;                   // 空行跳过

			idx_x = -1, idx_y++;            // 每行处理前，列索引归零，行索引加一
			tile_map_temp.emplace_back();   // 地图添加新行

			std::string str_tile;
			std::stringstream str_stream(str_line);
			while (std::getline(str_stream, str_tile, ',')) // 用逗号分隔每个 tile 数据
			{
				idx_x++;
				tile_map_temp[idx_y].emplace_back();           // 当前行添加新 tile
				Tile& tile = tile_map_temp[idx_y].back();      // 获取引用
				load_tile_from_string(tile, str_tile);         // 将字符串转为 tile 对象
			}
		}

		file.close();  // 关闭文件

		// 如果地图为空或列为空，加载失败
		if (tile_map_temp.empty() || tile_map_temp[0].empty())
			return false;

		tile_map = tile_map_temp;     // 更新正式地图数据

		generate_map_cache();         // 根据地图生成特殊标记的路径缓存（家/出生点）

		return true;
	}

	// 获取地图宽度（列数）
	size_t get_width() const
	{
		if (tile_map.empty())
			return 0;

		return tile_map[0].size();
	}

	// 获取地图高度（行数）
	size_t get_height() const
	{
		return tile_map.size();
	}

	// 获取整个 tile_map 引用（用于渲染等）
	const TileMap& get_tile_map() const
	{
		return tile_map;
	}

	// 获取基地位置索引（home）
	const SDL_Point& get_idx_home() const
	{
		return idx_home;
	}

	// 获取所有敌人生成点的路径池
	const SpawnerRoutePool& get_idx_spawner_pool() const
	{
		return spwaner_route_pool;
	}

	// 在指定位置放置塔
	void place_tower(const SDL_Point& idx_tile)
	{
		tile_map[idx_tile.y][idx_tile.x].has_tower = true;
	}

private:
	TileMap tile_map;                          // 存储整个地图的二维瓦片信息
	SDL_Point idx_home = { 0 };                // 玩家基地位置（通过特殊标记识别）
	SpawnerRoutePool spwaner_route_pool;       // 所有生成点编号及对应路径

private:
	// 去除字符串前后空格与 tab
	std::string trim_str(const std::string& str)
	{
		size_t begin_idx = str.find_first_not_of(" \t");
		if (begin_idx == std::string::npos)
			return "";
		size_t end_idx = str.find_last_not_of(" \t");
		size_t idx_range = end_idx - begin_idx + 1;

		return str.substr(begin_idx, idx_range);
	}

	// 将单个 tile 的字符串信息解析为 Tile 对象
	void load_tile_from_string(Tile& tile, const std::string& str)
	{
		std::string str_tidy = trim_str(str);   // 清理前后空格

		std::string str_value;
		std::vector<int> values;                // 用于存储反斜杠分割后的整数值
		std::stringstream str_stream(str_tidy);

		while (std::getline(str_stream, str_value, '\\')) // 以 '\' 分割
		{
			int value;
			try
			{
				value = std::stoi(str_value);   // 尝试转换为整数
			}
			catch (const std::invalid_argument&)
			{
				value = -1;                      // 失败则设为 -1
			}
			values.push_back(value);
		}

		// 设置 tile 对象的属性（考虑边界）
		tile.terrian = (values.size() < 1 || values[0] < 0) ? 0 : values[0];
		tile.decoration = (values.size() < 2) ? -1 : values[1];
		tile.direction = (Tile::Direction)((values.size() < 3 || values[2] < 0) ? 0 : values[2]);
		tile.special_flag = (values.size() <= 3) ? -1 : values[3];
	}

	// 从地图中分析特殊点（基地、出生点），并生成路径 Route 缓存
	void generate_map_cache()
	{
		for (int y = 0; y < get_height(); y++)
		{
			for (int x = 0; x < get_width(); x++)
			{
				const Tile& tile = tile_map[y][x];
				if (tile.special_flag < 0)
					continue;

				if (tile.special_flag == 0)         // 0 表示基地
				{
					idx_home.x = x;
					idx_home.y = y;
				}
				else                                // 其它正整数代表出生点编号
				{
					spwaner_route_pool[tile.special_flag] = Route(tile_map, { x, y });
				}
			}
		}
	}
};

#endif // !_MAP_H_
