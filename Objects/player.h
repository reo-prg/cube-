#pragma once
#include <Objects/object.h>
#include <Graphic/ImageMng.h>
#include <StageMng.h>
#include <func/CanGripCube.h>
#include <Objects/CHECK_DIR.h>
#include <func/CheckHitObj.h>

#define PL_G_ACC	0.5		// プレイヤーの重力加速度
#define JUMP_DELEY	3		// ジャンプの間隔(時間)
#define PL_SPEED	4		// プレイヤーのはやさ


class player :
	public object
{
public:
	player();
	player(Vector2Template<double> pos, double rad, Vector2Template<int> size, int color);
	~player();

	void Update(void)override;
private:
	double _initVel;					// 初速
	int _jumpDeley;						// 次ジャンプできるようになるための間隔
	std::shared_ptr<object> _gripCube;	// 掴んでいるキューブ

	void control(void);
	void velUpdate(void);
	bool CheckHitCube(CHECK_DIR);
};

