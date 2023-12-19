
#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER

//-------------------------------------------------------------------------

#include "Play.h"
#include "Headers.h"
#include "Variables.h"
#include "Structs.h"
#include "MainHeaders.h"

using namespace std;


Flags flags;
Timers t;
GameState gameState;

Platform platform1;
Platform platform2;
Platform platform3;
Platform platform4;
Platform platform5;

std::vector<std::string> levelList;


// The entry point for a PlayBuffer program
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground("Data//Backgrounds//forest1.png");
	Play::LoadBackground("Data//Backgrounds//forest3.png");
	Play::LoadBackground("Data//Backgrounds//forest2.png");
	Play::LoadBackground("Data//Backgrounds//forest.png");

	SpriteOrigins();
	CreateLobby();
	CreateLevelList();
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	UpdateGameStates();

	CameraControl();
	WalkingDurationControl(elapsedTime);
	AnimationDurationControl(elapsedTime);
	UpdateDestroyed();

	return Play::KeyDown(VK_ESCAPE);
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

void SpriteOrigins()
{
	//Play::MoveSpriteOrigin("cat_go_right", 0, -3);
	//Play::MoveSpriteOrigin("cat_go_left", 0, -3);
	Play::MoveSpriteOrigin("cat_poop_right", 0, -1);
	Play::MoveSpriteOrigin("cat_poop_left", 0, -1);
	Play::MoveSpriteOrigin("cat_hiss_new", 0, -2);
	Play::MoveSpriteOrigin("cat_hiss_new_left", 0, -2);
	Play::MoveSpriteOrigin("cat_walk_right", 0, -2);
	Play::MoveSpriteOrigin("cat_walk_left", 0, -2);

	Play::MoveSpriteOrigin("cat_sits_right", 0, 1);
	Play::MoveSpriteOrigin("cat_sits_left", 0, 1);

	Play::MoveSpriteOrigin("cat_walk_dontlook_right", 0, -2);
	Play::MoveSpriteOrigin("cat_walk_look_right", 0, -2);
	Play::MoveSpriteOrigin("cat_walk_dontlook_left", 0, -2);
	Play::MoveSpriteOrigin("cat_walk_look_left", 0, -2);
	Play::MoveSpriteOrigin("cat_look_left", 0, -2);
	Play::MoveSpriteOrigin("cat_look_left", 0, -2);

	Play::MoveSpriteOrigin("cat_hairball_right", 0, -2);
	Play::MoveSpriteOrigin("cat_hairball_left", 0, -2);
	Play::MoveSpriteOrigin("cat_lick_left", 0, -2);
	Play::MoveSpriteOrigin("cat_lick_left", 0, -2);

	Play::MoveSpriteOrigin("cat_reborn_right", 0, 20);
	Play::MoveSpriteOrigin("cat_reborn_left", 0, 20);
}

void CreateLobby()
{
	int id = Play::CreateGameObject(TYPE_LOBBY_CAT, { DISPLAY_WIDTH - 155, DISPLAY_HEIGHT - 145 }, 10, "cat_sits_left");
	GameObject& objCat = Play::GetGameObject(id);
	objCat.velocity = PLAYER_VELOCITY_DEFAULT;
	objCat.scale = 4.5f;
	objCat.animSpeed = 0.1f;

	for (int i = 0; i < LOBBY_FLEA_COUNT; i++)
	{
		CreateLobbyFlea();
	}
}

void CreateLobbyFlea()
{
	int id = Play::CreateGameObject(TYPE_LOBBY_FLEA, { Randomize(DISPLAY_WIDTH), Randomize(DISPLAY_HEIGHT) }, 10, "bug_up");
	GameObject& objFlea = Play::GetGameObject(id);
	objFlea.rotation = Randomize(628, 0.1f);
	objFlea.state = STATE_WALK;
	objFlea.scale = 0.5f;
	objFlea.animSpeed = 0.1f;
	objFlea.rotSpeed = Randomize(10, 0.001f);
	objFlea.speed = Randomize(30, 0.1f);

	if (objFlea.speed < 1.0f)
		objFlea.speed = 1.f;
}

// camera movement 
void CameraControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	Point2D cameraPos = { 0, 0 };

	if (objPlayer.pos.y < CAMERA_THRESHOLD_Y * TILE_SIZE)
	{
		cameraPos.y -= CAMERA_THRESHOLD_Y * TILE_SIZE - objPlayer.pos.y;
		cameraPos.x = objPlayer.pos.x - CAMERA_OFFSET_X;
	}
	/*else if (objPlayer.pos.x < CAMERA_THRESHOLD_X.x * TILE_SIZE)
		cameraPos.x = leftLimit - TILE_SIZE;
	else if (objPlayer.pos.x > CAMERA_THRESHOLD_X.y * TILE_SIZE)
		cameraPos.x = rightLimit - (TILE_WIDTH * TILE_SIZE) + 5;*/
	else
		cameraPos = { objPlayer.pos.x - CAMERA_OFFSET_X, 0 };

	if (gameState.state != STATE_PLAY)
		cameraPos = { 0, 0 };

	Play::SetCameraPosition(cameraPos);
}

// set player pos y when grounded
void SetPlayerPos(int pos)
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	(flags.isGroundFloor) ?
		objPlayer.pos.y = PLAYER_START_POS.y * TILE_SIZE - PLAYER_OFFSET_Y :
		objPlayer.pos.y = pos - PLAYER_OFFSET_Y;

	if (objPlayer.pos.y > BOTTOM_LINE)
		objPlayer.pos.y = BOTTOM_LINE;
}

// set the floor according to the player pos
void SetFloor()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	(objPlayer.pos.y >= PLAYER_START_POS.y * TILE_SIZE - PLAYER_OFFSET_Y) ?
		flags.isGroundFloor = true : flags.isGroundFloor = false;

	if (objPlayer.oldPos.y <= platform1.SIXTH_FLOOR_HEIGHT * TILE_SIZE)
		gameState.floor = 6;
	else if (objPlayer.oldPos.y <= platform1.FIFTH_FLOOR_HEIGHT * TILE_SIZE - TILE_SIZE - PLAYER_OFFSET_Y)
		gameState.floor = 5;
	else if (objPlayer.oldPos.y <= platform1.FOURTH_FLOOR_HEIGHT * TILE_SIZE - TILE_SIZE - PLAYER_OFFSET_Y)
		gameState.floor = 4;
	else if (objPlayer.oldPos.y <= platform1.THIRD_FLOOR_HEIGHT * TILE_SIZE - TILE_SIZE - PLAYER_OFFSET_Y)
		gameState.floor = 3;
	else if (objPlayer.oldPos.y <= platform1.SECOND_FLOOR_HEIGHT * TILE_SIZE - TILE_SIZE * 2)
		gameState.floor = 2;
	else if (objPlayer.oldPos.y <= platform1.GROUND_FLOOR_HEIGHT * TILE_SIZE - TILE_SIZE)
		gameState.floor = 1;
}

void PoopControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (gameState.treats > 2 && gameState.floor < 6)
	{
		t.poopTimer += DELTA_TIME;

		if (t.poopTimer / DELTA_TIME < 10 && t.poopTimer > 0.f)
		{
			(flags.right) ? Play::SetSprite(objPlayer, "cat_poop_right", 0.05f) : Play::SetSprite(objPlayer, "cat_poop_left", 0.05f);
		}
		else
		{
			gameState.treats = 0;
			t.poopTimer = 0.f;
			int id = Play::CreateGameObject(TYPE_POO, { objPlayer.pos.x, objPlayer.pos.y + POO_OFFSET_Y }, 10, "the_poo");
			GameObject& poo = Play::GetGameObject(id);
			poo.scale = 2.f;
			poo.animSpeed = 0.1f;

			(flags.right) ? Play::SetSprite(objPlayer, "cat_sits_right", 0.1f) : Play::SetSprite(objPlayer, "cat_sits_left", 0.1f);
		}
	}
}

// update game states
void UpdateGameStates()
{
	switch (gameState.state)
	{
		case STATE_PLAY:
		{
			UpdatePlayer();
			UpdateFleas();
			UpdateTreats();
			UpdatePoo();
			UpdateFlies();
			UpdateWater();
			UpdateStars();
			UpdateBox();
			Draw();
			break;
		}
		case STATE_PAUSE:
		{
			DrawLobby();
			if (Play::KeyDown(VK_RETURN))
			{
				gameState.state = STATE_PLAY;
			}
			break;
		}
		case STATE_LOBBY:
		{
			DrawLobby();

			if (Play::KeyDown(VK_RETURN))
			{
				LoadLevel();
				gameState.state = STATE_PLAY;
			}
			break;
		}
		case STATE_GAMEOVER:
		{
			DrawLobby();
			if (Play::KeyDown(VK_RETURN))
			{
				gameState.level = 1;
				RestartGame();
				LoadLevel();
				gameState.state = STATE_PLAY;
			}
			break;
		}
		case STATE_VICTORY:
		{
			DrawVictory();
			break;
		}
	}

	if (flags.levelPassed)
	{
		if (gameState.level == levelList.size())
		{
			gameState.state = STATE_VICTORY;
			flags.levelPassed = false;
			return;
		}

    	gameState.level++;
		RestartGame();
		LoadLevel();
		gameState.state = STATE_PLAY;
		flags.levelPassed = false;
	}

	if (Play::KeyDown(VK_BACK))
	{
		gameState.state = STATE_PAUSE;
	}

	if (Play::KeyDown(VK_TAB))
	{
		gameState.level = 1;
		RestartGame();
		CreateLobby();
		gameState.state = STATE_LOBBY;
	}
}

// update game characters
void UpdatePlayer()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	SetFloor();

	switch (gameState.playerState)
	{
	case STATE_GROUNDED:
	{
		IdlePlayerControls();
		PoopControl();
		break;
	}
	case STATE_WALK:
	{
		WalkingPlayerControls();
		break;
	}
	case STATE_FALL:
	{
		Fall();
		break;
	}
	case STATE_JUMP:
	{
		Jump();
		break;
	}
	case STATE_ATTACHED:
	{
		AttachedPlayerControls();

		break;
	}
	case STATE_HISS:
	{
		Hiss();
		break;
	}
	case STATE_FURBALL:
	{
		FurballPlayerControls();
		break;
	}
	case STATE_REBORN:
	{
		Reborn();
		break;
	}
	}

	if (Play::KeyDown(VK_SHIFT) && !IsPlayerCollidingAnyPlatformUpper())
	{
		gameState.playerPrevState = gameState.playerState;
		gameState.playerState = STATE_FURBALL;
	}

	Play::UpdateGameObject(objPlayer);
}

void UpdateFleas()
{
	vector <int> vFleas = Play::CollectGameObjectIDsByType(TYPE_FLEA);
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);


	for (int fleaId : vFleas)
	{
		GameObject& flea = Play::GetGameObject(fleaId);

		if (IsPlayerCollidingUpperPart(flea) && gameState.playerState == STATE_FURBALL)
		{
			flea.pos = FLEA_START_POS;
			flea.state = STATE_FALL;
		}

		switch (flea.state)
		{
		case STATE_HIDE:
		{
			t.fleaTimer += DELTA_TIME;
			break;
		}
		case STATE_IDLE:
		{
			FleaAI(flea);
			break;
		}
		case STATE_WALK:
		{
			WalkingFlea(flea);
			break;
		}
		case STATE_FALL:
		{
			FallingFlea(flea);
			break;
		}
		case STATE_JUMP:
		{
			FleaJump(flea);
			break;
		}
		}
		Play::UpdateGameObject(flea);
	}
}

void UpdateFlies()
{
	//UpdateFlyByType(TYPE_RIGHT_FLY, RIGHT_FLY_START_POS, platform1.LIMIT_RIGHT);
	//UpdateFlyByType(TYPE_LEFT_FLY, LEFT_FLY_START_POS, platform1.LIMIT_LEFT);

	vector <int> vRFlies = Play::CollectGameObjectIDsByType(TYPE_RIGHT_FLY);
	for (int flyId : vRFlies)
	{
		GameObject& objFly = Play::GetGameObject(flyId);

		objFly.pos.x += FLY_SPEED;
		objFly.pos.y += sin(objFly.pos.x / 100) * 3;

		if (objFly.pos.x > platform1.LIMIT_RIGHT * TILE_SIZE)
			objFly.pos = RIGHT_FLY_START_POS * TILE_SIZE;

		if (IsPlayerCollidingUpperPart(objFly))
		{
			if (gameState.lives < 9)
				gameState.lives++;

			gameState.score += 100;
			Play::DestroyGameObject(flyId);
			flags.flyActive = false;
		}

		Play::UpdateGameObject(objFly);
	}

	vector <int> vLFlies = Play::CollectGameObjectIDsByType(TYPE_LEFT_FLY);
	for (int flyId : vLFlies)
	{
		GameObject& objFly = Play::GetGameObject(flyId);

		objFly.pos.x -= FLY_SPEED;
		objFly.pos.y += sin(objFly.pos.x / 100) * 3;

		if (objFly.pos.x < platform1.LIMIT_LEFT * TILE_SIZE)
			objFly.pos = LEFT_FLY_START_POS * TILE_SIZE;

		if (IsPlayerCollidingUpperPart(objFly))
		{
			if (gameState.lives < 9)
				gameState.lives++;

			gameState.score += 100;
			Play::DestroyGameObject(flyId);
			flags.flyActive = false;
		}

		Play::UpdateGameObject(objFly);
	}
}

void UpdateFlyByType(int TYPE, Point2D pos, int limit)
{
	vector <int> vLFlies = Play::CollectGameObjectIDsByType(TYPE);
	for (int flyId : vLFlies)
	{
		GameObject& objFly = Play::GetGameObject(flyId);

		(TYPE == TYPE_RIGHT_FLY) ? objFly.pos.x += FLY_SPEED : objFly.pos.x -= FLY_SPEED;
		objFly.pos.y += sin(objFly.pos.x / 100) * 3;

		if (objFly.pos.x < limit * TILE_SIZE)
			objFly.pos = pos * TILE_SIZE;

		if (IsPlayerCollidingUpperPart(objFly))
		{
			if (gameState.lives < 9)
				gameState.lives++;

			Play::DestroyGameObject(flyId);
		}
		Play::UpdateGameObject(objFly);
	}
}

void UpdateTreats()
{
	vector <int> vTreats = Play::CollectGameObjectIDsByType(TYPE_TREAT);
	vector <int> vFlowers = Play::CollectGameObjectIDsByType(TYPE_FLOWER);

	if (vTreats.size() == 0 && vFlowers.size() == 0)
	{
		flags.isBoxExit = true;
		GameObject& objBox = Play::GetGameObjectByType(TYPE_BOX);
		objBox.pos = gameState.exitPos;
		objBox.frame = 2;
	}

	for (int treatId : vTreats)
	{
		GameObject& treat = Play::GetGameObject(treatId);

		if (IsPlayerCollidingUpperPart(treat))
		{
			gameState.score += TREAT_SCORE;
			gameState.treats++;
			Play::DestroyGameObject(treatId);
		}
		Play::UpdateGameObject(treat);
	}

	for (int flowerId : vFlowers)
	{
		GameObject& flower = Play::GetGameObject(flowerId);

		if (IsPlayerCollidingUpperPart(flower))
		{
			gameState.score += FLOWER_SCORE;
			Play::DestroyGameObject(flowerId);
		}
		Play::UpdateGameObject(flower);
	}
}

void UpdatePoo()
{
	vector <int> vPoo = Play::CollectGameObjectIDsByType(TYPE_POO);
	vector <int> vLFlies = Play::CollectGameObjectIDsByType(TYPE_LEFT_FLY);
	vector <int> vRFlies = Play::CollectGameObjectIDsByType(TYPE_RIGHT_FLY);

	if (vPoo.size() > vLFlies.size() + vRFlies.size() && !flags.flyActive)
	{
		flags.flyActive = true;
		int random = Play::RandomRoll(2);

		if (random == 1)
		{
			int id = Play::CreateGameObject(TYPE_LEFT_FLY, LEFT_FLY_START_POS * TILE_SIZE, 10, "left_side_fly_motion");
			GameObject& objFly = Play::GetGameObject(id);
			objFly.scale = 2.5f;
			objFly.animSpeed = 0.1f;
		}
		else
		{
			int id = Play::CreateGameObject(TYPE_RIGHT_FLY, RIGHT_FLY_START_POS * TILE_SIZE, 10, "right_side_fly_motion");
			GameObject& objFly = Play::GetGameObject(id);
			objFly.scale = 2.5f;
			objFly.animSpeed = 0.1f;
		}
	}

	for (int pooId : vPoo)
	{
		GameObject& poo = Play::GetGameObject(pooId);

		if (IsPlayerCollidingUpperPart(poo) && gameState.playerState == STATE_FURBALL)
			poo.type = TYPE_DESTROYED;

		Play::UpdateGameObject(poo);
	}
}

void UpdateWater()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	GameObject& splash = Play::GetGameObjectByType(TYPE_SPLASH);

	if (t.splashTimer > 0.5f)
	{
		splash.type = TYPE_DESTROYED;
		flags.splashed = false;
		t.splashTimer = 0.f;
	}

	if (flags.splashed)
		t.splashTimer += DELTA_TIME;

	vector <int> vWater = Play::CollectGameObjectIDsByType(TYPE_WATER);
	for (int waterId : vWater)
	{
		GameObject& water = Play::GetGameObject(waterId);

		if (IsPlayerColliding(water))
		{
			int id = Play::CreateGameObject(TYPE_SPLASH, { water.pos.x + 20, water.pos.y - 40 }, 10, "splash");
			GameObject& splash = Play::GetGameObject(id);
			splash.scale = 2.5f;
			splash.animSpeed = 0.1f;
			flags.splashed = true;
			gameState.lives--;
			ResetPlayer();
			objPlayer.pos = objPlayer.rebornPos;

			(gameState.lives > 0) ? gameState.playerState = STATE_REBORN : gameState.playerState = STATE_DEAD;
		}

		Play::UpdateGameObject(water);
	}
	Play::UpdateGameObject(splash);
}

void UpdateStars()
{
	vector <int> vStars = Play::CollectGameObjectIDsByType(TYPE_STAR);

	for (int starId : vStars)
	{
		GameObject& star = Play::GetGameObject(starId);

		Play::UpdateGameObject(star);
	}
}

void UpdateBox()
{
	GameObject& objBox = Play::GetGameObjectByType(TYPE_BOX);

	if (t.animationTimer > 0.2f)
		flags.isBoxHalf = false;

	if (flags.isBoxHalf)
		t.animationTimer += DELTA_TIME;

	(flags.isBoxOpen) ? objBox.frame = 3 : objBox.frame = 0;

	if (!IsPlayerColliding(objBox) && !flags.isBoxExit)
		(flags.isBoxHalf) ? objBox.frame = 2 : objBox.frame = 0;

	if (IsPlayerColliding(objBox) && flags.isBoxExit)
	{
		flags.levelPassed = true;
		flags.isBoxOpen = false;
	}
	else if (!IsPlayerColliding(objBox))
	{
		if (t.animationTimer < 0.1f)
			flags.isBoxHalf = true;
	}

	Play::UpdateGameObject(objBox);
}

// timers 
void CoyoteControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (!IsPlayerCollidingAnyPlatform() && !IsPlayerStillCollidingLadder() && gameState.floor != 1)
		t.coyoteTimer += DELTA_TIME;

	if (!IsPlayerCollidingAnyPlatform() && !IsPlayerStillCollidingLadder() && !IsPlayerCollidingLadder() && objPlayer.pos.y < PLAYER_START_POS.y * TILE_WIDTH && t.coyoteTimer > 0.1f)
	{
		t.coyoteTimer = 0.f;
		flags.isGrounded = false;
		gameState.playerState = STATE_FALL;
	}
}

void WalkingDurationControl(float time)
{
	if (gameState.playerState == STATE_WALK)
	{
		if (t.walkTimer > 0.7f && !Play::KeyDown(VK_LEFT) && !Play::KeyDown(VK_RIGHT))
		{
			gameState.playerState = STATE_GROUNDED;
			t.walkTimer = 0.f;
		}
		else
			t.walkTimer += time;
	}
}

void AnimationDurationControl(float time)
{
	if (flags.isAnimating)
	{
		if (t.animationTimer > 0.7f)
		{
			flags.isAnimating = false;
			t.animationTimer = 0.f;
		}
		else
			t.animationTimer += time;
	}
}

// controls for different player states
void WalkingPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	(flags.right) ?
		Play::SetSprite(objPlayer, "cat_walk_right", 0.2f) :
		Play::SetSprite(objPlayer, "cat_walk_left", 0.2f);

	if (Play::KeyDown(VK_RIGHT) || Play::KeyDown('D'))
	{
		if (IsPlayerCollidingAnyPlatformUpper())
			objPlayer.pos.x = objPlayer.oldPos.x;
		else
			objPlayer.pos.x += 20;
		flags.right = true;
	}
	else if (Play::KeyDown(VK_LEFT) || Play::KeyDown('A'))
	{
		if (IsPlayerCollidingAnyPlatformUpper())
			objPlayer.pos.x = objPlayer.oldPos.x;
		else
			objPlayer.pos.x -= 20;
		flags.right = false;
	}

	CoyoteControl();
	JumpControls();
}

void LookAroundControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (t.idleTimer > 24)
	{
		flags.AnimationChanged = false;
		objPlayer.animSpeed = 0.f;
		t.idleTimer = 0.f;
		(flags.right) ? Play::SetSprite(objPlayer, "cat_walk_right", 0.f) : Play::SetSprite(objPlayer, "cat_walk_left", 0.f);
	}

	else if (t.idleTimer > 18)
	{
		flags.AnimationChanged = true;
		(flags.right) ? Play::SetSprite(objPlayer, "cat_walk_dontlook_right", 1.5f) : Play::SetSprite(objPlayer, "cat_walk_dontlook_left", 0.05f);
	}

	else if (t.idleTimer > 14)
	{
		flags.AnimationChanged = false;
		(flags.right) ? Play::SetSprite(objPlayer, "cat_look_right", 1.5f) : Play::SetSprite(objPlayer, "cat_look_left", 0.05f);
	}
	else if (t.idleTimer > 10)
	{
		flags.AnimationChanged = true;
		(flags.right) ? Play::SetSprite(objPlayer, "cat_walk_look_right", 1.5f) : Play::SetSprite(objPlayer, "cat_walk_look_left", 0.05f);
	}

}

void HairballControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (t.hairballTimer > 5.f)
		t.hairballTimer = 0.f;

	if (t.idleTimer > 5.f)
		t.hairballTimer += DELTA_TIME;

	if (t.hairballTimer / DELTA_TIME < 10 && t.hairballTimer > 0.f)
		(flags.right) ? Play::SetSprite(objPlayer, "cat_hairball_right", 0.01f) : Play::SetSprite(objPlayer, "cat_hairball_left", 0.01f);
	else
	{
		(flags.right) ? Play::SetSprite(objPlayer, "cat_sits_right", 0.1f) : Play::SetSprite(objPlayer, "cat_sits_left", 0.1f);
	}
}

void LickControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (t.lickTimer > 5.f)
		t.lickTimer = 0.f;

	if (t.idleTimer > 5.f)
		t.lickTimer += DELTA_TIME;

	if (t.lickTimer / DELTA_TIME < 10 && t.lickTimer > 0.f)
		(flags.right) ? Play::SetSprite(objPlayer, "cat_lick_right", 0.01f) : Play::SetSprite(objPlayer, "cat_lick_left", 0.01f);
	else
	{
		(flags.right) ? Play::SetSprite(objPlayer, "cat_sits_right", 0.1f) : Play::SetSprite(objPlayer, "cat_sits_left", 0.1f);
	}
}

void IdlePlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	objPlayer.rotation = Play::DegToRad(0);



	t.idleTimer += DELTA_TIME;

	(flags.right) ?
		Play::SetSprite(objPlayer, "cat_sits_right", 0.1f) :
		Play::SetSprite(objPlayer, "cat_sits_left", 0.1f);

	if (Play::KeyDown(VK_RIGHT) || Play::KeyDown(VK_LEFT) || Play::KeyDown('D') || Play::KeyDown('A'))
	{
		gameState.playerState = STATE_WALK;
		t.idleTimer = 0.f;

		//if (!flags.newDirection)
		//{
		//	gameState.playerState = STATE_WALK;
		//	//flags.newDirection = true;
		//}
		//else
		//{
		//	flags.right = !flags.right;
		//	flags.newDirection = false;
		//}
	}


	// if player is standing on ladder, then attach player to ladder
	else if (Play::KeyDown(VK_DOWN) && IsPlayerStillCollidingLadder())
	{
		t.idleTimer = 0.f;
		(flags.right) ?
			objPlayer.pos = { ladderPos.x - PLAYER_AABB_UPPER.x, ladderPos.y + TILE_SIZE * 2 } :
			objPlayer.pos = { ladderPos.x + PLAYER_AABB_UPPER.x, ladderPos.y + TILE_SIZE * 2 };

		flags.isGrounded = false;
		gameState.playerState = STATE_ATTACHED;

	}

	flags.isGrounded = true;

	CoyoteControl();
	JumpControls();

	//HairballControl();
	//LickControl();

	Play::UpdateGameObject(objPlayer);
}

void AttachedPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	objPlayer.animSpeed = 0.f;

	t.idleTimer += DELTA_TIME;

	if (!IsPlayerCollidingLadder)
		gameState.playerState = STATE_FALL;


	// if player is not standing on ladder or colliding with ladder, then fall
	if (!IsPlayerCollidingLadder() && !IsPlayerOnLadder())
	{
		gameState.playerState = STATE_FALL;
		return;
	}

	if (gameState.floor == 2 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_SECOND + LADDER_OFFSET_Y
		|| gameState.floor == 3 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_THIRD + LADDER_OFFSET_Y
		|| gameState.floor == 4 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_FOURTH + LADDER_OFFSET_Y
		|| gameState.floor == 5 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_FIFTH + LADDER_OFFSET_Y
		|| gameState.floor == 6 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_SIXTH + LADDER_OFFSET_Y)
	{
		if (gameState.floor == 2)
			ladderPos.y = platform1.LADDER_TOP_FLOOR_SECOND;
		else if (gameState.floor == 3)
			ladderPos.y = platform1.LADDER_TOP_FLOOR_THIRD;
		else if (gameState.floor == 4)
			ladderPos.y = platform1.LADDER_TOP_FLOOR_FOURTH;
		else if (gameState.floor == 5)
			ladderPos.y = platform1.LADDER_TOP_FLOOR_FIFTH;
		else if (gameState.floor == 6)
			ladderPos.y = platform1.LADDER_TOP_FLOOR_SIXTH;

		t.idleTimer = 0.f;
		SetPlayerPos(ladderPos.y);
		objPlayer.pos.x = ladderPos.x;
		gameState.playerState = STATE_GROUNDED;
	}

	if (flags.right)
	{
		objPlayer.rotation = Play::DegToRad(-90);
		objPlayer.pos.x = ladderPos.x - TILE_SIZE - ladderOffsetX;
		Play::SetSprite(objPlayer, "cat_walk_right", 0.f);
	}
	else
	{
		objPlayer.rotation = Play::DegToRad(90);
		objPlayer.pos.x = ladderPos.x + TILE_SIZE + ladderOffsetX;
		Play::SetSprite(objPlayer, "cat_walk_left", 0.f);
	}

	if (Play::KeyDown(VK_UP) || Play::KeyDown('W'))
	{
		t.idleTimer = 0.f;
		objPlayer.pos.y -= 10;
		objPlayer.animSpeed = 0.1f;
	}
	else if (Play::KeyDown(VK_DOWN) || Play::KeyDown('S'))
	{
		t.idleTimer = 0.f;
		if (IsPlayerCollidingAnyPlatform() && objPlayer.pos.y > SECOND_FLOOR_HEIGHT * TILE_SIZE)
		{
			(flags.right) ? objPlayer.pos.x -= 20 : objPlayer.pos.x += 20;

			(groundedPosY > PLAYER_START_POS.y * TILE_SIZE - PLAYER_OFFSET_Y) ?
				SetPlayerPos(PLAYER_START_POS.y * TILE_SIZE) :
				SetPlayerPos(groundedPosY);

			gameState.playerState = STATE_GROUNDED;
		}
		else
			objPlayer.pos.y += 10;

		objPlayer.animSpeed = 0.1f;
	}
	else if (Play::KeyDown(VK_LEFT))
	{
		t.idleTimer = 0.f;
		flags.right = true;
	}
	else if (Play::KeyDown(VK_RIGHT))
	{
		t.idleTimer = 0.f;
		flags.right = false;
	}

	else if (Play::KeyDown(VK_SPACE) && !IsPlayerCollidingAnyPlatformUpper())
	{
		t.idleTimer = 0.f;
		objPlayer.pos.x += sin(objPlayer.rotation) * 25;
		objPlayer.pos.y -= cos(objPlayer.rotation) * 25;
		objPlayer.rotation = 0;
	}

	//LookAroundControl();
}

void FurballPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	Play::SetSprite(objPlayer, "ball_furball", 0.f);

	(flags.right) ? objPlayer.rotSpeed = 1.1f : objPlayer.rotSpeed = -1.1f;

	if (t.furballTimer > 2.f && !Play::KeyDown(VK_RIGHT) && !Play::KeyDown(VK_LEFT))
	{
		gameState.playerState = STATE_GROUNDED;
		objPlayer.rotSpeed = 0.f;
		objPlayer.rotation = Play::DegToRad(0);
		t.furballTimer = 0.f;
	}

	// && objPlayer.rotation == Play::DegToRad(0)
	if (Play::KeyDown(VK_RIGHT))
	{
		t.furballTimer = 0.f;
		objPlayer.pos.x += 20;
	}
	else if (Play::KeyDown(VK_LEFT))
	{
		t.furballTimer = 0.f;
		objPlayer.pos.x -= 20;
	}

	if (IsPlayerCollidingAnyPlatformUpper())
	{
		gameState.playerState = STATE_GROUNDED;
		objPlayer.rotSpeed = 0.f;
		objPlayer.rotation = Play::DegToRad(0);
		t.furballTimer = 0.f;
	}

	t.furballTimer += DELTA_TIME;
}

// controls for different flea states
void WalkingFlea(GameObject& flea)
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	flea.velocity = (flea.right) ? ENEMY_VELOCITY_WALK_RIGHT : ENEMY_VELOCITY_WALK_LEFT;
	flea.rotation = 0;

	LoopObject(flea);
	FleaAI(flea);

	(flea.right) ? Play::SetSprite(flea, "bug_right", 0.05f) : Play::SetSprite(flea, "bug_left", 0.05f);

	if (!IsFleaCollidingAnyPlatform(flea) && flea.state != STATE_JUMP)
	{
		flea.velocity = (flea.right) ? ENEMY_VELOCITY_FALL_RIGHT : ENEMY_VELOCITY_FALL_LEFT;
		flea.acceleration.y = GRAVITY_ACCELERATION.y;
		flea.state = STATE_FALL;
	}

	if (IsPlayerCollidingUpperPart(flea))
	{
		flea.pos = FLEA_START_POS * TILE_SIZE;
		flea.state = STATE_WALK;
		gameState.playerPrevState = gameState.playerState;
		gameState.playerState = STATE_HISS;
		gameState.lives--;
	}
}

void FallingFlea(GameObject& flea)
{
	if (IsFleaCollidingAnyPlatform(flea))
	{
		flea.velocity = { 0.f, 0.f };
		flea.acceleration = { 0.f, 0.f };
		flea.state = STATE_WALK;
	}

	if (IsPlayerCollidingUpperPart(flea))
	{
		flea.pos = FLEA_START_POS * TILE_SIZE;
		flea.state = STATE_WALK;
		gameState.playerPrevState = gameState.playerState;
		gameState.playerState = STATE_HISS;
		gameState.lives--;
	}
}

void AttachedFlea(GameObject& flea)
{

}

void FleaJump(GameObject& flea)
{
	t.jumpTimer += DELTA_TIME;

	flea.rotation = Play::DegToRad(-30);

	if (flea.pos.y > DISPLAY_HEIGHT)
		flea.pos = { leftLimit, 0 };

	(flea.velocity.y < 0) ?
		flea.velocity += GRAVITY * FALL_MULTIPLIER * DELTA_TIME :
		flea.velocity += GRAVITY * DELTA_TIME;

	if (IsFleaCollidingAnyPlatform(flea))
	{
		flea.velocity = PLAYER_VELOCITY_DEFAULT;
		flea.acceleration = PLAYER_VELOCITY_DEFAULT;
		t.jumpTimer = 0.f;
		flea.state = STATE_WALK;
		flags.fleaActivated = false;
	}

	if (IsPlayerCollidingUpperPart(flea))
	{
		flea.pos = FLEA_START_POS * TILE_SIZE;
		flea.state = STATE_WALK;
		gameState.playerPrevState = gameState.playerState;
		gameState.playerState = STATE_HISS;
		gameState.lives--;
	}

}

void FleaAI(GameObject& flea)
{
	if (DistanceFromPlayer(flea) < 300 && DistanceYFromPlayer(flea) < TILE_SIZE * 2)
	{
		flea.velocity = (flea.right) ? ENEMY_VELOCITY_JUMP_RIGHT : ENEMY_VELOCITY_JUMP_LEFT;
		flea.acceleration.y = 0.f;
		flea.state = STATE_JUMP;
		flags.fleaActivated = true;
	}
}

// jump off the ground / ladder
void JumpControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (!IsPlayerCollidingLadder())
	{
		if (Play::KeyDown(VK_LEFT) && Play::KeyPressed(VK_SPACE))
		{
			t.idleTimer = 0.f;
			flags.isGrounded = false;
			objPlayer.velocity = PLAYER_VELOCITY_JUMP_LEFT;
			gameState.playerState = STATE_JUMP;
		}
		else if (Play::KeyDown(VK_RIGHT) && Play::KeyPressed(VK_SPACE))
		{
			t.idleTimer = 0.f;
			flags.isGrounded = false;
			objPlayer.velocity = PLAYER_VELOCITY_JUMP_RIGHT;
			gameState.playerState = STATE_JUMP;
		}
		else if (Play::KeyPressed(VK_SPACE) || Play::KeyDown('2'))
		{
			t.idleTimer = 0.f;
			flags.collided = false;
			flags.isGrounded = false;
			objPlayer.velocity = PLAYER_VELOCITY_JUMP;
			gameState.playerState = STATE_JUMP;
		}
	}
}

// updating behaviours for a PLAYER
void Jump()
{
	JumpCollisionControl();

	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	t.coyoteTimer = 0.f;

	(t.jumpTimer < 1.0f) ? t.jumpTimer += DELTA_TIME : t.jumpTimer = 0.f;

	(flags.right) ?
		Play::SetSprite(objPlayer, "cat_jump_right", 0.05f) :
		Play::SetSprite(objPlayer, "cat_jump_left", 0.05f);

	(flags.right) ?
		objPlayer.pos.x += objPlayer.velocity.x * DELTA_TIME :
		objPlayer.pos.x -= objPlayer.velocity.x * DELTA_TIME;

	(objPlayer.velocity.y < 0 && !Play::KeyDown(VK_SPACE) && t.jumpTimer < 0.5f) ?
		gravityMultiplyer = LOW_JUMP_MULTIPLIER :
		gravityMultiplyer = 1.f;

	(objPlayer.velocity.y < 0) ?
		objPlayer.velocity += GRAVITY * DELTA_TIME * gravityMultiplyer :
		objPlayer.velocity += GRAVITY * FALL_MULTIPLIER * DELTA_TIME * gravityMultiplyer;
}

void JumpCollisionControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (IsPlayerCollidingLadder())
		gameState.playerState = STATE_ATTACHED;

	if (IsPlayerCollidingAnyPlatform() && t.jumpTimer > 0.3f)
	{
		flags.isGrounded = true;
		SetPlayerPos(groundedPosY);
		gameState.playerState = STATE_GROUNDED;
		objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
		t.jumpTimer = 0.f;
	}
	else if (IsPlayerCollidingAnyPlatformUpper() && objPlayer.velocity.y < 0)
	{
		objPlayer.velocity.y = 0.f;
		gameState.playerState = STATE_FALL;
	}

	if (IsPlayerCollidingWalls())
	{
		objPlayer.velocity = { 0.f, 0.f };
		gameState.playerState = STATE_FALL;
	}

	if (IsPlayerStillCollidingLadder())
	{
		gameState.playerState = STATE_GROUNDED;
		objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	}
}

void Fall()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	objPlayer.acceleration = GRAVITY_ACCELERATION;

	(!flags.right) ?
		Play::SetSprite(objPlayer, "cat_fall_left", 0.1f) :
		Play::SetSprite(objPlayer, "cat_fall_right", 0.1f);

	(!flags.right) ?
		objPlayer.velocity += PLAYER_VELOCITY_FALL_RIGHT * DELTA_TIME :
		objPlayer.velocity += PLAYER_VELOCITY_FALL_LEFT * DELTA_TIME;

	if (gameState.playerPrevState == STATE_ATTACHED)
		objPlayer.velocity.x *= -1;

	if (IsPlayerCollidingAnyPlatform())
	{
		// compare PLAYER POS Y with just assigned (in collide function) value of GROUNDED POS Y (potential player pos) 
		// only ground player if he is above the platform
		if (objPlayer.pos.y <= groundedPosY
			|| objPlayer.pos.y > PLAYER_START_POS.y * TILE_SIZE - PLAYER_OFFSET_Y)
		{
			flags.isGrounded = true;
			objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
			objPlayer.acceleration = PLAYER_VELOCITY_DEFAULT;

			(groundedPosY > PLAYER_START_POS.y * TILE_SIZE - PLAYER_OFFSET_Y) ?
				SetPlayerPos(PLAYER_START_POS.y * TILE_SIZE) :
				SetPlayerPos(groundedPosY);

			gameState.playerState = STATE_GROUNDED;
		}
		else if (objPlayer.pos.x > objPlayer.oldPos.x)
			objPlayer.pos.x = objPlayer.oldPos.x - 10;
		else if (objPlayer.pos.x < objPlayer.oldPos.x)
			objPlayer.pos.x = objPlayer.oldPos.x + 10;

		objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	}

	else if (IsPlayerCollidingLadder())
	{
		objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
		objPlayer.acceleration = PLAYER_VELOCITY_DEFAULT;
		flags.isGrounded = false;
		gameState.playerState = STATE_ATTACHED;
	}
}

void Hiss()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	t.hissTimer += DELTA_TIME;

	if (gameState.playerPrevState == STATE_JUMP || gameState.playerPrevState == STATE_FALL || gameState.playerPrevState == STATE_ATTACHED || t.hissTimer > 1.2f)
	{
		t.hissTimer = 0.f;
		if (gameState.playerPrevState != STATE_HISS)
			gameState.playerState = gameState.playerPrevState;
		else
			gameState.playerState = STATE_GROUNDED;
	}

	(flags.right && gameState.playerState != STATE_FURBALL) ?
		Play::SetSprite(objPlayer, "cat_hiss", 0.1f) :
		Play::SetSprite(objPlayer, "cat_hiss_new_left", 0.1f);

}

void Reborn()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	t.rebornTimer += DELTA_TIME;
	objPlayer.acceleration.y = 0.f;

	if (t.rebornTimer < 1.2f)
	{
		Play::SetSprite(objPlayer, "cat_reborn_right", 0.1f);
	}
	else
	{
		t.rebornTimer = 0.f;
		gameState.playerState = STATE_GROUNDED;
		objPlayer.pos = objPlayer.rebornPos;
		flags.right = true;
	}
}

// checks for collisions
bool IsPlayerOnLadder()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	vector <int> vLadders = Play::CollectGameObjectIDsByType(TYPE_LADDER);
	for (int ladderId : vLadders)
	{
		GameObject& ladder = Play::GetGameObject(ladderId);

		if (flags.right)
		{
			if (ladder.pos.x + PLATFORM_AABB.x > objPlayer.pos.x
				&& ladder.pos.x - PLATFORM_AABB.x < objPlayer.pos.x + PLAYER_AABB_UPPER.x)
				return true;
		}
		else
		{
			if (ladder.pos.x + PLATFORM_AABB.x > objPlayer.pos.x - PLAYER_AABB_UPPER.x
				&& ladder.pos.x - PLATFORM_AABB.x < objPlayer.pos.x)
				return true;
		}
	}
	return false;
}

bool IsPlayerCollidingLadder()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	vector <int> vLadders = Play::CollectGameObjectIDsByType(TYPE_LADDER);
	for (int ladderId : vLadders)
	{
		GameObject& ladder = Play::GetGameObject(ladderId);
		if (IsPlayerCollidingUpperPart(ladder))
		{
			ladderPos = ladder.pos;
			return true;
		}
	}
	return false;
}

bool IsPlayerStillCollidingLadder()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	vector <int> vLadders = Play::CollectGameObjectIDsByType(TYPE_LADDER);
	for (int ladderId : vLadders)
	{
		GameObject& ladder = Play::GetGameObject(ladderId);

		if (IsPlayerCollidingBottomPart(ladder))
			return true;
	}
	return false;
}

bool IsPlayerCollidingAnyPlatform()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		if (IsPlayerCollidingBottomPart(platform))
			return true;
	}

	vector <int> vWalls = Play::CollectGameObjectIDsByType(TYPE_WALL);
	for (int wallId : vWalls)
	{
		GameObject& wall = Play::GetGameObject(wallId);
		if (IsPlayerCollidingBottomPart(wall))
			return true;
	}

	if (IsPlayerCollidingGround())
		return true;

	return false;
}

bool IsPlayerCollidingGround()
{
	vector <int> vGrounds = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int groundId : vGrounds)
	{
		GameObject& ground = Play::GetGameObject(groundId);
		if (IsPlayerCollidingBottomPart(ground))
			return true;
	}
	return false;
}

bool IsPlayerCollidingAnyPlatformUpper()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		if (IsPlayerCollidingUpperPart(platform))
			return true;
	}

	vector <int> vWalls = Play::CollectGameObjectIDsByType(TYPE_WALL);
	for (int wallId : vWalls)
	{
		GameObject& wall = Play::GetGameObject(wallId);
		if (IsPlayerCollidingUpperPart(wall))
			return true;
	}

	vector <int> vGrounds = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int groundId : vGrounds)
	{
		GameObject& ground = Play::GetGameObject(groundId);
		if (IsPlayerCollidingUpperPart(ground))
			return true;
	}
	return false;
}

bool IsPlayerCollidingAnyPlatformBoth()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		if (IsPlayerCollidingBottomPart(platform))
			return true;
		if (IsPlayerCollidingUpperPart(platform))
			return true;
	}

	vector <int> vGrounds = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int groundId : vGrounds)
	{
		GameObject& ground = Play::GetGameObject(groundId);
		if (IsPlayerCollidingBottomPart(ground))
			return true;
		if (IsPlayerCollidingUpperPart(ground))
			return true;
	}
	return false;
}

bool IsPlayerCollidingUpperPart(const GameObject& object)
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	Vector2D AABB = { 0, 0 };
	switch (object.type)
	{
	case TYPE_PLATFORM:
	{
		AABB = PLATFORM_AABB;
		break;
	}
	case TYPE_GROUND:
	{
		AABB = GROUND_PLATFORM_AABB;
		break;
	}
	case TYPE_LADDER:
	{
		AABB = PLATFORM_AABB;
		break;
	}
	case TYPE_WALL:
	{
		AABB = WALL_AABB;
		break;
	}
	case TYPE_FLEA:
	{
		AABB = ENEMY_AABB;
		break;
	}
	case TYPE_TREAT:
	{
		AABB = TREATS_AABB;
		break;
	}
	case TYPE_FLOWER:
	{
		AABB = TREATS_AABB;
		break;
	}
}

	if (flags.right)
	{
		if (objPlayer.pos.y - PLAYER_AABB_UPPER.y < object.pos.y + AABB.y &&
			objPlayer.pos.y + PLAYER_AABB_UPPER.y > object.pos.y - AABB.y)
		{
			if (objPlayer.pos.x + PLAYER_AABB_UPPER.x > object.pos.x - AABB.x &&
				objPlayer.pos.x < object.pos.x + AABB.x)
				return true;
		}
	}
	else
	{
		if (objPlayer.pos.y - PLAYER_AABB_UPPER.y < object.pos.y + AABB.y &&
			objPlayer.pos.y + PLAYER_AABB_UPPER.y > object.pos.y - AABB.y)
		{
			if (objPlayer.pos.x > object.pos.x - AABB.x &&
				objPlayer.pos.x - PLAYER_AABB_UPPER.x < object.pos.x + AABB.x)
				return true;
		}
	}
	return false;
}

bool IsPlayerCollidingBottomPart(const GameObject& object)
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	Vector2D AABB = { 0, 0 };
	switch (object.type)
	{
	case TYPE_PLATFORM:
	{
		AABB = PLATFORM_AABB;
		break;
	}
	case TYPE_GROUND:
	{
		AABB = GROUND_PLATFORM_AABB;
		break;
	}
	case TYPE_LADDER:
	{
		AABB = PLATFORM_AABB;
		break;
	}
	}

	if (objPlayer.pos.y < object.pos.y + AABB.y &&
		objPlayer.pos.y + PLAYER_AABB_BOTTOM.y > object.pos.y - AABB.y)
	{
		if (objPlayer.pos.x > object.pos.x - AABB.x - COLLISION_BOOST &&
			objPlayer.pos.x < object.pos.x + AABB.x + COLLISION_BOOST)
		{
			groundedPosY = object.pos.y - PLAYER_AABB_BOTTOM.y - PLAYER_OFFSET_Y;

			// set player velocity to 0 so he doesnt fall through the platform
			if (t.jumpTimer > 0.f)
				objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;

			flags.collided = true;
			return true;
		}
	}
	return false;
}

bool IsPlayerCollidingWalls()
{
	vector <int> vWalls = Play::CollectGameObjectIDsByType(TYPE_WALL);
	for (int wallId : vWalls)
	{
		GameObject& wall = Play::GetGameObject(wallId);

		if (IsPlayerCollidingUpperPart(wall))
			return true;
	}
	return false;
}

bool IsPlayerColliding(const GameObject& object)
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	Vector2D AABB = { 0.f, 0.f };

	switch (object.type)
	{
	case TYPE_PLATFORM:
		AABB = PLATFORM_AABB;
		break;
	case TYPE_WATER:
		AABB = PLATFORM_AABB;
		break;
	}

	if (objPlayer.pos.y - PLAYER_AABB_BOTTOM.y < object.pos.y + AABB.y &&
		objPlayer.pos.y + PLAYER_AABB_BOTTOM.y > object.pos.y - AABB.y)
	{
		if (objPlayer.pos.x + PLAYER_AABB_BOTTOM.x > object.pos.x - AABB.x &&
			objPlayer.pos.x - PLAYER_AABB_BOTTOM.x < object.pos.x + AABB.x)
			return true;
	}
	return false;
}

// fleas collisions with platforms
bool IsFleaCollidingAnyPlatform(GameObject& flea)
{
	vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		if (IsFleaColliding(flea, platform))
			return true;
	}

	vector <int> vGrounds = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int groundId : vGrounds)
	{
		GameObject& ground = Play::GetGameObject(groundId);
		if (IsFleaColliding(flea, ground))
			return true;
	}

	vector <int> vWater = Play::CollectGameObjectIDsByType(TYPE_WATER);
	for (int waterId : vWater)
	{
		GameObject& water = Play::GetGameObject(waterId);
		if (IsFleaColliding(flea, water))
			return true;
	}
	return false;
}

bool IsFleaColliding(GameObject& flea, const GameObject& object)
{
	vector <int> vFleas = Play::CollectGameObjectIDsByType(TYPE_FLEA);

	Vector2D AABB = { 0.f, 0.f };

	switch (object.type)
	{
	case TYPE_PLATFORM:
		AABB = PLATFORM_AABB;
		break;
	case TYPE_GROUND:
		AABB = GROUND_PLATFORM_AABB;
		break;
	}

	if (flea.pos.y - ENEMY_AABB.y < object.pos.y + AABB.y &&
		flea.pos.y + ENEMY_AABB.y > object.pos.y - AABB.y)
	{
		if (flea.pos.x + ENEMY_AABB.x > object.pos.x - AABB.x &&
			flea.pos.x - ENEMY_AABB.x < object.pos.x + AABB.x)
		{
			flea.platformPos = object.pos;
			return true;
		}
	}

	return false;
}

// if object is out of x bounds , loop it to the other side
void LoopObject(GameObject& object)
{
	if (object.pos.x < leftLimit )
		object.pos.x = rightLimit;
	else if (object.pos.x > rightLimit )
		object.pos.x = leftLimit;
}

// helper functions
float DistanceFromPlayer(const GameObject& object)
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	return sqrt(pow(objPlayer.pos.x - object.pos.x, 2) + pow(objPlayer.pos.y - object.pos.y, 2));
}

float DistanceYFromPlayer(const GameObject& object)
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	return abs(objPlayer.pos.y - object.pos.y);
}

float DotProduct(const Vector2D& v1, const Vector2D& v2)
{
	Vector2D n1 = v1;
	Vector2D n2 = v2;
	Normalise(n1);
	Normalise(n2);
	return n1.x * n2.x + n1.y * n2.y;
}

void Normalise(Vector2D& v)
{
	float length = sqrt(v.x * v.x + v.y * v.y);
	v.x /= length;
	v.y /= length;
}

float Randomize(int range, float multiplier)
{
	return (float)(rand() % range) * multiplier;
}

// drawing functions
void Draw()
{
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();

	DrawLevel();
	//DrawDebugInfo();
	DrawGameStats();

	Play::PresentDrawingBuffer();
}

void DrawGameStats()
{
	Play::SetDrawingSpace(Play::SCREEN);

	Play::DrawFontText("105px", "Score: " + std::to_string(gameState.score), { 250, 50 }, Play::RIGHT);
	Play::DrawFontText("105px", "Level: " + std::to_string(gameState.level), { DISPLAY_WIDTH - 250, 50 }, Play::LEFT);

	(gameState.lives > 0) ?
		Play::DrawSpriteRotated("red_heart", { 450, 50 }, 1, 0.f, 3.5f) :
		Play::DrawSpriteRotated("white_heart", { 450, 50 }, 1, 0.f, 3.5f);

	(gameState.lives > 1) ?
		Play::DrawSpriteRotated("red_heart", { 500, 50 }, 1, 0.f, 3.5f) :
		Play::DrawSpriteRotated("white_heart", { 500, 50 }, 1, 0.f, 3.5f);

	(gameState.lives > 2) ?
		Play::DrawSpriteRotated("red_heart", { 550, 50 }, 1, 0.f, 3.5f) :
		Play::DrawSpriteRotated("white_heart", { 550, 50 }, 1, 0.f, 3.5f);

	(gameState.lives > 3) ?
		Play::DrawSpriteRotated("red_heart", { 600, 50 }, 1, 0.f, 3.5f) :
		Play::DrawSpriteRotated("white_heart", { 600, 50 }, 1, 0.f, 3.5f);

	(gameState.lives > 4) ?
		Play::DrawSpriteRotated("red_heart", { 650, 50 }, 1, 0.f, 3.5f) :
		Play::DrawSpriteRotated("white_heart", { 650, 50 }, 1, 0.f, 3.5f);

	(gameState.lives > 5) ?
		Play::DrawSpriteRotated("red_heart", { 700, 50 }, 1, 0.f, 3.5f) :
		Play::DrawSpriteRotated("white_heart", { 700, 50 }, 1, 0.f, 3.5f);

	(gameState.lives > 6) ?
		Play::DrawSpriteRotated("red_heart", { 750, 50 }, 1, 0.f, 3.5f) :
		Play::DrawSpriteRotated("white_heart", { 750, 50 }, 1, 0.f, 3.5f);

	(gameState.lives > 7) ?
		Play::DrawSpriteRotated("red_heart", { 800, 50 }, 1, 0.f, 3.5f) :
		Play::DrawSpriteRotated("white_heart", { 800, 50 }, 1, 0.f, 3.5f);

	(gameState.lives > 8) ?
		Play::DrawSpriteRotated("red_heart", { 850, 50 }, 1, 0.f, 3.5f) :
		Play::DrawSpriteRotated("white_heart", { 850, 50 }, 1, 0.f, 3.5f);

	Play::SetDrawingSpace(Play::WORLD);
}

void DrawDebugInfo()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	vector <int> vFleas = Play::CollectGameObjectIDsByType(TYPE_FLEA);
	//GameObject& f1 = Play::GetGameObject(vFleas[0]);
	//GameObject& f2 = Play::GetGameObject(vFleas[1]);

	/*vector <int> vRightFlies = Play::CollectGameObjectIDsByType(TYPE_RIGHT_FLY);
	vector <int> vLeftFlies = Play::CollectGameObjectIDsByType(TYPE_LEFT_FLY);
	GameObject& fly1 = Play::GetGameObject(vRightFlies[0]);
	GameObject& fly2 = Play::GetGameObject(vLeftFlies[0]);*/

	//GameObject& fly1 = Play::GetGameObjectByType(TYPE_RIGHT_FLY);
	//GameObject& fly2 = Play::GetGameObjectByType(TYPE_LEFT_FLY);


	GameObject& objBox = Play::GetGameObjectByType(TYPE_BOX);


	Play::SetDrawingSpace(Play::SCREEN);

	//Play::DrawFontText("64px", "flea1: " + std::to_string(f1.pos.y), { DISPLAY_WIDTH - 10, 250 }, Play::RIGHT);
	//Play::DrawFontText("64px", "flea2: " + std::to_string(f2.acceleration.y), { DISPLAY_WIDTH - 10, 100 }, Play::RIGHT);

	//Play::DrawFontText("64px", "cat state: " + std::to_string(gameState.playerState), { DISPLAY_WIDTH - 10, 150 }, Play::RIGHT);
	Play::DrawFontText("64px", "cat vy: " + std::to_string(objPlayer.velocity.y), { DISPLAY_WIDTH - 10 , 200 }, Play::RIGHT);
	Play::DrawFontText("64px", "cat a: " + std::to_string(objPlayer.acceleration.y), { DISPLAY_WIDTH - 10, 250 }, Play::RIGHT);

	//Play::DrawFontText("105px", "Score: " + std::to_string(gameState.score), { DISPLAY_WIDTH / 2 , 50 }, Play::CENTRE);

	//Play::DrawFontText("64px", "box exit: " + std::to_string(objBox.exitPos.x), { DISPLAY_WIDTH - 10, 300 }, Play::RIGHT);
	//Play::DrawFontText("64px", "grounded: " + std::to_string(flags.isGrounded ), { DISPLAY_WIDTH- 10, 350 }, Play::RIGHT);
	//Play::DrawFontText("64px", "Fly1 px: " + std::to_string(fly1.pos.x), { DISPLAY_WIDTH - 10, 400 }, Play::RIGHT);
	//Play::DrawFontText("64px", "Fly2 px: " + std::to_string(fly2.pos.x), { DISPLAY_WIDTH - 10, 450 }, Play::RIGHT);

	Play::SetDrawingSpace(Play::WORLD);
}

void DrawLevel()
{
	DrawObjectsOfType(TYPE_WALL);
	DrawObjectsOfType(TYPE_PLATFORM);
	DrawObjectsOfType(TYPE_TREAT);
	DrawObjectsOfType(TYPE_LADDER);
	DrawObjectsOfType(TYPE_WATER);
	DrawObjectsOfType(TYPE_GROUND);
	DrawObjectsOfType(TYPE_BUSH);
	DrawObjectsOfType(TYPE_TREE);
	DrawObjectsOfType(TYPE_FLEA);
	DrawObjectsOfType(TYPE_FLOWER);
	DrawObjectsOfType(TYPE_POO);
	DrawObjectsOfType(TYPE_RIGHT_FLY);
	DrawObjectsOfType(TYPE_LEFT_FLY);
	DrawObjectsOfType(TYPE_PLAYER);
	DrawObjectsOfType(TYPE_BOX);
	DrawObjectsOfType(TYPE_STAR);
	DrawObjectsOfType(TYPE_SPLASH);
}

void DrawObjectsOfType(int TYPE)
{
	vector <int> vObjects = Play::CollectGameObjectIDsByType(TYPE);
	for (int objectId : vObjects)
	{
		Play::DrawObjectRotated(Play::GetGameObject(objectId));
	}
}

void DrawRect(const GameObject& object, Vector2D AABB)
{
	Play::DrawRect(Point2D{ object.pos.x - AABB.x, object.pos.y - AABB.y }, Point2D{ object.pos.x + AABB.x, object.pos.y + AABB.y }, Play::cWhite);
}

void DrawLobby()
{
	Play::ClearDrawingBuffer(Play::cWhite);

	(gameState.state == STATE_PAUSE) ?
		Play::DrawBackground(1) :
		Play::DrawBackground(2);

	UpdateLobby();

	(gameState.state == STATE_PAUSE) ?
		Play::DrawFontText("151px", ". . .Paused. . .", { 500, 230 }, Play::CENTRE) :
		Play::DrawFontText("151px", "cat & fleas. . .", { 400, 230 }, Play::CENTRE);

	(gameState.state == STATE_PAUSE) ?
		Play::DrawFontText("64px", "Enter to Back Playing . . . Tab to restart", { 500, 310 }, Play::CENTRE) :
		Play::DrawFontText("64px", "Enter to Play . . . Back to Pause", { 400, 310 }, Play::CENTRE);

	DrawObjectsOfType(TYPE_LOBBY_FLEA);
	DrawObjectsOfType(TYPE_LOBBY_CAT);

	Play::PresentDrawingBuffer();
}

void DrawVictory()
{
	Play::ClearDrawingBuffer(Play::cWhite);

	Play::DrawBackground(3);

	Play::DrawFontText("151px", "Victory !!!", { 500, 230 }, Play::CENTRE);
	Play::DrawFontText("64px", ". . .Tab to restart game. . .", { 500, 310 }, Play::CENTRE);

	Play::PresentDrawingBuffer();
}

void UpdateLobby()
{
	GameObject& objCat = Play::GetGameObjectByType(TYPE_LOBBY_CAT);
	Play::UpdateGameObject(objCat);

	vector <int> vLobbyFleas = Play::CollectGameObjectIDsByType(TYPE_LOBBY_FLEA);

	for (int fleaId : vLobbyFleas)
	{
		GameObject& objFlea = Play::GetGameObject(fleaId);
		objFlea.pos.x += sin(objFlea.rotation) * objFlea.speed;
		objFlea.pos.y -= cos(objFlea.rotation) * objFlea.speed;

		if (objFlea.pos.y > DISPLAY_HEIGHT + 10)
			objFlea.pos.y = -10;
		else if (objFlea.pos.x > DISPLAY_WIDTH + 10)
			objFlea.pos.x = -10;
		else if (objFlea.pos.x < -10)
			objFlea.pos.x = DISPLAY_WIDTH + 10;
		else if (objFlea.pos.y < -10)
			objFlea.pos.y = DISPLAY_HEIGHT - 10;

		Play::UpdateGameObject(objFlea);
	}
}

// remove destroyed objects
void UpdateDestroyed()
{
	vector <int> vDestroyed = Play::CollectGameObjectIDsByType(TYPE_DESTROYED);

	for (int destroyedId : vDestroyed)
	{
		Play::DestroyGameObject(destroyedId);
	}
}

// reset player to starting pos
void ResetPlayer()
{
	gameState.playerState = STATE_GROUNDED;
	Play::GetGameObjectByType(TYPE_PLAYER).pos = { PLAYER_START_POS.x * TILE_SIZE, PLAYER_START_POS.y * TILE_SIZE - PLAYER_OFFSET_Y };
	Play::GetGameObjectByType(TYPE_PLAYER).velocity = PLAYER_VELOCITY_DEFAULT;
}

void DestroyAllObjects()
{
	DestroyObjectsOfType(TYPE_GROUND);
	DestroyObjectsOfType(TYPE_PLATFORM);
	DestroyObjectsOfType(TYPE_LADDER);
	DestroyObjectsOfType(TYPE_WALL);
	DestroyObjectsOfType(TYPE_BUSH);
	DestroyObjectsOfType(TYPE_TREE);
	DestroyObjectsOfType(TYPE_TREAT);
	DestroyObjectsOfType(TYPE_WATER);
	DestroyObjectsOfType(TYPE_FLEA);
	DestroyObjectsOfType(TYPE_BOX);
	DestroyObjectsOfType(TYPE_FLOWER);
	DestroyObjectsOfType(TYPE_POO);
	DestroyObjectsOfType(TYPE_RIGHT_FLY);
	DestroyObjectsOfType(TYPE_LEFT_FLY);
	DestroyObjectsOfType(TYPE_PLAYER);
	DestroyObjectsOfType(TYPE_STAR);
	DestroyObjectsOfType(TYPE_SPLASH);
	DestroyObjectsOfType(TYPE_LOBBY_FLEA);
	DestroyObjectsOfType(TYPE_LOBBY_CAT);
	
}

void DestroyObjectsOfType(int TYPE)
{
	vector <int> vObjects = Play::CollectGameObjectIDsByType(TYPE);
	for (int objectId : vObjects)
	{
		Play::GetGameObject(objectId).type = TYPE_DESTROYED;
	}
}

void RestartGame()
{
	DestroyAllObjects();

	gameState.score = 0;
	gameState.lives = 9;
	gameState.treats = 0;
	gameState.floor = 0;
	gameState.playerState = STATE_JUMP;
	gameState.playerPrevState = STATE_GROUNDED;
	gameState.enemyState = STATE_HIDE;
	//gameState.state = STATE_PLAY;

	flags.isBoxExit = false;
	flags.isBoxOpen = true;
	flags.isBoxHalf = false;
	flags.right = true;

	t.jumpTimer = 0.f;

	//CreateGamePlay();
}

void LoadLevel(void)
{
	for (int id_obj : Play::CollectAllGameObjectIDs())
		Play::DestroyGameObject(id_obj);

	std::ifstream levelfile;

	levelfile.open(levelList.at(gameState.level - 1));

	std::string sType, sX, sY, sSprite, sRot;

	std::getline(levelfile, sType);

	while (!levelfile.eof())
	{
		std::getline(levelfile, sType);
		std::getline(levelfile, sX);
		std::getline(levelfile, sY);
		std::getline(levelfile, sSprite);

		int id = -1;

		if (sType == "TYPE_PLAYER")
		{
			id = Play::CreateGameObject(TYPE_PLAYER, { std::stof(sX) - 30, std::stof(sY) }, 0, sSprite.c_str());
			GameObject& objPlayer = Play::GetGameObject(id);
			objPlayer.rebornPos = { std::stof(sX) + 100, std::stof(sY) };
			objPlayer.velocity = PLAYER_VELOCITY_JUMP_RIGHT;
			objPlayer.scale = PLAYER_SCALE;
		}
		
		if (sType == "TYPE_FLEA")
		{
			id = Play::CreateGameObject(TYPE_FLEA, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			GameObject& objFlea = Play::GetGameObject(id);
			objFlea.velocity = ENEMY_VELOCITY_FALL_RIGHT;
			objFlea.state = STATE_WALK;
			objFlea.scale = FLEA_SCALE;
			objFlea.rotation = Play::DegToRad(-30);
			objFlea.animSpeed = .05f;
		}

		if (sType == "TYPE_BOX")
		{
			id = Play::CreateGameObject(TYPE_BOX, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			GameObject& objBox = Play::GetGameObject(id);
			objBox.scale = BOX_SCALE;
		}

		if (sType == "TYPE_PLATFORM")
		{
			id = Play::CreateGameObject(TYPE_PLATFORM, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			Play::GetGameObject(id).scale = SCALE;
		}

		if (sType == "TYPE_GROUND")
		{
			id = Play::CreateGameObject(TYPE_GROUND, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			Play::GetGameObject(id).scale = SCALE;
		}

		if (sType == "TYPE_TREAT")
		{
			id = Play::CreateGameObject(TYPE_TREAT, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			Play::GetGameObject(id).scale = SCALE;
		}

		if (sType == "TYPE_WATER")
		{
			id = Play::CreateGameObject(TYPE_WATER, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			Play::GetGameObject(id).scale = WATER_SCALE;
			Play::GetGameObject(id).animSpeed = 0.1f;
		}

		if (sType == "TYPE_TREE")
		{
			id = Play::CreateGameObject(TYPE_TREE, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			Play::GetGameObject(id).scale = TREE_SCALE;
		}

		if (sType == "TYPE_BUSH")
		{
			id = Play::CreateGameObject(TYPE_BUSH, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			Play::GetGameObject(id).scale = SCALE;
		}

		if (sType == "TYPE_LADDER")
		{
			id = Play::CreateGameObject(TYPE_LADDER, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			Play::GetGameObject(id).scale = SCALE;
		}

		if (sType == "TYPE_WALL")
		{
			id = Play::CreateGameObject(TYPE_WALL, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			GameObject& objWall = Play::GetGameObject(id);
			objWall.scale = SCALE;
			
			if (objWall.pos.x < 0)
				leftLimit = objWall.pos.x + WALL_LIMIT_OFFSET;
			else if (objWall.pos.x > 0)
				rightLimit = objWall.pos.x - WALL_LIMIT_OFFSET;

		}

		if (sType == "TYPE_FLOWER")
		{
			id = Play::CreateGameObject(TYPE_FLOWER, { std::stof(sX), std::stof(sY) }, 0, sSprite.c_str());
			Play::GetGameObject(id).scale = SCALE;
		}

		if (sType == "TYPE_EXIT")
			gameState.exitPos = { std::stof(sX), std::stof(sY) };

	}

	levelfile.close();

}

void CreateLevelList()
{
	int count = 0;
	bool finished = false;

	while (!finished)
	{
		std::string fname = "Level";
		if (count > 0) fname += std::to_string(count);
		fname += ".lev";

		//check if level file exists
		if (std::filesystem::exists(fname))
		{
			levelList.push_back(fname);
			count++;
		}
		else
			finished = true;
	}
}



























