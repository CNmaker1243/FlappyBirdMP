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

void DesenharPassaro(float x, float y, float raio, Color cor)
{
    DrawCircle((int)x, (int)y, (int)raio, cor);
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

    DrawRectangle((int)x, 0, (int)larguraCano,
                  (int)topoInferior, DARKGREEN);
    DrawRectangle((int)(x + 5), 0, (int)(larguraCano - 10),
                  (int)topoInferior, GREEN);
    DrawRectangle((int)(x - 5), (int)(topoInferior - 18),
                  (int)(larguraCano + 10), 18, DARKGREEN);
    DrawRectangle((int)(x - 2), (int)(topoInferior - 15),
                  (int)(larguraCano + 4), 12, LIME);

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

    InitWindow(largura, altura, "Flappy Bird Multiplayer");
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)GetTime());

    int estadoJogo = MENU;

    // Jogador 1
    float bird1X = 200;
    float bird1Y = 170;
    float bird1Velocity = 0;
    bool bird1Alive = true;

    // Jogador 2
    float bird2X = 200;
    float bird2Y = 240;
    float bird2Velocity = 0;
    bool bird2Alive = true;

    const float birdRadius = 18;
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
    const float pipeGap = 175;
    float gapY = 220;

    const float velocidadeInicial = 200;
    const float aumentoPorPonto = 8;
    const float velocidadeMaxima = 360;
    float pipeSpeed = velocidadeInicial;

    // Tempo sobrevivido por jogador
    float tempoJogador1 = 0;
    float tempoJogador2 = 0;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        if (estadoJogo == MENU)
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                bird1Y = 170;
                bird2Y = 240;
                bird1Velocity = 0;
                bird2Velocity = 0;
                bird1Alive = true;
                bird2Alive = true;

                pipeX = largura;
                gapY = (float)GetRandomValue(125, 285);
                score = 0;
                scored = false;
                pipeSpeed = velocidadeInicial;
                tempoJogador1 = 0;
                tempoJogador2 = 0;

                estadoJogo = PLAYING;
            }
        }
        else if (estadoJogo == PLAYING)
        {
            // Controles independentes: ESPACO e SETA PARA CIMA
            if (bird1Alive && IsKeyPressed(KEY_SPACE))
            {
                bird1Velocity = jumpForce;
            }

            if (bird2Alive && IsKeyPressed(KEY_UP))
            {
                bird2Velocity = jumpForce;
            }

            // Física independente do jogador 1
            if (bird1Alive)
            {
                tempoJogador1 += deltaTime;
                bird1Velocity += gravity * deltaTime;
                bird1Y += bird1Velocity * deltaTime;

                if (bird1Y + birdRadius >= groundY)
                {
                    bird1Y = groundY - birdRadius;
                    bird1Alive = false;
                }
            }

            // Física independente do jogador 2
            if (bird2Alive)
            {
                tempoJogador2 += deltaTime;
                bird2Velocity += gravity * deltaTime;
                bird2Y += bird2Velocity * deltaTime;

                if (bird2Y + birdRadius >= groundY)
                {
                    bird2Y = groundY - birdRadius;
                    bird2Alive = false;
                }
            }

            // Os canos continuam andando enquanto houver jogador vivo
            if (bird1Alive || bird2Alive)
            {
                pipeX -= pipeSpeed * deltaTime;
            }

            // Pontuação compartilhada: passar por um cano soma 1 ponto
            if (!scored && pipeX + pipeWidth < 200 - birdRadius)
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
                gapY = (float)GetRandomValue(125, 285);
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

            // Colisões independentes
            if (bird1Alive &&
                (CheckCollisionCircleRec(
                    (Vector2){bird1X, bird1Y},
                    birdRadius, canoSuperior
                ) ||
                 CheckCollisionCircleRec(
                    (Vector2){bird1X, bird1Y},
                    birdRadius, canoInferior
                )))
            {
                bird1Alive = false;
            }

            if (bird2Alive &&
                (CheckCollisionCircleRec(
                    (Vector2){bird2X, bird2Y},
                    birdRadius, canoSuperior
                ) ||
                 CheckCollisionCircleRec(
                    (Vector2){bird2X, bird2Y},
                    birdRadius, canoInferior
                )))
            {
                bird2Alive = false;
            }

            // A partida termina quando os dois jogadores são eliminados
            if (!bird1Alive && !bird2Alive)
            {
                estadoJogo = GAME_OVER;
            }
        }
        else if (estadoJogo == GAME_OVER)
        {
            if (IsKeyPressed(KEY_R))
            {
                estadoJogo = MENU;
            }
        }

        int nivel = score / 5 + 1;

        BeginDrawing();
        ClearBackground((Color){135, 206, 235, 255});

        DesenharNuvem(70, 80);
        DesenharNuvem(360, 130);
        DesenharNuvem(610, 65);

        if (estadoJogo == MENU)
        {
            DrawRectangle(0, 0, largura, 12, DARKBLUE);
            DesenharPainel(130, 45, 540, 320,
                           (Color){245, 250, 255, 240}, DARKBLUE);

            DrawText("FLAPPY BIRD", 225, 65, 44, DARKBLUE);
            DrawText("MULTIPLAYER LOCAL", 270, 115, 22, DARKGREEN);

            DesenharPassaro(220, 175, birdRadius, GOLD);
            DesenharPassaro(220, 230, birdRadius, SKYBLUE);

            DrawText("JOGADOR 1: ESPACO", 285, 165, 20, DARKBLUE);
            DrawText("JOGADOR 2: SETA PARA CIMA", 285, 220, 18, DARKBLUE);
            DrawText("Cada jogador tenta sobreviver!", 230, 265, 19, DARKGRAY);
            DrawText("ESPACO PARA COMECAR", 260, 305, 22, MAROON);
            DrawText(TextFormat("RECORDE: %i", record),
                     315, 337, 20, ORANGE);

            DesenharChao(largura, altura, groundY);
        }
        else
        {
            DesenharCanos(pipeX, gapY, pipeWidth, pipeGap, groundY);
            DesenharChao(largura, altura, groundY);

            // Jogador 1 dourado; jogador 2 azul-claro
            if (bird1Alive)
                DesenharPassaro(bird1X, bird1Y, birdRadius, GOLD);
            else
                DrawText("J1 ELIMINADO", 25, 120, 18, MAROON);

            if (bird2Alive)
                DesenharPassaro(bird2X, bird2Y, birdRadius, SKYBLUE);
            else
                DrawText("J2 ELIMINADO", 25, 145, 18, DARKBLUE);

            DesenharPainel(14, 14, 220, 92,
                           (Color){255, 255, 255, 220}, DARKBLUE);
            DrawText("J1: ESPACO", 28, 22, 18, (Color){184, 134, 11, 255});
            DrawText("J2: SETA PARA CIMA", 28, 45, 17, DARKBLUE);
            DrawText(TextFormat("Nivel: %i", nivel), 28, 73, 20, MAROON);

            DrawRectangleRounded(
                (Rectangle){345, 15, 110, 62},
                0.25f, 8, (Color){255, 255, 255, 225}
            );
            DrawText(TextFormat("%i", score), 380, 20, 34, DARKBLUE);
            DrawText("PONTOS", 366, 55, 14, DARKGRAY);

            DesenharPainel(590, 15, 195, 42,
                           (Color){255, 255, 255, 220}, ORANGE);
            DrawText(TextFormat("RECORDE: %i", record),
                     608, 26, 19, DARKBLUE);

            if (estadoJogo == GAME_OVER)
            {
                DrawRectangle(0, 0, largura, altura,
                              (Color){0, 0, 0, 125});
                DesenharPainel(155, 90, 490, 280,
                               (Color){250, 250, 255, 245}, DARKBLUE);

                DrawText("FIM DE JOGO", 275, 110, 36, RED);

                const char *resultado;
                if (tempoJogador1 > tempoJogador2)
                    resultado = "JOGADOR 1 VENCEU!";
                else if (tempoJogador2 > tempoJogador1)
                    resultado = "JOGADOR 2 VENCEU!";
                else
                    resultado = "EMPATE!";

                DrawText(resultado, 245, 165, 27, DARKGREEN);
                DrawText(TextFormat("J1 sobreviveu: %.1f s", tempoJogador1),
                         260, 215, 20, (Color){184, 134, 11, 255});
                DrawText(TextFormat("J2 sobreviveu: %.1f s", tempoJogador2),
                         260, 245, 20, DARKBLUE);
                DrawText(TextFormat("Pontuacao compartilhada: %i", score),
                         235, 280, 19, DARKGRAY);
                DrawText("APERTE R PARA VOLTAR AO MENU",
                         205, 325, 18, DARKGREEN);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
