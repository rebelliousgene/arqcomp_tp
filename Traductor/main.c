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


typedef struct ///para asociar nombre con valor de los registros
{
    char nom[4];
} Registro;

typedef struct ///instruccion a guardar en IMG
{
    int v1; ///32bits
    int v2; /// 32 bits

}tipoInst;

typedef struct ///codigo instruccion antes de pasar a maquina
{
    char cod[2];
    char op1;
    char op2;
} instruccion;


typedef struct ///estructura de rotulo
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


int comparaCodigos(char *pInstruccion, char mnem[][4], int cantCodigos)
{
    int enc=-1;
    int i=1;
    while ( i <= cantCodigos && enc == -1)
    {
        if (strstr(pInstruccion,mnem[i]))
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


int devOperando(char *op, char *tipo; Registro registros[]) ///modifico tipo y devuelvo los 32 bits codOP
{
    char operando[4]= {0x00,0x00,0x00,0x00};
    int entero=0; ///para conversiones;
    int reg;
    int codOP; /// representa 32 bits de codigo operando
    if (op != NULL)
    {
    reg=siEsRegistro(op,registros);
    if (reg != -1)
    {
        *tipo=0x01;
        operando[3] = reg; ///utiliza ultimos 4 bits;

    }
    else
    {
        if (strchr(op,'[') && strchr(op,']'))
        {
            *tipo=0x02;
            if (strstr(op,"ES"))
            {
                operando[0]=ES;
                operando[0] = operando[0] << 4;
            }
            else
            {
                operando[0]=DS;
                operando[0]= operando[0] << 4;
            }

        }
        else
        {
            *tipo=0x00;
            if (strchr(operando,'@')) ///si es octal
            {
                while (op[1] != 0)
                {
                entero= entero + op[1] & 00000111;
                op[1] = op[1] >>3;
                }
            }
            else if (strchr(operando,'%')) ///si es hexa
            {

            }
            else
            {///es decimal

            }
        }
    }
//    strchr(op,'#') || strchr(op,'@') || strchr(op,'#')

    }
    else
        codOP=0;
    return codOP;
}


void corteDatos(char *pRotulo, char *pInstruccion, char mnem[][4], int cantCodigos, Registro registros[])
{
    int codOP1=0;  ///En caso de que no posea algun operando
    int codOP2=0;
    instruccion v;
    int indice=-1;
    char *operando1=NULL;
    char *operando2=NULL;
    char *Mnemonico;
    Mnemonico = strtok(pInstruccion," ");
    printf("mnemonico: %s\n", Mnemonico);
    indice = comparaCodigos(Mnemonico,mnem,cantCodigos);
    if (indice != -1)
    {
        strcpy(v.cod, mnem[indice]);
        operando1 = strtok(NULL," ,");
        if (operando1)
        {
            strcpy(v.op1,operando1);
            printf("OPERANDO1: %s\n",operando1);
        }
        operando2= strtok(NULL,", ");
        if (operando2)
        {
            printf("operando2 %s",operando2);
            strcpy(v.op2,operando2); ///ver si qutiar
        }

       codOP1=devOperando(operando1,registros); /// devuelve los codigos de operando de 32 bits
       codOP2=devOperando(operando2,registros);
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
    char *pInstruccion;
    char *p;
    char *cadena;
    FILE *archASM = fopen(asmNOM,"rt");
    instruccion ins;
    char dato;
    int i=0;
    int k=1;

    if (archASM)
    {
      dato = fgetc(archASM);
        while (!feof(archASM))
        {
            assembler[0]=dato;
            while(dato != '\n' && dato != EOF)
            {
                dato=fgetc(archASM);
                assembler[k] = dato;
                k++;
            }
            assembler[k]= '\0';
            k=1;

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
                        pInstruccion = strstr(cadena, ":");
                        *(pInstruccion) = '\0';
                        pInstruccion++;
                        printf("pInstruccion %s", pInstruccion);
                    }
                    else
                    {
                        pInstruccion = cadena;
                    }
                    if (p=strstr(pInstruccion,"//"))
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
                    while (*pInstruccion == ' ')
                        pInstruccion++;


                    corteDatos(pRotulo,pInstruccion,mnem,cantCodigos,registros);
                }
            }
        dato = fgetc(archASM);
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
