#include <Scene/GameScene.h>

std::vector<std::shared_ptr<object>> GameScene::_objList;

GameScene::GameScene()
{
}

GameScene::GameScene(int stage)
{
	// 初期化
	_keyOldR = false;
	StageMngIns.UpdateStagecount(stage);
	_update = &GameScene::animUpdate;
	_plMove = &GameScene::moveFall;
	_result = &GameScene::moveResult;
	_count	= 0;
	_animCount = 0;
	_theta = 0;
	_keySpaceOld = 1;
	_stageCount = stage;

	_resultScreen = MakeScreen(704, 544, false);

	// アニメーション
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::LEFT, 10));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::RIGHT, 10));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::LEFT, 10));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::RIGHT, 9));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::LEFT, 8));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::RIGHT, 7));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::LEFT, 6));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::RIGHT, 5));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::LEFT, 2));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::RIGHT, 2));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::LEFT, 2));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::RIGHT, 2));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::LEFT, 2));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::RIGHT, 2));
	_LRAnim.emplace_back(std::make_pair(OBJ_STATS::MAX, -1));	 // 終了
}


GameScene::~GameScene()
{
}

Base_unq GameScene::Update(Base_unq scene)
{
	// 最後まで終わったらステージ選択へ
	if ((this->*_update)())
	{
		scene = std::make_unique<StageSelectScene>();
		_objList.clear();
	}

	return std::move(scene);
}

bool GameScene::clearCheck(void)
{
	auto pl_data = std::find_if(_objList.begin(), _objList.end(), [](std::shared_ptr<object> data) { return data->getType() == OBJ_TYPE::PLAYER; });
	if (pl_data == _objList.end())
	{
		AST();
		return false;
	}

	if (StageMngIns.getStageData({ static_cast<int>((*pl_data)->getPos().x + (*pl_data)->getSize().x / 2), static_cast<int>((*pl_data)->getPos().y + (*pl_data)->getSize().y / 2) }) == 1 ||
		StageMngIns.getStageData({ static_cast<int>((*pl_data)->getPos().x + (*pl_data)->getSize().x / 2), static_cast<int>((*pl_data)->getPos().y + (*pl_data)->getSize().y / 2) }) == 3)
	{
		return true;
	}
	return false;
}

bool GameScene::moveFall(void)
{
	auto pl_data = std::find_if(_objList.begin(), _objList.end(), [](std::shared_ptr<object> data) { return data->getType() == OBJ_TYPE::PLAYER; });

	(*pl_data)->setPos({ (*pl_data)->getPos().x, (*pl_data)->getPos().y + 20.0 });
	_count++;

	if (_count >= 40)
	{
		_update = &GameScene::objUpdate;
		ImageMngIns.setEffect(EFFECT::SMOKE, { static_cast<int>((*pl_data)->getPos().x) + CubeSize / 2,static_cast<int>((*pl_data)->getPos().y) + CubeSize / 2 });
		_startTime = steady_clock::now();
	}
	return false;
}

bool GameScene::moveUp(void)
{
	auto pl_data = std::find_if(_objList.begin(), _objList.end(), [](std::shared_ptr<object> data) { return data->getType() == OBJ_TYPE::PLAYER; });

	(*pl_data)->setPos({ (*pl_data)->getPos().x, (*pl_data)->getPos().y - 20.0 });
	(*pl_data)->setState(static_cast<OBJ_STATS>(_count % 2));
	_count++;

	if (_count >= 80)
	{
		_update = &GameScene::resultUpdate;
		_count = 0;
	}
	return false;
}

bool GameScene::moveLR(void)
{
	auto pl_data = std::find_if(_objList.begin(), _objList.end(), [](std::shared_ptr<object> data) { return data->getType() == OBJ_TYPE::PLAYER; });

	(*pl_data)->setState(_LRAnim[_animCount].first);
	_count++;
	if (_count >= _LRAnim[_animCount].second)
	{
		_count = 0;
		_animCount++;
		if (_LRAnim[_animCount].first == OBJ_STATS::MAX)
		{
			_plMove = &GameScene::moveUp;
		}
	}

	return false;
}

bool GameScene::moveResult(void)
{
	_theta += 2;
	if (_theta >= 90)
	{
		_result = &GameScene::resultScene;
	}
	return false;
}

bool GameScene::resultScene(void)
{
	if ((CheckHitKey(KEY_INPUT_SPACE) && (!_keySpaceOld)) || ((SceneMngIns.GetPad() & PAD_INPUT_2) != 0 && (SceneMngIns.GetPadOld() & PAD_INPUT_2) == 0))
	{
		return true;
	}

	_keySpaceOld = CheckHitKey(KEY_INPUT_SPACE);
	return false;
}

bool GameScene::objUpdate(void)
{
	for (auto data : _objList)
	{
		data->Update();
	}

	if (CheckHitKey(KEY_INPUT_R) && (!_keyOldR) || ((SceneMngIns.GetPad() & PAD_INPUT_10) != 0 && (SceneMngIns.GetPadOld() & PAD_INPUT_10) == 0))
	{
		StageMngIns.resetObj();
		_startTime = steady_clock::now();
	}
	_keyOldR = CheckHitKey(KEY_INPUT_R);

	if (clearCheck())
	{
		_update = &GameScene::animUpdate;
		_plMove = &GameScene::moveLR;
		_count = 0;

		_clearTime = steady_clock::now();
		int time = static_cast<int>(duration_cast<milliseconds>(_clearTime - _startTime).count());

		RankMngIns.checkRank(_stageCount, time);

		int tmpScreen = RankMngIns.getRankScreen(_stageCount);
		SetDrawScreen(_resultScreen);
		ClsDrawScreen();
		DrawGraph(0, 0, ImageMngIns.getImage("clearFlame")[0], false);
		DrawGraph(165, 310, tmpScreen, true);

		DrawRotaGraph(CLEAR_TIME_OFFSET_X + (NUM_SIZE_X + 8) * 0, CLEAR_TIME_OFFSET_Y, 2.0, 0.0, ImageMngIns.getImage("number")[time / (MIN * 10)], true);
		DrawRotaGraph(CLEAR_TIME_OFFSET_X + (NUM_SIZE_X + 8) * 1, CLEAR_TIME_OFFSET_Y, 2.0, 0.0, ImageMngIns.getImage("number")[time / MIN % 10], true);
		DrawRotaGraph(CLEAR_TIME_OFFSET_X + (NUM_SIZE_X + 8) * 2, CLEAR_TIME_OFFSET_Y, 2.0, 0.0, ImageMngIns.getImage("number")[10], true);
		DrawRotaGraph(CLEAR_TIME_OFFSET_X + (NUM_SIZE_X + 8) * 3, CLEAR_TIME_OFFSET_Y, 2.0, 0.0, ImageMngIns.getImage("number")[time % MIN / (SEC * 10)], true);
		DrawRotaGraph(CLEAR_TIME_OFFSET_X + (NUM_SIZE_X + 8) * 4, CLEAR_TIME_OFFSET_Y, 2.0, 0.0, ImageMngIns.getImage("number")[time % MIN / SEC % 10], true);
		DrawRotaGraph(CLEAR_TIME_OFFSET_X + (NUM_SIZE_X + 8) * 5, CLEAR_TIME_OFFSET_Y, 2.0, 0.0, ImageMngIns.getImage("number")[10], true);
		DrawRotaGraph(CLEAR_TIME_OFFSET_X + (NUM_SIZE_X + 8) * 6, CLEAR_TIME_OFFSET_Y, 2.0, 0.0, ImageMngIns.getImage("number")[time % SEC / 100], true);
		DrawRotaGraph(CLEAR_TIME_OFFSET_X + (NUM_SIZE_X + 8) * 7, CLEAR_TIME_OFFSET_Y, 2.0, 0.0, ImageMngIns.getImage("number")[time % 100 / 10], true);
		DrawRotaGraph(CLEAR_TIME_OFFSET_X + (NUM_SIZE_X + 8) * 8, CLEAR_TIME_OFFSET_Y, 2.0, 0.0, ImageMngIns.getImage("number")[time % 10], true);


		//DrawGraph(CLEAR_TIME_OFFSET_X + NUM_SIZE_X * 0, CLEAR_TIME_OFFSET_Y, ImageMngIns.getImage("number")[time / (MIN * 10)], true);
		//DrawGraph(CLEAR_TIME_OFFSET_X + NUM_SIZE_X * 1, CLEAR_TIME_OFFSET_Y, ImageMngIns.getImage("number")[time / MIN % 10], true);
		//DrawGraph(CLEAR_TIME_OFFSET_X + NUM_SIZE_X * 2, CLEAR_TIME_OFFSET_Y, ImageMngIns.getImage("number")[10], true);
		//DrawGraph(CLEAR_TIME_OFFSET_X + NUM_SIZE_X * 3, CLEAR_TIME_OFFSET_Y, ImageMngIns.getImage("number")[time % MIN / (SEC * 10)], true);
		//DrawGraph(CLEAR_TIME_OFFSET_X + NUM_SIZE_X * 4, CLEAR_TIME_OFFSET_Y, ImageMngIns.getImage("number")[time % MIN / SEC % 10], true);
		//DrawGraph(CLEAR_TIME_OFFSET_X + NUM_SIZE_X * 5, CLEAR_TIME_OFFSET_Y, ImageMngIns.getImage("number")[10], true);
		//DrawGraph(CLEAR_TIME_OFFSET_X + NUM_SIZE_X * 6, CLEAR_TIME_OFFSET_Y, ImageMngIns.getImage("number")[time % SEC / 100], true);
		//DrawGraph(CLEAR_TIME_OFFSET_X + NUM_SIZE_X * 7, CLEAR_TIME_OFFSET_Y, ImageMngIns.getImage("number")[time % 100 / 10], true);
		//DrawGraph(CLEAR_TIME_OFFSET_X + NUM_SIZE_X * 8, CLEAR_TIME_OFFSET_Y, ImageMngIns.getImage("number")[time % 10], true);
	}

	Draw();

	if ((SceneMngIns.GetPad() & PAD_INPUT_10) != 0 && (SceneMngIns.GetPad() & PAD_INPUT_5) != 0 && (SceneMngIns.GetPad() & PAD_INPUT_6) != 0)
	{
		_objList.clear();
		return true;
	}

	return false;
}

bool GameScene::animUpdate(void)
{
	(this->*_plMove)();
	Draw();

	return false;
}

bool GameScene::resultUpdate(void)
{
	bool tmpBool = (this->*_result)();

	Draw();
	ImageMngIns.AddDraw({ _resultScreen, SceneMngIns.ScreenCenter.x, static_cast<int>(sin(RAD(_theta)) * RESULT_MOVE_OFFSET - 300), 0.0, LAYER::UI, 30000 });
	return tmpBool;
}

void GameScene::Draw(void)
{
	for (auto data : _objList)
	{
		data->objDraw();
	}
	StageMngIns.Update();
}
