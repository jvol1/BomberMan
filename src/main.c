#include "raylib.h"
#include <stdlib.h>

#define MAX_OBSTACLES 98
#define MAX_MONSTERS 2
#define SIZE_REC 60

#ifdef WIN64
char* pathM2 = "..\\assets\\m2.png";
#else
char pathM2[50] = "./assets/M2.png";
char pathM3[50] = "./assets/M3.png";
char pathMonimonstro[50] = "./assets/moniMonstro1.png";
char pathCaixa[50] = "./assets/caixa.png";
#endif

enum DIRECTIONS {UP = 1, DOWN, RIGHT, LEFT};
typedef enum Type {PLAYER, MONSTER, OBSTACLE} Type;


typedef struct {
    bool exploded;
    int range;
    Texture2D texture;
    Rectangle rec; 
} Bomb;

typedef struct {
    int life;
    Rectangle rec;
    Texture2D texture;
    Bomb *bombs;
    int num_bombs;
}Player;

typedef struct {
    Rectangle rec;
    Texture2D texture;
}Obstacle;

typedef struct {
    bool active;
    int id;
    int direction;
    Rectangle rec;
    Texture2D texture;
}Monster;



void UpdateGame(Player *playerPtr, Obstacle *obstacles, Monster *monsters, int screenWidth, int screenHeight);
void InitPlayer(Player *playerPtr, int screenWidth, int screenHeight);
void InitObstacles(Obstacle *obstacles);
void InitMonsters(Monster *monsters, Texture2D M2texture, Texture2D M3texture, int screenWidth, int screenHeight);
int CheckCollisionMultipleRecs(Rectangle rec, Obstacle *obstacle, Monster *monsters, Type TYPE, int id);
int findShortestPath(Player player, Monster monster);
void createBomb(Player *player);

int main(){
    int screenWidth = 1920;
    int screenHeight = 1080;
    
    Player player;
    Obstacle obstacles[MAX_OBSTACLES];
    Monster monsters[MAX_MONSTERS];

    InitWindow(screenWidth, screenHeight, "IP - THE GAME");
    ToggleFullscreen();
    // As textura tem que ser inicializadas na main
    // Textures devem ser carregadas depois da inicializacao da Janela 
    player.texture = LoadTexture(pathMonimonstro);
    Texture2D obstacleTexture = LoadTexture(pathCaixa);

    Texture2D M2texture = LoadTexture(pathM2);
    Texture2D M3texture = LoadTexture(pathM3);

    InitPlayer(&player, screenWidth, screenHeight);
    InitObstacles(obstacles);
    InitMonsters(monsters, M2texture, M3texture, screenWidth, screenHeight);

    SetTargetFPS(60);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
            UpdateGame(&player, obstacles, monsters, screenWidth, screenHeight);
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawRectangle(player.rec.x, player.rec.y, player.rec.width, player.rec.height, RED);
            for(int i = 0; i < player.num_bombs; i++){
                DrawRectangle(player.bombs[i].rec.x, player.bombs[i].rec.y, player.bombs[i].rec.width, player.bombs[i].rec.height, YELLOW);
            }

            for(int i = 0; i < MAX_OBSTACLES; i++)
                DrawTexture(obstacleTexture, obstacles[i].rec.x, obstacles[i].rec.y, RAYWHITE);
            
            for(int i = 0; i < MAX_MONSTERS; i++){
                DrawRectangle(monsters[i].rec.x, monsters[i].rec.y, monsters[i].rec.width, monsters[i].rec.height, GREEN);
            }
        EndDrawing();
    }



    UnloadTexture(player.texture);
    UnloadTexture(obstacleTexture);
    UnloadTexture(M2texture);
    UnloadTexture(M3texture);

    CloseWindow();

    return 0;
}

void UpdateGame(Player *playerPtr, Obstacle *obstacles, Monster *monsters, int screenWidth, int screenHeight){
    // Atualiza o jogo a cada frame
    int randomKey;

    if(IsKeyDown(KEY_RIGHT)){
        playerPtr->rec.x += 4;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles, monsters, PLAYER, -1) == true)
            playerPtr->rec.x -= 4;
    }

    if(IsKeyDown(KEY_LEFT)){
        playerPtr->rec.x -= 4;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles, monsters, PLAYER, -1) == true)
            playerPtr->rec.x += 4;
    }

    if(IsKeyDown(KEY_UP)){
        playerPtr->rec.y -= 4;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles, monsters, PLAYER, -1) == true)
            playerPtr->rec.y += 4;
    }

    if(IsKeyDown(KEY_DOWN)){
        playerPtr->rec.y += 4;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles, monsters, PLAYER, -1) == true)
            playerPtr->rec.y -= 4;
    }
    if (IsKeyDown(KEY_ENTER)){
        createBomb(playerPtr);
    }



    if (playerPtr->rec.x <= 0) playerPtr->rec.x = 0;
    if (playerPtr->rec.x + playerPtr->rec.width >= screenWidth) playerPtr->rec.x = screenWidth - playerPtr->rec.width;
    if (playerPtr->rec.y <= 0) playerPtr->rec.y = 0;
    if (playerPtr->rec.y + playerPtr->rec.height >= screenHeight) playerPtr->rec.y = screenHeight - playerPtr->rec.width;
    
    int i;
    for (i = 0; i < MAX_MONSTERS; i++){
    	int direction = findShortestPath(*playerPtr, monsters[i]);
        int id = monsters[i].id;
    	if (direction == LEFT){
    		monsters[i].rec.x -= 4;
    		if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles, monsters, MONSTER, id)){
    			monsters[i].rec.x += 4;
    		}
    	}
    	else if (direction == RIGHT){
    		monsters[i].rec.x += 4;
    		if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles, monsters, MONSTER, id)){
    			monsters[i].rec.x -= 4;
    		}
    	}
    	else if (direction == UP){
    		monsters[i].rec.y -= 4;
    		if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles, monsters, MONSTER, id)){
    			monsters[i].rec.y += 4;
    		}
    	}
  	else if (direction == DOWN){
   		monsters[i].rec.y += 4;
   		if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles, monsters, MONSTER, id)){
    			monsters[i].rec.y -= 4;
    		}
    	}
    	if (monsters[i].rec.x <= 0) monsters[0].rec.x = 0;
    	if (monsters[i].rec.x + monsters[i].rec.width >= screenWidth) monsters[i].rec.x = screenWidth - monsters[i].rec.width;
    	if (monsters[i].rec.y <= 0) monsters[i].rec.y = 0;
    	if (monsters[i].rec.y + monsters[i].rec.height >= screenHeight) monsters[i].rec.y = screenHeight - monsters[i].rec.height;
    }
}

int findShortestPath(Player player, Monster monster){
	int delta_x = monster.rec.x - player.rec.x ;
	int delta_y = monster.rec.y - player.rec.y ;
	if (abs(delta_x) >= abs(delta_y)) {
		if (delta_x >= 0)
			return LEFT;
		else return RIGHT;
	}
	else {
		if (delta_y >= 0) 
			return UP;
		else return DOWN;
	}
}

void InitPlayer(Player *playerPtr, int screenWidth, int screenHeight){
    // Inicializa o player 
    playerPtr->life = 3;
    playerPtr->rec = (Rectangle) {500, 10, 64, 64};
    playerPtr->num_bombs = 0;
    playerPtr->bombs = NULL;
}


void InitObstacles(Obstacle *obstacles){
    int i, j, k;

    k = 0;
    for(i = 0; i < 7; i++){
        for(j = 0; j < 14; j++){
            obstacles[k].rec.x = 128 + 128 * j;
            obstacles[k].rec.y = 128 + 128 * i;
            obstacles[k].rec.height = SIZE_REC;
            obstacles[k].rec.width = SIZE_REC;
            k++;
        }
    }
}

void InitMonsters(Monster *monsters, Texture2D M2texture, Texture2D M3texture, int screenWidth, int screenHeight){

    monsters[0].active = 1;
    monsters[0].rec.x = 10;
    monsters[0].rec.y = 10;
    monsters[0].rec.width = SIZE_REC;
    monsters[0].rec.height = SIZE_REC;
    monsters[0].texture = M2texture;
    monsters[0].id = 1;
    

    monsters[1].active = 1;
    monsters[1].rec.x = 1500;
    monsters[1].rec.y = 10;
    monsters[1].rec.width = SIZE_REC;
    monsters[1].rec.height = SIZE_REC;
    monsters[1].texture = M3texture;
    monsters[1].id = 2;
}

//Criar função de colisão geral
int CheckCollisionMultipleRecs(Rectangle rec, Obstacle *obstacle, Monster *monsters, Type TYPE, int id){
    int i;

    for(i = 0; i < MAX_OBSTACLES; i++){
        if(CheckCollisionRecs(obstacle[i].rec, rec) == true)
            return true;
    }
    if (TYPE == MONSTER){
    	for (i = 0; i < MAX_MONSTERS; i++){
		if(CheckCollisionRecs(monsters[i].rec, rec) == true && monsters[i].id != id)
	            return true;

	}
    }
    return false;
}


void createBomb(Player *playerPtr){
    int num = ++playerPtr->num_bombs;
    playerPtr->bombs = realloc(playerPtr->bombs, sizeof(Bomb) * num);
    playerPtr->bombs[num-1] = (Bomb) { .exploded = false, .range = 5, 
        .rec = (Rectangle) {.height = 40, .width = 40, .x = playerPtr->rec.x, .y = playerPtr->rec.y} 
        };
}