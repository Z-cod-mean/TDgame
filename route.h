#ifndef _ROUTE_H_
#define _ROUTE_H_

#include "tile.h"  // 包含瓦片类定义

#include <SDL.h>    // SDL基础库
#include <vector>   // 向量容器

// 路径类（用于计算敌人在地图上的移动路径）
class Route
{
public:
	typedef std::vector<SDL_Point> IdxList;  // 路径索引列表类型别名

public:
	Route() = default;  // 默认构造函数

	// 构造函数：根据地图和起点计算路径
	Route(const TileMap& map, const SDL_Point& idx_origin)
	{
		// 获取地图尺寸
		size_t width_map = map[0].size();   // 地图宽度（列数）
		size_t height_map = map.size();     // 地图高度（行数）
		SDL_Point idx_next = idx_origin;    // 当前路径点索引

		// 循环构建路径
		while (true)
		{
			// 边界检查：超出地图范围则终止
			if (idx_next.x >= width_map || idx_next.y >= height_map)
				break;

			// 重复索引检查：避免路径循环
			if (check_duplicate_idx(idx_next))
				break;
			else
				idx_list.push_back(idx_next);  // 添加新索引到路径

			bool is_next_dir_exist = true;  // 是否存在有效方向标志
			const Tile& tile = map[idx_next.y][idx_next.x];  // 获取当前瓦片

			// 特殊标志检查：0表示路径终点
			if (tile.special_flag == 0)
				break;

			// 根据瓦片方向确定下一个索引
			switch (tile.direction)
			{
			case Tile::Direction::Up:     // 向上移动
				idx_next.y--;
				break;
			case Tile::Direction::Down:   // 向下移动
				idx_next.y++;
				break;
			case Tile::Direction::Left:   // 向左移动
				idx_next.x--;
				break;
			case Tile::Direction::Right:  // 向右移动
				idx_next.x++;
				break;
			default:  // 无效方向
				is_next_dir_exist = false;
				break;
			}

			// 无有效方向则终止
			if (!is_next_dir_exist)
				break;
		}
	}

	~Route() = default;  // 默认析构函数

	// 获取路径索引列表
	const IdxList& get_idx_list() const
	{
		return idx_list;
	}

private:
	IdxList idx_list;  // 存储路径上的所有瓦片索引

private:
	// 检查索引是否已在路径中（避免循环）
	bool check_duplicate_idx(const SDL_Point& target_idx)
	{
		// 遍历现有路径点
		for (const SDL_Point& idx : idx_list)
		{
			// 检查坐标是否匹配
			if (idx.x == target_idx.x && idx.y == target_idx.y)
				return true;
		}

		return false;  // 未找到重复索引
	}

};

#endif // !_ROUTE_H_
