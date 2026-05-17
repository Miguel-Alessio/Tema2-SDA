#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ALPHABET_SIZE 26

typedef struct FileNode {
    char id[105];
    int score;
    struct FileNode *next;
    struct FileNode *prev;
    int nr_cuvinte;
    char **cuvinte_cheie;
} FileNode;

typedef struct FileRefNode {
    FileNode *file;
    struct FileRefNode *next;
} FileRefNode;

typedef struct TrieNode {
    bool end_of_word;
    struct TrieNode *children[ALPHABET_SIZE];
    FileRefNode *file_refs;
} TrieNode;

typedef struct {
    FileNode **files;
    int size;
} Heap;

typedef struct {
    const char **names;
    int size;
    int capacity;
} FileArray;

int cmp_str(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void DEL(FileNode **head, FileNode **tail, TrieNode* trie_root, const char* id_de_sters);

FileNode* create_file_node(const char* nume_id, int score_initial) {
    FileNode* new_node = (FileNode*)malloc(sizeof(FileNode));
    if (new_node == NULL) return NULL;
    strncpy(new_node->id, nume_id, 104);
    new_node->id[104] = '\0';
    new_node->score = score_initial;
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->nr_cuvinte = 0;
    new_node->cuvinte_cheie = NULL;
    return new_node;
}

FileRefNode* create_file_ref_node(FileNode* fisier_tinta) {
    FileRefNode* new_node = (FileRefNode*)malloc(sizeof(FileRefNode));
    if (new_node == NULL) return NULL;
    new_node->file = fisier_tinta;
    new_node->next = NULL;
    return new_node;
}

TrieNode* create_trie_node() {
    TrieNode* new_node = (TrieNode*)malloc(sizeof(TrieNode));
    if (new_node == NULL) return NULL;
    new_node->end_of_word = false;
    new_node->file_refs = NULL;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        new_node->children[i] = NULL;
    }
    return new_node;
}

void insert_in_trie(TrieNode* root, const char* word, FileNode* file_ptr) {
    TrieNode* curr = root;
    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a';
        if (index < 0 || index >= ALPHABET_SIZE) continue;
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

void ADD(FileNode **head, FileNode **tail, TrieNode* trie_root, const char* id, int score_initial, char (*cuvinte_cheie)[105], int numar_cuvinte) {
    FileNode *curr = *head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            printf("EXISTS\n");
            return;
        }
        curr = curr->next;
    }
    FileNode *new_file = create_file_node(id, score_initial);
    if (*head == NULL) {
        *head = new_file;
        *tail = new_file;
    } else {
        (*tail)->next = new_file;
        new_file->prev = *tail;
        *tail = new_file;
    }
    new_file->nr_cuvinte = 0;
    new_file->cuvinte_cheie = malloc(numar_cuvinte * sizeof(char*));
    for (int i = 0; i < numar_cuvinte; i++) {
        bool dup = false;
        for (int j = 0; j < new_file->nr_cuvinte; j++) {
            if (strcmp(new_file->cuvinte_cheie[j], cuvinte_cheie[i]) == 0) {
                dup = true;
                break;
            }
        }
        if (!dup) {
            new_file->cuvinte_cheie[new_file->nr_cuvinte] = malloc(strlen(cuvinte_cheie[i]) + 1);
            strcpy(new_file->cuvinte_cheie[new_file->nr_cuvinte], cuvinte_cheie[i]);
            insert_in_trie(trie_root, cuvinte_cheie[i], new_file);
            new_file->nr_cuvinte++;
        }
    }
    printf("OK\n");
}

bool has_no_children(TrieNode* node) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) return false;
    }
    return true;
}

bool remove_from_trie(TrieNode* curr, const char* word, int depth, FileNode* target_file) {
    if (curr == NULL) return false;
    if ((size_t)depth == strlen(word)) {
        FileRefNode* prev_ref = NULL;
        FileRefNode* curr_ref = curr->file_refs;
        while (curr_ref) {
            if (curr_ref->file == target_file) {
                if (prev_ref == NULL) curr->file_refs = curr_ref->next;
                else prev_ref->next = curr_ref->next;
                free(curr_ref);
                break;
            }
            prev_ref = curr_ref;
            curr_ref = curr_ref->next;
        }
        if (curr->file_refs == NULL) curr->end_of_word = false;
        return (curr->file_refs == NULL && has_no_children(curr));
    }
    int index = word[depth] - 'a';
    if (index < 0 || index >= ALPHABET_SIZE) return false;
    
    if (remove_from_trie(curr->children[index], word, depth + 1, target_file)) {
        free(curr->children[index]);
        curr->children[index] = NULL;
    }
    return (curr->end_of_word == false && has_no_children(curr));
}

void DEL(FileNode **head, FileNode **tail, TrieNode* trie_root, const char* id_de_sters) {
    FileNode *curr = *head;
    while (curr && strcmp(curr->id, id_de_sters) != 0) {
        curr = curr->next;
    }
    if (curr == NULL) {
        printf("NOT FOUND\n");
        return;
    }
    for (int i = 0; i < curr->nr_cuvinte; i++) {
        remove_from_trie(trie_root, curr->cuvinte_cheie[i], 0, curr);
        free(curr->cuvinte_cheie[i]);
    }
    free(curr->cuvinte_cheie);
    curr->cuvinte_cheie = NULL;
    
    if (curr->prev) curr->prev->next = curr->next;
    else *head = curr->next;
    if (curr->next) curr->next->prev = curr->prev;
    else *tail = curr->prev;
    free(curr);
    printf("OK\n");
}

void ADDKW(const char *word, FileNode *head, TrieNode *trie_root, const char *id) {
    FileNode *curr = head;
    while (curr != NULL && strcmp(curr->id, id) != 0) curr = curr->next;
    if (curr == NULL) {
        printf("NOT FOUND\n");
        return;
    }
    for (int i = 0; i < curr->nr_cuvinte; i++) {
        if (strcmp(curr->cuvinte_cheie[i], word) == 0) {
            printf("OK\n");
            return;
        }
    }
    curr->cuvinte_cheie = realloc(curr->cuvinte_cheie, (curr->nr_cuvinte + 1) * sizeof(char*));
    curr->cuvinte_cheie[curr->nr_cuvinte] = malloc(strlen(word) + 1);
    strcpy(curr->cuvinte_cheie[curr->nr_cuvinte], word);
    curr->nr_cuvinte++;
    insert_in_trie(trie_root, word, curr);
    printf("OK\n");
}

void DELKW(const char *word, FileNode **head, FileNode **tail, TrieNode *trie_root, const char *id_de_gasit) {
    FileNode *curr = *head;
    while (curr && strcmp(curr->id, id_de_gasit) != 0) curr = curr->next;
    if (!curr) {
        printf("NOT FOUND\n");
        return;
    }
    for (int i = 0; i < curr->nr_cuvinte; i++) {
        if (strcmp(curr->cuvinte_cheie[i], word) == 0) {
            remove_from_trie(trie_root, word, 0, curr);
            free(curr->cuvinte_cheie[i]);
            for (int j = i; j < curr->nr_cuvinte - 1; j++) {
                curr->cuvinte_cheie[j] = curr->cuvinte_cheie[j + 1];
            }
            curr->nr_cuvinte--;
            if (curr->nr_cuvinte == 0) {
                free(curr->cuvinte_cheie);
                curr->cuvinte_cheie = NULL;
                char tmp_id[105];
                strcpy(tmp_id, curr->id);
                DEL(head, tail, trie_root, tmp_id);
            } else {
                curr->cuvinte_cheie = realloc(curr->cuvinte_cheie, curr->nr_cuvinte * sizeof(char*));
                printf("OK\n");
            }
            return;
        }
    }
    printf("OK\n");
}

void FIND(const char *word, TrieNode *trie_root) {
    TrieNode *curr = trie_root;
    for (size_t i = 0; i < strlen(word); i++) {
        int index = word[i] - 'a';
        if (index < 0 || index >= ALPHABET_SIZE || curr->children[index] == NULL) {
            printf("EMPTY\n");
            return;
        }
        curr = curr->children[index];
    }
    if (curr->end_of_word && curr->file_refs != NULL) {
        int count = 0;
        FileRefNode *ref_curr = curr->file_refs;
        while (ref_curr) { count++; ref_curr = ref_curr->next; }
        
        const char **nume_files = malloc(count * sizeof(const char*));
        ref_curr = curr->file_refs;
        int i = 0;
        while (ref_curr) { nume_files[i++] = ref_curr->file->id; ref_curr = ref_curr->next; }
        
        qsort(nume_files, count, sizeof(const char*), cmp_str);
        printf("%d", count);
        for (int j = 0; j < count; j++) {
            printf(" %s", nume_files[j]);
        }
        printf("\n");
        free(nume_files);
    } else {
        printf("EMPTY\n");
    }
}

void insertHeap(Heap *heap, FileNode *new_node) {
    heap->files[heap->size] = new_node;
    int i = heap->size;
    heap->size++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap->files[i]->score > heap->files[parent]->score || 
           (heap->files[i]->score == heap->files[parent]->score && strcmp(heap->files[i]->id, heap->files[parent]->id) < 0)) {
            FileNode *temp = heap->files[parent];
            heap->files[parent] = heap->files[i];
            heap->files[i] = temp;
            i = parent;
        } else {
            break;
        }
    }
}

void heapifyDown(Heap *heap, int i) {
    int max = i;
    int left = (2 * i) + 1;
    int right = (2 * i) + 2;
    if (left < heap->size) {
        if (heap->files[left]->score > heap->files[max]->score ||
           (heap->files[left]->score == heap->files[max]->score && 
            strcmp(heap->files[left]->id, heap->files[max]->id) < 0)) {
            max = left;
        }
    }
    if (right < heap->size) {
        if (heap->files[right]->score > heap->files[max]->score ||
           (heap->files[right]->score == heap->files[max]->score && 
            strcmp(heap->files[right]->id, heap->files[max]->id) < 0)) {
            max = right;
        }
    }
    if (max != i) {
        FileNode *temp = heap->files[i];
        heap->files[i] = heap->files[max];
        heap->files[max] = temp;
        heapifyDown(heap, max);
    }
}

void TOPK(const char *word, int k, TrieNode *trie_root) {
    TrieNode *curr = trie_root;
    for (size_t i = 0; i < strlen(word); i++) {
        int index = word[i] - 'a';
        if (index < 0 || index >= ALPHABET_SIZE || curr->children[index] == NULL) {
            printf("EMPTY\n");
            return;
        }
        curr = curr->children[index];
    }
    if (!curr->end_of_word || curr->file_refs == NULL) {
        printf("EMPTY\n");
        return;
    }
    
    int count = 0;
    FileRefNode *ref = curr->file_refs;
    while (ref) { count++; ref = ref->next; }
    
    Heap h;
    h.size = 0;
    h.files = (FileNode**)malloc(count * sizeof(FileNode*));
    ref = curr->file_refs;
    while (ref) {
        insertHeap(&h, ref->file);
        ref = ref->next;
    }
    
    int to_extract = (k < h.size) ? k : h.size;
    printf("%d", to_extract);
    for (int i = 0; i < to_extract; i++) {
        printf(" %s", h.files[0]->id);
        h.files[0] = h.files[h.size - 1];
        h.size--;
        heapifyDown(&h, 0);
    }
    printf("\n");
    free(h.files);
}

void sortAndPrintRefs(FileRefNode *head) {
    if (head == NULL) return;
    int count = 0;
    FileRefNode *curr = head;
    while (curr) { count++; curr = curr->next; }
    
    const char **nume_files = malloc(count * sizeof(const char*));
    curr = head;
    int i = 0;
    while (curr) { nume_files[i++] = curr->file->id; curr = curr->next; }
    
    qsort(nume_files, count, sizeof(const char*), cmp_str);
    printf("%d", count);
    for (int j = 0; j < count; j++) {
        printf(" %s", nume_files[j]);
    }
    printf("\n");
    free(nume_files);
}

void printRecursive(TrieNode *node, char *buffer, int depth) {
    if (node == NULL) return;
    if (node->end_of_word && node->file_refs != NULL) {
        buffer[depth] = '\0'; 
        printf("%s ", buffer);
        sortAndPrintRefs(node->file_refs);
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            buffer[depth] = i + 'a';
            printRecursive(node->children[i], buffer, depth + 1);
        }
    }
}

bool hasWords(TrieNode *node) {
    if (!node) return false;
    if (node->end_of_word && node->file_refs != NULL) return true;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] && hasWords(node->children[i])) {
            return true;
        }
    }
    return false;
}

void PRINT(TrieNode *root) {
    if (root == NULL || !hasWords(root)) {
        printf("EMPTY\n");
        return;
    }
    char buffer[105];
    printRecursive(root, buffer, 0);
}

void add_unique_file(FileArray *arr, const char *id) {
    if (arr->size == arr->capacity) {
        arr->capacity = arr->capacity == 0 ? 32 : arr->capacity * 2;
        arr->names = realloc(arr->names, arr->capacity * sizeof(const char*));
    }
    arr->names[arr->size++] = id;
}

void gather_files(TrieNode *node, FileArray *arr) {
    if (!node) return;
    if (node->end_of_word && node->file_refs) {
        FileRefNode *ref = node->file_refs;
        while (ref) {
            add_unique_file(arr, ref->file->id);
            ref = ref->next;
        }
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        gather_files(node->children[i], arr);
    }
}

void PREFIX(const char *pref, TrieNode *root) {
    TrieNode *curr = root;
    for (size_t i = 0; i < strlen(pref); i++) {
        int index = pref[i] - 'a';
        if (index < 0 || index >= ALPHABET_SIZE || curr->children[index] == NULL) {
            printf("EMPTY\n");
            return;
        }
        curr = curr->children[index];
    }
    
    FileArray arr = {NULL, 0, 0};
    gather_files(curr, &arr);
    
    if (arr.size == 0) {
        printf("EMPTY\n");
        free(arr.names);
        return;
    }
    
    qsort(arr.names, arr.size, sizeof(const char*), cmp_str);
    
    int unique_count = 1;
    for (int i = 1; i < arr.size; i++) {
        if (strcmp(arr.names[i], arr.names[i - 1]) != 0) {
            unique_count++;
        }
    }
    
    printf("%d %s", unique_count, arr.names[0]);
    for (int i = 1; i < arr.size; i++) {
        if (strcmp(arr.names[i], arr.names[i - 1]) != 0) {
            printf(" %s", arr.names[i]);
        }
    }
    printf("\n");
    free(arr.names);
}

int main() {
    freopen("indexare.in", "r", stdin);
    freopen("indexare.out", "w", stdout);

    TrieNode *trie_root = create_trie_node();
    FileNode *head = NULL, *tail = NULL;
    
    int n_ops;
    if (scanf("%d", &n_ops) != 1) return 0;

    while (n_ops > 0) {
        char cmd[20];
    
        if (scanf("%s", cmd) != 1) {
         
            PRINT(trie_root);
            n_ops--;
            continue;
        }
        n_ops--;
        
        if (strcmp(cmd, "ADD") == 0) {
            char id[105];
            int sc, n_kw;
            scanf("%s %d %d", id, &sc, &n_kw);
            char (*kws)[105] = malloc(n_kw * 105);
            for (int i = 0; i < n_kw; i++) {
                scanf("%s", kws[i]);
            }
            ADD(&head, &tail, trie_root, id, sc, kws, n_kw);
            free(kws);
        } else if (strcmp(cmd, "DEL") == 0) {
            char id[105];
            scanf("%s", id);
            DEL(&head, &tail, trie_root, id);
        } else if (strcmp(cmd, "ADDKW") == 0) {
            char id[105], kw[105];
            scanf("%s %s", id, kw);
            ADDKW(kw, head, trie_root, id);
        } else if (strcmp(cmd, "DELKW") == 0) {
            char id[105], kw[105];
            scanf("%s %s", id, kw);
            DELKW(kw, &head, &tail, trie_root, id);
        } else if (strcmp(cmd, "FIND") == 0) {
            char kw[105];
            scanf("%s", kw);
            FIND(kw, trie_root);
        } else if (strcmp(cmd, "TOPK") == 0) {
            char kw[105];
            int k;
            scanf("%s %d", kw, &k);
            TOPK(kw, k, trie_root);
        } else if (strcmp(cmd, "PREFIX") == 0) {
            char pref[105];
            scanf("%s", pref);
            PREFIX(pref, trie_root);
        } else if (strcmp(cmd, "PRINT") == 0) {
            PRINT(trie_root);
        }
    }
    return 0;
}