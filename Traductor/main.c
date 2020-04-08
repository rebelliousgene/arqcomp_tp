#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
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


typedef struct ///para asociar nombre con valor de los registros
{
    char nom[2];
    int valor;
} Registro;

typedef struct ///linea a guardar en IMG
{
    int codIns; ///32bits de instruccion pasado a entero
    int CODop1; ///cod operando1 32 bits
    int CODop2; ///cod operando2 32 bits
} tipoInst;


typedef struct ///estructura de rotulo
{
    char nom[15];
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
    strcpy(mnem[0x07],"");
    strcpy(mnem[0x08],"");
    strcpy(mnem[0x09],"");
    strcpy(mnem[0x10],"");
    strcpy(mnem[0x11],"");
    strcpy(mnem[0x12],"");
    strcpy(mnem[0x13],"CMP");
    strcpy(mnem[0x14],"");
    strcpy(mnem[0x15],"");
    strcpy(mnem[0x16],"");
    strcpy(mnem[0x17],"SWAP");
    strcpy(mnem[0x18],"");
    strcpy(mnem[0x19],"RND");
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
    strcpy(mnem[0x31],"AND");
    strcpy(mnem[0x32],"OR");
    strcpy(mnem[0x33],"NOT");
    strcpy(mnem[0x34],"XOR");
    strcpy(mnem[0x37],"SHL");
    strcpy(mnem[0x38],"SHR");
    strcpy(mnem[0x44],"RET");
    strcpy(mnem[0x81],"SYS");
    strcpy(mnem[0x8F],"STOP");
}


void inicRegistros(Registro registros[])
{
    registros[0].valor=0;
    registros[1].valor=0;
    strcpy(registros[DS].nom,"DS");
    registros[DS].valor=0;
    strcpy(registros[ES].nom,"ES");
    registros[ES].valor=1000;
    strcpy(registros[IP].nom,"IP");
    registros[DS].valor=0;
    strcpy(registros[AC].nom,"AC");
    registros[5].valor=0;
    registros[6].valor=0;
    registros[7].valor=0;
    registros[AC].valor=0;
    strcpy(registros[CC].nom,"CC");
    registros[CC].valor=0;
    strcpy(registros[AX].nom,"AX");
    registros[AX].valor=0;
    strcpy(registros[BX].nom,"BX");
    registros[BX].valor=0;
    strcpy(registros[CX].nom,"CX");
    registros[CX].valor=0;
    strcpy(registros[DX].nom,"DX");
    registros[DX].valor=0;
    strcpy(registros[EX].nom,"EX");
    registros[EX].valor=0;
    strcpy(registros[FX].nom,"FX");
    registros[FX].valor=0;
}


int comparaCodigos(char *pInstruccion, char mnem[][4])
{
    int enc=-1;
    int i=0x01;
    while ( i <= 0x8F && enc == -1)
    {

        if (!strcmp(pInstruccion,mnem[i]))
        {
            enc = i;
        }
        i= i + 0x01;
    }
    return enc;
}


int siEsRegistro(char *op, Registro registros[])
{
    int i = 10;
    int enc=-1;
    while (i < 16 && strcmp(op,registros[i].nom) != 0)
    {
        i++;
    }
    if (i<16)
        enc=i;
    return enc;
}

int siEsRotulo(char *op, Rotulo v[], int cantR)
{
    int i =0;
    int enc=-1;
    while (i< cantR && enc == -1 )
    {
        if (!strcmp(op,v[i].nom))
            enc = i;

        i++;
    }
    return enc;
}


int devOperando(int *tipo,char *op, Registro registros[], int indiceMNEM, Rotulo vecRotulos[], int cantR) ///modifico tipo y devuelvo los 32 bits codOP
{
    long int entero=0; ///para conversiones;
    int reg;
    char *trash;
    int codOP; /// representa 32 bits de codigo operando maquina
//    printf("indice MNEM %X", indiceMNEM);
    if (op && op[0] != ' ' && op[0] != '\n' && op[0] != '\0')
    {
        reg=siEsRegistro(op,registros);
        if (reg != -1)
        {
            printf("\treg: %04X\n", reg);
            *tipo=1;
            codOP=reg; ///ultimos 4 bits

        }
        else
        {
            if (strchr(op,'[') && strchr(op,']')) ///directo
            {
                *tipo=2;
                char* opAux=strtok(op,"[ ]");
                if (strchr(opAux,':'))
                {
                    if(strstr(opAux,"ES"))
                    {
                        codOP = ES;
                    }
                    else if(strstr(opAux,"DS"))
                    {
                        codOP=DS;
                    }
                    opAux=opAux+3;
                    codOP = (codOP << 28);
                    codOP = codOP | atoi(opAux);
                    printf("num %X\n", codOP);
                }
                else
                {
                    if (isdigit(opAux[0]))
                    {
                        codOP= DS;
                        codOP = (codOP << 28) | atoi(opAux); ///expresión en 32 bits del registro
                    }
                    else
                    {
                        codOP=0xFFFFFFFF;
                        printf("\tERROR: NO ES UNA EXPRESIÓN VALIDA\n");
                    }
                }
            }
            else
            {
                *tipo=0;
                if (strchr(op,'@')) ///si es octal
                {
                    entero = strtoul(++op,&trash,8);
                    codOP= entero;
                }
                else if (strchr(op,'%')) ///si es hexa
                {
                    entero = strtoul(++op,&trash,16);
//                    printf("Hexa : %04x\n",entero);
                    codOP= entero;
//                    printf("codop %04X",codOP);
                }

                else if (strchr(op,'#') || isdigit((op)[0]))
                {
                    if (strchr(op,'#'))
                        op++;

                    entero=atoi(op);
                    codOP= entero;
                }
                else if (((indiceMNEM >= 0x20 && indiceMNEM <=0x29) || indiceMNEM == 0x13) && isalpha(op[0]))
                {
                    int j;
                    j=siEsRotulo(op,vecRotulos,cantR);
                    if(j == -1 || vecRotulos[j].linea == -1)
                    {
                        printf("\tLLAMADA A ROTULO REPETIDO O INEXISTENTE\n");
                        codOP= 0xFFFFFFFF;
                    }
                    else
                        codOP = vecRotulos[j].linea; ///guarda la celda en memoria donde se encuentra el rotulo
                }
                else
                {
                    codOP=0xFFFFFFFF;
                    printf("\tERROR: no corresponden los datos de operando\n");
                }

            }
        }

    }
    else
    {
        *tipo = 0;
        codOP=0;
    }
    return codOP;
}

void impresionLinea(tipoInst linea, char *LineaReal, int nLinea, int nCelda)
{
    printf("\ncodigo Linea\t");
    int aux;
    aux = linea.codIns;
    int i = 1;
    if (linea.codIns != 0xFFFFFFFF)
        printf("[%04X %04X]: ",nCelda>>16,nCelda);
    while ( i >= 0)
    {
        aux = linea.codIns >> 16*i & 0xFFFF;
        printf("%04X ",aux);
        i--;
    }
    i=1;
    while (i>=0)
    {
        aux=linea.CODop1 >> 16*i & 0xFFFF;
        printf("%04X ",aux);
        i--;
    }
    i=1;
    while (i>=0)
    {
        aux=linea.CODop2 >> 16*i & 0xFFFF;
        printf("%04X ",aux);
        i--;
    }
    if (linea.codIns != 0xFFFFFFFF)
        printf("%d: // %s\n",nLinea,LineaReal);


}




tipoInst corteDatos(Rotulo rotulos[], int cantR, char *pInstruccion, char mnem[][4], Registro registros[])
{
    int codOP1=0;  ///En caso de que no posea algun operando
    int codOP2=0;
    int tipo = 0;
    tipoInst ins;
    ins.codIns= 0;
    ins.CODop1= 0;
    ins.CODop2 = 0;
    int tipoOP1, tipoOP2;
    tipoOP1=0;
    tipoOP2=0;
    int indice=-1;
    char *operando1=NULL;
    char *operando2=NULL;
    char *Mnemonico;
    Mnemonico = strtok(pInstruccion," ");
    printf("\nmnemonico: %s\t", Mnemonico);
    indice = comparaCodigos(Mnemonico,mnem);
    printf("INDICE MNEMONICO: %d\t", indice);
    if (indice != -1)
    {
        operando1 = strtok(NULL," ,");
        if (operando1)
        {
            printf("OPERANDO1: %s\t",operando1);
        }
        operando2= strtok(NULL," \0");
        if (operando2)
        {
            printf("operando2 %s\n",operando2);
        }


        codOP1=devOperando(&tipo,operando1,registros,indice,rotulos,cantR); /// devuelve los codigos de operando 1 de 32 bits
        tipoOP1=tipo;
        printf("\nOPERANDO 2\n");
        codOP2=devOperando(&tipo,operando2,registros,0,rotulos,cantR);/// devuelve los codigos de operando 2 de 32 bits
        tipoOP2=tipo;
//        printf("CODOP1: %04X\t", codOP1);
//        printf("CODOP2: %04X\t", codOP2);
//        printf("indice: %04X\t", indice);
//        printf("TIPOOP1: %04X\t", tipoOP1);
//        printf("TIPOOP2: %04X\t",tipoOP2);

    }

    if (indice != -1 && codOP1 != 0xFFFFFFFF && codOP2 != 0xFFFFFFFF  && tipo != 0xFFFFFFFF) /// si no hay errores
    {
        ins.codIns= indice;
        ins.codIns = (ins.codIns<<16) | (tipoOP1 << 8) | tipoOP2;
//        printf("codions %08X\n", ins.codIns);
        ins.CODop1 = codOP1;
        ins.CODop2 = codOP2;
        printf("TODO OK\n");

    }
    else
    {
        printf("\t ERROR: LA INSTRUCCION ES INVALIDA\n");
        ins.codIns = ins.codIns | 0xFFFFFFFF;
        ins.CODop1 = 0xFFFFFFFF;
        ins.CODop2 = 0xFFFFFFFF;
    }

    return ins;
}


int siRotuloRepetido(Rotulo v[], int cantR, char *nuevo)
{
    int i = 0;
    while (i < cantR && strcmp(nuevo,v[i].nom))
    {
        i++;
    }
    return (i < cantR);
}


int lecturaRotulos(char *asmNOM, Rotulo vecRotulos[]) ///LEE LOS ROTULOS A LO LARGO DEL PROGRAMA
{
    FILE *archASM=fopen(asmNOM,"rt");
    char *assembler = (char*)malloc(sizeof(char)*128);
    int nLinea=1; ///Lineas de instrucción
    Rotulo r;
    char *pCadena=NULL;
    char *pRenglon= (char*)malloc(sizeof(char)*128);
    char *pRotulo=NULL;
    char dato;
    int h;
    int i=0; ///para el vector de rotulos
    int k=0; ///para los caracteres de linea en archivo
    if (archASM)
    {
        dato = fgetc(archASM);
        while (!feof(archASM))
        {
            while(dato != EOF && dato != '\n')
            {
                assembler[k] = dato;
                dato=fgetc(archASM);
                k++;
            }
            assembler[k]= '\0'; ///finalizo la linea de assembler
            k=0;
            strcpy(pRenglon,assembler); ///prenglon ahora tiene todo lo de assembler
            strupr(pRenglon);
            if (pRenglon[0] != '\0' && pRenglon[0] != '\n' && pRenglon[0] != '/')
            {
                pCadena=strtok(pRenglon,"\t\n");

                if (strchr(pCadena,':')) ///PUEDE HABER ROTULO Y OPERANDO DIRECTO
                {
                    pRotulo= strtok(pCadena,":");

                    if (strchr(pRotulo,'[')) ///NO TIENE ROTULO
                        pRotulo=NULL;


                    if (pRotulo != NULL)
                    {
                        pRotulo=strtok(pRotulo," ");
                        printf("ROTULO %s\n", pRotulo);
//                        printf("Rotulo %s\n", pRotulo);
                        if (!siRotuloRepetido(vecRotulos,i,pRotulo))
                        {
                            strcpy(r.nom,pRotulo);
                            r.linea=nLinea;
                            vecRotulos[i]=r;
                            i++;
                        }
                        else
                        {
                            h=siEsRotulo(pRotulo,vecRotulos,i);
                            vecRotulos[h].linea = -1; ///hará FFFF en la linea correspondiente a la llamada al rotulo
                        }
                    }
                }
                nLinea++;
            }
            pRotulo=NULL;
            pCadena=NULL;
            dato=fgetc(archASM);
        }
        fclose(archASM);
    }
//    for (int j=0; j < i; j++)
//        printf("%s\n", vecRotulos[j].nom);
    return i;
}


int traduccion(char *asmNOM, int ram[2000], Rotulo vecRotulos[], char mnem[][4], Registro registros[], char comando,int cantR)
{
    tipoInst linea; ///Linea a imprimir y guardar en RAM
    char *assembler = (char*)malloc(sizeof(char)*128);
    int nLinea=1; ///Lineas de instrucción
    int nCelda=0; ///indice de celda de instruccion
    int copiarAIMG=1; ///si existe algun error no copiará a img.
    char *pRenglon = (char *)malloc(sizeof(char)*128); ///renglon leído
    char *pInstruccion;
    char *p;
    char *cadena;
    char *subOp; ///para cortes
    FILE *archASM = fopen(asmNOM,"rt");
    char dato;
    int k=0;

    if (archASM)
    {
        dato = fgetc(archASM);
        while (!feof(archASM))
        {
            while(dato != '\n' && dato != EOF)
            {
                assembler[k]=dato;
                dato=fgetc(archASM);
                k++;
            }
            assembler[k]= '\0';
            k=0;


            if (assembler[0] != '\0' && assembler[0] != '\n')
            {
                strcpy(pRenglon,assembler);
                cadena=strtok(pRenglon,"\t\n");
                strupr(cadena); ///TOUPPER A LA CADENA
                if (cadena[0] == '/')  ///SI ES SOLO UN COMENTARIO
                {
                    printf("ES SOLO UN COMENTARIO\n");
                    cadena=strstr(cadena,"//");
                    printf("%s\n", cadena);
                }
                else
                {
                    if (strchr(cadena,'[') && strchr(cadena,':')) ///PUEDE HABER ROTULO Y OPERANDO DIRECTO
                    {
                        subOp = strstr(cadena,":");
                        if (strchr(subOp,']')) ///NO TIENE ROTULO, TIENE SOLO OPERANDO
                            pInstruccion=cadena;
                        else
                        {
                            subOp=strtok(cadena,":");
                            pInstruccion=strtok(NULL,"\n\0");

                        }

                    }
                    else
                    {
                        if (strchr(cadena,':'))
                        {
                            subOp=strtok(cadena,":");
                            pInstruccion=strtok(NULL,"\n\0");
                        }
                        else
                            pInstruccion=cadena;
                    }
                }

                if ((p=strstr(pInstruccion,"//")))
                {
                    *p= '\0';
                }

                ///pulido en el mnemonico
                while (*pInstruccion == ' ')
                    pInstruccion++;


                linea= corteDatos(vecRotulos,cantR,pInstruccion,mnem,registros);
                nLinea++;

                if (linea.codIns != 0xFFFFFFFF)
                {
                    ram[nCelda] = linea.codIns;
                    ram[nCelda+1] = linea.CODop1;
                    ram[nCelda+2] = linea.CODop2;
                    if (comando != 'o')
                        impresionLinea(linea,assembler,nLinea,nCelda);
                    nCelda +=3;

                }
                else
                {
                    copiarAIMG=0;
                    if (comando != 'o')
                        impresionLinea(linea,assembler,nLinea,nCelda);
                }
            }
            else
            {
                printf("Linea en blanco\n");
            }
//            pRotulo=NULL;
            cadena=NULL;
            pInstruccion=NULL;
            p=NULL;
            dato = fgetc(archASM);
        }
        if (nCelda>0)
            registros[DS].valor=nCelda-2; ///guarda el valor de DS QUE APUNTA AL DATA SEGMENTif (comando != 'o')
    }

    fclose(archASM);
    return copiarAIMG;
}

void copiaraIMG(int RAM[2000], Registro reg[], char *nomIMG)
{
    FILE *archIMG=fopen(nomIMG,"wb");
    int i=0;
    if (archIMG)
    {

        while (i < 16)
        {
            fwrite(&(reg[i].valor),sizeof(int),1,archIMG);
            i++;
        }
        fwrite(RAM,sizeof(int),2000,archIMG);
        fclose(archIMG);
    }

}



int main(int argc, char *argv[])
{
//    int reg[16];
    int ram[2000] = {0};
    char mnem[144][4];
    int c; ///determina si se copia a img o no
    Rotulo vecRotulos[10];
    Registro registros[16];
    inicRegistros(registros);
    inicCodigos(mnem);

//    char asmNOM[] = "archASM.asm";
    char *asmNOM;
    char *imgNOM;
    char comando=' ';
    int cantR;
    //traduccion(asmNOM,reg,ram,vecRotulos,mnem,registros,comando,cantR);


    for (int i = 0 ; i< argc; i++)
    {
        if (strstr(argv[i], ".asm"))
        {
            asmNOM= (char *)malloc(strlen(argv[i])*sizeof(char));
            asmNOM = argv[i];
            cantR=lecturaRotulos(asmNOM,vecRotulos);

        }
        if (strstr(argv[i], "-"))
        {
//            comando = (char*)malloc(strlen(argv[i])*sizeof(char));
            comando = argv[i+1];
        }
        if (strstr(argv[i], ".img"))
        {
            imgNOM = (char*)malloc(strlen(argv[i])*sizeof(char));
            imgNOM = argv[i];
        }

    }
    c=traduccion(asmNOM,ram,vecRotulos,mnem,registros,comando,cantR);
    if (c)
        copiaraIMG(ram,registros,imgNOM);

    return 0;
}
