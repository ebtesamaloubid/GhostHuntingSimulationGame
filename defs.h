#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         1000000 //org 64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     8000
#define GHOST_WAIT      900
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define LOGGING         C_TRUE
#define MAX_EVIDENCE  3

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

typedef struct Room Room;
typedef struct RoomNode RoomNode;
typedef struct RoomList RoomList;
typedef struct House House;
typedef struct Ghost Ghost;
typedef struct Hunter Hunter;
typedef struct EvidenceNode EvidenceNode;
typedef struct EvidenceList EvidenceList;
typedef struct HunterNode HunterNode;
typedef struct HunterList HunterList;
//typedef struct HunterGhostPair;




enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

// change the v names
typedef struct Room {
    char name[MAX_STR];
    struct RoomList *connectedRooms; // Linked list of connected rooms
    EvidenceList *evidenceList; // Linked list of evidence in the room
    HunterList *hunters; // Linked list of hunters in the room
    Ghost *ghost; // Pointer to the ghost (NULL if no ghost)
    sem_t roomSem; // Semaphore for thread safety
}Room;

typedef struct RoomNode{

    RoomNode* next;
    struct Room* roomptr;
}RoomNode;

typedef struct RoomList {
    struct RoomNode *head; 
    struct RoomNode *tail;
}RoomList;

typedef struct House{
    HunterList *hunters; // Array or linked list of hunters
    struct RoomList *rooms; // Linked list of all rooms in the house
    EvidenceList *sharedEvidence; // Shared evidence list
    int arr[4];
} HouseType;

typedef struct Ghost{
    GhostClass ghostType;
    struct Room *currentRoom;
    int boredomTimer;
} Ghost;

typedef struct Hunter{
    char name[MAX_STR];
    struct Room *currentRoom;
    EvidenceType evidenceType;
    EvidenceList *sharedEvidence; // Pointer to the house's shared evidence list
    int fear;
    int boredom;
} Hunter;

typedef struct EvidenceNode{

    EvidenceNode* next;
    EvidenceType evidence;
}EvidenceNode;

typedef struct EvidenceList{
    struct EvidenceNode *head; 
    struct EvidenceNode *tail;
}EvidenceList;

typedef struct HunterNode{

    HunterNode* next;
    struct Hunter* hunter;
}HunterNode;

typedef struct HunterList {
    struct HunterNode *head; 
    struct HunterNode *tail;
}HunterList;
typedef struct HunterGhostPair {
    Hunter *hunter;
    Ghost *ghost;
} HunterGhostPair;




// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);


// evd: 
void leaveEvidence(Ghost *ghost);
void addEvidenceToRoom(Room *room, EvidenceType evidence);
EvidenceType getRandomEvidence(GhostClass ghostType);
void collectEvidenceIfAvailable(Hunter *hunter);
void addEvidenceToSharedCollection(EvidenceList *sharedEvidence, EvidenceType evidence);
int isEvidenceSufficientForIdentification(EvidenceList *sharedEvidence);
EvidenceNode* creatEvidenceNode(EvidenceType evidence);
EvidenceList* initEvidenceList();
void reviewCollectedEvidence(Hunter *hunter);
GhostClass matchEvidence(EvidenceType evArray[3]);

//ghost: 
void moveGhostToRandomRoom(Ghost *ghost);
void updateGhostBoredom(Ghost *ghost);
void performGhostAction(Ghost *ghost);
void initGhost(Ghost **ghost, HouseType house);

// House: 
void populateRooms(HouseType* house);
//void initHouse(HouseType* house);
Room* getRoom(char name[MAX_STR], HouseType house);
void initHouse(HouseType** house);

// Hunter
Hunter* HunterInit(char* hunter, Room* room, HouseType* house);
int isHunterInRoom(Room *room);
void checkAndExitHunter(Hunter *hunter);
void performHunterAction(Hunter *hunter, Ghost *ghost);
HunterNode* createHunterNode( Hunter* hunter );
HunterList* initHunterList();
void updateFearAndBoredom(Hunter *hunter, Ghost *ghost);
int reasonForExiting(Hunter* hunter,int numLeft);
int getEquipment(HouseType *house);

// main 
void *hunterThreadFunction(void *arg);
void *ghostThreadFunction(void *arg);

// room:
int countConnectedRooms(Room *room);
Room *getConnectedRoomAtIndex(Room *room, int index);
void moveToRandomConnectedRoom(Hunter *hunter);
RoomList* initRoomList();
RoomNode* createRoomNode( Room* room );

void connectRooms(Room* room1, Room* room2);
void connectRoomsHelper(Room* room, RoomNode* node);
void addRoom(RoomList** listPtr, Room* room);
struct Room* createRoom(const char* roomName);
//int isGhostInTheRoom(Room* room, Ghost ghost);
void removeHunterFromRoom(Room* room, Hunter* hunter);
void printConnectedRooms(Room* room);
//void printAllRooms(Room* room)


//freeing data
void freeHouse(HouseType* house);
void freeHunterList(HunterList* list);
void freeEvidenceList(EvidenceList* list);
void freeRoom(Room* room);
void freeRoomList(RoomList* list) ;
void freeHunter(Hunter* hunter);
void freeConnectedRoomList(RoomList* list);
void freeGhost(Ghost *ghost);