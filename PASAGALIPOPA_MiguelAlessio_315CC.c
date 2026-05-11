#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct FileNode{
    char id[50];
    int scor;
    struct FileNode *next;
    struct FileNode *prev;
}FileNode;

typedef struct FileRefNode{
    FileNode *file;
    struct FileRefNode *next;
}FileRefNode;

#define ALPHABET_SIZE 26

typedef struct TrieNode{
    bool end_of_word;
    struct TrieNode *children[ALPHABET_SIZE];
    FileRefNode *file_refs;
}TrieNode;

FileNode* create_file_node(const char* nume_id, int scor_initial){
    FileNode* new_node=(FileNode*)malloc(sizeof(FileNode));
    if(new_node==NULL){
        printf("Error");
        return NULL;
    }
    strncpy(new_node->id,nume_id,49);
    new_node->id[49]='\0';
    new_node->scor=scor_initial;
    new_node->prev=NULL;
    new_node->next=NULL;
    return new_node;
}

FileRefNode* create_file_ref_node(FileNode* fisier_tinta) {
    FileRefNode* new_node = (FileRefNode*)malloc(sizeof(FileRefNode));
    if (new_node == NULL) {
        printf("Error");
        return NULL;
    }
    new_node->file = fisier_tinta;
    new_node->next = NULL;
    return new_node;
}

TrieNode* create_trie_node(){
    TrieNode* new_node=(TrieNode*)malloc(sizeof(TrieNode));
    if(new_node==NULL){
        printf("Error");
        return NULL;
    }
    new_node->end_of_word=false;
    new_node->file_refs=NULL;
    for(int i=0;i<ALPHABET_SIZE;i++){
        new_node->children[i]=NULL;
    }
    return new_node;
}

// Funcție care inserează UN cuvânt în Trie și îl leagă de fisier
void insert_in_trie(TrieNode* root, const char* word, FileNode* file_ptr) {
    TrieNode* curr = root; // Începem de la rădăcină
    
    // Parcurgem literele cuvântului una câte una
    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a'; // Transformăm litera ('a'-'z') în index (0-25)
        
        // Dacă nodul pentru această literă nu există, îl creăm
        if (curr->children[index] == NULL) {
            curr->children[index] = create_trie_node();
        }
        
        // Coborâm pe ramura respectivă
        curr = curr->children[index];
    }
    
    // Am ajuns la finalul cuvântului
    curr->end_of_word = true;
    
    // Creăm "puntea" de legătură
    FileRefNode* new_ref = create_file_ref_node(file_ptr);
    
    // Inserăm referința la începutul listei de referințe a acestui nod Trie (cel mai simplu mod - O(1))
    new_ref->next = curr->file_refs;
    curr->file_refs = new_ref;
}

void ADD(FileNode *new_file,const char* id,int scor_initial){
    if(new_file->id){
        printf("Error");
        return;
    }
    new_file=create_file_node(id,scor_initial);
    create_file_ref_node(new_file);


}