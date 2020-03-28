#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

const int DS = 2;
const int ES = 3;
const int IP = 4;

int RAM[2000];
int REG[16];

typedef void (*T_FUNC)(double *arg1, double arg2);

void func_MOV(double *arg1, double arg2);
void func_ADD(double *arg1, double arg2);
void func_SUB(double *arg1, double arg2);
void func_MUL(double *arg1, double arg2);
void func_DIV(double *arg1, double arg2);
void func_MOD(double *arg1, double arg2);
void func_CMP(double *arg1, double arg2);
void func_SWAP(double *arg1, double arg2);
void func_RND(double *arg1, double arg2);
void func_AND(double *arg1, double arg2);
void func_OR(double *arg1, double arg2);
void func_NOT(double *arg1, double arg2);
void func_XOR(double *arg1, double arg2);
void func_SHL(double *arg1, double arg2);
void func_SHR(double *arg1, double arg2);
void func_JMP(double *arg1, double arg2);
void func_JE(double *arg1, double arg2);
void func_JG(double *arg1, double arg2);
void func_JL(double *arg1, double arg2);
void func_JZ(double *arg1, double arg2);
void func_JP(double *arg1, double arg2);
void func_JN(double *arg1, double arg2);
void func_JNZ(double *arg1, double arg2);
void func_JNP(double *arg1, double arg2);
void func_JNN(double *arg1, double arg2);
void func_SYS(double *arg1, double arg2);
void func_STOP(double *arg1, double arg2);

void CargarImagen(char *url, int flag);
void CargarMnemonicos(T_FUNC *mnemonicos);

int main(int arge, char *arg[])
{
    char url[256];
    int flag = 0;

    if (arge > 1)
    {
        for (int i = 0; i < arge; i++)
        {
            if (i == 1)
                strcpy(url, arg[i]);
            else
                if (strcmp(arg[i], "-d") == 0)
                    flag = 1;
        }
        CargarImagen(url, flag);
    }

    printf("_");
    getch();
    return 0;
}

void CargarRegistroYRAM(FILE *file)
{
    fread(REG, sizeof(int), 16, file);
    fread(RAM, sizeof(int), 2000, file);
}
int buscarMnemonico(int celda)
{
    return celda >> 16;
}
void tipoOperandos(int celda, int *tipo, int *operando)
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
                (*operando) = REG[DS] + ((*operando)&0x0fffffff);
            }
            else
            {
                (*operando) = REG[ES] + ((*operando)&0x0fffffff);
            }
        break;
    }
}

void ObtenerInstruccion(int i, int *codInstruccion, int *codOperando1, int *codOperando2)
{
    (*codInstruccion) = RAM[i];
    (*codOperando1) = RAM[i+1];
    (*codOperando2) = RAM[i+2];

    //printf("\n%08x %08x %08x", *codInstruccion, *codOperando1, *codOperando2);
}
void ObtenerOperandos(int Inst, int *tOper1, int *Oper1, int *tOper2, int *Oper2)
{
    tipoOperandos((Inst&0x0000ff00) >> 8, tOper1, Oper1);
    tipoOperandos(Inst&0x000000ff, tOper2, Oper2);
}
void InterpretarInstruccion(int *tInst, int codInst)
{
    (*tInst) = buscarMnemonico(codInst);
}
void EjecutarInstruccion(T_FUNC* mnemonicos, int tMnemonico, int tOper1, int Oper1, int tOper2, int Oper2)
{
    double arg1, arg2;
    int auxIP = REG[IP];
    if (tOper1 == 0)
        arg1 = Oper1;
    else
        if (tOper1 == 1)
            arg1 = REG[Oper1];
        else
            arg1 = RAM[Oper1];

    if (tOper2 == 0)
        arg2 = Oper2;
    else
        if (tOper2 == 1)
            arg2 = REG[Oper2];
        else
            arg2 = RAM[Oper2];

    mnemonicos[tMnemonico](&arg1, arg2);

    if (auxIP == REG[IP])
        REG[IP]+=3;

    if (tOper1 == 1)
        REG[Oper1] = arg1;
    else
        RAM[Oper1] = arg1;
}
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
void MostrarRAM(char *StringMemonicos[])
{
    int i, oper1, oper2;
    for (i = 0; i < REG[DS]; i+=3)
    {
        ObtenerInstruccion(i, &RAM[i], &oper1, &oper2);

        printf("[%08x]: %s", i, StringMemonicos[buscarMnemonico(RAM[i])]);
        MostrarArgumento((RAM[i]&0x0000ff00) >> 8, oper1);
        printf(",");
        MostrarArgumento((RAM[i]&0x000000ff), oper2);
        printf("\n");
    }
    printf("\n");
}
void CargarImagen(char *url, int flag)
{
    //flag =1;
    //FILE *imgFile = fopen("imagenes\\imagen3.txt", "rb");
    FILE *imgFile = fopen(url, "rb");
    if (imgFile)
    {
        int tipoMnemonico, codInstruccion, tipoOperando1, tipoOperando2, codOperando1, codOperando2;
        T_FUNC mnemonicos[256];
        char *StringMmemonicos[256];

        CargarMnemonicos (mnemonicos);
        CargarRegistroYRAM(imgFile);
        fclose(imgFile);

        if (flag)
        {
            CargarStringMnemonicos(StringMmemonicos);
            MostrarRAM(StringMmemonicos);
        }

        while (REG[IP]>=0 && REG[IP] < REG[DS])
        {
            ObtenerInstruccion(REG[IP], &codInstruccion, &codOperando1, &codOperando2);
            ObtenerOperandos(codInstruccion, &tipoOperando1, &codOperando1, &tipoOperando2, &codOperando2);
            InterpretarInstruccion(&tipoMnemonico, codInstruccion);
            EjecutarInstruccion(mnemonicos, tipoMnemonico, tipoOperando1, codOperando1, tipoOperando2, codOperando2);
        }

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

void func_MOV(double *arg1, double arg2)
{
    (*arg1) = arg2;
}
void func_ADD(double *arg1, double arg2)
{
    (*arg1)+=arg2;
}
void func_SUB(double *arg1, double arg2)
{
    printf("SUB");
}
void func_MUL(double *arg1, double arg2)
{
    printf("MUL");
}
void func_DIV(double *arg1, double arg2)
{
    printf("DIV");
}
void func_MOD(double *arg1, double arg2)
{
    printf("MOD");
}
void func_CMP(double *arg1, double arg2)
{
    printf("CMP");
}
void func_SWAP(double *arg1, double arg2)
{
    printf("SWAP");
}
void func_RND(double *arg1, double arg2)
{
    printf("RND");
}
void func_AND(double *arg1, double arg2)
{
    printf("AND");
}
void func_OR(double *arg1, double arg2)
{
    printf("OR");
}
void func_NOT(double *arg1, double arg2)
{
    printf("NOT");
}
void func_XOR(double *arg1, double arg2)
{
    printf("XOR");
}
void func_SHL(double *arg1, double arg2)
{
    printf("SHL");
}
void func_SHR(double *arg1, double arg2)
{
    printf("SHR");
}
void func_JMP(double *arg1, double arg2)
{
    printf("JMP");
}
void func_JE(double *arg1, double arg2)
{
    printf("JE");
}
void func_JG(double *arg1, double arg2)
{
    printf("JG");
}
void func_JL(double *arg1, double arg2)
{
    printf("JL");
}
void func_JZ(double *arg1, double arg2)
{
    printf("JZ");
}
void func_JP(double *arg1, double arg2)
{
    printf("JP");
}
void func_JN(double *arg1, double arg2)
{
    printf("JN");
}
void func_JNZ(double *arg1, double arg2)
{
    printf("JNZ");
}
void func_JNP(double *arg1, double arg2)
{
    printf("JNP");
}
void func_JNN(double *arg1, double arg2)
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
void func_SYS(double *arg1, double arg2)
{
    int conf = REG[0xa];
    int prompt = 0, endline = 0, caracter = 0;
    //printf("\n%d", RAM[RAM[DS]+1]);
    switch ((int)(*arg1))
    {
        case 1://READ
            {
                int DX = REG[0xd];
                if (DX < REG[DS])
                    DX+=REG[DS];

                if ((conf & 0xf000) == 0x0000)
                    prompt = 1;

                if ((conf & 0x0f00) == 0x0100)
                { // LEE STRING
                    char cadena[50];
                    if (prompt)
                    {
                        printf("[%08d]: ", (REG[0xd]));
                    }
                    fflush(stdin);
                    gets(cadena);
                    for (int i = 0; i < strlen(cadena); i++)
                    {
                        RAM[DX + i] = (int)cadena[i];
                    }
                }
                else
                {
                    for (int i = 0; i < REG[0xc]; i++)
                        {
                        if (prompt)
                        {
                            printf("[%08d]: ", (REG[0xd] + i));
                        }
                        if ((conf & 0x0008) == 0x0008)
                            scanf("%x", &RAM[DX + i]);
                        if ((conf & 0x0004) == 0x0004)
                            scanf("%o", &RAM[DX + i]);
                        if ((conf & 0x0001) == 0x0001)
                            scanf("%d", &RAM[DX + i]);

                        if (endline)
                            printf("\n");
                    }
                }

            }
        break;
        case 2://WRITE
            {
                int DX = REG[0xd];
                if (DX < REG[DS])
                    DX+=REG[DS];

                if ((conf & 0xf000) == 0x0000)
                    prompt = 1;

                if ((conf & 0x0f00) == 0x0000)
                    endline = 1;

                if ((conf & 0x00f0) == 0x0010)
                    caracter = 1;

                for (int i = 0; i < REG[0xc]; i++)
                {
                    if (prompt && (i == 0 || endline))
                        printf("[%08d]: ", (REG[0xd] + i));

                    if (caracter)
                        printf("%c", RAM[DX + i]);

                    if ((conf & 0x0008) == 0x0008)
                        printf("%%%x", RAM[DX + i]);
                    if ((conf & 0x0004) == 0x0004)
                        printf("@%o", RAM[DX + i]);
                    if ((conf & 0x0001) == 0x0001)
                        printf("%d", RAM[DX + i]);

                    if (endline)
                        printf("\n");
                }
            }
        break;
        case 3://DUMP
            {
                if ((conf & 0xf000) == 0x0000)
                    prompt = 1;

                if ((conf & 0x0f00) == 0x0000)
                    endline = 1;

                if ((conf & 0x00f0) == 0x0010)
                    caracter = 1;

                for (int i = 0; i < 16; i++)
                {
                    if ((i > 1 && i < 5)||(i > 7))
                    {
                        if (prompt && (i == 0 || endline))
                            printf("[%s]: ", getNombreDelRegistro(i));

                        if (caracter)
                            printf("%c", REG[i]);

                        if ((conf & 0x0008) == 0x0008)
                            printf("%%%04x", REG[i]);
                        if ((conf & 0x0004) == 0x0004)
                            printf("@%04o", REG[i]);
                        if ((conf & 0x0001) == 0x0001)
                            printf("%04d", REG[i]);
                        if (endline)
                            printf("\n");
                    }
                }
            }
        break;
    }
}
void func_STOP(double *arg1, double arg2)
{
    printf("STOP");
}
