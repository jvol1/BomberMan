#include "raylib.h"

typedef struct {
    Vector2 position;
    Texture2D texture;
} Player;

typedef struct {
    int x;
    int y;
}Frame;


typedef struct {
    bool destructible;
    Vector2 position;
    int tam;
}Obstacle;


int main(void){
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    Player player;
    Obstacle obstacles[30];


    InitWindow(screenWidth, screenHeight, "IP - THE GAME");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    player.texture = LoadTexture("C:\\Users\\Acer\\Desktop\\GAME IP\\assets\\moniMonstro1.png"); // Texture loading // Hard Coded 
    Texture2D obstacleTexture = LoadTexture("C:\\Users\\Acer\\Desktop\\GAME IP\\assets\\caixa.png");


    Rectangle frameRec = { 0.0f, 0.0f, (float)player.texture.width/2, (float)player.texture.height/4};
    Frame currentFrame = {0 , 0};
    int framesCounter = 0;
    int framesSpeed = 3;            // Number of spritesheet frames shown by second

    SetTargetFPS(30);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        framesCounter++;

        if(IsKeyDown(KEY_RIGHT)){

            player.position.x += 5.0f;
            currentFrame.y = 0;       
        }
        if(IsKeyDown(KEY_LEFT)){
            player.position.x -= 5.0f;
            currentFrame.y = 1;       
        }
        if(IsKeyDown(KEY_DOWN)){
            player.position.y += 5.0f;
            currentFrame.y = 2;       
        }
        if(IsKeyDown(KEY_UP)){
            player.position.y -= 5.0f;
            currentFrame.y = 3; 
        }


        if (framesCounter >= (30/framesSpeed)){
            framesCounter = 0;
            currentFrame.x++;;

            if (currentFrame.x > 1) 
                currentFrame.x = 0;

            frameRec.x = (float) currentFrame.x * (float) player.texture.width / 2;
            frameRec.y = (float) currentFrame.y * (float) player.texture.height / 4;
        }

        /*
        // Wall behaviour
        if (player.rec.x <= 0) player.rec.x = 0;
        if (player.rec.x + player.rec.width >= screenWidth) player.rec.x = screenWidth - player.rec.width;
        if (player.rec.y <= 0) player.rec.y = 0;
        if (player.rec.y + player.rec.height >= screenHeight) player.rec.y = screenHeight - player.rec.height;
        */
       
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTextureRec(player.texture, frameRec, player.position, WHITE);  // Draw part of the texture
            DrawTexture(obstacleTexture, obstacles[0].position.x, obstacles[0].position.y, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(player.texture);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}