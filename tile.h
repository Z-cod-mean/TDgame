#ifndef _TILE_H_
#define _TILE_H_

#include <vector>  // ������������

#define SIZE_TILE 48  // ��Ƭ�ߴ磨���أ�

// ��Ƭ�ṹ�壨��ʾ��ͼ�ϵ�һ����Ԫ��
struct Tile
{
	// ����ö�٣�������Ƭ�����ӷ���
	enum class Direction
	{
		None = 0,  // �޷�����ͨ��Ƭ��
		Up,        // �������ӣ�·������
		Down,      // �������ӣ�·������
		Left,      // �������ӣ�·������
		Right      // �������ӣ�·������
	};

	int terrian = 0;       // �������ͣ�0=��ͨ���棬����ֵ��ʾ������Σ�
	int decoration = -1;   // װ����ID��-1=��װ�Σ�
	int special_flag = -1; // �����־��-1=���������ԣ�0=·���յ㣩
	bool has_tower = false; // �Ƿ��з�������true=�ѱ�ռ�ã�
	Direction direction = Direction::None; // ·������None=��·����Ƭ��
};

// ��Ƭ��ͼ���ͱ�������ά�����ṹ��
typedef std::vector<std::vector<Tile>> TileMap;

#endif // !_TILE_H_