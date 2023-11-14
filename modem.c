// modem.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define num_networks 10

typedef enum {kData, kWifi, kUnknown} MEDIUM;

typedef struct network {
 char network_name[20]; // change network_name from a pointer to a string
 int signal_strength;
 MEDIUM connection_medium;
 bool password_saved;
} network;

struct network cached_networks[num_networks];

void ChooseNetwork(char* network) {
  printf("Network chosen: %s\n", network);
}

MEDIUM ConvertIntToMedium(int int_medium) {
  if (int_medium == 1) {
    return kData;
  } else if (int_medium == 2) {
    return kWifi;
  } else {
    return kUnknown;
  }
}

/**
  * TODO: This function is buggy and not complete
  *
  * We should first fix this function and then work on fixing ScanNetworksV2().
  * The fixes found in this function will help determine how to fix V2.
  */
void ScanNetworks(FILE *fp, char *connection_type) {
  char temp_name[20], password_saved[10];
  int i = 0, medium;

  while (fscanf(fp, "%s %d %d %s", temp_name, &medium, &cached_networks[i].signal_strength,
                                   password_saved) != EOF) { // loop until EOF is reached


    if(strcmp(connection_type, "wifi") == 0 && ConvertIntToMedium(medium) != 1) {
      continue; // if looking for wifi and network is not a wifi connection, skip
    }
    else if (strcmp(connection_type, "data") == 0 && ConvertIntToMedium(medium) != 0) {
      continue; // if looking for data and network is not a data connection, skip
    }

    strcpy(cached_networks[i].network_name, temp_name); // save network name
    cached_networks[i].connection_medium = ConvertIntToMedium(medium); // convert and save medium

    if(strcmp(password_saved, "true") == 0) { // convert bool string to a bool value
      cached_networks[i].password_saved = true;
    }
    else {
      cached_networks[i].password_saved = false;
    }

    printf("Network name: %s\n", cached_networks[i].network_name); // print network name
    i++;
  }
}

/**
  * This function early-exits from networks that we don't already have access
  * to. This way we can still scan for 5 networks, but we won't display/attempt
  * to make a decision vs networks we don't have the password for.
  *
  * TODO: This function is buggy and not complete
  */
void ScanNetworksV2(FILE *fp, char *connection_type) {

  char temp_name[20], password_saved[10];
  int i = 0, medium, temp_signal_strength;

  while (fscanf(fp, "%s %d %d %s", temp_name, &medium, &temp_signal_strength,
     password_saved) != EOF) { // loop until EOF is reached

    if(strcmp(password_saved, "true") == 0) { // convert bool string to a bool value
        cached_networks[i].password_saved = true; // save password
    }
    else { // skip network if we do not have the password
        continue;
    }
    
    if(strcmp(connection_type, "wifi") == 0 && ConvertIntToMedium(medium) != 1) {
      continue; // if looking for wifi and network is not a wifi connection, skip
    }
    else if (strcmp(connection_type, "data") == 0 && ConvertIntToMedium(medium) != 0) {
      continue; // if looking for data and network is not a data connection, skip
    }

    strcpy(cached_networks[i].network_name, temp_name); // save network name
    cached_networks[i].signal_strength = temp_signal_strength; // save signal strength
    cached_networks[i].connection_medium = ConvertIntToMedium(medium); // convert and save medium

    printf("Network name: %s\n", cached_networks[i].network_name); // print network name
    i++;
  }
}


void ScanNetworksV3() {
  // If you were to iterate and modify ScanNetworksV2 to be even better,
  // what would you do? You DO NOT need to write any code, you can just
  // explain what you would do or write psuedocode.
  /*
  To improve upon ScanNetworksV2, I would dynamically allocate an array of cached networks
  because I noticed that since we are only caching networks that we have the password for, 
  there is some is a lot of memory left unused in the statically defined cached_networks array. To do 
  this, I would use malloc to dynamically allocate enough memory for the name, signal strength, 
  medium, and password whenever another element is to be added to the cached_networks array.
  */
}

/**
  * This function selects what network we should connect to based on certain
  * criteria.
  *
  * 1. We should only choose networks that we can connect to
  * 2. We should only connect to networks based on what criteria we want
  *    (i.e., Wi-Fi, Data, either).
  * 3. We should pick the network with the highest signal strength
  *
  * criteria    String denoting "wifi", "data", or "greedy"
  * return      String of best network_name
  */
char* DetermineNetwork(char* criteria) {
  int maxSignalStrength = 0;
  char* best_network_name = NULL;
  for(int i = 0; i < num_networks;i++){ // iterate through networks
    if(cached_networks[i].password_saved == 1){ // if we have network password
      if(strcmp(criteria, "wifi") == 0 && cached_networks[i].connection_medium == kWifi){ // if we want wifi and network is wifi
        if(cached_networks[i].signal_strength > maxSignalStrength){ // if signal strength is higher than max
          best_network_name = cached_networks[i].network_name;
          maxSignalStrength = cached_networks[i].signal_strength;
        }  
      }
      else if(strcmp(criteria, "data") == 0 && cached_networks[i].connection_medium == kData){ // if we want data and network is data
        if(cached_networks[i].signal_strength > maxSignalStrength){ // if signal strength is higher than max
          best_network_name = cached_networks[i].network_name;
          maxSignalStrength = cached_networks[i].signal_strength;
        }
      }
      else if(strcmp(criteria, "greedy") == 0){ // looking for either data or wifi
        if(cached_networks[i].signal_strength > maxSignalStrength){ // if signal strength is higher than max
          best_network_name = cached_networks[i].network_name;
          maxSignalStrength = cached_networks[i].signal_strength;
        }
      }
    }
  }
  return best_network_name;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Incorrect command argument. Please pass in wifi, data, or greedy");
    return 1;
  }

  FILE *fp;

  fp = fopen("experiment_data", "r");

  printf("Starting up modem...\n");
  printf("Scanning nearby network connections...\n");
  ScanNetworksV2(fp, argv[1]);
  printf("Networks cached. Now determining network to connect...\n");
  printf("Connection Medium Criteria: %s\n", argv[1]);
  ChooseNetwork(DetermineNetwork(argv[1]));

  fclose(fp);

  return 0;
}
