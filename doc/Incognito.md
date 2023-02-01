
# Incognito Nano App: Technical Specifications

This page details the protocol implemented of the Incognito Nano App.

## Framework

### APDUs

The messaging format of the app is compatible with the [APDU protocol](https://developers.ledger.com/docs/nano-app/application-structure/#apdu-interpretation-loop).

The main commands use `CLA = 0xE0`

| CLA | INS | COMMAND NAME        | DESCRIPTION |
|-----|-----|---------------------|-------------|
|  E0 |  01 | INS_GET_APP_VERSION | Return the current version of the app |
|  E0 |  02 | INS_GET_ADDR     | Return and show on screen an address for a current wallet |
|  E0 |  03 | INS_GET_VIEW  | Return and show on screen a view key for a current wallet |
|  E0 |  05 | INS_SWITCH_KEY| Switch to a wallet based on an index|
|  E0 |  06 | INS_GET_OTA  | Return an OTA key for a current wallet |
|  E0 |  08 | INS_KEY_IMG  | Return a key image of coin |
|  E0 |  21 | INS_GEN_ALPHA  | Generate a set of random values|
|  E0 |  22 | INS_CALC_C  | Compute and return C (one of step to sign a transaction)|
|  E0 |  23 | INS_CALC_R  | Compute and return R (one of step to sign a transaction)|
|  E0 |  24 | INS_COIN_PRIV  | Compute and return a private key of coin |
|  E0 |  27 | INS_DECRYPT_COIN  | Decrypt and return a value of coin |
|  E0 |  28 | INS_CALC_CCA  | Compute and return C for a confidential asset transaction|
|  E0 |  60 | INS_TRUST_DVC  | Set to trust the device|
|  E0 |  61 | INS_CONFIRM_TX  | Confirm to sign a transaction|



## Status Words

| SW     | SW name                      | Description |
|--------|------------------------------|-------------|
| 0x6985 | `SW_DENY`                    | Rejected by user |
| 0x6A86 | `SW_WRONG_P1P2`              | Either `P1` or `P2` is incorrect |
| 0x6A87 | `SW_WRONG_DATA_LENGTH`       | `Lc` or minimum APDU length is incorrect |
| 0x6D00 | `SW_INS_NOT_SUPPORTED`       | No command exists with `INS` |
| 0x6E00 | `SW_CLA_NOT_SUPPORTED`       | Bad `CLA` used for this application |
| 0xB000 | `SW_WRONG_RESPONSE_LENGTH`   | Wrong response length (buffer size problem) |
| 0xB007 | `SW_BAD_STATE`               | Abrted because unexpected state reached |
| 0xB008 | `SW_SIGNATURE_FAIL`          | Invalid signature|
| 0x9000 | `SW_OK`                      | Success |


## Commands


### INS_GET_APP_VERSION
This command returns the current version of Incognito Nano App

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 01    |

**Input data** No input data

**Output data**
| Length | Description |
|--------|-------------|
| `3`| Current version|

### INS_GET_ADDR
This command allows requesting the address of the current wallet of the device. PaymentAddress consists of the following components.
-   ***PublicKey***. It is used as a way to designate UTXOs to the receiver.
-   ***PublicViewKey***. It is used to encrypt the transferred amount.
-   ***PublicOTAKey***. It is used to create  [one-time addresses](https://we.incognito.org/t/one-time-address-improve-on-privacy-and-pdex-performance/6680), and conceal (encrypt) the assets transacted ([confidential assets](https://we.incognito.org/t/ongoing-confidential-asset/5607)). 

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 02    |

**Input data** No input data
**Output data**: Return the current payment address in string format. For example `12swnTiikvBhfDRhwiC39LjxPe18pSpGSQftwjqxWGmZorpVUre3SKXGRLmJxCQpfNQNA4CrK9C423QHTo4v6aLoPrMGtqBdztGaYPfRjeJFqAPsyRbr7BRvSRd5umtTwiHHuimxjEVFGAgiuZZN`


### INS_GET_VIEW
Return the `PrivateViewKey` of the current wallet used to decrypt the amount of the received transaction, after validating it with the user using the trusted screen.

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 03    |

**Input data** No input data
**Output data**

| Length      | Description     |
|-------------|-----------------|
| `32`  | Return the `PrivateViewKey` of the current wallet |

### INS_SWITCH_KEY
Change to a new wallet based on the index number after validating it with the user using the trusted screen.

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 05    |

**Input data**

|Param | Length  | Name                   | Description |
|------|---------|------------------------|-------------|
| `dataBuffer` | `4`| `account_number`      | The index number of wallet  |


**Output data**

| Length      | Description     |
|-------------|-----------------|
| `4`  | The index number of wallet|
| `96`| The payment address|


### INS_GET_OTA
Return the `PrivateOTAKey` of the current wallet used to filter and decrypt the token in the received transaction, after validating it with the user using the trusted screen.

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 05    |

**Input data** No input data.

**Output data**

| Length | Description                |
|--------|----------------------------|
| `32`    | Return the `PrivateOTAKey` of the current wallet   |

### INS_KEY_IMG
Return the `KeyImage` of a coin. 

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 05    |

**Input data** 
| Param | Length  | Name                   | Description |
| ------|---------|------------------------|-------------|
|`databuffer`| `32` | `encryptKmB64`      |Hash of `TxRandomDetail` and `index` that get from a coin data  |
| | `32` | `coinPubKeyB64`      | Coin public key in Base64 format  |

**Output data**

| Length | Description                |
|--------|----------------------------|
| `32`    | Return the `keyImage` of a coin|


### INS_GEN_ALPHA
Generate a set of random values that are used to sign a transaction. Note that we have two sets: `alpha` is used to transfer PRV and `alphaToken` is used to transfer Token. 

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 21    |

**Input data** 

| Param  | Length| Name            | Description |
|---------|--------|-----------------|-------------|
| `p1`     | `1` | `number`     | Number of alpha will be generated (maximum 32) |
| `p2`     | `1`   | `isToken`| Flag to determine to generate a set of `alpha` or `alphaToken`|

**Output data** No output data

### INS_CALC_C
Calculate C - one of steps to sign a transaction

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 22    |

**Input data** 

| Param  | Length| Name            | Description |
|---------|--------|-----------------|-------------|
| `databuffer` | `32` | `H`     | Input value to calculate C |
| `p1`     | `1` | `number`     | If `p1=1`,`G` (base point of curve ED25519) is used to calculate C, otherwise, `H` will be used|

**Output data** 
| Length | Description                |
|--------|----------------------------|
| `32`| Return the result of the calculation|

### INS_CALC_R
Calculate R - one of steps to sign a transaction

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 23    |

**Input data** 

| Param  | Length| Name            | Description |
|---------|--------|-----------------|-------------|
| `databuffer` | `32` | `cPi`     | Input value to calculate R, that is the output of `INS_CALC_C`|
| `p1`     | `1` | `number`     | Flag to determine that uses `alpha` or `alphaToken` to transfer PRV or token corresponsding|
| `p2`  | `1` | `number` | The index of input coins will be used in the transaction (maximum 32)

**Output data** 
| Length | Description                |
|--------|----------------------------|
| `32`| Return the result of the calculation|



### INS_COIN_PRIV
Calculate coin's private key.

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 24    |

**Input data** 

| Param  | Length| Name            | Description |
|---------|--------|-----------------|-------------|
| `databuffer` | `32` | `coinH`     | Coin data that is used to calculate a coin private key|
| `p1`     | `1` | `number`     | Flag to determine the way to calculate the coin private key |
| `p2`  | `1` | `number` | Index of input coin|


**Output data**  No output data



### INS_DECRYPT_COIN
Decrypt coin value

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 27    |

**Input data** 

| Param  | Length| Name            | Description |
|---------|--------|-----------------|-------------|
| `databuffer` | `32` | `txConcealRnd`     | Transaction conceal random point|
|     | `32` | `coinAmtStr`     | Cyphertext of encrypted amount |
|  | `32` | `coinRndStr` | Coin randomness  |


**Output data**  No output data
| Length | Description                |
|--------|----------------------------|
| `32`| Amount|
| `32`| Mask|

### INS_CALC_CCA
Calculate C - one of the steps to sign a confidential asset transaction. The different compare with calculate C in the transfer PRV transaction is the list of `alphaToken` to be used instead of `alpha`.

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 28    |

**Input data** 

| Param  | Length| Name            | Description |
|---------|--------|-----------------|-------------|
| `databuffer` | `32` | `H`     | Input value to calculate C |
| `p1`     | `1` | `number`     | If `p1=1`,`G` is used to calculate C, otherwise, `H` will be used|

**Output data** 
| Length | Description                |
|--------|----------------------------|
| `32`| Return the result|


### INS_CONFIRM_TX
Confirm transaction is used to show the transaction detail to the user using the trusted screen.

**Command**

| *CLA* | *INS* |
|-------|-------|
| E0    | 61    |

**Input data**: 

| Param  | Length| Name            | Description |
|---------|--------|-----------------|-------------|
| `databuffer` | `8` | `amount`     | The amount to transfer |
|     | `96` | `address`     | The address of a receiver|

**Output data** : No output data


