#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ARRIVA printf("fin qua ci arriva \n");
#define STAMPA(a) printf("il valore è %d", a);
#define DEBUG 1
#define VECTOR_INITIAL_CAPACITY 100
#define VECTOR_INCREMENT 100
#define HASH_TABLE_SIZE 100
#define RELS_ARRAY_SIZE 30
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
// Funzioni per array dinamico
void vector_init(Vector *vector);
Vector* vector_create();
void vector_append(Vector *vector, VECTOR_TYPE value);
VECTOR_TYPE vector_get(Vector *vector, int index);
int vector_size(Vector *vector);
void vector_set(Vector *vector, int index, VECTOR_TYPE value);
void vector_double_capacity_if_full(Vector *vector);
void vector_free(Vector *vector);
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
    //TODO mettere puntatore a leaderboard (da stabilire che data structure)
    char* name;
    Hashtable hashtable;
};
typedef struct relation Relation;
//_________________________________________________________________
//Funzioni per hashtable chain
struct arrayitem* hashtable_create();
struct node* get_element(struct node *list, int index);
int remove_element(struct arrayitem* hashtable, char* key);
void hashtable_init(struct arrayitem* hashtable);
void hashtable_display(struct arrayitem* hashtable);
struct node* hashtable_insert(struct arrayitem* hashtable, char* key);
int hashtable_ispresent(struct arrayitem* hashtable, char* key);
//_________________________________________________________________
//Funzioni Entity
Entity* entity_create(char* name);
//_________________________________________________________________
//Funzioni per relations array
void relations_init(Relation* relations);
int comparator(const void *p, const void *q);
int relations_new_type(Relation* relations_array, char* name);
//_________________________________________________________________
//int hash(int);
unsigned long hash(unsigned char *str);
//_________________________________________________________________
//FUNZIONI DEL PROGETTO

int main() {

    //this simulate stdin
    freopen("test.txt", "r", stdin);
    //Initialize relations array
    Relation relations[RELS_ARRAY_SIZE];
    relations_init(relations);
    //Initialize general hashtable to know entites observed
    Hashtable observed = hashtable_create();

    int match = -1;
    char* command;
    char *param1, *param2, *param3;
    int cmd = -1;
    while((match = scanf("%ms", &command)) == 1) {
        if (strcmp(command, "addent") == 0) {
            scanf("%ms", &param1);
            cmd = addent;
            hashtable_insert(observed, param1);
        } else if (strcmp(command, "delent") == 0) {
            scanf("%ms", &param1);
            cmd = delent;
        } else if (strcmp(command, "addrel") == 0) {
            scanf("%ms %ms %ms", &param1, &param2, &param3);
            cmd = addrel;
            if(hashtable_ispresent(observed, param2) && hashtable_ispresent(observed, param3)) {
                /*se entrambe le entità coinvolte sono osservate procedo con inserire la relazione*/
                Hashtable curr_hash = relations[relations_new_type(relations, param1)].hashtable;
                struct node* ent1 = hashtable_insert(curr_hash, param2);
                struct node* ent2 = hashtable_insert(curr_hash, param3);

            }

        } else if (strcmp(command, "delrel") == 0) {
            scanf("%ms %ms %ms", &param1, &param2, &param3);
            cmd = delrel;
        } else if (strcmp(command, "report") == 0) {
            cmd = report;
        } else if(strcmp(command, "end") == 0){ //suppongo che se non è nessun altro comando allora è end.
            cmd = end;
        } else break;

        //DA QUI LAVORIAMO SUL COMANDO CORRENTE
    }

    hashtable_display(observed);
    /*
     * RELATIONS WITH DYNAMIC VECTOR TEST
    Vector relations;
    vector_init(&relations);
    char* tempstring;
    int i=0;
    while(scanf("%ms", &tempstring) && tempstring != NULL){
        vector_append(&relations, tempstring);
        i++;
    }
    printf("abbiamo aggiunto %d elementi \n questoe ultimo elem: %s", i, vector_get(&relations, i-1));
    */



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
    /*
       //TEST OF DYNAMIC ARRAY VECTOR TYPE
       Vector dynamic_array;
       vector_init(&dynamic_array);
       for(int i=0; i<1000; i++)
           vector_append(&dynamic_array, i);
       for(int i=0; i<1000; i++)
           printf("\n%d", vector_get(&dynamic_array, i));
       printf("\nget: %d", vector_get(&dynamic_array, 1));
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
        if (temp->key == key)
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
        if (temp->key == key)
        {
            return temp;
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
int remove_element(struct arrayitem* hashtable, char* key)
{
    int index = hash(key);
    struct node *list = (struct node*) hashtable[index].head;

    if (list == NULL)
    {
        printf("This key does not exists\n");
        return -1;
    }
    else
    {
        int find_index = find(list, key);

        if (find_index == -1)
        {
            printf("This key does not exists\n");
            return -1;
        }
        else
        {
            struct node *temp = list;
            if (temp->key == key)
            {

                hashtable[index].head = temp->next;
                printf("This key has been removed\n");
                return 1;
            }

            while (temp->next->key != key)
            {
                temp = temp->next;
            }

            if (hashtable[index].tail == temp->next)
            {
                temp->next = NULL;
                hashtable[index].tail = temp;

            }
            else
            {
                temp->next = temp->next->next;

            }

            printf("This key has been removed\n");
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
                printf("key= %s  value= %s\t", temp->key, temp->value->in_rel);
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
/*this function checks if relation already present and return array index of the rel. and if not it adds the new relation and return index
 * TODO possibile miglioramento se inserisco subito ordinato e cerco ordinato*/
int relations_new_type(Relation* relations_array, char* name){
    for(int i=0; i<RELS_ARRAY_SIZE; i++){
        if(relations_array[i].name != NULL) {
            if (relations_array[i].name == name) {
                if (DEBUG) { printf("relation %s gia presente", name); }
                return i;
            }
        }else{
            relations_array[i].name = name;
            relations_array[i].hashtable = (Hashtable) malloc(sizeof(Hashtable));
            qsort(relations_array, RELS_ARRAY_SIZE, sizeof(Relation), comparator); //after hashtable_insert new rel we quicksort the array
            return relations_new_type(relations_array, name); //TODO verifcare se funziona
        }
    }
}
void relations_init(Relation* relations){
    for(int i=0; i<RELS_ARRAY_SIZE; i++){
        relations[i].hashtable = NULL;
        relations[i].name = NULL;
    }
}

/*Create and initialize a new dynamic vector*/
Vector* vector_create(){
    Vector* new_vector = malloc(sizeof(Vector));
    vector_init(new_vector);
    return new_vector;
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


