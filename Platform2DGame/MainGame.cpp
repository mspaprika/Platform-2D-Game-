#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"  

constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;

const int GROUND_FLOOR_HEIGHT{ DISPLAY_HEIGHT - 40 };
const int SECOND_FLOOR_HEIGHT{ DISPLAY_HEIGHT - 200 };
const int THIRD_FLOOR_HEIGHT{ DISPLAY_HEIGHT - 440 };

const Point2D PLAYER_START_POS{ 60, GROUND_FLOOR_HEIGHT - 70 };
const int PLAYER_POS_GROUND_FLOOR{ GROUND_FLOOR_HEIGHT - 70 };
const int PLAYER_POS_SECOND_FLOOR{ SECOND_FLOOR_HEIGHT - 60 };
const int PLAYER_POS_THIRD_FLOOR{ THIRD_FLOOR_HEIGHT - 60 };

const Point2D FLEA_START_POS{ 400, SECOND_FLOOR_HEIGHT - 35 };

const Vector2D PLAYER_VELOCITY_DEFAULT{ 0, 0 };
const Vector2D PLAYER_VELOCITY_WALK{ 10, 0 };

const Vector2D PLAYER_VELOCITY_JUMP{ 0, -8 };
const Vector2D PLAYER_VELOCITY_JUMP_LEFT{ -5, -8 };
const Vector2D PLAYER_VELOCITY_JUMP_RIGHT{ 5, -8 };

const float FALL_MULTIPLIER{ 7.0f };
const float LOW_JUMP_MULTIPLIER{ 5.5f };

const Vector2D PLAYER_VELOCITY_FALL_RIGHT{ 15, 0 };
const Vector2D PLAYER_VELOCITY_FALL_LEFT{ -15, 0 };

const Vector2D PLAYER_AABB{ 30, 45 };
const Vector2D PLAYER_AABB_BOTTOM{ 15, 48 };
const Vector2D PLAYER_AABB_UPPER{ 30, 25 };

const Vector2D PLATFORM_AABB{ 20, 20 };

const Vector2D GRAVITY_ACCELERATION{ 0, 0.9f };
const Vector2D GRAVITY{ 0, 8.f };
const Vector2D ACCELERATION{ 0, 0.5f };
const Vector2D FRICTION{ 0.5f, 0.5f };

Point2D cameraPos{ 0, 0 };

const float DELTA_TIME{ 0.016f };

float walkTimer = 0.f;
float jumpTimer = 0.f;
float gravityMultiplyer = 1.f;
Point2D platformPos{ 0, 0 };
Point2D ladderPos{ 0, 0 };

enum PlayerState
{
	STATE_GROUNDED = 0,
	STATE_WALK,
	STATE_JUMP,
	STATE_FALL,
	STATE_ATTACHED,
	STATE_DEAD,
};

enum GameObjectType
{
	TYPE_PLAYER= 0,
	TYPE_ENEMY,
	TYPE_FLEA,
	TYPE_PLATFORM,
	TYPE_LADDER,
	TYPE_TREAT,
	TYPE_SPECIAL,
	TYPE_BOX,
};

enum GameFlow
{
	STATE_PLAY = 0,
	STATE_PAUSE,
	STATE_GAMEOVER,
	STATE_LEVELCOMPLETE,
	STATE_VICTORY,
};

struct Platform1
{
	int platformSpacing{ 30 };
	int grassSpacing{ 35 };
	int ladderOffset{ grassSpacing + 5 };

	int groundFloorHeight{ GROUND_FLOOR_HEIGHT };
	int secondFloorHeight{ SECOND_FLOOR_HEIGHT };
	int thirdFloorHeight{ THIRD_FLOOR_HEIGHT };

	std::vector <int> groundFloorPosX{ -1000 };
	std::vector <int> groundFloorWidth{ 450 };

	std::vector <int> secondFloorPosX{ 350, 800 };
	std::vector <int> secondFloorWidth{ 10, 10 };	

	std::vector <int> thirdFloorPosX{ 150, 500, 900 };
	std::vector <int> thirdFloorWidth{ 3, 5, 5 };

	std::vector <int> ladderPosX{ 350 - ladderOffset + 1 };
	std::vector <int> ladderPosY{ GROUND_FLOOR_HEIGHT - ladderOffset };
	std::vector <int> ladderHeight{ 5 };
};

struct CatTreats
{
	std::vector <int> salmonPosX{ 180 };
	std::vector <int> salmonPosY{ THIRD_FLOOR_HEIGHT - 50 };

	std::vector <int> legPosX{ 520 };
	std::vector <int> legPosY{ THIRD_FLOOR_HEIGHT - 50 };
};

struct Flags
{
	bool playMode{ true };
	bool paused{ false };
	bool sound{ false };
	bool music{ false };
	bool levelPassed{ false };
	bool levelInfo{ true };
	bool right{ true };
	bool jumping{ false };
};

struct GameState
{
	int score{ 0 };
	int level{ 1 };
	int highScore{ 0 };
	int floor{ 0 };

	PlayerState playerState = STATE_GROUNDED;
	PlayerState enemyState = STATE_GROUNDED;
	GameFlow state = STATE_PLAY;
};

Flags flags;
GameState gameState;
Platform1 platform1;
CatTreats catTreats;

void Draw();
void DrawLevelOne();
void DrawGameStats();

void CreateGamePlay();
void CreatePlatform();
void LoopObject(GameObject& object);

void UpdatePlatform();
void UpdatePlayer();
void WalkingPlayerControls();
void IdlePlayerControls();
void AttachedPlayerControls();
void Jump();
void JumpControls();
void SetPlayerPos();
void SetFloor();
void Fall();
void WalkingDurationControl(float time);
void UpdateFleas();
void CatTreatsPlacement();

float DotProduct(const Vector2D& v1, const Vector2D& v2);
void Normalise(Vector2D& v);

bool IsPlayerColliding(const GameObject& object);
bool IsPlayerCollidingUpperPart(const GameObject& object);
bool IsPlayerCollidingBottomPart(const GameObject& object);
bool IsPlayerCollidingAnyPlatform();
bool IsPlayerCollidingLadder();
bool IsPlayerStillCollidingLadder();
bool IsPlayerOnLadder();

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground("Data//Backgrounds//green-background.png");	
	
	CreateGamePlay();
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	Point2D cameraPos = { objPlayer.pos.x - 600, 0 };
	Play::SetCameraPosition(cameraPos);

	cameraPos = Play::GetCameraPosition();

	switch (gameState.state)
	{
		case STATE_PLAY:
		{
			UpdatePlayer();
			UpdateFleas();
			break;
		}
		case STATE_PAUSE:
		{
			break;
		}
		case STATE_GAMEOVER:
		{
			break;
		}
		case STATE_LEVELCOMPLETE:
		{
			break;
		}
		case STATE_VICTORY:
		{
			break;
		}
	}

	WalkingDurationControl(elapsedTime);
	Draw();

	return Play::KeyDown( VK_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

void CreateGamePlay()
{
	CreatePlatform();
	CatTreatsPlacement();
	Play::MoveSpriteOrigin("cat_go_right", 0, -3);
	Play::MoveSpriteOrigin("cat_go_left", 0, -3);

	int id = Play::CreateGameObject(TYPE_PLAYER, PLAYER_START_POS, 10, "cat_sits_right");
	
	GameObject& objPlayer = Play::GetGameObject(id);
	objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	objPlayer.scale = 4.5f;

	id = Play::CreateGameObject(TYPE_FLEA, FLEA_START_POS, 10, "fleas");
	GameObject& objFlea = Play::GetGameObject(id);
	objFlea.scale = 3.5f;
	objFlea.animSpeed = .05f;
}

void CatTreatsPlacement()
{

	for (int i = 0; i < catTreats.salmonPosX.size(); i++)
	{
		int id = Play::CreateGameObject(
			TYPE_TREAT,
			Point2D{ catTreats.salmonPosX[i], catTreats.salmonPosY[i] },
			10,
			"salmon");
		GameObject& salmon = Play::GetGameObject(id);
		salmon.scale = 2.5f;
	}

	for (int i = 0; i < catTreats.legPosX.size(); i++)
	{
		int id = Play::CreateGameObject(
			TYPE_TREAT,
			Point2D{ catTreats.legPosX[i], catTreats.legPosY[i] },
			10,
			"leg");
		GameObject& leg = Play::GetGameObject(id);
		leg.scale = 2.5f;
	}	
}

void CreatePlatform()
{
	int j = 0;
	for (int width : platform1.groundFloorWidth)
	{
		for (int i = 0; i < platform1.groundFloorWidth[j]; i++)
		{
			int id = Play::CreateGameObject(
				TYPE_PLATFORM,
				Point2D{ platform1.groundFloorPosX[j] + (platform1.grassSpacing * i), platform1.groundFloorHeight},
				10,
				"the_ground");
			GameObject& platform = Play::GetGameObject(id);
			platform.scale = 1.2f;
		}
		j++;
	}

	j = 0;

	for (int width : platform1.secondFloorWidth)
	{
		for (int i = 0; i < platform1.secondFloorWidth[j]; i++)
		{
			int id = Play::CreateGameObject(
				TYPE_PLATFORM,
				Point2D{ platform1.secondFloorPosX[j] + (platform1.grassSpacing * i), platform1.secondFloorHeight },
				10,
				"grass_like_ground");
			GameObject& platform = Play::GetGameObject(id);
			platform.scale = 2.5f;
		}
		j++;
	}

	j = 0;

	for (int width : platform1.thirdFloorWidth)
	{
		for (int i = 0; i < platform1.thirdFloorWidth[j]; i++)
		{
			int id = Play::CreateGameObject(
				TYPE_PLATFORM,
				Point2D{ platform1.thirdFloorPosX[j] + (platform1.grassSpacing * i), platform1.thirdFloorHeight },
				10,
				"grass_like_ground");
			GameObject& platform = Play::GetGameObject(id);
			platform.scale = 2.5f;
		}
		j++;
	}

	j = 0;

	for (int height : platform1.ladderHeight)
	{
		for (int i = 0; i < platform1.ladderHeight[j]; i++)
		{
			int id = Play::CreateGameObject(
				TYPE_LADDER,
				Point2D{ platform1.ladderPosX[j], platform1.ladderPosY[j] - (platform1.platformSpacing * i)},
				10,
				"climb_grass");
			GameObject& ladder = Play::GetGameObject(id);
			ladder.scale = 2.5f;
		}
	}
}

void DrawGameStats()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	GameObject& objFlea = Play::GetGameObjectByType(TYPE_FLEA);

	Play::SetDrawingSpace(Play::SCREEN);
	
	Play::DrawFontText("64px", "floor: " + std::to_string(gameState.floor), { DISPLAY_WIDTH - 200, 50 }, Play::CENTRE);
	Play::DrawFontText("64px", "cat vx: " + std::to_string(objPlayer.velocity.x), { DISPLAY_WIDTH - 150, 100 }, Play::CENTRE);
	Play::DrawFontText("64px", "cat vy: " + std::to_string(objPlayer.velocity.y), { DISPLAY_WIDTH - 150, 150 }, Play::CENTRE);
	Play::DrawFontText("64px", "Cat state: " + std::to_string(gameState.playerState), { DISPLAY_WIDTH - 400, 50 }, Play::CENTRE);
	//Play::DrawFontText("64px", "CAMERA: " + std::to_string(cameraPos.y), { DISPLAY_WIDTH - 400, 100 }, Play::CENTRE);
	//Play::DrawFontText("105px", (gameState.powerActivated) ? "SHIELD ACTIVATED" : "", { DISPLAY_WIDTH / 2 , 150 }, Play::CENTRE);
	//Play::DrawFontText("132px", "Level: " + std::to_string(gameState.level), { DISPLAY_WIDTH / 2, 50 }, Play::CENTRE);
	//Play::DrawFontText("132px", (gameState.levelInfo) ? "LEVEL " + std::to_string(gameState.level) : "", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
	//Play::DrawFontText("64px", "Collisions: " + std::to_string(gameState.asteroidCollisions), { 300, 100 }, Play::CENTRE);

	Play::SetDrawingSpace(Play::WORLD);
}

void UpdatePlatform()
{
	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);

		if (IsPlayerColliding(platform) && jumpTimer > 0.5f)
		{
			gameState.playerState = STATE_GROUNDED;
			objPlayer.pos = platform.pos;
			jumpTimer = 0.f;
		}
		

		Play::UpdateGameObject(platform);
	}
}

void UpdatePlayer()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	SetFloor();
	//LoopObject(objPlayer);

	switch (gameState.playerState)
	{           
		case STATE_GROUNDED:
		{
			IdlePlayerControls();
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
	}
	Play::UpdateGameObject(objPlayer);
}

void UpdateFleas()
{
	std::vector <int> vFleas = Play::CollectGameObjectIDsByType(TYPE_FLEA);

	for (int fleaId : vFleas)
	{
		GameObject& flea = Play::GetGameObject(fleaId);
		LoopObject(flea);

		Play::UpdateGameObject(flea);
	}
}

void WalkingDurationControl(float time)
{
	if (gameState.playerState == STATE_WALK)
	{
		if (walkTimer > 0.7f)
		{
			gameState.playerState = STATE_GROUNDED;
			walkTimer = 0.f;
		}
		else
			walkTimer += time;
	}
}

void Jump()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (IsPlayerCollidingLadder())
		gameState.playerState = STATE_ATTACHED;

	

	if (flags.right)
		Play::SetSprite(objPlayer, "cat_jump_right", 0.05f);                 
	else
		Play::SetSprite(objPlayer, "cat_jump_left", 0.05f);


	if (jumpTimer < 1.0f)
		jumpTimer += DELTA_TIME;

	if (!flags.right)		
		objPlayer.pos.x -= objPlayer.velocity.x * DELTA_TIME;
	else
		objPlayer.pos.x += objPlayer.velocity.x * DELTA_TIME;

	if (objPlayer.velocity.y < 0 && !Play::KeyDown(VK_SPACE))
		gravityMultiplyer = LOW_JUMP_MULTIPLIER;
	else
		gravityMultiplyer = 1.f;

	
	if (objPlayer.velocity.y < 0)
		objPlayer.velocity += GRAVITY * DELTA_TIME * gravityMultiplyer;
	else
		objPlayer.velocity += GRAVITY * FALL_MULTIPLIER * DELTA_TIME * gravityMultiplyer;	



	std::vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);

		if (IsPlayerCollidingUpperPart(platform))
			gameState.playerState = STATE_FALL;
		
		if (IsPlayerCollidingBottomPart(platform) && jumpTimer > 0.3f)
		{	
			objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
			gameState.playerState = STATE_GROUNDED;
			SetPlayerPos();
			jumpTimer = 0.f;
 		}	
	}
 
	if (IsPlayerStillCollidingLadder())
	{
		gameState.playerState = STATE_GROUNDED;
		objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	} 
}

void SetPlayerPos()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (gameState.floor == 3)
		objPlayer.pos.y = PLAYER_POS_THIRD_FLOOR;
	else if (gameState.floor == 2)
		objPlayer.pos.y = PLAYER_POS_SECOND_FLOOR;
	else if (gameState.floor == 1)
		objPlayer.pos.y = PLAYER_POS_GROUND_FLOOR;
}

void SetFloor()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (objPlayer.oldPos.y <= platform1.thirdFloorHeight)
		gameState.floor = 3;
	else if (objPlayer.oldPos.y <= platform1.secondFloorHeight)
		gameState.floor = 2;
	else if (objPlayer.oldPos.y <= platform1.groundFloorHeight)
		gameState.floor = 1;
}

void WalkingPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	
	if (flags.right)
		Play::SetSprite(objPlayer, "cat_go_right", 0.5f);
	else
		Play::SetSprite(objPlayer, "cat_go_left", 0.5f);


	if (!IsPlayerCollidingAnyPlatform() && !IsPlayerStillCollidingLadder() && objPlayer.pos.y < PLAYER_START_POS.y)
		gameState.playerState = STATE_FALL;

	if (IsPlayerCollidingLadder())
		gameState.playerState = STATE_ATTACHED;

 	if (Play::KeyDown(VK_RIGHT))
	{   
		objPlayer.pos.x += 10;
		flags.right = true;	
	}
	else if (Play::KeyDown(VK_LEFT))
	{
		objPlayer.pos.x -= 10;
		flags.right = false;
	}

	JumpControls();
}

void IdlePlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	objPlayer.rotation = Play::DegToRad(0);

	if (flags.right)
		Play::SetSprite(objPlayer, "cat_sits_right", 0.1f);
	else
		Play::SetSprite(objPlayer, "cat_sits_left", 0.1f);

	if (Play::KeyDown(VK_RIGHT))
		gameState.playerState = STATE_WALK;
	else if (Play::KeyDown(VK_LEFT))
		gameState.playerState = STATE_WALK;

	JumpControls();

	Play::UpdateGameObject(objPlayer);
}

void JumpControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (Play::KeyDown(VK_LEFT) && Play::KeyPressed(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP_LEFT;
		gameState.playerState = STATE_JUMP;
	}
	else if (Play::KeyDown(VK_RIGHT) && Play::KeyPressed(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP_RIGHT;
		gameState.playerState = STATE_JUMP;
	}
	else if (Play::KeyPressed(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP;
		gameState.playerState = STATE_JUMP;
	}
}

void AttachedPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	
	
	objPlayer.animSpeed = 0.f;

	if (!IsPlayerCollidingLadder() && !IsPlayerOnLadder())
		gameState.playerState = STATE_FALL;

	if (!IsPlayerCollidingLadder())
	{
		gameState.playerState = STATE_GROUNDED;
		objPlayer.pos = { ladderPos.x, ladderPos.y - 55 };
	} 

	if (flags.right)
	{
		objPlayer.rotation = Play::DegToRad(-90);
		Play::SetSprite(objPlayer, "cat_walk_right", 0.f);
		//objPlayer.pos.x = ladderPos.x - PLATFORM_AABB.x;
	}
	else
	{
		objPlayer.rotation = Play::DegToRad(90);
		Play::SetSprite(objPlayer, "cat_walk_left", 0.f);
		//objPlayer.pos.x = ladderPos.x + PLATFORM_AABB.x;
	}

	if (Play::KeyDown(VK_UP))
	{
		objPlayer.pos.y -= 5;
		objPlayer.animSpeed = 0.1f;
	}
	else if (Play::KeyDown(VK_DOWN))
	{
		objPlayer.pos.y += 5;	
		objPlayer.animSpeed = 0.1f;
	}
	/*else if (Play::KeyDown(VK_LEFT) && flags.right)
	{
		objPlayer.velocity = { -10, 10 };
	}
	else if (Play::KeyDown(VK_RIGHT) && !flags.right)
	{
		objPlayer.velocity = { 10, 10 };
	}*/
}

void Fall()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (!flags.right)
		Play::SetSprite(objPlayer, "cat_look_left", 0.1f);
	else
		Play::SetSprite(objPlayer, "cat_look_right", 0.1f);


	if (!flags.right)
		objPlayer.velocity += PLAYER_VELOCITY_FALL_LEFT * DELTA_TIME;
	else
		objPlayer.velocity += PLAYER_VELOCITY_FALL_RIGHT * DELTA_TIME;

	objPlayer.acceleration = GRAVITY_ACCELERATION;

	if (IsPlayerCollidingAnyPlatform())
	{
		SetPlayerPos();
		gameState.playerState = STATE_GROUNDED;
		objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
		objPlayer.acceleration = PLAYER_VELOCITY_DEFAULT;
	}
}

bool IsPlayerOnLadder()
{
	std::vector <int> vLadders = Play::CollectGameObjectIDsByType(TYPE_LADDER);

	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	for (int ladderId : vLadders)
	{
		GameObject& ladder = Play::GetGameObject(ladderId);

		if (flags.right)
		{
			if (ladder.pos.x + PLATFORM_AABB.x > objPlayer.pos.x
				&& ladder.pos.x - PLATFORM_AABB.x < objPlayer.pos.x + PLAYER_AABB_UPPER.x)
			{
				return true;
			}
		}
		else
		{
			if (ladder.pos.x + PLATFORM_AABB.x > objPlayer.pos.x - PLAYER_AABB_UPPER.x
				&& ladder.pos.x - PLATFORM_AABB.x < objPlayer.pos.x)
			{
				return true;
			}
		}
	}
	return false;
}

bool IsPlayerCollidingLadder()
{
	std::vector <int> vLadders = Play::CollectGameObjectIDsByType(TYPE_LADDER);

	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

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
	std::vector <int> vLadders = Play::CollectGameObjectIDsByType(TYPE_LADDER);

	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	for (int ladderId : vLadders)
	{
		GameObject& ladder = Play::GetGameObject(ladderId);

		if (IsPlayerCollidingBottomPart(ladder))
		{
			return true;
		}
	}
	return false;
}

bool IsPlayerCollidingAnyPlatform()
{
	std::vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		if (IsPlayerCollidingBottomPart(platform))
		{
			return true;
		}
	}
	return false;
}

bool IsPlayerCollidingUpperPart(const GameObject& object)
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	
	Vector2D AABB = PLAYER_AABB_UPPER;

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

bool IsPlayerCollidingBottomPart(const GameObject& object  )
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	Vector2D AABB = PLATFORM_AABB;

	if (objPlayer.pos.y + PLAYER_AABB_BOTTOM.y <= object.pos.y + AABB.y &&
		objPlayer.pos.y + PLAYER_AABB_BOTTOM.y > object.pos.y - AABB.y)
	{
		if (objPlayer.pos.x > object.pos.x - AABB.x &&
			objPlayer.pos.x < object.pos.x + AABB.x)
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

void LoopObject(GameObject& object)
{
	if(object.pos.x < 0)
		object.pos.x = DISPLAY_WIDTH;
	else if (object.pos.x > DISPLAY_WIDTH)
		object.pos.x = 0;
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

void Draw()
{
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();

	if (gameState.level == 1)
		DrawLevelOne();


	DrawGameStats();

	Play::PresentDrawingBuffer();
}

void DrawLevelOne()
{

	std::vector <int> vLadders = Play::CollectGameObjectIDsByType(TYPE_LADDER);
	for (int ladderId : vLadders)
	{
		GameObject& ladder = Play::GetGameObject(ladderId);
		Play::DrawObjectRotated(ladder);
	}

	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		//Play::DrawRect(Point2D{ platform.pos.x - PLATFORM_AABB.x, platform.pos.y - PLATFORM_AABB.y }, Point2D{ platform.pos.x + PLATFORM_AABB.x, platform.pos.y + PLATFORM_AABB.y }, Play::cWhite);
		Play::DrawObjectRotated(platform);
	}

	std::vector <int> vTreats = Play::CollectGameObjectIDsByType(TYPE_TREAT);
	for (int treatId : vTreats)
	{
		GameObject& treat = Play::GetGameObject(treatId);
		Play::DrawObjectRotated(treat);
	}
	

	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	std::vector <int> vFleas = Play::CollectGameObjectIDsByType(TYPE_FLEA);

	for (int fleaId : vFleas)
	{
		GameObject& flea = Play::GetGameObject(fleaId);
		Play::DrawObjectRotated(flea);
	}

	Play::DrawObjectRotated(objPlayer);
	//Play::DrawRect(Point2D{ objPlayer.pos.x - PLAYER_AABB_BOTTOM.x, objPlayer.pos.y - PLAYER_AABB_BOTTOM.y }, Point2D{ objPlayer.pos.x + PLAYER_AABB_BOTTOM.x, objPlayer.pos.y + PLAYER_AABB_BOTTOM.y }, Play::cRed);
	//Play::DrawRect(Point2D{ objPlayer.pos.x, objPlayer.pos.y - PLAYER_AABB_UPPER.y }, Point2D{ objPlayer.pos.x + PLAYER_AABB_UPPER.x, objPlayer.pos.y + PLAYER_AABB_UPPER.y }, Play::cWhite);
	
}







