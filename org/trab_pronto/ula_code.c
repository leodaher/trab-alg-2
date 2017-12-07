//
// Universidade de Sao Paulo
//
// SSC0610 - Organizacao de Computadores Digitais I
// Professor Paulo Sergio Lopes de Souza
//
// Trabalho 2
// Processador em C
//
// Alunos:
// Giuliano Barbosa Prado - 7961109
// Igor Ramom Machado Alves - 7986890
// Ilan Sales Galvao de Figueiredo - 7656321
// Laise Aquino Cardoso e Silva - 7986924
//
// Engenharia de Computacao 2012
//
// Data para entrega:  08/11/2013
//
// Arquivos enviados:
// cpu_multi_code.c
// ula_code.c
//
// Arquivo atual:
// ula_code.c
//

// Somador completo para 1 bit
void somador_completo (char a, char b, char cin, char* s, char* cout)
{
    // Saida do somador completo: a XOR b XOR cin
    (*s)=(a^b^cin);
    // Carry out do somador completo: a&b ou a&cin ou b&cin
    (*cout)=(a&b || a&cin || b&cin);
}

// Faz a soma de 32 bits
void soma (int a, int b, int* s, char* overflow)
{
    // Iterador para calcular a soma de cada bit
    int i;
    // Saida auxiliar (para resultado de cada soma de bits)
    char s_aux;
    // C atual e C anterior (para determinar overflow)
    char last_c_aux=0;
    char c_aux=0;
    // Inicializa o resultado como zero
    (*s)=0;
    // Para cada bit...
    for (i=0;i<32;i++)
    {
        // Salva o c anterior
        last_c_aux=c_aux;
        // Faz a soma dos bits, e recebe c como entrada
        somador_completo((a&(1<<i))>>i, (b&(1<<i))>>i, c_aux, &s_aux, &c_aux);
        // Atualiza o valor de s, na posicao do bit somado
        (*s)=((*s)|(s_aux<<i));
    }
    // Ocorre overflow quando o "vai um e vem um" tem valor oposto
    (*overflow)=(last_c_aux^c_aux);
}

// Faz a subtracao de 32 bits
// Obs: para fazer uma subtracao de a-b, basta somar a com o complemento 2 de b
// Logo, basta inicializar c (cin) como 1 e inverter os bits de b, e realizar a soma
void subtracao (int a, int b, int* s, char* overflow)
{
    // Iterador para calcular a subtracao de cada bit
    int i;
    // Saida auxiliar (para resultado de cada soma de bits)
    char s_aux;
    // C atual e C anterior (para determinar overflow)
    // Obs: inicializa c_aux como 1 para fazer subtracao, conforme explicado acima
    char last_c_aux=1;
    char c_aux=1;
    // Inicializa o resultado como zero
    (*s)=0;
    // Para cada bit...
    for (i=0;i<32;i++)
    {
        // Salva o c anterior
        last_c_aux=c_aux;
        // Faz a soma de a e !b, e recebe c como entrada (inicializado em 1)
        somador_completo((a&(1<<i))>>i, !((b&(1<<i))>>i), c_aux, &s_aux, &c_aux);
        // Atualiza o valor de s, na posicao do bit subtraido
        (*s)=((*s)|(s_aux<<i));
        
    }
    // Ocorre overflow quando o "vai um e vem um" tem valor oposto
    (*overflow)=(last_c_aux^c_aux);
}

// Faz um AND de 32 bits
void and (int a, int b, int* s, char* overflow)
{
    // Iterador para calcular o AND de cada bit
    int i;
    // Inicializa o resultado como zero
    (*s)=0;
    // Para cada bit...
    for (i=0;i<32;i++)
    {
        // Checa se o bit na posicao i de a E b sao 1
        if (a&(1<<i) && b&(1<<i))
        {
            // Caso seja, atualiza o valor de s na posicao i
            (*s)=((*s)|1<<i);
        }
    }
    // Nao ha overflow em AND
    (*overflow)=0;
}

// Faz um OR de 32 bits
void or (int a, int b, int* s, char* overflow)
{
    // Iterador para calcular o OR de cada bit
    int i;
    // Inicializa o resultado como zero
    (*s)=0;
    // Para cada bit...
    for (i=0;i<32;i++)
    {
        // Checa se o bit na posicao i de a OU b sao 1
        if (a&(1<<i) || b&(1<<i))
        {
            // Caso seja, atualiza o valor de s na posicao i
            (*s)=((*s)|1<<i);
        }
    }
    // Nao ha overflow em AND
    (*overflow)=0;
}

// Faz um SLT de 32 bits
void slt (int a, int b, int * s, char* overflow)
{
    // Subtrai a-b
    subtracao(a, b, s, overflow);
    // Caso a-b<0 (ou seja, a<b), entao s=1
    // Obs: para detectar se e negativo, verifico se o bit mais significativo e 1
    (*s)=((*s)&(1<<31))?1:0;
}

int ula( int a, int b, char ula_op, int * result_ula, char * zero, char * overflow)
{
    // Soma
    if (ula_op==0x2) //0b0010
        soma(a, b, result_ula, overflow);
    // Subtracao
    else if (ula_op==0x6) //0b0110
        subtracao(a, b, result_ula, overflow);
    // And
    else if (ula_op==0x0) //0b0000
        and(a, b, result_ula, overflow);
    // Or
    else if (ula_op==0x1) //0b0001
        or(a, b, result_ula, overflow);
    // Set on less than
    else if (ula_op==0x7) //0b0111
        slt(a, b, result_ula, overflow);
    // Determina se e zero
    (*zero) = ((*result_ula)==0);
    return 0;
}