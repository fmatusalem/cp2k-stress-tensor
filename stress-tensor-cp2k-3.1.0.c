/*BY FILIPE MATUSALEM, SEPT 2020     filipematus@gmail.com */
/*Program to extract and average the stress tensor from CP2K output*/
/*Compilation: g++ -o stress_tensor-cp2k.x stress_tensor-cp2k.c*/
#include <stdio.h>
#include <getopt.h> // *GNU* Para o getopt_long()
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int main(int argc, char *argv[])
{
FILE *cp2kout,*stress_tensor,*moving;
int i,j,k,l,nsteps,minstep,sinal,delay;
char str1[150],ch,lixo[150];
float min,percent,version;

if( argc < 2 ){
printf("\n\n");
printf("EEEEEEE   RRRRRRRR   RRRRRRRR   OOOOOOO   RRRRRRRR\n");
printf("EE        RR    RR   RR    RR   OO   OO   RR    RR\n");
printf("EE        RR    RR   RR    RR   OO   OO   RR    RR\n");
printf("EEEE      RRRRRRRR   RRRRRRRR   OO   OO   RRRRRRRR\n");
printf("EE        RRRR       RRRR       OO   OO   RRRR\n");
printf("EE        RR  RR     RR  RR     OO   OO   RR  RR\n");
printf("EEEEEEE   RR    RR   RR    RR   OOOOOOO   RR    RR\n\n");

printf("Enter the name of cp2k output file: -f filename. Use -h for help. \n\n");

 exit(0);}


printf("-----------------------------------------------------------------------------------------------\n\n");
printf("----------------Extract stress tensor components from CP2K output results-----------------\n\n");


min=0;
percent=1;
delay=1000000;

/*-----------arguments---------------*/

	int opt=0;  

//Specifying the expected options

static struct option long_options[] = {
        {"inputfile", required_argument, 0,  'f' },
        {"exclude"  , required_argument, 0,  'e' },
        {"percent"  , required_argument, 0,  'p' },
        {"average"  , required_argument, 0,  'a' },        
        {"help"     ,       no_argument, 0,  'h' },
        {0,           0,                 0,   0  }
    };

int long_index =0;
    while ((opt = getopt_long(argc, argv,"f:e:p:a:h",long_options, &long_index )) != -1) {
        switch (opt) {
                        case 'f' : strcpy(str1,optarg); printf ("option -f // CP2K output filename: %s\n", optarg);
                                break;

                        case 'e' : min = atof(optarg); printf ("option -e // Eliminate the first %s %% MD steps\n", optarg);
                                break;

                        case 'p' : percent = atof(optarg); printf ("option -p // Strain imposed at each MD step: %s %%\n", optarg);
                                break;

                        case 'a' : delay = atoi(optarg); printf ("option -a // Number of MD steps included in moving average: %s MD steps\n", optarg);
                                break;

                                
                        case 'h' : // Ajuda
                                printf("\n-f [string] The name of the cp2k output file must be entered!! Option -f filename\n");
                                printf("-e [float] Can be entered to eliminate some first steps (in percentage). \n");
                                printf("-p [float] Multiply the step number by a value. Useful to convert the step number to time or percentage of strain.\n");
                                printf("-a [int] Enter the number of steps to compute moving average. Default 3%% MD steps\n");
                                printf("-h print this help.\n\n");
                                exit(0);
                        default : // Qualquer parametro nao tratado
                                printf("Incorrect parameter, please use option -h for help!\n");
                                exit(0);
        }
    }

printf("\n-----------------------------------------------------------------------------------------------\n\n");


cp2kout = fopen(str1,"r"); /* Arquivo ASCII, para leitura */
if(!cp2kout)
{
printf( "Error opening argument 1 file\n");

exit(0);
}

stress_tensor = fopen("stress_tensor.dat","w"); /* Arquivo ASCII, para escrita */
if(!stress_tensor)
{
printf( "Error creating stress_tensor.dat file\n");
exit(0);
}

fprintf(stress_tensor,"#step sigma11 sigma12 sigma13 sigma21 sigma22 sigma23 sigma31 sigma32 sigma33\n");

moving = fopen("stress_tensor_moving_ave.dat","w"); /* Arquivo ASCII, para escrita */
if(!moving)
{
printf( "Error creating stress_tensor_moving_ave.dat file\n");
exit(0);
}

fprintf(moving,"#step sigma11 sigma12 sigma13 sigma21 sigma22 sigma23 sigma31 sigma32 sigma33\n");

//find cp2k version
do
fscanf(cp2kout,"%s",str1);                                      /*posiciona o  após a palavra xxx*/
while(strcmp(str1,"version")!=0);
do
fscanf(cp2kout,"%s",str1);                                      /*posiciona o  após a palavra xxx*/
while(strcmp(str1,"version")!=0);
fscanf(cp2kout,"%f",&version);

printf("\nCP2K version: %.2f\n\n",version);


nsteps=0;
while (fscanf(cp2kout,"%s",str1) != EOF){            /*conta steps*/
if(strcmp(str1,"ENERGY|")==0)nsteps++;                      
}

minstep=(int)floor(min*nsteps/100);


printf("\nTotal number of MD steps = %d\n",nsteps);
if(minstep!=0)printf("Number of initial MD steps excluded = %d\n",minstep);

if(delay==1000000)delay=(int)floor(0.03*nsteps);
printf("Steps in moving average = %d\n",delay);

rewind(cp2kout);

float tensor[3][3],average[9],moving_ave[3][3][delay];

for(i=0;i<delay;i++)for(j=0;j<3;j++){
moving_ave[j][0][i]=0;
moving_ave[j][1][i]=0;
moving_ave[j][2][i]=0;
}


k=0;
for(j=0;j<nsteps;j++){

do
fscanf(cp2kout,"%s",str1);                                      /*posiciona o  após a palavra xxx*/
while(strcmp(str1,"ENERGY|")!=0);

//cp2k version >=8.1
if(version>8){
for(i=0;i<4;i++){
do
ch = getc(cp2kout);              /*chega ao fim da linha*/
while(ch!='\n');
}

for(i=0;i<3;i++){
fscanf(cp2kout,"%s",lixo);fscanf(cp2kout,"%s",lixo);
fscanf(cp2kout,"%f",&tensor[i][0]);
fscanf(cp2kout,"%f",&tensor[i][1]);
fscanf(cp2kout,"%f",&tensor[i][2]);
}
}
else {
for(i=0;i<6;i++){
do
ch = getc(cp2kout);              /*chega ao fim da linha*/
while(ch!='\n');
}

for(i=0;i<3;i++){
fscanf(cp2kout,"%s",lixo);
fscanf(cp2kout,"%f",&tensor[i][0]);
fscanf(cp2kout,"%f",&tensor[i][1]);
fscanf(cp2kout,"%f",&tensor[i][2]);
}
}


moving_ave[0][0][j%delay]=tensor[0][0];
moving_ave[0][1][j%delay]=tensor[0][1];
moving_ave[0][2][j%delay]=tensor[0][2];
moving_ave[1][0][j%delay]=tensor[1][0];
moving_ave[1][1][j%delay]=tensor[1][1];
moving_ave[1][2][j%delay]=tensor[1][2];
moving_ave[2][0][j%delay]=tensor[2][0];
moving_ave[2][1][j%delay]=tensor[2][1];
moving_ave[2][2][j%delay]=tensor[2][2];

//printf("%d\n",j%delay);

if(j>=minstep){

for(i=0;i<9;i++)average[i]=0;

if(j>delay){

for(i=0;i<delay;i++){
average[0]=average[0]+moving_ave[0][0][i];
average[1]=average[1]+moving_ave[0][1][i];
average[2]=average[2]+moving_ave[0][2][i];
average[3]=average[3]+moving_ave[1][0][i];
average[4]=average[4]+moving_ave[1][1][i];
average[5]=average[5]+moving_ave[1][2][i];
average[6]=average[6]+moving_ave[2][0][i];
average[7]=average[7]+moving_ave[2][1][i];
average[8]=average[8]+moving_ave[2][2][i];
}

average[0]=average[0]/delay;
average[1]=average[1]/delay;
average[2]=average[2]/delay;
average[3]=average[3]/delay;
average[4]=average[4]/delay;
average[5]=average[5]/delay;
average[6]=average[6]/delay;
average[7]=average[7]/delay;
average[8]=average[8]/delay;
}


//printf("l=%d j=%d j_mod_delay=%d moving_ave020=%f moving_ave021=%f moving_ave022=%f average[2]=%f\n",l,j,j%delay,moving_ave[0][2][0],moving_ave[0][2][1],moving_ave[0][2][2],average[2]);

fprintf(stress_tensor,"%f %f %f %f %f %f %f %f %f %f \n",percent*k,tensor[0][0],tensor[0][1],tensor[0][2],tensor[1][0],tensor[1][1],tensor[1][2],tensor[2][0],tensor[2][1],tensor[2][2]);

if(j>delay)fprintf(moving,"%f %f %f %f %f %f %f %f %f %f \n",percent*(k-delay/2),average[0],average[1],average[2],average[3],average[4],average[5],average[6],average[7],average[8]);


k++;


}
}


printf("Stress tensor components and averages written to stress_tensor.dat and stress_tensor_moving_ave.dat!\n\n");
if(minstep>0)printf("The first %d MD steps were not counted to the average\n\n",minstep);
printf("-----------------------------------------------------------------------------------------------\n\n");

fclose(cp2kout);
fclose(stress_tensor);
fclose(moving);
}
