#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>

//Group stores data on a group
typedef struct Group {
  unsigned int members;
  unsigned int max_names;
  char** names;
} Group;

Group** process_input(FILE *fp, int* next_group);
int get_FP_ID(char** fingerprints, Group** groups, char* this_print,
	      int* next_group, int* group_limit);
void push_groups(Group** groups, int group_count);

// The main program gets the groups then pushes them out
int main(int argc, char** argv) {
  if ((argc > 1) || (stdin == NULL)) exit(1);
  (void)argv;
  int group_count = 0;
  Group** groups = process_input(stdin, &group_count);
  push_groups(groups, group_count);
}

// process_input takes the input and organizes it by finger print
Group** process_input(FILE *fp, int* next_group) {
  int group_limit = 256;
  char** fingerprints = (char**)calloc(group_limit, sizeof(char*));
  Group** groups = (Group**)calloc(group_limit, sizeof(Group*));
  char* buffer = (char*)calloc(1028, sizeof(char));
  int id = 0, format = 0;
  
  // continue reading till the input is empty
  while (!(feof(fp))) {
    // read first character
    format = 0;
    buffer[format] = fgetc(fp);
    // Check if whitespace, then check EOF
    while (!(isspace(buffer[format]) || feof(fp)))
      buffer[++format] = fgetc(fp);
    // if format is correct (nonzero), close fingerprint and get id 
    if (format) {
      buffer[format] = '\0';
      id = get_FP_ID(fingerprints, groups, buffer, next_group, &group_limit);
    }
    // read rest of line, store in buffer, if incorrect then ignore
    if ((fgets(buffer, 256, fp) == NULL) || (!format)) {
      if (!feof(fp)) fprintf(stderr, "Unusable line ignored");
      continue;
    }
    // Save new name
    groups[id]->names[groups[id]->members] =
      (char*)calloc(strlen(buffer)+1, sizeof(char));
    strcpy(groups[id]->names[groups[id]->members++], buffer);
    // if the name array is full, double the length
    if (groups[id]->members == groups[id]->max_names) {
      groups[id]->max_names *= 2;
      groups[id]->names =
        (char**)realloc(groups[id]->names,
                        groups[id]->max_names*sizeof(char*));
    }
  }
  // delete each print in fingerprints
  for (id = 0; id < *next_group; id++) free(fingerprints[id]);
  // delete fingerprint array
  free(fingerprints);
  // delete buffer
  free(buffer);
  return groups;
}  

// ge_FP_ID gets the id of the fingerprint and makes one if it is new
int get_FP_ID(char** fingerprints, Group** groups,  char* this_print,
	      int* next_group, int* group_limit) {
  //return if fingerprint id found
  int id = -1;
  //check id
  while (++id < *next_group)
    if (!strcmp(fingerprints[id], this_print)) return id;
  // otherwise create a new group
  groups[id] = (Group*)malloc(sizeof(Group));
  // create new name array
  groups[id]->max_names = 32;
  groups[id]->names = (char**)calloc(groups[id]->max_names, sizeof(char*));
  groups[id]->members = 0;  
  // create fingerprint
  fingerprints[id] =
    (char*)calloc(strlen(this_print)+1, sizeof(char));
  strcpy(fingerprints[id], this_print);
  *next_group += 1;
  // if our fingprint and group arrays are full, double their size
  if (id == *group_limit) {
    *group_limit *= 2;
    groups = (Group**)realloc(groups, *group_limit*sizeof(Group*)+1);
    fingerprints = (char**)realloc(groups, *group_limit*sizeof(char*)+1);
  }

  return id;
}

// push_groups pushes all groups out of memory and all groups with multiple
//  members to stdout
void push_groups(Group** groups, int group_count) {
  int id = 0;
  int last_group = -1;
  unsigned int name_index = 0;
  while (id < group_count) {
    name_index = 0;
    // if one member, we just free the name
    if (groups[id]->members == 1) {
      free(groups[id]->names[name_index]);
      free(groups[id]->names);
      free(groups[id++]);
    } else {
      // to avoid the final newline, push the last group with multiple members
      if (last_group > -1) {
        while (name_index < groups[last_group]->members) {
	  printf("%s", groups[last_group]->names[name_index]);
          free(groups[last_group]->names[name_index++]);
        }
	printf("\n");
        free(groups[last_group]->names);
        free(groups[last_group]);
      }
      last_group = id++;
    }
  }
  // There are no more groups, push last group with multiple members
  name_index = 0;
  while (name_index < groups[last_group]->members) {
    printf("%s", groups[last_group]->names[name_index]);
    free(groups[last_group]->names[name_index++]);
  }
  free(groups[last_group]->names);
  free(groups[last_group]);
  free(groups);
}
