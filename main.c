#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef enum { AND, OR, NAND, NOR, XOR, NOT, PASS, DECODER, MULTIPLEXER } kind_t;

typedef struct var {
    int value;
    char* name;
    int initialized;
} var;

typedef struct gate {
    kind_t kind;
    int numInputs;
    int* inputIndices;
    int numOutputs;
    int* outputIndices;
    int numSelectors;
    int* selectorIndices;
    int visited;
} gate;

gate *circut = NULL;
var *variables;
int nextIndex = 0;

void insert(char *name, int value, int visited) {
    variables = realloc(variables, (1+nextIndex)*sizeof(var));
    variables[nextIndex].name = malloc(17*sizeof(char));
    strcpy(variables[nextIndex].name, name);
    variables[nextIndex].value=value;
    variables[nextIndex].initialized=visited;
    nextIndex++;
}

int getIndex (char *name) {
    for (int i=0; i<nextIndex; i++) {
        if (strcmp(variables[i].name, name) == 0)
            return i; }
    return -1; // not found
}

int binDec (int* inputs, int numInputs){
    int sum = 0, power = 0;
    for (int i=numInputs-1; i>=0; i--) {
        sum += (pow(2,power) * variables[inputs[i]].value);
        power++;
    }
    return sum;
}

void and (struct gate g) {
    variables[g.outputIndices[0]].value = variables[g.inputIndices[0]].value && variables[g.inputIndices[1]].value;
}

void or (struct gate g) {
    variables[g.outputIndices[0]].value = variables[g.inputIndices[0]].value || variables[g.inputIndices[1]].value;
}

void not (struct gate g) {
    variables[g.outputIndices[0]].value = !variables[g.inputIndices[0]].value;
}

void pass (struct gate g) {
    variables[g.outputIndices[0]].value = variables[g.inputIndices[0]].value;
}

void xor (struct gate g) {
    variables[g.outputIndices[0]].value = variables[g.inputIndices[0]].value ^ variables[g.inputIndices[1]].value;
}

void nand (struct gate g) {
    variables[g.outputIndices[0]].value = !(variables[g.inputIndices[0]].value && variables[g.inputIndices[1]].value);
}

void nor (struct gate g) {
    variables[g.outputIndices[0]].value = !(variables[g.inputIndices[0]].value || variables[g.inputIndices[1]].value);
}

void multi (struct gate g) {
    int sum = binDec(g.selectorIndices, g.numSelectors);
    variables[g.outputIndices[0]].value = variables[g.inputIndices[sum]].value;
}

void deco (struct gate g) {
    for (int i=0; i<g.numOutputs; i++) {
        variables[g.outputIndices[i]].value = 0; }
    int sum = binDec(g.inputIndices, g.numInputs);
    variables[g.outputIndices[sum]].value = 1;
}

int allInitialized (int steps) {
    for (int i=0; i<steps; i++) {
        if (circut[i].visited == 0) {
            return 0;
            break; }
    }
    return 1;
}

void printInputs (int* inputs, int numInputs) {
    for (int i=0; i<numInputs; i++)
        printf("%d ", variables[inputs[i]].value);
    printf("|");
}

void printOutputs (int *outputs, int numOutputs) {
    for (int i=0; i<numOutputs; i++)
        printf(" %d", variables[outputs[i]].value);
}

int main(int argc, const char * argv[]) {

    FILE *file = fopen(argv[1], "r");
    
    /* HANDLE ARGUMENT ERRORS */
    if (argc != 2) {
       printf("Error: incorrect number of arguments.\n");
        return 0; } // failure
    else if (!file) {
        printf("Error: could not open file.\n");
        return 0; } // failure
    
    char read[17] = ""; // allow one extra space for end line character
    int numInputs=0, numOutputs=0, notConstant=0, topSortNecessary=0;
    insert("0", 0, 1);
    insert("1", 1, 1);

    fscanf(file, "%16s", read); // read "INPUT"
    fscanf(file, "%d", &numInputs); // read the number of inputs
    int* inputs = malloc(numInputs*sizeof(int));
    for (int i=0; i<numInputs; i++) {
        memset(read,0,sizeof(read));
        fscanf(file, "%16s", read);
        insert(read, 0, 1);
        inputs[i] = getIndex(read);
        if (inputs[i] != 0 && inputs[i] != 1) {
            insert(read, 0, 1);
            notConstant++;
        }
    }
    memset(read,0,sizeof(read));
    
    fscanf(file, "%16s", read); // read "OUTPUT"
    fscanf(file, "%d", &numOutputs); // read the number of outputs
    int* outputs = malloc(numOutputs*sizeof(int));
    for (int o=0; o<numOutputs; o++) {
        memset(read,0,sizeof(read));
        fscanf(file, "%16s", read);
        insert(read, 0, 1);
        outputs[o] = getIndex(read);
    }
    memset(read,0,sizeof(read));
    
    int in = 0, out = 1, selectors = 0, steps = 0;
    kind_t type = AND;
    int* sels = NULL;
    while (!feof(file)) {
        out = 1;
        selectors = 0;
        fscanf(file, "%16s", read);
        if (strcmp(read, "AND")==0) {
            in = 2;
            type = AND;
        }
        else if (strcmp(read, "OR")==0) {
            in = 2;
            type = OR;
        }
        else if (strcmp(read, "NOT")==0) {
            in = 1;
            type = NOT;
        }
        else if (strcmp(read, "XOR")==0) {
            in = 2;
            type = XOR;
        }
        else if (strcmp(read, "NOR")==0) {
            in = 2;
            type = NOR;
        }
        else if (strcmp(read, "NAND")==0) {
            in = 2;
            type = NAND;
        }
        else if (strcmp(read, "PASS")==0) {
            in = 1;
            type = PASS;
        }
        else if (strcmp(read, "DECODER")==0) {
            fscanf(file, "%d", &in);
            type = DECODER;
            out = pow(2, in);
        }
        else if (strcmp(read, "MULTIPLEXER")==0) {
            fscanf(file, "%d", &selectors);
            in = pow(2, selectors);
            type = MULTIPLEXER;
            sels = malloc(selectors * sizeof(int));
        }
        else {
            steps++;
            continue;
        }
        memset(read,0,sizeof(read));
        circut = realloc(circut, (1+steps)*sizeof(gate));
        circut[steps].kind = type;
        circut[steps].visited=1;
        
        /*LOAD GATE INPUTS*/
        circut[steps].inputIndices = malloc(in * sizeof(int));
        for (int i=0; i<in; i++) {
            fscanf(file, "%16s", read);
            if (getIndex(read) == -1) {
                insert(read, 0, 0);
                topSortNecessary=1; // must perform topological sort on circut
                circut[steps].visited=0;
            }
            circut[steps].inputIndices[i] = getIndex(read);
        }
        /*LOAD GATE SELECTORS*/
            circut[steps].selectorIndices = malloc(selectors * sizeof(int));
            for (int s=0; s<selectors; s++) {
                fscanf(file, "%16s", read);
                circut[steps].selectorIndices[s] = getIndex(read);
        }
        /*LOAD GATE OUTPUTS*/
        circut[steps].outputIndices = malloc(out * sizeof(int));
        for (int o=0; o<out; o++) {
            fscanf(file, "%16s", read);
            if (getIndex(read) == -1)
                insert(read, 0, 1);
            circut[steps].outputIndices[o] = getIndex(read);
        }
        circut[steps].numInputs = in;
        circut[steps].numOutputs = out;
        circut[steps].numSelectors = selectors;
        steps++;
    }
    steps--;
    /*END BUILD CIRCUT*/

    /*BEGIN CREATE TRUTH TABLE*/
    int rows = (int) pow(2,notConstant);
    for (int i=0; i<rows; i++) {
        
        int k = numInputs-1;
        for (int j=0; j<numInputs; j++) {
            if (inputs[j] != 0 && inputs[j] != 1) // if input is 0 or 1, it should remain constant
                variables[inputs[j]].value = (i/(int)pow(2, k))%2;
            k--; }
        
        if (i != 0)
            printf("\n");
        printInputs(inputs, numInputs);
        
        for (int s=0; s<steps; s++) {
            switch (circut[s].kind) {
                case AND: and(circut[s]); break;
                case OR: or(circut[s]); break;
                case NOR: nor(circut[s]); break;
                case XOR: xor(circut[s]); break;
                case NAND: nand(circut[s]); break;
                case PASS: pass(circut[s]); break;
                case NOT: not(circut[s]); break;
                case MULTIPLEXER: multi(circut[s]); break;
                case DECODER: deco(circut[s]); break;
            }
        }
        printOutputs(outputs, numOutputs);
       
   }
    printf("\n");
    
    free(inputs);
    free(outputs);
    free(sels);
    for (int s=0; s<steps; s++) {
        free(circut[s].inputIndices);
        free(circut[s].outputIndices);
        free(circut[s].selectorIndices);
    }
    free(circut);
    for (int v=0; v<nextIndex; v++) {
        free(variables[v].name);
    }
    free(variables);
    return 0;
}
