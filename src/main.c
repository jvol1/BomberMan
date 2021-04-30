#include "raylib.h"
#define MAX_OBSTACLES 28

typedef struct {
    Rectangle rec;
    Texture2D texture;
} Player;

typedef struct {
    Rectangle rec;
    Texture2D texture;
} Obstacle;


int main(void){
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    Player player;
    Obstacle obstacles[25];

    InitWindow(screenWidth, screenHeight, "IP - THE GAME");

    // As textura tem que ser inicializadas na main
    // Textures devem ser carregadas depois da inicializacao da Janela 
    player.texture = LoadTexture("C:\\Users\\Acer\\Desktop\\BomberMan\\assets\\moniMonstro1.png");
    Texture2D obstacleTexture = LoadTexture("C:\\Users\\Acer\\Desktop\\BomberMan\\assets\\caixa.png");


    InitPlayer(&player);
    InitObstacles(obstacles);


    Rectangle frameRec = { 0.0f, 0.0f, (float)player.texture.width/2, (float)player.texture.height/4};
    Vector2 currentFrame = {0 , 0};
    int framesCounter = 0;
    int framesSpeed = 3;            // Number of spritesheet frames shown by second

    SetTargetFPS(60);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        framesCounter++;
        UpdateGame(&player, obstacles, screenWidth, screenHeight);

        if (framesCounter >= (30/framesSpeed)){
            framesCounter = 0;
            currentFrame.x++;;

            if (currentFrame.x > 1) 
                currentFrame.x = 0;

            frameRec.x = (float) currentFrame.x * (float) player.texture.width / 2;
            frameRec.y = (float) currentFrame.y * (float) player.texture.height / 4;
        }
       


        BeginDrawing();

            ClearBackground(RAYWHITE);

            // DrawTextureRec(player.texture, frameRec, (Vector2) {player.rec.x, player.rec.y}, RAYWHITE);  // Draw part of the texture
            DrawRectangle(player.rec.x, player.rec.y, 100, 100, RED);

            for(int i = 0; i < MAX_OBSTACLES; i++)
                DrawRectangle(obstacles[i].rec.x, obstacles[i].rec.y, 128, 128, BLUE);

        EndDrawing();
    }



    UnloadTexture(player.texture);
    UnloadTexture(obstacleTexture);

    CloseWindow();

    return 0;
}

void UpdateGame(Player *playerPtr, Obstacle *obstacles, int screenWidth, int screenHeight){
    // Atualiza o jogo a cada frame

    if(IsKeyDown(KEY_RIGHT)){
        playerPtr->rec.x += 5;
        if(CheckCollisionMultipleRecs(*playerPtr, obstacles) == true)
            playerPtr->rec.x -= 5;
    }

    if(IsKeyDown(KEY_LEFT)){
        playerPtr->rec.x -= 5;
        if(CheckCollisionMultipleRecs(*playerPtr, obstacles) == true)
            playerPtr->rec.x += 5;
    }

    if(IsKeyDown(KEY_UP)){
        playerPtr->rec.y -= 5;
        if(CheckCollisionMultipleRecs(*playerPtr, obstacles) == true)
            playerPtr->rec.y += 5;
    }

    if(IsKeyDown(KEY_DOWN)){
        playerPtr->rec.y += 5;
        if(CheckCollisionMultipleRecs(*playerPtr, obstacles) == true)
            playerPtr->rec.y -= 5;
    }


    if (playerPtr->rec.x <= 0) playerPtr->rec.x = 0;
    if (playerPtr->rec.x + playerPtr->rec.width >= screenWidth) playerPtr->rec.x = screenWidth - playerPtr->rec.width;
    if (playerPtr->rec.y <= 0) playerPtr->rec.y = 0;
    if (playerPtr->rec.y + playerPtr->rec.height >= screenHeight) playerPtr->rec.y = screenHeight - playerPtr->rec.width;    
}

void InitPlayer(Player *playerPtr){
    // Inicializa o player 

    playerPtr->rec = (Rectangle) {0, 0, 100, 100};
}


void InitObstacles(Obstacle *obstacles){
    int i, j, k;

    k = 0;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 7; j++){
            obstacles[k].rec.x = 128 + 256 * j;
            obstacles[k].rec.y = 128 + 256 * i;
            obstacles[k].rec.height = 128;
            obstacles[k].rec.width = 128;
            k++;
        }
    }
}


int CheckCollisionMultipleRecs(Player player, Obstacle *obstacle){
    int i;

    for(i = 0; i < MAX_OBSTACLES; i++){
        if(CheckCollisionRecs(obstacle[i].rec, player.rec) == true)
            return true;
    }
    return false;
}