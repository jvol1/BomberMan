#include <stdlib.h>
#include "raylib.h"

#define RECT (60)
#define MAX_OBSTACLES (243)
#define MAX_OBSTCLES_INDESTRUCTIBLE (181)
#define MAX_MONSTERS 4
#define SECOND (60)
#define SPEED (4)

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
typedef enum Type {PLAYER = 5, MONSTER, OBSTACLE, BOMB} Type;

typedef struct {
    bool exploded;
    int range;
    Texture2D texture;
    Rectangle rec; 
    int tempo;
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
    bool active;
    bool Out;
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
int CheckCollisionMultipleRecs(Rectangle rec, Obstacle *obstacle, Type t);
int CheckCollisionMultipleRecsDes(Rectangle rec, Obstacle *obstacle, Type t);
int findShortestPath(Player player, Monster monster);
void createBomb(Player *player);
void controlMonsters(Player *playerPtr, Monster* monsters, Obstacle *obstacles);
void controlBombs(Player *playerPtr, Obstacle *obstacles, Monster *monsters);
bool checkCollisionMonsters(Rectangle rec, Monster *monsters, int id, Type t);

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
                if (!player.bombs[i].exploded){
                    DrawRectangle(player.bombs[i].rec.x, player.bombs[i].rec.y, player.bombs[i].rec.width, player.bombs[i].rec.height, YELLOW);
                }
            }

            for(int i = 0; i < MAX_OBSTACLES; i++){
                if(!obstacles[i].destructible)
                    DrawTexture(obstacleTexture, obstacles[i].rec.x, obstacles[i].rec.y, RAYWHITE);
                else if (obstacles[i].active)
                    DrawTexture(obstacleTextureDes, obstacles[i].rec.x, obstacles[i].rec.y, RAYWHITE);
            }
            
            for(int i = 0; i < MAX_MONSTERS && monsters[i].active == true; i++){
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
        playerPtr->rec.x += SPEED;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles, PLAYER) == true)
            playerPtr->rec.x -= SPEED;
    }

    if(IsKeyDown(KEY_LEFT)){
        playerPtr->rec.x -= SPEED;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles, PLAYER) == true)
            playerPtr->rec.x += SPEED;
    }

    if(IsKeyDown(KEY_UP)){
        playerPtr->rec.y -= SPEED;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles, PLAYER) == true)
            playerPtr->rec.y += SPEED;
    }

    if(IsKeyDown(KEY_DOWN)){
        playerPtr->rec.y += SPEED;
        if(CheckCollisionMultipleRecs(playerPtr->rec, obstacles, PLAYER) == true)
            playerPtr->rec.y -= SPEED;
    }
    if (IsKeyDown(KEY_ENTER)){
        createBomb(playerPtr);
    }

    if (playerPtr->rec.x <= 0) playerPtr->rec.x = 0;
    if (playerPtr->rec.x + playerPtr->rec.width >= screenWidth) playerPtr->rec.x = screenWidth - playerPtr->rec.width;
    if (playerPtr->rec.y <= 0) playerPtr->rec.y = 0;
    if (playerPtr->rec.y + playerPtr->rec.height >= screenHeight) playerPtr->rec.y = screenHeight - playerPtr->rec.width;
    
    controlMonsters(playerPtr, monsters, obstacles);

    controlBombs(playerPtr, obstacles, monsters);
}

void Explosion(Bomb bomb, Obstacle *Obstacles, Monster *monsters){
    int i, collision = 0;
    //direita da bomba
    for (i = 0; i < bomb.range && collision == 0; i++){
        Rectangle explosion = {.height = RECT, .width = RECT, .x = bomb.rec.x + (i * RECT), .y = bomb.rec.y};
        bool colMonster = checkCollisionMonsters(explosion, monsters, -1, BOMB);
        bool colObstacle = CheckCollisionMultipleRecs(explosion, Obstacles, BOMB);
        bool colObstacleDes = CheckCollisionMultipleRecsDes(explosion, Obstacles, BOMB);

        if ( !( colMonster | colObstacle | colObstacleDes) ){
            DrawRectangle(explosion.x, explosion.y, explosion.width, explosion.height, YELLOW);
        }
        else collision = 1;
    }
    collision = 0;
    //esquerda da bomba
    for (i = 0; i < bomb.range && collision == 0; i++){
        Rectangle explosion = {.height = RECT, .width = RECT, .x = bomb.rec.x - (i * RECT), .y = bomb.rec.y};
        bool colMonster = checkCollisionMonsters(explosion, monsters, -1, BOMB);
        bool colObstacle = CheckCollisionMultipleRecs(explosion, Obstacles, BOMB);
        bool colObstacleDes = CheckCollisionMultipleRecsDes(explosion, Obstacles, BOMB);

        if ( !( colMonster | colObstacle | colObstacleDes) ){
            DrawRectangle(explosion.x, explosion.y, explosion.width, explosion.height, YELLOW);
        }
        else collision = 1;
    }
    collision = 0;
    //cima da bomba
    for (i = 0; i < bomb.range && collision == 0; i++){
        Rectangle explosion = {.height = RECT, .width = RECT, .x = bomb.rec.x, .y = bomb.rec.y + (i * RECT)};
        bool colMonster = checkCollisionMonsters(explosion, monsters, -1, BOMB);
        bool colObstacle = CheckCollisionMultipleRecs(explosion, Obstacles, BOMB);
        bool colObstacleDes = CheckCollisionMultipleRecsDes(explosion, Obstacles, BOMB);

        if ( !( colMonster | colObstacle | colObstacleDes) ){
            DrawRectangle(explosion.x, explosion.y, explosion.width, explosion.height, YELLOW);
        }
        else collision = 1;
    }
    collision = 0;
    //baixo da bomba
    for (i = 0; i < bomb.range && collision == 0; i++){
        Rectangle explosion = {.height = RECT, .width = RECT, .x = bomb.rec.x, .y = bomb.rec.y - (i * RECT)};
        bool colMonster = checkCollisionMonsters(explosion, monsters, -1, BOMB);
        bool colObstacle = CheckCollisionMultipleRecs(explosion, Obstacles, BOMB);
        bool colObstacleDes = CheckCollisionMultipleRecsDes(explosion, Obstacles, BOMB);

        if ( !( colMonster | colObstacle | colObstacleDes) ){
            DrawRectangle(explosion.x, explosion.y, explosion.width, explosion.height, YELLOW);
        }
        else collision = 1;
    }
}

void controlBombs(Player *playerPtr, Obstacle *obstacles, Monster *monsters){
    register int i;
    for (i = 0; i < playerPtr->num_bombs; i++){
        if ( !(playerPtr->bombs[i]).exploded ){
            playerPtr->bombs[i].tempo += 1;
            if (playerPtr->bombs[i].tempo > 3*SECOND){
                Explosion(playerPtr->bombs[i], obstacles, monsters);
                playerPtr->bombs[i].exploded = true;
            }
        }
    }
}

void controlMonsters(Player *playerPtr, Monster* monsters, Obstacle *obstacles){
    int i;
    for (i = 0; i < MAX_MONSTERS; i++){
    	int direction = findShortestPath(*playerPtr, monsters[i]);

    	if (direction == LEFT){
    		monsters[i].rec.x -= SPEED;
    		if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles, PLAYER) || checkCollisionMonsters(monsters[i].rec, monsters, monsters[i].id, MONSTER)){
    			monsters[i].rec.x += SPEED;
    		}
    	}
    	else if (direction == RIGHT){
    		monsters[i].rec.x += SPEED;
    		if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles,PLAYER) || checkCollisionMonsters(monsters[i].rec, monsters, monsters[i].id, MONSTER)){
    			monsters[i].rec.x -= SPEED;
    		}
    	}
    	else if (direction == UP){
    		monsters[i].rec.y -= SPEED;
    		if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles, PLAYER) || checkCollisionMonsters(monsters[i].rec, monsters, monsters[i].id, MONSTER)){
    			monsters[i].rec.y += SPEED;
    		}
    	}
  	    else if (direction == DOWN){
   		    monsters[i].rec.y += SPEED;
   		    if (CheckCollisionMultipleRecs(monsters[i].rec, obstacles, PLAYER) || checkCollisionMonsters(monsters[i].rec, monsters, monsters[i].id, MONSTER)){
    			monsters[i].rec.y -= SPEED;
    		}
    	}
    }
}
int findShortestPath(Player player, Monster monster){
	int delta_x = monster.rec.x - player.rec.x ;
	int delta_y = monster.rec.y - player.rec.y ;
	if (abs(delta_x) >= abs(delta_y)) {
		if (delta_x >= 0){
            return LEFT;
        }
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
    playerPtr->rec = (Rectangle) {RECT, RECT, 64, 64};
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
            obstacles[k].rec.height = RECT;
            obstacles[k].rec.width = RECT;
            obstacles[k].destructible = 0;
            obstacles[k].active = true;
            k++;
        }
    }

    for(i = 0; i < 30; i++){
        obstacles[k].rec.x = i * 64;
        obstacles[k].rec.y = 0;
        obstacles[k].rec.height = RECT;
        obstacles[k].rec.width = RECT;
        obstacles[k].destructible = 0;
        obstacles[k].active = true;
        k++;
    }

    for(i = 0; i < 30; i++){
        obstacles[k].rec.x = i * 64;
        obstacles[k].rec.y = 1080 - 60;
        obstacles[k].rec.height = RECT;
        obstacles[k].rec.width = RECT;
        obstacles[k].destructible = 0;
        obstacles[k].active = true;
        k++;
    }

    for(i = 1; i < 16; i++){
        obstacles[k].rec.x = 0;
        obstacles[k].rec.y = i * 64;
        obstacles[k].rec.height = RECT;
        obstacles[k].rec.width = RECT;
        obstacles[k].destructible = 0;
        obstacles[k].active = true;
        k++;
    }

    for(i = 1; i < 16; i++){
        obstacles[k].rec.x = 1920 - 60;
        obstacles[k].rec.y = i * 64;
        obstacles[k].rec.height = RECT;
        obstacles[k].rec.width = RECT;
        obstacles[k].destructible = 0;
        obstacles[k].active = true;
        k++;
    }

    int randIntX;
    int randIntY;
    for(i = 1; i < 61; i++){  // 60 objetos destrutives
        randIntX = GetRandomValue(2, 28);
        randIntY = GetRandomValue(2, 13);
        obstacles[k].rec.x = 64 * randIntX;
        obstacles[k].rec.y = 64 * randIntY;
        obstacles[k].rec.height = RECT;
        obstacles[k].rec.width = RECT;
        obstacles[k].destructible = 1;
        obstacles[k].active = true;

        while(CheckCollisionMultipleRecsDes(obstacles[k].rec, obstacles, OBSTACLE) == true){
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

    for (i = 0; i < MAX_MONSTERS; i++){
        monsters[i].active = true;
        monsters[i].rec.width = RECT;
        monsters[i].rec.height = RECT;
        monsters[i].id = i+1;

        while(CheckCollisionMultipleRecs(monsters[i].rec, obstcles, MONSTER) == true){
            RandIntX = GetRandomValue(2, 28);
            RandIntY = GetRandomValue(2, 13);
            monsters[i].rec.x = 64 * RandIntX;
            monsters[i].rec.y = 64 * RandIntY;
        }
    }
}

void createBomb(Player *playerPtr){
    int num = ++playerPtr->num_bombs;
    playerPtr->bombs = realloc(playerPtr->bombs, sizeof(Bomb) * num);
    playerPtr->bombs[num-1] = (Bomb) { .exploded = false, .range = 9, 
        .rec = (Rectangle) {.height = RECT, .width = RECT, .x = playerPtr->rec.x, .y = playerPtr->rec.y} 
        , .tempo = 0};
}

int CheckCollisionMultipleRecs(Rectangle rec, Obstacle *obstacle, Type t){
    int i;
    for(i = 0; i < MAX_OBSTACLES; i++){
        if (!obstacle[i].active) continue;
        if(CheckCollisionRecs(obstacle[i].rec, rec) == true){
            if (t == BOMB && obstacle[i].destructible){
                obstacle[i].active = false;
            }
            return true;
        }
    }
    return false;
}

int CheckCollisionMultipleRecsDes(Rectangle rec, Obstacle *obstacle, Type t){
    int i;
    for(i = 0; i < MAX_OBSTCLES_INDESTRUCTIBLE; i++){
        if (!obstacle[i].active) continue;
        if(CheckCollisionRecs(obstacle[i].rec, rec) == true){
            /*if (t == BOMB){
                obstacle[i].active = false;
            }*/
            return true;
        }
    }
    return false;
}

bool checkCollisionMonsters(Rectangle rec, Monster *monsters, int id, Type t){
    //id = -1 for other types
    int i;
    for(i = 0; i < MAX_MONSTERS; i++){
        if(CheckCollisionRecs(rec, monsters[i].rec) == true && (id != monsters[i].id)){
            if (t == BOMB){
                monsters[i].active = false;
            }
            return true;
        }
    }
    return false;
}



