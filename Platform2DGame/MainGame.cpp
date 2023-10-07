#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"  

using namespace std;

constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;

const float DELTA_TIME{ 0.016f }; // seconds

const int COLLISION_BOOST{ 25 }; // pixels

// offsets
const int CAMERA_OFFSET_X{ 600 }; // pixels
const int PLAYER_OFFSET_Y{ 8 }; // pixels
const int FLEA_OFFSET_Y{ 15 }; // pixels
const int BOX_OFFSET_Y{ 20 }; // pixels
const int WALL_OFFSET{ 20 }; // pixels
const int FLOOR_OFFSET{ 18 }; // pixels
const int LADDER_OFFSET_Y{ 30 }; // pixels
const int LADDER_OFFSET_X{ 10 }; // pixels
const int TILE_SIZE{ 38 }; // pixels
const int POO_OFFSET_Y{ 35 }; // pixels
const int WATER_OFFSET_Y{ 10 }; // pixels

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
const Point2D RIGHT_FLY_START_POS{ -35 , THIRD_FLOOR_HEIGHT };
const Point2D LEFT_FLY_START_POS{ 35 , THIRD_FLOOR_HEIGHT };

const int FLY_SPEED{ 2 };

const Vector2D PLAYER_VELOCITY_DEFAULT{ 0, 0 };
const Vector2D PLAYER_VELOCITY_WALK{ 10, 0 };

const Vector2D PLAYER_VELOCITY_JUMP{ 0, -9 };
const Vector2D PLAYER_VELOCITY_JUMP_LEFT{ -2, -9 }; 
const Vector2D PLAYER_VELOCITY_JUMP_RIGHT{ 2, -9 };

const Vector2D ENEMY_VELOCITY_DEFAULT{ 0, 0 };
const Vector2D ENEMY_VELOCITY_JUMP{ 2, -4 };
const Vector2D ENEMY_VELOCITY_WALK{ 3, 0 };

const float FALL_MULTIPLIER{ 2.5f };
const float LOW_JUMP_MULTIPLIER{ 3.5f };

const Vector2D PLAYER_VELOCITY_FALL_RIGHT{ 1, 2 };
const Vector2D PLAYER_VELOCITY_FALL_LEFT{ -1, 2 };

// all AABBs in pixels
const Vector2D PLAYER_AABB{ 30, 45 };
const Vector2D PLAYER_AABB_BOTTOM{ 20, 48 };
const Vector2D PLAYER_AABB_UPPER{ 35, 25 };
const Vector2D ENEMY_AABB{ 20, 15 };
const Vector2D FLY_AABB{ 15, 15 };
const Vector2D TREATS_AABB{ 20, 20 };

const Vector2D GROUND_PLATFORM_AABB{ 20, 60 };
const Vector2D PLATFORM_AABB{ 20, 20 };
const Vector2D WALL_AABB{ 60, 20 };
const Vector2D WATER_AABB{ 20, 20 };

const Vector2D GRAVITY_ACCELERATION{ 0, 0.5f };
const Vector2D GRAVITY{ 0, 10.f };
const Vector2D ACCELERATION{ 0, 0.3f };
const Vector2D FRICTION{ 0.5f, 0.5f };

Point2D cameraPos{ 0, 0 };
Point2D platformPos{ 0, 0 };
Point2D ladderPos{ 0, 0 };

int groundedPosY = 0;

float furballTimer = 0.f;
float hissTimer = 0.f;
float fleaTimer = 0.f;
float walkTimer = 0.f;
float jumpTimer = 0.f;
float poopTimer = 0.f;
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
	STATE_FURBALL,
	STATE_HISS,
	STATE_HIDE,
	STATE_DEAD,
};

enum GameObjectType
{
	TYPE_PLAYER = 0,
	TYPE_ENEMY,
	TYPE_FLEA,
	TYPE_RIGHT_FLY,
	TYPE_LEFT_FLY,
	TYPE_PLATFORM,
	TYPE_GROUND,
	TYPE_LADDER,
	TYPE_WATER,
	TYPE_TREAT,
	TYPE_FLOWER,
	TYPE_POO,
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

	Point2D playerStartPos{  };
	Point2D fleaStartPos{  };

	bool isWater{ false };

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

	vector <int> NUT_WALL_POS_X{  };
	vector <int> NUT_WALL_POS_Y{  };
	vector <int> NUT_WALL_HEIGHT{  };
	vector <int> NUT_WALL_WIDTH{  };

	vector <int> C_FLOOR_WIDTH{  };
	vector <int> C_FLOOR_POS_X{  };
	vector <int> C_FLOOR_POS_Y{  };

	vector <int> HEART_WALL_POS_X{  };
	vector <int> HEART_WALL_POS_Y{  };
	vector <int> HEART_WALL_HEIGHT{  };
	vector <int> HEART_WALL_WIDTH{  };

	vector <int> HEART_FLOOR_WIDTH{  };
	vector <int> HEART_FLOOR_POS_X{  };
	vector <int> HEART_FLOOR_POS_Y{  };

	vector <int> WAFFLE_POS_X{ }; 
	vector <int> WAFFLE_POS_Y{  }; 
	vector <int> WAFFLE_HEIGHT{  }; 
	vector <int> WAFFLE_WIDTH{  };

	Point2D waterPos{  };
	int waterLength{  };

	vector <int> SALMON_POS_X{  };
	vector <int> SALMON_POS_Y{  };

	vector <int> CHICKEN_POS_X{  };
	vector <int> CHICKEN_POS_Y{  };

	vector <int> FLOWER_POS_X{  };
	vector <int> FLOWER_POS_Y{  };

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
	bool flyActive{ false };

	bool fleaActivated{ false };
};

struct GameState
{
	int score{ 0 };
	int level{ 1 };
	int lives{ 9 };
	int highScore{ 0 };
	int floor{ 0 };
	int treats{ 0 };

	PlayerState playerState = STATE_GROUNDED;
	PlayerState playerPrevState = STATE_GROUNDED;
	PlayerState enemyState = STATE_HIDE;
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
void ResetPlayer();
void PlayerAndEnemyRelations();

void Draw();
void DrawLevel();
void DrawGameStats();
void DrawDebugInfo();

void CoordsPlatform1();

void CreateGamePlay();
void CreateObjects(Platform& platform);
void CreatePlatform(Platform& platform);
void BuildSideWalls(Platform& platform);
void CreateWalls(vector <int>& w, vector <int>& h, vector <int>& x, vector <int>& y, float scale, const char* s, int TYPE);
void CreateFloors(vector <int>& w, vector <int>& x, vector <int>& y, float scale, const char* s, int TYPE);
void ItemsPlacement(Platform& platform);
void PlaceWater(Platform& platform);

void Jump();
void Fall();
void Hiss();
void FleaJump();
void WalkingPlayerControls();
void IdlePlayerControls();
void AttachedPlayerControls();
void FurballPlayerControls();
void JumpControls();

void UpdatePlayer();
void UpdateEnemy();
void UpdateFleas();
void UpdateTreats();
void UpdatePoo();
void UpdateFlies();
void PoopControl();
void UpdateWater();

void UpdateDestroyed();
void LoopObject(GameObject& object);

float DotProduct(const Vector2D& v1, const Vector2D& v2);
void Normalise(Vector2D& v);
float Randomize(int range, float multiplier = 1.0f);

bool IsPlayerColliding(const GameObject& object);
bool IsEnemyColliding(const GameObject& object);
bool IsPlayerCollidingUpperPart(const GameObject& object);
bool IsPlayerCollidingBottomPart(const GameObject& object);
bool IsPlayerCollidingAnyPlatform();
bool IsPlayerCollidingAnyPlatformBoth();
bool IsPlayerCollidingAnyPlatformUpper();
bool IsPlayerCollidingLadder();
bool IsPlayerStillCollidingLadder();
bool IsPlayerOnLadder();
bool IsPlayerCollidingGround();

void JumpCollisionControl();

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground("Data//Backgrounds//forest1.png");	
	
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
			UpdateEnemy();
			UpdateFleas();
			UpdateTreats();
			UpdatePoo();
			UpdateFlies();
			UpdateWater();
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
		ResetPlayer();

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

// insert values / coords for the platforms  
void CoordsPlatform1()
{
	// 1st floor : 18
	// 2nd floor : 13
	// 3rd floor : 8
	// 4th floor : 3
	// 5th floor : 1
	// 6th floor : -4

	Platform& p = platform1;
	
	const int FLOOR_1 = p.GROUND_FLOOR_HEIGHT;
	const int FLOOR_2 = p.SECOND_FLOOR_HEIGHT;
	const int FLOOR_3 = p.THIRD_FLOOR_HEIGHT;
	const int FLOOR_4 = p.FOURTH_FLOOR_HEIGHT;
	const int FLOOR_5 = p.FIFTH_FLOOR_HEIGHT;
	const int FLOOR_6 = p.SIXTH_FLOOR_HEIGHT;

	// player and enemies start position
	p.playerStartPos = { 2, FLOOR_1 - 2 };
	p.fleaStartPos = { -8, FLOOR_2 - 1};
	
	// horizontal platforms / floors
	p.GROUND_FLOOR_WIDTH.insert(p.GROUND_FLOOR_WIDTH.begin(), { p.LIMIT_RIGHT - p.LIMIT_LEFT });
	p.GROUND_FLOOR_POS_X.insert(p.GROUND_FLOOR_POS_X.begin(), { p.LIMIT_LEFT });
	p.GROUND_FLOOR_POS_Y.insert(p.GROUND_FLOOR_POS_Y.begin(), { FLOOR_1 });

	p.C_FLOOR_WIDTH.insert(p.C_FLOOR_WIDTH.begin(), { 10, 11, 11, 3, 3 });
	p.C_FLOOR_POS_X.insert(p.C_FLOOR_POS_X.begin(), { -13, 10, 10, 0, -7 });
	p.C_FLOOR_POS_Y.insert(p.C_FLOOR_POS_Y.begin(), { FLOOR_2 - 1, FLOOR_2 - 1, FLOOR_3 - 1, FLOOR_3 - 1, FLOOR_3 - 1, });

	p.HEART_FLOOR_WIDTH.insert(p.HEART_FLOOR_WIDTH.begin(), {34 });
	p.HEART_FLOOR_POS_X.insert(p.HEART_FLOOR_POS_X.begin(), { -13 });
	p.HEART_FLOOR_POS_Y.insert(p.HEART_FLOOR_POS_Y.begin(), { FLOOR_4 - 1 });

	// grass with dirt
	p.GRASS_WIDTH.insert(p.GRASS_WIDTH.begin(), {  });
	p.GRASS_FLOOR_POS_X.insert(p.GRASS_FLOOR_POS_X.begin(), { 5, 2 });
	p.GRASS_FLOOR_POS_Y.insert(p.GRASS_FLOOR_POS_Y.begin(), { FLOOR_4, FLOOR_2 });

	// vertical platforms / walls
	p.NUT_WALL_HEIGHT.insert(p.NUT_WALL_HEIGHT.begin(), { 2, 2 });
	p.NUT_WALL_WIDTH.insert(p.NUT_WALL_WIDTH.begin(), { 2, 2 });
	p.NUT_WALL_POS_X.insert(p.NUT_WALL_POS_X.begin(), { -15, 21 });
	p.NUT_WALL_POS_Y.insert(p.NUT_WALL_POS_Y.begin(), { FLOOR_2 + 3, FLOOR_2 + 3 });

	p.HEART_WALL_HEIGHT.insert(p.HEART_WALL_HEIGHT.begin(), { 3, 3, 2, 2, 9, 13 });
	p.HEART_WALL_WIDTH.insert(p.HEART_WALL_WIDTH.begin(), { 2, 2, 2, 2, 2, 2 });
	p.HEART_WALL_POS_X.insert(p.HEART_WALL_POS_X.begin(), { -15, 21, -15, 21, -15, 21 });
	p.HEART_WALL_POS_Y.insert(p.HEART_WALL_POS_Y.begin(), { FLOOR_2, FLOOR_2, FLOOR_3, FLOOR_3, FLOOR_6, FLOOR_6 });

	// dirt blocks
	p.DIRT_HEIGHT.insert(p.DIRT_HEIGHT.begin(), {  });
	p.DIRT_WIDTH.insert(p.DIRT_WIDTH.begin(), { 2, 5 });
	p.DIRT_POS_X.insert(p.DIRT_POS_X.begin(), { -1, 10 });
	p.DIRT_POS_Y.insert(p.DIRT_POS_Y.begin(), { FLOOR_1 - 10, FLOOR_1 - 15 });

	// ladders
	p.WAFFLE_HEIGHT.insert(p.WAFFLE_HEIGHT.begin(), { 15 });
	p.WAFFLE_WIDTH.insert(p.WAFFLE_WIDTH.begin(), { 1 });
	p.WAFFLE_POS_X.insert(p.WAFFLE_POS_X.begin(), { 3 });
	p.WAFFLE_POS_Y.insert(p.WAFFLE_POS_Y.begin(), { FLOOR_4 });

	// water
	p.waterPos = { };
	p.waterLength = {  };

	// cat treats
	p.SALMON_POS_X.insert(p.SALMON_POS_X.begin(), { 7 });
	p.SALMON_POS_Y.insert(p.SALMON_POS_Y.begin(), { FLOOR_2 - 1 });

	p.CHICKEN_POS_X.insert(p.CHICKEN_POS_X.begin(), { 5 });
	p.CHICKEN_POS_Y.insert(p.CHICKEN_POS_Y.begin(), { FLOOR_3 - 1 });

	p.FLOWER_POS_X.insert(p.FLOWER_POS_X.begin(), { 0 });
	p.FLOWER_POS_Y.insert(p.FLOWER_POS_Y.begin(), { FLOOR_4 - 1 });

	// boxes
	p.BOX_POS_X.insert(p.BOX_POS_X.begin(), { -9, -8, -7, -8, -7, -6 });
	p.BOX_POS_Y.insert(p.BOX_POS_Y.begin(), { FLOOR_2 - 1, FLOOR_2 - 1, FLOOR_2 - 1, FLOOR_2 - 2, FLOOR_2 - 2, FLOOR_2 - 1, });
}

void CoordsPlatform2()
{
	// 1st floor : 18
	// 2nd floor : 13
	// 3rd floor : 8
	// 4th floor : 3
	// 5th floor : 1
	// 6th floor : -4
	
	Platform& p = platform2;

	const int FLOOR_1 = p.GROUND_FLOOR_HEIGHT;
	const int FLOOR_2 = p.SECOND_FLOOR_HEIGHT;
	const int FLOOR_3 = p.THIRD_FLOOR_HEIGHT;
	const int FLOOR_4 = p.FOURTH_FLOOR_HEIGHT;
	const int FLOOR_5 = p.FIFTH_FLOOR_HEIGHT;
	const int FLOOR_6 = p.SIXTH_FLOOR_HEIGHT;

	// player and enemies start position
	p.playerStartPos = { 2, FLOOR_1 - 2 };
	p.fleaStartPos = { -8, FLOOR_2 };

	// horizontal platforms / floors
	p.GROUND_FLOOR_WIDTH.insert(p.GROUND_FLOOR_WIDTH.begin(), {  });
	p.GROUND_FLOOR_POS_X.insert(p.GROUND_FLOOR_POS_X.begin(), {  });
	p.GROUND_FLOOR_POS_Y.insert(p.GROUND_FLOOR_POS_Y.begin(), {  });

	p.C_FLOOR_WIDTH.insert(p.C_FLOOR_WIDTH.begin(), {  });
	p.C_FLOOR_POS_X.insert(p.C_FLOOR_POS_X.begin(), {  });
	p.C_FLOOR_POS_Y.insert(p.C_FLOOR_POS_Y.begin(), {  });

	p.HEART_FLOOR_WIDTH.insert(p.HEART_FLOOR_WIDTH.begin(), {  });
	p.HEART_FLOOR_POS_X.insert(p.HEART_FLOOR_POS_X.begin(), {  });
	p.HEART_FLOOR_POS_Y.insert(p.HEART_FLOOR_POS_Y.begin(), {  });

	// grass with dirt
	p.GRASS_WIDTH.insert(p.GRASS_WIDTH.begin(), {  });
	p.GRASS_FLOOR_POS_X.insert(p.GRASS_FLOOR_POS_X.begin(), {  });
	p.GRASS_FLOOR_POS_Y.insert(p.GRASS_FLOOR_POS_Y.begin(), {  });

	// vertical platforms / walls
	p.NUT_WALL_HEIGHT.insert(p.NUT_WALL_HEIGHT.begin(), {  });
	p.NUT_WALL_WIDTH.insert(p.NUT_WALL_WIDTH.begin(), {  });
	p.NUT_WALL_POS_X.insert(p.NUT_WALL_POS_X.begin(), {  });
	p.NUT_WALL_POS_Y.insert(p.NUT_WALL_POS_Y.begin(), {  });

	p.HEART_WALL_HEIGHT.insert(p.HEART_WALL_HEIGHT.begin(), {  });
	p.HEART_WALL_WIDTH.insert(p.HEART_WALL_WIDTH.begin(), {  });
	p.HEART_WALL_POS_X.insert(p.HEART_WALL_POS_X.begin(), {  });
	p.HEART_WALL_POS_Y.insert(p.HEART_WALL_POS_Y.begin(), {  });

	// dirt blocks
	p.DIRT_HEIGHT.insert(p.DIRT_HEIGHT.begin(), {  });
	p.DIRT_WIDTH.insert(p.DIRT_WIDTH.begin(), {  });
	p.DIRT_POS_X.insert(p.DIRT_POS_X.begin(), {  });
	p.DIRT_POS_Y.insert(p.DIRT_POS_Y.begin(), {  });

	// ladders
	p.WAFFLE_HEIGHT.insert(p.WAFFLE_HEIGHT.begin(), {  });
	p.WAFFLE_WIDTH.insert(p.WAFFLE_WIDTH.begin(), {  });
	p.WAFFLE_POS_X.insert(p.WAFFLE_POS_X.begin(), {  });
	p.WAFFLE_POS_Y.insert(p.WAFFLE_POS_Y.begin(), {  });

	// water
	p.waterPos = {  };

	// cat treats
	p.SALMON_POS_X.insert(p.SALMON_POS_X.begin(), {  });
	p.SALMON_POS_Y.insert(p.SALMON_POS_Y.begin(), {  });

	p.CHICKEN_POS_X.insert(p.CHICKEN_POS_X.begin(), {  });
	p.CHICKEN_POS_Y.insert(p.CHICKEN_POS_Y.begin(), {  });

	p.FLOWER_POS_X.insert(p.FLOWER_POS_X.begin(), { 0 });
	p.FLOWER_POS_Y.insert(p.FLOWER_POS_Y.begin(), { FLOOR_4 - 1 });

	// boxes
	p.BOX_POS_X.insert(p.BOX_POS_X.begin(), { });
	p.BOX_POS_Y.insert(p.BOX_POS_Y.begin(), {  });
}

void CoordsPlatform3()
{
	// 1st floor : 18
	// 2nd floor : 13
	// 3rd floor : 8
	// 4th floor : 3
	// 5th floor : 1
	// 6th floor : -4
	
	Platform& p = platform2;

	const int FLOOR_1 = p.GROUND_FLOOR_HEIGHT;
	const int FLOOR_2 = p.SECOND_FLOOR_HEIGHT;
	const int FLOOR_3 = p.THIRD_FLOOR_HEIGHT;
	const int FLOOR_4 = p.FOURTH_FLOOR_HEIGHT;
	const int FLOOR_5 = p.FIFTH_FLOOR_HEIGHT;
	const int FLOOR_6 = p.SIXTH_FLOOR_HEIGHT;

	// player and enemies start position
	p.playerStartPos = { 2, FLOOR_1 - 2 };
	p.fleaStartPos = { -8, FLOOR_2 };

	// horizontal platforms / floors
	p.GROUND_FLOOR_WIDTH.insert(p.GROUND_FLOOR_WIDTH.begin(), {  });
	p.GROUND_FLOOR_POS_X.insert(p.GROUND_FLOOR_POS_X.begin(), {  });
	p.GROUND_FLOOR_POS_Y.insert(p.GROUND_FLOOR_POS_Y.begin(), {  });

	p.C_FLOOR_WIDTH.insert(p.C_FLOOR_WIDTH.begin(), {  });
	p.C_FLOOR_POS_X.insert(p.C_FLOOR_POS_X.begin(), {  });
	p.C_FLOOR_POS_Y.insert(p.C_FLOOR_POS_Y.begin(), {  });

	p.HEART_FLOOR_WIDTH.insert(p.HEART_FLOOR_WIDTH.begin(), {  });
	p.HEART_FLOOR_POS_X.insert(p.HEART_FLOOR_POS_X.begin(), {  });
	p.HEART_FLOOR_POS_Y.insert(p.HEART_FLOOR_POS_Y.begin(), {  });

	// grass with dirt
	p.GRASS_WIDTH.insert(p.GRASS_WIDTH.begin(), {  });
	p.GRASS_FLOOR_POS_X.insert(p.GRASS_FLOOR_POS_X.begin(), {  });
	p.GRASS_FLOOR_POS_Y.insert(p.GRASS_FLOOR_POS_Y.begin(), {  });

	// vertical platforms / walls
	p.NUT_WALL_HEIGHT.insert(p.NUT_WALL_HEIGHT.begin(), {  });
	p.NUT_WALL_WIDTH.insert(p.NUT_WALL_WIDTH.begin(), {  });
	p.NUT_WALL_POS_X.insert(p.NUT_WALL_POS_X.begin(), {  });
	p.NUT_WALL_POS_Y.insert(p.NUT_WALL_POS_Y.begin(), {  });

	p.HEART_WALL_HEIGHT.insert(p.HEART_WALL_HEIGHT.begin(), {  });
	p.HEART_WALL_WIDTH.insert(p.HEART_WALL_WIDTH.begin(), {  });
	p.HEART_WALL_POS_X.insert(p.HEART_WALL_POS_X.begin(), {  });
	p.HEART_WALL_POS_Y.insert(p.HEART_WALL_POS_Y.begin(), {  });

	// dirt blocks
	p.DIRT_HEIGHT.insert(p.DIRT_HEIGHT.begin(), {  });
	p.DIRT_WIDTH.insert(p.DIRT_WIDTH.begin(), {  });
	p.DIRT_POS_X.insert(p.DIRT_POS_X.begin(), {  });
	p.DIRT_POS_Y.insert(p.DIRT_POS_Y.begin(), {  });

	// ladders
	p.WAFFLE_HEIGHT.insert(p.WAFFLE_HEIGHT.begin(), {  });
	p.WAFFLE_WIDTH.insert(p.WAFFLE_WIDTH.begin(), {  });
	p.WAFFLE_POS_X.insert(p.WAFFLE_POS_X.begin(), {  });
	p.WAFFLE_POS_Y.insert(p.WAFFLE_POS_Y.begin(), {  });

	// water
	p.waterPos = {  };

	// cat treats
	p.SALMON_POS_X.insert(p.SALMON_POS_X.begin(), {  });
	p.SALMON_POS_Y.insert(p.SALMON_POS_Y.begin(), {  });

	p.CHICKEN_POS_X.insert(p.CHICKEN_POS_X.begin(), {  });
	p.CHICKEN_POS_Y.insert(p.CHICKEN_POS_Y.begin(), {  });

	// boxes
	p.BOX_POS_X.insert(p.BOX_POS_X.begin(), { });
	p.BOX_POS_Y.insert(p.BOX_POS_Y.begin(), {  });

	p.FLOWER_POS_X.insert(p.FLOWER_POS_X.begin(), { 0 });
	p.FLOWER_POS_Y.insert(p.FLOWER_POS_Y.begin(), { FLOOR_4 - 1 });
}

// create all the objects for a game level
void CreateGamePlay()
{
	switch (gameState.level)
	{
		case 1:
		{
			CoordsPlatform1();
			CreatePlatform(platform1);
			ItemsPlacement(platform1);
			CreateObjects(platform1);
			break;
		}
		default:
		{
			CoordsPlatform2();
			CreatePlatform(platform2);
			ItemsPlacement(platform2);
			CreateObjects(platform2);
			break;
		}
	}
}

// create player and enemies
void CreateObjects(Platform& platform)
{
	int id = Play::CreateGameObject(TYPE_PLAYER, { platform.playerStartPos.x * TILE_SIZE, platform.playerStartPos.y * TILE_SIZE - PLAYER_OFFSET_Y }, 10, "cat_sits_right");
	GameObject& objPlayer = Play::GetGameObject(id);
	objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	objPlayer.scale = 4.5f;

	id = Play::CreateGameObject(TYPE_ENEMY, { platform.fleaStartPos.x * TILE_SIZE, platform.fleaStartPos.y * TILE_SIZE - FLEA_OFFSET_Y }, 10, "bug_right");
	GameObject& objEnemy = Play::GetGameObject(id);
	objEnemy.scale = 0.5f;
	objEnemy.rotation = Play::DegToRad(-30);
	objEnemy.animSpeed = .05f;
	objEnemy.velocity = PLAYER_VELOCITY_DEFAULT;

	Play::MoveSpriteOrigin("cat_go_right", 0, -3);
	Play::MoveSpriteOrigin("cat_go_left", 0, -3);
	Play::MoveSpriteOrigin("cat_poop_right", 0, -3);
	Play::MoveSpriteOrigin("cat_poop_left", 0, -3);
	Play::MoveSpriteOrigin("cat_hiss_new", 0, -3);
	Play::MoveSpriteOrigin("cat_hiss_new_left", 0, -3);
}

// create the platform for a game level
void CreatePlatform(Platform& platform)
{
	BuildSideWalls(platform);
	PlaceWater(platform1);

	CreateFloors(platform.GROUND_FLOOR_WIDTH, platform.GROUND_FLOOR_POS_X, platform.GROUND_FLOOR_POS_Y, 2.5f, "the_ground", TYPE_GROUND);

	//to decide which sprite to use.........
	CreateFloors(platform.GRASS_WIDTH, platform.GRASS_FLOOR_POS_X, platform.GRASS_FLOOR_POS_Y, 2.5f,  "heart_choco", TYPE_PLATFORM);
	CreateFloors(platform.C_FLOOR_WIDTH, platform.C_FLOOR_POS_X, platform.C_FLOOR_POS_Y, 2.5f, "c_choco", TYPE_PLATFORM);
	CreateFloors(platform.HEART_FLOOR_WIDTH, platform.HEART_FLOOR_POS_X, platform.HEART_FLOOR_POS_Y, 2.5f, "heart_choco", TYPE_PLATFORM);

	CreateWalls(platform.HEART_WALL_WIDTH, platform.HEART_WALL_HEIGHT, platform.HEART_WALL_POS_X, platform.HEART_WALL_POS_Y, 2.5f, "heart_choco", TYPE_PLATFORM);
	CreateWalls(platform.NUT_WALL_WIDTH, platform.NUT_WALL_HEIGHT, platform.NUT_WALL_POS_X, platform.NUT_WALL_POS_Y, 2.5f,  "choco_nuts", TYPE_PLATFORM);

	//to decide which sprite to use.........
	CreateWalls(platform.DIRT_WIDTH, platform.DIRT_HEIGHT, platform.DIRT_POS_X, platform.DIRT_POS_Y, 2.5f, "jumpy_texture", TYPE_PLATFORM);

	CreateWalls(platform.WAFFLE_WIDTH, platform.WAFFLE_HEIGHT, platform.WAFFLE_POS_X, platform.WAFFLE_POS_Y, 2.5f, "waff", TYPE_LADDER);
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

void PlaceWater(Platform& platform)
{
	for (int i = 0; i < platform.waterLength; i++)
	{
		int id = Play::CreateGameObject(TYPE_WATER, { platform.waterPos.x * TILE_SIZE + TILE_SIZE * i, platform.waterPos.y * TILE_SIZE }, 10, "water_long");
		GameObject& water = Play::GetGameObject(id);
		water.scale = 6.f;
		water.animSpeed = 0.3f;
	}
}

// place other items for a game level
void ItemsPlacement(Platform& platform)
{
	for (int i = 0; i < platform.SALMON_POS_X.size(); i++)
	{
		int id = Play::CreateGameObject(
			TYPE_TREAT,
			Point2D{ platform.SALMON_POS_X[i] * TILE_SIZE, platform.SALMON_POS_Y[i] * TILE_SIZE },
			10,
			"croissant");
		Play::GetGameObject(id).scale = 2.5f;
		Play::GetGameObject(id).rotation = Randomize(Play::RandomRoll(360), 0.1);
	}

	for (int i = 0; i < platform.CHICKEN_POS_X.size(); i++)
	{
		int id = Play::CreateGameObject(
			TYPE_TREAT,
			Point2D{ platform.CHICKEN_POS_X[i] * TILE_SIZE, platform.CHICKEN_POS_Y[i] * TILE_SIZE },
			10,
			"apple");
		Play::GetGameObject(id).scale = 2.f;
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

	if (platform.isWater)
	{
		int id = Play::CreateGameObject(TYPE_WATER, platform.waterPos * TILE_SIZE, 10, "water_long");
		Play::GetGameObject(id).scale = 6.f;
	}

	for (int i = 0; i < platform.FLOWER_POS_X.size(); i++)
	{
		Point2D pos = { platform.FLOWER_POS_X[i] * TILE_SIZE, platform.FLOWER_POS_Y[i] * TILE_SIZE };
		int random = Play::RandomRoll(4);
		int id = 0;

		if (random == 1)
		{
			id = Play::CreateGameObject(TYPE_FLOWER, pos, 10, "yellow_flower");
			Play::GetGameObject(id).scale = 2.5f;
		}
		else if (random == 2)
		{
			id = Play::CreateGameObject(TYPE_FLOWER, pos, 10, "violet_flower");
			Play::GetGameObject(id).scale = 2.5f;
		}
		else if (random == 3)
		{
			id = Play::CreateGameObject(TYPE_FLOWER, pos, 10, "white_flower");
			Play::GetGameObject(id).scale = 2.5f;
		}
		else
		{
			id = Play::CreateGameObject(TYPE_FLOWER, pos, 10, "pink_flower");
			Play::GetGameObject(id).scale = 2.5f;
		}

		Play::GetGameObject(id).scale = 2.5f;
	}
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
	else if (objPlayer.pos.x < CAMERA_THRESHOLD_X.x * TILE_SIZE)
		cameraPos.x = (platform1.LIMIT_LEFT * TILE_SIZE) - TILE_SIZE;
	else if (objPlayer.pos.x > CAMERA_THRESHOLD_X.y * TILE_SIZE)
		cameraPos.x = (platform1.LIMIT_RIGHT * TILE_SIZE) - (TILE_WIDTH * TILE_SIZE) + 5;
	else
		cameraPos = { objPlayer.pos.x - CAMERA_OFFSET_X, 0 };

	Play::SetCameraPosition(cameraPos);
}

// set player pos y when grounded
void SetPlayerPos(int pos)
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	(flags.isGroundFloor) ?
		objPlayer.pos.y = PLAYER_START_POS.y * TILE_SIZE - PLAYER_OFFSET_Y :
		objPlayer.pos.y = pos - PLAYER_OFFSET_Y;
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

	if (gameState.treats > 0 && gameState.floor < 2)
	{
		poopTimer += DELTA_TIME;

		if (poopTimer / DELTA_TIME < 10 && poopTimer > 0.f)
		{
			(flags.right) ? Play::SetSprite(objPlayer, "cat_poop_right", 0.05f) : Play::SetSprite(objPlayer, "cat_poop_left", 0.05f) ;
		}
		else
		{
			gameState.treats = 0;
			poopTimer = 0.f;
			int id = Play::CreateGameObject(TYPE_POO, { objPlayer.pos.x, objPlayer.pos.y + POO_OFFSET_Y}, 10, "the_poo");
			GameObject& poo = Play::GetGameObject(id);
			poo.scale = 2.f;
			poo.animSpeed = 0.1f;

			(flags.right) ? Play::SetSprite(objPlayer, "cat_sits_right", 0.1f) : Play::SetSprite(objPlayer, "cat_sits_left", 0.1f);
		}
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
	}

	if (Play::KeyDown(VK_SHIFT))
	{
		gameState.playerPrevState = gameState.playerState;
		gameState.playerState = STATE_FURBALL;
	}

	PlayerAndEnemyRelations();

	Play::UpdateGameObject(objPlayer);
}

void UpdateEnemy()
{
	GameObject& objEnemy = Play::GetGameObjectByType(TYPE_ENEMY);
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	switch (gameState.enemyState)
	{
		case STATE_HIDE:
		{
			fleaTimer += DELTA_TIME;
			break;
		}
		case STATE_WALK:
		{		
			objEnemy.rotation = 0;
			fleaTimer += DELTA_TIME;
			break;
		}
		case STATE_FALL:
		{		
			break;
		}
		case STATE_JUMP:
		{		
			FleaJump();
			break;
		}
		case STATE_ATTACHED:
		{
			
			break;
		}
	}

	PlayerAndEnemyRelations();

	Play::UpdateGameObject(objEnemy);
}

void PlayerAndEnemyRelations()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	GameObject& objEnemy = Play::GetGameObjectByType(TYPE_ENEMY);

	/*if (IsEnemyColliding(objPlayer))
	{
		objEnemy.pos = FLEA_START_POS * TILE_SIZE;
		gameState.enemyState = STATE_HIDE;
		gameState.playerState = STATE_HISS;
		gameState.lives--;
	}*/


	if (objEnemy.pos.x > platform1.LIMIT_RIGHT * TILE_SIZE)
		objEnemy.pos.x = platform1.LIMIT_LEFT * TILE_SIZE;

	if (fleaTimer > 1.0f)
	{
		objEnemy.velocity = ENEMY_VELOCITY_WALK;
		gameState.enemyState = STATE_WALK;
	}

	if (fleaTimer > 3.f)
	{
		objEnemy.rotation = Play::DegToRad(-30);
		objEnemy.velocity = ENEMY_VELOCITY_JUMP;
		gameState.enemyState = STATE_JUMP;
		fleaTimer = 1.f;
	}

	if (IsPlayerCollidingUpperPart(objEnemy))
	{
		objEnemy.pos = FLEA_START_POS * TILE_SIZE;
		gameState.enemyState = STATE_HIDE;
		gameState.playerPrevState = gameState.playerState;
		gameState.playerState = STATE_HISS;
		gameState.lives--;

	}
}

void UpdateFleas()
{
	vector <int> vFleas = Play::CollectGameObjectIDsByType(TYPE_FLEA);

	for (int fleaId : vFleas)
	{
		GameObject& flea = Play::GetGameObject(fleaId);

		if (fleaTimer > 2.f)
		{
			flea.pos.x += 10;
			fleaTimer = 0.f;
		}
		//LoopObject(flea);
		fleaTimer += DELTA_TIME;
		Play::UpdateGameObject(flea);
	}
}

void UpdateFlies()
{
	vector <int> vRFlies = Play::CollectGameObjectIDsByType(TYPE_RIGHT_FLY);
	for (int flyId : vRFlies)
	{
		GameObject& objFly = Play::GetGameObject(flyId);

		objFly.pos.x += FLY_SPEED;
		objFly.pos.y += sin(objFly.pos.x / 100) * 3;

		if (objFly.pos.x > platform1.LIMIT_RIGHT * TILE_SIZE)
			objFly.pos = RIGHT_FLY_START_POS;

		if (IsPlayerCollidingUpperPart(objFly))
		{
			if (gameState.lives < 9)
				gameState.lives++;

			Play::DestroyGameObject(flyId);
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
			objFly.pos = LEFT_FLY_START_POS;

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

	for (int treatId : vTreats)
	{
		GameObject& treat = Play::GetGameObject(treatId);

		if (IsPlayerColliding(treat))
		{
			gameState.score += 10;
			gameState.treats++;
			Play::DestroyGameObject(treatId);
		}
		Play::UpdateGameObject(treat);
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
		Play::UpdateGameObject(poo);
	}
}

void UpdateWater()
{
	vector <int> vWater = Play::CollectGameObjectIDsByType(TYPE_WATER);

	for (int waterId : vWater)
	{
		GameObject& water = Play::GetGameObject(waterId);

		if (IsPlayerColliding(water))
		{
			ResetPlayer();
			gameState.lives--;
		}

		Play::UpdateGameObject(water);
	}
}

// timers 
void CoyoteControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (!IsPlayerCollidingAnyPlatform() && !IsPlayerStillCollidingLadder() && gameState.floor != 1)
		coyoteTimer += DELTA_TIME;

	if (!IsPlayerCollidingAnyPlatform() && !IsPlayerStillCollidingLadder() && !IsPlayerCollidingLadder() && objPlayer.pos.y < PLAYER_START_POS.y * TILE_WIDTH && coyoteTimer > 0.1f)
	{
		coyoteTimer = 0.f;
		flags.isGrounded = false;
		gameState.playerState = STATE_FALL;
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

// controls for different player states
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

void AttachedPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
	objPlayer.animSpeed = 0.f;

	// if player is not standing on ladder or colliding with ladder, then fall
	if (!IsPlayerCollidingLadder() && !IsPlayerOnLadder())
		gameState.playerState = STATE_FALL;

	if (gameState.floor == 2 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_SECOND + LADDER_OFFSET_Y
		|| gameState.floor == 3 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_THIRD + LADDER_OFFSET_Y
		|| gameState.floor == 4 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_FOURTH + LADDER_OFFSET_Y
		|| gameState.floor == 5 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_FIFTH + LADDER_OFFSET_Y
		|| gameState.floor == 6 && objPlayer.pos.y <= platform1.LADDER_TOP_FLOOR_SIXTH + LADDER_OFFSET_Y)
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

		SetPlayerPos(ladderPos.y);
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
			(flags.right) ? objPlayer.pos.x -= 15 : objPlayer.pos.x += 15;

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

	else if (Play::KeyDown(VK_SPACE) && !IsPlayerCollidingAnyPlatformUpper())
	{
		objPlayer.pos.x += sin(objPlayer.rotation) * 25;
		objPlayer.pos.y -= cos(objPlayer.rotation) * 25;
		objPlayer.rotation = 0;
	}
}

void FurballPlayerControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	Play::SetSprite(objPlayer, "furball", 0.f);

	(flags.right) ? objPlayer.rotSpeed = 1.1f : objPlayer.rotSpeed = -1.1f;

	if (furballTimer > 2.f  && !Play::KeyDown(VK_RIGHT) && !Play::KeyDown(VK_LEFT))
	{
		gameState.playerState = STATE_GROUNDED;
		objPlayer.rotSpeed = 0.f;
		objPlayer.rotation = Play::DegToRad(0);
		furballTimer = 0.f;
	}
	
	// && objPlayer.rotation == Play::DegToRad(0)
	if(Play::KeyDown(VK_RIGHT))
	{
		furballTimer = 0.f;
		objPlayer.pos.x += 10;	
	}
	else if (Play::KeyDown(VK_LEFT))
	{
		furballTimer = 0.f;
		objPlayer.pos.x -= 10;
	}

	furballTimer += DELTA_TIME;
}

// jump off the ground / ladder
void JumpControls()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (!IsPlayerCollidingLadder())
	{
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
}

// updating behaviours for a PLAYER
void Jump()
{
	JumpCollisionControl();

	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	coyoteTimer = 0.f;

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
}

void JumpCollisionControl()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);

	if (IsPlayerCollidingLadder())
		gameState.playerState = STATE_ATTACHED;

	vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);

		if (IsPlayerCollidingBottomPart(platform) && jumpTimer > 0.3f)
		{
			flags.isGrounded = true;
			SetPlayerPos(groundedPosY);
			gameState.playerState = STATE_GROUNDED;
			objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
			jumpTimer = 0.f;
		}
		else if (IsPlayerCollidingUpperPart(platform) && objPlayer.velocity.y < 0)
		{
			objPlayer.velocity.y = 0.f;
			gameState.playerState = STATE_FALL;
		}
	}

	vector <int> vGrounds = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int groundId : vGrounds)
	{
		GameObject& ground = Play::GetGameObject(groundId);

		if (IsPlayerCollidingBottomPart(ground) && jumpTimer > 0.5f)
		{
			flags.isGrounded = true;
			SetPlayerPos(groundedPosY - TILE_SIZE);
			gameState.playerState = STATE_GROUNDED;
			objPlayer.velocity = PLAYER_VELOCITY_DEFAULT;
			jumpTimer = 0.f;
		}
	}

	vector <int> vWalls = Play::CollectGameObjectIDsByType(TYPE_WALL);
	for (int wallId : vWalls)
	{
		GameObject& wall = Play::GetGameObject(wallId);

		if (IsPlayerCollidingUpperPart(wall))
		{
			objPlayer.velocity = { 0.f, 0.f };
			gameState.playerState = STATE_FALL;
		}
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

	if (gameState.playerPrevState == STATE_JUMP || gameState.playerPrevState == STATE_FALL || hissTimer > 1.2f)
	{
		gameState.playerState = gameState.playerPrevState;
		hissTimer = 0.f;
	}

	(flags.right) ?
		Play::SetSprite(objPlayer, "cat_hiss", 0.1f) :
		Play::SetSprite(objPlayer, "cat_hiss_new_left", 0.1f);
	hissTimer += DELTA_TIME;
}

// updating jump for ENEMY
void FleaJump()
{
	GameObject& objEnemy = Play::GetGameObjectByType(TYPE_ENEMY);
	
	objEnemy.velocity += GRAVITY * DELTA_TIME;

	vector <int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);

		if (IsEnemyColliding(platform))
		{
			objEnemy.velocity = PLAYER_VELOCITY_DEFAULT;
			gameState.enemyState = STATE_WALK;
		}
	}

	vector <int> vGrounds = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int groundId : vGrounds)
	{
		GameObject& ground = Play::GetGameObject(groundId);

		if (IsEnemyColliding(ground))
		{
			objEnemy.velocity = PLAYER_VELOCITY_DEFAULT;
			gameState.enemyState = STATE_WALK;
		}
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
		case TYPE_ENEMY:
			AABB = ENEMY_AABB;
			break;
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

bool IsEnemyColliding(const GameObject& object)
{
	GameObject& objEnemy = Play::GetGameObjectByType(TYPE_ENEMY);

	Vector2D AABB = { 0.f, 0.f };

	switch (object.type)
	{
		case TYPE_PLATFORM:
			AABB = PLATFORM_AABB;
			break;
		case TYPE_GROUND:
			AABB = GROUND_PLATFORM_AABB;
			break;
		case TYPE_PLAYER:
			AABB = PLAYER_AABB_BOTTOM;
			break;
	}
	
	if (objEnemy.pos.y - ENEMY_AABB.y < object.pos.y + AABB.y &&
		objEnemy.pos.y + ENEMY_AABB.y > object.pos.y - AABB.y)
	{
		if (objEnemy.pos.x + ENEMY_AABB.x > object.pos.x - AABB.x &&
			objEnemy.pos.x - ENEMY_AABB.x < object.pos.x + AABB.x)
			return true;
	}
	return false;
}

// if object is out of x bounds , loop it to the other side
void LoopObject(GameObject& object)
{
	if(object.pos.x < 0)
		object.pos.x = DISPLAY_WIDTH;
	else if (object.pos.x > DISPLAY_WIDTH)
		object.pos.x = 0;
}

// helper functions
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
	DrawDebugInfo();
	DrawGameStats();

	Play::PresentDrawingBuffer();
}

void DrawGameStats()
{
	Play::SetDrawingSpace(Play::SCREEN);

	Play::DrawFontText("64px", "Score: " + std::to_string(gameState.score), { 150, 50 }, Play::RIGHT);

	(gameState.lives > 0) ?
		Play::DrawSpriteRotated("red_heart", { 450, 50 }, 1, 0.f,  3.5f ) :
		Play::DrawSpriteRotated("white_heart", { 450, 50 }, 1, 0.f,  3.5f );

	(gameState.lives > 1) ?
		Play::DrawSpriteRotated("red_heart", { 500, 50 }, 1, 0.f, 3.5f ) :
		Play::DrawSpriteRotated("white_heart", { 500, 50 }, 1, 0.f, 3.5f );

	(gameState.lives > 2) ?
		Play::DrawSpriteRotated("red_heart", { 550, 50 }, 1, 0.f, 3.5f ) :
		Play::DrawSpriteRotated("white_heart", { 550, 50 }, 1, 0.f, 3.5f );

	(gameState.lives > 3) ?
		Play::DrawSpriteRotated("red_heart", { 600, 50 }, 1, 0.f, 3.5f ) :
		Play::DrawSpriteRotated("white_heart", { 600, 50 }, 1, 0.f, 3.5f );

	(gameState.lives > 4) ?
		Play::DrawSpriteRotated("red_heart", { 650, 50 }, 1, 0.f, 3.5f ) :
		Play::DrawSpriteRotated("white_heart", { 650, 50 }, 1, 0.f, 3.5f );

	(gameState.lives > 5) ?
		Play::DrawSpriteRotated("red_heart", { 700, 50 }, 1, 0.f, 3.5f ) :
		Play::DrawSpriteRotated("white_heart", { 700, 50 }, 1, 0.f, 3.5f );

	(gameState.lives > 6) ?
		Play::DrawSpriteRotated("red_heart", { 750, 50 }, 1, 0.f, 3.5f ) :
		Play::DrawSpriteRotated("white_heart", { 750, 50 }, 1, 0.f, 3.5f );

	(gameState.lives > 7) ?
		Play::DrawSpriteRotated("red_heart", { 800, 50 }, 1, 0.f, 3.5f ) :
		Play::DrawSpriteRotated("white_heart", { 800, 50 }, 1, 0.f, 3.5f );

	(gameState.lives > 8) ?
		Play::DrawSpriteRotated("red_heart", { 850, 50 }, 1, 0.f, 3.5f ) :
		Play::DrawSpriteRotated("white_heart", { 850, 50 }, 1, 0.f, 3.5f );

	Play::SetDrawingSpace(Play::WORLD);
}

void DrawDebugInfo()
{
	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	GameObject& objEnemy = Play::GetGameObjectByType(TYPE_ENEMY);

	Play::SetDrawingSpace(Play::SCREEN);

	Play::DrawFontText("64px", "Cat state: " + std::to_string(gameState.playerState), { DISPLAY_WIDTH - 150, 50 }, Play::CENTRE);
	Play::DrawFontText("64px", "floor: " + std::to_string(gameState.floor), { DISPLAY_WIDTH - 150, 100 }, Play::CENTRE);
	Play::DrawFontText("64px", "cat px: " + std::to_string(objPlayer.pos.x), { DISPLAY_WIDTH - 150, 150 }, Play::CENTRE);
	Play::DrawFontText("64px", "cat rot: " + std::to_string(objPlayer.rotation), { DISPLAY_WIDTH - 150, 200 }, Play::CENTRE);
	Play::DrawFontText("64px", "G pos Y: " + std::to_string(groundedPosY), { DISPLAY_WIDTH - 150, 250 }, Play::CENTRE);

	//Play::DrawFontText("105px", "Score: " + std::to_string(gameState.score), { DISPLAY_WIDTH / 2 , 50 }, Play::CENTRE);

	Play::DrawFontText("64px", "treats: " + std::to_string(gameState.treats), { DISPLAY_WIDTH - 150, 300 }, Play::CENTRE);
	Play::DrawFontText("64px", "grounded " + std::to_string(flags.isGrounded ), { DISPLAY_WIDTH - 150, 350 }, Play::CENTRE);
	Play::DrawFontText("64px", "furballTimer" + std::to_string(furballTimer), { DISPLAY_WIDTH - 150, 400 }, Play::CENTRE);
	Play::DrawFontText("64px", "Flea vy : " + std::to_string(objEnemy.velocity.y), { 50, 150 }, Play::CENTRE);

	Play::SetDrawingSpace(Play::WORLD);
}

void DrawLevel()
{
	vector <int> vWalls = Play::CollectGameObjectIDsByType(TYPE_WALL);

	for (int wallId : vWalls)
	{
		GameObject& wall = Play::GetGameObject(wallId);
		Play::DrawObjectRotated(wall);
		//Play::DrawRect(Point2D{ wall.pos.x - WALL_AABB.x, wall.pos.y - WALL_AABB.y }, Point2D{ wall.pos.x + WALL_AABB.x, wall.pos.y + WALL_AABB.y }, Play::cWhite);
	}

	vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int platformId : vPlatforms)
	{
		GameObject& platform = Play::GetGameObject(platformId);
		Play::DrawObjectRotated(platform);
		//Play::DrawRect(Point2D{ platform.pos.x - PLATFORM_AABB.x, platform.pos.y - PLATFORM_AABB.y }, Point2D{ platform.pos.x + PLATFORM_AABB.x, platform.pos.y + PLATFORM_AABB.y }, Play::cWhite);
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
		//Play::DrawRect(Point2D{ ladder.pos.x - PLATFORM_AABB.x, ladder.pos.y - PLATFORM_AABB.y }, Point2D{ ladder.pos.x + PLATFORM_AABB.x, ladder.pos.y + PLATFORM_AABB.y }, Play::cWhite);
	}

	vector <int> vGrounds = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int groundId : vGrounds)
	{
		GameObject& ground = Play::GetGameObject(groundId);
		Play::DrawObjectRotated(ground);
		//Play::DrawRect(Point2D{ ground.pos.x - GROUND_PLATFORM_AABB.x, ground.pos.y - GROUND_PLATFORM_AABB.y }, Point2D{ ground.pos.x + GROUND_PLATFORM_AABB.x, ground.pos.y + GROUND_PLATFORM_AABB.y }, Play::cWhite);
	}

	vector <int> vWater = Play::CollectGameObjectIDsByType(TYPE_WATER);

	for (int waterId : vWater)
	{
		GameObject& water = Play::GetGameObject(waterId);
		Play::DrawObjectRotated(water);
	}

	vector <int> vFleas = Play::CollectGameObjectIDsByType(TYPE_FLEA);
	for (int fleaId : vFleas)
	{
		GameObject& flea = Play::GetGameObject(fleaId);
		Play::DrawObjectRotated(flea);
	}

	GameObject& objEnemy = Play::GetGameObjectByType(TYPE_ENEMY);
	Play::DrawObjectRotated(objEnemy);
	Play::DrawRect(Point2D{ objEnemy.pos.x - ENEMY_AABB.x, objEnemy.pos.y - ENEMY_AABB.y }, Point2D{ objEnemy.pos.x + ENEMY_AABB.x, objEnemy.pos.y + ENEMY_AABB.y }, Play::cWhite);

	vector <int> vBoxes = Play::CollectGameObjectIDsByType(TYPE_BOX);
	for (int boxId : vBoxes)
	{
		GameObject& objBox = Play::GetGameObject(boxId);
		Play::DrawObjectRotated(objBox);
	}

	vector <int> vFlowers = Play::CollectGameObjectIDsByType(TYPE_FLOWER);	
	for (int flowerId : vFlowers)
	{
		GameObject& objFlower = Play::GetGameObject(flowerId);
		Play::DrawObjectRotated(objFlower);
	}

	vector <int> vPoos = Play::CollectGameObjectIDsByType(TYPE_POO);
	for (int pooId : vPoos)
	{
		GameObject& objPoo = Play::GetGameObject(pooId);
		Play::DrawObjectRotated(objPoo);
	}

	vector <int> vRFlies = Play::CollectGameObjectIDsByType(TYPE_RIGHT_FLY);
	for (int flyId : vRFlies)
	{
		GameObject& objFly = Play::GetGameObject(flyId);
		Play::DrawObjectRotated(objFly);
		Play::DrawRect(Point2D{ objFly.pos.x - FLY_AABB.x, objFly.pos.y - FLY_AABB.y }, Point2D{ objFly.pos.x + FLY_AABB.x, objFly.pos.y + FLY_AABB.y }, Play::cWhite);
	}

	vector <int> vLFlies = Play::CollectGameObjectIDsByType(TYPE_LEFT_FLY);
	for (int flyId : vLFlies)
	{
		GameObject& objFly = Play::GetGameObject(flyId);
		Play::DrawObjectRotated(objFly);
		Play::DrawRect(Point2D{ objFly.pos.x - FLY_AABB.x, objFly.pos.y - FLY_AABB.y }, Point2D{ objFly.pos.x + FLY_AABB.x, objFly.pos.y + FLY_AABB.y }, Play::cWhite);
	}

	GameObject& objPlayer = Play::GetGameObjectByType(TYPE_PLAYER);
	Play::DrawObjectRotated(objPlayer);
	Play::DrawRect(Point2D{ objPlayer.pos.x - PLAYER_AABB_BOTTOM.x, objPlayer.pos.y - PLAYER_AABB_BOTTOM.y }, Point2D{ objPlayer.pos.x + PLAYER_AABB_BOTTOM.x, objPlayer.pos.y + PLAYER_AABB_BOTTOM.y }, Play::cRed);
	//Play::DrawRect(Point2D{ objPlayer.pos.x, objPlayer.pos.y - PLAYER_AABB_UPPER.y }, Point2D{ objPlayer.pos.x + PLAYER_AABB_UPPER.x, objPlayer.pos.y + PLAYER_AABB_UPPER.y }, Play::cWhite);
	
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









