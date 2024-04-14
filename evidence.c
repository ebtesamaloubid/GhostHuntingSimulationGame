#include "defs.h"
/*
    Leaves evidence in the current room of the ghost.
    in: ghost - pointer to the Ghost structure.
    Generates and adds evidence to the room based on the ghost's type.
    Logs the action of leaving evidence.
*/
void leaveEvidence(Ghost *ghost) {
    EvidenceType evidenceType = getRandomEvidence(ghost->ghostType); // Implement this based on ghost type
    //addEvidenceToSharedCollection(ghost->currentRoom->evidenceList, evidenceType);
    addEvidenceToRoom(ghost->currentRoom, evidenceType); // Implement this

    l_ghostEvidence(evidenceType, ghost->currentRoom->name); // Log evidence left
}
/*
    Returns a randomly generated evidence type.
    out: randomly generated EvidenceType.
    Generates a random evidence type using randInt.
    return: EvidenceType value.
*/
EvidenceType getRandomEvidenceType() {
    return (EvidenceType) randInt(0, EV_COUNT);
}
/*
    Adds evidence to a room.
    in: room - pointer to the Room structure.
    in: evidence - EvidenceType to add to the room.
    Adds a new EvidenceNode with the given evidence to the room's evidence list.
*/
void addEvidenceToRoom(Room *room, EvidenceType evidence) {
    //if (EV_COUNT < MAX_EVIDENCE) {

        EvidenceNode* newNode = creatEvidenceNode(evidence); 
        if (room->evidenceList->tail != NULL) {
            room->evidenceList->tail->next = newNode;
            //printf("\nADDING TO Tail\n");
        } else {

            //printf("\nADDING TO HEAD\n");
            room->evidenceList->head = newNode;
        }
        room->evidenceList->tail = newNode;
    //}
}
/*
    Returns a random evidence type based on the ghost's class.
    in: ghostType - class of the ghost.
    Generates and returns evidence specific to the ghost's class.
    return: EvidenceType specific to the ghost's class.
*/
EvidenceType getRandomEvidence(GhostClass ghostType) {

    int evidence = getRandomEvidenceType();
    switch (ghostType) {
        case POLTERGEIST:
            return evidence;
            // Assuming Poltergeist can leave any evidence
        case BANSHEE:
            if (evidence == 2)
            {
                return SOUND;
            }
            return evidence;
            // Specific evidence types for Banshee
            // Example: return TEMPERATURE;
        case BULLIES:
            if (evidence == 1)
             {
                return FINGERPRINTS;
            }
            else if(evidence == 2){
                return SOUND;
            }
            return evidence;

            // Specific evidence types for Bullies
            // Example: return SOUND;
        case PHANTOM:

            if (evidence == 1)
             {
                return FINGERPRINTS;
            }
            else if(evidence == 2){
                return SOUND;
            }
            return TEMPERATURE;
            // Specific evidence types for Phantom
            // Example: return FINGERPRINTS;
        default:
            return EV_UNKNOWN;
    }
}
/*
    Collects evidence if available in the hunter's current room.
    in: hunter - pointer to the Hunter structure.
    Checks the current room for evidence matching the hunter's equipment and collects it.
    Logs the action of collecting evidence.
*/
void collectEvidenceIfAvailable(Hunter *hunter) {

   // printf("%s hunter name", hunter->name);
    if (!hunter || !hunter->currentRoom) {
        return;
    }

    Room *currentRoom = hunter->currentRoom;

    if (!currentRoom->evidenceList) {
         //printf("\nITS NULL\n");
        return;
    }

    if (currentRoom->evidenceList->head == NULL){
        return;
    }
    EvidenceNode* curEvidence = currentRoom->evidenceList->head;

   while (curEvidence != NULL){

        if (curEvidence->evidence == hunter->evidenceType) {
            //printf("\nITS A MATCH\n");
            addEvidenceToSharedCollection(hunter->sharedEvidence, hunter->evidenceType);
            l_hunterCollect(hunter->name, hunter->evidenceType, currentRoom->name); // Log collection
            break;
        }

        curEvidence = curEvidence->next;
    }
}

/*
    Reviews the collected evidence and checks for sufficiency.
    in: hunter - pointer to the Hunter structure.
    Determines if the collected evidence is sufficient for ghost identification.
    Logs the review and exits the hunter thread if evidence is sufficient.
*/
void reviewCollectedEvidence(Hunter *hunter) {
    if (isEvidenceSufficientForIdentification(hunter->sharedEvidence)) {
        l_hunterReview(hunter->name, LOG_SUFFICIENT);
        l_hunterExit(hunter->name, LOG_EVIDENCE);
        pthread_exit(NULL);
    } else {
        l_hunterReview(hunter->name, LOG_INSUFFICIENT);
    }
}/*
    Adds evidence to a shared evidence collection.
    in: sharedEvidence - pointer to the shared EvidenceList.
    in: evidence - EvidenceType to add to the collection.
    Adds a new EvidenceNode with the given evidence to the shared evidence list.
*/
void addEvidenceToSharedCollection(EvidenceList *sharedEvidence, EvidenceType evidence) {
    
    EvidenceNode* newNode = creatEvidenceNode(evidence);  // Removed '&'
   
    if (sharedEvidence->tail != NULL) {
         //EvidenceNode* cur = sharedEvidence->head;
        sharedEvidence->tail->next = newNode;
        sharedEvidence->tail = newNode;

    } else {
        sharedEvidence->head = newNode;
        sharedEvidence->tail = newNode;
    }
}

/*
    Determines if the collected evidence is sufficient for ghost identification.
    in: sharedEvidence - pointer to the shared EvidenceList.
    return: 1 if the evidence is sufficient, 0 otherwise.
    Evaluates the collected evidence and determines if it meets the criteria for sufficiency.
*/
int isEvidenceSufficientForIdentification(EvidenceList *sharedEvidence) {

    
    if (!sharedEvidence || !sharedEvidence->head) {
        
        return 0;
    }

    //printf("%d evelist\n", sharedEvidence->head->evidence); // Now safe inside the NULL check

    int sound = 0, temp = 0, fingerprints = 0, emf = 0;
    EvidenceNode* evidence = sharedEvidence->head;

    while(evidence != NULL){

        if (evidence->evidence == EMF){
            //printf("found emf \n");
            emf = 1;
        }
        if (evidence->evidence == TEMPERATURE){
            //printf("found temp \n");
            temp = 1;
        }
        if (evidence->evidence == FINGERPRINTS){
            //printf("found fingers \n");
            fingerprints = 1;
        }
        if (evidence->evidence == SOUND){
            //printf("found sound \n");

            sound = 1;
        }

        evidence = evidence->next;
    }
    if (sound + fingerprints + emf + temp >= 3) {

        return 1; // Sufficient evidence
    }

    return 0; // Not enough evidence
}
/*
    Creates a new EvidenceNode with the given evidence.
    in: evidence - EvidenceType to be added to the node.
    return: pointer to the newly created EvidenceNode. Returns NULL if allocation fails.
*/
EvidenceNode* creatEvidenceNode(EvidenceType evidence) {
    EvidenceNode* newNode = malloc(sizeof(EvidenceNode));
    if (newNode == NULL) return NULL;
    newNode->evidence = evidence;
    newNode->next = NULL;
    return newNode;
}
/*
    Initializes an evidence list.
    return: pointer to the initialized EvidenceList structure. Returns NULL if allocation fails.
*/
EvidenceList* initEvidenceList(){
    EvidenceList* newList = malloc(sizeof(EvidenceList));
    newList->head = NULL;
    newList->tail = NULL;
    return newList;

}
/*
    Matches collected evidence to determine the ghost class.
    in: evArray - array of collected evidence types.
    return: GhostClass based on the matching of evidence. Returns GH_UNKNOWN if no match is found.
    Evaluates the evidence and returns the corresponding ghost class.
*/
GhostClass matchEvidence(EvidenceType evArray[3]) {

    int temp = 0, fingerprint = 0, emf = 0, sound = 0;

    for (int i = 0; i < 3; i++) {

        if (evArray[i] == SOUND) {
            sound = 1;
        } else if (evArray[i] == TEMPERATURE) {
            temp = 1;
        } else if (evArray[i] == EMF) {
            emf = 1;
        } else {
            fingerprint = 1;
        }
    }
    
    if (emf == 1 && temp == 1 && fingerprint == 1) {
        return POLTERGEIST;
    } else if (emf == 1 && temp == 1 && sound == 1) {
        return BANSHEE;
    } else if (emf == 1 && sound == 1 && fingerprint == 1) {
        return BULLIES;
    } else if (temp == 1 && sound == 1 && fingerprint == 1) {
        return PHANTOM;
    }
    return GH_UNKNOWN;
}
/*
    Frees all resources associated with an evidence list.
    in: list - pointer to the EvidenceList structure to be freed.
    Frees all evidence nodes in the list and then the list itself.
*/
void freeEvidenceList(EvidenceList* list) {
    if (!list) return;

    EvidenceNode* current = list->head;
    while (current != NULL) {
        //EvidenceNode* nextNode = current->next;

        EvidenceNode* temp = current;
        current = current->next;
        //current = nextNode; // Move to the next node
        
        free(temp); // Free the current node
        
    }

    free(list); // Free the list structure itself
}
