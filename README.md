# Indexer

Important Notes on this implementation:
	-This implementation's definition of proper alphanumeric order is that it treat numbers 0-9 as an extension of the alphabet. So 0 > Z and the range contains 36 characters
	-When two files have the same count for a word this implementation compares the entire file path for the files relative to the place from which the program was started as opposed to just its local name
	
Trie Data Structure: The Trie data structure is made up of struct trieNode*’s that have the parameters

char data; The relevant character stored in the array from the input file

int count; The number of times a character occurs in the input file.

int isWord; This is used as a boolean as well as a way to index into the proper spot in the word array (not the same as the next array of the struct). When build node is called this value is initialized to -1 to show false, and anything greater than -1 is true. When a character needs to be marked as the end of a word its value is changed to 0. In the trieDFS method where the word array is being built (the word array is used to print the word part of the result in alphabetical order) the isWord is changed to the index of that word in the word array.

int level; This shows the level of the data structure where the current array sits. The root starts at -1 and the level goes up by one with each next array, so the first actual character in a chain would be stored at level zero. This is also used to update the correct index of the file buffer with a new character when dumping the file buffer into the word array.

struct trieNode_* next[36]; Initialized to size 36 in order to be able to store all alphabetical characters a-z and numbers 0-9. A character subtraction is used to calculate the index of insertion in the array in order to give an insertion time of O(1). Each node has its own sub array of 36.

struct trieNode_* parent; Makes reference to the parent of a node in the array. Not used in this program

countNode** countarr; This is an array that contains node structs which are made up of an int count and char* file path. The countarr is initially set to null but when a word is found the countarr at that spot in the trie is allocated to the size of the global file count. When a count for a particular file needs to be added that file is hashed to the proper spot in the count array and then a count node for it is either created or if the node already exists then the count for that word in that file is incremented.

----------------------------------------------------------------------------------------------------------------------------

The data value of the root of the trie is set to ‘$’ so that it can be conditioned on. This is acceptable because a nonalphanumeric character will never be added to the array. The isWord value of the root is set to -1 as well as the count since the count will never be incremented. Anything in the subarray of a node containing a valid alphanumeric character is the next letter in a chain which will end in a word, with the last character in a word (but not necessarily the whole chain) having its isWord value sent to 0. For example you can have the chain c-a-t-e-r-s with “cat” being marked as a word (t->isWord = 0) and “caters” being marked as a word (s->isWord = 0). All characters are set to lowercase before insertion into the Trie. The tree can be searched and printed out using a modified DFS.

If a directory is given as input a linked list of file nodes (containing file paths) is made. Those are then passed one by one to the filing function.

Before printing the count arrays in the tries are mergesorted in descending order.
