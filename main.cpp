#include "table.h"
using namespace sf;

//int worms_number = thread::hardware_concurrency() - 1;
int worms_number = 16;
int worms_length = 4;

int screen_x = 1000;
int screen_y = 600;

int rows = 20;
int columns = 40;

// Create Window
RenderWindow* window = new RenderWindow(VideoMode(screen_x, screen_y), "Multithreading Worms"); 

int main() { 

	Table* t = new Table(window, worms_number, worms_length, screen_x, screen_y, rows, columns);

	//window->setFramerateLimit(50); // FPS Limiter	
	Event event;
	vector<thread> treads(worms_number);
	for (int i = 0; i < worms_number; i++) treads[i] = thread(&Table::send_thread, t, i);
	
	while (window->isOpen()) { 
		 
		while (window->pollEvent(event)) { 
			if (event.type == Event::Closed) window->close(); 
		} 
	
		window->clear(Color::Blue); 
		t->draw();
		RectangleShape r;
		window->display(); // Mostrar Pantalla
	}
	
	return 0; 
}
