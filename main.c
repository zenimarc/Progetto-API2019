#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ARRIVA printf("fin qua ci arriva \n");
#define STAMPA(a) printf("il valore è %d", a);
#define DEBUG 0
#define VECTOR_INITIAL_CAPACITY 8
#define VECTOR_INCREMENT 10
#define HASH_TABLE_SIZE 1024
#define RELS_ARRAY_SIZE 20
#define VECTOR_TYPE struct ent*

enum cmd {addent=0, delent=1, addrel=2, delrel=3, report=4, end=5};



//_______________________________________________________________
// STRUCT PER ENTRY IN HASH TABLE
struct arrayitem
{
    struct node *head;
    /* head pointing the first element of Linked List at an index of Hash Table */

    struct node *tail;
    /* tail pointing the last element of Linked List at an index of Hash Table */
};
typedef struct arrayitem* Hashtable;

//_______________________________________________________________
// Struct per array dinamico
typedef struct {
    int size;      // slots used
    int capacity;  // total available slots
    VECTOR_TYPE *data;     // array of entity we're storing
} Vector;
//_________________________________________________________________
//STRUCT PER ENTITA'
typedef struct ent{
    char* name;
    Vector* in_rel;
    Vector* out_rel;
} Entity;
//_________________________________________________________________
// STRUCT PER ENTRY IN LINKED LIST OF HASH TABLE
struct node
{
    char* key;
    Entity* value;
    struct node *next;
};
//_________________________________________________________________
// STRUCT PER RELAZIONI (TIPO REL, PUNTATORE A HASHTABLE DI PERTINENZA E LEADERBOARD)
struct relation
{
    Vector* leaderboard;
    int max_inrel;
    char* name;
    Hashtable hashtable;
};
typedef struct relation Relation;
//_________________________________________________________________

// Funzioni per array dinamico
void vector_init(Vector *vector);
Vector* vector_create();
void vector_append(Vector *vector, VECTOR_TYPE value);
VECTOR_TYPE vector_get(Vector *vector, int index);
int vector_size(Vector *vector);
int vector_find(Vector* vector, Entity* entity);
void vector_set(Vector *vector, int index, VECTOR_TYPE value);
void vector_double_capacity_if_full(Vector *vector);
void vector_half_capacity(Vector *vector);
void vector_free(Vector *vector);
void vector_qsort(Vector* vector);
int vector_max(Vector* vector);
int vector_remove(Vector* vector, Entity* ent);
int leaderboard_update(Relation* curr_rel, Entity* ent);
void leaderboard_rebuild(Relation* relations, int rel_index);
//_________________________________________________________________

//Funzioni per hashtable chain
struct arrayitem* hashtable_create();
struct node* get_element(struct node *list, int index);
int hashtable_remove_element(struct arrayitem* hashtable, char* key);
void hashtable_init(struct arrayitem* hashtable);
void hashtable_display(struct arrayitem* hashtable);
struct node* hashtable_insert(struct arrayitem* hashtable, char* key);
struct node* hashtable_insert_observed(struct arrayitem* hashtable, char* key);
struct node* find_top(struct node *list, int max);
int hashtable_ispresent(struct arrayitem* hashtable, char* key);
struct node* hashtable_find_node_entity(struct arrayitem* hashtable, char* key);
//_________________________________________________________________
//Funzioni Entity
Entity* entity_create(char* name);
//_________________________________________________________________
//Funzioni per relations array
void relations_init(Relation* relations);
int comparator(const void *p, const void *q);
int entity_comparator(const void *p, const void *q);
int relations_new_type(Relation* relations_array, char* name);
int relations_find_index(Relation* relations_array, char* name);
void leaderboard_remove(Relation* relations, int rel_index, Entity* ent);
//_________________________________________________________________
//int hash(int);
unsigned long hash(unsigned char *str);
//_________________________________________________________________
//FUNZIONI DEL PROGETTO
void fix_report_top(Relation* relations);
void report_top(Relation* relations);
void do_delrel(Relation* relations, char* param1, char* param2, char* param3);

int main() {

    //this simulate stdin
    //freopen("test.txt", "r", stdin);
    //Initialize relations array
    Relation relations[RELS_ARRAY_SIZE];
    relations_init(relations);
    //Initialize general hashtable to know entites observed
    Hashtable observed = hashtable_create();
    int trash =0;
    int match = -1;
    char* command;
    char *param1, *param2, *param3;
    int cmd = -1;
    while((match = scanf("%ms", &command)) == 1) {
        if (strcmp(command, "addent") == 0) {
            trash= scanf("%ms", &param1);
            cmd = addent;
            hashtable_insert_observed(observed, param1);
        } else if (strcmp(command, "delent") == 0) {
            trash= scanf("%ms", &param1);
            cmd = delent;
            Entity* ent = NULL;
            struct node* node = NULL;
            if(hashtable_remove_element(observed, param1) == 1){
                //in this case we succesfully removed entity from observed and now we have to delete it from all relations hashtable
                for(int rel_index=0; rel_index < RELS_ARRAY_SIZE; rel_index++) {
                    if (relations[rel_index].hashtable != NULL) {
                        //in this case there is an hashable associated to this relation
                        node = hashtable_find_node_entity(relations[rel_index].hashtable, param1); //todo: magari controllare che non sia null prima di richiedere value
                        if (node != NULL) {
                            ent = node->value;
                            //now we delete the entity from hashtable
                            hashtable_remove_element(relations[rel_index].hashtable, ent->name);
                            //we're gonna delete this entity from leaderboard if present
                            leaderboard_remove(relations, rel_index, ent);
                            //we're gonna delete all out_rel of all entites we find in in_rel vector of this ent, envolving this ent.
                            if (ent->in_rel != NULL) {
                                for (int j = 0; j < ent->in_rel->size; j++) {
                                    Entity *ent_with_rel = ent->in_rel->data[j];
                                    vector_remove(ent_with_rel->out_rel, ent);
                                }
                            }
                            //we're gonna delete all in_rel of all entites we find in out_rel vector of this ent, envolving this ent.
                            if (ent->out_rel != NULL) {
                                for (int j = 0; j < ent->out_rel->size; j++) {
                                    Entity *ent_with_rel = ent->out_rel->data[j];
                                    vector_remove(ent_with_rel->in_rel, ent);
                                    leaderboard_remove(relations, rel_index, ent_with_rel);
                                }
                            }
                            //now we rebuild the leaderboard
                            leaderboard_rebuild(relations, rel_index);
                            //at least we free the memory of eliminated ent
                            if(ent->in_rel != NULL){ vector_free(ent->in_rel); }
                            if(ent->out_rel != NULL){ vector_free(ent->out_rel); }
                            free(ent->name);
                            free(ent);
                        }
                    }
                }
            }
            free(param1);

        } else if (strcmp(command, "addrel") == 0) {
            trash= scanf("%ms %ms %ms", &param1, &param2, &param3);
            cmd = addrel;

            if(hashtable_ispresent(observed, param1) && hashtable_ispresent(observed, param2)) {
                /*se entrambe le entità coinvolte sono osservate procedo con inserire la relazione*/
                int rel_index = relations_new_type(relations, param3);
                Hashtable curr_hash = relations[rel_index].hashtable;
                struct node* ent1 = hashtable_insert(curr_hash, param1); //qui in pratica ent1 e ent2 sono puntatori a nodi che puntano a entità magari cambierò in punt a ent diretto.
                struct node* ent2 = hashtable_insert(curr_hash, param2);
                if(vector_find(ent1->value->out_rel, ent2->value) == -1) { //se non trovo ent2 nel vettore relazioni out di ent1 allora rel. non presente ancora, quindi AGGIUNGO rel.
                    //se i vector non sono ancora creati li creo ora
                    if(ent1->value->out_rel == NULL){
                        ent1->value->out_rel = vector_create();
                        vector_init(ent1->value->out_rel);
                    }
                    if(ent2->value->in_rel == NULL){
                        ent2->value->in_rel = vector_create();
                        vector_init(ent2->value->in_rel);
                    }
                    vector_append(ent1->value->out_rel, ent2->value);
                    vector_append(ent2->value->in_rel, ent1->value);
                    if(DEBUG){printf("\nho aggiunto %s %s %s", ent1->value->name, param3, ent2->value->name);}
                    //aggiornamento leaderboard della rel corrente
                    leaderboard_update(&relations[rel_index], ent2->value);
                    //free(param1)

                }else{
                    if (DEBUG) {printf("\nrelazione %s %s %s GIA PRESENTE", ent1->value->name, param3, ent2->value->name);}
                }

            }else{
                //non devo aggiungere niente, libero memoria buffer
                free(param1);
                free(param2);
                free(param3);
            }

        } else if (strcmp(command, "delrel") == 0) {
            trash= scanf("%ms %ms %ms", &param1, &param2, &param3);
            cmd = delrel;
            //TODO: non dovrebbe essere necesario ma potrei controllare se le entità sono observed
            if(hashtable_ispresent(observed, param1) && hashtable_ispresent(observed, param2)) {
                do_delrel(relations, param1, param2, param3);
            }
            free(param1);
            free(param2);
        } else if (strcmp(command, "report") == 0) {
            cmd = report;
            for(int i=0; i<RELS_ARRAY_SIZE; i++){
                if(relations[i].leaderboard != NULL) {
                    vector_qsort(relations[i].leaderboard);
                }
            }
            report_top(relations);
        } else if(strcmp(command, "end") == 0){ //suppongo che se non è nessun altro comando allora è end.
            cmd = end;
        } else break;

        free(command);
    }

/*
 * BLOCCO PER TEST HASH E COLLISIONI
    char** a = (char**)malloc(HASH_TABLE_SIZE*sizeof(char*));
    for(int i=0; i<HASH_TABLE_SIZE;i++)
        a[i] = NULL;
    char* tempstring;
    int i=0;
    int collisions =0;
    while(scanf("%ms", &tempstring) && tempstring != NULL){
        int index = (hash(tempstring)%HASH_TABLE_SIZE);
        if (a[index] == NULL){
            a[i] = tempstring;
        }
        else {
            printf("\ntrovata collisione dopo %d parole con parola: %s e prec: %s", i, tempstring, a[index]);
            collisions++;
            //return 12;
        }
        i++;
    }
    printf("\n->con una tab hash grande: %d blocchi\n"
           "->sono state trovate %d collisioni su un tot di: %d parole in input "
           "\n->Pari al %f percento di collisioni sulla totalità degli input", HASH_TABLE_SIZE, collisions,i, (float) collisions/i);
*/
    return 0;
}

void vector_init(Vector *vector) {
    // initialize size and capacity
    vector->size = 0;
    vector->capacity = VECTOR_INITIAL_CAPACITY;

    // allocate memory for vector->data
    vector->data = malloc(sizeof(VECTOR_TYPE) * vector->capacity);
}

void vector_append(Vector *vector, VECTOR_TYPE value) {
    // make sure there's room to expand into
    vector_double_capacity_if_full(vector);

    // append the value and increment vector->size
    vector->data[vector->size++] = value;
}

VECTOR_TYPE vector_get(Vector *vector, int index) {
    if (index >= vector->size || index < 0) {
        printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
        exit(1);
    }
    return vector->data[index];
}
//this function returns the vector used slots (so the number os relations)
int vector_size(Vector *vector){
    return vector->size;
}

void vector_set(Vector *vector, int index, VECTOR_TYPE value) {
    if(index > vector->capacity){
        vector->capacity += VECTOR_INCREMENT;
        vector->data = realloc(vector->data, sizeof(VECTOR_TYPE) * vector->capacity);
    }
    // set the value at the desired index
    vector->data[index] = value;
}

void vector_double_capacity_if_full(Vector *vector) {
    if (vector->size >= vector->capacity) {
        // double vector->capacity and resize the allocated memory accordingly
        vector->capacity *= 2;
        vector->data = realloc(vector->data, sizeof(VECTOR_TYPE) * vector->capacity);
    }
}

void vector_half_capacity(Vector *vector){
    if (vector->size < vector->capacity/2) {
        // double vector->capacity and resize the allocated memory accordingly
        vector->capacity = vector->capacity /2;
        vector->data = realloc(vector->data, sizeof(VECTOR_TYPE) * vector->capacity);
    }
}

/*this function tries to find an entity in the vector and if found returns its index
 * else return -1 if entity not found*/
int vector_find(Vector* vector, Entity* entity){
    if(vector == NULL){
        return -1;
    }
    for(int i=0; i<vector->size; i++) //TODO verificare che non sia sballato di 1 e magari bisognerà ottimizzare ricerca
        if(vector->data[i] == entity){
            /*if we've found the entity the relations is present*/
            return i;
        }
    return -1;
}

void vector_free(Vector *vector) {
    free(vector->data);
}

unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381; //5381
    int c;

    while (c = *str++)
        hash = hash * 33 + c; /* hash * 33 + c (shift 5)*/

    return hash%HASH_TABLE_SIZE;
}

int hash2(int key)
{
    return (key % HASH_TABLE_SIZE);
}

//FUNZIONI PER HASH
/*This function create a new hashtable with pre defined size and initialize it
 * returns the pointer to the hashtable*/
struct arrayitem* hashtable_create(){
    Hashtable hashtable = (struct arrayitem*) malloc(HASH_TABLE_SIZE * sizeof(struct arrayitem));
    hashtable_init(hashtable);
    return hashtable;
}

/*
 *This function finds the given key in the Linked List
 *Returns it's index
 *Returns -1 in case key is not present
*/
int find(struct node *list, char* key)
{
    int return_value = 0;
    struct node *temp = list;
    while (temp != NULL)
    {
        if (strcmp(temp->key, key) == 0)
        {
            return return_value;
        }
        temp = temp->next;
        return_value++;
    }
    return -1;
}

/*
 *This function finds the given key in the Linked List
 *Returns it's pointer if present
 *Returns NULL in case key is not present
*/
struct node* find_pointer(struct node *list, char* key)
{
    int return_value = 0;
    struct node *temp = list;
    while (temp != NULL)
    {
        if (strcmp(temp->key, key) == 0)
        {
            return temp;
        }
        temp = temp->next;
        return_value++;
    }
    return NULL;
}
/*
 *This function finds the top entity in the Linked List if has more inrel than indicated (max)
 *Returns it's pointer if present else NULL
*/
struct node* find_top(struct node *list, int max)
{
    int return_value = 0;
    struct node *temp = list;
    while (temp != NULL){
        if(temp->value->in_rel != NULL){
            if (temp->value->in_rel->size >= max)
            {
                return temp;
            }
        }
        temp = temp->next;
        return_value++;
    }
    return NULL;
}
/*This function verifies if a key is already present in the general hashtable*/
int hashtable_ispresent(struct arrayitem* hashtable, char* key){
    int index = hash(key);
    struct node *list = (struct node*) hashtable[index].head;
    if (list == NULL){
        /* Absence of Linked List at a given Index of Hash Table */
        return 0;
    }else{
        /* A Linked List is present at given index of Hash Table */
        int find_index = find(list, key);
        if (find_index == -1){
            //Key not found in existing linked list
            return 0;
        }else{
            return 1;
        }
    }
}
/*this function retrieve the pointer to the node of the entity indicated in key searching in indicated hashtable
 * returns NULL if can't find the key*/
struct node* hashtable_find_node_entity(struct arrayitem* hashtable, char* key){
    int index = hash(key);
    struct node *list = (struct node*) hashtable[index].head;
    if (list == NULL){
        /* Absence of Linked List at a given Index of Hash Table */
        return NULL;
    }else{
        /* A Linked List is present at given index of Hash Table */
        struct node* node_found = find_pointer(list, key);
        if (node_found == NULL){
            //Key not found in existing linked list
            return NULL;
        }else{
            return node_found;
        }
    }
}

/*inserisce una nuova entità se non presente e restituisce il puntatore al node della lista che contiene i riferimenti per l'entità
 * se restituisce NULL è perchè è già presente l'entità*/
struct node* hashtable_insert(struct arrayitem* hashtable, char* key)
{
    //float n = 0.0;
    /* n => Load Factor, keeps check on whether rehashing is required or not */

    int index = hash(key);

    /* Extracting Linked List at a given index */
    struct node *list = (struct node*) hashtable[index].head;


    if (list == NULL)
    {
        /* Absence of Linked List at a given Index of Hash Table */

        /* Creating an item to insert in the Hash Table */
        struct node *item = (struct node*) malloc(sizeof(struct node));
        item->key = key;
        item->value = entity_create(key);
        item->next = NULL;

        if (DEBUG) {printf("Inserting %s(key) \n", key);}
        hashtable[index].head = item;
        hashtable[index].tail = item;
        //size++;
        return item;
    }
    else
    {
        /* A Linked List is present at given index of Hash Table */

        struct node* pointer_found = find_pointer(list, key);
        if (pointer_found == NULL)
        {
            /*
             *Key not found in existing linked list
             *Adding the key at the end of the linked list
            */

            /* Creating an item to insert in the Hash Table */
            struct node *item = (struct node*) malloc(sizeof(struct node));
            item->key = key;
            item->value = entity_create(key);
            item->next = NULL;

            hashtable[index].tail->next = item;
            hashtable[index].tail = item;
            //size++;
            return item;

        }else
        {
            /*
             *Key already present in linked list
             *nothing to do
            */
            if (DEBUG){printf("%s key is already present, nothing to do", key);}
            //struct node *element = get_element(list, find_index);
            //element->value = value;
            return pointer_found;
        }

    }

    /*Calculating Load factor
    n = (1.0 * size) / max;
    if (n >= 0.75)
    {
        //rehashing

        printf("going to rehash\n");
        rehash();

    }*/

}

struct node* hashtable_insert_observed(struct arrayitem* hashtable, char* key)
{
    //float n = 0.0;
    /* n => Load Factor, keeps check on whether rehashing is required or not */

    int index = hash(key);

    /* Extracting Linked List at a given index */
    struct node *list = (struct node*) hashtable[index].head;


    if (list == NULL)
    {
        /* Absence of Linked List at a given Index of Hash Table */

        /* Creating an item to insert in the Hash Table */
        struct node *item = (struct node*) malloc(sizeof(struct node));
        item->key = key;
        item->value = NULL;
        item->next = NULL;

        if (DEBUG) {printf("Inserting %s(key) \n", key);}
        hashtable[index].head = item;
        hashtable[index].tail = item;
        //size++;
        return item;
    }
    else
    {
        /* A Linked List is present at given index of Hash Table */

        struct node* pointer_found = find_pointer(list, key);
        if (pointer_found == NULL)
        {
            /*
             *Key not found in existing linked list
             *Adding the key at the end of the linked list
            */

            /* Creating an item to insert in the Hash Table */
            struct node *item = (struct node*) malloc(sizeof(struct node));
            item->key = key;
            item->value = NULL;
            item->next = NULL;

            hashtable[index].tail->next = item;
            hashtable[index].tail = item;
            //size++;
            return item;

        }else
        {
            /*
             *Key already present in linked list
             *nothing to do except free the buffer
            */
            free(key);
            if (DEBUG){printf("%s key is already present, nothing to do", key);}
            //struct node *element = get_element(list, find_index);
            //element->value = value;
            return pointer_found;
        }

    }

    /*Calculating Load factor
    n = (1.0 * size) / max;
    if (n >= 0.75)
    {
        //rehashing

        printf("going to rehash\n");
        rehash();

    }*/

}

/* Returns the node (Linked List item) located at given index  */
struct node* get_element(struct node *list, int index)
{
    int i = 0;
    struct node *temp = list;
    while (i != index)
    {
        temp = temp->next;
        i++;
    }
    return temp;
}
/* To remove an element from Hash Table
 * return -1 if key doesnt exist
 * return 1 if succesfully removed*/
int hashtable_remove_element(struct arrayitem* hashtable, char* key)
{
    int index = hash(key);
    struct node *list = (struct node*) hashtable[index].head;

    if (list == NULL)
    {
        if(DEBUG){printf("This key does not exists\n");}
        return -1;
    }
    else
    {
        int find_index = find(list, key);

        if (find_index == -1)
        {
            if(DEBUG){printf("This key does not exists\n");}
            return -1;
        }
        else
        {
            struct node *temp = list;
            if (strcmp(temp->key, key) == 0)
            {

                hashtable[index].head = temp->next;
                free(temp);
                if(DEBUG){printf("This key has been removed\n");}
                return 1;
            }

            while (strcmp(temp->next->key, key) != 0)
            {
                temp = temp->next;
            }

            if (hashtable[index].tail == temp->next)
            {
                temp->next = NULL;
                hashtable[index].tail = temp;
                free(temp->next);

            }
            else
            {
                struct node* to_delete = temp->next;
                temp->next = temp->next->next;
                free(to_delete);

            }

            if(DEBUG){printf("This key has been removed\n");}
            return 1;
        }

    }

}

/* To hashtable_display the contents of Hash Table */
void hashtable_display(struct arrayitem* hashtable)
{
    int i = 0;
    for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
        struct node *temp = hashtable[i].head;
        if (temp == NULL)
        {
            printf("array[%d] has no elements\n", i);

        }
        else
        {
            printf("array[%d] has elements-: ", i);
            while (temp != NULL)
            {
                printf("key= %s\t", temp->key);
                temp = temp->next;
            }
            printf("\n");

        }
    }
}

/* For initializing the Hash Table */
void hashtable_init(struct arrayitem* hashtable)
{
    int i = 0;
    for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
        hashtable[i].head = NULL;
        hashtable[i].tail = NULL;
    }

}

/* To create a new entity based on the unique name */
Entity* entity_create(char* name){
    Entity* entity = (Entity*) malloc(sizeof(Entity));
    entity->name = name;
    entity->in_rel=NULL;
    entity->out_rel=NULL;
    return entity;
}
/*To compare two relation structs*/
int comparator(const void *p, const void *q)
{
    if (((Relation *) p)->name == NULL || ((Relation *) q)->name == NULL) {return 0;} else {
        return strcmp(((Relation *) p)->name, ((Relation *) q)->name);
    }
}

int entity_comparator(const void *p, const void *q){
    Entity* ent1 = (*((Entity**)p));
    Entity* ent2 = (*((Entity**)q)); //TODO nota BENE come ho fatto nella compare che riceve un struct ent*** ovvero Entity** quindi deferenzio
    if(ent1 == NULL){
        if(ent2 == NULL){
            //in case ent1 and ent2 are NULL
            return 0;
        }else {
            //in case ent1 == NULL and ent2 has a value
            return 1;
        }
    }else{
        if(ent2 == NULL){
            //in case ent1 != NULL and ent2 is NULL
            return -1;
        }else{
            //in case ent1 and ent2 are != NULL, comparing by string
            if (ent1->name == NULL || ent2->name == NULL) {return 0;} else {
                return strcmp(ent1->name, ent2->name);
            }
        }
    }
}

/*To compare two Entity structs
 * TODO: non va la comparazione della inrel size ma nell'attuale impl non serve e viene usata funz sopra.*/
int entity_comparator2(const void *p, const void *q){
    if (((Entity *) p)->name == NULL || ((Entity *) q)->name == NULL || ((Entity *) p)->in_rel == NULL || ((Entity *) q)->in_rel == NULL) {return 0;} else {
        if(((Entity*)p)->in_rel->size == ((Entity*)q)->in_rel->size) {
            return strcmp(((Entity *) p)->name, ((Entity *) q)->name);
        }else if(((Entity*)p)->in_rel->size < ((Entity*)q)->in_rel->size) {
            return -1;
        }else{
            return 1;
        }
    }
}
/*this function checks if relation already present and return array index of the rel. and if not it adds the new relation and return index
 * TODO possibile miglioramento se inserisco subito ordinato e cerco ordinato*/
int relations_new_type(Relation* relations_array, char* name){
    for(int i=0; i<RELS_ARRAY_SIZE; i++){
        if(relations_array[i].name != NULL) {
            if (strcmp(relations_array[i].name, name) == 0) {
                if (DEBUG) { printf("\nrelation %s gia presente", name); }
                //visto che è già presente posso liberare il buffer del nome
                free(name);
                return i;
            }
        }else{
            relations_array[i].name = name;
            relations_array[i].hashtable = hashtable_create();
            relations_array[i].leaderboard = vector_create();
            qsort(relations_array, RELS_ARRAY_SIZE, sizeof(Relation), comparator); //after hashtable_insert new rel we quicksort the relations array
            return relations_find_index(relations_array, name); //TODO verifcare se funziona
        }
    }
}


/*returns index of relation array of the indicated relation name
 * returns -1 if relation not found*/
int relations_find_index(Relation* relations_array, char* name) {
    for (int i = 0; i < RELS_ARRAY_SIZE; i++) {
        if (relations_array[i].name != NULL) {
            if (strcmp(relations_array[i].name, name) == 0) {
                if (DEBUG) { printf("\nrelation %s trovata", name); }
                return i;
            }
        }
    }
    return -1;
}
void relations_init(Relation* relations){
    for(int i=0; i<RELS_ARRAY_SIZE; i++){
        relations[i].hashtable = NULL;
        relations[i].name = NULL;
        relations[i].leaderboard = NULL;
        relations[i].max_inrel = 0;
    }
}
//TODO per ora trova solo una entity con inrel max ma dobbiamo salvare tutte quelle parimerito (array di supporto)
void fix_report_top(Relation* relations){
    Hashtable curr_hash;
    int maxrel=0;
    struct node* topent = NULL;
    struct node* possible_topent = NULL;
    for(int i=0; i<RELS_ARRAY_SIZE; i++) {
        curr_hash = relations[i].hashtable;
        if (curr_hash != NULL) {
            if(DEBUG){printf("\nsto testando %s", relations[i].name);}
            for (int j = 0; j < HASH_TABLE_SIZE; j++) {
                if (curr_hash[j].head != NULL) {
                    possible_topent = find_top(curr_hash[j].head, maxrel);
                    if(possible_topent != NULL) {
                        topent = possible_topent;
                        maxrel = topent->value->in_rel->size;
                    }
                }
            }
            if(topent != NULL){
                printf("la top entity e: %s con %d relsin", topent->value->name, maxrel);
            }
        }else return;
    }
}
void leaderboard_print_names(Vector* leaderboard){
    for(int i=0; i<leaderboard->size; i++){
        fputs(leaderboard->data[i]->name, stdout);
        fputc(32, stdout);
    }
}

void report_top(Relation* relations) {
    int blank = 1;
    Vector* curr_leaderboard = NULL;
    for (int i = 0; i < RELS_ARRAY_SIZE; i++) {
        curr_leaderboard = relations[i].leaderboard;
        if (curr_leaderboard != NULL && curr_leaderboard->size != 0) {
            blank = 0;
            fputs(relations[i].name, stdout);
            fputc(32, stdout);
            leaderboard_print_names(curr_leaderboard);
            printf("%d; ", curr_leaderboard->data[0]->in_rel->size);
        }
    }
    if (blank){
        fputs("none", stdout);
    }
    fputc('\n', stdout);
}

/*Create and initialize a new dynamic vector*/
Vector* vector_create(){
    Vector* new_vector = malloc(sizeof(Vector));
    vector_init(new_vector);
    return new_vector;
}
/*this function find the max value */
int vector_max(Vector* vector){
    int max=0;
    for(int i=0; i<vector->size; i++){
        if(vector->data[i]->in_rel->size >= max){
            max = vector->data[i]->in_rel->size;
        }
    }
    return max;
}
/*this function removes the entity in the vector if found and returns 0
 * if not found returns -1*/
int vector_remove(Vector* vector, Entity* ent){
    if(vector==NULL){return -1;}
    int vector_index = vector_find(vector, ent);
    if(vector_index != -1){//rimuovo la rel solo se l'ho trovata, se non c'era vabbè
        vector_set(vector, vector_index, NULL);
        //dopo che ho settato a null faccio sort per mettere il null in fondo
        vector_qsort(vector);
        //poi decremento di 1 la size del vector
        vector->size--;
        //we try to half the vector if possible (the function will check)
        vector_half_capacity(vector);
        return 0;
    }else{
        return -1;
    }
}

void vector_qsort(Vector* vector){
    qsort(&vector->data[0], vector->size, sizeof(VECTOR_TYPE), entity_comparator); //TODO tenere d'occhio
}

/*this function update the leaderboard adding new entities with same score and return 0; But if the new entity is higher
 * free the leaderboard and create a new leaderboard with this entity*/
int leaderboard_update(Relation* curr_rel, Entity* ent){
    if(curr_rel == NULL || ent == NULL){return -1;}
    if(ent->in_rel->size == 0){return 0;} //non devo appendere un entità che abbia 0 rels nella leaderboard mai!
    if(curr_rel->leaderboard->size != 0){
        if(ent->in_rel->size > curr_rel->max_inrel){
            /*in this case the new entity is strictly major than the old one
             * delete or re-init all leaderboard and add this new entity*/
            vector_free(curr_rel->leaderboard);
            free(curr_rel->leaderboard);
            curr_rel->leaderboard = vector_create();
            vector_append(curr_rel->leaderboard, ent);
            //aggiorno maxinrel con nuovo score di entità appena aggiunta
            curr_rel->max_inrel = ent->in_rel->size;
            return -1;
        }else if(ent->in_rel->size == curr_rel->max_inrel){
            /*if vector contains entities with same score we appnd the new one and than quicksort by name*/
            vector_append(curr_rel->leaderboard, ent);
            return 0;
        }else{
            return 0;
        }
    }
    vector_append(curr_rel->leaderboard, ent);
    curr_rel->max_inrel = ent->in_rel->size;
    return 0;
}

void do_delrel(Relation* relations, char* param1, char* param2, char* param3){
    int rel_index = relations_find_index(relations, param3);
    if(rel_index != -1) {
        struct node *node1 = hashtable_find_node_entity(relations[rel_index].hashtable, param1);
        struct node *node2 = hashtable_find_node_entity(relations[rel_index].hashtable, param2);
        if (node1 != NULL && node2 != NULL) { //elimino la rel solo se trovo entrambe le enitità
            Entity *ent1 = node1->value; //todo: non dovrebbe generare null però al limite controllare prima di pescare value
            Entity *ent2 = node2->value;
            vector_remove(ent1->out_rel, ent2);
            vector_remove(ent2->in_rel, ent1);
            if(vector_find(relations[rel_index].leaderboard, ent2) != -1){
                //ent2 era nella leaderboard
                leaderboard_rebuild(relations, rel_index);
            }
        }
    }

}
/*this function delete the leaderboard and completely rebuild it*/
void leaderboard_rebuild(Relation* relations, int rel_index){

    relations[rel_index].max_inrel = 0;

    if (relations[rel_index].leaderboard != NULL && relations[rel_index].hashtable != NULL) {
        if(DEBUG){printf("\nsto testando %s", relations[rel_index].name);}

        int i = 0;
        for (i = 0; i < HASH_TABLE_SIZE; i++)
        {
            struct node *temp = relations[rel_index].hashtable[i].head;
            if (temp == NULL)
            {
                //nessuna lista in questo index della hashtable

            }
            else
            {
                //è stata trovata una lista di nodi con entità
                while (temp != NULL)
                {
                    //qui provo tutti i nodi della lista
                    if(temp->value->in_rel != NULL){ //todo: #time questo controllo potrebbe essere tolto perche già fatto in leaderboard_update
                        leaderboard_update(&relations[rel_index], temp->value);
                    }
                    temp = temp->next;
                }
            }
        }
    }else{
        printf("passati dei null a leaderboard_rebuild!");
    }
}
void leaderboard_remove(Relation* relations, int rel_index, Entity* ent){
    int top_ent_index = vector_find(relations[rel_index].leaderboard, ent);
    if (top_ent_index != -1) {
        //se trovo l'entità ricevente nella leaderboard la rimuovo perchè sicuramente non ha più lo stesso punteggio avendo perso una rel.
        vector_set(relations[rel_index].leaderboard, top_ent_index, NULL);
        vector_qsort(relations[rel_index].leaderboard);
        if(relations[rel_index].leaderboard->size > 0) {
            relations[rel_index].leaderboard->size--;
        }
    }
}

/*
//VERSIONE DA FILE
//  Open the file for reading
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    int line_count = 0;
    ssize_t line_size;
    FILE *fp = fopen("input.txt", "r");
    if (!fp)
    {
        fprintf(stderr, "Error opening file '%s'\n", "input.txt");
        return EXIT_FAILURE;
    }

    //Get the first line of the file.
    line_size = getline(&line_buf, &line_buf_size, fp);

    //Loop through until we are done with the file.
    while (line_size >= 0)
    {
        //Increment our line count
        line_count++;

        //Show the line details
        printf("line[%06d]: chars=%06zd, buf size=%06zu, contents: %s", line_count,
               line_size, line_buf_size, line_buf);

        //Get the next line
        line_size = getline(&line_buf, &line_buf_size, fp);
    }

    //Free the allocated line buffer
    free(line_buf);
    line_buf = NULL;

    //Close the file now that we are done with it
    fclose(fp);
    */


