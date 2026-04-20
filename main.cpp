#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x = 1600;
int screen_y = 900;

// prototypes
void draw_player(RenderWindow& window, Sprite& playerSprite, float player_x, float player_y);
void display_level(RenderWindow& window, const int height, const int width, char** lvl, Sprite& wallSprite1, const int cell_size);

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
	}

	for (int i = 0; i < 25; i++)
	{
		lvl[13][i] = 'g';
	}
	
	Texture wallTex1;
	Sprite wallSprite1;

	wallTex1.loadFromFile("Sprites/blocks/stone.png");
	wallSprite1.setTexture(wallTex1);

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
	float groundLevel = 13 * cell_size - Pheight;

	Texture playerTex;
	Sprite playerSprite;

	playerTex.loadFromFile("Sprites/Character.png");
	playerSprite.setTexture(playerTex);
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

		

		// Smooth movement and jump
		if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) // move left
		{
			player_x -= velocityX;

		}

		if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) // move right
		{
			player_x += velocityX;
			
		}

		if ((Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::Space)) && onGround) // jump
		{
			velocityY = -20;
			onGround = false;
		}
		
		// X Bounds
		if(player_x < 0)
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

		velocityY += gravity;
		player_y += velocityY;

		window.clear();

		display_level(window, height, width, lvl, wallSprite1, cell_size);
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

void display_level(RenderWindow& window, const int height, const int width, char** lvl, Sprite& wallSprite1, const int cell_size)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (lvl[i][j] == 'g')
			{
				wallSprite1.setPosition(j * cell_size, i * cell_size);
				window.draw(wallSprite1);
			}
		}
	}
}
