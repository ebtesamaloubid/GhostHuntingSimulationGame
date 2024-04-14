#include "defs.h"
/*
    Initializes a Hunter structure with given parameters.
    in: hunter - name of the hunter.
    in: room - starting room of the hunter.
    in: house - house in which the hunter is located.
    Allocates memory for a new Hunter and sets its initial properties.
    return: pointer to the newly created Hunter structure. Returns NULL on error.
*/
Hunter* HunterInit(char* hunter, Room* room, HouseType* house) {
    if (!hunter || !room || !house || !house->sharedEvidence) {
    
        return NULL;
    }
    // Handle memory allocation failure
    Hunter* hunter1 = malloc(sizeof(Hunter));
    if (!hunter1) {
        
        return NULL;
    }

    strcpy(hunter1->name, hunter);
    hunter1->currentRoom = room;
    hunter1->evidenceType = getEquipment(house);
    hunter1->sharedEvidence = house->sharedEvidence;
    hunter1->fear = 0;
    hunter1->boredom = 0;

    l_hunterInit(hunter, hunter1->evidenceType);

    return hunter1;
}

/*
    Randomly selects an equipment for the hunter.
    in: house - pointer to the HouseType structure.
    Randomly selects an equipment that has not been selected yet.
    return: integer representing the selected equipment.
*/
int getEquipment(HouseType *house){

    int randomInt=0;
    int bln =-1;
    
    randomInt= randInt(0,4);

    while(bln -1){

    for (int i =0; i < 4; i ++){
     if (house->arr[i] == randomInt){
     house->arr[i] = -1;
   
        return randomInt;}
    }

    randomInt = randInt(0,4);
    }
    return 0; 

}
/*
    Checks if there is a hunter in the room.
    in: room - pointer to the Room structure.
    return: 1 if there is a hunter in the room, 0 otherwise.
*/

int isHunterInRoom(Room *room) {

    if (room->hunters->head == NULL){
        return 0;
    }
    
    return 1; // No hunter in the room
}
/*
    Checks hunter's fear and boredom levels and exits if necessary.
    in: hunter - pointer to the Hunter structure.
    If fear or boredom reaches a maximum threshold, logs hunter exit and ends the hunter thread.
*/
void checkAndExitHunter(Hunter *hunter) {
    if (hunter->fear >= FEAR_MAX || hunter->boredom >= BOREDOM_MAX) {
        int reason;
         if ( hunter->fear >=FEAR_MAX){
            reason = LOG_FEAR;
         }
         else reason = LOG_BORED;
        l_hunterExit(hunter->name, reason);
        pthread_exit(NULL);
    }
}
/*
    Performs a random action for the hunter.
    in: hunter - pointer to the Hunter structure.
    in: ghost - pointer to the Ghost structure.
    Randomly selects and performs an action (move, collect evidence, review evidence, check exit).
*/
void performHunterAction(Hunter *hunter, Ghost *ghost) {
    int action = randInt(0, 4); // 0: Move, 1: Collect Evidence, 2: Review Evidence, 3: Check Exit
   
    switch (action) {
        
        case 0: 
            moveToRandomConnectedRoom(hunter); 
        
            
            break;
        case 1: 

            
            collectEvidenceIfAvailable(hunter); 
            break;
        case 2: 
            reviewCollectedEvidence(hunter); 
            break;
        case 3: 
            checkAndExitHunter(hunter); 
            break;
    }
    // Update fear and boredom
// call it in the main
   updateFearAndBoredom(hunter, ghost); // This function should manage fear and boredom levels
}
/*
    Creates a node for a given hunter.
    in: hunter - pointer to the Hunter structure.
    return: pointer to the newly created HunterNode structure. Returns NULL if allocation fails.
*/
HunterNode* createHunterNode( Hunter* hunter ){

    HunterNode* newNode = malloc(sizeof(HunterNode));
    newNode->hunter = hunter;
    newNode->next = NULL;
    return newNode;

}
/*
    Initializes a list of hunters.
    return: pointer to the initialized HunterList structure. Returns NULL if allocation fails.
*/
HunterList* initHunterList(){
    HunterList* newList = malloc(sizeof(HunterList));
    newList->head = NULL;
    newList->tail = NULL;
    return newList;

}
/*
    Updates the fear and boredom levels of a hunter.
    in: hunter - pointer to the Hunter structure.
    in: ghost - pointer to the Ghost structure.
    Increases fear if in the same room as the ghost, and boredom otherwise. Exits the thread if levels are too high.
*/
void updateFearAndBoredom(Hunter *hunter, Ghost *ghost) {
    // Check if the hunter is in the same room as the ghost
    if (hunter->currentRoom == ghost->currentRoom) {
        // Increase fear if in the same room as the ghost
        hunter->fear++;
        // Reset boredom to 0
        hunter->boredom = 0;
    } else {
        // Increase boredom if not in the same room as the ghost
        hunter->boredom++;
    }

    // Check if fear or boredom levels have reached their maximum
    if (hunter->fear >= FEAR_MAX || hunter->boredom >= BOREDOM_MAX) {
        // Log and exit if necessary
        enum LoggerDetails exitReason = (hunter->fear >= FEAR_MAX) ? LOG_FEAR : LOG_BORED;
        l_hunterExit(hunter->name, exitReason);
        pthread_exit(NULL);
    }
}
/*
    Frees all resources associated with a hunter list.
    in: list - pointer to the HunterList structure to be freed.
    Frees all hunters in the list and then the list itself.
*/
void freeHunterList(HunterList* list) {
    if (!list) return;

    HunterNode* current = list->head;
    while (current != NULL) {
        HunterNode* temp = current;
        current = current->next;

        freeHunter(temp->hunter); // Free the Hunter object
        free(temp); // Free the HunterNode
    }

    free(list);
}
/*
    Frees all resources associated with a hunter.
    in: hunter - pointer to the Hunter structure to be freed.
    Performs a NULL check before attempting to free to avoid freeing a NULL pointer.
*/
void freeHunter(Hunter* hunter) {
    if (!hunter) return; // Always check for NULL

    
    free(hunter);
}

