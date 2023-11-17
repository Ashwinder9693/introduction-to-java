#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME_LENGTH 50

// Candidate structure
typedef struct
{
    char name[MAX_NAME_LENGTH];
    int votes;
    int eliminated;
} Candidate;

// Global variables
Candidate *candidates;
int **preferences;
int voter_count, candidate_count;

// Function prototypes
int get_int(const char *prompt);
char *get_string(const char *prompt);
int is_number(const char *str);
void tabulate(void);
int print_winner(void);
int find_min(void);
int is_tie(int min);
void eliminate(int min);

int main(void)
{
    // Get the number of candidates
    do
    {
        candidate_count = get_int("Number of candidates: ");

        if (candidate_count < 2)
        {
            printf("There must be at least 2 candidates. Please enter a valid number.\n");
        }

    } while (candidate_count < 2);

    // Allocate memory for candidates and preferences
    candidates = malloc(candidate_count * sizeof(Candidate));
    preferences = malloc(MAX_NAME_LENGTH * sizeof(int *));
    for (int i = 0; i < MAX_NAME_LENGTH; i++)
    {
        preferences[i] = malloc(candidate_count * sizeof(int));
    }

    // Populate candidates array
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].votes = 0;
        candidates[i].eliminated = 0;

        int is_unique; // Declare is_unique here

        // Prompt user until a unique name is entered
        do
        {
            printf("Enter candidate %d's name: ", i + 1);
            strncpy(candidates[i].name, get_string(""), MAX_NAME_LENGTH - 1);

            // Check if the entered name is unique
            is_unique = 1;
            for (int j = 0; j < i; j++)
            {
                if (strcasecmp(candidates[i].name, candidates[j].name) == 0)
                {
                    is_unique = 0;
                    printf("Candidate name must be unique. Please enter a different name.\n");
                    break;
                }
            }

            // Check if the entered name is not empty
            if (!candidates[i].name[0])
            {
                is_unique = 0;
                printf("Candidate name cannot be empty. Please enter a valid name.\n");
            }

        } while (!is_unique);
    }

    // Get the number of voters
    do
    {
        char *voter_input = get_string("Number of voters: ");
        if (is_number(voter_input))
        {
            voter_count = atoi(voter_input);

            if (voter_count < 1)
            {
                printf("Invalid input. Please enter a number greater than 0.\n");
            }
        }
        else
        {
            printf("Invalid input. Please enter a valid number.\n");
        }
    } while (voter_count < 1);

    // Get voter preferences
    for (int i = 0; i < voter_count; i++)
    {
        printf("Rank preferences for voter %d:\n", i + 1);
        for (int j = 0; j < candidate_count;)
        {
            printf("Rank %d: ", j + 1);
            char *name;
            do
            {
                name = get_string("");

                // Check if the candidate name is valid
                int found = 0;
                for (int k = 0; k < candidate_count; k++)
                {
                    if (strcasecmp(name, candidates[k].name) == 0)
                    {
                        preferences[i][j] = k;
                        found = 1;
                        break;
                    }
                }

                if (!found)
                {
                    printf("Invalid candidate name. Please try again.\n");
                }
                else
                {
                    j++; // Move to the next rank only if the name is valid
                }
            } while (!name || name[0] == '\0');
        }
        printf("\n"); // New line after each voter's preferences
    }

    // Conduct the runoff election
    while (1)
    {
        tabulate();

        if (print_winner())
        {
            break;
        }

        int min = find_min();
        int tie = is_tie(min);

        if (tie)
        {
            printf("Tie! The election is inconclusive.\n");
            break;
        }

        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }

    // Free allocated memory
    for (int i = 0; i < MAX_NAME_LENGTH; i++)
    {
        free(preferences[i]);
    }
    free(preferences);
    free(candidates);

    return 0;
}

// Function to get an integer input
int get_int(const char *prompt)
{
    int value;
    char input[MAX_NAME_LENGTH];

    do
    {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) != NULL)
        {
            if (sscanf(input, "%d", &value) != 1)
            {
                printf("Invalid input. Please enter a valid number.\n");
                value = -1; // Set a flag value to indicate an error
            }
            else if (value <= 0)
            {
                printf("Invalid input. Please enter a number greater than 0.\n");
            }
        }
        else
        {
            printf("Error reading input.\n");
            exit(EXIT_FAILURE);
        }
    } while (value <= 0);

    return value;
}

// Function to get a string input
char *get_string(const char *prompt)
{
    static char buffer[MAX_NAME_LENGTH];
    printf("%s", prompt);

    if (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        // Remove newline character, if present
        size_t length = strlen(buffer);
        if (length > 0 && buffer[length - 1] == '\n')
        {
            buffer[length - 1] = '\0';
        }
        return buffer;
    }
    else
    {
        // Handle error or end-of-file
        printf("Error reading input.\n");
        exit(EXIT_FAILURE);
    }
}

// Function to check if a string is a number
int is_number(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

// Function to tabulate votes
void tabulate(void)
{
    for (int i = 0; i < voter_count; i++)
    {
        int choice = 0;
        while (candidates[preferences[i][choice]].eliminated)
        {
            choice++;
        }
        candidates[preferences[i][choice]].votes++;
    }
}

// Function to print the winner
int print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes > voter_count / 2)
        {
            printf("Winner: %s\n", candidates[i].name);
            return 1;
        }
    }
    return 0;
}

// Function to find the minimum number of votes
int find_min(void)
{
    int min_votes = voter_count;

    for (int i = 0; i < candidate_count; i++)
    {
        if (!candidates[i].eliminated && candidates[i].votes < min_votes)
        {
            min_votes = candidates[i].votes;
        }
    }

    return min_votes;
}

// Function to check if there is a tie
int is_tie(int min_votes)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (!candidates[i].eliminated && candidates[i].votes != min_votes)
        {
            return 0;
        }
    }
    return 1;
}

// Function to eliminate candidates
void eliminate(int min_votes)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (!candidates[i].eliminated && candidates[i].votes == min_votes)
        {
            candidates[i].eliminated = 1;
        }
    }
}
