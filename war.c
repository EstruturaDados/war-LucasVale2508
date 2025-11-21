#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME 30
#define MAX_COR 15

/* Estrutura de território */
typedef struct
{
    char nome[30];
    char cor[15];
    int tropas;
} Territorio;

/* Estrutura de jogador */
typedef struct
{
    char nome[MAX_NOME];
    char cor[MAX_COR];
    char *missao; /* missão alocada dinamicamente */
} Jogador;

/* ---------- Protótipos ---------- */
Territorio *criarMapa(int total);
void cadastrarTerritorios(Territorio *t, int total);
void listarTerritorios(Territorio *t, int total);
void liberarMapa(Territorio *mapa);

void atribuirMissao(char *destino, char *missoes[], int totalMissoes, Territorio *mapa, int totalMapa);
char *alocarCopiaString(const char *src);
void exibirMissao(const char *missao); /* missão passada por valor (ponteiro por valor) */
int verificarMissao(char *missao, Territorio *mapa, int total, const char *corJogador);

int rolarDado();
void atacar(Territorio *atacante, Territorio *defensor);
void liberarMemoria(Territorio *mapa, Jogador *players, int numPlayers);

/* ---------- main ---------- */
int main()
{
    srand((unsigned)time(NULL));

    int total;
    printf("Digite quantos territórios deseja cadastrar: ");
    if (scanf("%d", &total) != 1 || total <= 0)
    {
        printf("Entrada inválida. Encerrando.\n");
        return 1;
    }

    /* alocação dinâmica do mapa */
    Territorio *territorios = criarMapa(total);
    if (!territorios)
    {
        printf("Erro ao alocar mapa.\n");
        return 1;
    }

    cadastrarTerritorios(territorios, total);

    /* Configuração de jogadores (fixo 2 jogadores) */
    int numPlayers = 2;
    Jogador players[2];
    for (int i = 0; i < numPlayers; i++)
    {
        printf("\n=== Jogador %d ===\n", i + 1);
        printf("Nome: ");
        scanf(" %29[^\n]", players[i].nome);
        printf("Cor do jogador (ex: Vermelho, Azul): ");
        scanf(" %14s", players[i].cor);
        players[i].missao = NULL;
    }

    /* Vetor de templates de missão (pelo menos 5) */
    char *templates[] = {
        "Conquistar 3 territorios seguidos",
        "Eliminar todas as tropas da cor %s",
        "Controlar maioria dos territorios",
        "Conquistar o territorio %s",
        "Conquistar 2 territorios da cor %s"};
    int totalTemplates = sizeof(templates) / sizeof(templates[0]);

    /* Atribui missão a cada jogador: sorteia e armazena dinamicamente */
    for (int i = 0; i < numPlayers; i++)
    {
        char buffer[200];
        atribuirMissao(buffer, templates, totalTemplates, territorios, total);
        players[i].missao = alocarCopiaString(buffer);
        if (!players[i].missao)
        {
            printf("Erro ao alocar missão.\n");
            liberarMemoria(territorios, players, numPlayers);
            return 1;
        }
        /* Exibe missão apenas uma vez (por valor: ponteiro passado por valor) */
        printf("\nMissão do jogador %s (cor %s):\n  -> ", players[i].nome, players[i].cor);
        exibirMissao(players[i].missao);
    }

    /* Loop principal */
    int op;
    while (1)
    {
        printf("\n===== MENU =====\n");
        printf("1 - Listar territórios\n");
        printf("2 - Atacar\n");
        printf("3 - Ver missões (somente leitura)\n");
        printf("4 - Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &op) != 1)
        {
            printf("Entrada inválida. Encerrando.\n");
            break;
        }

        if (op == 1)
        {
            listarTerritorios(territorios, total);
        }
        else if (op == 2)
        {
            listarTerritorios(territorios, total);

            int idJogador;
            printf("Qual jogador está atacando? (1 ou 2): ");
            if (scanf("%d", &idJogador) != 1 || idJogador < 1 || idJogador > numPlayers)
            {
                printf("Jogador inválido.\n");
                continue;
            }
            idJogador--; /* índice 0 ou 1 */

            int idxAtq, idxDef;
            printf("Escolha o índice do território atacante: ");
            if (scanf("%d", &idxAtq) != 1)
            {
                printf("Entrada inválida.\n");
                continue;
            }
            printf("Escolha o índice do território defensor: ");
            if (scanf("%d", &idxDef) != 1)
            {
                printf("Entrada inválida.\n");
                continue;
            }

            /* Validações */
            if (idxAtq < 0 || idxAtq >= total || idxDef < 0 || idxDef >= total)
            {
                printf("Índices inválidos.\n");
                continue;
            }
            if (idxAtq == idxDef)
            {
                printf("Um território não pode atacar a si mesmo.\n");
                continue;
            }
            /* jogador deve controlar o território atacante */
            if (strcmp(territorios[idxAtq].cor, players[idJogador].cor) != 0)
            {
                printf("Erro: o jogador %s não controla o território atacante %s.\n",
                       players[idJogador].nome, territorios[idxAtq].nome);
                continue;
            }
            /* não atacar território da mesma cor */
            if (strcmp(territorios[idxAtq].cor, territorios[idxDef].cor) == 0)
            {
                printf("Erro: não pode atacar território da mesma cor.\n");
                continue;
            }
            /* atacante precisa ter tropas */
            if (territorios[idxAtq].tropas <= 0)
            {
                printf("O território atacante não tem tropas suficientes.\n");
                continue;
            }

            /* Executa ataque (usa ponteiros) */
            atacar(&territorios[idxAtq], &territorios[idxDef]);

            /* Exibe estado pós-ataque */
            printf("\n--- Estado pós-ataque ---\n");
            printf("Atacante: %s | Cor: %s | Tropas: %d\n",
                   territorios[idxAtq].nome, territorios[idxAtq].cor, territorios[idxAtq].tropas);
            printf("Defensor: %s | Cor: %s | Tropas: %d\n",
                   territorios[idxDef].nome, territorios[idxDef].cor, territorios[idxDef].tropas);

            /* Verificar missões de cada jogador (silenciosamente) */
            for (int p = 0; p < numPlayers; p++)
            {
                if (verificarMissao(players[p].missao, territorios, total, players[p].cor))
                {
                    printf("\n=====================================\n");
                    printf("Missão cumprida! Jogador %s (cor %s) venceu!\n", players[p].nome, players[p].cor);
                    printf("Missão: %s\n", players[p].missao);
                    printf("=====================================\n");
                    liberarMemoria(territorios, players, numPlayers);
                    return 0;
                }
            }

            /* Verifica fim de jogo tradicional (todas as cores iguais) */
            int fim = 1;
            for (int i = 1; i < total; i++)
            {
                if (strcmp(territorios[0].cor, territorios[i].cor) != 0)
                {
                    fim = 0;
                    break;
                }
            }
            if (fim)
            {
                printf("\n===== FIM DE JOGO =====\n");
                printf("Todos os territórios agora pertencem ao exército: %s\n", territorios[0].cor);
                liberarMemoria(territorios, players, numPlayers);
                return 0;
            }
        }
        else if (op == 3)
        {
            for (int i = 0; i < numPlayers; i++)
            {
                printf("\nJogador %s (cor %s) - Missão: %s\n", players[i].nome, players[i].cor, players[i].missao);
            }
        }
        else if (op == 4)
        {
            printf("Encerrando o jogo.\n");
            break;
        }
        else
        {
            printf("Opção inválida.\n");
        }
    }

    /* libera memória */
    liberarMemoria(territorios, players, numPlayers);
    return 0;
}

/* ---------- Implementações ---------- */

/* aloca dinamicamente o mapa */
Territorio *criarMapa(int total)
{
    Territorio *m = (Territorio *)malloc(total * sizeof(Territorio));
    return m;
}

/* cadastra territórios via entrada */
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

/* lista territórios */
void listarTerritorios(Territorio *t, int total)
{
    printf("\n===== TERRITÓRIOS =====\n");
    for (int i = 0; i < total; i++)
    {
        printf("[%d]  %s | Cor: %s | Tropas: %d\n", i, t[i].nome, t[i].cor, t[i].tropas);
    }
}

/* atribui missão sorteada para destino (destino é buffer fornecido pelo chamador) */
void atribuirMissao(char *destino, char *missoes[], int totalMissoes, Territorio *mapa, int totalMapa)
{
    int idx = rand() % totalMissoes;
    char temp[200];

    /* Se o template contém %s, preenche com cor ou nome de território aleatório */
    if (strstr(missoes[idx], "%s") != NULL)
    {
        /* decide se vai preencher com cor ou nome conforme template */
        if (strstr(missoes[idx], "tropas") != NULL || strstr(missoes[idx], "cor") != NULL)
        {
            int r = rand() % totalMapa;
            snprintf(temp, sizeof(temp), missoes[idx], mapa[r].cor);
        }
        else if (strstr(missoes[idx], "territorio") != NULL)
        {
            int r = rand() % totalMapa;
            snprintf(temp, sizeof(temp), missoes[idx], mapa[r].nome);
        }
        else
        {
            /* fallback */
            int r = rand() % totalMapa;
            snprintf(temp, sizeof(temp), missoes[idx], mapa[r].cor);
        }
    }
    else
    {
        strncpy(temp, missoes[idx], sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = '\0';
    }

    /* copia para destino (usado por chamador para alocar dinamicamente depois) */
    strncpy(destino, temp, 199);
    destino[199] = '\0';
}

/* aloca dinâmica e copia string */
char *alocarCopiaString(const char *src)
{
    size_t n = strlen(src);
    char *p = (char *)malloc(n + 1);
    if (p)
        strcpy(p, src);
    return p;
}

/* exibe missão (passada por valor - ponteiro copiado por valor) */
void exibirMissao(const char *missao)
{
    printf("%s\n", missao);
}

/* rola dado 1..6 */
int rolarDado()
{
    return rand() % 6 + 1;
}

/* implementar atacar conforme solicitado:
   - usa rand() para rolar um dado 1..6 para cada lado
   - se atacante vencer: defensor troca de cor e recebe metade das tropas do atacante (mínimo 1)
   - se atacante perder ou empatar: atacante perde 1 tropa
*/
void atacar(Territorio *atacante, Territorio *defensor)
{
    printf("\n--- Ataque: %s (%s, %d tropas) atacando %s (%s, %d tropas) ---\n",
           atacante->nome, atacante->cor, atacante->tropas,
           defensor->nome, defensor->cor, defensor->tropas);

    int dadoAtq = rolarDado();
    int dadoDef = rolarDado();
    printf("Rolagem: Atacante = %d | Defensor = %d\n", dadoAtq, dadoDef);

    if (dadoAtq > dadoDef)
    {
        printf("Resultado: Atacante venceu!\n");
        /* transfere cor */
        strncpy(defensor->cor, atacante->cor, MAX_COR - 1);
        defensor->cor[MAX_COR - 1] = '\0';
        /* transfere metade das tropas */
        int transferidas = atacante->tropas / 2;
        if (transferidas < 1)
            transferidas = 1;
        defensor->tropas = transferidas;
        atacante->tropas -= transferidas;
        if (atacante->tropas < 0)
            atacante->tropas = 0;
        printf("%s agora pertence a %s e tem %d tropas (transferidas).\n",
               defensor->nome, defensor->cor, defensor->tropas);
    }
    else
    {
        printf("Resultado: Defensor resistiu!\n");
        atacante->tropas -= 1;
        if (atacante->tropas < 0)
            atacante->tropas = 0;
        printf("Atacante perde 1 tropa. Tropas restantes do atacante: %d\n", atacante->tropas);
    }
}

/* verificarMissao: lógica simples para os templates usados.
   Retorna 1 se missão cumprida, 0 caso contrário.
   Nota: a função recebe a missão já formatada (com cor/nome preenchido),
   e a cor do jogador (para algumas verificações).
*/
int verificarMissao(char *missao, Territorio *mapa, int total, const char *corJogador)
{
    /* 1) Conquistar 3 territorios seguidos */
    if (strstr(missao, "Conquistar 3 territorios seguidos") != NULL)
    {
        int consec = 0;
        for (int i = 0; i < total; i++)
        {
            if (strcmp(mapa[i].cor, corJogador) == 0)
            {
                consec++;
                if (consec >= 3)
                    return 1;
            }
            else
                consec = 0;
        }
        return 0;
    }

    /* 2) Eliminar todas as tropas da cor X
       extrai cor alvo do texto: assume formato "Eliminar todas as tropas da cor <X>"
    */
    if (strstr(missao, "Eliminar todas as tropas da cor") != NULL)
    {
        char corAlvo[MAX_COR];
        if (sscanf(missao, "Eliminar todas as tropas da cor %14s", corAlvo) == 1)
        {
            int soma = 0;
            for (int i = 0; i < total; i++)
            {
                if (strcmp(mapa[i].cor, corAlvo) == 0)
                    soma += mapa[i].tropas;
            }
            return (soma == 0) ? 1 : 0;
        }
        return 0;
    }

    /* 3) Controlar maioria dos territorios (mais de 50%) */
    if (strstr(missao, "Controlar maioria dos territorios") != NULL)
    {
        int count = 0;
        for (int i = 0; i < total; i++)
            if (strcmp(mapa[i].cor, corJogador) == 0)
                count++;
        return (count > total / 2) ? 1 : 0;
    }

    /* 4) Conquistar o territorio NAME */
    if (strstr(missao, "Conquistar o territorio") != NULL)
    {
        char nomeAlvo[30];
        if (sscanf(missao, "Conquistar o territorio %29[^\n]", nomeAlvo) >= 1)
        {
            for (int i = 0; i < total; i++)
            {
                if (strcmp(mapa[i].nome, nomeAlvo) == 0)
                {
                    return (strcmp(mapa[i].cor, corJogador) == 0) ? 1 : 0;
                }
            }
        }
        return 0;
    }

    /* 5) Conquistar 2 territorios da cor X
       verificamos quantos territorios da cor alvo agora pertencem ao jogador (simplificação)
    */
    if (strstr(missao, "Conquistar 2 territorios da cor") != NULL)
    {
        char corAlvo[MAX_COR];
        if (sscanf(missao, "Conquistar 2 territorios da cor %14s", corAlvo) == 1)
        {
            int cont = 0;
            for (int i = 0; i < total; i++)
            {
                /* simplificação: conta quantos territórios agora têm a cor do jogador */
                if (strcmp(mapa[i].cor, corJogador) == 0)
                    cont++;
            }
            return (cont >= 2) ? 1 : 0;
        }
        return 0;
    }

    /* se não reconheceu o template, considera não cumprida */
    return 0;
}

/* libera mapa e missões dos jogadores */
void liberarMemoria(Territorio *mapa, Jogador *players, int numPlayers)
{
    if (mapa)
        free(mapa);
    for (int i = 0; i < numPlayers; i++)
    {
        if (players[i].missao)
        {
            free(players[i].missao);
            players[i].missao = NULL;
        }
    }
}