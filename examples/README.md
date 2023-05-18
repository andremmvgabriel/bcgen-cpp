## Circuits with OR gates

| Function                | Input 1 Size | Input 2 Size | Output Size | No. ANDs | No. XORs | No. INVs | No. ORs |
| :---------------------: | :----------: | :----------: | :---------: | :------: | :------: | :------: | :-----: |
| Addition                | 64           | 64           | 64          | 190      | 129      | 1        | 63      |
| Subtract                | 64           | 64           | 64          | 190      | 192      | 64       | 63      |
| Negation                | 64           | -            | 64          | 64       | 1        | 65       | 0       |
| Signed Multiplication   | 64           | 64           | 128         | 20480    | 16385    | 127      | 8128    |
| Unsigned Multiplication | 64           | 64           | 128         | 20226    | 16129    | 1        | 8001    |
| Signed Division         | 64           | 64           | 64          | 28924    | 16451    | 12420    | 12414   |
| Unsigned Division       | 64           | 64           | 64          | 28162    | 16066    | 12225    | 12033   |
| Zero Equality           | 64           | 64           | 64          | 64       | 65       | 2        | 63      |
| AES-128 ECB Encryption  | 64           | 64           | 64          | 858784   | 450097   | 102401   | 783120  |
| AES-128 ECB Decryption  | 64           | 64           | 64          | 2262784  | 1613905  | 102401   | 1312320 |

## Circuits without OR gates

| Function                | Input 1 Size | Input 2 Size | Output Size | No. ANDs | No. XORs | No. INVs |
| :---------------------: | :----------: | :----------: | :---------: | :------: | :------: | :------: |
| Addition                | 64           | 64           | 64          | 253      | 129      | 190      |
| Subtract                | 64           | 64           | 64          | 253      | 192      | 253      |
| Negation                | 64           | -            | 64          | 64       | 1        | 65       |
| Signed Multiplication   | 64           | 64           | 128         | 28608    | 16385    | 24511    |
| Unsigned Multiplication | 64           | 64           | 128         | 28227    | 16129    | 24004    |
| Signed Division         | 64           | 64           | 64          | 41338    | 16451    | 49662    |
| Unsigned Division       | 64           | 64           | 64          | 40195    | 16066    | 48324    |
| Zero Equality           | 64           | 64           | 64          | 127      | 65       | 191      |
| AES-128 ECB Encryption  | 64           | 64           | 64          | 1641904  | 450097   | 2451761  |
| AES-128 ECB Decryption  | 64           | 64           | 64          | 3575104  | 1613905  | 4039361  |
