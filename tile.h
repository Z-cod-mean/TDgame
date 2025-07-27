#ifndef _TILE_H_
#define _TILE_H_

#include <vector>  // 包含向量容器

#define SIZE_TILE 48  // 瓦片尺寸（像素）

// 瓦片结构体（表示地图上的一个单元格）
struct Tile
{
	// 方向枚举（定义瓦片的连接方向）
	enum class Direction
	{
		None = 0,  // 无方向（普通瓦片）
		Up,        // 向上连接（路径方向）
		Down,      // 向下连接（路径方向）
		Left,      // 向左连接（路径方向）
		Right      // 向右连接（路径方向）
	};

	int terrian = 0;       // 地形类型（0=普通地面，其他值表示特殊地形）
	int decoration = -1;   // 装饰物ID（-1=无装饰）
	int special_flag = -1; // 特殊标志（-1=无特殊属性，0=路径终点）
	bool has_tower = false; // 是否有防御塔（true=已被占用）
	Direction direction = Direction::None; // 路径方向（None=非路径瓦片）
};

// 瓦片地图类型别名（二维向量结构）
typedef std::vector<std::vector<Tile>> TileMap;

#endif // !_TILE_H_