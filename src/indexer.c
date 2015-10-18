#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "itrie.h"
#include "indexer.h"

int main(int argc,char** argv){
	char* writepath;
	char* filepath;

	//check if there are fewer than two arguments, and if there are then print and error and return
	if(argc < 3){
		fprintf(stderr, "invalid input\n");
		return 1;
	}

	//save paths to the file to be written to and the file/directory data is being taken from
	writepath = argv[1];
	filepath = argv[2];

	//check if the file the user wants to write to already exists and ask if they want to overwrite it
	if(access(writepath,F_OK) != -1){

		char userinput[100];
		int flag = 0;

		while(flag == 0){
		
			printf("The file you with to write to already exists, do you wish to overwrite it? (Press 1 for yes 2 for no):\n");
			fgets(userinput,99,stdin);

			if(sscanf(userinput,"%d",&flag) != 1){
				flag = 0;
				printf("Invalid sscanf() try again:\n");
				continue;
			}

			if(flag < 1 || flag > 2){
				flag = 0;
				printf("Please pick either 1 or 2, you number is outside of that range, try again:\n");
			}

			if(flag == 2){
				printf("You chose not to overwrite. Exiting program.");
				return 1;
			} 
	
		}
	}

	gWordCount = 0; //set global word count
	gFileCount = 0; //set global file count
	maxWordLength = 0; //set maximum word length
	root = buildNode('$',-1,-1); //'$' serves no purpose other than having a value to condition on to detect the root

	//directory case
	if(dircheck(filepath)){

		fileRoot = buildFileNode("$"); //declare and initialize file linked list root
		fileTemp = fileRoot;
		directIt(filepath);
		addFileLL();
		writeCounts(writepath);
	}
	
	//file case
	else{
		gFileCount++;
		fileIt(filepath,0); //filecount passed as one and file index passed as 0 since there is only one file
		writeCounts(writepath);
		
	}

	deleteTrie(root);

	return 0;
}

//checks if a file is a directory. If it is it returns 1 otherwise it returns 0.
int dircheck(char* filepath){
	struct stat buf;
	
	stat(filepath, &buf);

	return S_ISDIR(buf.st_mode);
}

//single file case. called multiple times by the directory case
int fileIt(char* path, int fileindex){
	int fsize = 0; 
	char* fileBuffer; 
	char c = '0';
	int i = 0;

	FILE* tobefiled = fopen(path, "r");

	if(tobefiled == NULL){
			fprintf(stderr, "can't open file\n");
			return 0;
	}

	fseek(tobefiled,0L,SEEK_END);
	fsize = ftell(tobefiled); 
	fseek(tobefiled,0L,SEEK_SET);
	fileBuffer = (char*)calloc(fsize + 1,sizeof(char));

	while((c = fgetc(tobefiled)) != EOF){
		fileBuffer[i] = c;
		i++;
	}

	fclose(tobefiled);

	buildTrie(fileBuffer,fsize,gFileCount,fileindex,path);

	if(fWordCount == 0){
		printf("'%s' contains no word tokens\n", path);
		return 0;
	}

	fWordCount = 0;
	free(fileBuffer);
	return 1;
}

//directory case. Calls itself if it finds a directory and calls a function to generate a linked list of file paths.
void directIt(char* filepath){	
	
	DIR* dir;
	struct dirent* ent;

		if ((dir = opendir(filepath)) != NULL) {
			
			//make sure every file and subdirectory is hit
			while ((ent = readdir(dir)) != NULL){
				
				//check for "." and ".." directories so the program doesnt enter an endless loop
				if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
					continue;
				}
				
				//find the length of the path and file name
				int plength = strlen(filepath);
				int flength = strlen(ent->d_name);
				
				//add path and file name together and copy them into a buffer so fileIt method works properly
				char buffer[plength+flength+1];
				memcpy(buffer, filepath, plength);
				
				//chck if the last character is a '/' and if it is then proceed normally
				if (filepath[plength-1] == '/') {
					memcpy(buffer+plength, ent->d_name, flength+1);
				} 

				//add a '/' if it is missing
				else {
					buffer[plength] = '/';
					memcpy(buffer+plength+1, ent->d_name, flength+1);
				}

				//if the what is found is a file then build a file node and attach it to the file path linked list. Also increment the global file count
				if (!dircheck(buffer)){
					buildFileLL(buffer);
					gFileCount++;
				} 

				//subdirectory found. process it through a recursive call
				else {
					directIt(buffer);
				}
			}
			
			closedir(dir);
		} 

		//case where directory cannot be opened
		else {
			fprintf(stderr, "couldn't open directory\n");
			return;
		}

	return;
}

//builds a node in the file path linked list
fileNode* buildFileNode(char* path){
	fileNode* node = (fileNode*)malloc(1*sizeof(fileNode));
	node->path = (char*)malloc(1*(strlen(path)+1));
	memcpy(node->path,path,strlen(path));
	node->path[strlen(path)] = '\0';
	node->next = NULL;

	return node;
}

//builds the file linked list
void buildFileLL(char* path){

	fileTemp->next = buildFileNode(path);
	fileTemp = fileTemp->next;

	return;

}

//adds the file linked list to the trie
void addFileLL(){
	
	int index = 0;
	
	//traverse the linked list and increment to index so that the file count can properly be hashed to the correct spot in a count array in the trie. Free during
	while(fileRoot != NULL){
		fileTemp = fileRoot->next;
		
		if(fileTemp != NULL){
			fileIt(fileTemp->path,index);
			index++;
		}

		free(fileRoot->path);
		free(fileRoot);
		fileRoot = fileTemp;
	}

	return;
}

//merge sorts the count arrays in the trie so that output prints correctly and prints the output in JSON format 
void writeCounts(char* writepath){
	FILE* target;
	trieNode* scout;
	char* buffer;
	int index;

	if(gWordCount == 0){
		printf("%s", "There were no tokens to be printed");
		return;
	}

	//prepare to write to file
	target = fopen(writepath, "w");
	scout = root;
	index = 0;

	buffer = (char*)calloc(1,maxWordLength+1);
	buffer[maxWordLength] = '\0';

	fprintf(target, "%s\n", "{\"list\" : [");

	trieDFS(scout,buffer,&index,target);

	fprintf(target,"%s", "]}");

	fclose(target);

	free(buffer);

	return;
}

//depth first search of the trie
int trieDFS(trieNode* scout,char* buffer,int* index, FILE* target){
	int i;

	i = 0;

	if(scout->level > -1){
		buffer[scout->level] = scout->data;
	}

	if(scout->isWord > -1){
		buffer[(scout->level)+1] = '\0';

		//terrible runtime but needed to work. Fix implementation later
		for(int x=0;x<gFileCount;x++){
			if(scout->countarr[x] == NULL){
				scout->countarr[x] = buildCountNode("$");
				scout->countarr[x]->count = 0;
			}
		}

		mergeSort(scout->countarr,0,gFileCount-1);
		writeBot(scout,buffer,target);

		scout->isWord = *index;

		(*index)++;
	}

	for(i = 0; i < 36; i++){
		if(scout == NULL){
			continue;
		}

		else if(scout->next != NULL){
			if(scout->next[i] != NULL){
				trieDFS(scout->next[i],buffer,index,target);
			}
			else{
				continue;
			}
		}
	}


	return 0;

}

//compare function for qsort to compare count value of two count nodes, currecntly not implemented. qsort chunks before merging to optimize to industry standard
int countNodeCompare(const void* a, const void* b){

	const int ca = ((countNode*)a)->count;
	const int cb = ((countNode*)b)->count;

	if(ca != cb){
		return cb - ca;
	}

	else{
		return strcmp(((countNode*)a)->filepath,((countNode*)b)->filepath);
	}
}

//what do you think it does?
void mergeSort(countNode** countarr, int lo, int hi){

	int mid;

	if(lo < hi){
		mid = ((lo+hi)/2);
		mergeSort(countarr,lo,mid);
		mergeSort(countarr,mid+1,hi);
		merge(countarr,lo,mid,hi);
	}

	return;
}

void merge(countNode** countarr, int lo, int mid, int hi){
	
	int i,m,k,l;
	countNode* temp[gFileCount-1];

	l = lo;
	i = lo;
	m = mid + 1;
	
	while((l<=mid) && (m<=hi)){

		if(countarr[l]->count > countarr[m]->count){
			temp[i] = countarr[l];
			l++;
		}

		else if(countarr[l]->count == countarr[m]->count){
			int check = strcmp(countarr[l]->filepath,countarr[m]->filepath);

			if(check <= 0){
				temp[i] = countarr[l];
				l++;
			}
		}

		else{
			temp[i] = countarr[m];
			m++;
		}
		i++;

	}

	if(l>mid){
		for(k=m;k<=hi;k++){
			temp[i] = countarr[k];
			i++;
		}
	}

	else{
		for(k=l;k<=mid;k++){
			temp[i] = countarr[k];
			i++;
		}
	}

	for(k=lo;k<=hi;k++){
		countarr[k] = temp[k];
	}

}

//writes to the file when a word is found in the trie
void writeBot(trieNode* scout, char* buffer, FILE* target){

	fprintf(target, "%s %s %s\n", "\t{\"" , buffer , "\" : [");

	int i;

	for(i = 0; i < gFileCount; i++){

		//in case the word count for a particular word is zero for one file but not for another
		if(scout->countarr[i]->count == 0){
			continue;
		}

		//print a comma after since it isn't the last entry
		if(i < gFileCount - 1){
			fprintf(target, "%s %s %s %d %s\n", "\t\t{\"", scout->countarr[i]->filepath, "\" : ", scout->countarr[i]->count, "},");
		}

		//no comma since it's the last entry
		else{
			fprintf(target, "%s %s %s %d %s\n", "\t\t{\"", scout->countarr[i]->filepath, "\" : ", scout->countarr[i]->count, "}");
		}

	}

	fprintf(target, "%s\n", "\t]},");

	return;
}

