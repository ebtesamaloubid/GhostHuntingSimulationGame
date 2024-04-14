#include "defs.h"

/*
    Dynamically allocates several rooms and populates the provided house.
    Note: You may modify this as long as room names and connections are maintained.
        out: house - the house to populate with rooms. Assumes house has been initialized.
*/
void populateRooms(HouseType* house) {
    // First, create each room

    // createRoom assumes that we dynamically allocate a room, initializes the values, and returns a RoomType*
    // create functions are pretty typical, but it means errors are harder to return aside from NULL
    struct Room* van                = createRoom("Van");
    struct Room* hallway            = createRoom("Hallway");
    struct Room* master_bedroom     = createRoom("Master Bedroom");
    struct Room* boys_bedroom       = createRoom("Boy's Bedroom");
    struct Room* bathroom           = createRoom("Bathroom");
    struct Room* basement           = createRoom("Basement");
    struct Room* basement_hallway   = createRoom("Basement Hallway");
    struct Room* right_storage_room = createRoom("Right Storage Room");
    struct Room* left_storage_room  = createRoom("Left Storage Room");
    struct Room* kitchen            = createRoom("Kitchen");
    struct Room* living_room        = createRoom("Living Room");
    struct Room* garage             = createRoom("Garage");
    struct Room* utility_room       = createRoom("Utility Room");

    // This adds each room to each other's room lists
    // All rooms are two-way connections
    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);


    //RoomNode* curRoom = 

    // Add each room to the house's room list
    addRoom(&house->rooms, van);
    addRoom(&house->rooms, hallway);
    addRoom(&house->rooms, master_bedroom);
    addRoom(&house->rooms, boys_bedroom);
    addRoom(&house->rooms, bathroom);
    addRoom(&house->rooms, basement);
    addRoom(&house->rooms, basement_hallway);
    addRoom(&house->rooms, right_storage_room);
    addRoom(&house->rooms, left_storage_room);
    addRoom(&house->rooms, kitchen);
    addRoom(&house->rooms, living_room);
    addRoom(&house->rooms, garage);
    addRoom(&house->rooms, utility_room);
}
/*
    Initializes a house structure.
    in/out: house - pointer to a pointer of the HouseType structure. Modified to point to the allocated HouseType.
    Allocates memory for a house and initializes its components: hunters, rooms, and sharedEvidence.
    Populates the 'arr' array with integers 0 to 3.
*/
void initHouse(HouseType** house){
   (*house) = malloc(sizeof(HouseType));
      if (house == NULL) return;
    (*house)->hunters = initHunterList();
    (*house)->rooms = initRoomList();
    (*house)->sharedEvidence = initEvidenceList();

    for (int i =0; i < 4; i ++){
     (*house)->arr[i] = i;
    }

}
/*
    Returns a pointer to the room with the given name in the specified house.
    in: name - name of the room to find.
    in: house - the house in which to search for the room.
    Traverses the list of rooms in the house and returns the room if found.
    return: pointer to the Room structure if found, NULL otherwise.
*/
Room* getRoom(char name[MAX_STR], HouseType house) {
    RoomNode* curRoom = house.rooms->head;

    while (curRoom != NULL) {
        if (strcmp(curRoom->roomptr->name, name) == 0) {
            return curRoom->roomptr;
        }
        curRoom = curRoom->next; // Move to the next room in the list
    }
    return NULL; // Return NULL if the room is not found
}
/*
    Frees all resources associated with a house.
    in: house - pointer to the HouseType structure to be freed.
    Frees the hunter list, room list, and evidence list associated with the house.
    Finally, frees the house itself.
    Performs a NULL check before attempting to free to avoid freeing a NULL pointer.
*/
void freeHouse(HouseType* house) {
    if (!house) return;

    freeHunterList(house->hunters);
    freeRoomList(house->rooms);
    freeEvidenceList(house->sharedEvidence);
    free(house);
}