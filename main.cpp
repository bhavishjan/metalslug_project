#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "Vehicle.h"
#include "Weapon.h"

using namespace sf;
using namespace std;

int screen_x = 1600;
int screen_y = 900;

// prototypes
void draw_player(RenderWindow& window, Sprite& playerSprite, float player_x, float player_y);
void display_level(RenderWindow& window, const int height, const int width, char** lvl, Sprite& wallSprite1, Sprite& bgSprite, const int cell_size);

int main()
{
	RenderWindow window(VideoMode(screen_x, screen_y), "Metal Slug", Style::Close);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	/////////////////////////////////////////////////////////////////
	// a cell is 64 by 64 pixels
	// 'g' = grass tile

	const int cell_size = 64;
	const int height = 14;
	const int width = 1100;

	char** lvl = NULL;

	lvl = new char* [height];
	for (int i = 0; i < height; i += 1)
	{
		lvl[i] = new char[width] {'\0'};
		for (int j = 0; j < width; ++j)
			lvl[i][j] = '.';   // empty cell
	}

	// Ground
	for (int j = 0; j < width; j++)
		lvl[13][j] = '#';


	// Mid Top & Mid Bottom Platforms
	for (int i = 2; i < 16; i++)
	{
		lvl[3][i] = '#';
		lvl[11][i] = '#';
	}


	//Mid Pattern
	lvl[4][8] = '#';
	lvl[4][9] = '#';

	lvl[5][1] = '#';
	lvl[5][2] = '#';
	lvl[5][3] = '#';
	lvl[5][4] = '#';
	lvl[5][7] = '#';
	lvl[5][8] = '#';
	lvl[5][9] = '#';
	lvl[5][10] = '#';
	lvl[5][13] = '#';
	lvl[5][14] = '#';
	lvl[5][15] = '#';
	lvl[5][16] = '#';

	lvl[6][7] = '#';
	lvl[6][10] = '#';

	lvl[7][3] = '#';
	lvl[7][4] = '#';
	lvl[7][5] = '#';
	lvl[7][6] = '#';
	lvl[7][7] = '#';
	lvl[7][10] = '#';
	lvl[7][11] = '#';
	lvl[7][12] = '#';
	lvl[7][13] = '#';
	lvl[7][14] = '#';
	lvl[7][15] = '#';

	lvl[8][7] = '#';
	lvl[8][10] = '#';

	lvl[9][1] = '#';
	lvl[9][2] = '#';
	lvl[9][3] = '#';
	lvl[9][4] = '#';
	lvl[9][7] = '#';
	lvl[9][8] = '#';
	lvl[9][9] = '#';
	lvl[9][10] = '#';
	lvl[9][13] = '#';
	lvl[9][14] = '#';
	lvl[9][15] = '#';
	lvl[9][16] = '#';

	lvl[10][8] = '#';
	lvl[10][9] = '#';


	Texture wallTex1;
	Sprite wallSprite1;
	Texture wallTex2;
	Sprite wallSprite2;
	Texture bgTexture;
	Sprite bgSprite;

	wallTex1.loadFromFile("Sprites/blocks/block.png");
	wallSprite1.setTexture(wallTex1);
	wallTex2.loadFromFile("Sprites/blocks/dirt.png");
	wallSprite2.setTexture(wallTex2);
	bgTexture.loadFromFile("Sprites/bg.jpg");
	bgSprite.setTexture(bgTexture);

	////////////////////////////////////////////////////////
	float player_x = 380;
	float player_y = 610;	// row 11 * 64 - Pheight = 704 - 94

	float max_speed = 5;
	float velocityX = 0;
	float velocityY = 0;
	float acceleration = 0.5;
	float gravity = 0.5;


	float scale_x = 0.5;
	float scale_y = 0.5;

	int raw_img_x = 115;
	int raw_img_y = 158;

	int Pheight = raw_img_y * scale_y;
	int Pwidth  = raw_img_x * scale_x;

	bool onGround = true;
	bool isfacingright = true;
	float groundLevel = 13 * cell_size - Pheight;

	Texture playerTex1;
	Texture playerTex2;
	Sprite playerSprite;

	playerTex1.loadFromFile("Sprites/character_facing_right.png");
	playerTex2.loadFromFile("Sprites/character_facing_left.png");
	playerSprite.setTexture(playerTex1);
	playerSprite.setScale(scale_x, scale_y);

	////////////////////////////////////////////////////////

	Event ev;
	while (window.isOpen())
	{
		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed)
			{
				window.close();
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		} 

		// jump movements
		if (Keyboard::isKeyPressed(Keyboard::Left)) 
		{
			player_x -= 10;
			if(isfacingright)
			{
				playerSprite.setTexture(playerTex2);
				isfacingright = false;
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Right)) 
		{
			player_x += 10;
			if(!isfacingright)
			{
				playerSprite.setTexture(playerTex1);
				isfacingright = true;
			}
		}

		if ((Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::Space)) && onGround)
		{
			velocityY = -15;
			onGround = false;
		}
		if (Keyboard::isKeyPressed(Keyboard::Down) && !onGround)
		{
			velocityY = 15;
			onGround = true;
		}
		
		velocityY += gravity;
		player_y += velocityY;


		// X Bounds
		if (player_x < 0)
			player_x = 0;

		else if (player_x + Pwidth > screen_x)
			player_x = screen_x - Pwidth;

		// Y Bound
		if (player_y >= groundLevel)
		{
			player_y = groundLevel;
			velocityY = 0;
			onGround = true;
		}
		else {
			onGround = false;
		}

		window.clear();
		
		display_level(window, height, width, lvl, wallSprite1, bgSprite, cell_size);
		draw_player(window, playerSprite, player_x, player_y);

		window.display();
	}
	return 0;
}


// functions

void draw_player(RenderWindow& window, Sprite& playerSprite, float player_x, float player_y)
{
	playerSprite.setPosition(player_x, player_y);
	window.draw(playerSprite);
}

void display_level(RenderWindow& window, const int height, const int width, char** lvl, Sprite& wallSprite1, Sprite& bgSprite, const int cell_size)
{
	bgSprite.setPosition(0, 0);
	window.draw(bgSprite);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (lvl[i][j] == '#')
			{
				wallSprite1.setPosition(j * cell_size, i * cell_size);
				window.draw(wallSprite1);
			}
		}
	}
	
}
