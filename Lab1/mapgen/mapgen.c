#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void viewmap();
void genmap();
void gen(int*,int,int*,int*);
int wallchk(int*,int);
void view(int*);
int prchk(int*,int*,int*);
int distance(int*,int*,int);

int main(){
    char mode;
    while(1){
        printf("Mode(Gen:g, View:v, Quit:q)>");
        scanf(" %c",&mode);
        switch(mode){
            case 'g':
                printf("-------------------------\nMap Generator\nBuild by Y.Narumi@2019\n-------------------------\n");
                genmap();
                break;

            case 'v':
                printf("-------------------------\nMap Viewer\nBuild by Y.Narumi@2019\n-------------------------\n");
                viewmap();
                break;

            case 'q':
	            return 0;

	        default:
	            break;
        }
    }
}

void viewmap(){
    FILE *mapf;
    int i,map[64];
    char fname[32],*p,ss[5];

    printf("Input File Name>");
    scanf(" ");
    fgets(fname,30,stdin);
    p=strchr(fname, '\n');
    if (p!=NULL) *p='\0';
    
    mapf=fopen(fname,"r");
    if(mapf==NULL){
        puts("FILE Open Failed!");
        return;
    }

    i=0;
    while(fgets(ss,3,mapf)!=NULL){
        p=strchr(ss, '\n');
        if (p!=NULL) *p='\0';
        if(isdigit(ss[0])||ss[0]=='-'){ 
            map[i]=atoi(ss);
            i++;
        }
    }
    
    puts("");
    printf("FileName:%s\n",fname);
    view(map);
    puts("");

    fclose(mapf);
    return;
}

void genmap(){
    int walls, pry, prd, i;
    int map[64]={0};
    char fname[32], save, *p;
    FILE *mapf;

    srand((unsigned)time(NULL));
    while(1){
        printf("How many Walls>");
        scanf("%d",&walls);
        if(walls>=0&&walls<30) break;
    }
    while(1){
        gen(map,walls,&pry,&prd);
        if(prchk(map,&pry,&prd)==0) break;
    }    
    
    map[prd]=1;
    view(map);
    puts("");
    save='?';
    while(!(save=='y'||save=='n')){
        printf("This map Save?(y/n)");
        scanf(" %c",&save);
    }
    if(save=='y'){
        printf("Input Save Name>");
        scanf(" ");
        fgets(fname,30,stdin);
        p=strchr(fname, '\n');
        if (p!=NULL) *p='\0';
        mapf=fopen(fname,"w");
        if(mapf==NULL){
            puts("FILE Create Failed!");
            return;
        }
        for(i=0;i<64;i++){
            fprintf(mapf,"%d\n",map[i]);
        }
        fclose(mapf);
    }
    return;
}

void gen(int *map,int walls,int *pry, int *prd){
    int i,gen,flg;
    puts("#logs--------->");
    for(i=0;i<walls;i++){
        while(1){
            gen=rand()%64;
            flg=0;
            if(gen%8!=0) flg+=wallchk(map,gen-1);
            if(gen%8!=7) flg+=wallchk(map,gen+1);
            if(gen>7) flg+=wallchk(map,gen-8);
            if(gen<56) flg+=wallchk(map,gen+8);
            if(flg==0&&map[gen]==0){
                map[gen]=-1;
                printf("%2d: Make Wall\n",gen);
                break;
            }
        }
    }
    while (1){
        gen=rand()%64;
        if(map[gen]==0){
            map[gen]=1;
            printf("%2d: Make Predator\n",gen);
            break;
        }
    }
    *prd=gen;
    while(1){
        gen=rand()%64;
        if(map[gen]==0){
            map[gen]=10;
            printf("%2d: Make Prey\n",gen);
            break;
        }
    }
    *pry=gen;
    return;
}

int wallchk(int *map,int n){
    int neibor,nwall=0;
    if(n%8==0){
        if(map[n+1]!=0) nwall++;
        if(n<8){
            neibor=2;
            if(map[n+8]!=0) nwall++;
        }else if(n>55){
            neibor=2;
            if(map[n-8]!=0) nwall++;
        }else{
            neibor=3;
            if(map[n-8]!=0) nwall++;
            if(map[n+8]!=0) nwall++;
        }
    }else if(n%8==7){
        if(map[n-1]!=0) nwall++;
        if(n<8){
            neibor=2;
            if(map[n+8]!=0) nwall++;
        }else if(n>55){
            neibor=2;
            if(map[n-8]!=0) nwall++;
        }else{
            neibor=3;
            if(map[n-8]!=0) nwall++;
            if(map[n+8]!=0) nwall++;
        }
    }else{
        if(map[n-1]!=0) nwall++;
        if(map[n-1]!=0) nwall++;
        if(n<8){
            neibor=3;
            if(map[n+8]!=0) nwall++;
        }else if(n>55){
            neibor=3;
            if(map[n-8]!=0) nwall++;
        }else{
            neibor=4;
            if(map[n-8]!=0) nwall++;
            if(map[n+8]!=0) nwall++;
        }
    }
    if((neibor-nwall)<=1) return 1;
    printf("%2d: OK!\n",n);
    return 0;
}

void view(int *map){
    int i,j;
    for(i=0;i<8;i++){
        for(j=0;j<8;j++){
            if(map[i*8+j]==0) printf(".");
            if(map[i*8+j]==-1) printf("#");
            if(map[i*8+j]==1) printf("@");
            if(map[i*8+j]==10) printf("*");
        }
        if(i==4) printf("   .:Free Cell");
        if(i==5) printf("   #:Wall");
        if(i==6) printf("   @:Predator");
        if(i==7) printf("   *:Prey");
        printf("\n");
    }
    return;
}

int prchk(int *map,int *pry,int *prdorg){
    int i,dist[4];
    int prd=*prdorg;
    int prevprd;

    for(i=0;i<64;i++){
        map[prd]=0;
        prevprd=prd;
        if(prd>7){
            if(map[prd-8]!=-1)
                dist[0]=distance(map,pry,prd-8);
            else dist[0]=100;
        }else dist[0]=100;
        if(prd<56){
            if(map[prd+8]!=-1)
                dist[1]=distance(map,pry,prd+8);
            else dist[1]=100;
        }else dist[1]=100;
        if(prd%8!=0){
            if(map[prd-1]!=-1)
                dist[2]=distance(map,pry,prd-1);
            else dist[2]=100;
        }else dist[2]=100;
        if(prd%8!=7){
            if(map[prd+1]!=-1)
                dist[3]=distance(map,pry,prd+1);
            else dist[3]=100;
        }else dist[3]=100;

        if(dist[0]==0||dist[1]==0||dist[2]==0||dist[3]==0){
            puts("Predator->Prey Path Enable!");
            puts("<---------logs#");
            map[prd]=0;
            return 0;
        }

        if(dist[0]<=dist[1]){
            if(dist[0]<=dist[2]){
                if(dist[0]<=dist[3])
                    prd-=8;
            }
        }
        if(dist[1]<=dist[2]){
            if(dist[1]<=dist[3]){
                if(dist[1]<=dist[0])
                    prd+=8;
            }
        }
        if(dist[2]<=dist[1]){
            if(dist[2]<=dist[0]){
                if(dist[2]<=dist[3])
                    prd-=1;
            }
        }
        if(dist[3]<=dist[2]){
            if(dist[3]<=dist[1]){
                if(dist[3]<=dist[0])
                    prd+=1;
            }
        }
        //map[prd]=1;
        //view(map);
        //puts("");
    }
    puts("Predator->Prey Path Disable!");
    return -1;
}

int distance(int *map,int *pry,int prd){
    int x,y;
    y=(prd-*pry)/8;
    x=prd-(*pry+y*8);
    return abs(x)+abs(y);
}