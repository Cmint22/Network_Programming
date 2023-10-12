#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define USAGE "./resolver <domain or IP>"

int main(int argc, char *argv[]) {
  // Check for valid number of arguments.
  if (argc != 2) {
    fprintf(stderr, USAGE);
    exit(1);
  }
  // Get the domain name or IP address from the command line.
  char *parameter = argv[1];
  // Check if the parameter is a domain name.
  struct hostent *he;
  if ((he = gethostbyname(parameter)) != NULL) {
    // Get the official IP address.
    char *ip_address = inet_ntoa(*((struct in_addr *)he->h_addr_list[0]));
    // Display the official IP address and a list of searchable secondary IP addresses.
    printf("Official IP address: %s\n", ip_address);
    printf("Searchable secondary IP addresses:\n");
    for (int i = 1; he->h_addr_list[i] != NULL; i++) {
      ip_address = inet_ntoa(*((struct in_addr *)he->h_addr_list[i]));
      printf("%s\n", ip_address);
    }
  } else if ((he = gethostbyaddr(parameter, 4, AF_INET)) != NULL) {
    // Get the official domain name.
    char *domain_name = he->h_name;
    // Display the official domain name and how it can be resolved to this IP address.
    printf("Official domain name: %s\n", domain_name);
    printf("Resolved to IP address: %s\n", parameter);
  } else {
    printf("No information found\n");
  }
  return 0;
}
