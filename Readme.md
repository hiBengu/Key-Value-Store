# Key-Value Store

A key-value store prototype as a library implementation.

## Usage

Compile the code inside the directory

```bash
make
```

Start the client application with the following command

```bash
sudo chmod 777 start_client.sh
./start_client.sh
```

Then you can use commands from terminal.

```bash
PUT $key $val

GET $key

DELETE $key

# Sends random numbers to store
BATCH $batch_number

# Reads the desired saved file
READMAP $num
```
