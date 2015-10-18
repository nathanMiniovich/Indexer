#ifndef INDEXER_H
#define INDEXER_H

struct fileNode_{
	char* path;
	struct fileNode_* next;
};

typedef struct fileNode_ fileNode;

int gWordCount; //global word count
int gFileCount; //global file count
int fWordCount; //word count for a specific file
fileNode* fileRoot; //file path linked list root
fileNode* fileTemp; //for traversing the file path linked list

int dircheck(char* filepath);
int fileIt(char* path, int fileindex);
void directIt(char* filepath);
fileNode* buildFileNode(char* path);
void buildFileLL(char* path);
void addFileLL();
void writeCounts(char* writepath);
int trieDFS(trieNode* scout,char* buffer,int* index, FILE* target);
int countNodeCompare(const void* a, const void* b);
void mergeSort(countNode** countarr, int lo, int hi);
void merge(countNode** countarr, int lo, int mid, int hi);
void writeBot(trieNode* scout, char* buffer, FILE* target);

#endif
