#include "defs.h"
int main() {
    // Initialize the random number generator
    srand(time(NULL));
    Room* van;
    // Initialize the house and populate rooms
    HouseType* house = NULL;
    initHouse(&house);
    populateRooms(house);

    char hunter1[MAX_STR], hunter2[MAX_STR], hunter3[MAX_STR], hunter4[MAX_STR];
    char evidenceName[MAX_STR];
    GhostClass result;
    int equipmentArray[] = {0,1,2,3};
    int equipmentIndex =0;
    EvidenceType filteredEquipmentArray[] = {-1, -1, -1};
    Hunter* h1, *h2, *h3, *h4;
    HunterGhostPair* arr[4];
    HunterNode* curHunter;
    EvidenceNode* curEvidence;
    pthread_t ghostThread;
    pthread_t hunterThreads[NUM_HUNTERS];
    int huntersThatLeft =0;

    // Input names for hunters
    printf("Enter p1's name: ");
    scanf("%63s", hunter1);
    printf("Enter p2's name: ");
    scanf("%63s", hunter2);
    printf("Enter p3's name: ");
    scanf("%63s", hunter3);
    printf("Enter p4's name: ");
    scanf("%63s", hunter4);

    // Get starting room for hunters and ghost
    char roomName[MAX_STR] = "Van";
    van = getRoom(roomName, *house);

    // Initialize hunters
    h1 = HunterInit(hunter1, van, house);
    h2 = HunterInit(hunter2, van, house);
    h3 = HunterInit(hunter3, van, house);
    h4 = HunterInit(hunter4, van, house);

    // Create and assign hunter nodes to the room's hunter list
    HunterNode* n1 = createHunterNode(h1);
    HunterNode* n2 = createHunterNode(h2);
    HunterNode* n3 = createHunterNode(h3);
    HunterNode* n4 = createHunterNode(h4);

    van->hunters->head = n1;
    n1->next = n2;
    n2->next = n3;
    n3->next = n4;
    van->hunters->tail = n4;

    house->hunters->head = n1;
    house->hunters->tail = n4;

   curHunter = house->hunters->head;


    // Initialize ghost
    Ghost* ghost;
    initGhost(&ghost, *house);
   
    // Create ghost thread
    if (pthread_create(&ghostThread, NULL, ghostThreadFunction, (void *)ghost) != 0) {
        perror("Failed to create ghost thread");
        exit(EXIT_FAILURE);
    }

   

    Hunter* hunterPointers[NUM_HUNTERS] = {h1, h2, h3, h4};
    // Create hunter threads

for (int i = 0; i < NUM_HUNTERS; i++) {

    HunterGhostPair *pair = malloc(sizeof(HunterGhostPair));
     
    
    if (!pair) {
        perror("Failed to allocate memory for HunterGhostPair");
        exit(EXIT_FAILURE);
    }

    pair->hunter = hunterPointers[i];
    pair->ghost = ghost;
    arr[i] = pair;

    if (pthread_create(&hunterThreads[i], NULL, hunterThreadFunction, (void *)arr[i]) != 0) {
        perror("Failed to create hunter thread");
        exit(EXIT_FAILURE);
    }
}

    // Joining threads
    for (int i = 0; i < NUM_HUNTERS; i++) {
        pthread_join(hunterThreads[i], NULL);
         
    }
    pthread_join(ghostThread, NULL);
  printf("------------------------------------------------------\n");

    while(curHunter != NULL){

        
        huntersThatLeft = reasonForExiting(curHunter->hunter, huntersThatLeft);
        curHunter = curHunter->next;
    }

    if (huntersThatLeft >= NUM_HUNTERS){
        printf("\nTHE GHOST HAS WON\n\n");
    }
    if (huntersThatLeft ==0 ){
         printf("\nTHE HUNTERS HAVE WON\n\n");
    }
    
    curEvidence = house->sharedEvidence->head;
   

    printf("Here is the evidence the hunters found: \n\n");

    while (curEvidence != NULL){
        
        
        evidenceToString(curEvidence->evidence, evidenceName);

        for (int i =0; i < 4; i ++){

            if (equipmentArray[i] == (int)curEvidence->evidence){
            equipmentArray[i]= -1;
            filteredEquipmentArray[equipmentIndex] = curEvidence->evidence;
            equipmentIndex ++;
            printf("%s\n", evidenceName);
    
        }  

    }
    curEvidence = curEvidence->next;

    }

    result = matchEvidence(filteredEquipmentArray);
     ghostToString(ghost->ghostType, evidenceName);
     printf("\nThe ghost was a %s\n" , evidenceName);

    if (result == ghost->ghostType){
        printf("The hunters successfully identified the ghost using the pieces of evidence\n");
    }

    
    return 0;

    freeGhost(ghost);
    freeHouse(house);
}
/*
    Determines if a hunter should exit the game based on their fear or boredom levels.
    in: hunter - pointer to the Hunter structure.
    in/out: numLeft - number of hunters that have already exited.
    return: Updated number of hunters that have exited.
*/

int reasonForExiting(Hunter* hunter, int numLeft){

   // char* reason = NULL;
    if (hunter->fear >= FEAR_MAX){
        printf("Hunter %s exited because of FEAR\n", hunter->name);
        numLeft ++;
    }
    else if (hunter->boredom >= BOREDOM_MAX){
    printf("Hunter %s exited because of BOREDOM\n", hunter->name);
    numLeft ++;
    }

   
    return numLeft;

}

/*
    Thread function for hunters.
    in: arg - pointer to a HunterGhostPair structure.
    Performs actions for the hunter and manages interactions with the ghost.
    return: NULL pointer.
*/
void *hunterThreadFunction(void *arg) {
    HunterGhostPair *pair = (HunterGhostPair *)arg;
    Hunter *hunter = pair->hunter;
    Ghost *ghost = pair->ghost;

    for (int i = 0; i < MAX_RUNS; i++) {
        i =0;
        if (i == MAX_RUNS -1){

            printf("MAX RUNS REACHED\n");
        }
        performHunterAction(hunter, ghost); // Use both hunter and ghost
        usleep(HUNTER_WAIT);
    }

   // free(pair); // Free the memory allocated for the pair
    return NULL;
}
/*
    Thread function for the ghost.
    in: arg - pointer to a Ghost structure.
    Manages ghost actions within the game environment.
    return: NULL pointer.
*/
void *ghostThreadFunction(void *arg) {
    Ghost *ghost = (Ghost *)arg;

    // Main loop for the ghost's actions
    for (int i = 0; i < MAX_RUNS; i++) {
        i =0;
        performGhostAction(ghost);
        usleep(GHOST_WAIT); // Wait for a specified time
    }

    return NULL;
}

