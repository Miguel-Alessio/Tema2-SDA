#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct FileNode{
    char id[50];
    int scor;
    struct FileNode *next;
    struct FileNode *prev;
    int nr_cuvinte;
    char cuvinte_cheie[20][50];
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
    new_file->nr_cuvinte = numar_cuvinte;
    for(int i = 0; i < numar_cuvinte; i++){
    strcpy(new_file->cuvinte_cheie[i], cuvinte_cheie[i]);
}
    printf("Fisierul %s a fost adaugat cu succes!\n",id);

}

bool has_no_children(TrieNode* node){
    for(int i=0;i<ALPHABET_SIZE;i++){
        if(node->children[i]){
            return false;
        }
    }
    return true;
}

bool remove_from_trie(TrieNode* curr,const char* word, int depth, FileNode* target_file){
    if(curr==NULL)return false;

    if(depth==strlen(word)){
        FileRefNode* prev_ref=NULL;
        FileRefNode* curr_ref=curr->file_refs;

        while(curr_ref){
            if(curr_ref->file==target_file){
                if(prev_ref==NULL)curr->file_refs=curr_ref->next;
                else prev_ref->next=curr_ref->next;

                free(curr_ref);
                break;
            }
            prev_ref=curr_ref;
            curr_ref=curr_ref->next;
        }
        if(curr->file_refs==NULL){
            curr->end_of_word=false;
        }
        return (curr->file_refs==NULL && has_no_children(curr));
        
    }
    int index=word[depth]-'a';
    bool should_delete_child=remove_from_trie(curr->children[index],word,depth+1,target_file);
    if(should_delete_child){
        free(curr->children[index]);
        curr->children[index]=NULL;
    }
    if(curr->end_of_word==false && has_no_children(curr)==true)return true;
    else return false;
}
void DEL(FileNode **head,FileNode **tail, TrieNode* trie_root,const char* id_de_sters){
    FileNode *curr=*head;
    while((curr!=NULL) && (strcmp(curr->id,id_de_sters)!=0)){
        curr=curr->next;
    }
    if(curr==NULL){
        printf("Error, no such id\n");
        return;
    }
    for(int i=0;i<curr->nr_cuvinte;i++){
        remove_from_trie(trie_root,curr->cuvinte_cheie[i],0,curr);
    }
    if(curr->prev){
        curr->prev->next=curr->next;
    }
    else{
        *head=curr->next;
    }
    if(curr->next){
        curr->next->prev=curr->prev;
    }
    else{
        *tail=curr->prev;
    }
    free(curr);
    printf("Success");

}

void ADDKW(const char *word,FileNode *head, TrieNode *trie_root,const char *id){
    FileNode *curr=head;
    while(curr!=NULL && strcmp(curr->id,id)!=0){
        curr=curr->next;
    }
    if(curr==NULL){
        printf("Error, nu such file");
        return;
    }
    for(int i=0;i<curr->nr_cuvinte;i++){
        if(strcmp(curr->cuvinte_cheie[i],word)==0){
            return;
        }
    }
    if(curr->nr_cuvinte<20){
        strcpy(curr->cuvinte_cheie[curr->nr_cuvinte],word);
        (curr->nr_cuvinte)++;
        insert_in_trie(trie_root,word,curr);
    }
    else{
        printf("Too much keywords in this file\n");
        return;
    }
}

void DELKW(const char *word,FileNode *head,TrieNode *trie_root, const char *id_de_gasit,FileNode *tail){
    FileNode *curr=head;
    while(curr && (strcmp(curr->id,id_de_gasit)!=0)){
        curr=curr->next;
    }
    if(!curr){
        printf("No such id");
        return;
    }
    
    for(int i=0;i<curr->nr_cuvinte;i++){
        if(strcmp(curr->cuvinte_cheie[i],word)==0){
            remove_from_trie(trie_root,word,0,curr);
            for(int j=i;j<curr->nr_cuvinte-1;j++){
                strcpy(curr->cuvinte_cheie[j],curr->cuvinte_cheie[j+1]);
            }
            (curr->nr_cuvinte)--;
            if (curr->nr_cuvinte == 0) {
            DEL(head, tail, trie_root, curr->id);
    } 
            return;
        }
    }
      
}

void FIND(const char *word,FileNode *head,TrieNode *trie_root){
    TrieNode *curr=trie_root;
    for(int i=0;i<strlen(word);i++){
        int index=word[i]-'a';
        if(curr->children[index]==NULL){
            printf("No such keyword");
            return;
        }
        curr=curr->children[index];
    }
    if (curr->end_of_word && curr->file_refs!=NULL) {
    char nume_fisiere[100][50];
    int count = 0;
    FileRefNode *ref_curr = curr->file_refs;
    while (ref_curr != NULL) {
        strcpy(nume_fisiere[count], ref_curr->file->id);
        count++;
        ref_curr = ref_curr->next;
    }
    for(int i=0;i<count-1;i++){
        for(int j=i+1;j<count;j++){
            if(strcmp(nume_fisiere[i],nume_fisiere[j])>0){
                char tmp[50];
                strcpy(tmp,nume_fisiere[j]);
                strcpy(nume_fisiere[j],nume_fisiere[i]);
                strcpy(nume_fisiere[i],tmp);
            }
        }
    }

    for(int i=0;i<count;i++){
        printf("%s ",nume_fisiere[i]);
    }
    }
}