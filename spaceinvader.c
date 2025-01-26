#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 800
#define STD_SIZE_X 32
#define STD_SIZE_Y 32
#define LARGURA_BALA 10
#define ALTURA_BALA 15


typedef struct Menu{
    int aberto;
    char nome[30];
}Menu;

typedef struct Bala{
    Rectangle pos;
    Color color;
    int ativa;
    int tempo;
    int velocidade;
    Sound tiro;
}Bala;

typedef struct Nave{
    Rectangle pos;
    Color color;
    Bala bala;
    int velocidade;
    int direcao;
    int tiroAtual;
    int tiroAnterior;
}Nave;

typedef struct Heroi{
    Texture2D sprite;
    Rectangle pos;
    Color color;
    Bala bala;
    int velocidade;
    int colisaoBordaEsquerda;
    int colisaoBordaDireita;
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
    Menu menu;
    Nave nave;
    Heroi heroi;
    Bordas bordas[4];
    Assets assets;
    int alturaJanela;
    int larguraJanela;
    int tempoAnimacao;
    int vidaBarricada[3];
}Jogo;

void IniciaJogo(Jogo *j);
void IniciaNaves(Jogo *j);
void AtualizaJogo(Jogo *j);
void DesenhaJogo(Jogo *j);
void AtualizaFrameDesenho(Jogo *j);
void AtualizaNavePos(Jogo *j);
void DesenhaNaves(Jogo *j);
void DesenhaHeroi(Jogo *j);
void ColisaoBordas(Jogo *j);
void DesenhaBordas(Jogo *j);
int ColisaoBalas(Jogo *j);
void DesenhaBordas(Jogo *j);
void AtiraBalas(Jogo *j);
void CarregaImagens(Jogo *j);
void DescarregaImagens(Jogo *j);
void ColisaoBordasHeroi(Jogo *j);
void AtiraBalaHeroi (Jogo *j);
int ColisaoBalasHeroi(Jogo *j);
void DesenhaBalasHeroi(Jogo *j);
void Pause(Jogo *j);
void SkinHeroi(Jogo *j);
void Acertou(Jogo *j);


int main(){
    InitAudioDevice();

    Jogo jogo;

    jogo.alturaJanela = ALTURA_JANELA;
    jogo.larguraJanela = LARGURA_JANELA;

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

void IniciaJogo(Jogo *j){
    j->menu.aberto = 1;

    j->tempoAnimacao = GetTime();

    j->heroi.pos = (Rectangle) {LARGURA_JANELA/2 - STD_SIZE_X/2, ALTURA_JANELA - STD_SIZE_Y -10, STD_SIZE_X, STD_SIZE_Y};
    j->heroi.color = BLUE;
    j->heroi.velocidade = 4;
    j->heroi.colisaoBordaDireita = 0;
    j->heroi.colisaoBordaEsquerda = 0;
    j->heroi.bala.ativa = 0;
    j->heroi.bala.velocidade = 5;
    j->heroi.vidas = 3;

    j->nave.tiroAtual = 0;
    j->nave.tiroAnterior = 0;
    j->nave.pos = (Rectangle) {0, 15, STD_SIZE_X, STD_SIZE_Y};
    j->nave.color = RED;
    j->nave.velocidade = 3;
    /*direcao = 1 faz nave mover para direita, direcao = 0 faz mover para esquerda*/
    j->nave.direcao = 1;
    j->nave.bala.ativa = 0;
    j->nave.bala.tempo = GetTime();
    j->nave.bala.velocidade = 5;
    j->nave.bala.tiro = LoadSound("assets/shoot.wav");

    //borda encima
    j->bordas[0].pos = (Rectangle){0, 0, LARGURA_JANELA, 10};
    //borda embaixo
    j->bordas[1].pos = (Rectangle){0, ALTURA_JANELA-10, LARGURA_JANELA, 10};
    //borda esquerda
    j->bordas[2].pos = (Rectangle){0, 0, 10, ALTURA_JANELA};
    //borda direita
    j->bordas[3].pos = (Rectangle){LARGURA_JANELA-10, 0, 10, ALTURA_JANELA};
}

void IniciaNaves(Jogo *j){

}

void AtualizaJogo(Jogo *j){
    if (j->menu.aberto == 0) {
    AtualizaNavePos(j);
    AtiraBalas(j);
    AtiraBalaHeroi(j);
    DrawText(TextFormat("Vidas: %i", j->heroi.vidas), 20, 20, 20, WHITE);
    if (IsKeyPressed(KEY_P)) {
        j->menu.aberto = 1;
    }
    }
}

void DesenhaJogo(Jogo *j){
    BeginDrawing();
    DesenhaBordas(j);
    if (j->menu.aberto == 0) {
    ClearBackground(BLACK);
    DesenhaNaves(j);
    DesenhaHeroi(j);
    } else if (j->menu.aberto == 1) {
        Pause(j);
        SkinHeroi(j);
    if (IsKeyPressed (KEY_ENTER)) {
        j->menu.aberto = 0;
    }
    }
    EndDrawing();
}

void AtualizaFrameDesenho(Jogo *j){
    AtualizaJogo(j);
    DesenhaJogo(j);
}

void AtualizaNavePos(Jogo *j){
    ColisaoBordas(j);
    if(j->nave.direcao == 1){
        j->nave.pos.x += j->nave.velocidade;
    }else{
        j->nave.pos.x -= j->nave.velocidade;
    }
}

void CarregaImagens(Jogo *j){
    j->assets.naveVerde = LoadTexture("assets/GreenAnimation.png");

}

void DescarregaImagens(Jogo *j){
    UnloadTexture(j->assets.naveVerde);
}

void DesenhaNaves(Jogo *j){
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
    DrawTexturePro(j->assets.naveVerde, frameRecNave, (Rectangle){j->nave.pos.x, j->nave.pos.y, 32, 32},
    (Vector2){0, 0}, 0.0f, WHITE);
    
}

void DesenhaHeroi(Jogo *j){
    DrawTexture(j->heroi.sprite, j->heroi.pos.x, j->heroi.pos.y, WHITE);
    ColisaoBordasHeroi(j);
    if (IsKeyDown(KEY_LEFT) && j->heroi.colisaoBordaEsquerda == 0) {
        j->heroi.pos.x -= j->heroi.velocidade;
        j->heroi.colisaoBordaDireita = 0;
    } else if (IsKeyDown(KEY_RIGHT) && j->heroi.colisaoBordaDireita == 0) {
        j->heroi.pos.x += j->heroi.velocidade;
        j->heroi.colisaoBordaEsquerda = 0;
    }

}

void DesenhaBordas(Jogo *j){
    for(int i = 0; i < 4; i++){
        DrawRectangleRec(j->bordas[i].pos, LIGHTGRAY);
    }
}

void DesenhaBalas(Jogo *j){
    DrawRectangleRec(j->nave.bala.pos, YELLOW);
}

void DesenhaBalasHeroi(Jogo *j){
    DrawRectangleRec(j->heroi.bala.pos, j->heroi.bala.color);
}

void AtiraBalas(Jogo *j){
    if(j->nave.bala.ativa == 0 && GetTime()-j->nave.bala.tempo > 3){
        j->nave.bala.pos = (Rectangle){j->nave.pos.x+j->nave.pos.width/2, j->nave.pos.y+j->nave.pos.height/2, 
        LARGURA_BALA, ALTURA_BALA};
        j->nave.tiroAtual ++;
        j->nave.bala.ativa = 1;
        j->nave.bala.tempo = GetTime();
        PlaySound(j->nave.bala.tiro);
    }
    else if(ColisaoBalas(j)){
        j->nave.bala.ativa = 0;
    }
    if(j->nave.bala.ativa == 1){
        j->nave.bala.pos.y += j->nave.bala.velocidade;
        DesenhaBalas(j);
    }
}
void AtiraBalaHeroi (Jogo *j) {
    if (IsKeyPressed(KEY_SPACE) && j->heroi.bala.ativa == 0) {
        j->heroi.bala.pos = (Rectangle) {j->heroi.pos.x+j->heroi.pos.width/2, j->heroi.pos.y+j->heroi.pos.height/2, LARGURA_BALA, ALTURA_BALA};
        j->heroi.bala.ativa = 1;
        PlaySound(j->nave.bala.tiro);
    } else if (ColisaoBalasHeroi(j)) {
        j->heroi.bala.ativa = 0;
    }
    if (j->heroi.bala.ativa == 1) {
        j->heroi.bala.pos.y -= j->heroi.bala.velocidade;
        DesenhaBalasHeroi(j);
    }
}



void ColisaoBordas(Jogo *j){
    if(CheckCollisionRecs(j->nave.pos, j->bordas[2].pos)){ //direita
        j->nave.direcao = 1;
    }else if(CheckCollisionRecs(j->nave.pos, j->bordas[3].pos)){ //esquerda
        j->nave.direcao = 0;
    }
}
void ColisaoBordasHeroi(Jogo *j){
    if(CheckCollisionRecs(j->heroi.pos, j->bordas[2].pos)){ //esquerda
        j->heroi.colisaoBordaEsquerda = 1;
    }else if(CheckCollisionRecs(j->heroi.pos, j->bordas[3].pos)){ //direita
        j->heroi.colisaoBordaDireita = 1;
    }
}

int ColisaoBalas(Jogo *j){
    // Colisao bala com heroi
    if(CheckCollisionRecs(j->heroi.pos, j->nave.bala.pos)){
        Acertou(j);
        return 1;
    }
    // Colisao bala com borda de baixo
    if(CheckCollisionRecs(j->nave.bala.pos, j->bordas[1].pos)){
        return 1;
    }
    return 0;
}

int ColisaoBalasHeroi(Jogo *j){
    // Colisao bala com heroi
    if(CheckCollisionRecs(j->nave.pos, j->heroi.bala.pos)){
        return 1;
    }
    // Colisao bala com borda de cima
    if(CheckCollisionRecs(j->heroi.bala.pos, j->bordas[0].pos)){
        return 1;
    }
    return 0;
}

void Pause(Jogo *j) {
    DrawText("Menu", ALTURA_JANELA/2-40, LARGURA_JANELA/4, 40 ,WHITE);
    DrawText("Pressione ENTER pra começar", ALTURA_JANELA/3-50, LARGURA_JANELA/3, 25 ,WHITE);
    DrawText("Pressione de 1 a 4 para escolher sua nave", ALTURA_JANELA/3-125, LARGURA_JANELA/3+35, 25 ,WHITE);
    DrawTexture(LoadTexture("assets/heroi.png"), ALTURA_JANELA/2 - 200,LARGURA_JANELA/2-16, WHITE);
    DrawTexture(LoadTexture("assets/heroi1.png"), ALTURA_JANELA/2 - 75,LARGURA_JANELA/2-16, WHITE);
    DrawTexture(LoadTexture("assets/heroi2.png"), ALTURA_JANELA/2 + 75,LARGURA_JANELA/2-16, WHITE);
    DrawTexture(LoadTexture("assets/heroi3.png"), ALTURA_JANELA/2 + 200,LARGURA_JANELA/2-16, WHITE);
}
void SkinHeroi(Jogo*j) {
    if (IsKeyPressed(KEY_ONE)) {
        j->heroi.sprite = LoadTexture("assets/heroi.png");
        j->heroi.bala.color = SKYBLUE;
        DrawRectangleRec(j->heroi.pos, BLACK);
        DrawTexture(j->heroi.sprite, j->heroi.pos.x,j->heroi.pos.y, WHITE);
    } else if (IsKeyPressed(KEY_TWO)) {
        j->heroi.sprite = LoadTexture("assets/heroi1.png");
        j->heroi.bala.color = RED;
        DrawRectangleRec(j->heroi.pos, BLACK);
        DrawTexture(j->heroi.sprite, j->heroi.pos.x,j->heroi.pos.y, WHITE);
    } else if (IsKeyPressed(KEY_THREE)) {
        j->heroi.sprite = LoadTexture("assets/heroi2.png");
        j->heroi.bala.color = GRAY;
        DrawRectangleRec(j->heroi.pos, BLACK);
        DrawTexture(j->heroi.sprite, j->heroi.pos.x,j->heroi.pos.y, WHITE);
    } else if (IsKeyPressed(KEY_FOUR)) {
        j->heroi.sprite = LoadTexture("assets/heroi3.png");
        j->heroi.bala.color = GREEN;
        DrawRectangleRec(j->heroi.pos, BLACK);
        DrawTexture(j->heroi.sprite, j->heroi.pos.x,j->heroi.pos.y, WHITE);
    }
}

void Acertou(Jogo *j) {
    if (j->nave.tiroAtual != j->nave.tiroAnterior) {
        j->nave.tiroAnterior = j->nave.tiroAtual;
        j->heroi.vidas --;
    }
}
