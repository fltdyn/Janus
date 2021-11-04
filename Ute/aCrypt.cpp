//
// DST Ute Library (Utilities Library)
//
// Defence Science and Technology (DST) Group
// Department of Defence, Australia.
// 506 Lorimer St
// Fishermans Bend, VIC
// AUSTRALIA, 3207
//
// Copyright 2005-2018 Commonwealth of Australia
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

//
// Title: aCrypt
// Class: aCrypt
//
// Description:
//   Encrypts/decrypts data using OpenSSL.
//
// Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 07/07/2014
// Based On: AsciiDataClass from DSTO SDHEngSoft library.
//

#include "UteConfig.h"
#ifdef HAVE_OPENSSL

// Local Includes
#include "aCrypt.h"
#include "aMessageStream.h"

// C/C++ Includes
#include <iostream>
#include <iomanip>
#include <sstream>

// OpenSSL Includes
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

using namespace std;

namespace dstoute {

  aCrypt::aCrypt()
   : encryptCtx_( 0),
     decryptCtx_( 0),
     rsaKeys_( 0),
     aesKey_( 0)
  {
  }

  aCrypt::~aCrypt()
  {
    if ( encryptCtx_) {
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
      EVP_CIPHER_CTX_free( encryptCtx_);
#else
      EVP_CIPHER_CTX_cleanup( encryptCtx_);
      delete encryptCtx_;
#endif
    }
    if ( decryptCtx_) {
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
      EVP_CIPHER_CTX_free( decryptCtx_);
#else
      EVP_CIPHER_CTX_cleanup( decryptCtx_);
      delete decryptCtx_;
#endif
    }
    if ( rsaKeys_) RSA_free( rsaKeys_);
    if ( aesKey_)  free( aesKey_);
  }

  void aCrypt::initCipher()
  {
    if ( !aesKey_) {
      throw_message( logic_error, "aCrypt::initCipher - AES key not set.");
    }

    /*
     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
     * nRounds is the number of times the we hash the material. More rounds are more secure but
     * slower.
     */
    int nRounds = 5;
    unsigned int salt[] = {12345, 54321};
    unsigned char key[ AES_KEYLEN], iv[ AES_KEYLEN];
    if ( EVP_BytesToKey( EVP_aes_256_cbc(), EVP_sha1(), (unsigned char*) salt,
         aesKey_, AES_KEYLEN, nRounds, key, iv) != int( AES_KEYLEN)) {
      throw_message( logic_error, aString( "aCrypt::initCipher - Key size is not % bits.").arg( AES_KEYLEN_BITS));
    }

    if ( !encryptCtx_) {
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
      encryptCtx_ = EVP_CIPHER_CTX_new();
#else
      encryptCtx_ = new EVP_CIPHER_CTX;
#endif
    }
    else {
      EVP_CIPHER_CTX_cleanup( encryptCtx_);
    }
    if ( !decryptCtx_) {
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
      decryptCtx_ = EVP_CIPHER_CTX_new();
#else
      decryptCtx_ = new EVP_CIPHER_CTX;
#endif
    }
    else {
      EVP_CIPHER_CTX_cleanup( decryptCtx_);
    }
    EVP_CIPHER_CTX_init( encryptCtx_);
    EVP_CIPHER_CTX_init( decryptCtx_);
    if ( EVP_EncryptInit_ex(  encryptCtx_, EVP_aes_256_cbc(), NULL, key, iv) != 1 ||
         EVP_DecryptInit_ex(  decryptCtx_, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
      throw_message( logic_error, "aCrypt::initCipher - Can't initialise cipher.");
    }
  }

  RSA* aCrypt::generateRandomRsaKeys()
  {
    /*
     * generate the keys, after checking the random number generator
     * for adequate seeding
     */
    if ( RAND_status() != 1) {
      throw_message( logic_error, "aCrypt::generateRandomRsaKeys - OpenSSL random generation not initialised.");
    }

#if OPENSSL_VERSION_NUMBER >= 0x00908000L
    RSA* rsaKeys = RSA_new();
    int ret = 0;
    BIGNUM *bn = 0;
    try {
      bn = BN_new();
      BN_set_word( bn, 17);
      ret = RSA_generate_key_ex ( rsaKeys, 2048, bn, 0);
      BN_free (bn);
    }
    catch (...) {
      BN_free (bn);
      throw_message( logic_error, aString( "aCrypt::generateRandomRsaKeys - RSA key generation failed with error: %").arg( ERR_get_error()));
    }
    if ( !ret) {
      throw_message( logic_error, aString( "aCrypt::generateRandomRsaKeys - RSA key generation failed with error: %").arg( ERR_get_error()));
    }
#else
    RSA* rsaKeys = 0;
    if ( !( rsaKeys = RSA_generate_key( 2048, 17, NULL, NULL))) {
      throw_message( logic_error, aString( "aCrypt::generateRandomRsaKeys - RSA key generation failed with error: %").arg( ERR_get_error()));
    }
#endif

    return rsaKeys;
  }

  void aCrypt::writeRsaKeys( const aString &baseFileName)
  {
    RSA* rsaKeys = generateRandomRsaKeys();

    //
    // Write the private key.
    //
    BIO *privateFile = 0;
    if ( !( privateFile = BIO_new( BIO_s_file()))) {
      throw_message( logic_error, "aCrypt::writeRsaKeys - Can't create private key BIO.");
    }
    if ( BIO_write_filename( privateFile, ( void* ) aString( "%_Private.key").arg( baseFileName).c_str()) != 1) {
      throw_message( std::logic_error,
        aString( "aCrypt::writeRsaKeys - Can't set private key BIO file name \"%_Private.key\".").arg( baseFileName));
    }
    if ( PEM_write_bio_RSAPrivateKey( privateFile, rsaKeys, NULL, NULL, 0, NULL, NULL) != 1) {
      throw_message( logic_error, "aCrypt::writeRsaKeys - Can't write private key.");
    }
    BIO_free( privateFile);

    //
    // Write the public key.
    //
    BIO * publicFile = 0;
    if ( !( publicFile = BIO_new( BIO_s_file()))) {
      throw_message( logic_error, "aCrypt::writeRsaKeys - Can't create public key BIO.");
    }
    if ( BIO_write_filename( publicFile, ( void* ) aString( "%_Public.key").arg( baseFileName).c_str()) != 1) {
      throw_message( logic_error,
        aString( "aCrypt::writeRsaKeys - Can't set public key BIO file name \"%_Public.key\".").arg( baseFileName));
    }

    if ( PEM_write_bio_RSAPublicKey( publicFile, rsaKeys) != 1) {
      throw_message( runtime_error, "aCrypt::writeRsaKeys - Can't write public key.");
      exit( 1);
    }
    BIO_free( publicFile);

    RSA_free( rsaKeys);
  }

  void aCrypt::setRsaKeys( RSA *rsaKeys)
  {
    if ( !rsaKeys) {
      throw_message( logic_error, "aCrypt::setRsaKeys - No RSA key provided.");
    }
    if ( rsaKeys_) RSA_free( rsaKeys_);
    rsaKeys_ = rsaKeys;
  }

  void aCrypt::setPrivateRsaKey( const aString &fileName)
  {
    BIO *bioFile = 0;
    if ( !(bioFile = BIO_new( BIO_s_file()))) {
      throw_message( logic_error, "aCrypt::setPrivateRsaKey - Can't create empty BIO file.");
    }
    if ( BIO_read_filename( bioFile, fileName.c_str()) != 1) {
      throw_message( logic_error, aString("aCrypt::setPrivateRsaKey - Can't read BIO file \"%\".").arg( fileName));
    }
    EVP_PKEY *evpPKey = 0;
    if ( !( evpPKey = PEM_read_bio_PrivateKey( bioFile, NULL, NULL, NULL))) {
      throw_message( logic_error, aString("aCrypt::setPrivateRsaKey - Can't read private RSA key from file \"%\".").arg( fileName));
    }
    BIO_free( bioFile);

    debug_code(
      PEM_write_PrivateKey( stdout, evpPKey, NULL, NULL, 0, 0, NULL);
      fflush( stdout);
    );

    if ( !( rsaKeys_ = EVP_PKEY_get1_RSA( evpPKey))) {
      throw_message( logic_error, "aCrypt::setPrivateRsaKey - Can't extract RSA key.");
    }
    EVP_PKEY_free( evpPKey);
  }

  void aCrypt::generateRandomAesKey()
  {
    if ( !aesKey_) {
      aesKey_ = (unsigned char*) malloc( AES_KEYLEN);
    }

    if ( RAND_status() != 1) {
      throw_message( logic_error, "aCrypt::generateRandomAesKey - OpenSSL RAND not initialised.");
    }
    if( RAND_bytes( aesKey_, AES_KEYLEN) == 0) {
      throw_message( logic_error, "aCrypt::generateRandomAesKey - Can't create AES key.");
    }

    debug_code(
      cout << "AES key (HEX): ";
      for( size_t i=0; i < AES_KEYLEN; i++) {
          printf("%x", aesKey_[i]);
      }
      cout << endl;
    );
  }

  void aCrypt::setPublicRsaKey( const aString &fileName)
  {
    if ( rsaKeys_) {
      RSA_free( rsaKeys_);
      rsaKeys_ = 0;
    }

    BIO *bioFile      = 0;
    if ( !( bioFile = BIO_new( BIO_s_file()))) {
      throw_message( logic_error, "aCrypt::setPublicRsaKey - Can't create empty BIO file.");
    }
    if ( BIO_read_filename( bioFile, fileName.c_str()) != 1) {
      throw_message( logic_error, aString("aCrypt::setPublicRsaKey - Can't read BIO file \"%\".").arg( fileName));
    }
    if ( !( rsaKeys_ = PEM_read_bio_RSAPublicKey( bioFile, NULL, NULL, NULL))) {
      throw_message( logic_error, aString("aCrypt::setPublicRsaKey - Can't read public RSA key from file \"%\".").arg( fileName));
    }
    EVP_PKEY *evpPKey = 0;
    if ( !( evpPKey = EVP_PKEY_new())) {
      throw_message( logic_error, "aCrypt::setPublicRsaKey - Can't create empty EVP_PKEY.");
    }
    if ( EVP_PKEY_set1_RSA( evpPKey, rsaKeys_) != 1) {
      throw_message( logic_error, "aCrypt::setPublicRsaKey - Can't set EVP_PKEY from RSA key.");
    }
    BIO_free( bioFile);

    debug_code(
      PEM_write_PUBKEY( stdout, evpPKey);
      fflush( stdout);
    );

    EVP_PKEY_free( evpPKey);
  }

  size_t aCrypt::rsaEncryptBuffer( const unsigned char *decData, size_t decDataLength, unsigned char* &encData)
  {
    if ( !rsaKeys_) {
      throw_message( logic_error, "aCrypt::rsaEncryptBuffer - Public RSA key not set.");
    }
    if ( !aesKey_) {
      generateRandomAesKey();
      initCipher();
    }

    // Encrypt the data.
    size_t encDataLength = aesEncrypt( decData, static_cast<int>( decDataLength), encData); // Actual buffer ENCRYPT_BUFFER longer.

    // Encrypt the AES key and add to end of data buffer.
    int encAesKeyLength = 0;
    if (( encAesKeyLength = RSA_public_encrypt( AES_KEYLEN, aesKey_, encData + encDataLength,
      rsaKeys_, RSA_PKCS1_PADDING)) != RSA_size( rsaKeys_)) {
      throw_message( logic_error, "aCrypt::rsaEncryptBuffer - Can't encrypt AES key.");
    }

    return encDataLength + encAesKeyLength;
  }

  void aCrypt::rsaEncryptFile( const aString &inFileName, const aString &outFileName)
  {
    if ( !rsaKeys_) {
      throw_message( logic_error, "aCrypt::rsaEncryptFile - Public RSA key not set.");
    }
    if ( !aesKey_) {
      generateRandomAesKey();
      initCipher();
    }

    // Encrypt the data.
    unsigned char *decData = 0; // Allocated by readFile().
    size_t decDataLength   = readFile( inFileName, decData);
    size_t encDataLength   = rsaEncryptBuffer( decData, decDataLength, decData); // Write back to same buffer!

    writeFile( outFileName.empty() ? inFileName : outFileName, decData, encDataLength);

    free( decData);
  }

  size_t aCrypt::rsaDecryptBuffer( const unsigned char *encData, size_t encDataLength, unsigned char* &decData)
  {
    if ( !rsaKeys_) {
      throw_message( logic_error, "aCrypt::rsaDecryptBuffer - Private RSA key not set.");
    }

    if ( !aesKey_) {
      aesKey_ = (unsigned char*) malloc( AES_KEYLEN);
    }
    // Note: AES key at end of file.
    if ( RSA_private_decrypt( AES_KEYLEN_BITS, encData + encDataLength - AES_KEYLEN_BITS,
                              aesKey_, rsaKeys_, RSA_PKCS1_PADDING) != int( AES_KEYLEN)) {
      throw_message( logic_error, "aCrypt::rsaDecryptBuffer - RSA key not valid.");
    }
    initCipher();

    debug_code(
      cout << "AES key (HEX): ";
      for( size_t i=0; i < AES_KEYLEN; i++) {
          printf("%x", aesKey_[i]);
      }
      printf( "\n");
      fflush( stdout);
    );

    // Note: AES key at end of file.
    size_t decDataLength = aesDecrypt( encData, static_cast<int>( encDataLength - AES_KEYLEN_BITS), decData);

    debug_code(
      cout << "decDataLength: " << decDataLength << endl;
    );

    return decDataLength;
  }

  void aCrypt::rsaDecryptFile( const aString &inFileName, const aString &outFileName)
  {
    if ( !rsaKeys_) {
      throw_message( logic_error, "aCrypt::rsaDecryptFile - Private RSA key not set.");
    }

    unsigned char *encData = 0; // Allocated by readFile().
    size_t encDataLength = readFile( inFileName, encData);
    size_t decDataLength = rsaDecryptBuffer( encData, encDataLength, encData); // Write back to same buffer!
    writeFile( outFileName.empty() ? inFileName : outFileName, encData, decDataLength);

    free( encData);
  }

  size_t aCrypt::aesEncrypt( const unsigned char *decData, int decDataLength, unsigned char* &encData)
  {
    /* Max cipher text length for n bytes of plain text is n + AES_BLOCK_SIZE - 1 bytes */
    int encLen = decDataLength + AES_BLOCK_SIZE + ENCRYPT_BUFFER; // Just in case we want to add AES key to end.
    int tmpLen = 0;
    if ( !encData) {
      encData = (unsigned char *) malloc( encLen);
    }

    if ( EVP_EncryptInit_ex( encryptCtx_, NULL, NULL, NULL, NULL) != 1 ||
         EVP_EncryptUpdate( encryptCtx_, encData, &encLen, decData, decDataLength) != 1 ||
         EVP_EncryptFinal_ex( encryptCtx_, encData + encLen, &tmpLen) != 1) {
      throw_message( logic_error, "aCrypt::aesEncrypt - AES encryption failed.");
    }

    return encLen + tmpLen;
  }

  size_t aCrypt::aesDecrypt( const unsigned char *encData, int encDataLength, unsigned char* &decData)
  {
    /* Because we have padding ON, we must allocate an extra cipher block size of memory */
    int decLen = encDataLength;
    int tmpLen = 0;

    if ( !decData) {
      decData = (unsigned char *) malloc( decLen + AES_BLOCK_SIZE);
    }

    if ( EVP_DecryptInit_ex( decryptCtx_, NULL, NULL, NULL, NULL) != 1 ||
         EVP_DecryptUpdate( decryptCtx_, decData, &decLen, encData, encDataLength) != 1 ||
         EVP_DecryptFinal_ex( decryptCtx_, decData + decLen, &tmpLen) != 1) {
      throw_message( logic_error, "aCrypt::aesDecrypt - AES decryption failed.");
    }

    return decLen + tmpLen;
  }

  void aCrypt::writeFile( const aString &fileName, const unsigned char *fileData, size_t fileDataLength, bool doAppend)
  {
    FILE *fd = 0;
    if ( !( fd = fopen( fileName.c_str(), doAppend ? "ab" : "wb"))) {
      throw_message( logic_error, aString( "aCrypt::writeFile - Can't open file \"%\".").arg( fileName));
    }

    if ( fwrite( fileData, 1, fileDataLength, fd) != fileDataLength) {
      throw_message( logic_error, aString( "aCrypt::writeFile - Can't write to file \"%\".").arg( fileName));
    }

    debug_code(
      cout << "writeBuffer length: " << fileDataLength << endl;
    );

    fclose( fd);
  }

  size_t aCrypt::readFile( const aString &fileName, unsigned char* &fileData)
  {
    FILE *fd = 0;
    if ( !( fd = fopen( fileName.c_str(), "rb"))) {
      throw_message( logic_error, aString( "aCrypt::readFile - Can't open file \"%\".").arg( fileName));
    }

    // Determine size of the file
    fseek( fd, 0, SEEK_END);
    size_t fileDataLength = ftell( fd);
    fseek( fd, 0, SEEK_SET);

    // Allocate space for the file
    fileData = ( unsigned char*) malloc( fileDataLength + ENCRYPT_BUFFER); // Add a bit to allow encrypt/decrypt to same buffer.
    if ( !fileData) {
      throw_message( logic_error, aString( "aCrypt::readFile - Can't allocate memory for file \"%\".").arg( fileName));
    }

    // Read the file into the buffer
    if ( fread( fileData, 1, fileDataLength, fd) != fileDataLength) {
      throw_message( logic_error, aString( "aCrypt::readFile - Can't read data from file \"%\".").arg( fileName));
    }
    fclose( fd);

    debug_code(
      cout << "readBuffer length: " << fileDataLength << endl;
    );

    return fileDataLength;
  }

  aString aCrypt::md5hash( const unsigned char *buffer, size_t bufferSize)
  {
    const int hashSize = MD5_DIGEST_LENGTH;
    unsigned char hash[ hashSize];
    MD5( (unsigned char*) buffer, bufferSize, hash);
    ostringstream hashOut;
    for ( int i = 0; i < hashSize; ++i) {
      hashOut << hex << setw(2) << setfill('0') << int( hash[ i]);
    }
    return hashOut.str();
}

  aString aCrypt::sha256hash( const unsigned char *buffer, size_t bufferSize)
  {
    const int hashSize = SHA256_DIGEST_LENGTH;
    unsigned char hash[ SHA256_DIGEST_LENGTH];
    SHA256( (unsigned char*) buffer, bufferSize, hash);
    ostringstream hashOut;
    for ( int i = 0; i < hashSize; ++i) {
      hashOut << hex << setw(2) << setfill('0') << int( hash[ i]);
    }
    return hashOut.str();
  }

} // namespace dstoute

#endif // HAVE_OPENSSL
