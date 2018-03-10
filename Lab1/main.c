#include <stdio.h>
#include <stdlib.h>
#include "StaticArray.h"
#include <string.h>

//
//void debug_for_dynamic_pointer(){
//    array_structure* new_object = create_array(10,sizeof(char)*10);
//    print_array(new_object);
//    new_object = remove_array_element(new_object,7);
//    printf("\n");
//    print_array(new_object);
//    new_object = add_to_array(new_object,"New Element");
//    printf("\n");
//    print_array(new_object);
//    printf("\n%d\n",get_ascii_sum("New Element"));
//    printf("\n%s\n",search_for_closest_ascii_sum(new_object,7));
//}


int main() {
    array *some_array = create_static_array(10);
    for(int i=0;i<10;i++){
        printf("%d.%s \n",i,some_array[i]);
    }
    return 0;
}