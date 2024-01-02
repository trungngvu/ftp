#define KEY_LENGTH 2048
#define PUB_EXP 65537

RSA *public_key;
RSA *private_key;
RSA *server_public_key;

int readKey()
{
    FILE *public_key_file = fopen("./RSA/public_key.pem", "rb");
    FILE *private_key_file = fopen("./RSA/private_key.pem", "rb");

    if (!public_key_file || !private_key_file)
    {
        fprintf(stderr, "Error opening key files\n");
        exit(EXIT_FAILURE);
    }

    public_key = PEM_read_RSAPublicKey(public_key_file, NULL, NULL, NULL);
    private_key = PEM_read_RSAPrivateKey(private_key_file, NULL, NULL, NULL);

    fclose(public_key_file);
    fclose(private_key_file);

    if (!public_key || !private_key)
    {
        fprintf(stderr, "Error reading key files\n");
        exit(EXIT_FAILURE);
    }
}

// Function to convert an RSA key to a string
char *rsa_key_to_string(RSA *key, int is_private)
{
    BIO *bio = BIO_new(BIO_s_mem());

    if (is_private)
    {
        PEM_write_bio_RSAPrivateKey(bio, key, NULL, NULL, 0, NULL, NULL);
    }
    else
    {
        PEM_write_bio_RSA_PUBKEY(bio, key);
    }

    size_t key_len = BIO_pending(bio);
    char *key_str = (char *)malloc(key_len + 1); // +1 for null-termination
    BIO_read(bio, key_str, key_len);
    key_str[key_len] = '\0';

    BIO_free(bio);
    return key_str;
}

// Function to convert a string to an RSA key
RSA *string_to_rsa_key(const char *key_str, int is_private)
{
    BIO *bio = BIO_new_mem_buf((void *)key_str, -1);

    RSA *key;
    if (is_private)
    {
        key = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
    }
    else
    {
        key = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
    }

    if (!key)
    {
        fprintf(stderr, "Error reading RSA key from string\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    BIO_free(bio);
    return key;
}

int sendEncrypted(int sock, char *plaintext, RSA *key)
{
    size_t plaintext_len = strlen(plaintext);
    // Encryption
    unsigned char ciphertext[KEY_LENGTH / 8];
    if (RSA_public_encrypt(plaintext_len, (const unsigned char *)plaintext, ciphertext, key, RSA_PKCS1_PADDING) == -1)
    {
        fprintf(stderr, "Error encrypting data\n");
        return -1;
    }
    send(sock, ciphertext, sizeof(ciphertext), 0);
    return 0;
}

int receiveDecrypted(int sock, char *decryptedtext, RSA *key)
{
    unsigned char ciphertext[KEY_LENGTH / 8];
    recv(sock, ciphertext, sizeof(ciphertext), 0);
    int decrypted_length = RSA_private_decrypt(KEY_LENGTH / 8, ciphertext, decryptedtext, key, RSA_PKCS1_PADDING);
    if (decrypted_length == -1)
    {
        return -1;
    }
    decryptedtext[decrypted_length] = '\0';
    return sizeof(decryptedtext);
}