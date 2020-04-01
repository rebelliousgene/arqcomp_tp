#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DS 2
#define ES 3
#define IP 4
#define AC 8
#define CC 9

#define AX 0x10
#define BX 0x11
#define CX 0x12
#define DX 0x13
#define EX 0x14
#define FX 0x15


typedef struct
{
    char nom[4];
    int num;
} Registro;

typedef struct
{
    char cod[8];
    char op1[4];
    char op2[4];
} instruccion;


typedef struct
{
    char nom[11];
    int linea;
} Rotulo;



void inicCodigos(char mnem[][4])
{
    strcpy(mnem[0x01],"MOV");
    strcpy(mnem[0x02],"ADD");
    strcpy(mnem[0x03],"SUB");
    strcpy(mnem[0x04],"MUL");
    strcpy(mnem[0x05],"DIV");
    strcpy(mnem[0x06],"MOD");
    strcpy(mnem[0x13],"CMP");
    strcpy(mnem[0x17],"RND");
    strcpy(mnem[0x19],"SWAP");
    strcpy(mnem[0x20],"JMP");
    strcpy(mnem[0x21],"JE");
    strcpy(mnem[0x22],"JG");
    strcpy(mnem[0x23],"JL");
    strcpy(mnem[0x24],"JZ");
    strcpy(mnem[0x25],"JP");
    strcpy(mnem[0x26],"JN");
    strcpy(mnem[0x27],"JNZ");
    strcpy(mnem[0x28],"JNP");
    strcpy(mnem[0x29],"JNN");
    strcpy(mnem[0x81],"SYS");
    strcpy(mnem[0x8F],"STOP");
}


void inicRegistros(Registro registros[])
{
    strcpy(registros[AX].nom,"AX");
    strcpy(registros[BX].nom,"BX");
    strcpy(registros[CX].nom,"CX");
    strcpy(registros[DX].nom,"DX");
    strcpy(registros[EX].nom,"EX");
    strcpy(registros[FX].nom,"FX");

}


int comparaCodigos(char *pMnemonico, char mnem[][4], int cantCodigos)
{
    int enc=-1;
    int i=1;
    while ( i <= cantCodigos && enc == -1)
    {
        if (strstr(pMnemonico,mnem[i]))
        {
            enc = i;
        }
        i++;
    }
    return enc;
}


int siEsRegistro(char *op, Registro registros[])
{
    int i = AX;
    int enc=-1;
    while (i <= FX && enc==-1)
    {
        if (strcmp(op,registros[i].nom))
        {
            enc = i;
        }
        i++;
    }

    return enc;
}


int devOperando(char *op, Registro registros[])
{
    char operando[4]= {0x00,0x00,0x00,0x00};
    int tipo = -1;
    int reg;
    int num;
    reg=siEsRegistro(op,registros);
    if (reg != -1)
    {
        tipo=0x01;
        operando[0] = reg;
    }
    else
    {
        if (strchr(op,'[') && strchr(op,']'))
        {
            tipo=0x02;
            if (strstr(op,"ES"))
            {
                operando[3]=ES;
            }
            else
            {
                operando[3]=DS;
            }

        }
        else
            tipo=0;
    }
//    strchr(op,'#') || strchr(op,'@') || strchr(op,'#')
    return tipo;
}


void corteDatos(char *pRotulo, char *pMnemonico, char mnem[][4], int cantCodigos, Registro registros[])
{
    int op1=0;  ///En caso de que no posea algun operando
//    int op2=0;
    instruccion v;
int indice=-1;
    char *operando1=NULL;
    char *operando2=NULL;
    char *Mnemonico;
    Mnemonico = strtok(pMnemonico," ");
    printf("mnemonico: %s\n", Mnemonico);
   indice = comparaCodigos(Mnemonico,mnem,cantCodigos);
    if (indice != -1)
    {
        operando1 = strtok(NULL," ,");
        printf("OPERANDO1: %s\n",operando1);
        operando2= strtok(NULL,", ");
        printf("operando2 %s",operando2);
        strcpy(v.cod, mnem[indice]);
        strcpy(v.op1,operando1);
        strcpy(v.op2,operando2);
        op1= devOperando(operando1,registros);
    }
    else
    {
        printf("Error, no se encontró el mnemonico\n");
    }




//    if (operando1)
//        op1 = atoi(operando1);
//    if (operando2)
//        op2=atoi(operando2);


}



void traduccion(char *asmNOM, int reg[16], int ram[2000], Rotulo vecRotulos[], char mnem[][4], int cantCodigos, Registro registros[])
{
    char *assembler = (char*)malloc(sizeof(char)*128);
    int nLinea=0;
    Rotulo r;
    char *pRotulo=NULL;
    char *pRenglon = (char *)malloc(sizeof(char)*128);
    char *pMnemonico;
    char *p;
    char *cadena;
    FILE *archASM = fopen(asmNOM,"rt");
    instruccion ins;
    int i=0;

    if (archASM)
    {

       fgets(assembler,43,archASM);
        while (!feof(archASM))
        {

            printf("assembler: %s\n",assembler);
            strcpy(pRenglon,assembler);
            cadena=strtok(pRenglon,"\t\n");
            strupr(cadena);
            if (cadena[0] != '\0')
            {
                if (strstr(cadena, "//") == cadena)  ///SI ES SOLO UN COMENTARIO
                {
                    printf("ES SOLO UN COMENTARIO\n");
                    cadena=strstr(cadena,"//");
                    printf("%s", cadena);
                }
                else
                {
                    nLinea++;
                    if (strchr(cadena,':'))
                    {
                        pRotulo = cadena;
                        pMnemonico = strstr(cadena, ":");
                        *(pMnemonico) = '\0';
                        pMnemonico++;
                        printf("pMnemonico %s", pMnemonico);
                    }
                    else
                    {
                        pMnemonico = cadena;
                    }
                    if (p=strstr(pMnemonico,"//"))
                    {
                        *p= '\0';
                    }
//            printf("assembler: %s\n",assembler);
                    if (pRotulo != NULL)
                    {
                        printf("rotulo:%s\n",pRotulo);
                        r.linea = nLinea;
                        strcpy(r.nom,pRotulo);
                        vecRotulos[i] = r;
                        i++;
                    }

                    nLinea++;
                    ///pulido en el mnemonico
                    while (*pMnemonico == ' ')
                        pMnemonico++;


                   corteDatos(pRotulo,pMnemonico,mnem,cantCodigos,registros);
                }
            }
           fgets(assembler,43,archASM);
        }


    }
    fclose(archASM);
}



int main(int argc, char *argv[])
{
    int reg[16];
    int ram[2000];
    char mnem[144][4];
    Rotulo vecRotulos[10];
    int cantCodigos=0x8F;
    Registro registros[16];
    inicRegistros(registros);

    char asmNOM[] = "archASM.asm";
    char *imgNOM;
    int i;
    char *comando;
    traduccion(asmNOM,reg,ram,vecRotulos,mnem,cantCodigos,registros);


//    for (int i = 0 ; i< argc; i++)
//    {
//        if (strstr(argv[i], ".asm"))
//        {
//            asmNOM= (char *)malloc(strlen(argv[i])*sizeof(char));
//            asmNOM = argv[i];
//            traduccion(asmNOM,reg,ram,vecRotulos);
//        }
//        if (strstr(argv[i], ".img"))
//        {
//            imgNOM = (char*)malloc(strlen(argv[i])*sizeof(char));
//            imgNOM = argv[i];
//        }
//        if (strstr(argv[i], "-"))
//        {
//            comando = (char*)malloc(strlen(argv[i])*sizeof(char));
//            comando = argv[i];
//        }
//    }
    return 0;
}
