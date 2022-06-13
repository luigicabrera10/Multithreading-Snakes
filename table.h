#include "worm.h"
#include <vector>

class Table{
	
	private:
		RenderWindow* w;
		vector < pair<int, int> > used_pairs;
		int ss_x, ss_y;
		
		int x_rect, y_rect;
		
	public:	
		
		vector <Worm> worms;
		mutex** mat;
		int rows, columns;
	
		pair <int, int> rand_head(vector< pair<int, int> >& used){
			pair <int,int> head = make_pair(rand() % columns, rand() % rows);
			for (int i = 0; i < used.size(); i ++){
				if (head.first == used[i].first && head.second == used[i].second){
					head = make_pair(rand() % columns, rand() % rows); 
					i=-1;
				}
			}
			used.push_back(head);
			return head;
		}
		
		pair <int, int> rand_body(vector< pair<int, int> >& used, pair<int,int> last){
			
			bool find;
			pair <int,int> body[4];
			body[0] = make_pair(last.first-1, last.second); 
			body[1] = make_pair(last.first, last.second-1); 
			body[2] = make_pair((last.first+1)%columns, last.second); 
			body[3] = make_pair(last.first, (last.second+1)%rows); 
			
			if (body[0].first < 0) body[0].first = columns-1;
			if (body[1].second < 0) body[1].second = rows-1;
			
			int count = 0;		
			for (int i = rand()%4; count < 4; i=(i+1)%4, count ++){
				find = true;
				for (int j = used.size()-1; j >= 0; j--){
					if (body[i].first == used[j].first && body[i].second == used[j].second){
						find = false;
						break;
					}
				}
				if (find){
					used.push_back(body[i]);
					return body[i];
				}
			}
			/*  Es posible que durante la generacion aleatoria de un gusano, su cuerpo
				quede encerrado entre otro gusano o el mismo evitando que se genere un 
				cuerpo mas largo (Poco probable) (Muy probable si genera muchos gusanos)
				Pero implementar un algoritmo que evite esto sería muy tedioso y largo,
				en caso esto suceda solo reinicie el programa, gracias por su comprension :)
			*/
			assert(0);
			
			
		}
		
	
		Table(RenderWindow* window, int n_worms, int worms_length, int ss_x, int ss_y, int rows, int columns){
			
			assert(worms_length > 1 && n_worms > 0);
			
			this->w = window;
			this->ss_x = ss_x;
			this->ss_y = ss_y;
			
			this->rows = rows;
			this->columns = columns;
			
			this->x_rect = (int) ss_x / columns;
			this->y_rect = (int) ss_y / rows;
			
			// Init table of char and mutex
			mat = new mutex*[rows];
			for (int i = 0; i < rows; i ++) {
				mat[i] = new mutex[columns];
			}
			
			
			// Create worms
			srand (time(NULL));
			vector < pair <int, int> > used_pairs;
			deque < pair <int, int> > worm;
 			
			for (int i = 0; i < n_worms; i ++){
				worm.push_back(rand_head(used_pairs));
				for (int j = 0; j < worms_length-1; j++) worm.push_back(rand_body(used_pairs,*(worm.end()-1)));
				worms.push_back(Worm(worm));
				worm.clear();
			}
			cout << endl;
		}
		
		void draw(){
			for (int i = 0 ; i < worms.size(); i ++) worms[i].draw(w, x_rect, y_rect);
		}
		
		
		void send_thread(int indx){
			worms[indx].move_infinite(mat, rows, columns);
		}
		

};
