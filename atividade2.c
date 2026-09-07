#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600

typedef enum {
    MOEDA_BRONZE,
    MOEDA_PRATA,
    MOEDA_OURO,
    MOEDA_DIAMANTE
} TipoMoeda;

typedef struct {
    Vector2 pos;
    float raio;
    TipoMoeda tipo;
    int valor;
    bool coletada;
    float tempoColeta;
} Moeda;

Color corDaMoeda(TipoMoeda tipo) {
    switch (tipo) {
        case MOEDA_BRONZE:   return (Color){160, 90, 40, 255};
        case MOEDA_PRATA:    return (Color){190, 190, 190, 255};
        case MOEDA_OURO:     return GOLD;
        case MOEDA_DIAMANTE: return SKYBLUE;
        default:             return WHITE;
    }
}

int valorDaMoeda(TipoMoeda tipo) {
    switch (tipo) {
        case MOEDA_BRONZE:   return 5;
        case MOEDA_PRATA:    return 10;
        case MOEDA_OURO:     return 25;
        case MOEDA_DIAMANTE: return 50;
        default:             return 0;
    }
}

Moeda *criarMoedas(int quantidade) {
    Moeda *moedas = (Moeda *)malloc(quantidade * sizeof(Moeda));
    if (moedas == NULL) return NULL;

    for (int i = 0; i < quantidade; i++) {
        Moeda *m = (moedas + i);
        m->pos = (Vector2){ GetRandomValue(30, LARGURA_JANELA - 30),
                            GetRandomValue(30, ALTURA_JANELA - 30) };
        m->raio = 10.0f;
        
        int chance = GetRandomValue(0, 9);
        if (chance == 0) {
            m->tipo = MOEDA_DIAMANTE;
        } else {
            m->tipo = (TipoMoeda)GetRandomValue(MOEDA_BRONZE, MOEDA_OURO);
        }

        m->valor = valorDaMoeda(m->tipo);
        m->coletada = false;
        m->tempoColeta = 0.0f;
    }
    return moedas;
}

bool tentarColetar(Moeda *m, Vector2 posJogador, float raioJogador) {
    if (m->coletada) return false;

    float dx = m->pos.x - posJogador.x;
    float dy = m->pos.y - posJogador.y;
    float distancia = sqrtf(dx * dx + dy * dy);
    float somaRaios = m->raio + raioJogador;

    if (distancia <= somaRaios) {
        m->coletada = true;
        m->tempoColeta = (float)GetTime();
        return true;
    }
    return false;
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 2 - ED1 (Raylib)");
    SetTargetFPS(60);

    Vector2 posJogador = { LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f };
    float raioJogador = 15.0f;
    float velocidadeJogador = 4.0f;
    int pontuacaoTotal = 0;

    int quantidadeMoedas = 12;
    Moeda *moedas = criarMoedas(quantidadeMoedas);

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) posJogador.x += velocidadeJogador;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  posJogador.x -= velocidadeJogador;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  posJogador.y += velocidadeJogador;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    posJogador.y -= velocidadeJogador;

        if (posJogador.x - raioJogador < 0) posJogador.x = raioJogador;
        if (posJogador.x + raioJogador > LARGURA_JANELA) posJogador.x = LARGURA_JANELA - raioJogador;
        if (posJogador.y - raioJogador < 0) posJogador.y = raioJogador;
        if (posJogador.y + raioJogador > ALTURA_JANELA) posJogador.y = ALTURA_JANELA - raioJogador;

        for (int i = 0; i < quantidadeMoedas; i++) {
            if (moedas[i].coletada && ((float)GetTime() - moedas[i].tempoColeta >= 3.0f)) {
                moedas[i].pos = (Vector2){ GetRandomValue(30, LARGURA_JANELA - 30),
                                            GetRandomValue(30, ALTURA_JANELA - 30) };
                moedas[i].coletada = false;
            }

            if (tentarColetar(moedas + i, posJogador, raioJogador)) {
                pontuacaoTotal += moedas[i].valor;
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int i = 0; i < quantidadeMoedas; i++) {
                if (!moedas[i].coletada) {
                    DrawCircleV(moedas[i].pos, moedas[i].raio, corDaMoeda(moedas[i].tipo));
                }
            }

            DrawCircleV(posJogador, raioJogador, BLUE);

            DrawText(TextFormat("Pontuação: %d", pontuacaoTotal), 10, 10, 20, DARKGRAY);
            DrawText("Use as Setas ou W,A,S,D para mover", 10, ALTURA_JANELA - 30, 14, GRAY);

        EndDrawing();
    }

    free(moedas);
    CloseWindow();
    
    return 0;
}