#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#define MAX 30

typedef struct v{
    char name[10];      // name of the car
    int x, y;           // x and y coordinates of the car
    int key;            // index of the car in the graph
    struct v* next;
}car;

typedef struct{
    char n[10];         // name of the car
    int a, b;           // x and y coordinates of the car
    int index;          // index of the car in the graph
}ver;

typedef struct{
    ver src;            // the source of the edge
    ver dest;           // the destination of the edge
    double weight;      // the distance between the source and the destination
}edge;


car* g[MAX];
edge e[MAX];
edge mst[MAX];
int path[MAX];
double dis[MAX];
int visited[MAX] = {0};
int N, R, k = -1;

void menu(void);
void addEdges(int i, int j);
void createEdge(car* veh1, car* veh2, double w);
void addVehicle(int k2);
int emptyGraph(void);
int find(char* veh);
void insert(car* h, ver newnode, double d);
void shiftEdges(int i);
void shiftVertices(int i);
void deleteNode(car* h, char* vname);
void dfs(car* s);
void bfs(car* s);
int isEmpty(car* q[]);
void minSpan(int n, int s);
void sort(int n);
void swap(edge* A, edge* B);
void shortPath(int s, int f);
void displayPath(int s, int f);
double distance(int i, int j);

void main(){
    FILE *ptr;
    int i, X, Y, k2, n = 0;
    int choice, flag, key1, key2;
    double d;
    char vname[10], veh1[10], veh2[10];
    car* new_, *w;

    ptr = fopen("data.txt", "r");
    fscanf(ptr, "%d", &N);
    fscanf(ptr, "%d", &R);
    //Fill in the head pointers (vehicles) in the graph
    for(i = 0; i < N; i++){
        new_ = (car*)malloc(sizeof(car));
        fscanf(ptr, "%s", new_->name);
        fscanf(ptr, "%d", &new_->x);
        fscanf(ptr, "%d", &new_->y);
        new_->key = i;
        new_->next = NULL;
        g[i] = new_;
    }
    //Create the array of edges with no duplicates
    for(i = 0; i < N; i++){
        addEdges(i, i+1);
    }
    //Create the sorted adjacency lists for each vehicle in the graph
    addVehicle(0);

    do{
        for(i = 0; i < MAX; i++){
            visited[i] = 0;
        }
        flag = 0;
        n = 0;
        menu();
        scanf("%d", &choice);
        if(choice == 1){                //Display all edges
            if(emptyGraph() == 0){      //If the graph is not empty
                printf("The edges of the graph are: \n");
                for(i = 0; i <= k; i++){
                    printf("(%s, %s, %.2f)\n",e[i].src.n,e[i].dest.n,e[i].weight);
                }
            }
            else{
                printf("\nThere are no edges in this graph!\n");
            }
        }
        else if(choice == 2){           //Display adjacency list of a vertex
            printf("Please enter the name of the vehicle: ");
            scanf("%s", vname);
            key1 = find(vname);
            if(key1 != -1){
                if(g[key1]->next == NULL){
                    printf("\nThe vehicle entered is not adjacent to any other vehicle!!\n");
                }
                else{
                    printf("\nThe names of the adjacent vehicles to %s are: ", vname);
                    for(w = g[key1]->next; w != NULL; w = w->next){
                        printf(" %s  ", w->name);
                    }
                    printf("\n");
                }
            }
            else{                       //To check whether the node entered exists or not
                printf("\nThe vehicle entered does not exist!\n");
            }
        }
        else if(choice == 3){           //Add a new vertex
            printf("Please enter the name, the x coordinate, and the y coordinate of the new vehicle: ");
            scanf("%s %d %d", vname, &X, &Y);
            key1 = find(vname);
            if(key1 != -1){
                printf("\nThe name of the vehicle entered already exists! Vehicles cannot have the same names!\n");
            }
            else{
                new_ = (car*)malloc(sizeof(car));
                strcpy(new_->name, vname);
                new_->x = X;
                new_->y = Y;
                new_->next = NULL;
                new_->key = N;
                g[N] = new_;
                k2 = k;             //We copied k because it will be modified once the new edges are added and we will need its previous value in function addVehicle
                addEdges(N, 0);
                N++;
                addVehicle(k2+1);
                printf("\nThe vehicle was added successfully.\n");
            }
        }
        else if(choice == 4){       //Delete a vehicle
            printf("Please enter the name of the vehicle to be deleted: ");
            scanf("%s", vname);
            key1 = find(vname);
            if(key1 != -1){
                //To delete the corresponding edges
                for(i = 0; i <= k; i++){
                    if(strcmp(e[i].src.n, vname) == 0 || strcmp(e[i].dest.n, vname) == 0){
                        shiftEdges(i);
                        i--;
                    }
                }
                //To delete the vertex from the graph
                g[key1]->next = NULL;
                shiftVertices(key1);
                //To delete the vertex from the adjacency lists
                for(i = 0; i < N; i++){
                    deleteNode(g[i], vname);
                }
                printf("\nThe vehicle was deleted successfully.\n");
            }
            else{
                printf("\nThe vehicle entered does not exist!\n");
            }
        }
        else if(choice == 5){       //Add an Edge
            printf("Please enter the names of the two vehicles to be connected: ");
            scanf("%s %s", veh1, veh2);
            key1 = find(veh1);
            key2 = find(veh2);
            if(key1 == -1 || key2 == -1){
                printf("\nThe vehicle(s) entered do(es) not exist!\n");
            }
            else if(key1 == key2){
                printf("\nAn edge cannot be added between a vehicle and itself!\n");
            }
            else{
                //To check whether the vehicles are connected or not
                for(i = 0; i <= k; i++){
                    if((strcmp(e[i].src.n, veh1) == 0 && strcmp(e[i].dest.n, veh2) == 0) || (strcmp(e[i].src.n, veh2) == 0 && strcmp(e[i].dest.n, veh1) == 0)){
                        flag = 1;
                        printf("\nThese vehicles are already connected!!\n");
                        break;
                    }
                }
                //If the vehicles are not connected
                if(flag != 1){
                    d = distance(key1, key2);
                    createEdge(g[key1], g[key2], d);
                    insert(g[key1], e[k].dest, d);
                    insert(g[key2], e[k].src, d);
                    printf("\nThe edge was added successfully.\n");
                }
            }
        }
        else if(choice == 6){       //Delete an Edge
            printf("Please enter the names of the two vehicles connected by the edge to be deleted: ");
            scanf("%s %s", veh1, veh2);
            key1 = find(veh1);
            key2 = find(veh2);
            if(key1 == -1 || key2 == -1){
                printf("\nThe vehicle(s) entered do(es) not exist!\n");
            }
            //If the vehicles exist
            else{
                //To delete the edge from the array of edges, if it exists
                for(i = 0; i <= k; i++){
                    if((strcmp(e[i].src.n, veh1) == 0 && strcmp(e[i].dest.n, veh2) == 0) || (strcmp(e[i].src.n, veh2) == 0 && strcmp(e[i].dest.n, veh1) == 0)){
                        flag = 1;
                        shiftEdges(i);
                        break;
                    }
                }
                if(flag == 0){
                    if(strcmp(veh1, veh2) == 0){
                        printf("\nThere is no edge between a vehicle and itself!!\n");
                    }
                    else{
                        printf("\nThese vehicles are originally disconnected!!\n");
                    }
                }
                //To delete the vehicles from each other's adjacency list (if the edge exists)
                else{
                    deleteNode(g[key1], veh2);
                    deleteNode(g[key2], veh1);
                    printf("\nThe edge was deleted successfully.\n");
                }
            }
        }
        else if(choice == 7 || choice == 8){           //DFS or BFS
            printf("Please enter the name of the starting vehicle: ");
            scanf("%s", vname);
            key1 = find(vname);
            if(key1 != -1){
                if(g[key1]->next == NULL){
                    printf("\nYou cannot reach any vehicle from %s.\n", vname);
                }
                else if(choice == 7){
                    printf("\nThe reachable vehicles from the vehicle %s are: ", vname);
                    dfs(g[key1]);
                }
                else if(choice == 8){
                    printf("\nThe reachable vehicles from the vehicle %s are: ", vname);
                    bfs(g[key1]);
                }
                printf("\n");
            }
            else{
                printf("\nThe vehicle entered does not exist!\n");
            }
        }
        else if(choice == 9){       //Minimal spanning tree
            printf("Please enter the name of the starting vehicle: ");
            scanf("%s", vname);
            key1 = find(vname);
            if(key1 != -1){
                if(g[key1]->next == NULL){
                    printf("\nThis vehicle is not connected to any other vehicle! No minimal spanning tree can be created starting %s!\n", vname);
                }
                else{
                    //Because a spanning tree works only for connected graphs
                    //we should exclude the vertices that are not connected or reachable from this vertex (in case the graph is not connected)
                    for(i = 0; i < N; i++){
                        if(g[i]->next != NULL){
                            n++;
                        }
                    }
                    minSpan(n,key1);            ////We pass the number of vertices reachable from the starting vertex and the index of the starting vehicle
                    printf("\nThe edges of the minimal spanning of the graph are:\n");
                    for(i = 0; i < n-1; i++){
                        printf("(%s %s %.2f)  ", mst[i].src.n, mst[i].dest.n, mst[i].weight);
                    }
                }
            }
            else{
                printf("\nThe vehicle entered does not exist!\n");
            }
        }
        else if(choice == 10){      //Shortest Path
            printf("Please enter the name of the starting vehicle and the name of the end vehicle: ");
            scanf("%s %s", veh1, veh2);
            key1 = find(veh1);
            key2 = find(veh2);
            if(key1 == -1 || key2 == -1){
                printf("\nThe vehicle(s) entered do(es) not exist!\n");
            }
            else if(key1 == key2){
                printf("\nYou entered the same vehicle!\nNo shortest path is found\nThe distance between %s and %s is: 0\n", veh1, veh2);
            }
            //If the vehicles exist:
            else{
                if(g[key1]->next == NULL || g[key2]->next == NULL){
                    flag = 1;
                    printf("\nThere is no path between the vehicles entered!!\n");
                }
                if(flag != 1){
                    shortPath(key1, key2);
                    printf("\nThe shortest path from the vehicle %s to the vehicle %s is: ",veh1, veh2);
                    displayPath(key1, key2);
                    printf("\nThe minimum distance between %s and %s is: %.2f\n",veh1, veh2, dis[key2]);
                }
            }
        }
        else if(choice == 11){
            printf("\nEnd of Program.\n");
        }
        else if(choice != 11){
            printf("\nInvalid Choice!!\n");
        }
    }while(choice != 11);
    fclose(ptr);
}

//This function takes as arguments the index of the starting vehicle and the index of the possible destination and forms an edge if possible
void addEdges(int i, int j){
    double d;
    while(j < N){
        d = distance(i, j);
        if(d <= R){
            createEdge(g[i], g[j], d);
        }
        j++;
    }
}

//This function takes the source, the destination and the weight of an edge and creates it in the array of edges
void createEdge(car* veh1, car* veh2, double w){
    k++;
    strcpy(e[k].src.n, veh1->name);
    e[k].src.a = veh1->x;
    e[k].src.b = veh1->y;
    e[k].src.index = veh1->key;
    strcpy(e[k].dest.n, veh2->name);
    e[k].dest.a = veh2->x;
    e[k].dest.b = veh2->y;
    e[k].dest.index = veh2->key;
    e[k].weight = w;
}

//This function takes as arguments a head pointer, a new node and the distance between them and inserts the new node following an increasing order of distances
void insert(car* h, ver newnode, double d){
    car* w;
    car* new_;
    double D;
    for(w = h; w->next != NULL; w = w->next){
        D = distance(h->key, w->next->key);
        if(d < D){
            new_ = (car*)malloc(sizeof(car));
            strcpy(new_->name, newnode.n);
            new_->x = newnode.a;
            new_->y = newnode.b;
            new_->key = newnode.index;
            new_->next = w->next;
            w->next = new_;
            return;
        }
    }
    if(w->next == NULL){
        new_ = (car*)malloc(sizeof(car));
        strcpy(new_->name, newnode.n);
        new_->x = newnode.a;
        new_->y = newnode.b;
        new_->key = newnode.index;
        new_->next = NULL;
        w->next = new_;
        return;
    }
}

//This function takes as argument an index of the array of edges from which it starts looking for adjacencies of each vertex
void addVehicle(int s){
    int i, j;
    for(i = 0; i < N; i++){
        for(j = s; j <= k; j++){
            if(strcmp(g[i]->name,e[j].src.n) == 0){
                insert(g[i], e[j].dest, e[j].weight);
            }
            else if(strcmp(g[i]->name,e[j].dest.n) == 0){
                insert(g[i], e[j].src, e[j].weight);
            }
        }
    }
}

//This function checks if the graph is empty or not; empty means has no edges
int emptyGraph(void){
    if(k == -1){
        return 1;
    }
    return 0;
}

//This function takes the name of a vehicle and checks whether it exists in the graph or not, it returns the index if the vehicle found, otherwise -1
int find(char* veh){
    int i;
    for(i = 0; i < N; i++){
        if(strcmp(g[i]->name, veh) == 0){
            return i;
        }
    }
    return -1;
}

//This function takes as argument the index of an edge that was deleted and it shifts up all the edges below
void shiftEdges(int i){
    int j;
    if(i != k){
        for(j = i; j <= k; j++){
            e[j] = e[j+1];
        }
    }
    k--;
}

//This function takes as argument the index of a vertex that was deleted and it shifts up all the edges below
void shiftVertices(int i){
    int j;
    if(i != N-1){
        for(j = i; j < N; j++){
            g[j] = g[j+1];
        }
    }
    N--;
}

//This function takes as argument a head pointer and the name of a vehicle to be deleted from the adjacency list
void deleteNode(car* h, char* vname){
    car* w, *temp;
    for(w = h; w->next != NULL; w = w->next){
        if(strcmp(w->next->name, vname) == 0){
            temp = w->next;
            w->next = temp->next;
            temp->next = NULL;
            free(temp);
            return;
        }
    }
}

//This function takes the starting vehicle and displays all the reachable vehicles (using DFS)
void dfs(car* s){
    car* w;
    printf("%s ", s->name);
    visited[s->key] = 1;
    for(w = s->next; w != NULL; w = w->next){
        if(visited[w->key] == 0){
            dfs(g[w->key]);
        }
    }
}

//This function takes the starting vehicle and displays all the reachable vehicles (using BFS)
void bfs(car* s){
    car* w, *temp;
    int i, j = 1;
    car* queue[MAX];
    for(i = 0; i < N; i++){
        queue[i] = NULL;
    }
    printf("%s ", s->name);
    queue[0] = s;
    visited[s->key] = 1;
    i = 0;
    while(isEmpty(queue) == 0){
        temp = queue[i];
        queue[i] = NULL;
        for(w = g[temp->key]; w != NULL; w = w->next){
            if(visited[w->key] == 0){
                printf("%s ", w->name);
                queue[j++] = w;
                visited[w->key] = 1;
            }
        }
        i++;
    }
}

//This function checks if the queue is empty or not, if empty it returns 1, otherwise it returns 0
int isEmpty(car* q[]){
    int i;
    for(i = 0; i < N; i++){
        if(q[i] != NULL){
            return 0;
        }
    }
    return 1;
}

//This function takes the number of vertices connected to other vehicles and the starting vehicle, and it creates a minimal spanning tree
void minSpan(int n, int s){
    int count = 0, i, key, j = 0;
    double min;
    ver T;
    //We copy the starting vehicle and the vehicle closer and adjacent to it to the mst array
    strcpy(T.n, g[s]->name);
    T.a = g[s]->x;
    T.b = g[s]->y;
    T.index = g[s]->key;
    mst[j].src = T;
    strcpy(T.n, g[s]->next->name);
    T.a = g[s]->next->x;
    T.b = g[s]->next->y;
    T.index = g[s]->next->key;
    mst[j].dest = T;
    mst[j].weight = distance(s, g[s]->next->key);
    //We set the two vehicles to visited
    visited[g[s]->key] = 1;
    visited[g[s]->next->key] = 1;
    count++;
    while(count < n-1){
        min = INT_MAX;        //We set min to INT_MAX and not to the range because an edge may has been added
        for(i = 0; i <= k; i++){
            //This if statement is to avoid cycles
            if((visited[e[i].src.index] == 1 && visited[e[i].dest.index] == 0) || (visited[e[i].src.index] == 0 && visited[e[i].dest.index] == 1)){
                if(e[i].weight <= min){
                    min = e[i].weight;
                    key = i;            //key holds the index of the edge with minimum weight
                }
            }
        }
        visited[e[key].src.index] = 1;
        visited[e[key].dest.index] = 1;
        mst[++j] = e[key];
        count++;
    }
    sort(n);
}

//This function takes the size of the minimal spanning tree and sorts it in increasing order of distances
void sort(int n){
    int i, j;
    for(i = 0; i < n-1; i++){
        for(j = i+1; j < n-1; j++){
            if(mst[i].weight > mst[j].weight){
                swap(&mst[i], &mst[j]);
            }
        }
    }
}

void swap(edge* A, edge* B){
    edge temp;
    temp = *A;
    *A = *B;
    *B = temp;
}

//This function finds the shortest path between two vehicles, it takes the indices of the starting and ending vehicles
void shortPath(int s, int f){
    int flag;
    int key;
    double min;
    car* w;
    int i;
    //Phase 1 : Initialization
    visited[s] = 1;
    path[s] = -1;       //No predecessor
    dis[s] = 0;
    //This look initializes all the vertices of the graph according to their position
    for(i = 0; i < N; i++){
        flag = 0;
        if(i == s){
            i++;
        }
        //This loop deals with the adjacent vertices to the starting vehicle
        for(w = g[s]; w != NULL; w = w->next){
            if(i == w->key){
                flag = 1;
                dis[i] = distance(s,i);
                path[w->key] = s;
            }
        }
        //Here we deal with the vertices that are not adjacent to the starting vehicle
        if(flag == 0){
            dis[i] = (double)INT_MAX;     //We consider INT_MAX to be infinity for integers
            path[i] = -1;                 //No predecessor
        }
    }
    //Phase 2 : Iteration
    do{
        min = (double)INT_MAX;          //We did not set min to the range because an edge may has been added
        for(i = 0; i < N; i++){
            if(visited[i] == 0 && dis[i] < min){
                min = dis[i];
                key = i;            //key holds the index of the vehicle with minimum distance from the start
            }
        }
        visited[key] = 1;
        for(w = g[key]; w != NULL; w = w->next){
            if(visited[w->key] == 0 && dis[key] + distance(key,w->key) < dis[w->key]){
                dis[w->key] = dis[key] + distance(key,w->key);
                path[w->key] = key;
            }
        }
    }while(visited[f] == 0);
}

//This function displays the shortest path recursively
void displayPath(int s, int f){
    if(f == s){
        return;
    }
    else{
        displayPath(s, path[f]);
        printf("(%s %s %.2f) ", g[path[f]]->name, g[f]->name, distance(f, path[f]));
    }
}

//This function calculates the distance between the two vertices of indices i and j in the graph
double distance(int i, int j){
    double d;
    int x1 = g[i]->x, x2 = g[j]->x;
    int y1 = g[i]->y, y2 = g[j]->y;
    d = sqrt(pow((x1-x2),2) + pow((y1-y2),2));
    return d;
}

void menu(void){
    printf("\n");
    for(int i = 0; i < 100; i++){
        printf("*");
    }
    printf("\nPlease choose an option:\n1- Display all Edges\n2- Display the vehicles adjacent to a vehicle\n3- Add a vehicle\n4- Delete a vehicle\n5- Add an edge\n6- Delete an edge\n7- DFS\n8- BFS\n9- Minimum Spanning Tree\n10- Shortest path between two vehicles\n11- Quit\n");
    for(int i = 0; i < 100; i++){
        printf("*");
    }
    printf("\nPlease enter your choice here==> ");
}
