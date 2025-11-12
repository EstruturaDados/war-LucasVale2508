#include <stdio.h>
#include <string.h>

/*
 * Estrutura: Territorio
 * Armazena os dados básicos de um território do jogo:
 * - nome: nome do território (ex: "Brasil")
 * - cor: cor do exército (ex: "Verde")
 * - tropas: quantidade de tropas posicionadas
 */
struct Territorio
{
    char nome[30];
    char cor[10];
    int tropas;
};

int main()
{
    // Declaração de um vetor para armazenar 5 territórios
    struct Territorio territorios[5];

    // Laço para cadastro dos 5 territórios
    for (int i = 0; i < 5; i++)
    {
        printf("\n=== Cadastro do Território %d ===\n", i + 1);

        // Entrada do nome do território
        printf("Digite o nome do território: ");
        scanf(" %29[^\n]", territorios[i].nome); // Lê até 29 caracteres ou até o Enter

        // Entrada da cor do exército
        printf("Digite a cor do exército: ");
        scanf(" %9s", territorios[i].cor); // Lê até 9 caracteres (sem espaços)

        // Entrada da quantidade de tropas
        printf("Digite a quantidade de tropas: ");
        scanf("%d", &territorios[i].tropas);
    }

    // Exibição dos dados após o cadastro
    printf("\n===== LISTA DE TERRITÓRIOS CADASTRADOS =====\n");
    for (int i = 0; i < 5; i++)
    {
        printf("\nTerritório %d:\n", i + 1);
        printf("Nome: %s\n", territorios[i].nome);
        printf("Cor do exército: %s\n", territorios[i].cor);
        printf("Tropas: %d\n", territorios[i].tropas);
    }

    printf("\nCadastro concluído com sucesso!\n");

    return 0;
}