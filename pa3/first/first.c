#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

int miss;
int hit;
int read;
int write;
struct line** cache;
unsigned long int push;
int write;

typedef struct line{
	unsigned long int tag;
	int valid;
	unsigned long int time;
}line;

void writecache(unsigned long int tagindex,unsigned long int setindex,int assoc);
void readcache(unsigned long int tagindex,unsigned long int setindex,int assoc);
line** createcache(int setnum,int assoc);



int main(int argc, char** argv){
  char work;
  unsigned long int addr;
  unsigned long int setmask;

  int setnum;
  int assoc;
  int offset;
  int setindex;
  int tagindex;
  int blocksize;
  int cachesize;
 
  FILE* fl;
  fl=fopen(argv[5],"r");

//check for direct,assoc
  if(strcmp(argv[3],"fifo")==0){
  	if(strcmp(argv[4],"direct")==0){
		cachesize=atoi(argv[1]);
  		blocksize=atoi(argv[2]);

		assoc=1;
		setnum=cachesize/blocksize;
 	 }

 	 else if(strcmp(argv[4],"assoc")==0){
		cachesize=atoi(argv[1]);
		blocksize=atoi(argv[2]);

		setnum=1;
		assoc=cachesize/blocksize;
 	 }

  	else{
		assoc= argv[4][6] -'0';
		cachesize=atoi(argv[1]);
  		blocksize=atoi(argv[2]);
    		setnum=cachesize/(blocksize*assoc);
  	} 
	offset = log2(blocksize);
        setindex = log2(setnum);
	//tag = 48 - (offset + setindex);
	setmask=((1<<setindex)-1);
	
//create cache
cache=createcache(setnum,assoc);


while(fscanf(fl, "%c %lx\n", &work, &addr)==2){
	setindex=(addr>>offset)&setmask;
	tagindex=addr>>(offset+setindex);
	
	if(work=='R'){
		readcache(tagindex,setindex,assoc);
	}
	
	else if(work=='W'){
		writecache(tagindex,setindex,assoc);
	}
}

fclose(fl);
printf("Memory reads: %d\n",read);
printf("Memory writes: %d\n",write);
printf("Cache hits: %d\n",hit);
printf("Cache misses: %d\n",miss);
}
return 0;
}


//read 
void readcache(unsigned long int tagindex,unsigned long int setindex,int assoc){
  int i;
  int j;
  int m;

	for(i=0;i<assoc;i++){
		if(cache[setindex][i].valid==0){
			miss++;
			read++;
			push++;
			cache[setindex][i].valid=1;
			cache[setindex][i].tag=tagindex;
			cache[setindex][i].time=push;
			return;
		}
		
		else{
			if(cache[setindex][i].valid==1 && cache[setindex][i].tag==tagindex){
				hit++;
				return;
			}
			
			if(cache[setindex][i].valid==1 && i==(assoc-1)){
				miss++;
				read++;
				m=0;
				
				for(j=0;j<assoc;j++){
					if(cache[setindex][j].time<=cache[setindex][m].time){
						m=j;
					}	
				}
			cache[setindex][m].valid=1;
	        	cache[setindex][m].tag=tagindex;
	        	push++;
	        	cache[setindex][m].time=push;
	       		return;
			}
		}
	}
return;
}
	

//write
void writecache(unsigned long int tagindex,unsigned long int setindex,int assoc){
  int i,j,m;

	for(i=0;i<assoc;i++){
		if(cache[setindex][i].valid==0){
			miss++;
			read++;
			write++;
			push++;
			cache[setindex][i].valid=1;
			cache[setindex][i].tag=tagindex;
			cache[setindex][i].time=push;	
			return;
		}

		else{
			if(cache[setindex][i].tag==tagindex){
				hit++;
				write++;
				return;
			}
				
			if(i==(assoc-1)){
				miss++;
				read++;
				write++;
				m=0;
				
				for(j=0;j<assoc;j++){
					if(cache[setindex][j].time<=cache[setindex][m].time){
						m=j;
					}	
				}
				cache[setindex][m].valid=1;
	        		cache[setindex][m].tag=tagindex;
	      			push++;
	       			cache[setindex][m].time=push;
	        		return;
			}
		}
	}
return;
}


line** createcache(int setnum,int assoc){
  int i;
  int j;

  cache=(line**)malloc(setnum*sizeof(line*));
  
	for(i=0;i<setnum;i++){
    		cache[i]=(line*)malloc((assoc)*sizeof(line));
 	 }

 	 for(i=0;i<setnum;i++){
   	 	for(j=0;j<assoc;j++){
         		cache[i][j].valid=0;
    		}
 	 }
return cache;
}




