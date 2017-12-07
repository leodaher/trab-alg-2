/* code-c - Student's code for mmcpu

   Copyright 2017  Monaco F. J.   <monaco@icmc.usp.br>

   This file is part of Muticlycle Mips CPU (MMCPU)

   MMCPU is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/* After you have implemented your functions, you may safely remove these lines. */
#include <stdio.h>
#include <stdlib.h>
#define not_implemented() fprintf(stderr, "Not implemented\n"); exit(EXIT_FAILURE)

/* Students, you are required to implemented the functions bellow.
   Please, refere to cpu.h for further information. */



int alu( int a, int b, char alu_op, int *result_alu, char *zero, char *overflow)
{
  return 0;
}


void control_unit(int IR, short int *sc)
{
    static char s=0;
    int ROM[10];
    // ROM com os dados gravados, no formado:
    // 2 bits para sequenciamento, 16 bits para sinal de controle
    ROM[0]=0x39408; //0b111001010000001000
    ROM[1]=0x10018; //0b010000000000011000
    ROM[2]=0x20014; //0b100000000000010100
    ROM[3]=0x31800; //0b110001100000000000
    ROM[4]=0x04002; //0b000100000000000010
    ROM[5]=0x02800; //0b000010100000000000
    ROM[6]=0x30044; //0b110000000001000100
    ROM[7]=0x00003; //0b000000000000000011
    ROM[8]=0x002A4; //0b000000001010100100
    ROM[9]=0x00500; //0b000000010100000000
    
    // Baseado em s, determina, na ROM, o valor do sinal de controle
    (*sc)=ROM[(int)s]&0xFFFF;
    
    // Baseado no AddrCtrl, determina o sequenciamento (sequencia, despacho ou voltar)
    if((ROM[(int)s]&separa_AddrCtrl)==sequencia)
        s=s+1;
    else if((ROM[(int)s]&separa_AddrCtrl)==tab_desp1)
        TabelaDespacho1(IR, &s);
    else if((ROM[(int)s]&separa_AddrCtrl)==tab_desp2)
        TabelaDespacho2(IR, &s);
    else if((ROM[(int)s]&separa_AddrCtrl)==volta_busca)
        s=0;
    
    // Criterio de parada: caso o conteudo lido seja zero, para o programa
    if (IR==0)
        loop=0;
}


void instruction_fetch(short int sc, int PC, int ALUOUT, int IR, int* PCnew, int* IRnew, int* MDRnew)
{
    // Variaveis representando entradas/saidas dos circuitos
    char zero, overflow, ula_op_aux;
    int MemData;
    int Address=0;
    int PC_entrada;
    int ULA_entrada1, ULA_entrada2;
    
    /* === Operacao aritmetica === */
    // Se ALUSrcA = 0, muda a 1a entrada da ULA para PC
    if (!(sc&separa_ALUSrcA))
        ULA_entrada1=PC;
    
    // Se ALUSrcB = 01, muda a 2a entrada da ULA para 4
    if (!(sc&separa_ALUSrcB1) && (sc&separa_ALUSrcB0))
        ULA_entrada2=4;
    
    // (Caso esteja no caso contemplado por essa funcao)
    if (!(sc&separa_ALUSrcA) && (!(sc&separa_ALUSrcB1) && (sc&separa_ALUSrcB0)))
    {
        // Determina o codigo e faz a operacao aritmetica
        ULA_Control(sc, IR, &ula_op_aux);
        ula(ULA_entrada1, ULA_entrada2, ula_op_aux, &ALUOUT, &zero, &overflow);
    }
    
    /* === Mudanca do PC === */
    // Se o PC Source for 00, muda a entrada do PC para a saida imediata da ALU
    if (!(sc&separa_PCSource1) && !(sc&separa_PCSource0))
        PC_entrada = ALUOUT;
    
    // Caso PCWrite esteja ativo, ou o valor da ULA seja zero e esteja ativo PCWriteCond
    if (((zero&&(sc&separa_PCWriteCond)) || (sc&separa_PCWrite)))
        // (Caso esteja no caso contemplado por essa funcao)
        if ((!(sc&separa_PCSource1) && !(sc&separa_PCSource0)))
            // Atualiza o valor de PC
            (*PCnew) = PC_entrada;
    
    /* === Registradores e memoria === */
    // Se IorD = 0, muda o endereco de entrada para leitura na memoria para PC
    if (!(sc&separa_IorD))
        Address = PC;
    
    // Se MemRead = 1, le o endereco de memoria
    if (sc&separa_MemRead)
        MemData = memoria[Address/4];
    
    // Muda, em todos os ciclos, o valor de MDR
    // (Caso esteja no caso contemplado por essa funcao)
    if (!(sc&separa_IorD))
        (*MDRnew) = MemData;
    
    // Se IRWrite estiver ativos
    if (sc&separa_IRWrite)
        // (Caso esteja no caso contemplado por essa funcao)
        if (!(sc&separa_IorD) && (sc&separa_MemRead))
            // Muda o valor do IR
            (*IRnew) = MemData;
}


void decode_register(short int sc, int IR, int PC, int A, int B, int *Anew, int *Bnew, int *ALUOUTnew)
{
    // Variaveis representando entradas/saidas dos circuitos
    char zero, overflow, ula_op_aux;
    int regA;
    int regB;
    int ULA_entrada1, ULA_entrada2;
    
    /* === Registradores e memoria === */
    // Em todos os ciclos, atualiza o valor de A e B
    regA = ((IR&separa_rs)>>21);
    regB = ((IR&separa_rt)>>16);
    (*Anew) = reg[regA];
    (*Bnew) = reg[regB];
    
    /* === Operacao aritmetica === */
    // Se ALUSrcA = 0, muda a 1a entrada da ULA para PC
    if (!(sc&separa_ALUSrcA))
        ULA_entrada1=PC;
    
    // Se ALUSrcB = 11, muda a 2a entrada da ULA para extende-sinal(imediato)<<2
    if ((sc&separa_ALUSrcB1) && (sc&separa_ALUSrcB0))
    {
        // Separa o imediato
        ULA_entrada2=(IR&separa_imediato);
        // Extensao de sinal (se for negativo, completa com 1. Senao, extende os zeros.)
        if (ULA_entrada2&0x00008000)
            ULA_entrada2 = ULA_entrada2|0xFFFF0000;
        else
            ULA_entrada2 = ULA_entrada2&0x0000FFFF;
        // Shift 2
        ULA_entrada2 = ULA_entrada2 << 2;
    }
    
    // (Caso esteja no caso contemplado por essa funcao)
    if (!(sc&separa_ALUSrcA) && (sc&separa_ALUSrcB0) && (sc&separa_ALUSrcB1))
    {
        // Determina o codigo e faz a operacao aritmetica
        ULA_Control(sc, IR, &ula_op_aux);
        ula(ULA_entrada1, ULA_entrada2, ula_op_aux, ALUOUTnew, &zero, &overflow);
    }
}



void exec_calc_end_branch(short int sc, int A, int B, int IR, int PC, int ALUOUT, int *ALUOUTnew, int *PCnew)
{
    // Variaveis representando entradas/saidas dos circuitos
    char zero, overflow, ula_op_aux;
    int ULA_entrada1, ULA_entrada2;
    int PC_entrada;
    
    /* === Operacao aritmetica === */
    // Se ALUSrcA = 0, muda a 1a entrada da ULA para PC
    if (!(sc&separa_ALUSrcA))
        ULA_entrada1=PC;
    // Se ALUSrcA = 0, muda a 1a entrada da ULA para A
    if (sc&separa_ALUSrcA)
        ULA_entrada1=A;
    // Se ALUSrcB = 00, muda a 2a entrada da ULA para B
    if (!(sc&separa_ALUSrcB1) && !(sc&separa_ALUSrcB0))
        ULA_entrada2=B;
    // Se ALUSrcB = 10, muda a 2a entrada da ULA para extende-sinal(imediato)
    if ((sc&separa_ALUSrcB1) && !(sc&separa_ALUSrcB0))
    {
        // Separa o imediato
        ULA_entrada2=(IR&separa_imediato);
        // Extensao de sinal (se for negativo, completa com 1. Senao, extende os zeros.)
        if (ULA_entrada2&0x00008000)
            ULA_entrada2 = ULA_entrada2|0xFFFF0000;
        else
            ULA_entrada2 = ULA_entrada2&0x0000FFFF;
    }
    
    // (Caso esteja no caso contemplado por essa funcao)
    if ((!(sc&separa_ALUSrcB1) && !(sc&separa_ALUSrcB0)) || ((sc&separa_ALUSrcB1) && !(sc&separa_ALUSrcB0)))
    {
        // Determina o codigo e faz a operacao aritmetica
        ULA_Control(sc, IR, &ula_op_aux);
        ula(ULA_entrada1, ULA_entrada2, ula_op_aux, ALUOUTnew, &zero, &overflow);
    }
    
    /* === Mudanca do PC === */
    // Se o PC Source for 01, muda a entrada do PC para ALUOUT
    if (!(sc&separa_PCSource1) && (sc&separa_PCSource0))
        PC_entrada = ALUOUT;
    
    // Se o PC Source for 10, muda a entrada do PC para concatenacao(PC[31:28],IR[25:0]<<2)
    if ((sc&separa_PCSource1) && !(sc&separa_PCSource0))
        PC_entrada = ((PC&separa_4bits_PC)|((IR&separa_endereco_jump)<<2));
    
    // Caso PCWrite esteja ativo, ou o valor da ULA seja zero e esteja ativo PCWriteCond
    if (((zero&&(sc&separa_PCWriteCond)) || (sc&separa_PCWrite)))
        // (Caso esteja no caso contemplado por essa funcao)
        if ((!(sc&separa_PCSource1) && (sc&separa_PCSource0)) || ((sc&separa_PCSource1) && !(sc&separa_PCSource0)))
            // Atualiza o valor de PC
            *PCnew = PC_entrada;
}


void write_r_access_memory(short int sc, int B, int IR, int ALUOUT, int PC, int *MDRnew, int *IRnew);
{
      // Variaveis representando entradas/saidas dos circuitos
    int Address=0;
    int MemData=0;
    char WriteRegister;
    int WriteData;
    
    /* === Registradores e memoria === */
    // Se IorD = 1, muda o endereco de entrada para leitura na memoria para o ALUOUT
    if (sc&separa_IorD)
        Address=ALUOUT;
    
    // Se MemRead = 1, le o endereco de memoria
    if (sc&separa_MemRead)
        MemData=memoria[Address/4];
    
    // Muda, em todos os ciclos, o valor de MDR
    // (Caso esteja no caso contemplado por essa funcao)
    if (sc&separa_IorD)
        (*MDRnew) = MemData;
    
    // Se MemWrite estiver ativos
    if (sc&separa_MemWrite)
        // (Caso esteja no caso contemplado por essa funcao)
        if (sc&separa_IorD)
            // Atualiza o valor de MemData (memoria[Address/4])
            memoria[Address/4] = B;
    
    
    // Se RegDest estiver em 1, muda o destino do dado para o valor em rd [15:11]
    if ((sc&separa_RegDst))
        WriteRegister = (IR&separa_rd)>>11;
    
    // Se MemToReg estiver em 0, muda o valor para o que esta em ALUOUT
    if (!(sc&separa_MemtoReg))
        WriteData = ALUOUT;
    
    // Se RegWrite estiver em 1
    if (sc&separa_RegWrite)
        // (Caso esteja no caso contemplado por essa funcao)
        if ((sc&separa_RegDst) && !(sc&separa_MemtoReg))
            // Atualiza o valor do registrador
            reg[(int)WriteRegister] = WriteData;
}


void write_ref_mem(short int sc, int IR, int MDR, int ALUOUT)
{
    char WriteRegister;
    int WriteData;
    
    // Se RegDest estiver em 0, muda o destino do dado para o valor em rt [20:16]
    if (!(sc&separa_RegDst))
        WriteRegister = (IR&separa_rt)>>16;
    
    // Se MemToReg estiver em 1, muda o valor para o que esta em MDR
    if (sc&separa_MemtoReg)
        WriteData = MDR;
    
    // Se RegWrite estiver em 1
    if (sc&separa_RegWrite)
        // (Caso esteja no caso contemplado por essa funcao)
        if (!(sc&separa_RegDst) && (sc&separa_MemtoReg))
            // Atualiza o valor do registrador
            reg[(int)WriteRegister]=WriteData;
}


