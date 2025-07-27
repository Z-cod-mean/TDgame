#ifndef _WAVE_H_
#define _WAVE_H_

#include "enemy_type.h"  // �����������Ͷ���

#include <vector>        // ��������

// ���˲��νṹ�壨����һ�����˵����ɹ���
struct Wave
{
	// �������������¼��ṹ��
	struct SpawnEvent
	{
		double interval = 0;       // ������һ���¼��ļ��ʱ�䣨�룩
		int spawn_point = 1;       // ���������������������ʱʹ�ã�
		EnemyType enemy_type = EnemyType::Slim;  // ���ɵĵ�������
	};

	double rawards = 0;             // ��ɸò��εĽ�ҽ���
	double interval = 0;            // �ò��ο�ʼǰ�ĵȴ�ʱ�䣨�룩
	std::vector<SpawnEvent> spawn_event_list;  // �����¼��б�
};

#endif // !_WAVE_H_
