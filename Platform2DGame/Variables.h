#pragma once


constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;

const float DELTA_TIME{ 0.016f }; // seconds

const int COLLISION_BOOST{ 25 }; // pixels

constexpr const char* CAR_E_SPR_NAME = "spr_car_magenta";
constexpr const char* BACKGROUND_NAME = "Data\\Backgrounds\\spr_background.png";
constexpr const char* AUDIO_NAME = "music";
constexpr const char* FONT_NAME = "64px";


constexpr const float PLAYER_SCALE = 4.5f;
constexpr const float FLEA_SCALE = 0.5f;
constexpr const float BOX_SCALE = 3.f;

constexpr const float SCALE = 2.5f;
constexpr const float WATER_SCALE = 7.0f;
constexpr const float TREE_SCALE = 1.5f;
constexpr const float APPLE_SCALE = 2.f;

const float BOTTOM_LINE{ 575.f }; // pixels

const int FLOWER_SCORE{ 100 };
const int TREAT_SCORE{ 50 };

int rightLimit{ 0 };
int leftLimit{ 0 };

const int WALL_LIMIT_OFFSET{ 90 }; // pixels

// offsets
const int CAMERA_OFFSET_X{ 600 }; // pixels
const int PLAYER_OFFSET_Y{ 8 }; // pixels
const int FLEA_OFFSET_Y{ 18 }; // pixels
const int FLEA_OFFSET_Y_GROUND{ 60 }; // pixels
const int BOX_OFFSET_Y{ 20 }; // pixels
const int WALL_OFFSET{ 20 }; // pixels
const int FLOOR_OFFSET{ 18 }; // pixels
const int LADDER_OFFSET_Y{ 30 }; // pixels
const int LADDER_OFFSET_X_STICK{ 16 }; // pixels
const int LADDER_OFFSET_X_SLIDE{ 17 }; // pixels
const int TILE_SIZE{ 38 }; // pixels
const int POO_OFFSET_Y{ 30 }; // pixels
const int WATER_OFFSET_Y{ -17 }; // pixels
const int TREE_OFFSET_Y{ 10 }; // pixels
const int BUSH_OFFSET_Y{ 5 }; // pixels

// other measurments are in tiles
// width and height in tiles, rounded to the bigger side so all screen is covered
const int TILE_HEIGHT{ 19 };
const int TILE_WIDTH{ 34 };

const Point2D CAMERA_THRESHOLD_X{ -15, 42 };
const int CAMERA_THRESHOLD_Y{ 5 };

// floors heights
const int GROUND_FLOOR_HEIGHT{ 18 };
const int SECOND_FLOOR_HEIGHT{ 14 };
const int THIRD_FLOOR_HEIGHT{ 9 };
const int FOURTH_FLOOR_HEIGHT{ 4 };
const int FIFTH_FLOOR_HEIGHT{ 2 };
const int SIXTH_FLOOR_HEIGHT{ -5 };

// player pos in all floors
const Point2D PLAYER_START_POS{ -5, GROUND_FLOOR_HEIGHT - 2 };
const int PLAYER_POS_GROUND_FLOOR{ GROUND_FLOOR_HEIGHT - 2 };
const int PLAYER_POS_SECOND_FLOOR{ SECOND_FLOOR_HEIGHT - 3 };
const int PLAYER_POS_THIRD_FLOOR{ THIRD_FLOOR_HEIGHT - 3 };
const int PLAYER_POS_FOURTH_FLOOR{ FOURTH_FLOOR_HEIGHT - 3 };
const int PLAYER_POS_FIFTH_FLOOR{ FIFTH_FLOOR_HEIGHT - 3 };
const int PLAYER_POS_SIXTH_FLOOR{ SIXTH_FLOOR_HEIGHT - 3 };

const Point2D ENEMY_START_POS{ -8, SECOND_FLOOR_HEIGHT - 3 };
const Point2D FLEA_START_POS{ 5, THIRD_FLOOR_HEIGHT - 3 };
const Point2D RIGHT_FLY_START_POS{ -35 , THIRD_FLOOR_HEIGHT };
const Point2D LEFT_FLY_START_POS{ 65 , THIRD_FLOOR_HEIGHT };

const int FLY_SPEED{ 2 };

// player velocity
const float PLAYER_VELOCITY_Y{ -15.f };
const Vector2D PLAYER_VELOCITY_DEFAULT{ 0.f, 0.f };
const Vector2D PLAYER_VELOCITY_WALK{ 10.f, 0.f };
const Vector2D PLAYER_VELOCITY_JUMP{ 0.f, PLAYER_VELOCITY_Y };
const Vector2D PLAYER_VELOCITY_JUMP_LEFT{ -8.f, PLAYER_VELOCITY_Y };
const Vector2D PLAYER_VELOCITY_JUMP_RIGHT{ 8.f, PLAYER_VELOCITY_Y };
const Vector2D PLAYER_VELOCITY_FALL_RIGHT{ 1.f, 12.f };
const Vector2D PLAYER_VELOCITY_FALL_LEFT{ -1.f, 12.f };

const float FALL_MULTIPLIER{ 2.5f };
const float LOW_JUMP_MULTIPLIER{ 3.5f };

// fleas velocity
const Vector2D ENEMY_VELOCITY_DEFAULT{ 0, 0 };
const Vector2D ENEMY_VELOCITY_JUMP_RIGHT{ 5, -20 };
const Vector2D ENEMY_VELOCITY_JUMP_LEFT{ -5, -20 };
const Vector2D ENEMY_VELOCITY_WALK_RIGHT{ 3, 0 };
const Vector2D ENEMY_VELOCITY_WALK_LEFT{ -3, 0 };
const Vector2D ENEMY_VELOCITY_FALL_RIGHT{ 1, 5 };
const Vector2D ENEMY_VELOCITY_FALL_LEFT{ -1, 5 };

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

// gravity and acceleration
const Vector2D GRAVITY_ACCELERATION{ 0, 0.6f };
const Vector2D GRAVITY{ 0, 22.f };
const Vector2D FLEA_GRAVITY{ 0, 12.f };
const Vector2D ACCELERATION{ 0, 0.3f };
const Vector2D FRICTION{ 0.5f, 0.5f };

Point2D cameraPos{ 0, 0 };
Point2D platformPos{ 0, 0 };
Point2D platformPosFlea{ 0, 0 };
Point2D ladderPos{ 0, 0 };
Point2D waterPos{ 0, 0 };

Point2D oldCameraPos{ 0, 0 };

float distanceFromPlayer{ 0.f };
float gravityMultiplyer = 1.f;
int ladderOffsetX = LADDER_OFFSET_X_STICK;
int groundedPosY = 0;

const int LOBBY_FLEA_COUNT{ 7 };
