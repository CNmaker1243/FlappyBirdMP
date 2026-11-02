#include "raylib.h"
#include <stdio.h>

#define MENU 0
#define PLAYING 1
#define GAME_OVER 2

void DesenharNuvem(int x, int y)
{
    DrawCircle(x, y, 20, RAYWHITE);
    DrawCircle(x + 20, y - 10, 25, RAYWHITE);
    DrawCircle(x + 45, y, 20, RAYWHITE);
    DrawRectangle(x - 1, y, 47, 20, RAYWHITE);
}

void DesenharPassaro(float x, float y, float raio)
{
    DrawCircle((int)x, (int)y, (int)raio, GOLD);

    DrawEllipse((int)(x - 5), (int)(y + 5), 10, 7, ORANGE);

    DrawCircle((int)(x + 8), (int)(y - 7), 6, RAYWHITE);
    DrawCircle((int)(x + 10), (int)(y - 7), 3, BLACK);

    DrawTriangle(
        (Vector2){x + raio - 3, y - 2},
        (Vector2){x + raio + 13, y + 3},
        (Vector2){x + raio - 3, y + 8},
        ORANGE
    );
}

void DesenharCanos(float x, float gapY, float larguraCano,
                   float abertura, float chaoY)
{
    float topoInferior = gapY - abertura / 2;
    float baixoSuperior = gapY + abertura / 2;

    // Cano superior
    DrawRectangle((int)x, 0, (int)larguraCano,
                  (int)topoInferior, DARKGREEN);
    DrawRectangle((int)(x + 5), 0, (int)(larguraCano - 10),
                  (int)topoInferior, GREEN);
    DrawRectangle((int)(x - 5), (int)(topoInferior - 18),
                  (int)(larguraCano + 10), 18, DARKGREEN);
    DrawRectangle((int)(x - 2), (int)(topoInferior - 15),
                  (int)(larguraCano + 4), 12, LIME);

    // Cano inferior
    DrawRectangle((int)x, (int)baixoSuperior, (int)larguraCano,
                  (int)(chaoY - baixoSuperior), DARKGREEN);
    DrawRectangle((int)(x + 5), (int)baixoSuperior,
                  (int)(larguraCano - 10),
                  (int)(chaoY - baixoSuperior), GREEN);
    DrawRectangle((int)(x - 5), (int)baixoSuperior,
                  (int)(larguraCano + 10), 18, DARKGREEN);
    DrawRectangle((int)(x - 2), (int)baixoSuperior,
                  (int)(larguraCano + 4), 12, LIME);
}

void DesenharChao(int largura, int altura, float chaoY)
{
    DrawRectangle(0, (int)chaoY, largura,
                  altura - (int)chaoY, BROWN);
    DrawRectangle(0, (int)chaoY, largura, 12, DARKGREEN);
    DrawRectangle(0, (int)chaoY, largura, 5, LIME);

    for (int x = 15; x < largura; x += 55)
    {
        DrawCircle(x, (int)chaoY + 28, 2, DARKBROWN);
    }
}

void DesenharPainel(float x, float y, float largura, float altura,
                    Color corFundo, Color corBorda)
{
    DrawRectangleRounded(
        (Rectangle){x, y, largura, altura},
        0.18f, 8, corFundo
    );
    DrawRectangleRoundedLines(
        (Rectangle){x, y, largura, altura},
        0.18f, 8, corBorda
    );
}

int main(void)
{
    const int largura = 800;
    const int altura = 450;

    InitWindow(largura, altura, "Flappy Bird - C e Raylib");
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)GetTime());

    int estadoJogo = MENU;

    float birdX = 200;
    float birdY = 200;
    float birdVelocity = 0;

    const float birdRadius = 20;
    const float gravity = 900;
    const float jumpForce = -350;

    int score = 0;
    int record = 0;
    bool scored = false;

    FILE *arquivo = fopen("recorde.txt", "r");
    if (arquivo != NULL)
    {
        if (fscanf(arquivo, "%d", &record) != 1 || record < 0)
        {
            record = 0;
        }
        fclose(arquivo);
    }

    const float groundY = 410;

    float pipeX = 800;
    const float pipeWidth = 70;
    const float pipeGap = 160;
    float gapY = 200;

    const float velocidadeInicial = 200;
    const float aumentoPorPonto = 8;
    const float velocidadeMaxima = 360;
    float pipeSpeed = velocidadeInicial;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // MENU INICIAL
        if (estadoJogo == MENU)
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                birdY = 200;
                birdVelocity = 0;

                pipeX = largura;
                gapY = (float)GetRandomValue(110, 300);

                score = 0;
                scored = false;
                pipeSpeed = velocidadeInicial;

                estadoJogo = PLAYING;
            }
        }
        // PARTIDA
        else if (estadoJogo == PLAYING)
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                birdVelocity = jumpForce;
            }

            birdVelocity += gravity * deltaTime;
            birdY += birdVelocity * deltaTime;

            if (birdY + birdRadius >= groundY)
            {
                birdY = groundY - birdRadius;
                estadoJogo = GAME_OVER;
            }

            pipeX -= pipeSpeed * deltaTime;

            if (!scored && pipeX + pipeWidth < birdX - birdRadius)
            {
                score++;
                scored = true;

                pipeSpeed = velocidadeInicial + score * aumentoPorPonto;
                if (pipeSpeed > velocidadeMaxima)
                {
                    pipeSpeed = velocidadeMaxima;
                }

                if (score > record)
                {
                    record = score;

                    FILE *arquivoRecorde = fopen("recorde.txt", "w");
                    if (arquivoRecorde != NULL)
                    {
                        fprintf(arquivoRecorde, "%d\n", record);
                        fclose(arquivoRecorde);
                    }
                }
            }

            if (pipeX + pipeWidth < 0)
            {
                pipeX = largura;
                gapY = (float)GetRandomValue(110, 300);
                scored = false;
            }

            Rectangle canoSuperior = {
                pipeX, 0, pipeWidth, gapY - pipeGap / 2
            };

            Rectangle canoInferior = {
                pipeX,
                gapY + pipeGap / 2,
                pipeWidth,
                groundY - (gapY + pipeGap / 2)
            };

            if (
                CheckCollisionCircleRec(
                    (Vector2){birdX, birdY},
                    birdRadius,
                    canoSuperior
                ) ||
                CheckCollisionCircleRec(
                    (Vector2){birdX, birdY},
                    birdRadius,
                    canoInferior
                )
            )
            {
                estadoJogo = GAME_OVER;
            }
        }
        // GAME OVER
        else if (estadoJogo == GAME_OVER)
        {
            if (IsKeyPressed(KEY_R))
            {
                estadoJogo = MENU;

                birdY = 200;
                birdVelocity = 0;
                pipeX = largura;
                gapY = (float)GetRandomValue(110, 300);

                score = 0;
                scored = false;
                pipeSpeed = velocidadeInicial;
            }
        }

        // NIVEL: muda a cada 5 pontos
        int nivel = score / 5 + 1;

        // DESENHO
        BeginDrawing();
        ClearBackground((Color){135, 206, 235, 255});

        // Nuvens
        DesenharNuvem(70, 80);
        DesenharNuvem(360, 130);
        DesenharNuvem(610, 65);

        if (estadoJogo == MENU)
        {
            // Faixa decorativa no topo
            DrawRectangle(0, 0, largura, 12, DARKBLUE);

            DesenharPainel(155, 55, 490, 285,
                           (Color){245, 250, 255, 235}, DARKBLUE);

            DrawText("FLAPPY BIRD", 218, 78, 46, DARKBLUE);
            DrawText("UMA AVENTURA NAS ALTURAS", 240, 132, 18, DARKGRAY);

            DesenharPassaro(200, 215, birdRadius);

            DrawText("DESVIE DOS CANOS!", 300, 185, 22, DARKGREEN);
            DrawText("ESPACO  -  COMECAR", 290, 225, 22, DARKBLUE);
            DrawText(TextFormat("RECORDE: %i", record),
                     315, 275, 24, ORANGE);

            DrawText("Feito em C com Raylib", 290, 315, 16, GRAY);

            DesenharChao(largura, altura, groundY);
        }
        else
        {
            DesenharCanos(pipeX, gapY, pipeWidth, pipeGap, groundY);
            DesenharChao(largura, altura, groundY);
            DesenharPassaro(birdX, birdY, birdRadius);

            // Painel de informacoes da partida
            DesenharPainel(14, 14, 220, 94,
                           (Color){255, 255, 255, 220}, DARKBLUE);

            DrawText("ESPACO: PULAR", 28, 23, 18, DARKBLUE);
            DrawText(TextFormat("Velocidade: %.0f", pipeSpeed),
                     28, 49, 18, DARKGRAY);
            DrawText(TextFormat("Nivel: %i", nivel),
                     28, 76, 20, MAROON);

            // Placar central
            DrawRectangleRounded(
                (Rectangle){345, 15, 110, 62},
                0.25f, 8, (Color){255, 255, 255, 225}
            );
            DrawText(TextFormat("%i", score), 380, 20, 34, DARKBLUE);
            DrawText("PONTOS", 366, 55, 14, DARKGRAY);

            // Recorde no canto superior direito
            DesenharPainel(590, 15, 195, 42,
                           (Color){255, 255, 255, 220}, ORANGE);
            DrawText(TextFormat("RECORDE: %i", record),
                     608, 26, 19, DARKBLUE);

            if (estadoJogo == GAME_OVER)
            {
                // Escurece o fundo para destacar o painel
                DrawRectangle(0, 0, largura, altura,
                              (Color){0, 0, 0, 125});

                DesenharPainel(170, 105, 460, 250,
                               (Color){250, 250, 255, 245}, DARKBLUE);

                DrawText("FIM DE JOGO", 270, 126, 36, RED);
                DrawText("Sua pontuacao", 300, 178, 20, DARKGRAY);
                DrawText(TextFormat("%i", score),
                         365, 204, 42, DARKBLUE);

                DrawText(TextFormat("Recorde: %i", record),
                         320, 258, 22, ORANGE);

                DrawText("APERTE R PARA VOLTAR AO MENU",
                         215, 310, 18, DARKGREEN);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
