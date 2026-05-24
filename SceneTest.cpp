#include "SceneTest.h"
#include <cassert>
#include <queue>

namespace
{
	
}

SceneTest::SceneTest():
	m_frameCount(0),
	m_type(kTypeDfs),
	m_moveCount(640),
	m_saikiCount(0),
	m_searchTime(0)
{
	clearCount();

	// ランダムに地形情報を生成
	for (int x = 0; x < kGridNumX; x++)
	{
		for (int y = 0; y < kGridNumY; y++)
		{
			if (GetRand(100) >= 70)
			{
				m_fieldTbl[y][x] = -1;
			}
			else
			{
				m_fieldTbl[y][x] = 1;
			}
		}
	}
	// ゴール位置をランダムに決定
	// ゴールが移動不可の地形に設定されると再抽選
	// 最悪永久にループするので移動負荷地形は多すぎないように配置すること
	do
	{
		m_goalX = GetRand(kGridNumX - 1);
		m_goalY = GetRand(kGridNumY - 1);
	} while (m_fieldTbl[m_goalY][m_goalX] < 0);
//	printfDx("goal %d, %d\n", m_goalX, m_goalY);
}

SceneTest::~SceneTest()
{
}

void SceneTest::init()
{
}

void SceneTest::update()
{
	m_frameCount++;

	int pad = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	if (pad & PAD_INPUT_1)
	{
		m_type = kTypeDfs;
	}
	else if (pad & PAD_INPUT_2)
	{
		m_type = kTypeBfs;
	}
	else if (pad & PAD_INPUT_3)
	{
		m_type = kTypeAStar;
	}

	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	int mouseIndexX = mouseX / kGridSize;
	int mouseIndexY = mouseY / kGridSize;

	clearCount();

	m_searchTime = GetNowHiPerformanceCount();
	m_saikiCount = 0;
	switch (m_type)
	{
	case kTypeDfs:
		setCount(mouseIndexX, mouseIndexY, m_moveCount);	// DFS
		break;
	case kTypeBfs:
		setCountLoop(mouseIndexX, mouseIndexY, m_moveCount * 4);	// BFS
		break;
	case kTypeAStar:
		setCountAstar(mouseIndexX, mouseIndexY, m_moveCount * 4);	// A*
		break;
	}
	m_searchTime = GetNowHiPerformanceCount() - m_searchTime;
}

void SceneTest::draw()
{
	for (int x = 0; x < kGridNumX; x++)
	{
		for (int y = 0; y < kGridNumY; y++)
		{
			int posX = x * kGridSize;
			int posY = y * kGridSize;
			DrawBox(posX, posY, posX + kGridSize, posY + kGridSize, 0xffffff, false);
			
			if (m_fieldTbl[y][x] < 0)
			{
				DrawBox(posX, posY, posX + kGridSize, posY + kGridSize, 0xff0000, true);
			}

			if (m_countTbl[y][x] >= 0)
			{
				float alpha = static_cast<float>(m_countTbl[y][x]) / m_moveCount;
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128*alpha + 120);
				DrawBox(posX, posY, posX + kGridSize, posY + kGridSize, 0xffff00, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			//	DrawFormatString(posX, posY, 0xffffff, "%d", m_countTbl[y][x]);
			}
		}
	}

	// ゴール位置までのルート表示
//	if (m_frameCount % 16 >= 8)
	{
		int posX = m_goalX * kGridSize;
		int posY = m_goalY * kGridSize;
		DrawBox(posX, posY, posX + kGridSize, posY + kGridSize, 0xff00ff, true);

		DrawLine(posX, kGridSize * kGridNumY, posX, kGridSize * kGridNumY+32, 0xff0000 );
		DrawLine(kGridSize * kGridNumX, posY, kGridSize * kGridNumX + 32, posY, 0xff0000);

		// ゴールへのルートを表示する
		if (m_countTbl[m_goalY][m_goalX] >= 0)
		{
			drawRoute();
		}
	}

	switch (m_type)
	{
	case kTypeDfs:
		DrawFormatString(1080, 16, 0xffffff, "DFS 深さ優先探索");
		break;
	case kTypeBfs:
		DrawFormatString(1080, 16, 0xffffff, "BFS 幅優先探索");
		break;
	case kTypeAStar:
		DrawFormatString(1080, 16, 0xffffff, "A*");
		break;
	}

	DrawFormatString(1080, 32, 0xffffff, "FPS:%f", GetFPS());
	DrawFormatString(1080, 48, 0xffffff, "探索回数:%d", m_saikiCount);
	DrawFormatString(1080, 64, 0xffffff, "合計探索時間:%d us", m_searchTime);
	

	DrawFormatString(1080, 128, 0xffffff, "Z:DFS 深さ優先探索 に切り替え");
	DrawFormatString(1080, 144, 0xffffff, "X:BFS 幅優先探索 に切り替え");
	DrawFormatString(1080, 160, 0xffffff, "C:A* に切り替え");


	DrawFormatString(1080, 320, 0xffffff, "■DFS 深さ優先探索");
	DrawFormatString(1080, 336, 0xffffff, "・大雑把に言うと全検索");
	DrawFormatString(1080, 352, 0xffffff, "・再帰を利用して実装する");
	DrawFormatString(1080, 368, 0xffffff, "・スタックオーバーフローに気をつける");
	DrawFormatString(1080, 384, 0xffffff, "・しっかり枝狩りすることを意識する");
	DrawFormatString(1080, 400, 0xffffff, "・検索範囲が狭ければこっちでゴリ押せる");

	DrawFormatString(1080, 480, 0xffffff, "■BFS 幅優先探索");
	DrawFormatString(1080, 496, 0xffffff, "・最短経路を求めたい場合はこっち");
	DrawFormatString(1080, 512, 0xffffff, "・全パターンを…という場合はこっちは使えない");

	DrawFormatString(1080, 560, 0xffffff, "■A* ");
	DrawFormatString(1080, 576, 0xffffff, "・こっちの方が近そう、というルートを優先探索");
	DrawFormatString(1080, 592, 0xffffff, "・ゴールまで直線でつないでそれに近いルートを優先する");
	DrawFormatString(1080, 608, 0xffffff, "・マップの形状によってはBFSより効率が下がる");
}

void SceneTest::clearCount()
{
	for (int x = 0; x < kGridNumX; x++)
	{
		for (int y = 0; y < kGridNumY; y++)
		{
			m_countTbl[y][x] = -1;
		}
	}
}

void SceneTest::setCount(int x, int y, int count)
{
	m_saikiCount++;

	// これ以上移動しない
	if (count < 0)	return;

	// 範囲外のチェック
	if (x < 0)	return;
	if (y < 0)	return;
	if (x >= kGridNumX)	return;
	if (y >= kGridNumY)	return;
	// 移動できない地形
	if (m_fieldTbl[y][x] < 0)	return;
	// もっと少ない手数で移動可能なルートが見つかっている
	if (m_countTbl[y][x] >= count)	return;

	m_countTbl[y][x] = count;
	setCount(x + 1, y, count - 1);
	setCount(x - 1, y, count - 1);
	setCount(x, y + 1, count - 1);
	setCount(x, y - 1, count - 1);
}

void SceneTest::setCountLoop(int startX, int startY, int startCount)
{
	// 範囲外のチェック
	if (startX < 0)	return;
	if (startY < 0)	return;
	if (startX >= kGridNumX)	return;
	if (startY >= kGridNumY)	return;

	struct Node { int x, y, c; };
	std::queue<Node> que;

	que.push({ startX, startY, startCount });
	m_countTbl[startY][startX] = startCount;

	while (!que.empty()) 
	{
		Node curr = que.front(); que.pop();
		m_saikiCount++; // カウント用

		int dx[] = { 1, -1, 0, 0 }, dy[] = { 0, 0, 1, -1 };
		for (int i = 0; i < 4; i++) 
		{
			// 次のチェック情報
			int nx = curr.x + dx[i];
			int ny = curr.y + dy[i];
			int nc = curr.c - 1;

			// 範囲外はチェックしない
			if (nx < 0 || nx >= kGridNumX || ny < 0 || ny >= kGridNumY) continue;
			// 移動不可能は地形はチェックしない
			if (m_fieldTbl[ny][nx] < 0 || nc < 0) continue;

			// すでにこれ以上の歩数で到達可能ならスキップ
			if (m_countTbl[ny][nx] >= nc) continue;

			m_countTbl[ny][nx] = nc;

			// ゴールが見つかったら強制終了
			if (ny == m_goalY && nx == m_goalX)	return;

			que.push({ nx, ny, nc });
		}
	}
}

void SceneTest::setCountAstar(int startX, int startY, int startCount)
{
	// 範囲外のチェック
	if (startX < 0)	return;
	if (startY < 0)	return;
	if (startX >= kGridNumX)	return;
	if (startY >= kGridNumY)	return;

	struct Node {
		int x;
		int y;
		int c;	// 実コスト		開始地点からの移動コスト
		int h;	// 推定コスト	ゴールまでの推定コスト(マンハッタン距離)
		int s;	// スコア		実コスト+推定コスト

		bool operator>(const Node& other) const {
			return s > other.s;
		}
	};

	// 開始地点のノード
	Node n;
	n.x = startX;
	n.y = startY;
	n.c = 0;
	n.h = abs(startX - m_goalX) + abs(startY - m_goalY);
	n.s = n.c + n.h;

	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> que;

	que.push(n);
	m_countTbl[startY][startX] = startCount;

	while (!que.empty())
	{
		Node curr = que.top();
		que.pop();

		m_saikiCount++; // カウント用

		// すでによりよいデータが存在している
	//	if (curr.s > m_countTbl[curr.y][curr.x])	continue;

		int dx[] = { 1, -1, 0, 0 }, dy[] = { 0, 0, 1, -1 };
		for (int i = 0; i < 4; i++)
		{
			// 次のチェック情報
			int nx = curr.x + dx[i];
			int ny = curr.y + dy[i];
		//	int nc = curr.c - 1;
			int nc = m_countTbl[curr.y][curr.x] - 1;

			// 範囲外はチェックしない
			if (nx < 0 || nx >= kGridNumX || ny < 0 || ny >= kGridNumY) continue;
			// 移動不可能は地形はチェックしない
			if (m_fieldTbl[ny][nx] < 0 || nc < 0) continue;

			// すでにこれ以上の歩数で到達可能ならスキップ
			if (m_countTbl[ny][nx] >= nc) continue;

			m_countTbl[ny][nx] = nc;

			// ゴールが見つかったら強制終了
			if (ny == m_goalY && nx == m_goalX)	return;

			Node n;
			n.x = nx;
			n.y = ny;
			n.c = curr.c+1;
			n.h = abs(nx - m_goalX) + abs(ny - m_goalY);
			n.s = n.c + n.h;

			que.push(n);
		}
	}
}

void SceneTest::drawRoute()
{
	// ゴールまでの歩数がわからない
	if (m_countTbl[m_goalY][m_goalX] < 0)	return;

	int indexX = m_goalX;
	int indexY = m_goalY;
	int count = m_countTbl[m_goalY][m_goalX];

	int move = 0;

//	while (count < m_moveCount)
	while(1)
	{
		int posX = indexX * kGridSize;
		int posY = indexY * kGridSize;
		DrawBox(posX, posY, posX + kGridSize, posY + kGridSize, 0xff00ff, true);

		count++;
		move++;
		if (indexX > 0 && m_countTbl[indexY][indexX - 1] == count)
		{
			indexX--;
		}
		else if (indexX < (kGridNumX - 1) && m_countTbl[indexY][indexX + 1] == count)
		{
			indexX++;
		}
		else if (indexY > 0 && m_countTbl[indexY-1][indexX] == count)
		{
			indexY--;
		}
		else if (indexY < (kGridNumY - 1) && m_countTbl[indexY + 1][indexX] == count)
		{
			indexY++;
		}
		else
		{
			break;
		}
	}

	DrawFormatString(1080, 80, 0xffffff, "歩数:%d", move);
}
