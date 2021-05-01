#include "raylib.h"
#include <stdlib.h>
#define MAX_OBSTACLES 243
#define MAX_OBSTCLES_INDESTRUCTIBLE 181

#define MAX_MONSTERS 2

#ifdef WIN64
char* pathM2 = "..\\assets\\m2.png";
#else
char* pathM2 = "../assets/M2.png";
char* pathM3 = "../assets/M3.png";
char* pathMonimonstro = "../assets/moniMonstro1.png";
char* pathCaixa = "../assets/caixa.png";
char* pathCaixaMadeira = "../assets/caixa_madeira.png";
#endif

enum DIRECTIONS {UP = 1, DOWN, RIGHT, LEFT} ;
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

typedef struct{
    int destructible;
    int active;
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
void InitMonsters(Monster *monsters, Obstacle *obstacles, Texture2D M2texture, Texture2D M3texture);
int CheckCollisionMultipleRecs(Rectangle rec, Obstacle *obstacle);
int CheckCollisionMultipleRecsDes(Rectangle rec, Obstacle *obstacle);
int findShortestPath(Player player, Monster monster);
void createBomb(Player *player);

int main(){
    int screenWidth = 1920;
    int screenHeight = 1080;
    
    Player player;
    Obstacle obstacles[MAX_OBSTACLES];
    Monster monsters[MAX_MONSTERS];

    InitWindow(screenWidth, screenHeight, "IP - THE GAME");
    // ToggleFullscreen();
    // As textura tem que ser inicializadas na main
    // Textures devem ser carregadas depois da inicializacao da Janela 
    player.texture = LoadTexture("./../assets/moniMonstro1.png");
    Texture2D obstacleTexture = LoadTexture(pathCaixa);

    Texture2D M2texture = LoadTexture(pathM2);
    Texture2D M3texture = LoadTexture(pathM3);
    Texture2D obstacleTextureDes = LoadTexture(pathCaixaMadeira); // obstaclesTextureDestructible 

    InitPlayer(&player, screenWidth, screenHeight);
    InitObstacles(obstacles);
    InitMonsters(monsters, obstacles, M2texture, M3texture);

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

            for(int i = 0; i < MAX_OBSTACLES; i++){
                if(obstacles[i].destructible == 0)
                    DrawTexture(obstacleTexture, obstacles[i].rec.x, obstacles[i].rec.y, RAYWHITE);
                else
                    DrawTexture(obstacleTextureDes, obstacles[i].rec.x, obstacles[i].rec.y, RAYWHITE);
            }
            
            for(int i = 0; i < MAX_MONSTERS; i++){
                DrawRectangle(monsters[i].rec.x, monsters[i].rec.y, monsters[i].rec.width, monsters[i].rec.height, GREEN);
            }
        EndDrawing();
    }



    UnloadTexture(player.texture);
    UnloadTexture(obstacleTexture);
    UnloadTexture(obstacleTextureDes);
    UnloadTexture(M2texture);
    UnloadTexture(M3texture);
    
    CloseWindow();

    return 0;
}

void UpdateGame(Player *playerPtr, Obstacle *obstacles, Monster *monsters, int screenWidth, int screenHeight){
    // Atualiza o jogo a cada frame

    if(IsKeyDown(KEY_RIGHT)){
        playerPtr->rec.x += 4;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles) == true)
            playerPtr->rec.x -= 4;
    }

    if(IsKeyDown(KEY_LEFT)){
        playerPtr->rec.x -= 4;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles) == true)
            playerPtr->rec.x += 4;
    }

    if(IsKeyDown(KEY_UP)){
        playerPtr->rec.y -= 4;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles) == true)
            playerPtr->rec.y += 4;
    }

    if(IsKeyDown(KEY_DOWN)){
        playerPtr->rec.y += 4;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles) == true)
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
    	if (direction == LEFT){
    		monsters[i].rec.x -= 4;
    		if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles)){
    			monsters[i].rec.x += 4;
    		}
    	}
    	else if (direction == RIGHT){
    		monsters[i].rec.x += 4;
    		if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles)){
    			monsters[i].rec.x -= 4;
    		}
    	}
    	else if (direction == UP){
    		monsters[i].rec.y -= 4;
    		if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles)){
    			monsters[i].rec.y += 4;
    		}
    	}
  	    else if (direction == DOWN){
   		    monsters[i].rec.y += 4;
   		    if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles)){
    			monsters[i].rec.y -= 4;
    		}
    	}

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
    playerPtr->rec = (Rectangle) {64, 64, 64, 64};
    playerPtr->num_bombs = 0;
    playerPtr->bombs = NULL;
}


void InitObstacles(Obstacle *obstacles){
    int i, j, k;

    k = 0;
    for(i = 0; i < 7; i++){
        for(j = 0; j < 13; j++){
            obstacles[k].rec.x = 128 + 128 * j;
            obstacles[k].rec.y = 128 + 128 * i;
            obstacles[k].rec.height = 60;
            obstacles[k].rec.width = 60;
            obstacles[k].destructible = 0;
            k++;
        }
    }

    for(i = 0; i < 30; i++){
        obstacles[k].rec.x = i * 64;
        obstacles[k].rec.y = 0;
        obstacles[k].rec.height = 60;
        obstacles[k].rec.width = 60;
        obstacles[k].destructible = 0;
        k++;
    }

    for(i = 0; i < 30; i++){
        obstacles[k].rec.x = i * 64;
        obstacles[k].rec.y = 1080 - 60;
        obstacles[k].rec.height = 60;
        obstacles[k].rec.width = 60;
        obstacles[k].destructible = 0;
        k++;
    }

    for(i = 1; i < 16; i++){
        obstacles[k].rec.x = 0;
        obstacles[k].rec.y = i * 64;
        obstacles[k].rec.height = 60;
        obstacles[k].rec.width = 60;
        obstacles[k].destructible = 0;
        k++;
    }

    for(i = 1; i < 16; i++){
        obstacles[k].rec.x = 1920 - 60;
        obstacles[k].rec.y = i * 64;
        obstacles[k].rec.height = 60;
        obstacles[k].rec.width = 60;
        obstacles[k].destructible = 0;
        k++;
    }

    int randIntX;
    int randIntY;
    for(i = 1; i < 61; i++){  // 60 objetos destrutives
        randIntX = GetRandomValue(2, 28);
        randIntY = GetRandomValue(2, 13);
        obstacles[k].rec.x = 64 * randIntX;
        obstacles[k].rec.y = 64 * randIntY;
        obstacles[k].rec.height = 60;
        obstacles[k].rec.width = 60;
        obstacles[k].destructible = 1;
        obstacles[k].active = 1;

        while(CheckCollisionMultipleRecsDes(obstacles[k].rec,obstacles) == true){
            randIntX = GetRandomValue(2, 28);
            randIntY = GetRandomValue(2, 13);
            obstacles[k].rec.x = 64 * randIntX;
            obstacles[k].rec.y = 64 * randIntY;
        }
        k++;
    }
}

void InitMonsters(Monster *monsters, Obstacle *obstcles, Texture2D M2texture, Texture2D M3texture){

    int RandIntX;
    int RandIntY;
    int i;

    monsters[0].active = 1;
    RandIntX = GetRandomValue(2, 28);
    RandIntY = GetRandomValue(2, 13);
    monsters[0].rec.x = 64 * RandIntX;
    monsters[0].rec.y = 64 * RandIntY;
    monsters[0].rec.width = 60;
    monsters[0].rec.height = 60;

    while(CheckCollisionMultipleRecs(monsters[0].rec, obstcles) == true){
        RandIntX = GetRandomValue(2, 28);
        RandIntY = GetRandomValue(2, 13);
        monsters[0].rec.x = 64 * RandIntX;
        monsters[0].rec.y = 64 * RandIntY;
    }
}

//Criar função de colisão geral
/*
int CheckCollisionMultipleRecs(Rectangle rec, Obstacle *obstacle, Monster *monsters, Type TYPE){
    int i;

    for(i = 0; i < MAX_OBSTACLES; i++){
        if(CheckCollisionRecs(obstacle[i].rec, rec) == true)
            return true;
    }/*
    if (TYPE == MONSTER){
    	for (i = 0; i < MAX_MONSTERS; i++){
		if(CheckCollisionRecs(monsters[i].rec, rec) == true && monsters[i].id)
	            return true;
	}
    }
    return false;
}
*/

void createBomb(Player *playerPtr){
    int num = ++playerPtr->num_bombs;
    playerPtr->bombs = realloc(playerPtr->bombs, sizeof(Bomb) * num);
    playerPtr->bombs[num-1] = (Bomb) { .exploded = false, .range = 5, 
        .rec = (Rectangle) {.height = 40, .width = 40, .x = playerPtr->rec.x, .y = playerPtr->rec.y} 
        };
}

int CheckCollisionMultipleRecs(Rectangle rec, Obstacle *obstacle){
    int i;
    for(i = 0; i < MAX_OBSTACLES; i++){
        if(CheckCollisionRecs(obstacle[i].rec, rec) == true)
            return true;
    }
    return false;
}

int CheckCollisionMultipleRecsDes(Rectangle rec, Obstacle *obstacle){
    int i;

    for(i = 0; i < MAX_OBSTCLES_INDESTRUCTIBLE; i++){
        if(CheckCollisionRecs(obstacle[i].rec, rec) == true)
            return true;
    }
    return false;
}