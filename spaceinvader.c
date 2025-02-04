#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define LARGURA_JANELA 600
#define ALTURA_JANELA 800
#define STD_SIZE_X 32
#define STD_SIZE_Y 32
#define LARGURA_BALA 10
#define ALTURA_BALA 15

typedef struct Menu {
    int pause; // 0 jogo rodando, 1 tela inicial, 2 jogo pausado, 3 fim de jogo
}Menu;

typedef struct Bala{
    Rectangle pos;
    Color color;
    int ativa;
    int tempo;
    int velocidade;
    Sound tiro;
}Bala;

typedef struct Barreira{
    Rectangle pos;
    Color color;
    int vida;
}Barreira;

typedef struct Nave{
    Rectangle pos;
    Color color;
    Bala bala;
    int recarga;
    int vida;
}Nave;

typedef struct Heroi{
    Texture2D sprites[4];
    Texture2D skin;
    Rectangle pos;
    Color color;
    Bala bala;
    int velocidade;
    int colisaoDireita;
    int colisaoEsquerda;
    int vidas;
}Heroi;

typedef struct Bordas{
    Rectangle pos;
} Bordas;

typedef struct Assets{
    Texture2D naveVerde;
    Sound tiro;
}Assets;

typedef struct Jogo{
    Nave nave[4][7];
    Heroi heroi;
    Bordas bordas[4];
    Assets assets;
    Barreira barreira[3][5];
    Menu menu;
    double velocidadeNaves;
    int alturaJanela;
    int larguraJanela;
    int tempoAnimacao;
    int dificuldade;
}Jogo;

void IniciaJogo(Jogo *j);
void IniciaNaves(Jogo *j);
void AtualizaJogo(Jogo *j);
void DesenhaJogo(Jogo *j);
void AtualizaFrameDesenho(Jogo *j);
void AtualizaNavePos(Jogo *j, int i, int k);
void DesenhaNaves(Jogo *j);
void DesenhaHeroi(Jogo *j);
void ColisaoBordas(Jogo *j);
void DesenhaBordas(Jogo *j);
void ColisaoBalas(Jogo *j, int i, int k);
void DesenhaBordas(Jogo *j);
void AtiraBalas(Jogo *j, int i, int k);
void CarregaImagens(Jogo *j);
void DescarregaImagens(Jogo *j);
void IniciaHeroi(Jogo *j);
void IniciaBordas(Jogo *j);
void MovimentoHeroi(Jogo *j);
void ColisaoBordaHeroi(Jogo *j);
void AtualizaHeroi(Jogo *j);
void AtualizaNaves(Jogo *j);
void AtiraHeroi(Jogo *j);
void DesenhaBalaHeroi(Jogo *j);
void ColisaoTiroHeroi(Jogo *j);
void IniciaBarreira(Jogo *j);
void DesenhaBarreira(Jogo *j);
void TelaInicial(Jogo *j);
int Atirou(Jogo *j, int i, int k);
void MenuPause(Jogo *j);
void EscolhaHeroi(Jogo *j);
void FimDeJogo(Jogo *j);



int main(){
    InitAudioDevice();

    Jogo jogo;

    jogo.alturaJanela = ALTURA_JANELA;
    jogo.larguraJanela = LARGURA_JANELA;
    jogo.menu.pause = 1;
    srand(time(NULL));

    InitWindow(jogo.larguraJanela, jogo.alturaJanela, "Space Invaders");
    SetTargetFPS(60);
    IniciaJogo(&jogo);
    CarregaImagens(&jogo);
    Music musicaJogo = LoadMusicStream("assets/musica.mp3");
    PlayMusicStream(musicaJogo);

    while(!WindowShouldClose()){
        UpdateMusicStream(musicaJogo);
        AtualizaFrameDesenho(&jogo);
    }
    UnloadMusicStream(musicaJogo);
    DescarregaImagens(&jogo);
    CloseWindow();
    return 0;
}

void CarregaImagens(Jogo *j){
    j->assets.naveVerde = LoadTexture("assets/GreenAnimation.png");
    j->heroi.sprites[0] = LoadTexture("assets/heroi.png");
    j->heroi.sprites[1] = LoadTexture("assets/heroi1.png");
    j->heroi.sprites[2] = LoadTexture("assets/heroi2.png");
    j->heroi.sprites[3] = LoadTexture("assets/heroi3.png");
}

void DescarregaImagens(Jogo *j){
    UnloadTexture(j->assets.naveVerde);
    for(int i = 0; i < 4; i ++) {
        UnloadTexture(j->heroi.sprites[i]);
    }
} 

void AtualizaJogo(Jogo *j){
    if (j->menu.pause == 0) {
    DrawText(TextFormat("Vidas: %i", j->heroi.vidas), 20, 20, 20, WHITE);
    AtualizaHeroi(j);
    AtualizaNaves(j);
    if (IsKeyPressed(KEY_F2)) j->heroi.vidas = 0;
    if (IsKeyPressed(KEY_P)) j->menu.pause = 2;
    } else if (j->menu.pause == 1) {
        TelaInicial(j);
    } else if (j->menu.pause == 2) {
        MenuPause(j);
    }
}

void DesenhaJogo(Jogo *j){
    BeginDrawing();
    if (j->menu.pause == 0) {
        ClearBackground(BLACK);
        DesenhaNaves(j);
        DesenhaHeroi(j);
        DesenhaBarreira(j);
    } else if (j->menu.pause == 1) {
        TelaInicial(j);
    } else if (j->menu.pause == 2) {
        MenuPause(j);
    } else if (j->menu.pause == 3) {
        FimDeJogo(j);
    }
    DesenhaBordas(j);
    EndDrawing();
}

void AtualizaFrameDesenho(Jogo *j){
    AtualizaJogo(j);
    DesenhaJogo(j);
}

void IniciaJogo(Jogo *j){
    j->tempoAnimacao = GetTime();
    IniciaHeroi(j);
    IniciaNaves(j);
    IniciaBordas(j);
    IniciaBarreira(j);
}

void TelaInicial(Jogo *j) {
    DrawText(TextFormat("Fazer a tela de inicio quando o\nprofessor passar arquivos"), 20, 400, 30, WHITE);
    if (IsKeyPressed(KEY_ENTER)) {
        j->menu.pause = 2;
    }
}

void FimDeJogo(Jogo *j) {
    ClearBackground(BLACK);
    DrawText("Acabou!", LARGURA_JANELA/3, ALTURA_JANELA/2-100,50,RED);
}

void MenuPause(Jogo *j) {
    int clear = 0;
    if (clear == 0) {
    ClearBackground(BLACK);
    clear ++;
    }
    DrawText("Menu", LARGURA_JANELA/3+50, ALTURA_JANELA/4, 40 ,WHITE);
    DrawText("Pressione ENTER pra começar", LARGURA_JANELA/5, ALTURA_JANELA/3, 25 ,WHITE);
    DrawText("Pressione de 1 a 4 para escolher sua nave", 35, ALTURA_JANELA/3+50, 25 ,WHITE);
    DrawTexture(j->heroi.sprites[0], LARGURA_JANELA/3-25, ALTURA_JANELA/3 + 100,WHITE);
    DrawTexture(j->heroi.sprites[1], LARGURA_JANELA/3 + 50, ALTURA_JANELA/3 + 100,WHITE);
    DrawTexture(j->heroi.sprites[2], LARGURA_JANELA/3 + 125, ALTURA_JANELA/3 + 100,WHITE);
    DrawTexture(j->heroi.sprites[3], LARGURA_JANELA/3 + 200, ALTURA_JANELA/3 + 100,WHITE);
    DrawTexture(j->heroi.skin, j->heroi.pos.x,j->heroi.pos.y, WHITE);
    EscolhaHeroi(j);
    if (IsKeyPressed(KEY_SPACE))
    j->menu.pause = 0;
}

void EscolhaHeroi(Jogo *j) {
        if (IsKeyPressed(KEY_ONE)) {
        ClearBackground(BLACK);
        j->heroi.skin = j->heroi.sprites[0];
        j->heroi.bala.color = SKYBLUE;
    } else if (IsKeyPressed(KEY_TWO)) {
        ClearBackground(BLACK);
        j->heroi.skin = j->heroi.sprites[1];
        j->heroi.bala.color = RED;
    } else if (IsKeyPressed(KEY_THREE)) {
        ClearBackground(BLACK);
        j->heroi.skin = j->heroi.sprites[2];
        j->heroi.bala.color = GRAY;
    } else if (IsKeyPressed(KEY_FOUR)) {
        ClearBackground(BLACK);
        j->heroi.skin = j->heroi.sprites[3];
        j->heroi.bala.color = GREEN;
    }
}













void IniciaBordas(Jogo *j) {
    //borda encima
    j->bordas[0].pos = (Rectangle){0, 0, LARGURA_JANELA, 10};
    //borda embaixo
    j->bordas[1].pos = (Rectangle){0, ALTURA_JANELA-10, LARGURA_JANELA, 10};
    //borda esquerda
    j->bordas[2].pos = (Rectangle){0, 0, 10, ALTURA_JANELA};
    //borda direita
    j->bordas[3].pos = (Rectangle){LARGURA_JANELA-10, 0, 10, ALTURA_JANELA};
}

void DesenhaBordas(Jogo *j){
    for(int i = 0; i < 4; i++){
        DrawRectangleRec(j->bordas[i].pos, LIGHTGRAY);
    }
}

void IniciaBarreira(Jogo *j) {
    for (int i = 0; i < 3; i ++) {
        for (int k = 0; k < 5; k ++) {
            j->barreira[i][k].color = GREEN;
            j->barreira[i][k].vida = 2;
            j->barreira[i][k].pos = (Rectangle) {((i+1)*LARGURA_JANELA/6 + (i*90)) - 10*k, 4*ALTURA_JANELA/5 + (k*15), 20*(k+1), 15};
        }
    }
}

void DesenhaBarreira(Jogo *j) {
    for(int i = 0; i < 3; i ++) {
        for (int k = 0; k < 5; k ++) {
            if (j->barreira[i][k].vida > 0)
            DrawRectangleRec(j->barreira[i][k].pos, j->barreira[i][k].color);
        }
    }
}










void IniciaHeroi(Jogo *j) {
    j->heroi.pos = (Rectangle) {LARGURA_JANELA/2 - STD_SIZE_X/2, ALTURA_JANELA - STD_SIZE_Y -10, STD_SIZE_X, STD_SIZE_Y};
    j->heroi.color = BLUE;
    j->heroi.velocidade = 5;
    j->heroi.colisaoDireita = 0;
    j->heroi.colisaoEsquerda = 0;
    j->heroi.vidas = 3;
    j->heroi.bala.color = SKYBLUE;
    j->heroi.bala.ativa = 0;
    j->heroi.bala.velocidade = 5;
}

void AtualizaHeroi(Jogo *j) {
    MovimentoHeroi(j);
    AtiraHeroi(j);
    if (j->heroi.vidas <= 0) {
        j->menu.pause = 3;
    }
}

void DesenhaHeroi(Jogo *j){
    DrawTexture(j->heroi.skin, j->heroi.pos.x, j->heroi.pos.y, WHITE);
}

void MovimentoHeroi(Jogo *j) {
    if (IsKeyDown(KEY_LEFT) && j->heroi.colisaoEsquerda == 0) {
        j->heroi.pos.x -= j->heroi.velocidade;
        j->heroi.colisaoDireita = 0;
    } else if (IsKeyDown(KEY_RIGHT) && j->heroi.colisaoDireita == 0) {
        j->heroi.pos.x += j->heroi.velocidade;
        j->heroi.colisaoEsquerda = 0;
    }
    ColisaoBordaHeroi(j);
}

void ColisaoBordaHeroi(Jogo *j) {
    if(CheckCollisionRecs(j->heroi.pos, j->bordas[2].pos)){ //esquerda
        j->heroi.colisaoEsquerda = 1;
    }else if(CheckCollisionRecs(j->heroi.pos, j->bordas[3].pos)){ //direita
        j->heroi.colisaoDireita = 1;
    }
}

void AtiraHeroi(Jogo *j) {
    if (IsKeyPressed(KEY_SPACE) && j->heroi.bala.ativa == 0) {
        j->heroi.bala.pos = (Rectangle) {(j->heroi.pos.x - LARGURA_BALA/2)+j->heroi.pos.width/2, j->heroi.pos.y-j->heroi.pos.height/2, 
        LARGURA_BALA, ALTURA_BALA};
        j->heroi.bala.ativa = 1;
    } else if (j->heroi.bala.ativa == 1) {
        DesenhaBalaHeroi(j);
        j->heroi.bala.pos.y -= j->heroi.bala.velocidade;
        ColisaoTiroHeroi (j);
    }
}

void DesenhaBalaHeroi(Jogo *j) {
    DrawRectangleRec(j->heroi.bala.pos, j->heroi.bala.color);
}

void ColisaoTiroHeroi(Jogo *j) {
    if (CheckCollisionRecs(j->heroi.bala.pos, j->bordas[0].pos)) {
        j->heroi.bala.ativa = 0;
    }
    for (int i = 0; i < 4; i ++) {
        for (int k = 0; k < 7; k ++) {
            if (CheckCollisionRecs(j->nave[i][k].pos, j->heroi.bala.pos) && j->nave[i][k].vida > 0) {
                j->heroi.bala.ativa = 0;
                j->nave[i][k].vida --;
            }
        }
    }
}











void IniciaNaves(Jogo *j){
    for (int i = 0; i < 4; i ++) {
        for (int k = 0; k < 7; k ++) {
            j->nave[i][k].pos = (Rectangle) {16 + 56 * k, 15 + 64 * i, STD_SIZE_X, STD_SIZE_Y};
            j->nave[i][k].color = RED;
            j->nave[i][k].bala.ativa = 0;
            j->nave[i][k].bala.tempo = GetTime();
            j->nave[i][k].bala.velocidade = 5;
            j->nave[i][k].bala.tiro = LoadSound("assets/shoot.wav");
            j->nave[i][k].vida = 1;
        }
    }
    j->velocidadeNaves = 0.075;
    j->dificuldade = 0;
}

void AtualizaNaves(Jogo *j) {
    for (int i = 0; i < 4; i ++) {
        for (int k = 0; k < 7; k ++) {
            AtualizaNavePos(j,i,k);
            if (j->nave[i][k].vida > 0) {
            AtiraBalas(j, i, k);
            }
        }
    }
}

void DesenhaNaves(Jogo *j){
    for (int i = 0; i < 4; i ++) {
    for (int k = 0; k < 7; k ++) {
        if (j->nave[i][k].vida > 0) {
            Vector2 tamanhoFrame = {32, 32};
            
            static Vector2 frame = {0, 0};
            static float tempoUltimaTroca = 0;
                
            if(GetTime() - tempoUltimaTroca >= 1){
                if(frame.x == 0){
                    frame.x = 1;
                }else{
                    frame.x = 0;
                }

                tempoUltimaTroca = GetTime();
            }
            Rectangle frameRecNave = {frame.x * tamanhoFrame.x, frame.y*tamanhoFrame.y,
            tamanhoFrame.x, tamanhoFrame.y};
            DrawTexturePro(j->assets.naveVerde, frameRecNave, (Rectangle){j->nave[i][k].pos.x, j->nave[i][k].pos.y, 32, 32},
            (Vector2){0, 0}, 0.0f, WHITE);
            }
        }
    }

}

void AtualizaNavePos(Jogo *j, int i, int k){
ColisaoBordas(j);
    for(int i = 0; i < 4; i ++)
    for(int k = 0; k < 7; k ++)
    j->nave[i][k].pos.x += j->velocidadeNaves;
}

void DesenhaBalas(Jogo *j, int i, int k){
    DrawRectangleRec(j->nave[i][k].bala.pos, YELLOW);
}

void AtiraBalas(Jogo *j, int i, int k){
    if(j->nave[i][k].bala.ativa == 0 && GetTime()-j->nave[i][k].bala.tempo > 3){
        if (Atirou(j, i, k) == 0) {
        j->nave[i][k].bala.pos = (Rectangle){j->nave[i][k].pos.x+j->nave[i][k].pos.width/2, j->nave[i][k].pos.y+j->nave[i][k].pos.height/2, 
        LARGURA_BALA, ALTURA_BALA};
        j->nave[i][k].bala.ativa = 1;
        PlaySound(j->nave[i][k].bala.tiro);
        } else {j->nave[i][k].bala.tempo = GetTime();}
    }
    if(j->nave[i][k].bala.ativa == 1){
        ColisaoBalas(j, i, k);
        j->nave[i][k].bala.pos.y += j->nave[i][k].bala.velocidade;
        DesenhaBalas(j, i, k);
    }
}

void ColisaoBordas(Jogo *j){
    if(CheckCollisionRecs(j->nave[0][0].pos, j->bordas[2].pos)){
        j->velocidadeNaves = 0.075;
    }else if(CheckCollisionRecs(j->nave[0][6].pos, j->bordas[3].pos)){
        j->velocidadeNaves = -0.075;
    }
}

void ColisaoBalas(Jogo *j, int i, int k){
    // Colisao bala com heroi
    if(CheckCollisionRecs(j->heroi.pos, j->nave[i][k].bala.pos)){
        j->nave[i][k].bala.ativa = 0;
        j->heroi.vidas --;
    }
    // Colisao bala com borda de baixo
    if(CheckCollisionRecs(j->nave[i][k].bala.pos, j->bordas[1].pos)){
        j->nave[i][k].bala.ativa = 0;
    }
    for (int h = 0; h < 3; h ++) {
        for (int g = 0; g < 5; g ++) {
            if(CheckCollisionRecs(j->nave[i][k].bala.pos, j->barreira[h][g].pos) && j->barreira[h][g].vida > 0) {
                j->nave[i][k].bala.ativa = 0;
                j->barreira[h][g].vida --;
            }
        }
    }
}

int Atirou(Jogo *j, int i, int k) {
    return rand() % 5;
}
