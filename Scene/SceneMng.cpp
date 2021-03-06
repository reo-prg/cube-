#include <DxLib.h>
#include <Scene/SceneMng.h>

SceneMng* SceneMng::sInstance = nullptr;

void SceneMng::Run(void)
{
	SystemInit();
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		UpdatePad();

		_runScene = _runScene->Update(std::move(_runScene));

		ImageMngIns.Draw();
	}
}

int SceneMng::GetPad(void)const
{
	return _padInput;
}

int SceneMng::GetPadOld(void)const
{
	return _padInputOld;
}

Vector2Template<int> SceneMng::GetStick(void)const
{
	return _stick;
}

Vector2Template<int> SceneMng::GetStickOld(void)const
{
	return _stickOld;
}

void SceneMng::StartVib(int pad, int power, int time)const
{
	StartJoypadVibration(pad, power, time);
}

bool SceneMng::SystemInit(void)
{
	SetWindowText("cube!");
	SetGraphMode(SceneMngIns.ScreenSize.x, SceneMngIns.ScreenSize.y, 16);			// 1024 * 764ドット、65536色モードに設定
	ChangeWindowMode(true);															// true:window false:フルスクリーン

	if (DxLib_Init() == -1)															// Dxライブラリの初期化
	{
		return false;
	}
	SetDrawScreen(DX_SCREEN_BACK);		// 描画先をバックバッファに設定

	// 画像読み込み
	ImageMngIns.getImage("image/TitleLogo.png", "logo");
	ImageMngIns.getImage("image/TitleMes.png", "TitleMes");
	ImageMngIns.getImage("image/SelectMes.png", "SelectMes", 700, 64, 1, 3);
	ImageMngIns.getImage("image/guide.png", "guide", 256, 64, 1, 4);
	ImageMngIns.getImage("image/cursor.png", "cursor");
	ImageMngIns.getImage("image/char_flame.png", "c_flame");
	ImageMngIns.getImage("image/name_char.png", "char", 32, 32, 10, 10);
	ImageMngIns.getImage("image/stage.png", "stage", 128, 96, 4, 2);
	ImageMngIns.getImage("image/stageflame.png", "s_flame");
	ImageMngIns.getImage("image/rank.png", "rank");
	ImageMngIns.getImage("image/flame.png", "flame");
	ImageMngIns.getImage("image/back.png", "back");
	ImageMngIns.getImage("image/stage_chip.png", "s_cube", 32, 32, 8, 8);
	ImageMngIns.getImage("image/cube.png", "cube", 32, 32, 3, 1);
	ImageMngIns.getImage("image/char.png", "player", 64, 32, 2, 8);
	ImageMngIns.getImage("image/clearFlame.png", "clearFlame");
	ImageMngIns.getImage("image/number.png", "number", 24, 32, 11, 1);

	// ゲームパッドの状態の初期化
	_stick = { 0,0 };
	_padInput = 0;
	_stickOld = { 0,0 };
	_padInputOld = INT_MAX;

	// 最初のシーンのを設定
	_runScene = std::make_unique<TitleScene>();

	return true;
}

void SceneMng::UpdatePad(void)
{
	_padInputOld = _padInput;
	_stickOld = _stick;
	_padInput = GetJoypadInputState(DX_INPUT_PAD1);
	GetJoypadAnalogInput(&_stick.x, &_stick.y, DX_INPUT_PAD1);
}

SceneMng::SceneMng() :ScreenSize{ 1024,768 }, ScreenCenter(ScreenSize / 2)
{
}


SceneMng::~SceneMng()
{
}
