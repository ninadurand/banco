#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct Conta {       //dados sob um unico nome 
    char numeroConta[20];
    float saldo;
    char nome[50];    //array de caracteres 
    char cpf[15];
    char telefone[20];
    int status;
};

int buscaConta(const char *numeroContaBusca);     // prototipo 
void salvarContaArquivo(const struct Conta *conta);
void carregarContasArquivo();
void consultarSaldo(const char *numeroContaConsulta);
void depositar(const char *numeroContaDeposito, float valorDeposito);
void sacar(const char *numeroContaSaque, float valorSaque);
void exibirContas();
void removerConta(const char *numeroContaRemover);
void limparContasInativas();
void cadastrarCliente();

int main() {
    char numeroContaConsulta[20], numeroContaDeposito[20], numeroContaSaque[20], numeroContaRemover[20];
    int opcao;

    FILE *arquivoExistente = fopen("cadastros.bin", "rb"); // executa quando abre independentemente sendo um ponteiro q agrupa o endereo de memoriaaa onde os arquivos vao ser mantidos 
    if (arquivoExistente == NULL) {                       //fopen abre arquivo e retorna oponteiro para o tipo file 
        FILE *criarArquivo = fopen("cadastros.bin", "wb");//cria arquivo se existe fecha 
        if (criarArquivo == NULL) {
            printf("Erro ao criar o arquivo para cadastros.\n");
            return EXIT_FAILURE;
        }
        fclose(criarArquivo);
    } else {
        fclose(arquivoExistente);
    }

    carregarContasArquivo();

    do {
        // MENU
        printf("\n---- Menu ----\n");
        printf("1. Cadastrar nova conta\n");
        printf("2. Consultar Saldo\n");
        printf("3. Fazer depósito\n");
        printf("4. Fazer saque\n");
        printf("5. Exibir contas\n");
        printf("6. Remover conta\n");
        printf("0. Sair\n");
        // MENU

        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                cadastrarCliente();
                break;
            case 2:
                printf("Informe o número da conta para consultar o saldo: ");
                scanf("%s", numeroContaConsulta);
                consultarSaldo(numeroContaConsulta);
                break;
            case 3:
                printf("Informe o número da conta para fazer o depósito: ");
                scanf("%s", numeroContaDeposito);
                float valorDeposito;
                printf("Informe o valor do depósito: ");
                scanf("%f", &valorDeposito);
                depositar(numeroContaDeposito, valorDeposito);
                break;
            case 4:
                printf("Informe o número da conta para fazer o saque: ");
                scanf("%s", numeroContaSaque);
                float valorSaque;
                printf("Informe o valor do saque: ");
                scanf("%f", &valorSaque);
                sacar(numeroContaSaque, valorSaque);
                break;
            case 5:
                exibirContas();
                break;
            case 6:
                printf("Informe o número da conta para remover: ");
                scanf("%s", numeroContaRemover);
                removerConta(numeroContaRemover);
                break;
            case 0:
                printf("Saindo do programa...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    limparContasInativas();

    return 0;
}


void cadastrarCliente() {
    struct Conta novaConta;

    printf("Informe o número da conta: ");
    scanf("%s", novaConta.numeroConta);

    int posicaoExistente = buscaConta(novaConta.numeroConta);
    if (posicaoExistente != -1) {
        printf("Número de conta já existente. Cadastro não realizado.\n");
        return;
    }

    printf("Informe o saldo: ");
    scanf("%f", &(novaConta.saldo));

    printf("Informe o nome: ");
    fflush(stdin);
    fgets(novaConta.nome, sizeof(novaConta.nome), stdin);  // pega o espaço e tira o //n 
    novaConta.nome[strcspn(novaConta.nome, "\n")] = '\0';

    printf("Informe o CPF: ");
    fflush(stdin);
    fgets(novaConta.cpf, sizeof(novaConta.cpf), stdin);
    novaConta.cpf[strcspn(novaConta.cpf, "\n")] = '\0';

    printf("Informe o telefone: ");
	fflush(stdin);
	fgets(novaConta.telefone, sizeof(novaConta.telefone), stdin);
	novaConta.telefone[strcspn(novaConta.telefone, "\n")] = '\0';

    novaConta.status = 1;

    salvarContaArquivo(&novaConta);
}


void consultarSaldo(const char *numeroContaConsulta) {
    int posicao = buscaConta(numeroContaConsulta);

    if (posicao >= 0) {
        FILE *arquivo = fopen("cadastros.bin", "rb");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo para leitura.\n");
            exit(EXIT_FAILURE);
        }

        struct Conta conta;
        fseek(arquivo, posicao * sizeof(struct Conta), SEEK_SET);   // move o ponteiro do arquivo para a posiçao da conta no arquivo 
        fread(&conta, sizeof(struct Conta), 1, arquivo);

        fclose(arquivo);

        printf("Saldo da conta %s: %.2f\n", numeroContaConsulta, conta.saldo);
    } else if (posicao == -1) {
        printf("Conta %s não cadastrada.\n", numeroContaConsulta);
    } else if (posicao == -2) {
        printf("Cadastro vazio.\n");
    } else {
        printf("Erro de leitura durante a busca.\n");
    }
}

void exibirContas() {
    FILE *arquivo = fopen("cadastros.bin", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        exit(EXIT_FAILURE);
    }

    struct Conta conta;

    printf("\n---- Lista de Contas ----\n");
    while (fread(&conta, sizeof(struct Conta), 1, arquivo) == 1) {
        if (conta.status == 1) {
            printf("Número da conta: %s\n", conta.numeroConta);
            printf("Nome do Titular: %s\n", conta.nome);
            printf("Telefone: %s\n", conta.telefone);
            printf("------------------------\n");
        }
    }

    fclose(arquivo);
}

void removerConta(const char *numeroContaRemover) {
    int posicao = buscaConta(numeroContaRemover);

    if (posicao >= 0) {
        FILE *arquivo = fopen("cadastros.bin", "rb+");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo para leitura e escrita.\n");
            exit(EXIT_FAILURE);
        }

        struct Conta conta;
        fseek(arquivo, posicao * sizeof(struct Conta), SEEK_SET);
        fread(&conta, sizeof(struct Conta), 1, arquivo);

        conta.status = 0;

        fseek(arquivo, posicao * sizeof(struct Conta), SEEK_SET);
        fwrite(&conta, sizeof(struct Conta), 1, arquivo);

        fclose(arquivo);

        printf("Conta %s removida com sucesso.\n", numeroContaRemover);
    } else if (posicao == -1) {
        printf("Conta %s não cadastrada.\n", numeroContaRemover);
    } else if (posicao == -2) {
        printf("Cadastro vazio.\n");
    } else {
        printf
        ("Erro de leitura durante a busca.\n");
    }
}

void depositar(const char *numeroContaDeposito, float valorDeposito) {
    int posicao = buscaConta(numeroContaDeposito);

    if (posicao >= 0) {
        FILE *arquivo = fopen("cadastros.bin", "rb+");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo para leitura e escrita.\n");
            exit(EXIT_FAILURE);
        }

        struct Conta conta;
        fseek(arquivo, posicao * sizeof(struct Conta), SEEK_SET);
        fread(&conta, sizeof(struct Conta), 1, arquivo);

        if (valorDeposito >= 0) {
            conta.saldo += valorDeposito;

            fseek(arquivo, posicao * sizeof(struct Conta), SEEK_SET);
            fwrite(&conta, sizeof(struct Conta), 1, arquivo);

            fclose(arquivo);

            printf("Depósito de %.2f realizado na conta %s. Novo saldo: %.2f\n", valorDeposito, numeroContaDeposito, conta.saldo);
        } else {
            fclose(arquivo);
            printf("Valor de depósito inválido. Tente novamente.\n");
        }
    } else if (posicao == -1) {
        printf("Conta %s não cadastrada.\n", numeroContaDeposito);
    } else if (posicao == -2) {
        printf("Cadastro vazio.\n");
    } else {
        printf("Erro de leitura durante a busca.\n");
    }
}

void sacar(const char *numeroContaSaque, float valorSaque) {
    int posicao = buscaConta(numeroContaSaque);

    if (posicao >= 0) {
        FILE *arquivo = fopen("cadastros.bin", "rb+");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo para leitura e escrita.\n");
            exit(EXIT_FAILURE);
        }

        struct Conta conta;
        fseek(arquivo, posicao * sizeof(struct Conta), SEEK_SET);
        fread(&conta, sizeof(struct Conta), 1, arquivo);

        if (valorSaque >= 0 && valorSaque <= conta.saldo) {
            conta.saldo -= valorSaque;

            fseek(arquivo, posicao * sizeof(struct Conta), SEEK_SET);
            fwrite(&conta, sizeof(struct Conta), 1, arquivo);

            fclose(arquivo);

            printf("Saque de %.2f realizado na conta %s. Novo saldo: %.2f\n", valorSaque, numeroContaSaque, conta.saldo);
        } else {
            fclose(arquivo);
            printf("Valor de saque inválido. Tente novamente.\n");
        }
    } else if (posicao == -1) {
        printf("Conta %s não cadastrada.\n", numeroContaSaque);
    } else if (posicao == -2) {
        printf("Cadastro vazio.\n");
    } else {
        printf("Erro de leitura durante a busca.\n");
    }
}

void limparContasInativas() {
    FILE *arquivo = fopen("cadastros.bin", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        exit(EXIT_FAILURE);
    }

    FILE *temp = fopen("temp.txt", "wb");
    if (temp == NULL) {
        printf("Erro ao criar o arquivo temporário.\n");
        fclose(arquivo);
        exit(EXIT_FAILURE);
    }

    struct Conta conta;

    while (fread(&conta, sizeof(struct Conta), 1, arquivo) == 1) {
        if (conta.status == 1) {
            fwrite(&conta, sizeof(struct Conta), 1, temp);
        }
    }

    fclose(arquivo);
    fclose(temp);

    remove("cadastros.bin");
    rename("temp.txt", "cadastros.bin");
}

void salvarContaArquivo(const struct Conta *conta) {
    FILE *arquivo = fopen("cadastros.bin", "ab");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        exit(EXIT_FAILURE);
    }

    fwrite(conta, sizeof(struct Conta), 1, arquivo);

    fclose(arquivo);
}

void carregarContasArquivo() {
    FILE *arquivo = fopen("cadastros.bin", "rb");
    if (arquivo == NULL) {
        printf("Arquivo não encontrado. Criando novo arquivo.\n");
        return;
    }

    fclose(arquivo);
}

int buscaConta(const char *numeroContaBusca) {
    FILE *arquivo = fopen("cadastros.bin", "rb");
    if (arquivo == NULL) {
        if (errno == ENOENT) {
            return -2;
        } else {
            printf("Erro ao abrir o arquivo para leitura durante a busca.\n");
            return -3; 
        }
    }

    struct Conta conta;
    int posicao = 0;

    while (fread(&conta, sizeof(struct Conta), 1, arquivo) == 1) {
        if (strcmp(conta.numeroConta, numeroContaBusca) == 0) {
            fclose(arquivo);
            return posicao;
        }
        posicao++;
    }

    fclose(arquivo);

    return -1; 
}
