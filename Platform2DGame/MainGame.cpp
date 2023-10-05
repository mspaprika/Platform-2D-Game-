#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"  

using namespace std;

constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;

const float DELTA_TIME{ 0.016f }; // seconds

const int COLLISION_BOOST{ 25 }; // pixels

const int CAMERA_OFFSET_X{ 600 }; // pixels
const int PLAYER_OFFSET_Y{ 8 }; // pixels
const int BOX_OFFSET_Y{ 20 }; // pixels
const int WALL_OFFSET{ 20 }; // pixels
const int FLOOR_OFFSET{ 18 }; // pixels
const int LADDER_OFFSET_Y{ 30 }; // pixels
const int LADDER_OFFSET_X{ 5 }; // pixels
const int TILE_SIZE{ 38 }; // pixels

// other measurments are in tiles
// width and height in tiles, rounded to the bigger side so all screen is covered
const int TILE_HEIGHT{ 19 };
const int TILE_WIDTH{ 34 };

const Point2D CAMERA_THRESHOLD_X{ -15, 42 };
const int CAMERA_THRESHOLD_Y{ 5 };

const int GROUND_FLOOR_HEIGHT{ 18 };
const int SECOND_FLOOR_HEIGHT{ 14 };
const int THIRD_FLOOR_HEIGHT{ 9 };
const int FOURTH_FLOOR_HEIGHT{ 4 };
const int FIFTH_FLOOR_HEIGHT{ 2 };
const int SIXTH_FLOOR_HEIGHT{ -5 };

const Point2D PLAYER_START_POS{ 2, GROUND_FLOOR_HEIGHT - 2 };
const int PLAYER_POS_GROUND_FLOOR{ GROUND_FLOOR_HEIGHT - 2 };
const int PLAYER_POS_SECOND_FLOOR{ SECOND_FLOOR_HEIGHT - 3 };
const int PLAYER_POS_THIRD_FLOOR{ THIRD_FLOOR_HEIGHT - 3 };
const int PLAYER_POS_FOURTH_FLOOR{ FOURTH_FLOOR_HEIGHT - 3 };
const int PLAYER_POS_FIFTH_FLOOR{ FIFTH_FLOOR_HEIGHT - 3 };
const int PLAYER_POS_SIXTH_FLOOR{ SIXTH_FLOOR_HEIGHT - 3 };

const Point2D FLEA_START_POS{ 4, SECOND_FLOOR_HEIGHT - 3 };

const Vector2D PLAYER_VELOCITY_DEFAULT{ 0, 0 };
const Vector2D PLAYER_VELOCITY_WALK{ 10, 0 };

const Vector2D PLAYER_VELOCITY_JUMP{ 0, -9 };
const Vector2D PLAYER_VELOCITY_JUMP_LEFT{ -3, -9 }; 
const Vector2D PLAYER_VELOCITY_JUMP_RIGHT{ 3, -9 };

const float FALL_MULTIPLIER{ 2.5f };
const float LOW_JUMP_MULTIPLIER{ 3.5f };

const Vector2D PLAYER_VELOCITY_FALL_RIGHT{ 1, 2 };
const Vector2D PLAYER_VELOCITY_FALL_LEFT{ -1, 2 };

const Vector2D PLAYER_AABB{ 30, 45 };
const Vector2D PLAYER_AABB_BOTTOM{ 20, 48 };
const Vector2D PLAYER_AABB_UPPER{ 35, 25 };
const Vector2D TREATS_AABB{ 20, 20 };

const Vector2D GROUND_PLATFORM_AABB{ 20, 60 };
const Vector2D PLATFORM_AABB{ 20, 20 };
const Vector2D WALL_AABB{ 60, 20 };

const Vector2D GRAVITY_ACCELERATION{ 0, 0.3f };
const Vector2D GRAVITY{ 0, 10.f };
const Vector2D ACCELERATION{ 0, 0.3f };
const Vector2D FRICTION{ 0.5f, 0.5f };

Point2D cameraPos{ 0, 0 };
Point2D platformPos{ 0, 0 };
Point2D ladderPos{ 0, 0 };

int groundedPosY = 0;

float walkTimer = 0.f;
float jumpTimer = 0.f;
float animationTimer = 0.f;
float coyoteTimer = 0.f;
float gravityMultiplyer = 1.f;

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
	TYPE_PLAYER = 0,
	TYPE_ENEMY,
	TYPE_FLEA,
	TYPE_PLATFORM,
	TYPE_GROUND,
	TYPE_LADDER,
	TYPE_TREAT,
	TYPE_SPECIAL,
	TYPE_BOX,
	TYPE_WALL,
	TYPE_DESTROYED,
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
	const Vector2D WALL_OFFSET{ 20, 20 };
	const Vector2D GROUND_FLOOR_POS{ LIMIT_LEFT, GROUND_FLOOR_HEIGHT };

	// position of the top tile top edge of the ladder
	const int LADDER_TOP_FLOOR_SECOND{ PLAYER_POS_SECOND_FLOOR + TILE_SIZE };
	const int LADDER_TOP_FLOOR_THIRD{ PLAYER_POS_THIRD_FLOOR + TILE_SIZE  };
	const int LADDER_TOP_FLOOR_FOURTH{ PLAYER_POS_FOURTH_FLOOR + TILE_SIZE  };
	const int LADDER_TOP_FLOOR_FIFTH{ PLAYER_POS_FIFTH_FLOOR + TILE_SIZE  };
	const int LADDER_TOP_FLOOR_SIXTH{ PLAYER_POS_SIXTH_FLOOR + TILE_SIZE  };

	// limits for all platforms
	const int LIMIT_LEFT{ -30 };
	const int LIMIT_RIGHT{ 60 };
	const int LIMIT_TOP{ 100 };

	const int GROUND_FLOOR_HEIGHT{ 18 };
	const int SECOND_FLOOR_HEIGHT{ 13 };
	const int THIRD_FLOOR_HEIGHT{ 8 };
	const int FOURTH_FLOOR_HEIGHT{ 3 };
	const int FIFTH_FLOOR_HEIGHT{ 0 };
	const int SIXTH_FLOOR_HEIGHT{ -4 };

	// side walls at limits
	const int LEFT_WALL_POS_X{ LIMIT_LEFT };
	const int RIGHT_WALL_POS_X{ LIMIT_RIGHT };
	const int WALL_HEIGHT{ LIMIT_TOP };

	vector <int> GROUND_FLOOR_WIDTH{  };
	vector <int> GROUND_FLOOR_POS_X{  };
	vector <int> GROUND_FLOOR_POS_Y{  };

	vector <int> GRASS_WIDTH{  };
	vector <int> GRASS_FLOOR_POS_X{  };
	vector <int> GRASS_FLOOR_POS_Y{  };

	vector <int> DIRT_POS_X{  };
	vector <int> DIRT_POS_Y{  };
	vector <int> DIRT_HEIGHT{  };
	vector <int> DIRT_WIDTH{  };

	vector <int> BRICK_WALL_POS_X{  };
	vector <int> BRICK_WALL_POS_Y{  };
	vector <int> BRICK_WALL_HEIGHT{  };
	vector <int> BRICK_WALL_WIDTH{  };

	vector <int> BRICK_FLOOR_WIDTH{  };
	vector <int> BRICK_FLOOR_POS_X{  };
	vector <int> BRICK_FLOOR_POS_Y{  };

	vector <int> SAND_WALL_POS_X{  };
	vector <int> SAND_WALL_POS_Y{  };
	vector <int> SAND_WALL_HEIGHT{  };
	vector <int> SAND_WALL_WIDTH{  };

	vector <int> SAND_FLOOR_WIDTH{  };
	vector <int> SAND_FLOOR_POS_X{  };
	vector <int> SAND_FLOOR_POS_Y{  };

	vector <int> LADDER_POS_X{ }; 
	vector <int> LADDER_POS_Y{  }; 
	vector <int> LADDER_HEIGHT{  }; 
	vector <int> LADDER_WIDTH{  };

	vector <int> SALMON_POS_X{  };
	vector <int> SALMON_POS_Y{  };

	vector <int> CHICKEN_POS_X{  };
	vector <int> CHICKEN_POS_Y{  };

	vector <int> BOX_POS_X{  };
	vector <int> BOX_POS_Y{  };
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
	bool isAnimating{ false };
	bool isClimbing{ false };
	bool isGroundFloor{ false };
	bool collided{ false };
	bool isGrounded{ false };
	bool newDirection{ true };
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

Platform platform1;
Platform platform2;

void CameraControl();
void WalkingDurationControl(float time);
void AnimationDurationControl(float time);
void CoyoteControl();
void SetFloor();
void SetPlayerPos(int pos);

void Draw();
void DrawLevel();
void DrawGameStats();

void CoordsPlatform1();

void CreateGamePlay();
void CreatePlatform(Platform& platform);
void BuildSideWalls(Platform& platform);
void CreateWalls(vector <int>& w, vector <int>& h, vector <int>& x, vector <int>& y, float scale, const char* s, int TYPE);
void CreateFloors(vector <int>& w, vector <int>& x, vector <int>& y, float scale, const char* s, int TYPE);
void ItemsPlacement(Platform& platform);

void Jump();
void Fall();
void WalkingPlayerControls();
void IdlePlayerControls();
void AttachedPlayerControls();
void JumpControls();

void UpdatePlayer();
void UpdateFleas();
void UpdateTreats();

void UpdateDestroyed();
void LoopObject(GameObject& object);

float DotProduct(const Vector2D& v1, const Vector2D& v2);
void Normalise(Vector2D& v);

bool IsPlayerColliding(const GameObject& object);
bool IsPlayerCollidingUpperPart(const GameObject& object);
bool IsPlayerCollidingBottomPart(const GameObject& object);
bool IsPlayerCollidingAnyPlatform();
bool IsPlayerCollidingAnyPlatformBoth();
bool IsPlayerCollidingAnyPlatformUpper();
bool IsPlayerCollidingLadder();
bool IsPlayerStillCollidingLadder();
bool IsPlayerOnLadder();
bool IsPlayerCollidingGround();
//bool IsPlayerNearPlatform(GameObject& object);

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground("Data//Backgrounds//forest1.png");	
	
	CreateGamePlay();
}

// Called by PlayBuffer every fra m e (        60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	switch (gameState.state)
	{
		case STATE_PLAY:
		{
			UpdatePlayer();
			UpdateFleas();
			UpdateTreats();
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

	if (Play::KeyDown(VK_TAB))
	{
		gameState.playerState = STATE_GROUNDED;
		Play::GetGameObjectByType(TYPE_PLAYER).pos = PLAYER_START_POS * TILE_SIZE;
		Play::GetGameObjectByType(TYPE_PLAYER).velocity = PLAYER_VELOCITY_DEFAULT;
	}

	CameraControl();
	WalkingDurationControl(elapsedTime);
	AnimationDurationControl(elapsedTime);
	UpdateDestroyed();
	Draw();

	return Play::KeyDown( VK_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

void CameraControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	Point2D cameraPos = { 0, 0 };

	if (objPlayer.pos.y < CAMERA_THRESHOLD_Y * TILE_SIZE)
	{
		cameraPos.y -= CAMERA_THRESHOLD_Y * TILE_SIZE - objPlayer.pos.y;
		cameraPos.x = objPlayer.pos.x - CAMERA_OFFSET_X;
	}
  	else if (objPlayer.pos.x < CAMERA_THRESHOLD_X.x * TILE_SIZE) 
		cameraPos.x = (platform1.LIMIT_LEFT * TILE_SIZE) - TILE_SIZE;
	else if (objPlayer.pos.x > CAMERA_THRESHOLD_X.y * TILE_SIZE)
		cameraPos.x = (platform1.LIMIT_RIGHT * TILE_SIZE) - (TILE_WIDTH * TILE_SIZE) + 5;
	/*else if (objPlayer.pos.y < CAMERA_THRESHOLD_Y &&
		objPlayer.pos.x < CAMERA_THRESHOLD_X.x * TILE_SIZE)
	{
		cameraPos.x = (platform1.LIMIT_LEFT * TILE_SIZE) - TILE_SIZE;
		cameraPos.y -= CAMERA_THRESHOLD_Y - objPlayer.pos.y;
	}
	else if (objPlayer.pos.y < CAMERA_THRESHOLD_Y &&
		objPlayer.pos.x > CAMERA_THRESHOLD_X.y * TILE_SIZE)
	{
		cameraPos.x = (platform1.LIMIT_RIGHT * TILE_SIZE) - DISPLAY_WIDTH - 2;
		cameraPos.y -= CAMERA_THRESHOLD_Y - objPlayer.pos.y;
	}*/
	else 
		cameraPos = { objPlayer.pos.x - CAMERA_OFFSET_X, 0 };

	Play::SetCameraPosition(cameraPos);
}

void CreateGamePlay()
{
	switch (gameState.level)
	{
		case 1:
		{
			CoordsPlatform1();
			CreatePlatform(platform1);
			ItemsPlacement(platform1);
			break;
		}
		default:
		{
			CoordsPlatform1();
			CreatePlatform(platform1);
			ItemsPlacement(platform1);
			break;
		}
	}

	Play::MoveSpriteOrigin("cat_go_right", 0, -3);
	Play::MoveSpriteOrigin("cat_go_left", 0, -3);

	int id = Play::CreateGameObject(TYPE_PLAYER, { PLAYER_START_POS.x * TILE_SIZE, (PLAYER_START_POS.y * TILE_SIZE) - PLAYER_OFFSET_Y }, 10, "cat_sits_right");
	
	GameObject& objPlayer = Play::GetGameObject(id);
	objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	objPlayer.scale = 4.5f;

	id = Play::CreateGameObject(TYPE_FLEA, FLEA_START_POS * TILE_SIZE, 10, "fleas");
	GameObject& objFlea = Play::GetGameObject(id);
	objFlea.scale = 3.5f;
	objFlea.animSpeed = .05f;
}

void ItemsPlacement(Platform& platform)
{
	for (int i = 0; i < platform.SALMON_POS_X.size(); i++)
	{
		int id = Play::CreateGameObject(
			TYPE_TREAT,
			Point2D{ platform.SALMON_POS_X[i] * TILE_SIZE, platform.SALMON_POS_Y[i] * TILE_SIZE },
			10,
			"salmon");
		GameObject& salmon = Play::GetGameObject(id);
		salmon.scale = 2.5f;
	}

	for (int i = 0; i < platform.CHICKEN_POS_X.size(); i++)
	{
		int id = Play::CreateGameObject(
			TYPE_TREAT,
			Point2D{ platform.CHICKEN_POS_X[i] * TILE_SIZE, platform.CHICKEN_POS_Y[i] * TILE_SIZE },
			10,
			"leg");
		GameObject& leg = Play::GetGameObject(id);
		leg.scale = 2.5f;
	}

	for (int i = 0; i < platform.BOX_POS_X.size(); i++)
	{
		int id = Play::CreateGameObject(
			TYPE_BOX,
			Point2D{ platform.BOX_POS_X[i] * TILE_SIZE, platform.BOX_POS_Y[i] * TILE_SIZE - BOX_OFFSET_Y },
			10,
			"wood_box");
		GameObject& box = Play::GetGameObject(id);
		box.scale = 2.5f;
	}
}

void CoordsPlatform1()
{
	// 1st floor : 18
	// 2nd floor : 13
	// 3rd floor : 8
	// 4th floor : 3
	// 5th floor : 1
	// 6th floor : -4
	// 
	// horizontal platforms / floors
	platform1.GROUND_FLOOR_WIDTH.insert(platform1.GROUND_FLOOR_WIDTH.begin(), { platform1.LIMIT_RIGHT - platform1.LIMIT_LEFT });
	platform1.GROUND_FLOOR_POS_X.insert(platform1.GROUND_FLOOR_POS_X.begin(), { platform1.LIMIT_LEFT });
	platform1.GROUND_FLOOR_POS_Y.insert(platform1.GROUND_FLOOR_POS_Y.begin(), { platform1.GROUND_FLOOR_HEIGHT });

	platform1.BRICK_FLOOR_WIDTH.insert(platform1.BRICK_FLOOR_WIDTH.begin(), { 10, 11, 11, 3, 3 });
	platform1.BRICK_FLOOR_POS_X.insert(platform1.BRICK_FLOOR_POS_X.begin(), { -13, 10, 10, 0, -7 });
	platform1.BRICK_FLOOR_POS_Y.insert(platform1.BRICK_FLOOR_POS_Y.begin(), { 
		platform1.SECOND_FLOOR_HEIGHT - 1, platform1.SECOND_FLOOR_HEIGHT - 1, platform1.THIRD_FLOOR_HEIGHT - 1, platform1.THIRD_FLOOR_HEIGHT - 1, platform1.THIRD_FLOOR_HEIGHT - 1, });

	platform1.SAND_FLOOR_WIDTH.insert(platform1.SAND_FLOOR_WIDTH.begin(), {34 });
	platform1.SAND_FLOOR_POS_X.insert(platform1.SAND_FLOOR_POS_X.begin(), { -13 });
	platform1.SAND_FLOOR_POS_Y.insert(platform1.SAND_FLOOR_POS_Y.begin(), { platform1.FOURTH_FLOOR_HEIGHT - 1 });

	// grass with dirt
	platform1.GRASS_WIDTH.insert(platform1.GRASS_WIDTH.begin(), {  });
	platform1.GRASS_FLOOR_POS_X.insert(platform1.GRASS_FLOOR_POS_X.begin(), { 5, 2 });
	platform1.GRASS_FLOOR_POS_Y.insert(platform1.GRASS_FLOOR_POS_Y.begin(), { platform1.FOURTH_FLOOR_HEIGHT, platform1.SECOND_FLOOR_HEIGHT });

	// vertical platforms / walls
	platform1.BRICK_WALL_HEIGHT.insert(platform1.BRICK_WALL_HEIGHT.begin(), { 2, 2 });
	platform1.BRICK_WALL_WIDTH.insert(platform1.BRICK_WALL_WIDTH.begin(), { 2, 2 });
	platform1.BRICK_WALL_POS_X.insert(platform1.BRICK_WALL_POS_X.begin(), { -15, 21 });
	platform1.BRICK_WALL_POS_Y.insert(platform1.BRICK_WALL_POS_Y.begin(), { platform1.SECOND_FLOOR_HEIGHT + 3, platform1.SECOND_FLOOR_HEIGHT + 3 });

	platform1.SAND_WALL_HEIGHT.insert(platform1.SAND_WALL_HEIGHT.begin(), { 3, 3, 2, 2, 9, 13 });
	platform1.SAND_WALL_WIDTH.insert(platform1.SAND_WALL_WIDTH.begin(), { 2, 2, 2, 2, 2, 2 });
	platform1.SAND_WALL_POS_X.insert(platform1.SAND_WALL_POS_X.begin(), { -15, 21, -15, 21, -15, 21 });
	platform1.SAND_WALL_POS_Y.insert(platform1.SAND_WALL_POS_Y.begin(), { platform1.SECOND_FLOOR_HEIGHT, platform1.SECOND_FLOOR_HEIGHT, platform1.THIRD_FLOOR_HEIGHT, platform1.THIRD_FLOOR_HEIGHT, SIXTH_FLOOR_HEIGHT, SIXTH_FLOOR_HEIGHT });

	// dirt blocks
	platform1.DIRT_HEIGHT.insert(platform1.DIRT_HEIGHT.begin(), {  });
	platform1.DIRT_WIDTH.insert(platform1.DIRT_WIDTH.begin(), { 2, 5 });
	platform1.DIRT_POS_X.insert(platform1.DIRT_POS_X.begin(), { -1, 10 });
	platform1.DIRT_POS_Y.insert(platform1.DIRT_POS_Y.begin(), { platform1.GROUND_FLOOR_HEIGHT - 10, platform1.GROUND_FLOOR_HEIGHT - 15 });

	// ladders
	platform1.LADDER_HEIGHT.insert(platform1.LADDER_HEIGHT.begin(), { 15 });
	platform1.LADDER_WIDTH.insert(platform1.LADDER_WIDTH.begin(), { 1 });
	platform1.LADDER_POS_X.insert(platform1.LADDER_POS_X.begin(), { 3 });
	platform1.LADDER_POS_Y.insert(platform1.LADDER_POS_Y.begin(), { platform1.FOURTH_FLOOR_HEIGHT });

	// cat treats
	platform1.SALMON_POS_X.insert(platform1.SALMON_POS_X.begin(), { 9 });
	platform1.SALMON_POS_Y.insert(platform1.SALMON_POS_Y.begin(), { platform1.SECOND_FLOOR_HEIGHT - 1 });

	platform1.CHICKEN_POS_X.insert(platform1.CHICKEN_POS_X.begin(), { 5 });
	platform1.CHICKEN_POS_Y.insert(platform1.CHICKEN_POS_Y.begin(), { platform1.THIRD_FLOOR_HEIGHT - 1 });

	// boxes
	platform1.BOX_POS_X.insert(platform1.BOX_POS_X.begin(), { -9, -8, -7, -8, -7 });
	platform1.BOX_POS_Y.insert(platform1.BOX_POS_Y.begin(), { platform1.SECOND_FLOOR_HEIGHT - 1, platform1.SECOND_FLOOR_HEIGHT - 1, platform1.SECOND_FLOOR_HEIGHT - 1, platform1.SECOND_FLOOR_HEIGHT - 2, platform1.SECOND_FLOOR_HEIGHT - 2 });
}

void CoordsPlatform2()
{
	// 1st floor : 18
	// 2nd floor : 13
	// 3rd floor : 8
	// 4th floor : 3
	// 5th floor : 1
	// 6th floor : -4
	// 
	// horizontal platforms / floors
	platform1.GROUND_FLOOR_WIDTH.insert(platform1.GROUND_FLOOR_WIDTH.begin(), { platform1.LIMIT_RIGHT - platform1.LIMIT_LEFT });
	platform1.GROUND_FLOOR_POS_X.insert(platform1.GROUND_FLOOR_POS_X.begin(), { platform1.LIMIT_LEFT });
	platform1.GROUND_FLOOR_POS_Y.insert(platform1.GROUND_FLOOR_POS_Y.begin(), { platform1.GROUND_FLOOR_HEIGHT });

	platform1.BRICK_FLOOR_WIDTH.insert(platform1.BRICK_FLOOR_WIDTH.begin(), {  });
	platform1.BRICK_FLOOR_POS_X.insert(platform1.BRICK_FLOOR_POS_X.begin(), {  });
	platform1.BRICK_FLOOR_POS_Y.insert(platform1.BRICK_FLOOR_POS_Y.begin(), {  });

	platform1.SAND_FLOOR_WIDTH.insert(platform1.SAND_FLOOR_WIDTH.begin(), {  });
	platform1.SAND_FLOOR_POS_X.insert(platform1.SAND_FLOOR_POS_X.begin(), {  });
	platform1.SAND_FLOOR_POS_Y.insert(platform1.SAND_FLOOR_POS_Y.begin(), { });
	// grass with dirt
	platform1.GRASS_WIDTH.insert(platform1.GRASS_WIDTH.begin(), {  });
	platform1.GRASS_FLOOR_POS_X.insert(platform1.GRASS_FLOOR_POS_X.begin(), {  });
	platform1.GRASS_FLOOR_POS_Y.insert(platform1.GRASS_FLOOR_POS_Y.begin(), {  });

	// vertical platforms / walls
	platform1.BRICK_WALL_HEIGHT.insert(platform1.BRICK_WALL_HEIGHT.begin(), {  });
	platform1.BRICK_WALL_WIDTH.insert(platform1.BRICK_WALL_WIDTH.begin(), {  });
	platform1.BRICK_WALL_POS_X.insert(platform1.BRICK_WALL_POS_X.begin(), {  });
	platform1.BRICK_WALL_POS_Y.insert(platform1.BRICK_WALL_POS_Y.begin(), {  });

	platform1.SAND_WALL_HEIGHT.insert(platform1.SAND_WALL_HEIGHT.begin(), {  });
	platform1.SAND_WALL_WIDTH.insert(platform1.SAND_WALL_WIDTH.begin(), {  });
	platform1.SAND_WALL_POS_X.insert(platform1.SAND_WALL_POS_X.begin(), {  });
	platform1.SAND_WALL_POS_Y.insert(platform1.SAND_WALL_POS_Y.begin(), {  });

	// dirt blocks
	platform1.DIRT_HEIGHT.insert(platform1.DIRT_HEIGHT.begin(), {  });
	platform1.DIRT_WIDTH.insert(platform1.DIRT_WIDTH.begin(), {  });
	platform1.DIRT_POS_X.insert(platform1.DIRT_POS_X.begin(), {  });
	platform1.DIRT_POS_Y.insert(platform1.DIRT_POS_Y.begin(), {  });

	// ladders
	platform1.LADDER_HEIGHT.insert(platform1.LADDER_HEIGHT.begin(), {  });
	platform1.LADDER_WIDTH.insert(platform1.LADDER_WIDTH.begin(), {  });
	platform1.LADDER_POS_X.insert(platform1.LADDER_POS_X.begin(), {  });
	platform1.LADDER_POS_Y.insert(platform1.LADDER_POS_Y.begin(), {  });

	// cat treats
	platform1.SALMON_POS_X.insert(platform1.SALMON_POS_X.begin(), {  });
	platform1.SALMON_POS_Y.insert(platform1.SALMON_POS_Y.begin(), {  });

	platform1.CHICKEN_POS_X.insert(platform1.CHICKEN_POS_X.begin(), {  });
	platform1.CHICKEN_POS_Y.insert(platform1.CHICKEN_POS_Y.begin(), {  });
}

void CoordsPlatform3()
{
	// 1st floor : 18
	// 2nd floor : 13
	// 3rd floor : 8
	// 4th floor : 3
	// 5th floor : 1
	// 6th floor : -4
	// 
	// horizontal platforms / floors
	platform1.GROUND_FLOOR_WIDTH.insert(platform1.GROUND_FLOOR_WIDTH.begin(), { platform1.LIMIT_RIGHT - platform1.LIMIT_LEFT });
	platform1.GROUND_FLOOR_POS_X.insert(platform1.GROUND_FLOOR_POS_X.begin(), { platform1.LIMIT_LEFT });
	platform1.GROUND_FLOOR_POS_Y.insert(platform1.GROUND_FLOOR_POS_Y.begin(), { platform1.GROUND_FLOOR_HEIGHT });

	platform1.BRICK_FLOOR_WIDTH.insert(platform1.BRICK_FLOOR_WIDTH.begin(), {  });
	platform1.BRICK_FLOOR_POS_X.insert(platform1.BRICK_FLOOR_POS_X.begin(), {  });
	platform1.BRICK_FLOOR_POS_Y.insert(platform1.BRICK_FLOOR_POS_Y.begin(), {  });

	platform1.SAND_FLOOR_WIDTH.insert(platform1.SAND_FLOOR_WIDTH.begin(), {  });
	platform1.SAND_FLOOR_POS_X.insert(platform1.SAND_FLOOR_POS_X.begin(), {  });
	platform1.SAND_FLOOR_POS_Y.insert(platform1.SAND_FLOOR_POS_Y.begin(), { });
	// grass with dirt
	platform1.GRASS_WIDTH.insert(platform1.GRASS_WIDTH.begin(), {  });
	platform1.GRASS_FLOOR_POS_X.insert(platform1.GRASS_FLOOR_POS_X.begin(), {  });
	platform1.GRASS_FLOOR_POS_Y.insert(platform1.GRASS_FLOOR_POS_Y.begin(), {  });

	// vertical platforms / walls
	platform1.BRICK_WALL_HEIGHT.insert(platform1.BRICK_WALL_HEIGHT.begin(), {  });
	platform1.BRICK_WALL_WIDTH.insert(platform1.BRICK_WALL_WIDTH.begin(), {  });
	platform1.BRICK_WALL_POS_X.insert(platform1.BRICK_WALL_POS_X.begin(), {  });
	platform1.BRICK_WALL_POS_Y.insert(platform1.BRICK_WALL_POS_Y.begin(), {  });

	platform1.SAND_WALL_HEIGHT.insert(platform1.SAND_WALL_HEIGHT.begin(), {  });
	platform1.SAND_WALL_WIDTH.insert(platform1.SAND_WALL_WIDTH.begin(), {  });
	platform1.SAND_WALL_POS_X.insert(platform1.SAND_WALL_POS_X.begin(), {  });
	platform1.SAND_WALL_POS_Y.insert(platform1.SAND_WALL_POS_Y.begin(), {  });

	// dirt blocks
	platform1.DIRT_HEIGHT.insert(platform1.DIRT_HEIGHT.begin(), {  });
	platform1.DIRT_WIDTH.insert(platform1.DIRT_WIDTH.begin(), {  });
	platform1.DIRT_POS_X.insert(platform1.DIRT_POS_X.begin(), {  });
	platform1.DIRT_POS_Y.insert(platform1.DIRT_POS_Y.begin(), {  });

	// ladders
	platform1.LADDER_HEIGHT.insert(platform1.LADDER_HEIGHT.begin(), {  });
	platform1.LADDER_WIDTH.insert(platform1.LADDER_WIDTH.begin(), {  });
	platform1.LADDER_POS_X.insert(platform1.LADDER_POS_X.begin(), {  });
	platform1.LADDER_POS_Y.insert(platform1.LADDER_POS_Y.begin(), {  });

	// cat treats
	platform1.SALMON_POS_X.insert(platform1.SALMON_POS_X.begin(), {  });
	platform1.SALMON_POS_Y.insert(platform1.SALMON_POS_Y.begin(), {  });

	platform1.CHICKEN_POS_X.insert(platform1.CHICKEN_POS_X.begin(), {  });
	platform1.CHICKEN_POS_Y.insert(platform1.CHICKEN_POS_Y.begin(), {  });
}

void CreatePlatform(Platform& platform)
{
	BuildSideWalls(platform);

	CreateFloors(platform.GRASS_WIDTH, platform.GRASS_FLOOR_POS_X, platform.GRASS_FLOOR_POS_Y, 2.5,  "new_grass", TYPE_PLATFORM);
	CreateFloors(platform.BRICK_FLOOR_WIDTH, platform.BRICK_FLOOR_POS_X, platform.BRICK_FLOOR_POS_Y, 2.5, "blue_brick", TYPE_PLATFORM);
	CreateFloors(platform.SAND_FLOOR_WIDTH, platform.SAND_FLOOR_POS_X, platform.SAND_FLOOR_POS_Y, 2.5, "sand_brick", TYPE_PLATFORM);

	CreateWalls(platform.SAND_WALL_WIDTH, platform.SAND_WALL_HEIGHT, platform.SAND_WALL_POS_X, platform.SAND_WALL_POS_Y, 2.5, "sand_brick", TYPE_PLATFORM);
	CreateWalls(platform.BRICK_WALL_WIDTH, platform.BRICK_WALL_HEIGHT, platform.BRICK_WALL_POS_X, platform.BRICK_WALL_POS_Y, 2.5,  "blue_brick", TYPE_PLATFORM);
	CreateWalls(platform.DIRT_WIDTH, platform.DIRT_HEIGHT, platform.DIRT_POS_X, platform.DIRT_POS_Y, 2.5, "dirrt", TYPE_PLATFORM);

	CreateWalls(platform.LADDER_WIDTH, platform.LADDER_HEIGHT, platform.LADDER_POS_X, platform.LADDER_POS_Y, 2.5, "climb_grass", TYPE_LADDER);
	CreateFloors(platform.GROUND_FLOOR_WIDTH, platform.GROUND_FLOOR_POS_X, platform.GROUND_FLOOR_POS_Y, 2.5, "the_ground", TYPE_GROUND);
}

void CreateWalls(vector <int>& w, vector <int>& h, vector <int>& x, vector <int>& y, float scale, const char* s, int TYPE)
{
	int j = 0;

	for (int height : h)
	{
		for (int width : w)
		{
			for (int i = 0; i < h[j]; i++)
			{
				for (int k = 0; k < w[j]; k++)
				{
					int id = Play::CreateGameObject( TYPE,
						Point2D{ x[j] * TILE_SIZE + (TILE_SIZE * k) , y[j] * TILE_SIZE + (TILE_SIZE * i) - WALL_OFFSET }, 10, s );
					Play::GetGameObject(id).scale = 2.5f;
				}
			}
		}
		j++;
	}
}

void CreateFloors(vector <int>& w, vector <int>& x, vector <int>& y, float scale, const char* s, int TYPE)
{
	int j = 0;

	for (int width : w)
	{
		for (int i = 0; i < w[j]; i++)
		{
			int id = Play::CreateGameObject( TYPE,
				Point2D{ ( x[j] * TILE_SIZE) + (TILE_SIZE * i), y[j] * TILE_SIZE + FLOOR_OFFSET }, 10, s );
			Play::GetGameObject(id).scale = scale;
		}
		j++;
	}
}

void BuildSideWalls(Platform& platform)
{
	for (int i = 0; i < platform.WALL_HEIGHT; i++)
	{
		int id = Play::CreateGameObject( TYPE_WALL,
			Point2D{ (platform.LEFT_WALL_POS_X * TILE_SIZE) + platform.WALL_OFFSET.x, (TILE_SIZE * i) },
			10, "left_side_wall");
		Play::GetGameObject(id).scale = 2.5f;

		int id2 = Play::CreateGameObject( TYPE_WALL,
			Point2D{ (platform.RIGHT_WALL_POS_X * TILE_SIZE) - platform.WALL_OFFSET.x * 3 , (TILE_SIZE * i) - platform.WALL_OFFSET.y },
			10, "right_side_wall");
		Play::GetGameObject(id2).scale = 2.5f;
	}
}

void UpdatePlayer()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	SetFloor();

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
	vector <int> vFleas = Play::CollectGameObjectIDsByType(TYPE_FLEA);

	for (int fleaId : vFleas)
	{
		GameObject& flea = Play::GetGameObject(fleaId);
		LoopObject(flea);

		Play::UpdateGameObject(flea);
	}
}

void UpdateTreats()
{
	vector <int> vTreats = Play::CollectGameObjectIDsByType(TYPE_TREAT);

	for (int treatId : vTreats)
	{
		GameObject& treat = Play::GetGameObject(treatId);

		if (IsPlayerColliding(treat))
		{
			gameState.score += 10;
			Play::DestroyGameObject(treatId);
		}
		Play::UpdateGameObject(treat);
	}
}

void WalkingDurationControl(float time)
{
	if (gameState.playerState == STATE_WALK)
	{
		if (walkTimer > 0.7f && !Play::KeyDown(VK_LEFT) && !Play::KeyDown(VK_RIGHT))
		{
			gameState.playerState = STATE_GROUNDED;
			walkTimer = 0.f;
		}
		else
			walkTimer += time;
	}
}

void AnimationDurationControl(float time)
{
	if (flags.isAnimating)
	{
		if (animationTimer > 0.7f)
		{
			flags.isAnimating = false;
			animationTimer = 0.f;
		}
		else
			animationTimer += time;
	}
}

void Jump()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	coyoteTimer = 0.f;

	if (IsPlayerCollidingLadder())
		gameState.playerState = STATE_ATTACHED;

	(jumpTimer < 1.0f) ? jumpTimer += DELTA_TIME : jumpTimer = 0.f;

	(flags.right) ? 
		Play::SetSprite(objPlayer, "cat_jump_right", 0.05f) : 
		Play::SetSprite(objPlayer, "cat_jump_left", 0.05f);	
	
	(flags.right) ?
		objPlayer.pos.x += objPlayer.velocity.x * DELTA_TIME :
		objPlayer.pos.x -= objPlayer.velocity.x * DELTA_TIME;

	(objPlayer.velocity.y < 0 && !Play::KeyDown(VK_SPACE) && jumpTimer < 0.5f) ? 
		gravityMultiplyer = LOW_JUMP_MULTIPLIER : 
		gravityMultiplyer = 1.f;
		
	(objPlayer.velocity.y < 0) ? 
		objPlayer.velocity += GRAVITY * DELTA_TIME * gravityMultiplyer : 
		objPlayer.velocity += GRAVITY * FALL_MULTIPLIER * DELTA_TIME * gravityMultiplyer;
		

	vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		
		GameObject& platform = Play::GetGameObject(platformId);
			
		if (IsPlayerCollidingBottomPart(platform) && jumpTimer > 0.3f )
		{	
			flags.isGrounded = true;
			SetPlayerPos(groundedPosY);
			gameState.playerState = STATE_GROUNDED;
			objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
			jumpTimer = 0.f;
 		}
		else if (IsPlayerCollidingUpperPart(platform) && objPlayer.velocity.y < 0 )
		{
			objPlayer.velocity.y = 0.f;
			gameState.playerState = STATE_FALL;
		}
	}

	vector <int> vGrounds = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int groundId : vGrounds)
	{
		GameObject& ground = Play::GetGameObject(groundId);

		if (IsPlayerCollidingBottomPart(ground) && jumpTimer > 0.3f)
		{
			flags.isGrounded = true;
			SetPlayerPos(groundedPosY - TILE_SIZE);
			gameState.playerState = STATE_GROUNDED;
			objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
			jumpTimer = 0.f;
		}
	}
 
	if (IsPlayerStillCollidingLadder())
	{
		gameState.playerState = STATE_GROUNDED;
		objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	} 
}

void SetPlayerPos(int pos)
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	(flags.isGroundFloor) ?
		objPlayer.pos.y = PLAYER_START_POS.y * TILE_SIZE - PLAYER_OFFSET_Y :
		objPlayer.pos.y = pos - PLAYER_OFFSET_Y;
}

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
	else if (objPlayer.oldPos.y <= platform1.GROUND_FLOOR_HEIGHT  * TILE_SIZE - TILE_SIZE)
		gameState.floor = 1;
}

void CoyoteControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (!IsPlayerCollidingAnyPlatform() && !IsPlayerStillCollidingLadder() && gameState.floor != 1)
		coyoteTimer += DELTA_TIME;

	if (!IsPlayerCollidingAnyPlatform() && !IsPlayerStillCollidingLadder() && objPlayer.pos.y < PLAYER_START_POS.y * TILE_WIDTH && coyoteTimer > 0.1f)
	{
		coyoteTimer = 0.f;
		flags.isGrounded = false;
		gameState.playerState = STATE_FALL;
	}
}

void WalkingPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	
	(flags.right) ? 
		Play::SetSprite(objPlayer, "cat_go_right", 0.2f) :
		Play::SetSprite(objPlayer, "cat_go_left", 0.2f);

 	if (Play::KeyDown(VK_RIGHT))
	{   
		if (IsPlayerCollidingAnyPlatformUpper())
			objPlayer.pos.x = objPlayer.oldPos.x;
		else
			objPlayer.pos.x += 10;
		flags.right = true;	
	}
	else if (Play::KeyDown(VK_LEFT))
	{
		if (IsPlayerCollidingAnyPlatformUpper())
			objPlayer.pos.x = objPlayer.oldPos.x;
		else
			objPlayer.pos.x -= 10;
		flags.right = false;
	}

	CoyoteControl();
	JumpControls();
}

void IdlePlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	objPlayer.rotation = Play::DegToRad(0);

	// if player is too low when grounded, then fix player pos y
	if (objPlayer.pos.y > groundedPosY)
		SetPlayerPos(groundedPosY);

	(flags.right) ?
		Play::SetSprite(objPlayer, "cat_sits_right", 0.1f) :
		Play::SetSprite(objPlayer, "cat_sits_left", 0.1f);

	if (Play::KeyDown(VK_RIGHT) || Play::KeyDown(VK_LEFT))
	{
			gameState.playerState = STATE_WALK;

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
		(flags.right) ?
			objPlayer.pos = { ladderPos.x - PLAYER_AABB_UPPER.x, ladderPos.y + TILE_SIZE * 2} :
			objPlayer.pos = { ladderPos.x + PLAYER_AABB_UPPER.x, ladderPos.y + TILE_SIZE * 2};

		flags.isGrounded = false;
		gameState.playerState = STATE_ATTACHED;
	}
	
	flags.isGrounded = true;

	CoyoteControl();
	JumpControls();

	Play::UpdateGameObject(objPlayer);
}

// jump off the ground / ladder
void JumpControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (Play::KeyDown(VK_LEFT) && Play::KeyPressed(VK_SPACE))
	{
		flags.isGrounded = false;
		objPlayer.velocity = PLAYER_VELOCITY_JUMP_LEFT;
		gameState.playerState = STATE_JUMP;
	}
	else if (Play::KeyDown(VK_RIGHT) && Play::KeyPressed(VK_SPACE))
	{
		flags.isGrounded = false;
		objPlayer.velocity = PLAYER_VELOCITY_JUMP_RIGHT;
		gameState.playerState = STATE_JUMP;
	}
	else if (Play::KeyPressed(VK_SPACE))
	{
		flags.collided = false;
		flags.isGrounded = false;
		objPlayer.velocity = PLAYER_VELOCITY_JUMP;
		gameState.playerState = STATE_JUMP;
	}
}

void AttachedPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	objPlayer.animSpeed = 0.f;

	// if player is not standing on ladder or colliding with ladder, then fall
	if (!IsPlayerCollidingLadder() && !IsPlayerOnLadder())
		gameState.playerState = STATE_FALL;

	if (   gameState.floor == 2 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_SECOND + LADDER_OFFSET_Y
		|| gameState.floor == 3 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_THIRD + LADDER_OFFSET_Y
		|| gameState.floor == 4 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_FOURTH + LADDER_OFFSET_Y
		|| gameState.floor == 5 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_FIFTH + LADDER_OFFSET_Y
		|| gameState.floor == 6 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_SIXTH + LADDER_OFFSET_Y  )
	{
		gameState.playerState = STATE_GROUNDED;

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

		objPlayer.pos.x = ladderPos.x;
	} 

	if (flags.right)
	{
		objPlayer.rotation = Play::DegToRad(-90);
		objPlayer.pos.x = ladderPos.x - TILE_SIZE - LADDER_OFFSET_X;
		Play::SetSprite(objPlayer, "cat_walk_right", 0.f);
	} 
	else 
	{
		objPlayer.rotation = Play::DegToRad(90);
		objPlayer.pos.x = ladderPos.x + TILE_SIZE + LADDER_OFFSET_X;
		Play::SetSprite(objPlayer, "cat_walk_left", 0.f);
	}

	if (Play::KeyDown(VK_UP))
	{	
		objPlayer.pos.y -= 5;
		objPlayer.animSpeed = 0.1f;
	}
	else if (Play::KeyDown(VK_DOWN))
	{
		if (IsPlayerCollidingAnyPlatform() && objPlayer.pos.y > SECOND_FLOOR_HEIGHT * TILE_SIZE)
		{
			(flags.right) ?  objPlayer.pos.x -= 15 : objPlayer.pos.x += 15;

			(groundedPosY > PLAYER_START_POS.y * TILE_SIZE - PLAYER_OFFSET_Y) ?
				SetPlayerPos(PLAYER_START_POS.y * TILE_SIZE) :
				SetPlayerPos(groundedPosY);

			gameState.playerState = STATE_GROUNDED;
		}
		else
			objPlayer.pos.y += 5;

		objPlayer.animSpeed = 0.1f;
	}
	else if (Play::KeyDown(VK_LEFT))
		flags.right = true;
	else if (Play::KeyDown(VK_RIGHT))
		flags.right = false;

	else if (Play::KeyDown(VK_SPACE))
	{
		objPlayer.pos.x += sin(objPlayer.rotation) * 25;	
		objPlayer.pos.y -= cos(objPlayer.rotation) * 25;
		objPlayer.rotation = 0;
	}
}

void Fall()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	objPlayer.acceleration = GRAVITY_ACCELERATION;

	(!flags.right) ?
		Play::SetSprite(objPlayer, "cat_look_left", 0.1f) :
		Play::SetSprite(objPlayer, "cat_look_right", 0.1f);

	(!flags.right) ?
		objPlayer.velocity += PLAYER_VELOCITY_FALL_RIGHT * DELTA_TIME :
		objPlayer.velocity += PLAYER_VELOCITY_FALL_LEFT * DELTA_TIME;

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

bool IsPlayerCollidingBottomPart(const GameObject& object  )
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

	if (objPlayer.pos.y + 10 < object.pos.y + AABB.y &&
		objPlayer.pos.y + PLAYER_AABB_BOTTOM.y > object.pos.y - AABB.y)
	{
		if (objPlayer.pos.x > object.pos.x - AABB.x - COLLISION_BOOST &&
			objPlayer.pos.x < object.pos.x + AABB.x + COLLISION_BOOST)
		{    
			groundedPosY = object.pos.y - PLAYER_AABB_BOTTOM.y - PLAYER_OFFSET_Y;

			// set player velocity to 0 so he doesnt fall through the platform
			objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;

			flags.collided = true;
			return true;
		}
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

	DrawLevel();
	DrawGameStats();

	Play::PresentDrawingBuffer();
}

void DrawGameStats()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	GameObject& objFlea = Play::GetGameObjectByType(TYPE_FLEA);

	Play::SetDrawingSpace(Play::SCREEN);

	Play::DrawFontText("64px", "Cat state: " + std::to_string(gameState.playerState), { DISPLAY_WIDTH - 150, 50 }, Play::CENTRE);
	Play::DrawFontText("64px", "floor: " + std::to_string(gameState.floor), { DISPLAY_WIDTH - 150, 100 }, Play::CENTRE);
	Play::DrawFontText("64px", "cat px: " + std::to_string(objPlayer.pos.x), { DISPLAY_WIDTH - 150, 150 }, Play::CENTRE);
	Play::DrawFontText("64px", "cat py: " + std::to_string(objPlayer.pos.y), { DISPLAY_WIDTH - 150, 200 }, Play::CENTRE);
	Play::DrawFontText("64px", "G pos Y: " + std::to_string(groundedPosY), { DISPLAY_WIDTH - 150, 250 }, Play::CENTRE);

	Play::DrawFontText("105px", "Score: " + std::to_string(gameState.score), { DISPLAY_WIDTH / 2 , 50 }, Play::CENTRE);

	Play::DrawFontText("64px", "collided: " + std::to_string(flags.collided), { DISPLAY_WIDTH - 150, 300 }, Play::CENTRE);
	Play::DrawFontText("64px", "grounded " + std::to_string(flags.isGrounded), { DISPLAY_WIDTH - 150, 350 }, Play::CENTRE);
	Play::DrawFontText("64px", "CAT VY: " + std::to_string(objPlayer.velocity.y) , { DISPLAY_WIDTH - 150, 400 }, Play::CENTRE);
	//Play::DrawFontText("64px", "G floor: " + std::to_string(flags.isGroundFloor), { 150, 450 }, Play::CENTRE);

	Play::SetDrawingSpace(Play::WORLD);
}

void DrawLevel()
{
	vector <int> vWalls = Play::CollectGameObjectIDsByType(TYPE_WALL);

	for (int wallId : vWalls)
	{
		GameObject& wall = Play::GetGameObject(wallId);
		Play::DrawObjectRotated(wall);
		Play::DrawRect(Point2D{ wall.pos.x - WALL_AABB.x, wall.pos.y - WALL_AABB.y }, Point2D{ wall.pos.x + WALL_AABB.x, wall.pos.y + WALL_AABB.y }, Play::cWhite);
	}

	vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		Play::DrawObjectRotated(platform);
		Play::DrawRect(Point2D{ platform.pos.x - PLATFORM_AABB.x, platform.pos.y - PLATFORM_AABB.y }, Point2D{ platform.pos.x + PLATFORM_AABB.x, platform.pos.y + PLATFORM_AABB.y }, Play::cWhite);
	}

	vector <int> vGrounds = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int groundId : vGrounds)
	{
		GameObject& ground = Play::GetGameObject(groundId);
		Play::DrawObjectRotated(ground);
		Play::DrawRect(Point2D{ ground.pos.x - GROUND_PLATFORM_AABB.x, ground.pos.y - GROUND_PLATFORM_AABB.y }, Point2D{ ground.pos.x + GROUND_PLATFORM_AABB.x, ground.pos.y + GROUND_PLATFORM_AABB.y }, Play::cWhite);
	}

	vector <int> vTreats = Play::CollectGameObjectIDsByType(TYPE_TREAT);
	for (int treatId : vTreats)
	{
		GameObject& treat = Play::GetGameObject(treatId);
		Play::DrawObjectRotated(treat);
	}

	vector <int> vLadders = Play::CollectGameObjectIDsByType(TYPE_LADDER);
	for (int ladderId : vLadders)
	{
		GameObject& ladder = Play::GetGameObject(ladderId);
		Play::DrawObjectRotated(ladder);
		Play::DrawRect(Point2D{ ladder.pos.x - PLATFORM_AABB.x, ladder.pos.y - PLATFORM_AABB.y }, Point2D{ ladder.pos.x + PLATFORM_AABB.x, ladder.pos.y + PLATFORM_AABB.y }, Play::cWhite);
	}

	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	vector <int> vFleas = Play::CollectGameObjectIDsByType(TYPE_FLEA);

	for (int fleaId : vFleas)
	{
		GameObject& flea = Play::GetGameObject(fleaId);
		Play::DrawObjectRotated(flea);
	}

	vector <int> vBoxes = Play::CollectGameObjectIDsByType(TYPE_BOX);

	for (int boxId : vBoxes)
	{
		GameObject& objBox = Play::GetGameObject(boxId);
		Play::DrawObjectRotated(objBox);
	}

	Play::DrawObjectRotated(objPlayer);
	Play::DrawRect(Point2D{ objPlayer.pos.x - PLAYER_AABB_BOTTOM.x, objPlayer.pos.y - PLAYER_AABB_BOTTOM.y }, Point2D{ objPlayer.pos.x + PLAYER_AABB_BOTTOM.x, objPlayer.pos.y + PLAYER_AABB_BOTTOM.y }, Play::cRed);
	//Play::DrawRect(Point2D{ objPlayer.pos.x, objPlayer.pos.y - PLAYER_AABB_UPPER.y }, Point2D{ objPlayer.pos.x + PLAYER_AABB_UPPER.x, objPlayer.pos.y + PLAYER_AABB_UPPER.y }, Play::cWhite);
	
}

void UpdateDestroyed()
{
	vector <int> vDestroyed = Play::CollectGameObjectIDsByType(TYPE_DESTROYED);

	for (int destroyedId : vDestroyed)
	{
		Play::DestroyGameObject(destroyedId);
	}
}







