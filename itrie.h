#ifndef ITRIE_H
#define ITRIE_H

struct countNode_ {
	int count;
	char* filepath;
};

typedef struct countNode_ countNode;

struct trieNode_ {
	char data;
	int count;
	int isWord;
	int level;
	struct trieNode_* next[36];
	struct trieNode_* parent;
	countNode** countarr; //word counts for files

};

typedef struct trieNode_ trieNode;

trieNode* root; //root of trie
int maxWordLength;

int add(char toadd,trieNode* parent);
void insert(trieNode* toinsert,trieNode* parent);
void deleteOne(trieNode* todelete);
void deleteTrie(trieNode* parent);
trieNode* buildNode(char data,int isWord,int level);
countNode* buildCountNode(char* path);
int buildTrie(char* input, size_t size, int filecount, int fileindex, char* path);
void printTrie(trieNode* parent);
int traverseTrie(trieNode* parent);

#endif
