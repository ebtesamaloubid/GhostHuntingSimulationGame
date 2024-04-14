#include "defs.h"
/*
    Moves the ghost to a random connected room.
    in: ghost - pointer to the Ghost structure.
    Uses semaphores to handle concurrency in room access.
    The ghost will not move if there are no connected rooms or if the next room is the same as the current one.
*/
void moveGhostToRandomRoom(Ghost *ghost) {
    Room *currentRoom = ghost->currentRoom;
    int numConnected = countConnectedRooms(currentRoom);
    if (numConnected == 0) return; // Stay if no connected rooms

    int randomIndex = randInt(0, numConnected + 1);

    Room *nextRoom = getConnectedRoomAtIndex(currentRoom, randomIndex);

    if (nextRoom == NULL || nextRoom == currentRoom) return; // Do not move if the selected room is the current room or NULL
    

    sem_wait(&currentRoom->roomSem);
    sem_wait(&nextRoom->roomSem);

    currentRoom->ghost = NULL; // Remove ghost from current room
    ghost->currentRoom = nextRoom; // Move ghost to the next room
    nextRoom->ghost = ghost; // Set ghost in the next room

    l_ghostMove(nextRoom->name);

    sem_post(&currentRoom->roomSem);
    sem_post(&nextRoom->roomSem);
}

/*
    Updates the ghost's boredom level.
    in: ghost - pointer to the Ghost structure.
    Resets the boredom timer if a hunter is present in the room, otherwise increments it.
    If boredom reaches a maximum threshold, logs ghost exit due to boredom and ends the ghost thread.
*/
void updateGhostBoredom(Ghost *ghost) {
    if (isHunterInRoom(ghost->currentRoom)) {
        ghost->boredomTimer = 0;
    } else {
        ghost->boredomTimer++;
        if (ghost->boredomTimer >= BOREDOM_MAX) {
            l_ghostExit(LOG_BORED); // Log ghost exit due to boredom
            pthread_exit(NULL); // End ghost thread
        }
    }
}
/*
    Performs a random action for the ghost.
    in: ghost - pointer to the Ghost structure.
    Chooses an action randomly (do nothing, leave evidence, or move).
    Always calls updateGhostBoredom at the end to update the ghost's boredom level.
*/
void performGhostAction(Ghost *ghost) {

    int action =0;

    //printf("hunter with ghost is %s",ghost->currentRoom->hunters->head->hunter->name);

    if (ghost->currentRoom->hunters->head == NULL){
        action = randInt(0, 3); // 0: Do nothing, 1: Leave evidence, 2: Move
    }
    else{
        action = randInt(0,2);
    }

    //printf("\n%d is the randint", action);
    

    switch (action) {
        case 1: leaveEvidence(ghost); break;
        case 2: moveGhostToRandomRoom(ghost); break;
        default: break; // Do nothing
    }
    updateGhostBoredom(ghost); // Always update boredom
}
/*
    Initializes a ghost and assigns it to a random room.
    in/out: ghost - pointer to a pointer of the Ghost structure. Modified to point to the allocated Ghost.
    in: house - the house in which the ghost is being initialized.
    Allocates memory for the ghost and sets its initial properties, including its type and starting room.
    Logs ghost initialization.
*/
void initGhost(Ghost **ghost, HouseType house) {
    int room = randInt(0, 11);
    char roomName[MAX_STR]; // Local array to store room names
    
  //  printf("%d \n", room);
    // Assign room names based on the random room number
    switch (room) {
        case 0:  strncpy(roomName, "Hallway", MAX_STR); break;
        case 1:  strncpy(roomName, "Master Bedroom", MAX_STR); break;
        case 2:  strncpy(roomName, "Boy's Bedroom", MAX_STR); break;
        case 3:  strncpy(roomName, "Bathroom", MAX_STR); break;
        case 4:  strncpy(roomName, "Basement", MAX_STR); break;
        case 5:  strncpy(roomName, "Basement Hallway", MAX_STR); break;
        case 6:  strncpy(roomName, "Right Storage Room", MAX_STR); break;
        case 7:  strncpy(roomName, "Left Storage Room", MAX_STR); break;
        case 8:  strncpy(roomName, "Kitchen", MAX_STR); break;
        case 9:  strncpy(roomName, "Living Room", MAX_STR); break;
        case 10: strncpy(roomName, "Garage", MAX_STR); break;
        default: strncpy(roomName, "Utility Room", MAX_STR); break;
    }
   // printf("%s \n", roomName);

    Room* startRoom = getRoom(roomName, house);
   // printf("%s \n", startRoom->name);

    // Allocate memory for the ghost
    *ghost = malloc(sizeof(Ghost));
    if (*ghost == NULL) return; // Check for allocation failure

    (*ghost)->ghostType = randomGhost(); // Assign a random ghost type
    //printf( "got assigned: %d",(*ghost)->ghostType);
    (*ghost)->currentRoom = startRoom;   // Set the starting room
    (*ghost)->boredomTimer = 0;          // Initialize boredom timer

    // Log ghost initialization
    l_ghostInit((*ghost)->ghostType, startRoom->name);
}
/*
    Frees the memory allocated for a ghost.
    in: ghost - pointer to the Ghost structure to be freed.
    Performs a NULL check before attempting to free to avoid freeing a NULL pointer.
*/
void freeGhost(Ghost *ghost){

     if (!ghost) return

     free(ghost);
}