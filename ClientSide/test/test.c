#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define KEY_LENGTH 2048
#define PUB_EXP 65537

void generate_keypair()
{
    RSA *keypair = RSA_new();
    BIGNUM *bne = BN_new();

    if (BN_set_word(bne, PUB_EXP) != 1)
    {
        fprintf(stderr, "Error setting BIGNUM exponent\n");
        exit(EXIT_FAILURE);
    }

    if (RSA_generate_key_ex(keypair, KEY_LENGTH, bne, NULL) != 1)
    {
        fprintf(stderr, "Error generating RSA keypair\n");
        exit(EXIT_FAILURE);
    }

    FILE *private_key_file = fopen("private_key.pem", "wb");
    FILE *public_key_file = fopen("public_key.pem", "wb");

    if (!private_key_file || !public_key_file)
    {
        fprintf(stderr, "Error opening key files\n");
        exit(EXIT_FAILURE);
    }

    if (PEM_write_RSAPrivateKey(private_key_file, keypair, NULL, NULL, 0, NULL, NULL) != 1)
    {
        fprintf(stderr, "Error writing private key to file\n");
        exit(EXIT_FAILURE);
    }

    if (PEM_write_RSAPublicKey(public_key_file, keypair) != 1)
    {
        fprintf(stderr, "Error writing public key to file\n");
        exit(EXIT_FAILURE);
    }

    fclose(private_key_file);
    fclose(public_key_file);

    RSA_free(keypair);
    BN_free(bne);
}

void encrypt_decrypt_example()
{
    FILE *public_key_file = fopen("public_key.pem", "rb");
    FILE *private_key_file = fopen("private_key.pem", "rb");

    if (!public_key_file || !private_key_file)
    {
        fprintf(stderr, "Error opening key files\n");
        exit(EXIT_FAILURE);
    }

    RSA *public_key = PEM_read_RSAPublicKey(public_key_file, NULL, NULL, NULL);
    RSA *private_key = PEM_read_RSAPrivateKey(private_key_file, NULL, NULL, NULL);

    fclose(public_key_file);
    fclose(private_key_file);

    if (!public_key || !private_key)
    {
        fprintf(stderr, "Error reading key files\n");
        exit(EXIT_FAILURE);
    }

    const char *plaintext = "Hello, RSA!";
    size_t plaintext_len = strlen(plaintext);

    // Encryption
    unsigned char ciphertext[KEY_LENGTH / 8];
    if (RSA_public_encrypt(plaintext_len, (const unsigned char *)plaintext, ciphertext, public_key, RSA_PKCS1_PADDING) == -1)
    {
        fprintf(stderr, "Error encrypting data\n");
        exit(EXIT_FAILURE);
    }

    // Decryption
    unsigned char decryptedtext[KEY_LENGTH / 8 + 1]; // +1 for null-termination
    int decrypted_length = RSA_private_decrypt(KEY_LENGTH / 8, ciphertext, decryptedtext, private_key, RSA_PKCS1_PADDING);
    if (decrypted_length == -1)
    {
        fprintf(stderr, "Error decrypting data\n");
        exit(EXIT_FAILURE);
    }

    decryptedtext[decrypted_length] = '\0';

    printf("Original: %s\n", plaintext);
    printf("Encrypted: ");
    for (size_t i = 0; i < KEY_LENGTH / 8; ++i)
    {
        printf("%02X", ciphertext[i]);
    }
    printf("\n");
    printf("Decrypted: %s\n", decryptedtext);

    RSA_free(public_key);
    RSA_free(private_key);
}

int main()
{
    generate_keypair();
    encrypt_decrypt_example();

    return 0;
}
