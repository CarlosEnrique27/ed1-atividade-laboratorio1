#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600

typedef struct {
    float dano;
    int alcance;
} DadosArma;

typedef struct {
    int cura;
} DadosPocao;

typedef struct {
    int absorcao;
} DadosEscudo;

typedef union {
    DadosArma arma;
    DadosPocao pocao;
    DadosEscudo escudo;
} DadosItem;

typedef enum {
    ITEM_ARMA,
    ITEM_POCAO,
    ITEM_ESCUDO
} TipoItem;

typedef struct {
    Vector2 pos;
    float raio;
    TipoItem tipo;
    DadosItem dados;
    bool coletado;
} Item;

typedef struct {
    Vector2 pos;
    float raio;
    float velocidade;
    int vida;
    float dano;
    int armadura;
} Jogador;

Color corDoItem(Item *it) {
    if (it->tipo == ITEM_ARMA) return RED;
    if (it->tipo == ITEM_ESCUDO) return BLUE;
    if (it->tipo == ITEM_POCAO) {
        if (it->dados.pocao.cura < 0) {
            return PURPLE; 
        }
        return GREEN; 
    }
    return WHITE;
}

Item *criarItens(int quantidade) {
    Item *itens = (Item *)malloc(quantidade * sizeof(Item));
    if (itens == NULL) return NULL;

    for (int i = 0; i < quantidade; i++) {
        Item *it = (itens + i);
        it->pos = (Vector2){ GetRandomValue(30, LARGURA_JANELA - 30),
                            GetRandomValue(30, ALTURA_JANELA - 30) };
        it->raio = 12.0f;
        it->coletado = false;
        
        // Sorteia o tipo de item (Arma, Poção ou Escudo)
        it->tipo = (TipoItem)GetRandomValue(ITEM_ARMA, ITEM_ESCUDO);

        if (it->tipo == ITEM_ARMA) {
            it->dados.arma.dano = (float)GetRandomValue(2, 8);
            it->dados.arma.alcance = GetRandomValue(1, 3);
        } else if (it->tipo == ITEM_POCAO) {
            int chanceVeneno = GetRandomValue(0, 9);
            if (chanceVeneno < 3) {
                // Poção envenenada (Exercício 2): cura negativa
                it->dados.pocao.cura = -GetRandomValue(10, 25);
            } else {
                // Poção normal
                it->dados.pocao.cura = GetRandomValue(10, 30);
            }
        } else if (it->tipo == ITEM_ESCUDO) {
            // Exercício 1: Escudo
            it->dados.escudo.absorcao = GetRandomValue(5, 15);
        }
    }
    return itens;
}

void aplicarItem(Jogador *j, Item *item) {
    if (item->coletado) return;

    switch (item->tipo) {
        case ITEM_ARMA:
            j->dano += item->dados.arma.dano;
            break;
        case ITEM_POCAO:
            j->vida += item->dados.pocao.cura;
            if (j->vida < 0) {
                j->vida = 0; 
            }
            break;
        case ITEM_ESCUDO:
            j->armadura += item->dados.escudo.absorcao; 
            break;
    }
    item->coletado = true;
}

bool tentarColetar(Item *item, Vector2 posJogador, float raioJogador) {
    if (item->coletado) return false;

    float dx = item->pos.x - posJogador.x;
    float dy = item->pos.y - posJogador.y;
    float distancia = sqrtf(dx * dx + dy * dy);
    float somaRaios = item->raio + raioJogador;

    if (distancia <= somaRaios) {
        return true;
    }
    return false;
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 3 - ED1 (Raylib)");
    SetTargetFPS(60);

    Jogador jogador = {
        .pos = { LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f },
        .raio = 15.0f,
        .velocidade = 4.0f,
        .vida = 100,
        .dano = 10.0f,
        .armadura = 0
    };

    int quantidadeItens = 12;
    Item *itens = criarItens(quantidadeItens);

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) jogador.pos.x += jogador.velocidade;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  jogador.pos.x -= jogador.velocidade;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  jogador.pos.y += jogador.velocidade;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    jogador.pos.y -= jogador.velocidade;

        if (jogador.pos.x - jogador.raio < 0) jogador.pos.x = jogador.raio;
        if (jogador.pos.x + jogador.raio > LARGURA_JANELA) jogador.pos.x = LARGURA_JANELA - jogador.raio;
        if (jogador.pos.y - jogador.raio < 0) jogador.pos.y = jogador.raio;
        if (jogador.pos.y + jogador.raio > ALTURA_JANELA) jogador.pos.y = ALTURA_JANELA - jogador.raio;

        for (int i = 0; i < quantidadeItens; i++) {
            if (tentarColetar(itens + i, jogador.pos, jogador.raio)) {
                aplicarItem(&jogador, itens + i);
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int i = 0; i < quantidadeItens; i++) {
                if (!itens[i].coletado) {
                    DrawCircleV(itens[i].pos, itens[i].raio, corDoItem(itens + i));
                }
            }

            DrawCircleV(jogador.pos, jogador.raio, DARKBLUE);

            DrawText(TextFormat("Vida: %d", jogador.vida), 10, 10, 20, RED);
            DrawText(TextFormat("Dano: %.1f", jogador.dano), 10, 35, 20, DARKGRAY);
            DrawText(TextFormat("Armadura: %d", jogador.armadura), 10, 60, 20, BLUE);
            DrawText("Vermelho: Arma | Verde: Pocao | Roxo: Veneno | Azul: Escudo", 10, ALTURA_JANELA - 30, 14, GRAY);

        EndDrawing();
    }

    free(itens);
    CloseWindow();
    
    return 0;
}