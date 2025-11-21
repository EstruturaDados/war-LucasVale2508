#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ------------------------------------------------------------
// STRUCT DO TERRITÓRIO
// ------------------------------------------------------------
typedef struct
{
    char nome[30];
    char cor[15];
    int tropas;
} Territorio;

// ------------------------------------------------------------
// PROTÓTIPOS DAS FUNÇÕES
// ------------------------------------------------------------
void cadastrarTerritorios(Territorio *t, int total);
void listarTerritorios(Territorio *t, int total);
int rolarDado();
void ordenarDesc(int v[], int tam);
void combate(Territorio *t, int atacante, int defensor);
int fimDeJogo(Territorio *t, int total);

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main()
{
    srand(time(NULL));

    int total;
    printf("Digite quantos territórios deseja cadastrar: ");
    scanf("%d", &total);

    // ALOCAÇÃO DINÂMICA — Agora funciona em qualquer compilador!
    Territorio *territorios = malloc(total * sizeof(Territorio));
    if (!territorios)
    {
        printf("Erro ao alocar memória!\n");
        return 1;
    }

    cadastrarTerritorios(territorios, total);

    int op, atacante, defensor;

    while (1)
    {
        printf("\n===== MENU =====\n");
        printf("1 - Listar territórios\n");
        printf("2 - Atacar\n");
        printf("3 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &op);

        if (op == 1)
        {
            listarTerritorios(territorios, total);
        }
        else if (op == 2)
        {
            listarTerritorios(territorios, total);

            printf("\nEscolha o território atacante: ");
            scanf("%d", &atacante);

            printf("Escolha o território defensor: ");
            scanf("%d", &defensor);

            // validações
            if (atacante < 0 || atacante >= total ||
                defensor < 0 || defensor >= total)
            {
                printf("Índice inválido!\n");
                continue;
            }

            if (atacante == defensor)
            {
                printf("Um território não pode atacar a si mesmo!\n");
                continue;
            }

            if (strcmp(territorios[atacante].cor, territorios[defensor].cor) == 0)
            {
                printf("Não pode atacar territórios da mesma cor!\n");
                continue;
            }

            combate(territorios, atacante, defensor);

            // Caso defensor perca todas as tropas → território é conquistado
            if (territorios[defensor].tropas <= 0)
            {
                printf("\n=== TERRITÓRIO CONQUISTADO! ===\n");
                strcpy(territorios[defensor].cor, territorios[atacante].cor);
                territorios[defensor].tropas = 1;
                territorios[atacante].tropas -= 1;
            }

            // verifica fim do jogo
            if (fimDeJogo(territorios, total))
            {
                printf("\n===== FIM DE JOGO =====\n");
                printf("Todos os territórios agora pertencem ao exército: %s\n",
                       territorios[0].cor);
                break;
            }
        }
        else if (op == 3)
        {
            printf("Saindo...\n");
            break;
        }
        else
        {
            printf("Opção inválida!\n");
        }
    }

    free(territorios); // libera memória
    return 0;
}

// ------------------------------------------------------------
// CADASTRO
// ------------------------------------------------------------
void cadastrarTerritorios(Territorio *t, int total)
{
    for (int i = 0; i < total; i++)
    {
        printf("\n===== Cadastro do Território %d =====\n", i);

        printf("Nome: ");
        scanf(" %29[^\n]", t[i].nome);

        printf("Cor do exército: ");
        scanf(" %14s", t[i].cor);

        printf("Tropas: ");
        scanf("%d", &t[i].tropas);
    }
}

// ------------------------------------------------------------
// LISTAR
// ------------------------------------------------------------
void listarTerritorios(Territorio *t, int total)
{
    printf("\n===== TERRITÓRIOS =====\n");
    for (int i = 0; i < total; i++)
    {
        printf("[%d]  %s | Cor: %s | Tropas: %d\n",
               i, t[i].nome, t[i].cor, t[i].tropas);
    }
}

// ------------------------------------------------------------
// DADOS
// ------------------------------------------------------------
int rolarDado()
{
    return rand() % 6 + 1;
}

void ordenarDesc(int v[], int tam)
{
    for (int i = 0; i < tam - 1; i++)
    {
        for (int j = i + 1; j < tam; j++)
        {
            if (v[j] > v[i])
            {
                int aux = v[i];
                v[i] = v[j];
                v[j] = aux;
            }
        }
    }
}

// ------------------------------------------------------------
// SISTEMA DE COMBATE
// ------------------------------------------------------------
void combate(Territorio *t, int atacante, int defensor)
{
    int dadosAtq = (t[atacante].tropas >= 3) ? 3 : (t[atacante].tropas == 2 ? 2 : 1);

    int dadosDef = (t[defensor].tropas >= 2) ? 2 : 1;

    int atq[3];
    int def[2];

    printf("\n===== ATAQUE INICIADO =====\n");

    printf("Dados do atacante (%d dados): ", dadosAtq);
    for (int i = 0; i < dadosAtq; i++)
    {
        atq[i] = rolarDado();
        printf("%d ", atq[i]);
    }

    printf("\nDados do defensor (%d dados): ", dadosDef);
    for (int i = 0; i < dadosDef; i++)
    {
        def[i] = rolarDado();
        printf("%d ", def[i]);
    }
    printf("\n");

    ordenarDesc(atq, dadosAtq);
    ordenarDesc(def, dadosDef);

    int comparacoes = (dadosAtq < dadosDef) ? dadosAtq : dadosDef;

    for (int i = 0; i < comparacoes; i++)
    {
        if (atq[i] > def[i])
        {
            printf("Atacante vence duelo: %d vs %d\n", atq[i], def[i]);
            t[defensor].tropas--;
        }
        else
        {
            printf("Defensor vence duelo: %d vs %d\n", def[i], atq[i]);
            t[atacante].tropas--;
        }
    }
}

// ------------------------------------------------------------
// FIM DE JOGO (TODOS TERRITÓRIOS MESMA COR)
// ------------------------------------------------------------
int fimDeJogo(Territorio *t, int total)
{
    for (int i = 1; i < total; i++)
    {
        if (strcmp(t[0].cor, t[i].cor) != 0)
            return 0;
    }
    return 1;
}