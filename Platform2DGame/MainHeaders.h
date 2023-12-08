#pragma once


void LoadLevel();
void CameraControl();
void WalkingDurationControl(float time);
void AnimationDurationControl(float time);
void CoyoteControl();
void SetFloor();
void SetPlayerPos(int pos);
void ResetPlayer();
void SpriteOrigins();

void Draw();
void DrawLevel();
void DrawGameStats();
void DrawDebugInfo();
void DrawLobby();
void DrawObjectsOfType(int TYPE);
void DrawRect(const GameObject& object, Vector2D AABB);
void UpdateLobby();

void CoordsPlatform1();
void CoordsPlatform2();
void CoordsPlatform3();
void CoordsPlatform4();
void CoordsPlatform5();

void CreateGamePlay();
void CreateLevel(Platform& platform);
void CreateLobby();
void CreateLobbyFlea();
void CreateObjects(Platform& platform);
void CreatePlatform(Platform& platform);
void BuildSideWalls(Platform& platform);
void CreateWalls(vector <int>& w, vector <int>& h, vector <int>& x, vector <int>& y, float scale, const char* s, int TYPE);
void CreateFloors(vector <int>& w, vector <int>& x, vector <int>& y, float scale, const char* s, int TYPE);
void ItemsPlacement(Platform& platform);
void PlacePlants(Platform& platform);

void Jump();
void Fall();
void Hiss();
void WalkingPlayerControls();
void IdlePlayerControls();
void AttachedPlayerControls();
void FurballPlayerControls();
void JumpControls();
void Reborn();

void UpdateGameStates();
void UpdatePlayer();
void UpdateFleas();
void UpdateTreats();
void UpdatePoo();
void UpdateFlies();
void UpdateFlyByType(int TYPE, Point2D pos, int limit);
void PoopControl();
void UpdateWater();
void UpdateStars();
void UpdateBox();

void WalkingFlea(GameObject& flea);
void FallingFlea(GameObject& flea);
void FleaJump(GameObject& flea);
void AttachedFlea(GameObject& flea);
void FleaAI(GameObject& flea);

void LookAroundControl();
void LickControl();
void HairballControl();

void DestroyObjectsOfType(int TYPE);
void UpdateDestroyed();
void LoopObject(GameObject& object);
float DistanceFromPlayer(const GameObject& object);
float DistanceYFromPlayer(const GameObject& object);

float DotProduct(const Vector2D& v1, const Vector2D& v2);
void Normalise(Vector2D& v);
float Randomize(int range, float multiplier = 1.0f);

bool IsPlayerColliding(const GameObject& object);
bool IsPlayerCollidingUpperPart(const GameObject& object);
bool IsPlayerCollidingBottomPart(const GameObject& object);
bool IsPlayerCollidingAnyPlatform();
bool IsPlayerCollidingAnyPlatformBoth();
bool IsPlayerCollidingAnyPlatformUpper();
bool IsPlayerCollidingLadder();
bool IsPlayerStillCollidingLadder();
bool IsPlayerOnLadder();
bool IsPlayerCollidingWalls();
bool IsPlayerCollidingGround();

bool IsFleaCollidingAnyPlatform(GameObject& flea);
bool IsFleaColliding(GameObject& flea, const GameObject& object);

void JumpCollisionControl();
void DestroyAllObjects();
void RestartGame();

void LoadLevel(void);