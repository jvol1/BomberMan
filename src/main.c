#include "raylib.h"

#define MAX_OBSTACLES 98
#define MAX_MONSTERS 2

typedef struct {
    int life;
    Rectangle rec;
    Texture2D texture;
}Player;

typedef struct {
    Rectangle rec;
    Texture2D texture;
}Obstacle;

typedef struct {
    bool active;
    int direction; // 1 direita, 2 esquerda, 3 cima, 4 baixo
    Rectangle rec;
    Texture2D texture;
}Monster;

void UpdateGame(Player *playerPtr, Obstacle *obstacles, Monster *monsters, int screenWidth, int screenHeight);
void InitPlayer(Player *playerPtr);
void InitObstacles(Obstacle *obstacles);
int CheckCollisionMultipleRecs(Rectangle rec, Obstacle *obstacle);


int main(void){
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    Player player;
    Obstacle obstacles[MAX_OBSTACLES];
    Monster monsters[MAX_MONSTERS];

    InitWindow(screenWidth, screenHeight, "IP - THE GAME");

    // As textura tem que ser inicializadas na main
    // Textures devem ser carregadas depois da inicializacao da Janela 
    player.texture = LoadTexture("C:\\Users\\Acer\\Desktop\\BomberMan\\assets\\moniMonstro1.png");
    Texture2D obstacleTexture = LoadTexture("C:\\Users\\Acer\\Desktop\\BomberMan\\assets\\caixa.png");


    Texture2D M2texture = LoadTexture("C:\\Users\\Acer\\Desktop\\BomberMan\\assets\\m2.png");
    Texture2D M3texture = LoadTexture("C:\\Users\\Acer\\Desktop\\BomberMan\\assets\\m3.png");


    InitPlayer(&player);
    InitObstacles(obstacles);
    InitMonsters(monsters, M2texture, M3texture);

    SetTargetFPS(60);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateGame(&player, obstacles, monsters, screenWidth, screenHeight);



        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawRectangle(player.rec.x, player.rec.y, player.rec.width, player.rec.height, RED);

            for(int i = 0; i < MAX_OBSTACLES; i++)
                DrawTexture(obstacleTexture, obstacles[i].rec.x, obstacles[i].rec.y, RAYWHITE);
            
            for(int i = 0; i < 1; i++){
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


    if (playerPtr->rec.x <= 0) playerPtr->rec.x = 0;
    if (playerPtr->rec.x + playerPtr->rec.width >= screenWidth) playerPtr->rec.x = screenWidth - playerPtr->rec.width;
    if (playerPtr->rec.y <= 0) playerPtr->rec.y = 0;
    if (playerPtr->rec.y + playerPtr->rec.height >= screenHeight) playerPtr->rec.y = screenHeight - playerPtr->rec.width;




    

    if (monsters[0].rec.x <= 0) monsters[0].rec.x = 0;
    if (monsters[0].rec.x + monsters[0].rec.width >= screenWidth) monsters[0].rec.x = screenWidth - monsters[0].rec.width;
    if (monsters[0].rec.y <= 0) monsters[0].rec.y = 0;
    if (monsters[0].rec.y + monsters[0].rec.height >= screenHeight) monsters[0].rec.y = screenHeight - monsters[0].rec.height;
}

void InitPlayer(Player *playerPtr){
    // Inicializa o player 
    playerPtr->life = 3;
    playerPtr->rec = (Rectangle) {0, 0, 64, 64};
}


void InitObstacles(Obstacle *obstacles){
    int i, j, k;

    k = 0;
    for(i = 0; i < 7; i++){
        for(j = 0; j < 14; j++){
            obstacles[k].rec.x = 128 + 128 * j;
            obstacles[k].rec.y = 128 + 128 * i;
            obstacles[k].rec.height = 60;
            obstacles[k].rec.width = 60;
            k++;
        }
    }
}

void InitMonsters(Monster *monsters, Texture2D M2texture, Texture2D M3texture){

    monsters[0].active = 1;
    monsters[0].rec.x = 0;
    monsters[0].rec.y = 0;
    monsters[0].rec.width = 60;
    monsters[0].rec.height = 60;
    monsters[0].texture = M2texture;
    

    monsters[1].active = 1;
    monsters[1].rec.x = 64;
    monsters[1].rec.y = 832;
    monsters[1].rec.width = 60;
    monsters[1].rec.height = 60;
    monsters[1].texture = M3texture;
}

int CheckCollisionMultipleRecs(Rectangle rec, Obstacle *obstacle){
    int i;

    for(i = 0; i < MAX_OBSTACLES; i++){
        if(CheckCollisionRecs(obstacle[i].rec, rec) == true)
            return true;
    }
    return false;
}


