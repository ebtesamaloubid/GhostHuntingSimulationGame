#include "defs.h"
/*
    Counts the number of rooms connected to a given room.
    in: room - pointer to the Room structure.
    return: count of connected rooms. Returns 0 if room or its connectedRooms is NULL.
*/
int countConnectedRooms(Room *room) {
    //printf(" %s is there  a room \n", room->name);
    if (!room || !room->connectedRooms) {
        // If the room pointer is NULL or the room's connectedRooms pointer is NULL, return 0
        return 0;
    }

    int count = 0;
    RoomNode *current = room->connectedRooms->head;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}
/*
    Returns a pointer to a connected room at a specified index.
    in: room - pointer to the Room structure.
    in: index - index of the connected room to return.
    return: pointer to the Room structure at the given index. Returns NULL if index is invalid or room is NULL.
*/
Room *getConnectedRoomAtIndex(Room *room, int index) {
    int i = 0;
    //RoomNode *current = room->connectedRooms;
    RoomNode *current = room->connectedRooms->head; // Updated

    //printf("current head: %s", room->connectedRooms->head->roomptr->name);

    while (current->next != NULL && i < index -1) {

       
        current = current->next;
        i++;
    }
    return (current != NULL) ? current->roomptr : NULL;
}

/*
    Moves a hunter to a random connected room.
    in: hunter - pointer to the Hunter structure.
    Moves the hunter to a different connected room if available, otherwise does nothing.
*/
void moveToRandomConnectedRoom(Hunter *hunter) {
    Room *currentRoom = hunter->currentRoom;
     Room *nextRoom;

    //printf("Hunters current room: %s",hunter->currentRoom->name);
    int numConnected = countConnectedRooms(currentRoom);
    //printf("Connected rooms to current room: %d",numConnected);
    if (numConnected == 0) return; // Stay if no connected rooms

    int randomIndex = randInt(0, numConnected + 1);

    if (numConnected == 1){

        nextRoom = currentRoom->connectedRooms->head->roomptr;
        //printf("in if statement\n\n next room: %s", nextRoom->name);

    }else{

    nextRoom = getConnectedRoomAtIndex(currentRoom, randomIndex);
    }
   
   

    if (nextRoom == NULL || nextRoom == currentRoom) return; // Do not move if the selected room is the current room or NULL

    sem_wait(&currentRoom->roomSem);
    sem_wait(&nextRoom->roomSem);

    removeHunterFromRoom(currentRoom, hunter); // Remove hunter from current room
    hunter->currentRoom = nextRoom; // Move hunter to the next room
   //printf ("I moved");
    l_hunterMove(hunter->name, nextRoom->name);

    sem_post(&currentRoom->roomSem);
    sem_post(&nextRoom->roomSem);
}
/*
    Initializes a list of rooms.
    return: pointer to the initialized RoomList structure. Returns NULL if allocation fails.
*/
RoomList* initRoomList(){
    RoomList* newList = malloc(sizeof(RoomList));
    newList->head = NULL;
    newList->tail = NULL;
    return newList;

}
/*
    Creates a node for a given room.
    in: room - pointer to the Room structure.
    return: pointer to the newly created RoomNode structure. Returns NULL if allocation fails.
*/

RoomNode* createRoomNode( Room* room ){

    RoomNode* newNode = malloc(sizeof(RoomNode));
    if (newNode == NULL) return NULL;
    newNode->roomptr = room;
    newNode->next = NULL;
    return newNode;

}

/*
    Removes a hunter from a room.
    in: room - pointer to the Room structure.
    in: hunter - pointer to the Hunter structure.
    Removes the hunter from the room's hunter list.
*/
void removeHunterFromRoom(Room* room, Hunter* hunter) {
    HunterNode *current = room->hunters->head;
    HunterNode *prev = NULL;

    //printf("\nremoving unter");

    while (current != NULL) {
        if (strcmp(current->hunter->name, hunter->name) == 0) {
            if (prev == NULL) {
                room->hunters->head = current->next;
            } else {
                prev->next = current->next;
            }
            if (current == room->hunters->tail) {
                room->hunters->tail = prev;
            }
            //free(current); // Free the node
            break;
        }
        prev = current;
        current = current->next;
    }
}

/*
    Connects two rooms bi-directionally.
    in: room1 - pointer to the first Room structure.
    in: room2 - pointer to the second Room structure.
    Links both rooms to each other, enabling movement between them.
*/
void connectRooms(Room* room1, Room* room2){

    RoomNode *node1, *node2;

    node1 = createRoomNode(room1);
    node2 = createRoomNode(room2);
    

    
    if ((node1 == NULL)|| (node2 == NULL)){
        return;
    }else{

        //printf("sending to connect helper 1\n");
        connectRoomsHelper(room1,node2);
        //printf("sending to connect helper 2\n");
        connectRoomsHelper(room2,node1);
    }

}
/*
    Helper function to connect a room to another room.
    in: room - pointer to the Room structure where connection is to be added.
    in: node - pointer to the RoomNode to be added to the room's connections.
    Adds the node to the room's connectedRooms list.
*/
// it should be RoomNode* not RoomNode
void connectRoomsHelper(Room* room, RoomNode* node){

    if (room->connectedRooms->head == NULL){
    room->connectedRooms->head = node;
    room->connectedRooms->tail = node;
    }
    else{
        room->connectedRooms->tail->next = node;
        room->connectedRooms->tail = node;

        //printf("Room %s has been connected to room %s\n", room->name, node->roomptr->name);

    }

}
/*
    Adds a room to a room list.
    in/out: listPtr - pointer to the pointer of RoomList. Modified to include the new room.
    in: room - pointer to the Room structure to add to the list.
    Adds the room to the end of the list.
*/

void addRoom(RoomList** listPtr, Room* room) {
    if (!listPtr || !room) return; // Check for NULL pointers

    // Dereference listPtr to get the actual list
    RoomList* list = *listPtr;

    RoomNode* newRoomNode = createRoomNode(room);
    if (!newRoomNode) return; // Check if RoomNode creation was successful

    if (list->head == NULL) {
        // If the list is empty, both head and tail point to the new node
        list->head = newRoomNode;
        list->tail = newRoomNode;
    } else {
        // If the list is not empty, append the new node and update the tail
        list->tail->next = newRoomNode;
        list->tail = newRoomNode;
    }
}

/*
    Creates a room with a given name.
    in: roomName - name of the room to create.
    return: pointer to the newly created Room structure. Returns NULL if allocation fails.
*/ 
struct Room* createRoom(const char* roomName) {
    // Allocate memory for a new room
    Room* room = malloc(sizeof(Room));
    if (!room) return NULL; // Check for allocation failure

    // Safely copy the room name into the room's name field
    // The strncpy function is used to avoid buffer overflow
    strncpy(room->name, roomName, MAX_STR - 1);
    room->name[MAX_STR - 1] = '\0'; // Ensure null termination

    // Initialize other members of Room
    room->connectedRooms = initRoomList(); // Initialize as no connected rooms
    room->evidenceList = initEvidenceList();   // No evidence in the room initially
    room->hunters = initHunterList();        // No hunters in the room initially
    room->ghost = NULL;          // No ghost in the room initially
   

    // Initialize semaphore for thread safety (if you're using it)
    if (sem_init(&room->roomSem, 0, 1) != 0) {
        // Handle semaphore initialization error
       // free(room);
        return NULL;
    }

    // Return the pointer to the newly created room
    return room;
}
/*
    Frees all resources associated with a room.
    in: room - pointer to the Room structure to be freed.
    Frees the room and its associated resources like evidenceList.
*/
void freeRoom(Room* room) {
    if (!room) return;

    freeEvidenceList(room->evidenceList);
    // Free other dynamically allocated fields in Room, if any

    free(room);
}
/*
    Frees a list of rooms.
    in: list - pointer to the RoomList structure to be freed.
    Frees all rooms in the list and then the list itself.
*/
void freeRoomList(RoomList* list) {
    if (!list) return;

    RoomNode* current = list->head;
    while (current != NULL) {
        RoomNode* temp = current;
        current = current->next;
        
        //freeConnectedRoomList(temp->roomptr->connectedRooms);
        freeRoom(temp->roomptr); // Free the Room
        free(temp); // Free the RoomNode

       
    }

    free(list);
}
/*
    Frees a list of connected rooms.
    in: list - pointer to the RoomList structure containing connected rooms to be freed.
    Frees all rooms in the connected list and then the list itself.
*/
void freeConnectedRoomList(RoomList* list) {
    if (!list) return;

    RoomNode* current = list->head;
    while (current != NULL) {
        RoomNode* temp = current;
        current = current->next;
        
        
        freeRoom(temp->roomptr); // Free the Room
        free(temp); // Free the RoomNode

       
    }

    free(list);
}