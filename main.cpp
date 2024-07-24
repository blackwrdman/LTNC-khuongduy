#include<SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <bits/stdc++.h>

using namespace std;

int score = 0, lvl = 1, prevScore;
int highscore = 0;
bool music = true;

//Screen dimension constants
const int SCREEN_WIDTH = 360;
const int SCREEN_HEIGHT = 640;
const int START = 520;
const int line1 = 24, line2 = 114, line3 = 208, line4 = 298;
const int maxLvl = 7;

//The dimensions of Cars
static const int CAR_WIDTH = 40;
static const int CAR_HEIGHT = 75;
//Maximum axis velocity of the car
static const int CARS_VEL = 4;
static const int degreeVel = 4;

//The dimensions of the OBSTACLE
static const int OBSTACLE_WIDTH = 40;
static const int OBSTACLE_HEIGHT = 40;

//Maximum axis velocity of the OBSTACLE
static const int OBSTACLE_VEL = 5;
static const int OBJ_START = -100;
static const int OBJ_DISTANCE = 75;
static const int reduceObjDistance = 5;
static const int OBSTACLE_VEL_INC = 1;

//Texture wrapper class
class Texture
{
    public:
        //Initializes variables
        Texture();

        //Deallocates memory
        ~Texture();

        //Loads image at specified path
        bool loadFromFile( std::string path );

        //Deallocates texture
        void free();

        //Set color modulation
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        //Renders texture at given point
        void render( int x, int y, int w, int h, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

        //Gets image dimensions
        int getWidth();
        int getHeight();

        //The actual hardware texture
        void update_sprite();
        SDL_Texture* mTexture;
        SDL_Rect sprite;
        int x;
        int y;
        int W;
        int H;

        //Image dimensions
        int mWidth;
        int mHeight;
};

//The mouse button
class LButton : public Texture
{
	public:
		//Initializes internal variables
		LButton();

		//Sets top left position and size
		void setPosition( int x, int y, int w, int h);

		//Handles mouse event
		bool handleEvent( SDL_Event* e );

	private:
		//Top left position
		SDL_Point mPosition;
};

class Text: public Texture
{
public:

    //Creates image from font string
    void loadFromRenderedText( std::string textureText, SDL_Color textColor );

    //input text and size
    void loadText(string text, int size);
};

class Score: public Texture
{
public:

    //Creates image from font string
    void loadFromRenderedText( std::string textureText, SDL_Color textColor );

    //input text and size
    void loadText(string text, int size);
};

class BlueCar: public Texture
{
    public:

    //Initializes the variables
    BlueCar();

    //Handle input
    void handle_Event( SDL_Event& e );

    //Moves the BlueCar
    void turnL();

    void turnR();

    void move();

    //Shows the car on the screen
    void render();

    //The velocity of the OBSTACLE
    int bVel;
    int bdegree;
};

class RedCar: public Texture
{
    public:

    //Initializes the variables
    RedCar();

    //Handle input
    void handle_Event( SDL_Event& e );

    //Moves the RedCar
    void turnL();

    void turnR();

    void move();

    //Shows the car on the screen
    void render();

    //The velocity of the OBSTACLE
    int rVel;
    int rdegree;
};

class Obstacle : public Texture
{
    public:
    Obstacle();

    //create the obstacle
    void create(int line);

    //moving the obstacle
    void update_Pos();

    //show the obstacle
    void show();

    //Obstacle 's properties
    int obVel, obLine, obType;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadDot();

bool loadBackground();

bool loadBlueCar();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font *gFont = NULL;

//The music that will be played
Mix_Music *mOpen = NULL;

//The sound effects that will be used
Mix_Chunk *mPause = NULL;
Mix_Chunk *mGameOver1 = NULL;
Mix_Chunk *mGameOver2 = NULL;
Mix_Chunk *mGameOver3 = NULL;
Mix_Chunk *mGameOver4 = NULL;
Mix_Chunk *mClick = NULL;
Mix_Chunk *mHighScore = NULL;
Mix_Chunk *mScore = NULL;


//Scene textures
Texture gBackground;
LButton gPlay, gDarkBackground, gMusicOn, gMusicOff, gPause, gHighScore, gReplay, gHome;
Texture gBlueCar, gRedCar;
Texture bDot, bSquare, rDot, rSquare;
Text gText;
Score scoreText, highScoreText, gscoreText, ghighScoreText;

//class Texture
Texture::Texture()
{
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

Texture::~Texture()
{
    //Deallocate
    free();
}

bool Texture::loadFromFile( std::string path )
{
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Color key image
        SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

void Texture::free()
{
    //Free texture if it exists
    if( mTexture != NULL )
    {
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
    //Modulate texture rgb
    SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void Texture::render( int x, int y, int w, int h, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, w, h };

    //Render to screen
    SDL_RenderCopyEx( gRenderer, mTexture, NULL, &renderQuad, angle, center, flip );
}

int Texture::getWidth()
{
    return mWidth;
}

int Texture::getHeight()
{
    return mHeight;
}

void Texture::update_sprite()
{
    sprite.x = x;
    sprite.y = y;
    sprite.h = H;
    sprite.w = W;
}

//class Text
void Text::loadFromRenderedText( string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
}

void Text::loadText( string text, int size)
{
	//Open the font
	gFont = TTF_OpenFont( "ttf/gasalt.thin.ttf", size );
	if( gFont == NULL )
	{
		printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
		//Render text
		SDL_Color textColor = { 220, 220, 220 };
		loadFromRenderedText( text, textColor );
	}
}

//class Text
void Score::loadFromRenderedText( string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
}

void Score::loadText( string text, int size)
{
	//Open the font
	gFont = TTF_OpenFont( "ttf/alienleaguebold.ttf", size );
	if( gFont == NULL )
	{
		printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
		//Render text
		SDL_Color textColor = { 220, 220, 220 };
		loadFromRenderedText( text, textColor );
	}
}

// Class LButton
LButton::LButton()
{
	mPosition.x = 0;
	mPosition.y = 0;
}

void LButton::setPosition( int x, int y, int w, int h )
{
	mPosition.x = x;
	mPosition.y = y;
	W = w;
    H = h;
}

bool LButton::handleEvent( SDL_Event* e )
{
	//If mouse event happened
	if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState( &x, &y );
		//cout << x << " " << y << endl;

		//Check if mouse is in button
		bool inside = true;

		//Mouse is left of the button
		if( x < mPosition.x )
		{
			inside = false;
		}
		//Mouse is right of the button
		else if( x > mPosition.x + W )
		{
			inside = false;
		}
		//Mouse above the button
		else if( y < mPosition.y )
		{
			inside = false;
		}
		//Mouse below the button
		else if( y > mPosition.y + H )
		{
			inside = false;
		}

		//Mouse is outside button
		if( inside && e->type == SDL_MOUSEBUTTONDOWN)
			return true;
	}
	return false;
}

//class Obstacle
Obstacle::Obstacle()
{
    mTexture = NULL;
    W = OBSTACLE_WIDTH;
    H = OBSTACLE_HEIGHT;
}

void Obstacle::create(int preLine)
{
    if (y <= OBJ_START || y >= SCREEN_HEIGHT + obVel)
    {
        //Y position
        y = OBJ_START;

        //X position
        obLine = rand() % 2;
        //cout << x << " " << y << endl;
        if (preLine > 180 && preLine < 360) switch (obLine)
        {
        case 0:
            x = line1;
            break;
        case 1:
            x = line2;
            break;
        }

        if (preLine < 180 && preLine > 0) switch (obLine)
        {
        case 0:
            x = line3;
            break;
        case 1:
            x = line4;
        break;
        }
        //Obstacle 's type
        obType = rand() % 2;
    }
}

void Obstacle::update_Pos()
{
    // Obstacle 's new velocity
    obVel = OBSTACLE_VEL + lvl/2*OBSTACLE_VEL_INC;
    y += obVel;
    //sync
    if (x == line1 || x == line2 || x == line3 || x == line4)
        update_sprite();
}

//Show the OBSTACLE
void Obstacle::show()
{
    switch (x)
        {
        case line1:
            if (obType == 1) bDot.render( x, y, OBSTACLE_WIDTH, OBSTACLE_HEIGHT);
            else if (obType == 0) bSquare.render( x, y, OBSTACLE_WIDTH, OBSTACLE_HEIGHT);
            break;
        case line2:
            if (obType == 1) bDot.render( x, y, OBSTACLE_WIDTH, OBSTACLE_HEIGHT);
            else if (obType == 0) bSquare.render( x, y, OBSTACLE_WIDTH, OBSTACLE_HEIGHT);
            break;
        case line3:
            if (obType == 1) rDot.render( x, y, OBSTACLE_WIDTH, OBSTACLE_HEIGHT);
            else if (obType == 0) rSquare.render( x, y, OBSTACLE_WIDTH, OBSTACLE_HEIGHT);
            break;
        case line4:
            if (obType == 1) rDot.render( x, y, OBSTACLE_WIDTH, OBSTACLE_HEIGHT);
            else if (obType == 0) rSquare.render( x, y, OBSTACLE_WIDTH, OBSTACLE_HEIGHT);
            break;
        }
}

//class BlueCar
BlueCar::BlueCar()
{
    //Initialize the offsets
    x = line1;
    y = START;

    //Initialize the velocity
    bVel = 0;
    bdegree = 0;
    W = CAR_WIDTH;
    H = CAR_HEIGHT;
}

void BlueCar::handle_Event( SDL_Event& e )
{
    //If a key was pressed
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        if ( e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_a)
        {
            if (x == line1) turnL();
            else if (x == line2) turnR();
        }
    }
}

void BlueCar::turnL()
{
    bVel = - CARS_VEL;
}

void BlueCar::turnR()
{
    bVel = CARS_VEL;
}

void BlueCar::move()
{
     //If the dot is between 2 lines
    if( ( x >= line1 ) && ( x <= line2 ) )
    {
        //Move back
        x -= bVel;
        //std::cout << x << std::endl;
    }

    //If the dot is between 2 lines
    if( ( x > line1 ) && ( x < line2 ) )
        if (x > 68) bdegree -= degreeVel;
        else if (x <= 68) bdegree += degreeVel;
    //std::cout << x << " " << bdegree << std::endl;

    if (x > line2) x = line2;
    else if (x < line1) x = line1;
    update_sprite();
}

void BlueCar::render()
{
    //Show the BlueCar
    gBlueCar.render( x, y, CAR_WIDTH, CAR_HEIGHT, bdegree);

}

//class RedCar
RedCar::RedCar()
{
    //Initialize the offsets
    x = line4;
    y = START;

    //Initialize the velocity
    rVel = 0;
    rdegree = 0;
    W = CAR_WIDTH;
    H = CAR_HEIGHT;
}

void RedCar::handle_Event( SDL_Event& e )
{
    //If a key was pressed
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        if ( e.key.keysym.sym == SDLK_RIGHT || e.key.keysym.sym == SDLK_d)
        {
            if (x == line3) turnR();
            else if (x == line4) turnL();
        }
    }
}

void RedCar::turnL()
{
    rVel = CARS_VEL;
}

void RedCar::turnR()
{
    rVel = - CARS_VEL;
}

void RedCar::move()
{
     //If the dot is between 2 lines
    if( ( x >= line3 ) && ( x <= line4 ) )
    {
        //Move back
        x -= rVel;
        //std::cout << x << std::endl;
    }

    //If the dot is between 2 lines
    if( ( x > line3 ) && ( x < line4 ) )
        if (x > 252) rdegree -= degreeVel;
        else if (x <= 252) rdegree += degreeVel;
    //std::cout << x << " " << degreeVel << " " << rdegree << std::endl;

    if (x > line4) x = line4;
    else if (x < line3) x = line3;
    update_sprite();
}

void RedCar::render()
{
    //Show the BlueCar
    gRedCar.render( x, y, CAR_WIDTH, CAR_HEIGHT, rdegree);
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "2CARS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}

				//Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadAudio()
{
    bool success = true;

    mScore = Mix_LoadWAV( "sound/Score.wav" );
    if( mScore == NULL )
	{
		printf( "Failed to load score sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	mHighScore = Mix_LoadWAV( "sound/HighScore.mp3" );
    if( mHighScore == NULL )
	{
		printf( "Failed to load high score sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
    mGameOver1 = Mix_LoadWAV( "sound/GameOver_Punch.mp3" );
    if( mGameOver1 == NULL )
	{
		printf( "Failed to load game over 1 sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
    mGameOver2 = Mix_LoadWAV( "sound/GameOver_Nope.mp3" );
    if( mGameOver2 == NULL )
	{
		printf( "Failed to load GameOver2  sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
    mGameOver3 = Mix_LoadWAV( "sound/GameOver_Wasted.mp3" );
    if( mGameOver3 == NULL )
	{
		printf( "Failed to load game over 3 sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	mGameOver4 = Mix_LoadWAV( "sound/GameOver_OOF.mp3" );
    if( mGameOver4 == NULL )
	{
		printf( "Failed to load game over 4 sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
    mClick = Mix_LoadWAV("sound/Click.wav");
    if( mClick == NULL )
	{
		printf( "Failed to load click sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	mPause = Mix_LoadWAV("sound/Pause.mp3");
    return success;
}
bool loadText()
{

}
bool loadMedia()
{
    //Loading success flag
    bool success = true;

    if( !gPlay.loadFromFile( "graphic/play.png" )
       || ( !gHome.loadFromFile( "graphic/home.png" ))
       || !gPause.loadFromFile( "graphic/Pause_button.png" )
       || !gMusicOn.loadFromFile( "graphic/music.png" )
       || !gMusicOff.loadFromFile( "graphic/music_off.png" )
       || !gDarkBackground.loadFromFile( "graphic/black_background.png")
       || !gReplay.loadFromFile( "graphic/replaybutton.png" )
       || !gHighScore.loadFromFile( "graphic/trophy.png" ))
    {
        printf( "Failed to load Media!\n" );
        success = false;
    }

    return success;
}

bool loadBackground()
{
    //Loading success flag
    bool success = true;

    if( !gBackground.loadFromFile( "graphic/map.png" ))
    {
        printf( "Failed to load background texture!\n" );
        success = false;
    }

    return success;
}

bool loadBlueCar()
{
    //Loading success flag
    bool success = true;

    if( !gBlueCar.loadFromFile( "graphic/Car_blue.png" ))
    {
        printf( "Failed to load car texture!\n" );
        success = false;
    }

    return success;
}

bool loadRedCar()
{
    //Loading success flag
    bool success = true;

    if( !gRedCar.loadFromFile( "graphic/Car_red.png" ))
    {
        printf( "Failed to load car texture!\n" );
        success = false;
    }

    return success;
}

bool loadDot()
{
    //Loading success flag
    bool success = true;

    if( !bDot.loadFromFile( "graphic/dot_blue.png" )
       || !rDot.loadFromFile( "graphic/dot_red.png" )
       || !bSquare.loadFromFile( "graphic/square_blue.png" )
       || !rSquare.loadFromFile( "graphic/square_red.png" ))
    {
        printf( "Failed to load car texture!\n" );
        success = false;
    }

    return success;
}

bool hitASquare(BlueCar& blue, RedCar& red, Obstacle& obstacle)
{
    if (SDL_HasIntersection( &blue.sprite, &obstacle.sprite )
        || SDL_HasIntersection( &red.sprite, &obstacle.sprite ))
        if (obstacle.obType == 1)
        {
            score++;
            if (music == true) Mix_PlayChannel( -1, mScore, 0 );
            //cout << score << endl;
            obstacle.obType = 2;
            //cout << obstacle.y << endl;
            return false;
        }
        else if (obstacle.obType == 0)
        {
            //cout << "SCORE: " << score << endl << "HIT A SQUARE" << endl <<"GAME OVER" << endl << obstacle.y << " " << obstacle.x;
            //close();
            return true;
        }
        return false;
}

bool missAPoint(Obstacle& obstacle)
{
    if ((obstacle.y + OBSTACLE_HEIGHT >= SCREEN_HEIGHT)
        && (obstacle.obType == 1)
        && (obstacle.y <= SCREEN_HEIGHT + obstacle.obVel))
    {
        //cout << "SCORE: " << score << endl << "MISS A POINT" << endl << "GAME OVER";
        return true;
    }
    else if ((obstacle.y >= SCREEN_HEIGHT)
             && (obstacle.obType == 0 || obstacle.obType == 2)
             && (obstacle.y < 1004))
    {
        return false;
    }
    return false;
}

void close()
{
    //Free loaded images
    rDot.free();
    bDot.free();
    rSquare.free();
    bSquare.free();
    gBackground.free();
    gBlueCar.free();
    gDarkBackground.free();
    gPlay.free();
    gReplay.free();
    gHighScore.free();
    gMusicOff.free();
    gMusicOn.free();
    gHome.free();
    gPause.free();

    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    srand(time(NULL));
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        //Load media
        if( !loadAudio()
           ||  !loadBackground()
           || (!loadDot())
           || (!loadBlueCar())
           || (!loadRedCar())
           || !loadMedia())
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Main loop flag
            bool play = false;
            bool pause = false;
            bool quit = false;
            bool home = true;
            bool replay = false;

            //Event handler
            SDL_Event e;

            //The objects that will be moving around on the screen
            Obstacle O1, O2, O3, O4, O5, O6;

            BlueCar blueCar;

            RedCar redCar;

            Text message, textScore, textHighScore;

            //While application is running
            while ( !quit )
            {
                if ( home )
                {
                    //input text
                    gText.loadText("2CARS", 150);
                    //Opening music
                    Mix_PlayMusic( mOpen, -1 );
                }
                //while in menu
                while ( home )
                {
                    while( SDL_PollEvent( &e ) != 0 )
                    {
                        //User requests quit
                        if( e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_ESCAPE))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            quit = true;
                            home = false;
                        }
                        if (gMusicOn.handleEvent( &e ))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            if (music == true) music = false;
                            else if (music == false) music = true;
                        }
                        if (gHighScore.handleEvent( &e ))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            if (music == true) Mix_PlayChannel( -1, mHighScore, 0 );
                            //Clear screen
                            SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                            SDL_RenderClear( gRenderer );

                            gBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                            gDarkBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

                            ghighScoreText.loadText("Best  ", 100);
                            highScoreText.loadText(to_string(highscore), 150);
                            ghighScoreText.render(100, 100, ghighScoreText.mWidth, ghighScoreText.mHeight - 10);
                            if (highscore >= 10)
                                highScoreText.render(133, 260, highScoreText.mWidth, highScoreText.mHeight - 30);
                            else if (highscore < 10)
                                highScoreText.render(150, 260, highScoreText.mWidth, highScoreText.mHeight - 30);
                            //Update screen
                            SDL_RenderPresent( gRenderer );
                            SDL_Delay(2000);
                        }
                        if (gPlay.handleEvent( &e ) || (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_SPACE))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            home = false;
                            play = true;

                            //Initialize variables
                            O1.y = -1004;
                            if (rand() % 2 == 1) O1.x = line4;
                            else O1.x = line1;
                            O2.y = -1004;
                            O3.y = -1004;
                            O4.y = -1004;
                            O5.y = -1004;
                            O6.y = 1004;
                            blueCar.x = line1;
                            redCar.x = line4;
                            blueCar.bdegree = 0;
                            redCar.rdegree = 0;
                            score = 0;
                            blueCar.bVel = 0;
                            redCar.rVel = 0;
                            lvl = 1;
                        }
                    }
                    //Clear screen
                    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                    SDL_RenderClear( gRenderer );

                    //button postion update
                    gPlay.setPosition( 82, 220, 210, 210 );
                    gHighScore.setPosition( 80, 460, 80, 80);
                    gMusicOn.setPosition( 200, 460, 80, 80);

                    //Render objects
                    gBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                    gDarkBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                    gPlay.render( 82, 220, 210, 210);
                    gHighScore.render( 80, 460, 80, 80);
                    if (music == true) gMusicOn.render( 200, 460, 80, 80);
                    else if (music == false) gMusicOff.render( 200, 460, 80, 80);
                    gText.render(25, 60, gText.mWidth, gText.mHeight);

                    //cout << music;

                    //Update screen
                    SDL_RenderPresent( gRenderer );
                }

                //Pause screen
                while ( pause )
                {
                    while( SDL_PollEvent( &e ) != 0 )
                    {
                        //User requests quit
                        if( e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_ESCAPE))
                        {
                            quit = true;
                            pause = false;
                        }
                        if (gPlay.handleEvent( &e ) || (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_SPACE))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            pause = false;
                            play = true;
                        }
                        if (gHome.handleEvent( &e ))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            pause = false;
                            home = true;
                        }
                        if (gMusicOn.handleEvent( &e ))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            if (music == true) music = false;
                            else if (music == false) music = true;
                        }
                    }
                    //Clear screen
                    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                    SDL_RenderClear( gRenderer );

                    //Freeze screen
                    gBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                    blueCar.render();
                    redCar.render();
                    O1.show();
                    O2.show();
                    O3.show();
                    O4.show();
                    O5.show();
                    O6.show();
                    gPause.render(10, 5, 40, 53);

                    gText.loadText("Continue?", 90);
                    gPlay.setPosition(98, 270, 180, 180);
                    gHome.setPosition(300, 80, 50, 50);
                    gMusicOn.setPosition( 300, 15, 50, 50);

                    gDarkBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                    gPlay.render( 98, 270, 180, 180);
                    gHome.render( 300, 80, 50, 50);
                    if (music == true) gMusicOn.render( 300, 15, 50, 50);
                    else if (music == false) gMusicOff.render( 300, 15, 50, 50);
                    gText.render(50, 180, gText.mWidth, gText.mHeight);

                    //Update screen
                    SDL_RenderPresent( gRenderer );

                    //Delay 3 sec before start
                    if (play == true)
                    {
                        if (music == true) Mix_PlayChannel( -1, mPause, 0 );
                        //Clear screen
                        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                        SDL_RenderClear( gRenderer );
                        gBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                        blueCar.render();
                        redCar.render();
                        O1.show();
                        O2.show();
                        O3.show();
                        O4.show();
                        O5.show();
                        O6.show();
                        scoreText.loadText("3", 150);
                        scoreText.render((SCREEN_WIDTH - scoreText.mWidth) / 2, (SCREEN_HEIGHT - scoreText.mHeight) / 2 - 100, scoreText.mWidth, scoreText.mHeight - 30);
                        //Update screen
                        SDL_RenderPresent( gRenderer );

                        SDL_Delay(1000);

                        //Clear screen
                        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                        SDL_RenderClear( gRenderer );
                        gBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                        blueCar.render();
                        redCar.render();
                        O1.show();
                        O2.show();
                        O3.show();
                        O4.show();
                        O5.show();
                        O6.show();
                        scoreText.loadText("2", 150);
                        scoreText.render((SCREEN_WIDTH - scoreText.mWidth) / 2, (SCREEN_HEIGHT - scoreText.mHeight) / 2 - 100, scoreText.mWidth, scoreText.mHeight - 30);
                        //Update screen
                        SDL_RenderPresent( gRenderer );

                        SDL_Delay(1000);

                        //Clear screen
                        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                        SDL_RenderClear( gRenderer );
                        gBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                        blueCar.render();
                        redCar.render();
                        O1.show();
                        O2.show();
                        O3.show();
                        O4.show();
                        O5.show();
                        O6.show();
                        scoreText.loadText("1", 150);
                        scoreText.render((SCREEN_WIDTH - scoreText.mWidth) / 2, (SCREEN_HEIGHT - scoreText.mHeight) / 2 - 100, scoreText.mWidth, scoreText.mHeight - 30);
                        //Update screen
                        SDL_RenderPresent( gRenderer );

                        SDL_Delay(1000);

                        //Clear screen
                        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                        SDL_RenderClear( gRenderer );
                        gBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                        blueCar.render();
                        redCar.render();
                        O1.show();
                        O2.show();
                        O3.show();
                        O4.show();
                        O5.show();
                        O6.show();
                        scoreText.loadText("GO", 150);
                        scoreText.render((SCREEN_WIDTH - scoreText.mWidth) / 2, (SCREEN_HEIGHT - scoreText.mHeight) / 2 - 100, scoreText.mWidth, scoreText.mHeight - 30);
                        //Update screen
                        SDL_RenderPresent( gRenderer );
                        SDL_Delay(1000);
                    }
                }

                //while playing game
                while( play )
                {
                    //Handle events on queue
                    while( SDL_PollEvent( &e ) != 0 )
                    {
                        //User requests quit
                        if( e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_ESCAPE))
                        {
                            quit = true;
                            play = false;
                        }
                        blueCar.handle_Event( e );
                        redCar.handle_Event( e );
                        if ( gPause.handleEvent( &e ) || (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_SPACE))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            play = false;
                            pause = true;
                        }
                    }

                    //Creat Obstacle
                    if ((O6.y >= OBJ_DISTANCE - reduceObjDistance * lvl && O6.y < OBJ_DISTANCE + O6.obVel - reduceObjDistance * lvl) || O6.y == 1004) O1.create(O6.x);
                    if (O1.y >= OBJ_DISTANCE - reduceObjDistance * lvl && O1.y < OBJ_DISTANCE + O1.obVel - reduceObjDistance * lvl) O2.create(O1.x);
                    if (O2.y >= OBJ_DISTANCE - reduceObjDistance * lvl && O2.y < OBJ_DISTANCE + O2.obVel - reduceObjDistance * lvl) O3.create(O2.x);
                    if (O3.y >= OBJ_DISTANCE - reduceObjDistance * lvl && O3.y < OBJ_DISTANCE + O3.obVel - reduceObjDistance * lvl) O4.create(O3.x);
                    if (O4.y >= OBJ_DISTANCE - reduceObjDistance * lvl && O4.y < OBJ_DISTANCE + O4.obVel - reduceObjDistance * lvl) O5.create(O4.x);
                    if (O5.y >= OBJ_DISTANCE - reduceObjDistance * lvl && O5.y < OBJ_DISTANCE + O5.obVel - reduceObjDistance * lvl) O6.create(O5.x);
                    //cout << O1.y << " " << O2.y << " " << O3.y << " " << O4.y << " " << O5.y << " " << O6.y << endl;
                    //cout << O1.y << " " << OBJ_START - (SCREEN_HEIGHT - START) << endl;

                    //Move
                    blueCar.move();
                    redCar.move();
                    O1.update_Pos();
                    O2.update_Pos();
                    O3.update_Pos();
                    O4.update_Pos();
                    O5.update_Pos();
                    O6.update_Pos();

                    //Survive Condition
                    //HIT A SQUARE
                    if ( hitASquare(blueCar, redCar, O1)
                        || hitASquare(blueCar, redCar, O2)
                        || hitASquare(blueCar, redCar, O3)
                        || hitASquare(blueCar, redCar, O4)
                        || hitASquare(blueCar, redCar, O5)
                        || hitASquare(blueCar, redCar, O6))
                    {
                        play = false;
                        replay = true;
                    }
                    //MISS A POINT
                    if ( missAPoint(O1)
                        || missAPoint(O2)
                        || missAPoint(O3)
                        || missAPoint(O4)
                        || missAPoint(O5)
                        || missAPoint(O6))
                    {
                        play = false;
                        replay = true;
                    }

                    //Set up media
                    gPause.setPosition(10, 5, 40, 53);
                    scoreText.loadText(to_string(score), 60);


                    //Clear screen
                    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                    SDL_RenderClear( gRenderer );

                    //Render objects
                    gBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                    blueCar.render();
                    redCar.render();
                    O1.show();
                    O2.show();
                    O3.show();
                    O4.show();
                    O5.show();
                    O6.show();
                    gPause.render(10, 5, 40, 53);
                    if (score >= 10) scoreText.render( 305, 8, scoreText.mWidth , scoreText.mHeight - 10);
                    else if (score < 10) scoreText.render( 320, 8, scoreText.mWidth , scoreText.mHeight - 10);

                    //Update screen
                    SDL_RenderPresent( gRenderer );

                    if ((score % 5 == 0) && (score > prevScore) && (lvl <= maxLvl)) lvl++;
                    prevScore = score;
                    //cout << lvl << " " << O1.obVel << " " << OBJ_DISTANCE - reduceObjDistance * lvl << endl;

                    if(replay == true && play == false)
                    {
                        switch (rand() % 4)
                        {
                            case 0:
                                if (music == true) Mix_PlayChannel( -1, mGameOver1, 0 );
                                SDL_Delay(500);
                                break;
                            case 1:
                                if (music == true) Mix_PlayChannel( -1, mGameOver2, 0);
                                SDL_Delay(150);
                                break;
                            case 2:
                                if (music == true) Mix_PlayChannel( -1, mGameOver3, 0 );
                                SDL_Delay(2400);
                                break;
                            case 3:
                                if (music == true) Mix_PlayChannel( -1, mGameOver4, 0 );
                                SDL_Delay(360);
                                break;
                        }

                        if (score >= highscore) highscore = score;

                    }
                }

                if ( replay == true )
                {
                    //Load text
                    gText.loadText("GAME OVER", 80);
                    ghighScoreText.loadText("Best  ", 50);
                    gscoreText.loadText("Score ", 50);
                    highScoreText.loadText(to_string(highscore), 50);
                    scoreText.loadText(to_string(score), 50);
                }
                //Replay and score screen
                while ( replay )
                {
                    while( SDL_PollEvent( &e ) != 0 )
                    {
                        //User requests quit
                        if( e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_ESCAPE))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            quit = true;
                            replay = false;
                        }
                        if ( gReplay.handleEvent( &e ) || (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_SPACE))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            replay = false;
                            play = true;

                            //Initialize variables
                            O1.y = -1004;
                            if (rand() % 2 == 1) O1.x = line4;
                            else O1.x = line1;
                            O2.y = -1004;
                            O3.y = -1004;
                            O4.y = -1004;
                            O5.y = -1004;
                            O6.y = 1004;
                            blueCar.x = line1;
                            redCar.x = line4;
                            blueCar.bdegree = 0;
                            redCar.rdegree = 0;
                            blueCar.bVel = 0;
                            redCar.rVel = 0;
                            score = 0;
                            lvl = 1;
                        }
                        if ( gHome.handleEvent( &e ))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            replay = false;
                            home = true;
                        }
                        if (gMusicOn.handleEvent( &e ))
                        {
                            if (music == true) Mix_PlayChannel( -1, mClick, 0 );
                            if (music == true) music = false;
                            else if (music == false) music = true;
                        }
                    }

                    //Clear screen
                    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                    SDL_RenderClear( gRenderer );

                    //Freeze screen
                    gBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                    blueCar.render();
                    redCar.render();
                    O1.show();
                    O2.show();
                    O3.show();
                    O4.show();
                    O5.show();
                    O6.show();
                    gPause.render(10, 5, 40, 53);

                    gReplay.setPosition( 105, 310, 150, 150);
                    gHome.setPosition( 90, 480, 70, 70);
                    gMusicOn.setPosition( 205, 480, 70, 70);

                    gDarkBackground.render( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                    gReplay.render( 105, 310, 150, 150);
                    gHome.render( 90, 480, 70, 70);
                    if (music == true) gMusicOn.render( 205, 480, 70, 70);
                    else gMusicOff.render( 205, 480, 70, 70);
                    gText.render(15, 80, gText.mWidth, gText.mHeight);
                    ghighScoreText.render(80, 225, ghighScoreText.mWidth, ghighScoreText.mHeight - 10);
                    gscoreText.render(80, 180, gscoreText.mWidth, gscoreText.mHeight - 10);
                    highScoreText.render(240, 225, highScoreText.mWidth, highScoreText.mHeight - 10);
                    scoreText.render(240, 180, scoreText.mWidth, scoreText.mHeight - 10);

                    //Update screen
                    SDL_RenderPresent( gRenderer );
                }
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}
