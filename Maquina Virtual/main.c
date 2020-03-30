#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define DS 2
#define ES 3
#define IP 4
#define AC 8
#define CC 9
#define AX 10
#define BX 11
#define CX 12
#define DX 13
#define EX 14
#define FX 15

typedef struct
{
    int RAM[2000];
    int REG[16];
}TMemoria;

typedef void (*T_FUNC)(TMemoria *memoria, int *arg1, int arg2);

void func_MOV(TMemoria *memoria, int *arg1, int arg2);
void func_ADD(TMemoria *memoria, int *arg1, int arg2);
void func_SUB(TMemoria *memoria, int *arg1, int arg2);
void func_MUL(TMemoria *memoria, int *arg1, int arg2);
void func_DIV(TMemoria *memoria, int *arg1, int arg2);
void func_MOD(TMemoria *memoria, int *arg1, int arg2);
void func_CMP(TMemoria *memoria, int *arg1, int arg2);
void func_SWAP(TMemoria *memoria, int *arg1, int arg2);
void func_RND(TMemoria *memoria, int *arg1, int arg2);
void func_AND(TMemoria *memoria, int *arg1, int arg2);
void func_OR(TMemoria *memoria, int *arg1, int arg2);
void func_NOT(TMemoria *memoria, int *arg1, int arg2);
void func_XOR(TMemoria *memoria, int *arg1, int arg2);
void func_SHL(TMemoria *memoria, int *arg1, int arg2);
void func_SHR(TMemoria *memoria, int *arg1, int arg2);
void func_JMP(TMemoria *memoria, int *arg1, int arg2);
void func_JE(TMemoria *memoria, int *arg1, int arg2);
void func_JG(TMemoria *memoria, int *arg1, int arg2);
void func_JL(TMemoria *memoria, int *arg1, int arg2);
void func_JZ(TMemoria *memoria, int *arg1, int arg2);
void func_JP(TMemoria *memoria, int *arg1, int arg2);
void func_JN(TMemoria *memoria, int *arg1, int arg2);
void func_JNZ(TMemoria *memoria, int *arg1, int arg2);
void func_JNP(TMemoria *memoria, int *arg1, int arg2);
void func_JNN(TMemoria *memoria, int *arg1, int arg2);
void func_SYS(TMemoria *memoria, int *arg1, int arg2);
void func_STOP(TMemoria *memoria, int *arg1, int arg2);

int CargarImagen(TMemoria *memoria, char *url);
void CargarMnemonicos(T_FUNC *mnemonicos);
void EjecutarMemoria(TMemoria *memoria);

void MostrarCodigoAssembler(TMemoria memoria);

int main(int arge, char *arg[])
{
    char url[256];
    int flag = 0;
    TMemoria memoria;

    if (arge > 1)
    {
        for (int i = 1; i < arge; i++)
        {
            if (i == 1)
                strcpy(url, arg[i]);
            else
                if (strcmp(arg[i], "-d") == 0)
                    flag = 1;
        }

        if (CargarImagen(&memoria, url))
        {
            if (flag)
            {
                MostrarCodigoAssembler(memoria);
            }

            EjecutarMemoria(&memoria);
        }
    }

    printf("_");
    getch();
    return 0;
}

int getMnemonico(int celda)
{
    return celda >> 16;
}

void ObtenerInstruccion(TMemoria memoria, int i, int *codInstruccion, int *codOperando1, int *codOperando2)
{
    (*codInstruccion) = memoria.RAM[i];
    (*codOperando1) = memoria.RAM[i+1];
    (*codOperando2) = memoria.RAM[i+2];
}
//MOSTRAR CODIGO ASSEMBLER
void MostrarArgumento(int tOper, int oper)
{
    if (tOper == 0x00)
        printf(" %d", oper);
    else
        if (tOper == 0x01)
            printf(" %xX", oper&0x0000000f);
        else
            printf(" [%s:%x]", ((oper&0xf0000000) == 0x20000000)? "DS":"ES",oper&0x0fffffff);
}
void MostrarCodigoAssembler(TMemoria memoria)
{
    char *StringMnemonicos[256];
    int i, oper1, oper2;

    CargarStringMnemonicos(StringMnemonicos);

    for (i = 0; i < memoria.REG[DS]; i+=3)
    {
        ObtenerInstruccion(memoria, i, &memoria.RAM[i], &oper1, &oper2);

        printf("[%08x]: %s", i, StringMnemonicos[getMnemonico(memoria.RAM[i])]);
        MostrarArgumento((memoria.RAM[i]&0x0000ff00) >> 8, oper1);
        printf(",");
        MostrarArgumento((memoria.RAM[i]&0x000000ff), oper2);
        printf("\n");
    }
    printf("\n");
}
//
int CargarImagen(TMemoria *memoria, char *url)
{
    FILE *imgFile = fopen(url, "rb");
    if (imgFile)
    {
        fread(memoria->REG, sizeof(int), 16, imgFile);
        fread(memoria->RAM, sizeof(int), 2000, imgFile);

        fclose(imgFile);
        return 1;
    }
    return 0;
}
void tipoOperandos(TMemoria memoria, int celda, int *tipo, int *operando)
{
    switch (celda)
    {
        case 0x00:
            (*tipo) = 0;
        break;
        case 0x01:
            (*tipo) = 1;
            (*operando) &= 0x0000000f;
        break;
        case 0x02:
            (*tipo) = 2;
            if (((*operando)&0xf0000000) == 0x20000000)
            {
                (*operando) = memoria.REG[DS] + ((*operando)&0x0fffffff);
            }
            else
            {
                (*operando) = memoria.REG[ES] + ((*operando)&0x0fffffff);
            }
        break;
    }
}
void ObtenerOperandos(TMemoria memoria, int Instancia, int *tOper1, int *Oper1, int *tOper2, int *Oper2)
{
    int inst1 = (Instancia&0x0000ff00) >> 8, inst2 = Instancia&0x000000ff;

    tipoOperandos(memoria, inst1, tOper1, Oper1);
    tipoOperandos(memoria, inst2, tOper2, Oper2);
}
void InterpretarInstruccion(int *tInst, int codInst)
{
    (*tInst) = getMnemonico(codInst);
}
void EjecutarInstruccion(TMemoria *memoria, T_FUNC* mnemonicos, int tMnemonico, int tOper1, int Oper1, int tOper2, int Oper2)
{
    int arg1, arg2, auxIP = memoria->REG[IP];
    if (tOper1 == 0)
        arg1 = Oper1;
    else
        if (tOper1 == 1)
            arg1 = memoria->REG[Oper1];
        else
            arg1 = memoria->RAM[Oper1];

    if (tOper2 == 0)
        arg2 = Oper2;
    else
        if (tOper2 == 1)
            arg2 = memoria->REG[Oper2];
        else
            arg2 = memoria->RAM[Oper2];

    mnemonicos[tMnemonico](memoria, &arg1, arg2);

    if (auxIP == memoria->REG[IP])
        memoria->REG[IP]+=3;

    if (tOper1 == 1)
        memoria->REG[Oper1] = arg1;
    else
        memoria->RAM[Oper1] = arg1;
}
void EjecutarMemoria(TMemoria *memoria)
{
    int tipoMnemonico, codInstruccion, tipoOperando1, tipoOperando2, codOperando1, codOperando2;
    T_FUNC mnemonicos[256];

    CargarMnemonicos (mnemonicos);

    while (memoria->REG[IP]>=0 && memoria->REG[IP] < memoria->REG[DS])
    {
        ObtenerInstruccion(*memoria, memoria->REG[IP], &codInstruccion, &codOperando1, &codOperando2);
        ObtenerOperandos(*memoria, codInstruccion, &tipoOperando1, &codOperando1, &tipoOperando2, &codOperando2);
        InterpretarInstruccion(&tipoMnemonico, codInstruccion);
        EjecutarInstruccion(memoria, mnemonicos, tipoMnemonico, tipoOperando1, codOperando1, tipoOperando2, codOperando2);
    }
}

//----------------------------------------------------------------

void CargarStringMnemonicos(char *StringMmemonicos[])
{
    StringMmemonicos[1] = "MOV";
    StringMmemonicos[2] = "ADD";
    StringMmemonicos[3] = "SUB";
    StringMmemonicos[4] = "MUL";
    StringMmemonicos[5] = "DIV";
    StringMmemonicos[6] = "MOD";
    StringMmemonicos[19] = "CMP";
    StringMmemonicos[23] = "SWAP";
    StringMmemonicos[25] = "RND";
    StringMmemonicos[49] = "AND";
    StringMmemonicos[50] = "OR";
    StringMmemonicos[51] = "NOT";
    StringMmemonicos[52] = "XOR";
    StringMmemonicos[55] = "SHL";
    StringMmemonicos[56] = "SHR";
    StringMmemonicos[32] = "JMP";
    StringMmemonicos[33] = "JE";
    StringMmemonicos[34] = "JG";
    StringMmemonicos[35] = "JL";
    StringMmemonicos[36] = "JZ";
    StringMmemonicos[37] = "JP";
    StringMmemonicos[38] = "JN";
    StringMmemonicos[39] = "JNZ";
    StringMmemonicos[40] = "JNP";
    StringMmemonicos[41] = "JNN";
    StringMmemonicos[129] = "SYS";
    StringMmemonicos[143] = "STOP";
}

void CargarMnemonicos(T_FUNC *mnemonicos)
{
    mnemonicos[1] = &func_MOV;
    mnemonicos[2] = &func_ADD;
    mnemonicos[3] = &func_SUB;
    mnemonicos[4] = &func_MUL;
    mnemonicos[5] = &func_DIV;
    mnemonicos[6] = &func_MOD;
    mnemonicos[19] = &func_CMP;
    mnemonicos[23] = &func_SWAP;
    mnemonicos[25] = &func_RND;
    mnemonicos[49] = &func_AND;
    mnemonicos[50] = &func_OR;
    mnemonicos[51] = &func_NOT;
    mnemonicos[52] = &func_XOR;
    mnemonicos[55] = &func_SHL;
    mnemonicos[56] = &func_SHR;
    mnemonicos[32] = &func_JMP;
    mnemonicos[33] = &func_JE;
    mnemonicos[34] = &func_JG;
    mnemonicos[35] = &func_JL;
    mnemonicos[36] = &func_JZ;
    mnemonicos[37] = &func_JP;
    mnemonicos[38] = &func_JN;
    mnemonicos[39] = &func_JNZ;
    mnemonicos[40] = &func_JNP;
    mnemonicos[41] = &func_JNN;
    mnemonicos[129] = &func_SYS;
    mnemonicos[143] = &func_STOP;
}

void func_MOV(TMemoria *memoria, int *arg1, int arg2)
{
    (*arg1) = arg2;
}
void func_ADD(TMemoria *memoria, int *arg1, int arg2)
{
    (*arg1)+=arg2;
}
void func_SUB(TMemoria *memoria, int *arg1, int arg2)
{
    printf("SUB");
}
void func_MUL(TMemoria *memoria, int *arg1, int arg2)
{
    printf("MUL");
}
void func_DIV(TMemoria *memoria, int *arg1, int arg2)
{
    printf("DIV");
}
void func_MOD(TMemoria *memoria, int *arg1, int arg2)
{
    printf("MOD");
}
void func_CMP(TMemoria *memoria, int *arg1, int arg2)
{
    printf("CMP");
}
void func_SWAP(TMemoria *memoria, int *arg1, int arg2)
{
    printf("SWAP");
}
void func_RND(TMemoria *memoria, int *arg1, int arg2)
{
    printf("RND");
}
void func_AND(TMemoria *memoria, int *arg1, int arg2)
{
    printf("AND");
}
void func_OR(TMemoria *memoria, int *arg1, int arg2)
{
    printf("OR");
}
void func_NOT(TMemoria *memoria, int *arg1, int arg2)
{
    printf("NOT");
}
void func_XOR(TMemoria *memoria, int *arg1, int arg2)
{
    printf("XOR");
}
void func_SHL(TMemoria *memoria, int *arg1, int arg2)
{
    printf("SHL");
}
void func_SHR(TMemoria *memoria, int *arg1, int arg2)
{
    printf("SHR");
}
void func_JMP(TMemoria *memoria, int *arg1, int arg2)
{
    printf("JMP");
}
void func_JE(TMemoria *memoria, int *arg1, int arg2)
{
    printf("JE");
}
void func_JG(TMemoria *memoria, int *arg1, int arg2)
{
    printf("JG");
}
void func_JL(TMemoria *memoria, int *arg1, int arg2)
{
    printf("JL");
}
void func_JZ(TMemoria *memoria, int *arg1, int arg2)
{
    printf("JZ");
}
void func_JP(TMemoria *memoria, int *arg1, int arg2)
{
    printf("JP");
}
void func_JN(TMemoria *memoria, int *arg1, int arg2)
{
    printf("JN");
}
void func_JNZ(TMemoria *memoria, int *arg1, int arg2)
{
    printf("JNZ");
}
void func_JNP(TMemoria *memoria, int *arg1, int arg2)
{
    printf("JNP");
}
void func_JNN(TMemoria *memoria, int *arg1, int arg2)
{
    printf("JNN");
}
char *getNombreDelRegistro(int i)
{
    switch (i)
    {
        case 2: return "DS";
        case 3: return "ES";
        case 4: return "IP";
        case 8: return "AC";
        case 9: return "CC";
        case 10: return "AX";
        case 11: return "BX";
        case 12: return "CX";
        case 13: return "DX";
        case 14: return "EX";
        case 15: return "FX";
    }
    return '\0';
}
void func_SYS(TMemoria *memoria, int *arg1, int arg2)
{
    int configuracion = memoria->REG[AX], prompt = 0, endline = 0, caracter = 0, direccion;

    switch ((int)(*arg1))
    {
        case 1://READ
            {
                direccion = memoria->REG[DX];
                if (direccion < memoria->REG[DS])
                    direccion += memoria->REG[DS];

                if ((configuracion & 0xf000) == 0x0000)
                    prompt = 1;

                if ((configuracion & 0x0f00) == 0x0100)
                { // LEE STRING
                    char cadena[50];
                    if (prompt)
                    {
                        printf("[%08d]: ", (memoria->REG[DX]));
                    }
                    fflush(stdin);
                    gets(cadena);
                    for (int i = 0; i < strlen(cadena); i++)
                    {
                        memoria->RAM[direccion + i] = (int)cadena[i];
                    }
                }
                else
                {
                    for (int i = 0; i < memoria->REG[CX]; i++)
                        {
                        if (prompt)
                        {
                            printf("[%08d]: ", (memoria->REG[DX] + i));
                        }
                        if ((configuracion & 0x0008) == 0x0008)
                            scanf("%x", &memoria->RAM[direccion + i]);
                        if ((configuracion & 0x0004) == 0x0004)
                            scanf("%o", &memoria->RAM[direccion + i]);
                        if ((configuracion & 0x0001) == 0x0001)
                            scanf("%d", &memoria->RAM[direccion + i]);

                        if (endline)
                            printf("\n");
                    }
                }

            }
        break;
        case 2://WRITE
            {
                direccion = memoria->REG[DX];
                if (direccion < memoria->REG[DS])
                    direccion += memoria->REG[DS];

                if ((configuracion & 0xf000) == 0x0000)
                    prompt = 1;

                if ((configuracion & 0x0f00) == 0x0000)
                    endline = 1;

                if ((configuracion & 0x00f0) == 0x0010)
                    caracter = 1;

                for (int i = 0; i < memoria->REG[CX]; i++)
                {
                    if (prompt && (i == 0 || endline))
                        printf("[%08d]: ", (memoria->REG[DX] + i));

                    if (caracter)
                        printf("%c", memoria->RAM[direccion + i]);

                    if ((configuracion & 0x0008) == 0x0008)
                        printf("%%%x", memoria->RAM[direccion + i]);
                    if ((configuracion & 0x0004) == 0x0004)
                        printf("@%o", memoria->RAM[direccion + i]);
                    if ((configuracion & 0x0001) == 0x0001)
                        printf("%d", memoria->RAM[direccion + i]);

                    if (endline)
                        printf("\n");
                }
            }
        break;
        case 3://DUMP
            {
                if ((configuracion & 0xf000) == 0x0000)
                    prompt = 1;

                if ((configuracion & 0x0f00) == 0x0000)
                    endline = 1;

                if ((configuracion & 0x00f0) == 0x0010)
                    caracter = 1;

                for (int i = 0; i < 16; i++)
                {
                    if ((i > 1 && i < 5)||(i > 7))
                    {
                        if (prompt && (i == 0 || endline))
                            printf("[%s]: ", getNombreDelRegistro(i));

                        if (caracter)
                            printf("%c", memoria->REG[i]);

                        if ((configuracion & 0x0008) == 0x0008)
                            printf("%%%04x", memoria->REG[i]);
                        if ((configuracion & 0x0004) == 0x0004)
                            printf("@%04o", memoria->REG[i]);
                        if ((configuracion & 0x0001) == 0x0001)
                            printf("%04d", memoria->REG[i]);
                        if (endline)
                            printf("\n");
                    }
                }
            }
        break;
    }
}
void func_STOP(TMemoria *memoria, int *arg1, int arg2)
{
    printf("STOP");
}
