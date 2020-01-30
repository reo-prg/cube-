#pragma once
#include <DxLib.h>
#include <_debug/_DebugConOut.h>
#include <common/Vector2.h>
#include <Graphic/ImageMng.h>

#define RankMngIns RankMng::getInstance()

#define STAGE_COUNT	8		// ステージの数
#define RANK_COUNT	3		// ランクの数

#define MIN			60000	// ミリ秒から分への変換用
#define SEC			1000	// ミリ秒から秒への変換用

#define NUM_SIZE_X	32		// 番号画像のXの大きさ
#define RANK_OFFSET	20		// ランク画像のオフセット
#define RANK_DUR	64		// ランク間の間隔
#define TIME_MAX	5999999	// 時間の最大値

class RankMng
{
public:

	static RankMng& getInstance()
	{
		if (sInstance == nullptr)
		{
			sInstance = new RankMng();
		}
		return *sInstance;
	}

	static void Destroy(void)
	{
		if (sInstance != nullptr)
		{
			delete sInstance;
		}
		sInstance = nullptr;
	}

	void RankInit(void);	// ランキングの読み取り

	int getClearTime(int stage, int rank);	// クリアタイムの取得
	int getRankScreen(int stage);			// ランキングのスクリーンを取得
	int checkRank(int stage, int clearTime);			// クリアタイムがランキングに入っているのかをチェック

private:
	static RankMng* sInstance;

	int _clearTimeRank[STAGE_COUNT][RANK_COUNT];	// クリアタイムの格納
	int _rankScreen;								// ランクが描画されているスクリーン

	void saveClearTime(void);

	RankMng();
	~RankMng();
};

