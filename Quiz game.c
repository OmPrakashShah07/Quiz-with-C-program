#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct Question {
    char question[100];
    char options[4][50];
    int correct;
};

struct Score {
    char name[50];
    int score;
};

void shuffle(int arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void saveScore(const char *name, int score) {
    FILE *file = fopen("scores.txt", "a");
    if (file == NULL) {
        printf("Error opening file for saving score!\n");
        return;
    }
    fprintf(file, "%s,%d\n", name, score);
    fclose(file);
}

void displayHighScores(int threshold) {
    FILE *file = fopen("scores.txt", "r");
    if (file == NULL) {
        printf("No scores recorded yet or error opening file!\n");
        return;
    }

    printf("\nScores greater than %d:\n", threshold);
    printf("--------------------\n");
    char line[100];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char name[50];
        int score;
        if (sscanf(line, "%[^,],%d", name, &score) == 2) {
            if (score > threshold) {
                printf("%s: %d\n", name, score);
                found = 1;
            }
        }
    }
    if (!found) {
        printf("No scores greater than %d found.\n", threshold);
    }
    fclose(file);
}

void displayHighestScorer() {
    FILE *file = fopen("scores.txt", "r");
    if (file == NULL) {
        printf("\nNo scores recorded yet or error opening file!\n");
        return;
    }

    char line[100];
    char highestNames[100][50];
    int highestScore = -1;
    int nameCount = 0;

    while (fgets(line, sizeof(line), file)) {
        char name[50];
        int score;
        if (sscanf(line, "%[^,],%d", name, &score) == 2) {
            if (score > highestScore) {
                highestScore = score;
                nameCount = 0;
                strcpy(highestNames[nameCount], name);
                nameCount++;
            } else if (score == highestScore) {
                strcpy(highestNames[nameCount], name);
                nameCount++;
            }
        }
    }
    fclose(file);

    if (highestScore >= 0) {
        printf("\nHighest Score: %d\n", highestScore);
        printf("Achieved by: ");
        for (int i = 0; i < nameCount; i++) {
            printf("%s", highestNames[i]);
            if (i < nameCount - 1) {
                printf(", ");
            }
        }
        printf("\n");
    } else {
        printf("\nNo scores recorded yet!\n");
    }
}

int loadQuestions(struct Question quiz[], int *numQuestions) {
    FILE *file = fopen("questions.txt", "r");
    if (file == NULL) {
        printf("Error opening questions file!\n");
        return 0;
    }

    char line[256];
    *numQuestions = 0;
    while (fgets(line, sizeof(line), file) && *numQuestions < 100) {
        char *token = strtok(line, ",");
        if (token) strcpy(quiz[*numQuestions].question, token);
        for (int i = 0; i < 4; i++) {
            token = strtok(NULL, ",");
            if (token) strcpy(quiz[*numQuestions].options[i], token);
        }
        token = strtok(NULL, ",");
        if (token) quiz[*numQuestions].correct = atoi(token);
        (*numQuestions)++;
    }
    fclose(file);
    return 1;
}

int main() {
    srand(time(0));
    struct Question quiz[100];
    int numQuestions = 0;

    // Load questions from file
    if (!loadQuestions(quiz, &numQuestions)) {
        printf("Failed to load questions. Exiting...\n");
        return 1;
    }
    if (numQuestions < 5) {
        printf("Not enough questions in file (need at least 5). Exiting...\n");
        return 1;
    }

    // Select 5 random questions
    int order[100];
    for (int i = 0; i < numQuestions; i++) order[i] = i;
    shuffle(order, numQuestions);
    
    char name[50];
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    int score = 0;
    int answer;
    printf("\nWelcome to the Quiz Game, %s!\n", name);
    printf("Answer each question by entering the option number (1-4)\n\n");
    for (int i = 0; i < 5; i++) {
        int q = order[i];
        printf("Question %d: %s\n", i + 1, quiz[q].question);
        for (int j = 0; j < 4; j++) {
            printf("%d. %s\n", j + 1, quiz[q].options[j]);
        }
        printf("Your answer (1-4): ");
        scanf("%d", &answer);
        
        if (answer - 1 == quiz[q].correct) {
            printf("Correct!\n\n");
            score++;
        } else {
            printf("Wrong! Correct answer was: %s\n\n", quiz[q].options[quiz[q].correct]);
        }
    }
    
    printf("Quiz Complete!\n");
    printf("Your score, %s: %d out of 5\n", name, score);
    if (score >= 4) printf("Excellent job!");
    else if (score >= 3) printf("Good effort!");
    else printf("Better luck next time!");

    // Save score to file
    saveScore(name, score);

    // Display highest scorer(s)
    displayHighestScorer();

    // Option to view high scores
    char viewScores;
    printf("\nDo you want to view scores greater than a certain value? (y/n): ");
    getchar();
    viewScores = getchar();
    if (viewScores == 'y' || viewScores == 'Y') {
        int threshold;
        printf("Enter the score threshold: ");
        scanf("%d", &threshold);
        displayHighScores(threshold);
    }

    printf("\nPress Enter to exit...");
    getchar();
    getchar();
    return 0;
}


