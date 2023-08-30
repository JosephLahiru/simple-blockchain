#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

// Define the block structure
struct Block
{
    int index;
    char timestamp[50];
    char data[50];
    char hash[65];
    char previous_hash[65];
};

// Function to convert SHA256 hash bytes to a string
char *sha256_to_str(unsigned char hash[SHA256_DIGEST_LENGTH])
{
    char *hash_str = malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(hash_str + (i * 2), "%02x", hash[i]);
    hash_str[64] = 0;
    return hash_str;
}

// Function to calculate the hash for a block
char *calculate_hash(struct Block block)
{
    char index_str[10];
    sprintf(index_str, "%d", block.index);
    char *to_hash = malloc(strlen(index_str) + strlen(block.timestamp) + strlen(block.data) + strlen(block.previous_hash) + 1);
    strcpy(to_hash, index_str);
    strcat(to_hash, block.timestamp);
    strcat(to_hash, block.data);
    strcat(to_hash, block.previous_hash);
    unsigned char hashed[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)to_hash, strlen(to_hash), hashed);
    char *hash = sha256_to_str(hashed);
    free(to_hash);
    return hash;
}

// Function to create the Genesis Block
struct Block create_genesis_block()
{
    struct Block genesis_block;
    genesis_block.index = 0;
    strcpy(genesis_block.timestamp, "0");
    strcpy(genesis_block.data, "0");
    strcpy(genesis_block.previous_hash, "0");
    strcpy(genesis_block.hash, calculate_hash(genesis_block));
    return genesis_block;
}

// Function to add a new block to the blockchain
struct Block create_new_block(struct Block last_block, char *data)
{
    struct Block new_block;
    new_block.index = last_block.index + 1;
    time_t now;
    time(&now);
    strcpy(new_block.timestamp, ctime(&now));
    strcpy(new_block.data, data);
    strcpy(new_block.previous_hash, last_block.hash);
    strcpy(new_block.hash, calculate_hash(new_block));
    return new_block;
}

// Function to validate the blockchain
int is_blockchain_valid(struct Block *blockchain, int blockchain_length)
{
    for (int i = 1; i < blockchain_length; i++)
    {
        struct Block current_block = blockchain[i];
        struct Block previous_block = blockchain[i - 1];
        if (strcmp(current_block.hash, calculate_hash(current_block)) != 0)
        {
            return 0;
        }
        if (strcmp(current_block.previous_hash, previous_block.hash) != 0)
        {
            return 0;
        }
    }
    return 1;
}

// Function to display the blockchain
void display_blockchain(struct Block *blockchain, int blockchain_length)
{
    for (int i = 0; i < blockchain_length; i++)
    {
        printf("Block %d\n", blockchain[i].index);
        printf("Timestamp: %s", blockchain[i].timestamp);
        printf("Data: %s\n", blockchain[i].data);
        printf("Hash: %s\n", blockchain[i].hash);
        printf("Previous Hash: %s\n", blockchain[i].previous_hash);
    }
}

// Function to save the blockchain to a file
void save_blockchain(struct Block *blockchain, int blockchain_length)
{
    FILE *file = fopen("blockchain.txt", "w");
    if (file == NULL)
    {
        printf("Unable to open file.\n");
        return;
    }
    for (int i = 0; i < blockchain_length; i++)
    {
        fprintf(file, "Block %d\n", blockchain[i].index);
        fprintf(file, "Timestamp: %s", blockchain[i].timestamp);
        fprintf(file, "Data: %s\n", blockchain[i].data);
        fprintf(file, "Hash: %s\n", blockchain[i].hash);
        fprintf(file, "Previous Hash: %s\n", blockchain[i].previous_hash);
    }
    fclose(file);
}

// Function to load the blockchain from a file
int load_blockchain(struct Block *blockchain)
{
    FILE *file = fopen("blockchain.txt", "r");
    if (file == NULL)
    {
        printf("Unable to open file.\n");
        return -1;
    }
    int i = 0;
    while (fscanf(file, "Block %d\n", &blockchain[i].index) != EOF)
    {
        fgets(blockchain[i].timestamp, 50, file);
        fgets(blockchain[i].data, 50, file);
        fscanf(file, "Hash: %s\n", blockchain[i].hash);
        fscanf(file, "Previous Hash: %s\n", blockchain[i].previous_hash);
        i++;
    }
    fclose(file);
    return i;
}

// Main function
int main()
{
    struct Block blockchain[10];
    int blockchain_length = load_blockchain(blockchain);
    if (blockchain_length == -1)
    {
        blockchain[0] = create_genesis_block();
        printf("Block 0 has been added to the blockchain!\n");
        printf("Hash: %s\n", blockchain[0].hash);
        blockchain_length = 1;
    }
    int choice, run = 1;
    char data[50];
    while (run)
    {
        printf("1. Add a new block\n");
        printf("2. Validate the blockchain\n");
        printf("3. Display the blockchain\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            printf("Enter data for the new block: ");
            scanf("%s", data);
            struct Block new_block = create_new_block(blockchain[blockchain_length - 1], data);
            blockchain[blockchain_length] = new_block;
            blockchain_length++;
            printf("Block %d has been added to the blockchain!\n", new_block.index);
            printf("Hash: %s\n", new_block.hash);
            break;
        case 2:
            if (is_blockchain_valid(blockchain, blockchain_length))
            {
                printf("The blockchain is valid.\n");
            }
            else
            {
                printf("The blockchain is not valid.\n");
            }
            break;
        case 3:
            display_blockchain(blockchain, blockchain_length);
            break;
        case 4:
            save_blockchain(blockchain, blockchain_length);
            run = 0;
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
    save_blockchain(blockchain, blockchain_length);
    return 0;
}
