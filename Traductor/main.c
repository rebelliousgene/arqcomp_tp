#include <stdio.h>
#include <string.h>
#include <ctype.h>

void InicializarRegistros(int reg[16],int ram[2000]);
void LeerAsmFile();
void Mnemonico(char ins[5],int nroinstruccion);
void Operando(char op[8],int nroinstruccion,int reg[16]);

int main(){
    int reg[16];
    int ram[2000];
    void InicializarRegistros(reg,ram);
    void LeerAsmFile();
    return 0;
}

void InicializarRegistros(int reg[16],int ram[2000]){
    int i;
    for(i=0;i<16;i++){
        reg[i]=0x0000;
    }
    for(i=0;i<2000;i++){
        ram[i]=0x0000;
    }
    reg[2]=500;     //DS empieza en la posicion 500 de RAM
    reg[3]=1000;        //ES empieza en la posicion 1000 de RAM
}

void LeerAsmFile(){
    FILE *asmFile = fopen("d:\\test.txt", "rt");
    char cadena[20],texto[20];
    char c;
    int indice = 0,nrolinea=0;
	int ins,op1,op2,i,j;
	ins=op1=op2=0; //*Para controlar que se cargo

	fscanf(asmFile,"%c",&c);
	while(!feof(asmFile)){
        i=0; nrolinea++;
        while(c!='\n'){     //Copia una linea del archivo en 'texto'
            texto[i++]=c;
            fscanf(asmFile,"%c",&c);
        }
        i=0;
        while(texto[i]==' '){ //Salta los espacios que hay antes del mnemonico
            i++;
        }
        c=texto[i];
        while(i<strlen(texto) && c!='/'){  //Pasa a la siguiente linea cuando haya terminado o haya un comentario
            while(c!=' '){      //Copia en 'cadena' una parte de 'texto'
                if(c==',' || c=='\t')
                    c=' ';
                else{
                    cadena[indice++]=c;
                    c=texto[++i];
                }
            }
            cadena[indice]='\0';
            if (cadena[0]!='\0'){
                if(ins==0){
                    for(j=0;j<strlen(cadena);j++)
                        cadena[j]=toupper(cadena[j]);
                    //Carga el mnemonico
                    ins=1;
                }
                else{
                    if(op1==0){
                        //Carga el primer operando
                        op1=1;
                    }
                    else{
                        //*Carga el seundo operando
                        op2=1;
                    }
                }
            }
        indice=0;
        c=texto[i];
        }
        ins=op1=op2=0;
        fscanf(asmFile,"%c",&c);
    }
    fclose(asmFile);
}

void Mnemonico(char ins[5],int nroinstruccion){
    int cod;
    if(!strcmp(ins,"MOV"))
        cod=0x01;
    if(!strcmp(ins,"ADD"))
        cod=0x02;
    if(!strcmp(ins,"SUB"))
        cod=0x03;
    if(!strcmp(ins,"MUL"))
        cod=0x04;
    if(!strcmp(ins,"DIV"))
        cod=0x05;
    if(!strcmp(ins,"MOD"))
        cod=0x06;
    if(!strcmp(ins,"CMP"))
        cod=0x13;
    if(!strcmp(ins,"SWAP"))
        cod=0x17;
    if(!strcmp(ins,"RND"))
        cod=0x19;
    if(!strcmp(ins,"AND"))
        cod=0x31;
    if(!strcmp(ins,"OR"))
        cod=0x32;
    if(!strcmp(ins,"NOT"))
        cod=0x33;
    if(!strcmp(ins,"XOR"))
        cod=0x34;
    if(!strcmp(ins,"SHL"))
        cod=0x37;
    if(!strcmp(ins,"SHR"))
        cod=0x38;
    if(!strcmp(ins,"JMP"))
        cod=0x20;
    if(!strcmp(ins,"JE"))
        cod=0x21;
    if(!strcmp(ins,"JG"))
        cod=0x22;
    if(!strcmp(ins,"JL"))
        cod=0x23;
    if(!strcmp(ins,"JZ"))
        cod=0x24;
    if(!strcmp(ins,"JP"))
        cod=0x25;
    if(!strcmp(ins,"JN"))
        cod=0x26;
    if(!strcmp(ins,"JNZ"))
        cod=0x27;
    if(!strcmp(ins,"JNP"))
        cod=0x28;
    if(!strcmp(ins,"JNN"))
        cod=0x29;
    if(!strcmp(ins,"SYS"))
        cod=0x81;
    if(!strcmp(ins,"STOP"))
        cod=0x8F;
}
void Operando(char op[8],int nroinstruccion,int reg[16]){
    int tipo,val,i,j; char aux[3];
    if(!strcmp(op,"AX")){
        tipo=0x01; val=0xA;
    }if(!strcmp(op,"BX")){
        tipo=0x01; val=0xB;
    }if(!strcmp(op,"CX")){
        tipo=0x01; val=0xC;
    }if(!strcmp(op,"DX")){
        tipo=0x01; val=0xD;
    }if(!strcmp(op,"EX")){
        tipo=0x01; val=0xE;
    }if(!strcmp(op,"FX")){
        tipo=0x01; val=0xF;
    }
    if(op[0]=='['){
        tipo=0x02; j=0;
        if(op[3]==':'){
            aux[0]=op[1];aux[1]=op[2];
            if(!strcmp(aux,"DS"))
                val=reg[2];
            if(!strcmp(aux,"ES"))
                val=reg[3];
            i=4;
        }
        else{
            val=0;i=1;
        }
        while(op[i]!=']'){
            aux[j]=op[i];
        }
        val+=atoi(aux);
    }
    else{
        val=atoi(op);
    }
}
