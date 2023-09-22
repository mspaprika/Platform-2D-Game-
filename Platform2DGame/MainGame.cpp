#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;

const Point2D PLAYER_START_POS{ 60, DISPLAY_HEIGHT - 90 };

const Vector2D PLAYER_VELOCITY_DEFAULT{ 0, 0 };
const Vector2D PLAYER_VELOCITY_WALK{ 7, 0 };
const Vector2D PLAYER_VELOCITY_JUMP{ 2, -2 };

const Vector2D PLAYER_AABB{ 40, 40 };
CONST Vector2D PLATFORM_AABB{ 20, 20 };

const Vector2D GRAVITY{ 0, 2.f };
const float DELTA_TIME{ 0.016f };


enum PlayerState
{
	STATE_IDLE = 0,
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

struct Platform
{
	Vector2D groundFloorPos{ 0, DISPLAY_HEIGHT - 40 };
	int groundFloorWidth{ 45 };
	int platformSpacing{ 30 };

};


struct Flags
{
	bool playMode{ true };
	bool paused{ false };
	bool sound{ false };
	bool music{ false };
	bool levelPassed{ false };
	bool levelInfo{ true };
};

struct GameState
{
	int score{ 0 };
	int level{ 1 };
	int highScore{ 0 };

	Flags flags;
	PlayerState playerState = STATE_IDLE;
	PlayerState enemyState = STATE_IDLE;
	GameFlow state = STATE_PLAY;
};

GameState gameState;
Platform platform1;

void Draw();
void DrawGameStats();

void CreateGamePlay();
void CreatePlatform();

void UpdatePlatform();
void UpdatePlayer();
void WalkingPlayerControls();
void IdlePlayerControls();
void Jump();

bool IsPlayerColliding(const GameObject& object);

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

	int id = Play::CreateGameObject(TYPE_PLAYER, PLAYER_START_POS, 10, "cat_bury");
	GameObject& objPlayer = Play::GetGameObject(id);
	objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	objPlayer.scale = 3.5f;
}

void CreatePlatform()
{
	for (int i = 0; i < platform1.groundFloorWidth; i++)
	{
		int id = Play::CreateGameObject( 
			TYPE_PLATFORM, 
			Point2D{ platform1.groundFloorPos.x + (platform1.platformSpacing * i), platform1.groundFloorPos.y}, 
			10, 
			"spr_lava_begin" );
	}
}

void DrawGameStats()
{

	//std::vector <int> vAsteroids = Play::CollectGameObjectIDsByType(TYPE_ASTEROID);
	//int size = vAsteroids.size();
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	//GameObject& objSpecialAsteroid = Play::GetGameObjectByType(TYPE_SPECIAL);
	//Play::DrawFontText("64px", "SCORE: " + std::to_string(gameState.score), { DISPLAY_WIDTH - 200, 50 }, Play::CENTRE);
	//Play::DrawFontText("64px", "HIGHSCORE: " + std::to_string(gameState.highScore), { DISPLAY_WIDTH - 200, 100 }, Play::CENTRE);
	Play::DrawFontText("64px", "Cat state: " + std::to_string(gameState.playerState), { DISPLAY_WIDTH - 400, 100 }, Play::CENTRE);
	//Play::DrawFontText("64px", "Asteroid rotation: " + std::to_string(objSpecialAsteroid.rotation), { DISPLAY_WIDTH - 400, 200 }, Play::CENTRE);
	//Play::DrawFontText("105px", (gameState.powerActivated) ? "SHIELD ACTIVATED" : "", { DISPLAY_WIDTH / 2 , 150 }, Play::CENTRE);
	//Play::DrawFontText("132px", "Level: " + std::to_string(gameState.level), { DISPLAY_WIDTH / 2, 50 }, Play::CENTRE);
	//Play::DrawFontText("132px", (gameState.levelInfo) ? "LEVEL " + std::to_string(gameState.level) : "", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
	//Play::DrawFontText("64px", "Collisions: " + std::to_string(gameState.asteroidCollisions), { 300, 100 }, Play::CENTRE);
	//::DrawFontText("64px", "Gems: " + std::to_string(gameState.gemsCollected) + " of " + std::to_string(MAX_GEMS), { 350, 50 }, Play::CENTRE);
}

void UpdatePlatform()
{
	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);

	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		Play::UpdateGameObject(platform);
	}
}

void UpdatePlayer()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	switch (gameState.playerState)
	{
		case STATE_IDLE:
		{
			IdlePlayerControls();
			Play::SetSprite(objPlayer, "cat_sits", 0.1f);
			break;
		}
		case STATE_WALK:
		{
			WalkingPlayerControls();
			Play::SetSprite(objPlayer, "cat_sniffs_start", 1.5f);
			objPlayer.animSpeed = 0.1f;
			break;
		}
		case STATE_JUMP:
		{
			Jump();
			Play::SetSprite(objPlayer, "cat_jump", 1.5f);
			objPlayer.animSpeed = 0.05f;
			
			break;
		}
	}

	Play::UpdateGameObject(objPlayer);
}

void Jump()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	objPlayer.pos += objPlayer.velocity * DELTA_TIME;
	objPlayer.velocity += GRAVITY * DELTA_TIME;

	std::vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		if (objPlayer.pos.y >= PLAYER_START_POS.y)
		{
			objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
			objPlayer.pos = Vector2D{ objPlayer.oldPos.x, PLAYER_START_POS.y };
			gameState.playerState = STATE_IDLE;
		}
	}
}

void WalkingPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	if (Play::KeyDown(VK_RIGHT))
	{
		Play::SetSprite(objPlayer, "cat_mouth", 0.1f);
		objPlayer.pos.x += 5;
		objPlayer.animSpeed = 0.01f;
		//gameState.playerState = STATE_IDLE;
		
	}
	else if (Play::KeyDown(VK_LEFT))
	{
		Play::SetSprite(objPlayer, "cat_sniffs_start", 0.1f);
		objPlayer.pos.x -= 5;
		objPlayer.animSpeed = 0.01f;
		//gameState.playerState = STATE_IDLE;
	}
	if (Play::KeyDown(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP;
		Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
		objPlayer.animSpeed = 2.f;
		gameState.playerState = STATE_JUMP;
	}
	else
		objPlayer.animSpeed = 0.1f;
	
	gameState.playerState = STATE_IDLE;
}

void IdlePlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	if (Play::KeyDown(VK_RIGHT) || Play::KeyDown(VK_LEFT))
	{
		gameState.playerState = STATE_WALK;
	}
	if (Play::KeyDown(VK_SPACE))
	{
		objPlayer.velocity = PLAYER_VELOCITY_JUMP;
		Play::SetSprite(objPlayer, "cat_jump_high", 1.5f);
		gameState.playerState = STATE_JUMP;
	}
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
		Play::DrawObject(platform);
		//Play::DrawRect(platform.pos, PLATFORM_AABB, Play::cRed);
	}

	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	
	Play::DrawObjectRotated(objPlayer);
	//Play::DrawRect(objPlayer.pos, PLAYER_AABB, Play::cRed);

	
	DrawGameStats();

	Play::PresentDrawingBuffer();
}







