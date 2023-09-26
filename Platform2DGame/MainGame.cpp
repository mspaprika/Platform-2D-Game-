#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"  

constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;

const Point2D PLAYER_START_POS{ 60, DISPLAY_HEIGHT - 110 };

const Vector2D PLAYER_VELOCITY_DEFAULT{ 0, 0 };
const Vector2D PLAYER_VELOCITY_WALK{ 7, 0 };
const Vector2D PLAYER_VELOCITY_JUMP{ 0, -5 };
const Vector2D PLAYER_VELOCITY_JUMP_LEFT{ -2, -5 };
const Vector2D PLAYER_VELOCITY_JUMP_RIGHT{ 2, -5 };

const Vector2D PLAYER_AABB{ 30, 45 };
const Vector2D PLAYER_AABB_DOWN{ 30, 48 };
const Vector2D PLAYER_AABB_UP{ 30, 35 };

const Vector2D PLATFORM_AABB{ 20, 20 };

const Vector2D GRAVITY{ 0, 5.f };
const Vector2D ACCELERATION{ 0, 0.5f };
const Vector2D FRICTION{ 0.5f, 0.5f };
const float DELTA_TIME{ 0.016f };

bool collision = false;
bool grounded = false;
float walkTimer = 0.f;
float jumpTimer = 0.f;
Point2D platformPos{ 0, 0 };


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
	TYPE_PLATFORM,
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
	int grassSpacing{ 25 };

	int groundFloorHeight{ DISPLAY_HEIGHT - 40 };
	int secondFloorHeight{ DISPLAY_HEIGHT - 200 };
	int thirdFloorHeight{ DISPLAY_HEIGHT - 440 };

	std::vector <int> groundFloorPosX{ 0 };
	std::vector <int> groundFloorWidth{ 45 };

	std::vector <int> secondFloorPosX{ 350, 800 };
	std::vector <int> secondFloorWidth{ 15, 10 };	

	std::vector <int> thirdFloorPosX{ 150, 500, 900 };
	std::vector <int> thirdFloorWidth{ 3, 5, 5 };
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

void Draw();
void DrawGameStats();

void CreateGamePlay();
void CreatePlatform();

void UpdatePlatform();
void UpdatePlayer();
void WalkingPlayerControls();
void IdlePlayerControls();
void Jump();

float DotProduct(const Vector2D& v1, const Vector2D& v2);
void Normalise(Vector2D& v);

bool IsPlayerColliding(const GameObject& object);
bool IsPlayerCollidingAnyPlatform();

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

	switch (gameState.state)
	{
		case STATE_PLAY:
		{
			//UpdatePlatform();
			UpdatePlayer();

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

	//UpdatePlatform();

	if (gameState.playerState == STATE_WALK)
	{
		if (walkTimer > 0.5f)
		{
			gameState.playerState = STATE_GROUNDED;
			walkTimer = 0.f;
		}
		else
			walkTimer += elapsedTime;
	}
	
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
	Play::MoveSpriteOrigin("cat_go_right", 0, -3);
	Play::MoveSpriteOrigin("cat_go_left", 0, -3);

	int id = Play::CreateGameObject(TYPE_PLAYER, PLAYER_START_POS, 10, "cat_sits_right");
	GameObject& objPlayer = Play::GetGameObject(id);
	objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	objPlayer.scale = 4.5f;
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
				Point2D{ platform1.groundFloorPosX[j] + (platform1.platformSpacing * i), platform1.groundFloorHeight},
				10,
				"ground");
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
				"gras");
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
				"gras");
			GameObject& platform = Play::GetGameObject(id);
			platform.scale = 2.5f;
		}
		j++;
	}
}

void DrawGameStats()
{

	//std::vector <int> vAsteroids = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);
	//int size = vAsteroids.size();
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	//GameObject& objSpecialAsteroid = Play::GetGameObjectByType(TYPE_SPECIAL);
	Play::DrawFontText("64px", "floor: " + std::to_string(gameState.floor), { DISPLAY_WIDTH - 200, 50 }, Play::CENTRE);
	Play::DrawFontText("64px", "cat velocity x: " + std::to_string(objPlayer.velocity.x), { DISPLAY_WIDTH - 200, 100 }, Play::CENTRE);
	Play::DrawFontText("64px", "Cat state: " + std::to_string(gameState.playerState), { DISPLAY_WIDTH - 400, 100 }, Play::CENTRE);
	Play::DrawFontText("64px", "RIGHT: " + std::to_string(flags.right), { DISPLAY_WIDTH - 400, 200 }, Play::CENTRE);
	//Play::DrawFontText("105px", (gameState.powerActivated) ? "SHIELD ACTIVATED" : "", { DISPLAY_WIDTH / 2 , 150 }, Play::CENTRE);
	//Play::DrawFontText("132px", "Level: " + std::to_string(gameState.level), { DISPLAY_WIDTH / 2, 50 }, Play::CENTRE);
	//Play::DrawFontText("132px", (gameState.levelInfo) ? "LEVEL " + std::to_string(gameState.level) : "", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
	//Play::DrawFontText("64px", "Collisions: " + std::to_string(gameState.asteroidCollisions), { 300, 100 }, Play::CENTRE);
	//::DrawFontText("64px", "Gems: " + std::to_string(gameState.gemsCollected) + " of " + std::to_string(MAX_GEMS), { 350, 50 }, Play::CENTRE);
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
			collision = true;
			objPlayer.pos = platform.pos;
			jumpTimer = 0.f;
		}
		

		Play::UpdateGameObject(platform);
	}
}

void UpdatePlayer()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);


	if (objPlayer.oldPos.y <= platform1.thirdFloorHeight)
		gameState.floor = 3;
	else if (objPlayer.oldPos.y <= platform1.secondFloorHeight)
		gameState.floor = 2;
	else if (objPlayer.oldPos.y <= platform1.groundFloorHeight)
		gameState.floor = 1;


	switch (gameState.playerState)
	{           
		case STATE_GROUNDED:
		{
			IdlePlayerControls();

			if (Play::KeyDown(VK_LEFT) && Play::KeyDown(VK_SPACE))
			{
				objPlayer.velocity = PLAYER_VELOCITY_JUMP_LEFT;
				objPlayer.velocity.x = -2;
				Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
				gameState.playerState = STATE_JUMP;

			}
			else if (Play::KeyDown(VK_RIGHT) && Play::KeyDown(VK_SPACE))
			{
				objPlayer.velocity = PLAYER_VELOCITY_JUMP_RIGHT;
				objPlayer.velocity.x = 2;
				Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
				gameState.playerState = STATE_JUMP;
			}

			if (flags.right)
				Play::SetSprite(objPlayer, "cat_sits_right", 0.1f);
			else
				Play::SetSprite(objPlayer, "cat_sits_left", 0.1f);
			break;
		}
		
		case STATE_WALK:
		{
			WalkingPlayerControls();

			if (Play::KeyDown(VK_LEFT) && Play::KeyDown(VK_SPACE))
			{
				objPlayer.velocity = PLAYER_VELOCITY_JUMP_LEFT;
				objPlayer.velocity.x = -2;
				Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
				gameState.playerState = STATE_JUMP;

			}
			else if (Play::KeyDown(VK_RIGHT) && Play::KeyDown(VK_SPACE))
			{
				objPlayer.velocity = PLAYER_VELOCITY_JUMP_RIGHT;
				objPlayer.velocity.x = 2;
				Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
				gameState.playerState = STATE_JUMP;
			}

			if (!IsPlayerCollidingAnyPlatform() && objPlayer.pos.y < PLAYER_START_POS.y)
				gameState.playerState = STATE_FALL;

			if (flags.right)
				Play::SetSprite(objPlayer, "cat_go_right", 1.5f);
			else
				Play::SetSprite(objPlayer, "cat_go_left", 1.5f);
			
			objPlayer.animSpeed = 0.1f;

			break;
		}

		case STATE_FALL:
		{
			objPlayer.velocity += GRAVITY * DELTA_TIME;

			if (IsPlayerCollidingAnyPlatform())
			{
				if (gameState.floor == 3)
					objPlayer.pos.y = platform1.thirdFloorHeight - 60;
				else if (gameState.floor == 2)
					objPlayer.pos.y = platform1.secondFloorHeight - 60;
				else if (gameState.floor == 1)
					objPlayer.pos.y = platform1.groundFloorHeight - 70;


				gameState.playerState = STATE_GROUNDED;

				objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
			}

			break;
		}
		case STATE_JUMP:
		{
			Jump();
			//JumpControls();
			if (flags.right)
				Play::SetSprite(objPlayer, "cat_jump", 1.5f);
			else
				Play::SetSprite(objPlayer, "cat_jump_left", 1.5f);  

			objPlayer.animSpeed = 0.05f;
			
			break;
		}
	}
	Play::UpdateGameObject(objPlayer);
}

void Jump()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (jumpTimer < 1.0f)
		jumpTimer += DELTA_TIME;

	if (!flags.right)		
		objPlayer.pos.x = objPlayer.pos.x - objPlayer.velocity.x * DELTA_TIME;
	else
		objPlayer.pos.x += objPlayer.velocity.x * DELTA_TIME;
	
	//objPlayer.acceleration = ACCELERATION - FRICTION;

	objPlayer.velocity += GRAVITY * DELTA_TIME;
	

	std::vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		
		if (IsPlayerColliding(platform) && jumpTimer > 1.0f)
		{	
			objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
			gameState.playerState = STATE_GROUNDED;
			jumpTimer = 0.f;
		}	
	}
}

void WalkingPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	if (Play::KeyDown(VK_RIGHT))
	{
		Play::SetSprite(objPlayer, "cat_go_right", 0.f);
		objPlayer.pos.x += 5;
		flags.right = true;	
	}
	else if (Play::KeyDown(VK_LEFT))
	{
		Play::SetSprite(objPlayer, "cat_go_left", 0.f);
		objPlayer.pos.x -= 5;
		flags.right = false;
	}
	
	if (Play::KeyDown(VK_LEFT) && Play::KeyDown(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP_LEFT;
		objPlayer.velocity.x = -2;
		Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
		gameState.playerState = STATE_JUMP;

	}
	else if (Play::KeyDown(VK_RIGHT) && Play::KeyDown(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP_RIGHT;
		objPlayer.velocity.x = 2;
		Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
		gameState.playerState = STATE_JUMP;
	}
	else if (Play::KeyDown(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP;

		if (flags.right)
			Play::SetSprite(objPlayer, "cat_jump_left", 1.5f);
		else
			Play::SetSprite(objPlayer, "cat_jump", 1.5f);

		objPlayer.animSpeed = 2.f;
		gameState.playerState = STATE_JUMP;
	}
}

void IdlePlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	if (Play::KeyDown(VK_RIGHT))
		gameState.playerState = STATE_WALK;
	else if (Play::KeyDown(VK_LEFT))
		gameState.playerState = STATE_WALK;


	if (Play::KeyDown(VK_LEFT) && Play::KeyDown(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP_LEFT;
		objPlayer.velocity.x = -2;
		Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
		gameState.playerState = STATE_JUMP;
	}
	else if (Play::KeyDown(VK_RIGHT) && Play::KeyDown(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP_RIGHT;
		objPlayer.velocity.x = 2;
		Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
		gameState.playerState = STATE_JUMP;
	}
	else if (Play::KeyDown(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP;
		Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
		gameState.playerState = STATE_JUMP;
	}

	Play::UpdateGameObject(objPlayer);
}

bool IsPlayerCollidingAnyPlatform()
{
	std::vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		if (IsPlayerColliding(platform))
		{
			//platformPos = platform.pos;
			collision = true;
			return true;
		}
	}
	collision = false;
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

	if (objPlayer.pos.y - PLAYER_AABB.y < object.pos.y + AABB.y &&
		objPlayer.pos.y + PLAYER_AABB.y > object.pos.y - AABB.y)
	{
		if (objPlayer.pos.x + PLAYER_AABB.x > object.pos.x - AABB.x &&
			objPlayer.pos.x - PLAYER_AABB.x < object.pos.x + AABB.x)
		{			
			return true;
		}
	}
	return false;
}

void Draw()
{
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();

	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		Play::DrawRect(Point2D{ platform.pos.x - PLATFORM_AABB.x, platform.pos.y - PLATFORM_AABB.y }, Point2D{ platform.pos.x + PLATFORM_AABB.x, platform.pos.y + PLATFORM_AABB.y }, Play::cWhite);
		Play::DrawObjectRotated(platform);
		//Play::DrawRect(platform.pos, PLATFORM_AABB, Play::cRed);
	}

	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	Play::DrawRect(Point2D{ objPlayer.pos.x - PLAYER_AABB.x, objPlayer.pos.y - PLAYER_AABB.y }, Point2D{ objPlayer.pos.x + PLAYER_AABB.x, objPlayer.pos.y + PLAYER_AABB.y }, Play::cRed);

	
	Play::DrawObjectRotated(objPlayer);
	//Play::DrawRect(objPlayer.pos, PLAYER_AABB, Play::cRed);

	
	DrawGameStats();

	Play::PresentDrawingBuffer();
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







