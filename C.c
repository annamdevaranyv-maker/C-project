#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OVERS 2
#define PLAYERS 11

struct Batsman {
    char name[50];
    int runs;
    int balls;
    int fours;                                  // structure for batsman
    int sixes;                                      
    int out;
    struct Batsman *next;
};

struct Bowler {
    char name[50];
    int runs_given;                             // structure for bowler
    int balls_bowled;
    int wickets;
    struct Bowler *next;
};

int innings(char team[], struct Batsman *bHead, struct Bowler *boHead,int target);
struct Batsman* BatsmanList(char team[], int players);
struct Bowler* BowlerList(char team[], int players);
void saveScore(char *team, struct Batsman *bHead, struct Bowler *boHead, int totalRuns, int extras);
void freeList(struct Batsman *bHead, struct Bowler *boHead);

void searchPlayer() {
    char name[50], line[200];
    int found = 0;

    printf("\nEnter player name to search (exact): ");             //asking user for searching player
    scanf("%s", name);

    FILE *fp = fopen("scorecards.txt", "r");                    //opening scorecards text file in read mode
    if (!fp) {
        printf("Scorecard file not found!\n");           //print this message if scorecards file not found
        return;
    }

    while (fgets(line, sizeof(line), fp)) {         // check if the exact name exists at beginning of a line (to avoid wrong matching)                                      
        if (strncmp(line, name, strlen(name)) == 0) {   //checks beginning of the line with entered name(only first N characters)
            printf("\n------------- PLAYER DETAILS -------------\n");
            printf("%s", line);
            found = 1;
            break;
        }
    }

    fclose(fp);

    if (!found)                                 // if no player found print player not found
        printf("\n Player '%s' not found in scorecards.txt\n", name);
}


int innings(char team[], struct Batsman *bHead, struct Bowler *boHead,int target) {
    int total_runs = 0, extras = 0, wickets = 0;
    struct Batsman *striker = bHead;
    struct Batsman *non_striker = bHead->next;

    printf("\n-------------- %s INNINGS START ---------------\n", team);

    for (int over = 1; over <= OVERS && wickets < 10; over++) {
        int bowlerNum;
        do {
            printf("\nEnter bowler number: ");          // checking if the bowler no is between 1 and 11
            scanf("%d", &bowlerNum);
            if (bowlerNum < 1 || bowlerNum > PLAYERS)
                printf("Invalid bowler number! Enter between 1 and %d.\n", PLAYERS);
        } while (bowlerNum < 1 || bowlerNum > PLAYERS);

        struct Bowler *bowler = boHead;
        for (int i = 1; i < bowlerNum; i++)
            bowler = bowler->next;                  // traversing bolwer node to that particular bolwernum

        for (int ball = 1; ball <= 6 && wickets < 10; ball++) {
            char input[30];
            printf("Ball %d is a (runs/out/wide/noball): ", ball);
            scanf("%s", input);

            if (strcmp(input, "wide") == 0) {               // checking if ball bowled is wide
                extras++;                               
                bowler->runs_given++;
                printf("Wide ball! +1 run\n");
                ball--;
                continue;
            }
            else if (strcmp(input, "noball") == 0) {       //check if its no ball
                extras++;
                bowler->runs_given++;
                printf("No ball! +1 run. Enter free hit runs: ");
                int fh;
                scanf("%d", &fh);
                extras += fh;
                printf("Free hit: %d runs added to extras\n", fh);
                ball--;
                continue;
            }
            else if (strcmp(input, "out") == 0) {           //check it the batsman is out 
                printf("Batsman %s OUT!\n", striker->name);
                striker->out = 1;
                striker->balls++;
                bowler->balls_bowled++;
                bowler->wickets++;
                wickets++;

                if (wickets < 10) {
                    int nextBat;                            // number for new batsmen 
                    struct Batsman *next = NULL;
                    do {
                        printf("Enter next batsman number (1-%d): ", PLAYERS);
                        scanf("%d", &nextBat);

                        if (nextBat < 1 || nextBat > PLAYERS) {
                            printf("Invalid batsman number! Try again.\n");
                            continue;
                        }

                        next = bHead;                           //next will point to that next batsman
                        for (int i = 1; i < nextBat; i++)
                            next = next->next;

                        if (next == non_striker) {              // if the user enters next batsman as non striker its points to NULL
                            printf("He is already at the crease (non-striker)! Choose another.\n");
                            next = NULL;
                            continue;
                        }

                        if (next->out == 1) {                  // if the user chooses the batsman who is already out it again points to NULL
                            printf("That batsman is already OUT! Choose another.\n");
                            next = NULL;
                        }

                    } while (!next);            // checking if structure next is not NULL if it is NULL it again runs the loop

                    striker = next;             // pointing striker to the new batsman 
                }
                continue;
            }
            int runs;
            while (1) {
                runs = atoi(input);             // converts string into integers
                if (runs >= 0 && runs <= 6) break;
                printf("Invalid runs! Enter 0–6: ");
                scanf("%s", input);
            }

            striker->runs += runs;
            striker->balls++;
            bowler->balls_bowled++;
            bowler->runs_given += runs;
            total_runs += runs;
            if (target!=-1 && total_runs+extras > target){              //if team chased the target it returns 
                printf("\n*** %s has chased the target succesfully! ***\n",team);
                saveScore(team, bHead, boHead, total_runs, extras);
                return total_runs + extras;
            }

            if (runs == 4) striker->fours++;        // increment no of fours and sixes scored by batsman
            if (runs == 6) striker->sixes++;

            if (runs == 1 || runs == 3 || runs == 5) {       // if batsman scores odd runs striker and non-striker get interchanged
                struct Batsman *temp = striker;
                striker = non_striker;
                non_striker = temp;
            }
        }

        struct Batsman *t = striker;        // changing striker and non-striker for end of every over
        striker = non_striker;
        non_striker = t;
    }

    printf("\n----------------- %s Batting Scorecard ----------------\n", team);       
    printf("Player\t\tRuns\tBalls\t4s\t6s\tStatus\n");      // printing batting scorcard at the end of first innings
    for (struct Batsman *b = bHead; b; b = b->next)
        if (b->balls > 0)                                   //printing attributes of all the batsmans who faced greater than 1 ball
            printf("%-12s\t%d\t%d\t%d\t%d\t%s\n",
                b->name, b->runs, b->balls, b->fours, b->sixes,
                b->out ? "OUT" : "NOT OUT");                // ternary operator to check if the batsman if out or notout

    printf("\n-------------------- Bowling Summary (Against %s) ----------------------\n", team);
    printf("Bowler\t\tRuns\tBalls\tWickets\n");

    for (struct Bowler *bo = boHead; bo; bo = bo->next)         // printing bowling summary
        if (bo->balls_bowled > 0)
            printf("%-12s\t%d\t%d\t%d\n",
                bo->name, bo->runs_given, bo->balls_bowled, bo->wickets);

    printf("\nTotal = %d (Extras: %d)\n", total_runs + extras, extras);     // printing total runs and extras
    saveScore(team, bHead, boHead, total_runs, extras);
    return total_runs + extras;
}

struct Batsman* BatsmanList(char team[], int players) {     // creating a linked list of batsmen
    struct Batsman *head = NULL, *tail = NULL;
    for (int i = 1; i <= players; i++) {
        struct Batsman *n = (struct Batsman*)malloc(sizeof(struct Batsman));
        sprintf(n->name, "%s_P%d", team, i);              // used sprintf for formatted string name 
        n->runs = n->balls = n->fours = n->sixes = n->out = 0;  // intializinf evrything to zero initially
        n->next = NULL;
        if (!head) head = tail = n;             // if head = NULL that is nothing intially,we assign head and tail to n
        else { tail->next = n; tail = n; }      
    }
    return head;
}
struct Bowler* BowlerList(char team[], int players) {   //creating linked lists for bowler
    struct Bowler *head = NULL, *tail = NULL;
    for (int i = 1; i <= players; i++) {
        struct Bowler *n = (struct Bowler*)malloc(sizeof(struct Bowler));
        sprintf(n->name, "%s_P%d", team, i);
        n->runs_given = n->balls_bowled = n->wickets = 0;
        n->next = NULL;
        if (!head) head = tail = n;
        else { tail->next = n; tail = n; }
    }
    return head;
}

void saveScore(char *team, struct Batsman *bHead, struct Bowler *boHead, int totalRuns, int extras) {
    FILE *fp = fopen("scorecards.txt", "a");
    fprintf(fp, "\n------------------ %s SCORECARD -----------------\n", team); // saving all the scores in scorecards text file
    fprintf(fp, "Player\tRuns\tBalls\t4s\t6s\tStatus\n");
    for (struct Batsman *b = bHead; b; b = b->next)
        if (b->balls > 0)                           //printing only those batsmens who have played atleast 1 ball
            fprintf(fp, "%s\t%d\t%d\t%d\t%d\t%s\n",     //printing all the details of the batsman
                b->name, b->runs, b->balls, b->fours, b->sixes,
                b->out ? "OUT" : "NOT OUT");

    fprintf(fp, "\nBowler\tRuns\tBalls\tWickets\n");
    for (struct Bowler *bo = boHead; bo; bo = bo->next)
        if (bo->balls_bowled > 0)                      //printing only those bowlers who bowls atleast 1 ball
            fprintf(fp, "%s\t%d\t%d\t%d\n",
                bo->name, bo->runs_given, bo->balls_bowled, bo->wickets);

    fprintf(fp, "\nTotal = %d (Extras: %d)\n", totalRuns + extras, extras);
    fclose(fp);             //closing the txt file
}

void freeList(struct Batsman *bHead, struct Bowler *boHead) {       // freeing the batsmen and bowlers linked lists
    while (bHead) { struct Batsman *t = bHead; bHead = bHead->next; free(t); }
    while (boHead) { struct Bowler *t = boHead; boHead = boHead->next; free(t); }
}

int main() {
    char teamA[20], teamB[20];
    printf("Enter Team A name: ");
    scanf("%s", teamA);
    printf("Enter Team B name: ");
    scanf("%s", teamB);

    struct Batsman *teamA_bats = BatsmanList(teamA, PLAYERS);
    struct Bowler *teamA_bowls = BowlerList(teamA, PLAYERS);        // creates players using linked lists
    struct Batsman *teamB_bats = BatsmanList(teamB, PLAYERS);
    struct Bowler *teamB_bowls = BowlerList(teamB, PLAYERS);

    int toss = rand() % 2, choose = rand() % 2;         // performs toss

    printf("\nTOSS: %s won the toss and chose to %s first.\n",
           toss == 0 ? teamA : teamB,
           choose == 0 ? "BAT" : "BOWL");

    int scoreA = 0, scoreB = 0;

    if ((toss == 0 && choose == 0) || (toss == 1 && choose == 1)) {
        scoreA = innings(teamA, teamA_bats, teamB_bowls,-1);
        printf("\n%s needs %d runs to win!\n", teamB, scoreA + 1);
        scoreB = innings(teamB, teamB_bats, teamA_bowls, scoreA);
    } else {
        scoreB = innings(teamB, teamB_bats, teamA_bowls, -1);
        printf("\n%s needs %d runs to win!\n", teamA, scoreB + 1);
        scoreA = innings(teamA, teamA_bats, teamB_bowls ,scoreB);
    }

    printf("\n-------------------- FINAL RESULT ---------------------\n");
    printf("%s: %d\n%s: %d\n", teamA, scoreA, teamB, scoreB);

    if (scoreA > scoreB)
        printf("%s won by %d runs!\n", teamA, scoreA - scoreB);
    else if (scoreB > scoreA)                                  // display match result
        printf("%s won by %d runs!\n", teamB, scoreB - scoreA);
    else
        printf("Match Tied!\n");
    char ch;
    printf("\nDo you want to search any player's statistics? (y/n): ");
    scanf("%c", &ch);
    if (ch == 'y' || ch == 'Y') {           // if player says to search any player then call this func
        searchPlayer();
    }

    freeList(teamA_bats, teamA_bowls);      // free linked lists
    freeList(teamB_bats, teamB_bowls);

    return 0;
}