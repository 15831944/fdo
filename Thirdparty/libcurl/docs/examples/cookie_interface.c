/*****************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 *  This example shows usage of simple cookie interface. 
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <curl/curl.h>

static void
print_cookies(CURL *curl)
{
  CURLcode res;
  struct curl_slist *cookies;
  struct curl_slist *nc;
  int i;

  printf("Cookies, curl knows:\n");
  res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
  if (res != CURLE_OK) {
    fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n", curl_easy_strerror(res));
    exit(1);
  }
  nc = cookies, i = 1;
  while (nc) {
    printf("[%d]: %s\n", i, nc->data);
    nc = nc->next;
    i++;
  }
  if (i == 1) {
    printf("(none)\n");
  }
  curl_slist_free_all(cookies);
}

int
main(void)
{
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (curl) {
    char nline[256];

    curl_easy_setopt(curl, CURLOPT_URL, "http://www.google.com/"); /* google.com sets "PREF" cookie */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* just to start the cookie engine */
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
      return 1;
    }

    print_cookies(curl);

    printf("Erasing curl's knowledge of cookies!\n");
    curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");

    print_cookies(curl);

    printf("-----------------------------------------------\n"
           "Setting a cookie \"PREF\" via cookie interface:\n");
#ifdef WIN32
#define snprintf _snprintf
#endif
    /* Netscape format cookie */
    snprintf(nline, 256, "%s\t%s\t%s\t%s\t%u\t%s\t%s",
      ".google.com", "TRUE", "/", "FALSE", time(NULL) + 31337, "PREF", "hello google, i like you very much!");
    res = curl_easy_setopt(curl, CURLOPT_COOKIELIST, nline);
    if (res != CURLE_OK) {
      fprintf(stderr, "Curl curl_easy_setopt failed: %s\n", curl_easy_strerror(res));
      return 1;            
    }

    /* HTTP-header style cookie */
    snprintf(nline, 256,
      "Set-Cookie: OLD_PREF=3d141414bf4209321; "
      "expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; domain=.google.com");
    res = curl_easy_setopt(curl, CURLOPT_COOKIELIST, nline);
    if (res != CURLE_OK) {
      fprintf(stderr, "Curl curl_easy_setopt failed: %s\n", curl_easy_strerror(res));
      return 1;            
    }

    print_cookies(curl);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
      return 1;
    }
  }
  else {
    fprintf(stderr, "Curl init failed!\n");
    return 1;
  }

  curl_global_cleanup();
  return 0;
}
