// C++17 demo publisher. Requires libcurl development files.
// Example: ./sensor_simulator | ./sensor_publisher
#include <curl/curl.h>
#include <iostream>
#include <iterator>
#include <string>
int main(int argc, char** argv) {
    const std::string url = argc > 1 ? argv[1] : "http://localhost:5000/api/readings";
    const std::string payload((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
    if (payload.empty()) { std::cerr << "Provide JSON on stdin.\n"; return 2; }
    CURL* curl = curl_easy_init();
    if (!curl) return 3;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    CURLcode result = curl_easy_perform(curl);
    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    if (result == CURLE_OK) std::cout << "HTTP " << status << "\n";
    else std::cerr << "Request failed: " << curl_easy_strerror(result) << "\n";
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return result == CURLE_OK && status == 201 ? 0 : 1;
}
