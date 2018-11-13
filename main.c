/*
* Implements a priority queue using an array based min-heap implementation.
* Runs a simulation of patients going into an emergency room with priorities
* assigned based on their symptoms.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//node definition of the ADT
typedef struct {
  int priority;
  char *data;
  int read;
} node_t;

//ADT list info
typedef struct {
  node_t *nodes;
  int len;
  int size;
} heap_t;

//Insertion on Heap
void insert(heap_t *h, int priority, char *data, int read) {
  if (h->len + 1 >= h->size) {
    h->size = h->size ? h->size * 2 : 4;
    h->nodes = (node_t *)realloc(h->nodes, h->size * sizeof(node_t));
  }

  int i = h->len + 1;
  int j = i / 2;
  while (i > 1 && h->nodes[j].priority > priority) 
  {
    h->nodes[i] = h->nodes[j];
    i = j;
    j = j / 2;
  }
  h->nodes[i].priority = priority;
  h->nodes[i].data = data;
  h->nodes[i].read = read;
  h->len++;
}

//Positions the root value down the Heap tree
void sink(heap_t *h, int end)
{
  int start=1;
  node_t v = h->nodes[start];
  while (start <= end / 2)
  {
    int j = 2 * start;
    if (j<end && h->nodes[j].priority>h->nodes[j + 1].priority) j++;  //2k < 2k+1 -> left < right
    if (v.priority <= h->nodes[j].priority) 
      break;
    h->nodes[start] = h->nodes[j];
    start = j;
  }
  h->nodes[start] = v;
}

//Build the pritority queue based on the Heap
void hsort(heap_t *h)
{
  int end = h->len;
  while (end > 1)
  {
    //Swap(h[end], h[start])
    node_t t;
    t.priority = h->nodes[end].priority;
    t.data = h->nodes[end].data;
    t.read = h->nodes[end].read;

    h->nodes[end].priority = h->nodes[1].priority;
    h->nodes[end].data = h->nodes[1].data;
    h->nodes[end].read = h->nodes[1].read;
        
    h->nodes[1].priority = t.priority;
    h->nodes[1].data = t.data;
    h->nodes[1].read = t.read;

    end = end - 1;
    if (end > 1) 
      sink(h, end);
  }
}

//Shows option menu
void displayMenu()
{
  printf("1) Add a new patient\n");
  printf("2) Run simulation\n");
  printf("3) Exit\n");
  printf("option> ");
}

//Concatenates 3 strings
char *concatenate(const char *a, const char *b, const char *c) 
{
  size_t alen = strlen(a);
  size_t blen = strlen(b);
  size_t clen = strlen(c);
  char *res = malloc(alen + blen + clen + 1);
  memcpy(res, a, alen);
  memcpy(res + alen, b, blen);
  memcpy(res + alen + blen, c, clen + 1);
  return res;
}

//Builds a separate array to print the data in the order it was read
void asort(node_t *a, int n) 
{
  int i = 1;
  for (; i < n; ++i) {
    node_t tmp=a[i];

    int j = i;
    while (j > 0 && tmp.read < a[j - 1].read) 
    {
      a[j] = a[j - 1];
      --j;
    }
    a[j] = tmp;
  }
}


int main(int argc, char **argv)
{
  //Create the root node of the Heap
  heap_t *h = (heap_t *)calloc(1, sizeof(heap_t));

  int count = 0;
  if (argc == 2)
  {
    FILE *fp = fopen(argv[1], "r");

    if (fp == NULL) {
      return 1;
    }

    //Load data file
    char line[256];
    while (fgets(line, 1024, fp))
    {
      char *cline = strtok(line, "\n");

      char* token = strtok(cline, " ");

      char id[16];
      sprintf(id, "%-9s", token);

      char *prior;
      int priority;
      char *symptom;
      int tok = 1;
      while (token) {
        if (tok == 1)
        {
          prior = strtok(NULL, " ");
          priority = atoi(prior);
          *token = *prior;
        }
        else if (tok == 2)
        {
          symptom = strtok(NULL, " ");
          *token = *symptom;
          break;
        }
        tok++;
      }

      char *sprior = concatenate("     ", prior, "       ");
      sprior = concatenate(id, sprior, symptom);
      insert(h, priority, sprior, count);
      count++;
    }
    fclose(fp);
  }
  else {
    printf("No data file provided\n\n");
  }

  //menu options
  int op;
  while(1)
  {
    displayMenu();
    scanf("%d", &op);

    if (op == 1)
    {
      //Enter manual data
      char sid[16];
      printf("Introduce the client ID (max 10 digits): ");
      scanf("%s", sid);
      char id[16];
      sprintf(id, "%-9s", sid);

      int priority=0;
      while (priority < 1 || priority>5)
      {
        printf("Introduce the priority (1-5): ");
        scanf("%d", &priority);
      }
      char cprior[256];
      snprintf(cprior, sizeof(cprior), "%d", priority);

      char symptom[2] = {'X','X'};
      while (strcmp(symptom, "CV") != 0 && strcmp(symptom, "HN") != 0 && strcmp(symptom, "EV") != 0 && strcmp(symptom, "GI") != 0
        && strcmp(symptom, "MH") != 0 &&  strcmp(symptom, "NC") != 0 && strcmp(symptom, "EC") != 0 && strcmp(symptom, "RC") != 0
        && strcmp(symptom, "SK") != 0 &&  strcmp(symptom, "SA") != 0 && strcmp(symptom, "TR") != 0)
      {
        printf("Introduce the 2-digit symptom code ('CV, 'HN','EV','GI','MH','NC','EC','RC','SK','SA','TR): ");
        scanf("%s", symptom);
      }
      
      char *sprior = concatenate("     ", cprior, "       ");
      sprior = concatenate(id, sprior, symptom);
      count++;

      insert(h, priority, sprior, count);

      printf("Data entered successfully\n");
    }
    else if (op == 2)
    {
      if (h->len > 0)
      {
        //Run simulation
        hsort(h);

        node_t *output = malloc(sizeof(node_t) * h->len);
        int timer = 1; 
        int o = 0;
        int j = h->len;
        for (; j >= 1; j--, o++) {
          char ctimer[256];
          snprintf(ctimer, sizeof(ctimer), "%d", timer);

          char *s = concatenate(h->nodes[j].data, "         ", ctimer);
          output[o].data = s;
          output[o].read = h->nodes[j].read;
          timer++;
        }

        asort(output, h->len);

        printf("PatientID Priority Symptom Code Time Finished\n");

        for (j = 0; j < h->len; j++) {
          printf("%s\n", output[j].data);
        }
        printf("\n");
      }
      else
      {
        printf("No data available for simulation. Please add patient data.\n");
      }
    }
    else {
      //exit with any other key
      break;
    }    
  }

 	return 0;
}