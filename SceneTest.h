#pragma once
#include "DxLib.h"
class SceneTest
{
public:
	SceneTest();
	virtual ~SceneTest();

	void init();
	void update();
	void draw();

private:
	enum Type
	{
		kTypeDfs,	// DFS 深さ優先探索
		kTypeBfs,	// BFS 幅優先探索
		kTypeAStar,	// A*
	};


private:
	// グリッドの数

	static constexpr int kGridNumX = 100;
	static constexpr int kGridNumY = 100;
	static constexpr int kGridSize = 8;

//	static constexpr int kGridNumX = 256;
//	static constexpr int kGridNumY = 144;
//	static constexpr int kGridSize = 4;

//	static constexpr int kGridNumX = 128;
//	static constexpr int kGridNumY = 72;
//	static constexpr int kGridSize = 8;

//	static constexpr int kGridNumX = 64;
//	static constexpr int kGridNumY = 36;
//	static constexpr int kGridSize = 16;

private:
	// 移動可能情報のクリア
	void clearCount();
	// 再帰を利用した移動可能情報の設定
	void setCount(int x, int y, int count);	// dfs

	void setCountLoop(int startX, int startY, int startCount);	// bfs

	void setCountAstar(int startX, int startY, int startCount);	// A*

	// ゴールへの道筋を表示
	void drawRoute();

private:

	int m_frameCount;

//	bool m_isDfs;	// dfs or bfs
	Type m_type;

	// 地形情報(移動コスト)	-1は移動不可
	int m_fieldTbl[kGridNumY][kGridNumX];

	int m_goalX;
	int m_goalY;

	// 再帰
	int m_moveCount;
	int m_countTbl[kGridNumY][kGridNumX];

	// 再帰関数が呼び出された回数
	int m_saikiCount;

	// 探索時間
	LONGLONG m_searchTime;
};

