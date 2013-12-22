#include "AbstractRestBase.h"

#include <iostream>
#include <curl/curl.h>
#include <cstring>
#include "util/json.h"

namespace rest {

struct string
{
	char *ptr;
	size_t len;
};

#define EXIT_FAILURE 1

void init_string(struct string *s)
{
	s->len = 0;
	s->ptr = (char *) malloc(s->len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
	size_t new_len = s->len + size*nmemb;
	s->ptr = (char *) realloc(s->ptr, new_len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr + s->len, ptr, size * nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

AbstractRestBase::AbstractRestBase()
{
}

AbstractRestBase::~AbstractRestBase()
{
}

json::Object AbstractRestBase::request(const std::string &method, const std::string &url, const std::string &token, const std::string &jsonStringPost)
{
	CURL *curl = curl_easy_init();

	std::string data;
	if (!curl)
		throw std::runtime_error("curl init failure");

	struct string s;
	init_string(&s);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
	if (method != "GET") {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
	}
	

	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, "Accept: application/json");
	chunk = curl_slist_append(chunk, "Content-Type: application/json");

	if (!token.empty())
		chunk = curl_slist_append(chunk, std::string(std::string("Token: ") + token).c_str());

	if (method != "GET" && jsonStringPost != "") {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStringPost.c_str());
	}
	
	CURLcode res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::string errorString = "curl_easy_perform() failed: ";
		errorString.append(curl_easy_strerror(res));
		throw std::runtime_error(errorString);
	}

	curl_easy_cleanup(curl);

	std::string jsonString(s.ptr, s.len);
	std::cout << " Received json: " << jsonString << std::endl;
	json::Object obj = json::Deserialize(jsonString);

	return obj;
}

}
