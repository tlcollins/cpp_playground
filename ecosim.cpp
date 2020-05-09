#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <ncurses.h>
#include <list>
#include <string>

using namespace std;

class prof {
  public:
    int a = rand() % 101; 
    int b = rand() % 101;
    
    int abs_dif = abs(a - b);
    int abs_score = abs( abs_dif - 100);
    int score_mult = abs_score / 4;
    int dif = a - b;
    int score = score_mult;  

    void display() {
      printf("|%10d |%10d |%10d |%10d |%10d |%10d \n", a, b, abs_dif, -dif, score_mult, score);
    }
};

int main(int argc, char* argv[]) {
  srand(time(NULL));
  list<prof> prof_objects;
  
  for (int x=0; x<10; x++)
     prof_objects.emplace_back();
  
  printf("|%10s |%10s |%10s |%10s |%10s |%10s \n", "Self", "Comp", "Dist", "Diff", "Score", "AccTotal");
  for (auto it : prof_objects)
    it.display();
  
  printf("\n\n");
  return 0;
}  //test
