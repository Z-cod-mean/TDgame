#ifndef _WAVE_H_
#define _WAVE_H_

#include "enemy_type.h"  // 包含敌人类型定义

#include <vector>        // 向量容器

// 敌人波次结构体（定义一波敌人的生成规则）
struct Wave
{
	// 单个敌人生成事件结构体
	struct SpawnEvent
	{
		double interval = 0;       // 距离上一个事件的间隔时间（秒）
		int spawn_point = 1;       // 出生点索引（多个出生点时使用）
		EnemyType enemy_type = EnemyType::Slim;  // 生成的敌人类型
	};

	double rawards = 0;             // 完成该波次的金币奖励
	double interval = 0;            // 该波次开始前的等待时间（秒）
	std::vector<SpawnEvent> spawn_event_list;  // 生成事件列表
};

#endif // !_WAVE_H_
