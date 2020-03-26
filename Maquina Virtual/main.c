#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void CargarImagen();

typedef void (*T_FUNC)(double *arg1, double *arg2);
void CargarMnemonicos(T_FUNC *mnemonicos);

void func_MOV(double *arg1, double *arg2);
void func_ADD(double *arg1, double *arg2);
void func_SUB(double *arg1, double *arg2);
void func_MUL(double *arg1, double *arg2);
void func_DIV(double *arg1, double *arg2);
void func_MOD(double *arg1, double *arg2);
void func_CMP(double *arg1, double *arg2);
void func_SWAP(double *arg1, double *arg2);
void func_RND(double *arg1, double *arg2);
void func_AND(double *arg1, double *arg2);
void func_OR(double *arg1, double *arg2);
void func_NOT(double *arg1, double *arg2);
void func_XOR(double *arg1, double *arg2);
void func_SHL(double *arg1, double *arg2);
void func_SHR(double *arg1, double *arg2);
void func_JMP(double *arg1, double *arg2);
void func_JE(double *arg1, double *arg2);
void func_JG(double *arg1, double *arg2);
void func_JL(double *arg1, double *arg2);
void func_JZ(double *arg1, double *arg2);
void func_JP(double *arg1, double *arg2);
void func_JN(double *arg1, double *arg2);
void func_JNZ(double *arg1, double *arg2);
void func_JNP(double *arg1, double *arg2);
void func_JNN(double *arg1, double *arg2);
void func_SYS(double *arg1, double *arg2);
void func_STOP(double *arg1, double *arg2);

int main(int arge, char *arg[])
{
    int RAM[2000], reg[16];
    int mnemonicos[256];

    /*FILE *a = fopen("d:\\testSize1.txt","wb");
    int x = 0x00040102;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x0000000a;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x20000020;
    fwrite(&x, sizeof(int), 1, a);
    fclose(a);*/
    /*int x;
    FILE *a = fopen("d:\\testSize2.txt","wb");

    x = 0x00010100;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x0000000E;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x000003E9;
    fwrite(&x, sizeof(int), 1, a);


    x = 0x00010100;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x0000000B;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x00000064;
    fwrite(&x, sizeof(int), 1, a);



    x = 0x00020101;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x0000000E;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x0000000B;
    fwrite(&x, sizeof(int), 1, a);



    x = 0x00010201;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x20000001;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x0000000E;
    fwrite(&x, sizeof(int), 1, a);



    x = 0x00010100;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x0000000A;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x00001101;
    fwrite(&x, sizeof(int), 1, a);


    x = 0x00010100;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x0000000D;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x00000001;
    fwrite(&x, sizeof(int), 1, a);


    x = 0x00010100;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x0000000C;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x00000001;
    fwrite(&x, sizeof(int), 1, a);


    x = 0x00810000;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x00000002;
    fwrite(&x, sizeof(int), 1, a);
    x = 0x00000000;
    fwrite(&x, sizeof(int), 1, a);

    fclose(a);*/




    CargarImagen();
    return 0;
}

int buscarMnemonico(int celda)
{
    return celda >> 16;
}
void tipoOperandos(int celda, int op)
{
    switch (celda)
    {
        case 0x00:
            printf(" %d", op);
        break;
        case 0x01:
            printf(" %xx", op&0x0000000f);
        break;
        case 0x02:
            printf(" [%s:%d]", ((op&0xf0000000) == 0x20000000)? "DS":"ES",op&0x0fffffff);
        break;
    }
}

void IdentificarInstruccion(T_FUNC *mnemonicos, int Inst, int Op1, int Op2)
{
    double a, b;
    int index = buscarMnemonico(Inst);
    mnemonicos[index](&a,&b);

    tipoOperandos((Inst&0x0000FF00) >> 8, Op1);
    printf(",");
    tipoOperandos(Inst&0x000000FF, Op2);

    printf("\n");
}
void CargarImagen()
{
    FILE *imgFile = fopen("d:\\testSize2.txt", "rb");
    int codInstruccion, codOperando1, codOperando2, n = 0;
    T_FUNC mnemonicos[256];
    CargarMnemonicos (mnemonicos);

    while (!feof(imgFile))
    {
        fread(&codInstruccion, sizeof(int), 1, imgFile);
        fread(&codOperando1, sizeof(int), 1, imgFile);
        fread(&codOperando2, sizeof(int), 1, imgFile);

        IdentificarInstruccion(mnemonicos, codInstruccion, codOperando1, codOperando2);

        n++;
    }

    fclose(imgFile);
}

//----------------------------------------------------------------

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

void func_MOV(double *arg1, double *arg2)
{
    printf("MOV");
}
void func_ADD(double *arg1, double *arg2)
{
    printf("ADD");
}
void func_SUB(double *arg1, double *arg2)
{
    printf("SUB");
}
void func_MUL(double *arg1, double *arg2)
{
    printf("MUL");
}
void func_DIV(double *arg1, double *arg2)
{
    printf("DIV");
}
void func_MOD(double *arg1, double *arg2)
{
    printf("MOD");
}
void func_CMP(double *arg1, double *arg2)
{
    printf("CMP");
}
void func_SWAP(double *arg1, double *arg2)
{
    printf("SWAP");
}
void func_RND(double *arg1, double *arg2)
{
    printf("RND");
}
void func_AND(double *arg1, double *arg2)
{
    printf("AND");
}
void func_OR(double *arg1, double *arg2)
{
    printf("OR");
}
void func_NOT(double *arg1, double *arg2)
{
    printf("NOT");
}
void func_XOR(double *arg1, double *arg2)
{
    printf("XOR");
}
void func_SHL(double *arg1, double *arg2)
{
    printf("SHL");
}
void func_SHR(double *arg1, double *arg2)
{
    printf("SHR");
}
void func_JMP(double *arg1, double *arg2)
{
    printf("JMP");
}
void func_JE(double *arg1, double *arg2)
{
    printf("JE");
}
void func_JG(double *arg1, double *arg2)
{
    printf("JG");
}
void func_JL(double *arg1, double *arg2)
{
    printf("JL");
}
void func_JZ(double *arg1, double *arg2)
{
    printf("JZ");
}
void func_JP(double *arg1, double *arg2)
{
    printf("JP");
}
void func_JN(double *arg1, double *arg2)
{
    printf("JN");
}
void func_JNZ(double *arg1, double *arg2)
{
    printf("JNZ");
}
void func_JNP(double *arg1, double *arg2)
{
    printf("JNP");
}
void func_JNN(double *arg1, double *arg2)
{
    printf("JNN");
}
void func_SYS(double *arg1, double *arg2)
{
    printf("SYS");
}
void func_STOP(double *arg1, double *arg2)
{
    printf("STOP");
}
