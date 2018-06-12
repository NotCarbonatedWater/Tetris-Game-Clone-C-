#include <iostream>
#include <SFML/Graphics.hpp> // library is provided by https://www.sfml-dev.org/ 
#include <time.h>

// main vars
const int BlockHieght = 20; // hieght of block movable area 
const int BlockWidth = 10; // width of block movable area 
const int BlockSize = 50; // pixel size of block
const int InitialX = 710; // where border starts 
const int InitialY = 0; // where border starts 
const int InitialX2 = 1525; // where border starts 
const int InitialY2 = 200; // where border starts 
int m = 0; // prediction block picker

/////////////////////////////////////////////////////////////////////////////////////////////////////////

int Shapes[7][4] = //shapes 
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};

struct Point
{
	int x, x2, y, y2;
} array1[4], array2[4], array3[4]; //array 1 is cuurent block array; array2 is array that holds old blocks

int field[BlockHieght][BlockWidth] = { 0 };

bool check()
{
	for (int index = 0; index < 4; index++)
	{
		if (array1[index].x < 0 || array1[index].x >= BlockWidth || array1[index].y >= BlockHieght) return 0; //stops blocks at the bottom
		else if (field[array1[index].y][array1[index].x]) return 0; //colluision dectition against other blocks

	}
	return 1;
};

void MakeNewBlock() // passes down prediction block and creates a new one 
{
	int n = m; // transfers prediction block to current
	m = rand() % 7; // creates a new new block for predicition block
	for (int i = 0; i<4; i++) // current block
	{
		array3[i].x2 = Shapes[m][i] % 2; // appends columns to prediction x array
		array3[i].y2 = Shapes[m][i] / 2; // appends rows to prediction y array 
	}
	for (int i = 0; i<4; i++) // current block
	{
		array1[i].x = Shapes[n][i] % 2; // appends columns to current x array 
		array1[i].y = Shapes[n][i] / 2; // appends rows to current y array
	}
}

int main()
{
	srand(time(0));

	sf::RenderWindow window(sf::VideoMode(1920, 1080), "My Program Code"); // creates window
	sf::Texture t1, t2, t3, t4, t5, t6; // images use for sprites 
	t1.loadFromFile("images/black_tile.png");
	t2.loadFromFile("images/pink_test.png");
	t3.loadFromFile("images/border_clear.png");
	t4.loadFromFile("images/small_screen.png");
	t5.loadFromFile("images/screen_score.png");
	t6.loadFromFile("images/black_tile_2.png");
	sf::Sprite s(t1), background(t2), frame(t3), smallWindow(t4), screenScore(t5), s2(t6); // asigns sprites

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// game play set up //
	int dx = 0; 
	bool rotate = 0; 
	int colorNum = 1;
	float timer = 0, delay = 0.3;
	sf::Clock clock;
	MakeNewBlock();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// actual running game starts here //
	while (window.isOpen()) // running window - game rules/control
	{
		//time
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += (time / 2);  // passing time

		//key events
		sf::Event e;
		while (window.pollEvent(e)) 
		{
			if (e.type == sf::Event::Closed) window.close(); // destroy windows if quit
			if (e.type == sf::Event::KeyPressed)
				if (e.key.code == sf::Keyboard::Up) rotate = true; // top arrow rotates block
				else if (e.key.code == sf::Keyboard::Left) dx = -1; // left arrow movement 
				else if (e.key.code == sf::Keyboard::Right) dx = 1; // right arrow movement  
				else if (e.key.code == sf::Keyboard::Escape) return 0; // exit screen  
				else if (e.key.code == sf::Keyboard::R) return 0; // right arrow movement  
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) delay = 0.05; // block speeds up as down arrow is pressed 
		
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// <- Move -> 
		for (int i = 0; i<4; i++) //adds left and right movement
		{
			array2[i] = array1[i]; 
			array1[i].x += dx; 
		} 
		if (!check()) for (int i = 0; i < 4; i++) //"walls" keeps blocks in playing area
		{
			array1[i] = array2[i];
		}
		//Rotate
		if (rotate)
		{
			Point p = array1[1]; //center of rotation
			for (int i = 0; i<4; i++)
			{
				int x = array1[i].y - p.y;
				int y = array1[i].x - p.x;
				array1[i].x = p.x - x;
				array1[i].y = p.y + y;
			}
			if (!check()) for (int index = 0; index < 4; index++)
			{
				array1[index] = array2[index];
			}
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (timer>delay) // "running timer" 
		{
			for (int i = 0; i<4; i++) 
			{
				array2[i] = array1[i];
				array1[i].y += 1; // blocks moves down
			}
			

			if (!check())
			{
				for (int i = 0; i < 4; i++)
				{
					field[array2[i].y][array2[i].x] = colorNum;

				}
				/////////////////////////////////////////////////////////////////////////////////////////////
				// creates new block and transfer //
				
				MakeNewBlock(); 
			}
			timer = 0;
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//check lines and destroy lines if x axis array is complete 
		int k = BlockHieght - 1;
		for (int i = BlockHieght - 1; i>0; i--)
		{
			int count = 0;
			for (int j = 0; j<BlockWidth; j++)
			{
				if (field[i][j])
				{
					count++; // if row is complete --
				}
				field[k][j] = field[i][j];
			}
			if (count < BlockWidth)
			{
				k--; //check row
			}
		}
		dx = 0; rotate = 0; delay = 0.3;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		//draw
		window.clear(sf::Color::White);
		window.draw(background);
		window.draw(frame);
		window.draw(smallWindow);

		for (int i = 0; i<BlockHieght; i++) // draws old blocks "draws laid blocks"
			for (int j = 0; j<BlockWidth; j++)
			{
				if (field[i][j] == 0) continue;
				s.setTextureRect(sf::IntRect(field[i][j] * BlockSize, 0, BlockSize, BlockSize));
				s.setPosition(j * BlockSize, i * BlockSize);
				s.move(InitialX, InitialY); //offset
				window.draw(s); // draws blocks at the bottom
			}

		for (int i = 0; i<4; i++) // draws falling blocks 
		{
			s.setTextureRect(sf::IntRect(colorNum * BlockSize, 0, BlockSize, BlockSize));
			s.setPosition(array1[i].x * BlockSize, array1[i].y * BlockSize);
			s.move(InitialX, InitialY); //offset
			window.draw(s);  // draws currently falling block
		}

		for (int i = 0; i<4; i++) // prediction block 
		{
			s2.setTextureRect(sf::IntRect(colorNum * BlockSize, 0, BlockSize, BlockSize));
			s2.setPosition(array3[i].x2 * BlockSize, array3[i].y2 * BlockSize);
			s2.move(InitialX2, InitialY2); //offset
			window.draw(s2); // draws prediction block the right of the screen 
		}
		window.display(); // shows player whole window
	}
	return 0; 
}
