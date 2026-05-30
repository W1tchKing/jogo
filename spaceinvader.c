#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define STD_SIZE_X 32
#define STD_SIZE_Y 32
#define LARGURA_BALA 10
#define ALTURA_BALA 15

typedef struct Config {
    char nome[11];
    int pontuacao;
    int nivel;
    int navesInimigas;
    int tirosEmTela;
    int contadordeletra;
    int tempoRodada;
    int corrigeTempo;
    int pegaTempo;
    int corrigeRelogio;
    int tempoJogoAberto;
}Config;

typedef struct Bala{
    Rectangle final;
    Color color;
    int ativa;
    int tempo;
    int velocidade;
    Sound tiro;
}Bala;

typedef struct Barreira{
    Rectangle final;
    Color color;
    int vida;
}Barreira;

typedef struct Nave{
    Rectangle final;
    Color color;
    Bala bala;
    int recarga;
    int vida;
}Nave;

typedef struct Heroi{
    Texture2D sprite;
    Rectangle final;
    Color color;
    Bala bala;
    int velocidade;
    int colisaoDireita;
    int colisaoEsquerda;
    int vidas;
}Heroi;

typedef struct Bordas{
    Rectangle final;
} Bordas;

typedef struct Assets{
    Texture2D naveVerde;
    Sound tiro;
}Assets;

typedef struct Jogo{
    Nave nave[4][9];
    Heroi heroi;
    Bordas bordas[4];
    Assets assets;
    Barreira barreira[3][5];
    Config config;
    FILE *arquivo;
    FILE *arquivo2;
    int tela; // 1 -> tela inicial, 2 -> jogo rodando, 3 -> tela final
    double velocidadeNaves;
    int alturaJanela;
    int larguraJanela;
    int tempoAnimacao;
    int dificuldade; //quantidade de tiros na tela
}Jogo;

//funções base -----------------------------
void IniciaJogo(Jogo *j);
void IniciaBordas(Jogo *j);
void AtualizaJogo(Jogo *j);
void AtualizaFrameDesenho(Jogo *j);
void DesenhaJogo(Jogo *j);
void DesenhaBordas(Jogo *j);
void CarregaImagens(Jogo *j);
void DescarregaImagens(Jogo *j);
void TelaInicial(Jogo *j);
void FimDeJogo(Jogo *j);
void passaNivel(Jogo *j);
void salvaNome(Jogo *j);
void salvaUltimosCinco(Jogo *j);
//funções heroi ----------------------------
void IniciaHeroi(Jogo *j);
void AtualizaHeroi(Jogo *j);
void DesenhaHeroi(Jogo *j);
void MovimentoHeroi(Jogo *j);
void ColisaoBordaHeroi(Jogo *j);
void AtiraHeroi(Jogo *j);
void DesenhaBalaHeroi(Jogo *j);
void ColisaoTiroHeroi(Jogo *j);
//funções naves -----------------------------
void IniciaNaves(Jogo *j);
void DesenhaNaves(Jogo *j);
void AtualizaNaves(Jogo *j);
void AtualizaNavefinal(Jogo *j, int i, int k);
void AtiraBalas(Jogo *j, int i, int k);
void ColisaoBordas(Jogo *j);
void ColisaoBalas(Jogo *j, int i, int k);
double Atirou(Jogo *j, int i, int k);
//funções barreiras --------------------------
void IniciaBarreira(Jogo *j);
void DesenhaBarreira(Jogo *j);
//tempo
void Tempo(Jogo *j);


//funções essenciais



int main(){
    InitAudioDevice();
    Jogo jogo;
    IniciaJogo(&jogo);
    jogo.arquivo = fopen("assets/historico_jogos", "a");
    jogo.config.nome[11] = '\0';
    jogo.config.contadordeletra = 0;
    jogo.alturaJanela = ALTURA_JANELA;
    jogo.larguraJanela = LARGURA_JANELA;
    jogo.tela = 0;
    srand(time(NULL));
    
    InitWindow(jogo.larguraJanela, jogo.alturaJanela, "Space Invaders");
    SetTargetFPS(60);
    CarregaImagens(&jogo);
    Music musicaJogo = LoadMusicStream("assets/musica.mp3");
    PlayMusicStream(musicaJogo);

    while(!WindowShouldClose()){
        jogo.config.tempoJogoAberto = GetTime();
        UpdateMusicStream(musicaJogo);
        AtualizaFrameDesenho(&jogo);
        if(jogo.tela == 0) {
        int letra = GetCharPressed();
        while (letra > 0) {
            if ((letra >= 32) && (letra <= 125) && jogo.config.contadordeletra <= 9) {
                jogo.config.nome[jogo.config.contadordeletra] = (char)(letra);
                jogo.config.nome[jogo.config.contadordeletra+1] = '\0';
                jogo.config.contadordeletra++;
            }
            letra = GetCharPressed();
        }
            if (IsKeyPressed(KEY_BACKSPACE)) {
                ClearBackground(BLACK);
                jogo.config.nome[jogo.config.contadordeletra-1] = '\0';
                jogo.config.contadordeletra --;
                if (jogo.config.contadordeletra < 0) jogo.config.contadordeletra = 0;
            }
        }
        if (jogo.tela == 2) {
            if (IsKeyPressed(KEY_ENTER)) {
                fclose(jogo.arquivo);
                UnloadMusicStream(musicaJogo);
                DescarregaImagens(&jogo);
                CloseWindow();
                return 0;
            }
        }
    }
    fclose(jogo.arquivo);
    UnloadMusicStream(musicaJogo);
    DescarregaImagens(&jogo);
    CloseWindow();
    return 0;
}

void CarregaImagens(Jogo *j){
    j->assets.naveVerde = LoadTexture("assets/GreenAnimation.png");
    j->heroi.sprite = LoadTexture("assets/heroi.png");
}

void DescarregaImagens(Jogo *j){
    UnloadTexture(j->assets.naveVerde);
    UnloadTexture(j->heroi.sprite);
} 

void AtualizaJogo(Jogo *j){
    if (j->tela == 1) {
    DrawText(TextFormat("Vidas: %i", j->heroi.vidas), 20, 20, 20, WHITE);
    DrawText(TextFormat("pontuacao: %i", j->config.pontuacao), 20, 60, 20, WHITE);
    DrawText(TextFormat("tempo rodada: %i", j->config.tempoRodada), 20, 100, 20, WHITE);
    AtualizaHeroi(j);
    AtualizaNaves(j);
    passaNivel(j);
    Tempo(j);
    if (IsKeyPressed(KEY_F2)) j->heroi.vidas = 0;
    } else if (j->tela == 0) {
        TelaInicial(j);
    } 
}

void DesenhaJogo(Jogo *j){
    BeginDrawing();
    if (j->tela == 1) {
        ClearBackground(BLACK);
        DesenhaNaves(j);
        DesenhaHeroi(j);
        DesenhaBarreira(j);
    } else if (j->tela == 0) {
        TelaInicial(j);
    } else if (j->tela == 2) {
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
    j->dificuldade = 1;
    j->config.tirosEmTela = 0;
    j->config.navesInimigas = 0;
    j->config.nivel = 0;
    j->config.pontuacao = 0;
    j->config.corrigeTempo = 0;
    IniciaHeroi(j);
    IniciaNaves(j);
    IniciaBordas(j);
    IniciaBarreira(j);
}

void TelaInicial(Jogo *j) {
    int larguraTexto = MeasureText(j->config.nome, 40);
    int xfinal = (LARGURA_JANELA - larguraTexto) / 2;
    int yfinal = ALTURA_JANELA - 450;

    DrawText("SPACE INVADERS!", 120, 40, 60, WHITE);
    DrawText("Insira seu nome:", 235, ALTURA_JANELA - 490, 40, DARKGREEN);

    DrawRectangle(xfinal - 5, yfinal - 5, larguraTexto + 10, 45, BLACK);
    if (j->config.contadordeletra != 0) {
        DrawText(j->config.nome, xfinal, yfinal, 40, RED);
    }

    DrawText("Historico:", 285, 305, 35, WHITE);
    DrawText(LoadFileText("assets/cincoUltimos"), LARGURA_JANELA / 3 + 10, 340, 29, GRAY);


    if (IsKeyPressed(KEY_ENTER) && j->config.contadordeletra > 0) {
        salvaNome(j);
        j->config.corrigeRelogio = j->config.tempoJogoAberto;
        j->tela = 1;
    }
}


void FimDeJogo(Jogo *j) {
    ClearBackground(BLACK);
    DrawText("Acabou!", LARGURA_JANELA/2-100, 100,50,RED);
}

void passaNivel(Jogo *j) {
    if (j->config.navesInimigas == 0) {
        IniciaNaves(j);
        IniciaBarreira(j);
        j->dificuldade ++;
        j->config.tirosEmTela = 0;
        j->config.nivel ++;
        j->config.corrigeTempo = j->config.pegaTempo;
    }
}






//funções do "mapa" -------------------------------------------------------



void IniciaBordas(Jogo *j) {
    //borda encima
    j->bordas[0].final = (Rectangle){0, 0, LARGURA_JANELA, 10};
    //borda embaixo
    j->bordas[1].final = (Rectangle){0, ALTURA_JANELA-10, LARGURA_JANELA, 10};
    //borda esquerda
    j->bordas[2].final = (Rectangle){0, 0, 10, ALTURA_JANELA};
    //borda direita
    j->bordas[3].final = (Rectangle){LARGURA_JANELA-10, 0, 10, ALTURA_JANELA};
}

void DesenhaBordas(Jogo *j){
    for(int i = 0; i < 4; i++){
        DrawRectangleRec(j->bordas[i].final, LIGHTGRAY);
    }
}

void IniciaBarreira(Jogo *j) {
    for (int i = 0; i < 3; i ++) {
        for (int k = 0; k < 5; k ++) {
            j->barreira[i][k].color = DARKGRAY;
            j->barreira[i][k].vida = 2;
            j->barreira[i][k].final = (Rectangle) {(LARGURA_JANELA/4 + i*LARGURA_JANELA/4) - 10*k, 3*ALTURA_JANELA/4 + 15*k, 20*(k+1), 15};
        }
    }
}

void DesenhaBarreira(Jogo *j) {
    for(int i = 0; i < 3; i ++) {
        for (int k = 0; k < 5; k ++) {
            if (j->barreira[i][k].vida > 0)
            DrawRectangleRec(j->barreira[i][k].final, j->barreira[i][k].color);
        }
    }
}





//funções do heroi -----------------------------------------------------------




void IniciaHeroi(Jogo *j) {
    j->heroi.final = (Rectangle) {LARGURA_JANELA/2 - STD_SIZE_X/2, ALTURA_JANELA - STD_SIZE_Y -10, STD_SIZE_X, STD_SIZE_Y};
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
        fputc(' ', j->arquivo);
        fprintf(j->arquivo,"%dpts\n",j->config.pontuacao);
        salvaUltimosCinco(j);
        j->tela = 2;
    }
}

void DesenhaHeroi(Jogo *j){
    DrawTexture(j->heroi.sprite, j->heroi.final.x, j->heroi.final.y, WHITE);
}

void MovimentoHeroi(Jogo *j) {
    if (IsKeyDown(KEY_LEFT) && j->heroi.colisaoEsquerda == 0) {
        j->heroi.final.x -= j->heroi.velocidade;
        j->heroi.colisaoDireita = 0;
    } else if (IsKeyDown(KEY_RIGHT) && j->heroi.colisaoDireita == 0) {
        j->heroi.final.x += j->heroi.velocidade;
        j->heroi.colisaoEsquerda = 0;
    }
    ColisaoBordaHeroi(j);
}

void ColisaoBordaHeroi(Jogo *j) {
    if(CheckCollisionRecs(j->heroi.final, j->bordas[2].final)){
        j->heroi.colisaoEsquerda = 1;
    }else if(CheckCollisionRecs(j->heroi.final, j->bordas[3].final)){
        j->heroi.colisaoDireita = 1;
    }
}

void AtiraHeroi(Jogo *j) {
    if (IsKeyPressed(KEY_SPACE) && j->heroi.bala.ativa == 0) {
        j->heroi.bala.final = (Rectangle) {(j->heroi.final.x - LARGURA_BALA/2)+j->heroi.final.width/2, j->heroi.final.y-j->heroi.final.height/2, 
        LARGURA_BALA, ALTURA_BALA};
        j->heroi.bala.ativa = 1;
        PlaySound(j->nave[0][0].bala.tiro);
    } else if (j->heroi.bala.ativa == 1) {
        DesenhaBalaHeroi(j);
        j->heroi.bala.final.y -= j->heroi.bala.velocidade;
        ColisaoTiroHeroi (j);
    }
}

void DesenhaBalaHeroi(Jogo *j) {
    DrawRectangleRec(j->heroi.bala.final, j->heroi.bala.color);
}

void ColisaoTiroHeroi(Jogo *j) {
    if (CheckCollisionRecs(j->heroi.bala.final, j->bordas[0].final)) {
        j->heroi.bala.ativa = 0;
    }
    for (int i = 0; i < 3; i ++) {
        for (int k = 0; k < 5; k ++) {
            if (CheckCollisionRecs(j->heroi.bala.final, j->barreira[i][k].final) && j->barreira[i][k].vida > 0) {
                j->heroi.bala.ativa = 0;
            }
        }
    }
    for (int i = 0; i < 4; i ++) {
        for (int k = 0; k < 9; k ++) {
            if (CheckCollisionRecs(j->nave[i][k].final, j->heroi.bala.final) && j->nave[i][k].vida > 0) {
                j->heroi.bala.ativa = 0;
                j->nave[i][k].vida --;
                j->config.navesInimigas --;
                j->config.pontuacao += 10;
                if(j->nave[i][k].bala.ativa == 1) {
                    j->config.tirosEmTela --;
                }
            }
        }
    }
}






//funções das naves verdes -------------------------------------------------------------






void IniciaNaves(Jogo *j){
    for (int i = 0; i < 4; i ++) {
        for (int k = 0; k < 9; k ++) {
            j->nave[i][k].final = (Rectangle) {16 + 56 * k, 15 + 96 * i, STD_SIZE_X, STD_SIZE_Y};
            j->nave[i][k].color = RED;
            j->nave[i][k].bala.ativa = 0;
            j->nave[i][k].bala.tempo = GetTime();
            j->nave[i][k].bala.velocidade = 5;
            j->nave[i][k].bala.tiro = LoadSound("assets/shoot.wav");
            j->nave[i][k].vida = 1;
            j->config.navesInimigas++;
        }
    }
    if(j->config.nivel == 0)
    j->velocidadeNaves = 0.050;
}

void AtualizaNaves(Jogo *j) {
    for (int i = 0; i < 4; i ++) {
        for (int k = 0; k < 9; k ++) {
            AtualizaNavefinal(j,i,k);
            if (j->nave[i][k].vida > 0) {
            AtiraBalas(j, i, k);
            }
        }
    }
}

void DesenhaNaves(Jogo *j){
    for (int i = 0; i < 4; i ++) {
    for (int k = 0; k < 9; k ++) {
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
            DrawTexturePro(j->assets.naveVerde, frameRecNave, (Rectangle){j->nave[i][k].final.x, j->nave[i][k].final.y, 32, 32},
            (Vector2){0, 0}, 0.0f, WHITE);
            }
        }
    }

}

void AtualizaNavefinal(Jogo *j, int i, int k){
ColisaoBordas(j);
    for(int i = 0; i < 4; i ++)
    for(int k = 0; k < 9; k ++)
    j->nave[i][k].final.x += j->velocidadeNaves;
}

void DesenhaBalas(Jogo *j, int i, int k){
    DrawRectangleRec(j->nave[i][k].bala.final, YELLOW);
}

void AtiraBalas(Jogo *j, int i, int k){
    if(j->nave[i][k].bala.ativa == 0 && GetTime()-j->nave[i][k].bala.tempo > 3 && j->config.tirosEmTela < 4+j->dificuldade){
        if (Atirou(j, i, k) == 0) {
        j->nave[i][k].bala.final = (Rectangle){j->nave[i][k].final.x+j->nave[i][k].final.width/2, j->nave[i][k].final.y+j->nave[i][k].final.height/2, 
        LARGURA_BALA, ALTURA_BALA};
        j->nave[i][k].bala.ativa = 1;
        j->config.tirosEmTela++;
        PlaySound(j->nave[i][k].bala.tiro);
        } else {j->nave[i][k].bala.tempo = GetTime();}
    }
    if(j->nave[i][k].bala.ativa == 1){
        ColisaoBalas(j, i, k);
        j->nave[i][k].bala.final.y += j->nave[i][k].bala.velocidade;
        DesenhaBalas(j, i, k);
    }
}

void ColisaoBordas(Jogo *j){
    if(CheckCollisionRecs(j->nave[0][0].final, j->bordas[2].final)){
        j->velocidadeNaves *= -1;
        if (j->velocidadeNaves < 0.5)
        j->velocidadeNaves += 0.0005;
    }else if(CheckCollisionRecs(j->nave[0][8].final, j->bordas[3].final)){
        j->velocidadeNaves *= -1;
        if (j->velocidadeNaves < 0.1)
        j->velocidadeNaves -= 0.0005;
    }
}

void ColisaoBalas(Jogo *j, int i, int k){
    // Colisao bala com heroi
    if(CheckCollisionRecs(j->heroi.final, j->nave[i][k].bala.final)){
        j->nave[i][k].bala.ativa = 0;
        j->heroi.vidas --;
        j->config.tirosEmTela --;
    }
    // Colisao bala com borda de baixo
    if(CheckCollisionRecs(j->nave[i][k].bala.final, j->bordas[1].final)){
        j->nave[i][k].bala.ativa = 0;
        j->config.tirosEmTela --;
    }
    for (int h = 0; h < 3; h ++) {
        for (int g = 0; g < 5; g ++) {
            if(CheckCollisionRecs(j->nave[i][k].bala.final, j->barreira[h][g].final) && j->barreira[h][g].vida > 0) {
                j->nave[i][k].bala.ativa = 0;
                j->barreira[h][g].vida --;
                j->config.tirosEmTela --;
            }
        }
    }
}

double Atirou(Jogo *j, int i, int k) {
    if (j->dificuldade < 12)
    return rand() % (10 - (int)(0.75*j->dificuldade));
    else return rand() % 2;
}



//funções de arquivo

void salvaNome(Jogo *j) {
    fputs(j->config.nome, j->arquivo);
}

void salvaUltimosCinco(Jogo *j) {
    char buf[60];
    char linhas[5][60];
    int cont = 0;

    // inicializa \n
    for (int i = 0; i < 5; i++) {
        linhas[i][0] = '\0';
    }

    //modo leitura
    fclose(j->arquivo);
    j->arquivo = fopen("assets/historico_jogos", "r");
    if (j->arquivo == NULL) {
        printf("erro p abrir o arquivo historico_jogos\n");
        return;
    }

    while (fgets(buf, sizeof(buf), j->arquivo)) {
        strcpy(linhas[cont % 5], buf);
        cont++;
    }

    j->arquivo2 = fopen("assets/cincoUltimos", "w");
    if (j->arquivo2 == NULL) {
        printf("erro p abrir o arquivo cincoUltimos\n");
        return;
    }

    for (int i = 0; i < 5; i++) {
        if (strlen(linhas[(cont + i) % 5]) > 0) {
            fprintf(j->arquivo2, "%s", linhas[(cont + i) % 5]);
        }
    }

    fclose(j->arquivo2);
}

//tempo
void Tempo(Jogo *j) {
    if (j->config.tempoRodada == 180) {
        j->heroi.vidas = 0;
    }
    j->config.pegaTempo = GetTime() - j->config.corrigeRelogio;
    j->config.tempoRodada = j->config.pegaTempo - j->config.corrigeTempo;
}
