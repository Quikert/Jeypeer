# Jeypeer
Jeypeer is a minimalist blockchain implementation with advanced cryptography and a simple P2P system for creating and sharing blocks. The project combines modern hashing with a lean C architecture, allowing for the exploration of blockchain concepts, event tokenization, and peer-to-peer communication in a direct and transparent way.

![Jeypeer](https://github.com/user-attachments/assets/802fdb58-2655-4071-bbd8-9ee7e90a7088)
---
## Key features
- Minimalist Blockchain
  - Creation of genesis block
  - Adding new blocks with arbitrary data
  = Chain integrity verification
- Cryptography
  - Hashing via sha256sum integrated into the shell
  - Alternative with its own implementation in C
- Tokenization
  - Breakdown of events/processes into tokens and logging
- Peers
  - TCP server that receives blocks and adds them to the chain
  - Client that sends blocks to peers
- Logs
  - The following files are used for tracking: blocks.log, process.log, and tokens.log
- Safe cleaning
  - The `clean` command overwrites and removes local files with double confirmation
 ---
## Structure for fools

 flowchart TD
    A[Jeypeer] --> B[init.sh]
    A --> C[jeypeer (binário)]
    A --> D[/c]
    A --> E[/crypt]
    A --> F[/logs]

    D --> D1[blocks.c]
    D --> D2[chains.c]
    D --> D3[peers.c]
    D --> D4[tokens.c]
    D --> D5[p2p.c]
    D --> D6[shellhash.c]
    D --> D7[process.c]

    E --> E1[crypt.c]

    F --> F1[blocks.log]
    F --> F2[process.log]
    F --> F3[tokens.log]
--- 
