#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ARRIVA printf("fin qua ci arriva \n");
#define STAMPA(a) printf("il valore è %d", a);
#define VECTOR_INITIAL_CAPACITY 100
#define VECTOR_INCREMENT 100
#define HASH_TABLE_SIZE 65536
#define VECTOR_TYPE char*

enum cmd {addent=0, delent=1, addrel=2, delrel=3, report=4, end=5};

//Struct per entità
typedef struct {
    char* name;


} Entity;


//_______________________________________________________________
// Struct per array dinamico
typedef struct {
    int size;      // slots used
    int capacity;  // total available slots
    VECTOR_TYPE *data;     // array of entity we're storing
} Vector;
// Funzioni per array dinamico
void vector_init(Vector *vector);
void vector_append(Vector *vector, VECTOR_TYPE value);
VECTOR_TYPE vector_get(Vector *vector, int index);
void vector_set(Vector *vector, int index, VECTOR_TYPE value);
void vector_double_capacity_if_full(Vector *vector);
void vector_free(Vector *vector);
//_________________________________________________________________



unsigned long hash(unsigned char *str);

int main() {

    //this simulate stdin
    freopen("names.txt", "r", stdin);
    int match = -1;
    char* command;
    char *param1, *param2, *param3;
    int cmd = -1;
    while((match = scanf("%ms", &command)) == 1) {
        if (strcmp(command, "addent") == 0) {
            scanf("%ms", &param1);
            cmd = addent;
        } else if (strcmp(command, "delent") == 0) {
            scanf("%ms", &param1);
            cmd = delent;
        } else if (strcmp(command, "addrel") == 0) {
            scanf("%ms %ms %ms", &param1, &param2, &param3);
            cmd = addrel;
        } else if (strcmp(command, "delrel") == 0) {
            scanf("%ms %ms %ms", &param1, &param2, &param3);
            cmd = delrel;
        } else if (strcmp(command, "report") == 0) {
            cmd = report;
        } else if(strcmp(command, "end") == 0){ //suppongo che se non è nessun altro comando allora è end.
            cmd = end;
        } else break;
        STAMPA(cmd)
        //DA QUI LAVORIAMO SUL COMANDO CORRENTE
    }

    Vector relations;
    vector_init(&relations);
    char* tempstring;
    int i=0;
    while(scanf("%ms", &tempstring) && tempstring != NULL){
        vector_append(&relations, tempstring);
        i++;
    }
    printf("abbiamo aggiunto %d elementi \n questoe ultimo elem: %s", i, vector_get(&relations, i-1));




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

    return hash;
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


