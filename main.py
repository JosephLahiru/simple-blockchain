import hashlib
import datetime
import pickle
import atexit


class Block:
    def __init__(self, index, previous_hash, timestamp, data, hash):
        self.index = index
        self.previous_hash = previous_hash
        self.timestamp = timestamp
        self.data = data
        self.hash = hash


def calculate_hash(index, previous_hash, timestamp, data):
    value = str(index) + str(previous_hash) + str(timestamp) + str(data)
    return hashlib.sha256(value.encode('utf-8')).hexdigest()


def create_genesis_block():
    return Block(0, '0', datetime.datetime.now(), 'Genesis Block', calculate_hash(0, '0', datetime.datetime.now(), 'Genesis Block'))


def create_new_block(previous_block, data):
    index = previous_block.index + 1
    timestamp = datetime.datetime.now()
    hash = calculate_hash(index, previous_block.hash, timestamp, data)
    return Block(index, previous_block.hash, timestamp, data, hash)


def is_chain_valid(blockchain):
    for i in range(1, len(blockchain)):
        current_block = blockchain[i]
        previous_block = blockchain[i - 1]
        if current_block.hash != calculate_hash(current_block.index, current_block.previous_hash, current_block.timestamp, current_block.data):
            print('Hash does not match contents of block %d.' %
                  current_block.index)
            return False
        if current_block.previous_hash != previous_block.hash:
            print('Block %d is not linked correctly to block %d.' %
                  (current_block.index, previous_block.index))
            return False
    return True


def save_blockchain(blockchain, filename='blockchain.p'):
    with open(filename, 'wb') as f:
        pickle.dump(blockchain, f)


def load_blockchain(filename='blockchain.p'):
    with open(filename, 'rb') as f:
        return pickle.load(f)


def exit_handler():
    print("Saving blockchain before exit...")
    save_blockchain(blockchain)
    print("Blockchain saved.")


# Register the exit handler
atexit.register(exit_handler)

# Load the blockchain
try:
    blockchain = load_blockchain()
    previous_block = blockchain[-1]
    print("Blockchain loaded.")
except (FileNotFoundError, EOFError):
    # If the file doesn't exist or is empty, create a new blockchain
    blockchain = [create_genesis_block()]
    previous_block = blockchain[0]
    print("New blockchain created.")

while True:
    print("\n1. Add a new block")
    print("2. Check blockchain validity")
    print("3. Display blockchain")
    print("4. Quit\n")
    choice = input("What do you want to do? ")

    if choice == '1':
        data = input("\nEnter block data: ")
        new_block = create_new_block(previous_block, data)
        blockchain.append(new_block)
        previous_block = new_block
        print('Block #{} has been added to the blockchain!'.format(new_block.index))
        print('Hash: {}\n'.format(new_block.hash))
    elif choice == '2':
        if is_chain_valid(blockchain):
            print("\nThe blockchain is valid.")
        else:
            print("\nThe blockchain is not valid.")
    elif choice == '3':
        for block in blockchain:
            print("\nBlock #{} has been added to the blockchain!".format(block.index))
            print("Hash: {}".format(block.hash))
            print("Data: {}".format(block.data))
            print("Timestamp: {}\n".format(block.timestamp))
    elif choice == '4':
        break
    else:
        print("Invalid choice, please enter a number from 1 to 4.")
