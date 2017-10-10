#include <SFML\Graphics.hpp>
#include <SFML\Graphics\Font.hpp>
#include <iostream>
#include <istream>
#include <thread>
#include <chrono>
#include <vector>
#include <windows.h>
#include <cstdlib>
#include <mutex>
#include <string>
#include <random>

using namespace std;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


static int number_of_threads = 5;
static int window_width = 1500;
static int window_height = 985;

std::random_device rd; // obtain a random number from hardware
std::mt19937 eng(rd()); // seed the generator
std::uniform_int_distribution<> range_x(25, window_width - 25); // define the range
std::uniform_int_distribution<> range_y(25, window_height - 25); // define the range

static int number_of_trees_to_spawn = 30;
static int total_trees = 0;
string key_pressed;
static int speed = 1200;
int spawn_counter = 1;
double spawn_seconds =1;

bool magazine_message = false;
bool nature_spawned = false;

vector <bool> messages_written;

int x, y;

std::thread nature;
std::thread l1;
std::thread l2;
std::thread l3;
std::thread l4;
std::thread l5;
std::thread time_thread;

vector <sf::RectangleShape> loadings;

std::chrono::steady_clock::time_point start_program;
std::chrono::steady_clock::time_point end_program;

std::chrono::duration<double> elapsed_program;
std::chrono::duration<double> global_time;

std::chrono::steady_clock::time_point global_start, global_update;

sf::RectangleShape magazine;

sf::RectangleShape lumber1;
sf::RectangleShape lumber2;
sf::RectangleShape lumber3;
sf::RectangleShape lumber4;
sf::RectangleShape lumber5;

vector <sf::CircleShape> normal_trees;
vector <sf::CircleShape> too_small_trees;
sf::Vector2f spawnVector;

clock_t place_wood_start;
clock_t place_wood_stop;
double place_wood_value;
bool start_taken = false;
bool global_time_taken = false;

std::mutex mu;

void search_for_trees(sf::RectangleShape &rec)
{
	sf::Vector2f tree_position=sf::Vector2f(0.f,0.f);
	for (int i = 0; i < normal_trees.size(); i++)
	{
		if (!normal_trees.at(i).busy)
		{
			normal_trees.at(i).busy = true;
			rec.focused_tree_number = i;
			tree_position = sf::Vector2f(normal_trees.at(i).getPosition().x, normal_trees.at(i).getPosition().y);

			double x;
			double y;
			x = tree_position.x - rec.getPosition().x;
			y = (tree_position.y - rec.getPosition().y) + 20;
			rec.move_Vector = sf::Vector2f(x / speed, y / speed);
			rec.focus = tree_position;
			messages_written.at(rec.number - 1) = false;
			return;
		}
	}
	
	mu.lock();
	if (rec.number == 1)
	{
		SetConsoleTextAttribute(hConsole, 6);
	}
	else if (rec.number == 2)
	{
		SetConsoleTextAttribute(hConsole, 4);
	}
	else if (rec.number == 3)
	{
		SetConsoleTextAttribute(hConsole, 3);
	}
	else if (rec.number == 4)
	{
		SetConsoleTextAttribute(hConsole, 1);
	}
	else if (rec.number == 5)
	{
		SetConsoleTextAttribute(hConsole, 2);
	}
	if (!messages_written.at(rec.number - 1))
	{
		std::cout << "Thread ";
		std::cout << rec.number;
		std::cout << " : no free trees " << endl;
		messages_written.at(rec.number - 1) = true;
	}
	mu.unlock();
}


class Magazine {

	
public:
	Magazine()
	{
		cout << "Magazine created" << endl;
	}

	
};

bool thread_loaded = false;

bool checkIfNearTarget(sf::RectangleShape &rec)
{
	if (rec.getPosition().x <= rec.focus.x + 10 && rec.getPosition().x >= rec.focus.x - 10
			&& rec.getPosition().y <= rec.focus.y + 25 && rec.getPosition().y >= rec.focus.y - 10)
	{
			return true;
	}
	else
	{
			return false;
	}
}

void spawn_tree()
{
	srand(time(NULL));
	x = rand() % ((window_width - 50) + 25);
	srand(time(NULL));
	y = rand() % ((window_height - 50) + 25);
	spawnVector = sf::Vector2f(x, y);
	normal_trees.push_back(sf::CircleShape(10.f));
	normal_trees.at(normal_trees.size() - 1).setPosition(spawnVector);
	normal_trees.at(normal_trees.size() - 1).setFillColor(sf::Color::Green);
}


void nature_spawn()
{
	srand(time(NULL));
	int add = rand() % 1000;

	if (!nature_spawned)
	{
		start_program = std::chrono::steady_clock::now();
		nature_spawned = true;
	}
	end_program = std::chrono::steady_clock::now();

	elapsed_program = end_program - start_program;

	if (elapsed_program.count() >= (spawn_seconds / 8) * spawn_counter)
	{
		loadings.at(spawn_counter - 1).setFillColor(sf::Color::Green);
		spawn_counter++;
		if (spawn_counter == 9)
		{
			if (total_trees < number_of_trees_to_spawn)
			{
				x = range_x(eng);
				y = range_y(eng);
				spawnVector = sf::Vector2f(x, y);

				too_small_trees.push_back(sf::CircleShape(5.f));
				too_small_trees.at(too_small_trees.size() - 1).setPosition(spawnVector);
				too_small_trees.at(too_small_trees.size() - 1).setFillColor(sf::Color::Transparent);
				too_small_trees.at(too_small_trees.size() - 1).setOutlineColor(sf::Color::Green);
				too_small_trees.at(too_small_trees.size() - 1).setOutlineThickness(1);
				too_small_trees.at(too_small_trees.size() - 1).age = 0;
				total_trees++;

				x = range_x(eng);
				y = range_y(eng);
				spawnVector = sf::Vector2f(x, y);

				too_small_trees.push_back(sf::CircleShape(5.f));
				too_small_trees.at(too_small_trees.size() - 1).setPosition(spawnVector);
				too_small_trees.at(too_small_trees.size() - 1).setFillColor(sf::Color::Transparent);
				too_small_trees.at(too_small_trees.size() - 1).setOutlineColor(sf::Color::Green);
				too_small_trees.at(too_small_trees.size() - 1).setOutlineThickness(1);
				too_small_trees.at(too_small_trees.size() - 1).age = 0;
				total_trees++;
			}

			for (int i = 0; i < too_small_trees.size(); i++)
			{
				too_small_trees.at(i).age += 10;
				if (too_small_trees.at(i).age == 10)
				{
					too_small_trees.at(i).setOutlineThickness(2);
				}
				if (too_small_trees.at(i).age == 20)
				{
					too_small_trees.at(i).setOutlineThickness(3);
				}
				if (too_small_trees.at(i).age == 30)
				{
					too_small_trees.at(i).setOutlineThickness(4);
				}
				if (too_small_trees.at(i).age == 40)
				{
					too_small_trees.at(i).setOutlineThickness(5);
				}
				if (too_small_trees.at(i).age == 50)
				{
					too_small_trees.at(i).setOutlineThickness(6);
				}
				if (too_small_trees.at(i).age == 60)
				{
					too_small_trees.at(i).setOutlineThickness(7);
				}
				if (too_small_trees.at(i).age == 70)
				{
					too_small_trees.at(i).setOutlineThickness(8);
				}
				if (too_small_trees.at(i).age == 80)
				{
					too_small_trees.at(i).setOutlineThickness(9);
				}
				if (too_small_trees.at(i).age == 90)
				{
					too_small_trees.at(i).setOutlineThickness(10);
				}
				if (too_small_trees.at(i).age >= 100)
				{
					too_small_trees.at(i).setFillColor(sf::Color::Green);
					normal_trees.push_back(too_small_trees.at(i));
					too_small_trees.erase(too_small_trees.begin() + i);
				}
				
			}

			for (int i = 0; i < loadings.size(); i++)
			{
				loadings.at(i).setFillColor(sf::Color::White);
			}
			spawn_counter = 1;
			start_program = std::chrono::steady_clock::now();
		}
	}
}

void time_function()
{
	if (!global_time_taken)
	{
		global_time_taken = true;
		global_start = std::chrono::steady_clock::now();
	}
	while (global_time.count() <= 1)
	{
		global_update = std::chrono::steady_clock::now();
		global_time = global_update - global_start;
		cout << global_time.count() << endl;
	}
}

void setConsoleColor(sf::RectangleShape &rec)
{
	if (rec.number == 1)
	{
		SetConsoleTextAttribute(hConsole, 6);
	}
	else if (rec.number == 2)
	{
		SetConsoleTextAttribute(hConsole, 4);
	}
	else if (rec.number == 3)
	{
		SetConsoleTextAttribute(hConsole, 3);
	}
	else if (rec.number == 4)
	{
		SetConsoleTextAttribute(hConsole, 1);
	}
	else if (rec.number == 5)
	{
		SetConsoleTextAttribute(hConsole, 2);
	}
}

void thread_function(sf::RectangleShape &rec)
{
	sf::Vector2f homeVector = sf::Vector2f(window_width / 2, window_height / 2 - 25);
	sf::Color brown = sf::Color(125, 45, 0);

	if (rec.focus == sf::Vector2f(0.f, 0.f))				//idle state, searching for trees
	{
		rec.busy = false;
		rec.goes_to_magazine = false;
		rec.setFillColor(sf::Color::White);
		search_for_trees(rec);								//searching for free trees if focus is Vector Zero
	}

	else if (rec.focus != sf::Vector2f(0.f, 0.f) && checkIfNearTarget(rec) == false)	// moving condition
	{
		rec.move(rec.move_Vector);														//if focus is not Vector Zero and lumber is not near target, he moves
	}

	else if (rec.focus != sf::Vector2f(0.f, 0.f) && rec.focus != homeVector && checkIfNearTarget(rec) == true)	//CUTTING TREE
	{
		//mu.lock();

		if (rec.cutting_tree == false)																			//setting start time, executes only once
		{
			mu.lock();
			setConsoleColor(rec);
			std::cout << "Thread ";
			std::cout << rec.number;
			std::cout << " : cutting tree " << endl;
			rec.start_cut = clock();
			rec.stop_cut = clock();
			rec.cutting_tree = true;		
			mu.unlock();
		}

		rec.stop_cut = clock();																				
		rec.cutting_time = ((double)rec.stop_cut - rec.start_cut) * 1000.0 / CLOCKS_PER_SEC;					//UPDATING TIME TO CUT

		if (rec.cutting_time > rec.cut_speed)																	//TREE CUT
		{

			normal_trees.at(rec.focused_tree_number).setFillColor(sf::Color::Transparent);
			normal_trees.at(rec.focused_tree_number).setOutlineThickness(0);
			normal_trees.at(rec.focused_tree_number).setOutlineColor(sf::Color::Green);

			rec.cutting_tree = false;
			rec.setFillColor(brown);
			rec.focus = homeVector;																				//After tree is CUT sets focus to magazine
			double x1 = rec.focus.x - rec.getPosition().x;
			double y1 = (rec.focus.y - rec.getPosition().y) + 20;

			rec.move_Vector = sf::Vector2f(x1 / speed, y1 / speed);
		}
	}

	else if (rec.focus == homeVector)																			// MAGAZINE
	{
			if (!start_taken)																					//ENTERING MAGAZINE
			{
				mu.lock();
				start_taken = true;
				setConsoleColor(rec);
				std::cout << "Thread ";
				std::cout << rec.number;
				std::cout << " : placing wood to magazine " << endl;
				messages_written.at(rec.number - 1) = false;
				magazine.setFillColor(rec.color);
				place_wood_start = clock();
				
				magazine.magazine_free = false;
				rec.in_magazine = true;
				magazine.numberInMagazine = rec.number;

				mu.unlock();
			}

			if(start_taken && rec.in_magazine)											//UPDATES TIME IN MAGAZINE
			{
				place_wood_stop = clock();
				place_wood_value = (double)(place_wood_stop - place_wood_start) * 1000.0 / CLOCKS_PER_SEC;
			}

			if (place_wood_value >= rec.magazine_speed && rec.in_magazine)				//WOOD PLACED
			{
				mu.lock();
				setConsoleColor(rec);
				cout << "Magazine unlocked" << endl;
				rec.setFillColor(sf::Color::White);
				rec.focus = sf::Vector2f(0.f, 0.f);
				rec.goes_to_magazine = false;
				rec.busy = false;
				rec.in_magazine = false;

				magazine.setFillColor(sf::Color::White);

				start_taken = false;
				place_wood_value = 0;
				mu.unlock();
			}
		else if(!magazine.magazine_free && !rec.in_magazine && !messages_written.at(rec.number - 1))		//IF MAGAZINE BUSY, WAIT
		{
			mu.lock();
			setConsoleColor(rec);
			cout << "Thread "<< rec.number << " waiting for releasing magazine" << endl;
			messages_written.at(rec.number - 1) = true;
			mu.unlock();
		}
	}
}

int main()
{
	Magazine mag;
	//srand(time(NULL));
	start_program = std::chrono::steady_clock::now();

	//SETTING LOADING RECTANGLES
	sf::RectangleShape loading_rec1(sf::Vector2f(15, 15));
	loading_rec1.setPosition(15, 15);
	loadings.push_back(loading_rec1);

	sf::RectangleShape loading_rec2(sf::Vector2f(15, 15));
	loading_rec2.setPosition(35, 15);
	loadings.push_back(loading_rec2);

	sf::RectangleShape loading_rec3(sf::Vector2f(15, 15));
	loading_rec3.setPosition(55, 15);
	loadings.push_back(loading_rec3);

	sf::RectangleShape loading_rec4(sf::Vector2f(15, 15));
	loading_rec4.setPosition(75, 15);
	loadings.push_back(loading_rec4);

	sf::RectangleShape loading_rec5(sf::Vector2f(15, 15));
	loading_rec5.setPosition(95, 15);
	loadings.push_back(loading_rec5);

	sf::RectangleShape loading_rec6(sf::Vector2f(15, 15));
	loading_rec6.setPosition(115, 15);
	loadings.push_back(loading_rec6);

	sf::RectangleShape loading_rec7(sf::Vector2f(15, 15));
	loading_rec7.setPosition(135, 15);
	loadings.push_back(loading_rec7);

	sf::RectangleShape loading_rec8(sf::Vector2f(15, 15));
	loading_rec8.setPosition(155, 15);
	loadings.push_back(loading_rec8);

	//ALL FLAGS FOR MESSAGES PRINTING (prevents spamming in console)
	for (int i = 0; i < number_of_threads; i++)
	{
		messages_written.push_back(false);
	}
	
	//SETTING ALL LUMBER VARIABLES
	lumber1 = sf::RectangleShape(sf::Vector2f(20, 20));								
	lumber1.busy = false;
	lumber1.setOutlineThickness(6);
	lumber1.setOutlineColor(sf::Color::Yellow);

	lumber1.focus = sf::Vector2f(0.f, 0.f);
	lumber1.cut_speed = 5000;
	lumber1.magazine_speed = 2500;
	lumber1.color = sf::Color::Yellow;
	lumber1.number = 1;
	lumber1.setPosition(sf::Vector2f(180, 350));
	lumber1.focus = sf::Vector2f(0.f, 0.f);
	lumber1.in_magazine = false;

	lumber2 = sf::RectangleShape(sf::Vector2f(20, 20));
	lumber2.setPosition(sf::Vector2f(220, 350));
	lumber2.setOutlineThickness(6);
	lumber2.setOutlineColor(sf::Color::Red);
	lumber2.focus = sf::Vector2f(0.f, 0.f);
	lumber2.cut_speed = 2000;
	lumber2.magazine_speed = 3500;
	lumber2.color = sf::Color::Red;
	lumber2.number = 2;
	lumber2.in_magazine = false;

	lumber3 = sf::RectangleShape(sf::Vector2f(20, 20));
	lumber3.setPosition(sf::Vector2f(260, 350));
	lumber3.setOutlineThickness(6);
	lumber3.setOutlineColor(sf::Color::Cyan);
	lumber3.focus = sf::Vector2f(0.f, 0.f);
	lumber3.cut_speed = 3000;
	lumber3.magazine_speed = 3000;
	lumber3.color = sf::Color::Cyan;
	lumber3.number = 3;
	lumber3.in_magazine = false;

	lumber4 = sf::RectangleShape(sf::Vector2f(20, 20));
	lumber4.setPosition(sf::Vector2f(300, 350));
	lumber4.setOutlineThickness(6);
	lumber4.setOutlineColor(sf::Color::Blue);
	lumber4.focus = sf::Vector2f(0.f, 0.f);
	lumber4.cut_speed = 5000;
	lumber4.magazine_speed = 1500;
	lumber4.color = sf::Color::Blue;
	lumber4.number = 4;
	lumber4.in_magazine = false;

	lumber5 = sf::RectangleShape(sf::Vector2f(20, 20));
	lumber5.setPosition(sf::Vector2f(340, 350));
	lumber5.setOutlineThickness(6);
	lumber5.setOutlineColor(sf::Color::Green);
	lumber5.focus = sf::Vector2f(0.f, 0.f);
	lumber5.focus = sf::Vector2f(0.f, 0.f);
	lumber5.cut_speed = 4500;
	lumber5.magazine_speed = 2500;
	lumber5.color = sf::Color::Green;
	lumber5.number = 5;
	lumber5.in_magazine = false;

	//SETTING WINDOW
	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Village");	
	window.setPosition(sf::Vector2i(410, 0));

	//SETTING MAGAZINE
	magazine = sf::RectangleShape(sf::Vector2f(100.f,50.f));
	magazine.setFillColor(sf::Color::White);
	magazine.setPosition(sf::Vector2f(window_width/2-50, window_height/2-25));
	magazine.magazine_free = true;

	//MAIN LOOP
	while (window.isOpen())
	{
		sf::Event event;
		
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.key.code == sf::Keyboard::Space)			//CLOSE IF SPACE PRESSED
			{
				window.close();
			}			
		}

		window.clear();
		
		//DRAWING ALL ELEMENTS IN PROGRAM EVERY FRAME(small trees, cuttable trees, loadings, lumbers and magazine)
		for (int i = 0; i < too_small_trees.size(); i++)
		{
			window.draw(too_small_trees.at(i));
		}
		for (int i = 0; i < normal_trees.size(); i++)
		{
			window.draw(normal_trees.at(i));
		}
		for (int i = 0; i < loadings.size(); i++)
		{
			window.draw(loadings.at(i));
		}
		window.draw(magazine);
		window.draw(lumber1);
		window.draw(lumber2);
		window.draw(lumber3);
		window.draw(lumber4);
		window.draw(lumber5);


		//THREAD SPAMMING TREES
		nature = std::thread(nature_spawn);
		nature.join();

		//DEFINING THREADS, PASSING LUMBERS to UNIVERSAL THREAD FUNCTION
		if (number_of_threads >= 1)
		{
				l1 = std::thread(thread_function, std::ref(lumber1));
		}
		
		if (number_of_threads >= 2)
		{
				l2 = std::thread(thread_function, std::ref(lumber2));	
		}
		
		if (number_of_threads >= 3)
		{
				l3 = std::thread(thread_function, std::ref(lumber3));					
		}
		if (number_of_threads >= 4)
		{			
				l4 = std::thread(thread_function, std::ref(lumber4));						
		}
		if (number_of_threads >= 5)
		{
				l5 = std::thread(thread_function, std::ref(lumber5));			
		}
		
		window.display();	
		
		//JOIN ALL THREADS
		l1.join(); l2.join(); l3.join(); l4.join(); l5.join();
	}

	std::cout << "End of Program " << endl;
	
	return 0;
}