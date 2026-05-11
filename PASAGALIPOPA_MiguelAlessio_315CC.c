#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct FileNode{
    char id[50];
    int scor;
    struct FileNode *next;
    struct FileNode *prev;
    int nr_cuvinte;
    char cuvinte_cheie[10][50];
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
        printf("Error\n");
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
        printf("Error\n");
        return NULL;
    }
    new_node->file = fisier_tinta;
    new_node->next = NULL;
    return new_node;
}

TrieNode* create_trie_node(){
    TrieNode* new_node=(TrieNode*)malloc(sizeof(TrieNode));
    if(new_node==NULL){
        printf("Error\n");
        return NULL;
    }
    new_node->end_of_word=false;
    new_node->file_refs=NULL;
    for(int i=0;i<ALPHABET_SIZE;i++){
        new_node->children[i]=NULL;
    }
    return new_node;
}

void insert_in_trie(TrieNode* root, const char* word, FileNode* file_ptr) {
    TrieNode* curr = root;
    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a';
        if (curr->children[index] == NULL) {
            curr->children[index] = create_trie_node();
        }
        curr = curr->children[index];
    }
    curr->end_of_word = true;
    FileRefNode* new_ref = create_file_ref_node(file_ptr);
    new_ref->next = curr->file_refs;
    curr->file_refs = new_ref;
}

void ADD(FileNode **head,FileNode **tail,TrieNode* trie_root,const char* id,int scor_initial,char cuvinte_cheie[][50],int numar_cuvinte){
    FileNode *curr=*head;
    while(curr){
        if(strcmp(curr->id,id)==0){
            printf("Error:Fisierul %s deja exista.\n",id);
            return;
        }
        curr=curr->next;
    }
    FileNode *new_file=create_file_node(id,scor_initial);

    if(*head==NULL){
        *head=new_file;
        *tail=new_file;
    }
    else{
        (*tail)->next=new_file;
        new_file->prev=*tail;
        *tail=new_file;
    }
    for(int i=0;i<numar_cuvinte;i++){
        insert_in_trie(trie_root,cuvinte_cheie[i],new_file);

    }
    printf("Fisierul %s a fost adaugat cu succes!\n",id);

}

FileNode del()