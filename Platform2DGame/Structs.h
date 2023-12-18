#pragma once

#include "Variables.h"
#include "Play.h"

using namespace std;

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
	STATE_IDLE,
	STATE_DEAD,
	STATE_REBORN,
};

enum GameObjectType
{
	TYPE_PLAYER = 0,
	TYPE_FLEA,
	TYPE_RIGHT_FLY,
	TYPE_LEFT_FLY,
	TYPE_PLATFORM,
	TYPE_GROUND,
	TYPE_LADDER,
	TYPE_WATER,
	TYPE_SPLASH,
	TYPE_TREAT,
	TYPE_FLOWER,
	TYPE_STAR,
	TYPE_POO,
	TYPE_TREE,
	TYPE_BUSH,
	TYPE_BOX,
	TYPE_WALL,
	TYPE_LOBBY_CAT,
	TYPE_LOBBY_FLEA,
	TYPE_DESTROYED,
	TYPE_CAR,
	TOTAL_TYPES
};

enum GameFlow
{
	STATE_PLAY = 0,
	STATE_LOBBY,
	STATE_PAUSE,
	STATE_GAMEOVER,
	STATE_LEVELCOMPLETE,
	STATE_VICTORY,
};


struct Platform
{
	const Vector2D WALL_OFFSET{ 20, 20 };
	const Vector2D GROUND_FLOOR_POS{ LIMIT_LEFT, GROUND_FLOOR_HEIGHT };
	int fleaQty{  };

	// player and enemies start / exit positions
	Point2D playerSpawnPos{  };
	Point2D playerRebirthPos{  };
	Point2D playerExitPos{  };

	vector <int> FLEA_SPAWN_POS_X{  };
	vector <int> FLEA_SPAWN_POS_Y{  };
	vector <int> FLEA_STATES{  };

	// position of the top tile top edge of the ladder
	const int LADDER_TOP_FLOOR_SECOND{ PLAYER_POS_SECOND_FLOOR + TILE_SIZE };
	const int LADDER_TOP_FLOOR_THIRD{ PLAYER_POS_THIRD_FLOOR + TILE_SIZE };
	const int LADDER_TOP_FLOOR_FOURTH{ PLAYER_POS_FOURTH_FLOOR + TILE_SIZE };
	const int LADDER_TOP_FLOOR_FIFTH{ PLAYER_POS_FIFTH_FLOOR + TILE_SIZE };
	const int LADDER_TOP_FLOOR_SIXTH{ PLAYER_POS_SIXTH_FLOOR + TILE_SIZE };

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

	vector <int> WATER_POS_X{  };
	vector <int> WATER_POS_Y{  };
	vector <int> WATER_WIDTH{  };

	Point2D waterPos{  };
	int waterLength{  };

	vector <int> TREE_POS_X{  };
	vector <int> TREE_POS_Y{  };

	vector <int> BUSH_POS_X{  };
	vector <int> BUSH_POS_Y{  };

	vector <int> SALMON_POS_X{  };
	vector <int> SALMON_POS_Y{  };

	vector <int> CHICKEN_POS_X{  };
	vector <int> CHICKEN_POS_Y{  };

	vector <int> FLOWER_POS_X{  };
	vector <int> FLOWER_POS_Y{  };

	vector <int> BOX_POS_X{  };
	vector <int> BOX_POS_Y{  };
};

struct Timers
{
	float walkTimer{ 0.f };
	float jumpTimer{ 0.f };
	float animationTimer{ 0.f };
	float coyoteTimer{ 0.f };
	float fleaTimer{ 0.f };
	float furballTimer{ 0.f };
	float hissTimer{ 0.f };
	float poopTimer{ 0.f };
	float idleTimer{ 0.f };
	float lookAroundTimer{ 0.f };
	float lickTimer{ 0.f };
	float hairballTimer{ 0.f };
	float rebornTimer{ 0.f };
	float splashTimer{ 0.f };
	float boxTimer{ 0.f };
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
	bool fleaRight{ true };

	bool jumping{ false };
	bool isAnimating{ false };
	bool isClimbing{ false };
	bool isGroundFloor{ false };
	bool collided{ false };
	bool isGrounded{ false };
	bool newDirection{ true };
	bool flyActive{ false };
	bool AnimationChanged{ false };

	bool fleaActivated{ false };
	bool splashed{ false };

	bool isBox{ false };
	bool isBoxHalf{ false };
	bool isBoxOpen{ true };
	bool isBoxExit{ false };

	bool catActivated{ false };
};

struct GameState
{
	int score{ 0 };
	int level{ 1 };
	int lives{ 9 };
	int highScore{ 0 };
	int floor{ 0 };
	int treats{ 0 };
	Point2D exitPos{  };

	PlayerState playerState = STATE_JUMP;
	PlayerState playerPrevState = STATE_GROUNDED;
	PlayerState enemyState = STATE_WALK;
	GameFlow state = STATE_LOBBY;
};