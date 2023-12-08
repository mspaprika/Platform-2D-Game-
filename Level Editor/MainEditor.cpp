////////////////////////////////////////////////////////////////////////////////////
// A simple racing game editor for Brake Pass using the PlayBuffer framework.
// Copyright 2020 Sumo Digital Limited
///////////////////////////////////////////////////////////////////////////////////
//
// Note that this editor loads all its sprites from the Brake Pass project 
// directory and modifies the data in Brake Pass\Level.lev to change the level.
// Debugging->Working Directory should be to be set to ..\Brake Pass
//
///////////////////////////////////////////////////////////////////////////////////
#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

// "Magic numbers" shouldn't appear randomly in your code!
// Give them a meaningful name and put them at the top of the function/file for easy access
constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;

constexpr int SNAP_PIXELS = 32;
constexpr int CAMERA_SPEED = SNAP_PIXELS * 2;

const Point2f HALF_DISPLAY{ DISPLAY_WIDTH / 2.0f, DISPLAY_HEIGHT / 2.0f };

constexpr const char* SPR_PLAYER = "cat_sits_right";
constexpr const char* SPR_BOX = "le_2Dbox";
constexpr const char* SPR_FLEA = "bug_right";
constexpr const char* SPR_WATER = "water_long";
constexpr const char* SPR_GROUND = "the_ground";
constexpr const char* SPR_HEART = "heart_choco";
constexpr const char* SPR_C = "c_choco";
constexpr const char* SPR_NUTS = "choco_nuts";
constexpr const char* SPR_WAFFLE = "waff";
constexpr const char* SPR_LEFT_WALL = "left_side_wall";
constexpr const char* SPR_RIGHT_WALL = "right_side_wall";
constexpr const char* SPR_TREE = "new_tree";
constexpr const char* SPR_BUSH= "bush";
constexpr const char* SPR_CROISSANT = "croissant";
constexpr const char* SPR_APPLE = "apple";
constexpr const char* SPR_BOX_2 = "wood_box";
constexpr const char* SPR_FLW_YELLOW = "yellow_flower";
constexpr const char* SPR_FLW_VIOLET = "violet_flower";
constexpr const char* SPR_FLW_WHITE = "white_flower";
constexpr const char* SPR_FLW_PINK = "pink_flower";

constexpr const float PLAYER_SCALE = 4.5f;
constexpr const float FLEA_SCALE = 0.5f;
constexpr const float BOX_SCALE = 3.f;

constexpr const float SCALE = 2.5f;
constexpr const float WATER_SCALE = 7.0f;
constexpr const float TREE_SCALE = 1.5f;
constexpr const float APPLE_SCALE = 2.f;

constexpr const char* SPR_BACKGROUND = "Data//Backgrounds//forest1.png";


// To add a new object type:
// 1) Add the new enumeration to GameObjectType 
// 2) Create new strings above for each sprite required for that object 
// 2) Add a new row of sprite names to SPRITE_NAMES. Each entry corresponds to the keyboard toggle [1-9]
// 3) Add a new string to EDIT_MODES
// 4) Add a new line at the start of DrawScene to draw your new type 
// 5) Add a new condition to LoadLevel which creates the appropriate object type 
enum GameObjectType
{
	TYPE_NOONE = -1,

	TYPE_PLAYER,
	TYPE_FLEA,

	TYPE_PLATFORM,
	TYPE_GROUND,
	TYPE_LADDER,
	TYPE_WATER,
	TYPE_TREAT,
	TYPE_FLOWER,
	TYPE_TREE,
	TYPE_BUSH,
	TYPE_BOX,
	TYPE_WALL,

	TOTAL_TYPES
};

// Each line represents all the different sprites available for each type of GameObject : there can be a maximum of 9 for each type
const char* SPRITE_NAMES[TOTAL_TYPES][9] =
{
	{ SPR_PLAYER },
	{ SPR_FLEA },
	{ SPR_C, SPR_HEART, SPR_NUTS, SPR_BOX_2 },
	{ SPR_GROUND },
	{ SPR_WAFFLE },
	{ SPR_WATER },
	{ SPR_CROISSANT, SPR_APPLE },
	{ SPR_FLW_YELLOW, SPR_FLW_VIOLET, SPR_FLW_WHITE, SPR_FLW_PINK },
	{ SPR_TREE },
	{ SPR_BUSH },
	{ SPR_BOX },
	{ SPR_LEFT_WALL, SPR_RIGHT_WALL },
};

// Each string represents BOTH the text displayed when editing a particular GameObject type AND the name of that type in the level file, so changing existing strings will break your existing level files!
const char* EDIT_MODES[ TOTAL_TYPES ] =
{
	"PLAYER", "FLEA", "PLATFORM", "GROUND", "LADDER", "WATER", "TREAT", "FLOWER", "TREE", "BUSH", "BOX", "WALL",
};

// The global state structure
struct EditorState
{
	int score = 0;
	GameObjectType editMode = TYPE_PLAYER;
	Point2f cameraTarget{ 0.0f, 0.0f };
	float zoom = 1.0f;
	int selectedObj = -1;
	Point2f selectedOffset{ 0.0f, 0.0f };
	int saveCooldown = 0;
	int rotation = 0;
	int spriteId = -1;
};

EditorState editorState;


//-------------------------------------------------------------------------
// Function declarations

void HandleControls();
void DrawScene();
void DrawGrid( int gridSize );
void DrawUserInterface();
bool PointInsideSpriteBounds( Point2f testPos, GameObject& obj );
void DrawObjectsOfType( GameObjectType type, float SCALE );
void SaveLevel();
void LoadLevel();

//-------------------------------------------------------------------------
// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground( SPR_BACKGROUND );
	editorState.cameraTarget = HALF_DISPLAY;
	//Play::ColourSprite( "64px", Play::cBlack );
	LoadLevel();
}

//-------------------------------------------------------------------------
// Called by the PlayBuffer once for each frame of the game (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	static float fTotalGameTime = 0.f;
	fTotalGameTime += elapsedTime;

	HandleControls();
	DrawScene();
	DrawUserInterface();

	Play::PresentDrawingBuffer();
	return Play::KeyDown( VK_ESCAPE );
}

//-------------------------------------------------------------------------
// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

//-------------------------------------------------------------------------
// Camera movement, object placement / deletion etc.
void HandleControls( void )
{
	static bool drag = false;
	static Point2f dragOrigin{ 0.0f, 0.0f };

	if( Play::KeyPressed( 'S' ) && editorState.saveCooldown < 0 )
		SaveLevel();

	// Movement
	//-----------------------------
	if( Play::KeyPressed( VK_RIGHT ) )
		editorState.cameraTarget.x += CAMERA_SPEED / editorState.zoom;

	if( Play::KeyPressed( VK_LEFT ) )
		editorState.cameraTarget.x -= CAMERA_SPEED / editorState.zoom;

	if( Play::KeyPressed( VK_UP ) )
		editorState.cameraTarget.y -= CAMERA_SPEED / editorState.zoom;

	if( Play::KeyPressed( VK_DOWN ) )
		editorState.cameraTarget.y += CAMERA_SPEED / editorState.zoom;

	// Rotation
	//-----------------------------

	if( Play::KeyPressed( VK_OEM_PERIOD ) ) // Also the > key
		editorState.rotation++;

	if( Play::KeyPressed( VK_OEM_COMMA ) ) // Also the < key
		editorState.rotation--;

	if( editorState.rotation == 4 )
		editorState.rotation = 0;

	if( editorState.rotation == -1 )
		editorState.rotation = 3;

	// Mode switching
	//-----------------------------

	if( Play::KeyPressed( VK_SPACE ) )
	{
		editorState.editMode = (GameObjectType)(editorState.editMode+1);
		
		if( editorState.editMode == TOTAL_TYPES )
			editorState.editMode = TYPE_PLAYER;

		editorState.selectedObj = -1;
		editorState.spriteId = -1;
	}

	// Mouse interaction
	//-----------------------------

	Point2f mouseWorldPos = ( Play::GetMousePos() + Play::GetCameraPosition() ) / editorState.zoom;
	Point2f mouseWorldSnapPos = mouseWorldPos;
	mouseWorldSnapPos.x -= (int)mouseWorldSnapPos.x % SNAP_PIXELS;
	mouseWorldSnapPos.y -= (int)mouseWorldSnapPos.y % SNAP_PIXELS;

	// LMB selects/creates things
	if( Play::GetMouseButton( Play::LEFT ) )
	{
		// If we've not currently got an object selected then we check if we've clicked on one
		if( editorState.selectedObj == -1 )
		{
			for( int id : Play::CollectGameObjectIDsByType( editorState.editMode ) )
			{
				GameObject& obj = Play::GetGameObject( id );
				if( PointInsideSpriteBounds( mouseWorldPos, obj ) )
				{
					// Take the properties of the selected object
					editorState.selectedObj = obj.GetId();
					editorState.selectedOffset = obj.pos - mouseWorldSnapPos;
					editorState.spriteId = obj.spriteId;

					// Use rotated versions of the sprite (A-D.png) if they exist
					// Faster than rotating in code!
					std::string sSprite = Play::GetSpriteName( obj.spriteId );

					if( sSprite[sSprite.length() - 2] == '-' )
					{
						switch( sSprite[sSprite.length() - 1] )
						{
							case 'A':  editorState.rotation = 0;  break;
							case 'B':  editorState.rotation = 1;  break;
							case 'C':  editorState.rotation = 2;  break;
							case 'D':  editorState.rotation = 3;  break;
						}
					}
				}
			}

			// If we've not clicked on an existing object then we create a new one
			if( editorState.selectedObj == -1 )
			{
				switch( editorState.editMode )
				{
					case TYPE_PLAYER:
						if( Play::CollectGameObjectIDsByType( TYPE_PLAYER ).size() == 0 )
							Play::CreateGameObject( TYPE_PLAYER, mouseWorldSnapPos, 50, SPRITE_NAMES[static_cast<int>( TYPE_PLAYER )][0] );
						else
							Play::GetGameObjectByType( TYPE_PLAYER ).pos = mouseWorldPos;
						break;
					default:

						editorState.selectedObj = Play::CreateGameObject( editorState.editMode, mouseWorldSnapPos, 50, SPRITE_NAMES[static_cast<int>( editorState.editMode )][0] );
						
						// Use the last sprite selected (reset by changing modes)
						if( editorState.spriteId != -1 )
							Play::GetGameObject( editorState.selectedObj ).spriteId = editorState.spriteId;
						
						editorState.selectedOffset = { 0.0f, 0.0f };
;						
						break;
				}
			}

		}
		else // If we have got an object currently selected
		{
			GameObject& obj = Play::GetGameObject( editorState.selectedObj );
			obj.pos = mouseWorldSnapPos + editorState.selectedOffset;

			int index = -1;

			// Change the sprite of the currently selected object
			if( Play::KeyDown( '1' ) ) { index = 0; };
			if( Play::KeyDown( '2' ) ) { index = 1; };
			if( Play::KeyDown( '3' ) ) { index = 2; };
			if( Play::KeyDown( '4' ) ) { index = 3; };
			if( Play::KeyDown( '5' ) ) { index = 4; };
			if( Play::KeyDown( '6' ) ) { index = 5; };
			if( Play::KeyDown( '7' ) ) { index = 6; };
			if( Play::KeyDown( '8' ) ) { index = 7; };
			if( Play::KeyDown( '9' ) ) { index = 8; };

			if( index != -1 )
			{
				// Don't allow selection of missing sprites
				while( SPRITE_NAMES[ static_cast<int>(editorState.editMode) ][ index ] == nullptr ) { index--; }

				obj.spriteId = Play::GetSpriteId( SPRITE_NAMES[static_cast<int>( editorState.editMode )][index] );
				editorState.spriteId = obj.spriteId;
			}

			// Use rotated versions of the sprite (A-D.png) if they exist
			std::string sSprite = Play::GetSpriteName( obj.spriteId );

			if( sSprite[sSprite.length() - 2] == '-' )
			{
				sSprite.erase( sSprite.length() - 1, 1 );

				switch( editorState.rotation )
				{
					case 0: sSprite += "A";  break;
					case 1: sSprite += "B";  break;
					case 2: sSprite += "C";  break;
					case 3: sSprite += "D";  break;
				}

				int rotSpriteId = Play::GetSpriteId( sSprite.c_str() );

				if( rotSpriteId != -1 )
					obj.spriteId = rotSpriteId;
			}

		}
	}
	else
	{
		editorState.selectedObj = -1;
	}

	// RMB deletes things
	if( Play::GetMouseButton( Play::RIGHT ) )
	{
		for( int id : Play::CollectGameObjectIDsByType( editorState.editMode ) )
		{
			GameObject& obj = Play::GetGameObject( id );
			if( PointInsideSpriteBounds( mouseWorldPos, obj ) )
			{
				if( obj.type != TYPE_PLAYER )
					Play::DestroyGameObject( id );
			}
		}
	}

	Play::SetCameraPosition( ( editorState.cameraTarget * editorState.zoom ) - HALF_DISPLAY );
}


//-------------------------------------------------------------------------
// Draw all the things in the world in the right order
void DrawScene( void )
{
	Play::ClearDrawingBuffer( Play::cBlack );
	Play::DrawBackground();

	DrawObjectsOfType( TYPE_PLAYER, PLAYER_SCALE );
	DrawObjectsOfType( TYPE_FLEA, FLEA_SCALE );
	DrawObjectsOfType( TYPE_PLATFORM, SCALE );
	DrawObjectsOfType( TYPE_GROUND, SCALE );
	DrawObjectsOfType( TYPE_LADDER, SCALE);
	DrawObjectsOfType( TYPE_WATER, WATER_SCALE );
	DrawObjectsOfType( TYPE_TREAT, APPLE_SCALE);
	DrawObjectsOfType( TYPE_FLOWER, SCALE);
	DrawObjectsOfType( TYPE_TREE, TREE_SCALE );
	DrawObjectsOfType( TYPE_BUSH, SCALE);
	DrawObjectsOfType( TYPE_BOX, BOX_SCALE);
	DrawObjectsOfType( TYPE_WALL, SCALE );

	if( editorState.selectedObj != -1 )
	{
		GameObject& obj = Play::GetGameObject( editorState.selectedObj );
		Point2f origin = Play::GetSpriteOrigin( obj.spriteId );
		Point2f size = { Play::GetSpriteWidth( obj.spriteId ), Play::GetSpriteHeight( obj.spriteId ) };

		// Corners of sprite drawing area
		Point2D p0 = -origin;
		Point2D p2 = { size.width - origin.x, size.height - origin.y };
		Point2D p1 = { p2.x, p0.y };
		Point2D p3 = { p0.x, p2.y };

		Matrix2D mTrans = MatrixRotation( obj.rotation ) * MatrixScale( obj.scale, obj.scale );

		p0 = (mTrans.Transform( p0 ) + obj.pos) * editorState.zoom;
		p1 = (mTrans.Transform( p1 ) + obj.pos) * editorState.zoom;
		p2 = (mTrans.Transform( p2 ) + obj.pos) * editorState.zoom;
		p3 = (mTrans.Transform( p3 ) + obj.pos) * editorState.zoom;

		DrawLine( p0, p1, Play::cWhite );
		DrawLine( p1, p2, Play::cWhite );
		DrawLine( p2, p3, Play::cWhite );
		DrawLine( p3, p0, Play::cWhite );

		std::string s = "X:" + std::to_string( (int)( obj.pos.x + 0.5f ) ) + " / Y:" + std::to_string( (int)( obj.pos.y + 0.5f ) );
		Play::DrawDebugText( ( obj.pos - origin + Point2f( size.x / 2.0f, -10.0f / editorState.zoom ) ) * editorState.zoom, s.c_str(), Play::cWhite );
	}
}

//-------------------------------------------------------------------------
// Draw all the UI elements in the right order
void DrawUserInterface( void )
{
	Play::SetDrawingSpace( Play::SCREEN );
	std::string sMode;

	sMode = EDIT_MODES[ editorState.editMode ];

	Play::DrawRect( { 0, 0 }, { DISPLAY_WIDTH, 50 }, Play::cYellow, true );
	Play::DrawFontText( "64px", "MODE : " + sMode, { DISPLAY_WIDTH / 2, 25 }, Play::CENTRE );
	Play::DrawFontText( "64px", std::to_string( (int)( ( editorState.zoom * 100.0f ) + 0.5f ) ) + "%", { DISPLAY_WIDTH / 6, 25 }, Play::CENTRE );
	Play::DrawFontText( "64px", std::to_string( Play::CollectGameObjectIDsByType( editorState.editMode ).size() ) + " " + sMode, { ( DISPLAY_WIDTH * 5 ) / 6, 25 }, Play::CENTRE );

	Play::DrawDebugText( { 20, DISPLAY_HEIGHT - 20 }, "HOLD 'H' FOR CONTROLS", Play::cBlack, false );

	if( Play::KeyDown( 'H' ) )
	{
		Play::DrawRect( { DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 4 }, { DISPLAY_WIDTH * 3 / 4, DISPLAY_HEIGHT * 3 / 4 }, Play::cBlack, true );
		Play::DrawRect( { DISPLAY_WIDTH / 4, DISPLAY_HEIGHT / 4 }, { DISPLAY_WIDTH * 3 / 4, DISPLAY_HEIGHT * 3 / 4 }, Play::cMagenta, false );
		int y = ( DISPLAY_HEIGHT / 4 ) + 50;
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y }, "LEVEL EDITOR HELP", Play::cMagenta );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 20 }, "---------------------", Play::cWhite );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 40 }, "NOTE: YOU CAN ONLY INTERACT WITH OBJECTS SPECIFIED BY THE MODE", Play::cWhite );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 40 }, "SPACE BAR = CHANGE OBJECT MODE", Play::cMagenta );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 20 }, "LEFT MOUSE BUTTON = ADD OR SELECT OBJECT", Play::cWhite );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 20 }, "LEFT MOUSE DRAG = MOVE OBJECT", Play::cMagenta );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 20 }, "LEFT MOUSE DRAG AND KEYS 1-9 = CHANGE OBJECT SPRITE", Play::cWhite );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 20 }, "< and > = ROTATE OBJECT", Play::cMagenta );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 20 }, "RIGHT MOUSE BUTTON = DELETE OBJECT", Play::cWhite );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 20 }, "ARROW KEYS = SCROLL", Play::cMagenta );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 20 }, "F1 = SHOW DEBUG INFO", Play::cWhite );
		Play::DrawDebugText( { DISPLAY_WIDTH / 2, y += 20 }, "S = SAVE LEVEL", Play::cMagenta );
	}

	if( --editorState.saveCooldown > 0 )
	{
		Play::DrawRect( { 0, DISPLAY_HEIGHT - 50 }, { DISPLAY_WIDTH, DISPLAY_HEIGHT }, Play::cOrange, true );
		Play::DrawFontText( "64px", "OVERWRITING LEVEL", { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 25 }, Play::CENTRE );
	}

	Play::SetDrawingSpace( Play::WORLD );
}


void DrawObjectsOfType( GameObjectType type, float SCALE )
{
	for( int id : Play::CollectGameObjectIDsByType( type ) )
	{
		GameObject& obj = Play::GetGameObject( id );
		if( obj.spriteId != -1 )
			Play::DrawSpriteRotated( obj.spriteId, obj.pos * editorState.zoom, 0, obj.rotation, 1.0f * editorState.zoom * SCALE );
	}
}

//-------------------------------------------------------------------------
// Test if we have clicked on a sprite
bool PointInsideSpriteBounds( Point2f testPos, GameObject& obj )
{
	Point2f origin = Play::GetSpriteOrigin( obj.spriteId );
	Point2f size = { Play::GetSpriteWidth( obj.spriteId ), Play::GetSpriteHeight( obj.spriteId ) };
	Point2f topLeft = obj.pos - origin;
	Point2f botRight = topLeft + size;
	return testPos.x > topLeft.x && testPos.x < botRight.x&& testPos.y > topLeft.y && testPos.y < botRight.y;
}

//-------------------------------------------------------------------------
// Loads the objects from the Brake Pass\Level.lev file
void LoadLevel( void )
{
	std::ifstream levelfile;
	levelfile.open( "Level.lev" );

	std::string sType, sX, sY, sSprite, sRot;

	std::getline( levelfile, sType );

	while( !levelfile.eof() )
	{
		std::getline( levelfile, sType );
		std::getline( levelfile, sX );
		std::getline( levelfile, sY );
		std::getline( levelfile, sSprite );

		int id = -1;

		// Each type is detected and loaded separately

		if( sType == "TYPE_PLAYER" )
			id = Play::CreateGameObject( TYPE_PLAYER, { std::stof( sX ), std::stof( sY ) }, 50, sSprite.c_str() );

		if( sType == "TYPE_FLEA" )
			id = Play::CreateGameObject( TYPE_FLEA, { std::stof( sX ), std::stof( sY ) }, 50, sSprite.c_str() );

		if( sType == "TYPE_PLATFORM" )
			id = Play::CreateGameObject( TYPE_PLATFORM, { std::stof( sX ), std::stof( sY ) }, 50, sSprite.c_str() );

		if( sType == "TYPE_GROUND" )
			id = Play::CreateGameObject( TYPE_GROUND, { std::stof( sX ), std::stof( sY ) }, 50, sSprite.c_str() );

		if( sType == "TYPE_LADDER" )
			id = Play::CreateGameObject( TYPE_LADDER, { std::stof( sX ), std::stof( sY ) }, 50, sSprite.c_str() );

		if( sType == "TYPE_WATER" ) 
			id = Play::CreateGameObject( TYPE_WATER, { std::stof( sX ), std::stof( sY ) }, 50, sSprite.c_str() );

		if( sType == "TYPE_TREAT" )
			id = Play::CreateGameObject( TYPE_TREAT, { std::stof( sX ), std::stof( sY ) }, 50, sSprite.c_str() );

		if( sType == "TYPE_FLOWER" )
			id = Play::CreateGameObject( TYPE_FLOWER, { std::stof( sX ), std::stof( sY ) }, 50, sSprite.c_str() );

		if( sType == "TYPE_TREE" )
			id = Play::CreateGameObject( TYPE_TREE, { std::stof( sX ), std::stof( sY ) }, 50, sSprite.c_str() );

	}

	levelfile.close();
}

//-------------------------------------------------------------------------
// Outputs the objects to the Brake Pass\Level.lev file
void SaveLevel( void )
{
	std::ofstream levelfile;
	levelfile.open( "Level.lev" );

	levelfile << "// This file is auto-generated by the Level Editor - it's not advisable to edit it directly as changes may be overwritten!\n";

	for( int id : Play::CollectAllGameObjectIDs() )
	{
		GameObject& obj = Play::GetGameObject( id );
		 
		// Saving is based on the strings in EDIT_MODES

		levelfile << "TYPE_" << EDIT_MODES[ obj.type ] << "\n";
		levelfile << std::to_string( obj.pos.x ) + "f\n" << std::to_string( obj.pos.y ) + "f\n";
		levelfile << Play::GetSpriteName( obj.spriteId ) << "\n";
	}
	
	levelfile.close();

	editorState.saveCooldown = 100;
}