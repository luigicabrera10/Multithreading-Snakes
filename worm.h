#include <SFML/Graphics.hpp> 
#include <iostream>
#include <deque>
#include <utility> // Pair
#include <assert.h>

#include <thread>
#include <mutex>

#include <stdlib.h> // random
#include <time.h> // seed time
#include <chrono> // sleep

using namespace std;
using namespace sf;

bool avoid_deadlock = 1;

class Worm{
	private:
		deque< pair<int, int> > coords;
		bool waiting = false;
		Color color{rand()%255,rand()%255,rand()%255};
		
	public:
		
		Worm(deque< pair<int, int> > &worm_coords){
			this->coords = worm_coords;
		}
		
		deque< pair<int, int> >::iterator coord_head(){
			return coords.begin();
		}
		
		deque< pair<int, int> >::iterator coord_tail(){
			return coords.end();
		}
		
		pair<int,int> avoid_loop(int rows, int columns){
			int scores[4] = {0,0,0,0};
			pair<int,int> aux;
			bool flag=true;
			int i=0, j;
			aux = make_pair(coords[0].first, coords[0].second);
			for (i=0; ; i++){
				aux.first-=1; flag=false;
				if (aux.first<0) aux.first = columns-1;
				for(j = 0; j < coords.size(); j++){
					if (aux.first == coords[j].first && aux.second == coords[j].second){
						flag=true; break;
					}
				}
				if (flag){
					scores[0] = i; break;
				}
			}
			aux = make_pair(coords[0].first, coords[0].second);
			for (i=0; ; i++){
				aux.first=(aux.first+1)%columns; flag=false;
				for(j = 0; j < coords.size(); j++){
					if (aux.first == coords[j].first && aux.second == coords[j].second){
						flag=true; break;
					}
				}
				if (flag){
					scores[1] = i; break;
				}
			}
			aux = make_pair(coords[0].first, coords[0].second);
			for (i=0; ; i++){
				aux.second-=1; flag=false;
				if (aux.second<0) aux.second = rows-1;
				for(j = 0; j < coords.size(); j++){
					if (aux.first == coords[j].first && aux.second == coords[j].second){
						flag=true; break;
					}
				}
				if (flag){
					scores[2] = i; break;
				}
			}
			aux = make_pair(coords[0].first, coords[0].second);
			for (i=0; ; i++){
				aux.second=(aux.second+1)%rows; flag=false;
				for(j = 0; j < coords.size(); j++){
					if (aux.first == coords[j].first && aux.second == coords[j].second){
						flag=true; break;
					}
				}
				if (flag){
					scores[3] = i; break;
				}
			}
			aux = make_pair(coords[0].first, coords[0].second);
			int max = 0;
			for(int i =0; i < 4; i++){
				if (scores[i]>scores[max]) max = i;
			}
			
			if (max == 0){
				if (coords[0].first == 0) return make_pair(columns-1, coords[0].second);
				return make_pair(coords[0].first-1, coords[0].second);
			}
			else if (max==1) return make_pair((coords[0].first+1)%columns, coords[0].second);
			else if (max==2){
				if (coords[0].second == 0) return make_pair(coords[0].first, rows-1);
				return make_pair(coords[0].first, coords[0].second-1);
			}
			return make_pair(coords[0].first, (coords[0].second+1)%rows);
		}
		
		pair<int, int> check_loop(pair<int, int> next_pos, int rows, int columns){
			for(int i = 0; i < coords.size(); i++){
				if (next_pos.first == coords[i].first && next_pos.second == coords[i].second){
					return avoid_loop(rows, columns);;
				}
			}
			return next_pos;
		}
		
		void move_infinite(mutex** mat, int rows, int columns){
			
			for (int i = 0; i < coords.size(); i ++) mat[coords[i].second][coords[i].first].lock();
			
			auto a = std::chrono::system_clock::now().time_since_epoch().count();
			srand (a+coords[0].first*coords[0].second);
				
			int ri, i, j;
			pair<int, int> new_positions[4];
			pair <int, int> unlock_coord;
			
			while (1){
				new_positions[0] = make_pair(coords[0].first-1, coords[0].second); 
				new_positions[1] = make_pair(coords[0].first, coords[0].second-1); 
				new_positions[2] = make_pair((coords[0].first+1)%columns, coords[0].second); 
				new_positions[3] = make_pair(coords[0].first, (coords[0].second+1)%rows); 
				
				if (new_positions[0].first < 0) new_positions[0].first = columns-1;
				if (new_positions[1].second < 0) new_positions[1].second = rows-1;
				
				j = 0;
				for (i = 0; i < 4; i ++){
					if (new_positions[i].first==coords[1].first && new_positions[i].second==coords[1].second) continue;
					else new_positions[j++] = new_positions[i];
				}
				
				
				ri = rand()%3;
				unlock_coord = *(coords.end()-1);
				if (avoid_deadlock) new_positions[ri] = check_loop(new_positions[ri], rows,columns);

				mat[new_positions[ri].second][new_positions[ri].first].lock();
				coords.push_front(new_positions[ri]);
				coords.pop_back();
				mat[unlock_coord.second][unlock_coord.first].unlock();
				
				this_thread::sleep_for(std::chrono::milliseconds(350)); // Dmsiado rapido
				
			}
		}
		
		void draw(RenderWindow* w, int x_rect, int y_rect){
			RectangleShape r;
			r.setFillColor(color);
			r.setSize(Vector2f(x_rect-4, y_rect-4));
			r.setOutlineThickness(2);
			r.setOutlineColor(Color(240, 240, 240)); // Head
			for (int i = 0; i < coords.size(); i++) {
				r.setPosition(Vector2f(x_rect*coords[i].first+2, y_rect*coords[i].second+2));
				w->draw(r);
				r.setOutlineColor(Color(0,0,0));
			}
		}
		
		
};

