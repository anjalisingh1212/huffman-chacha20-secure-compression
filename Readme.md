# Huffman Compression + ChaCha20-Poly1305 Encryption Tool

This is a C-based command-line tool that compresses files using Huffman encoding and then encrypts them using the ChaCha20-Poly1305 AEAD cipher. It also supports decompression and decryption using the original password.

---

##  Features

- **Lossless compression** using Huffman coding
- **Strong encryption** using ChaCha20-Poly1305 (via [libsodium](https://libsodium.gitbook.io/doc/))
- **Password-based encryption key** derived via Argon2
- **Compression effectiveness analysis**
- **Edge case handling** (e.g., small files, repeated characters, unsupported types)
- **Safe output**: Prevents accidental overwriting of existing files
- **Clean error handling** and messages

---


## How It Works

**Compression Phase:**
- Builds Huffman tree based on character frequency.
- Encodes data using variable-length codes.
- Packs encoded data into bits.
- Encrypts the result using a user-provided password.

**Decompression Phase:**
- Decrypts data using the password.
- Reconstructs the Huffman tree.
- Decodes the bitstream back to original content.

## 🗂 Supported File Types

 **Recommended (text-based files):**

- `.txt`, `.log`, `.cfg`, `.csv`, `.c`, `.cpp`, `.py`, `.json`, etc.

 **Not Recommended (already compressed formats):**

- `.jpg`, `.png`, `.mp4`, `.pdf`, `.zip`, `.gz`, `.rar`, `.7z`, etc.

>  Compressing already-compressed or binary files may result in *larger* output due to encryption and metadata overhead.

---

## Compression Effectiveness Report
**After compression, the tool shows:**
- Original file size
- Compressed+encrypted file size
- % space saved or increased
- Reason if compression was ineffective (e.g., input type, encryption overhead)

## Build Instructions

### Prerequisites

- GCC or Clang
- [libsodium](https://libsodium.gitbook.io/doc/installation)

#### On Debian/Ubuntu:

```bash
sudo apt update
sudo apt install libsodium-dev
```

## Build the Project
**Option 1: Using the provided script:**
    ./build_and_run
**Option 2: Compile manually:** 
    gcc -o run main.c compressor.c decompressor.c huffman.c encryptor.c decryptor.c utils.c -lsodium
