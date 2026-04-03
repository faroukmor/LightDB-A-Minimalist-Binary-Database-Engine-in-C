#include "db_lib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// اسم قاعدة البيانات
const char *DB_NAME = "todo_list.db";

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// دالة تهيئة النظام لأول مرة
void init_system() {
    // سننشئ الجدول إذا لم يكن موجوداً
    // id: رقم المهمة، name: نص المهمة، done: الحالة (0 أو 1)
    create_db((char*)DB_NAME, "id INT, name TXT, done INT");
}

void Add_task_to_db(int id) {
    char name[50];
    printf("Enter task name: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = 0;

    // تجهيز الأمر للمكتبة: "ID, Name, Done"
    char instruction[100];
    sprintf(instruction, "%d, %s, 0", id, name);
    
    write_data((char*)DB_NAME, instruction);
    printf("Task added to Database.\n");
}

void Search_in_db() {
    int type;
    char target[50], query[100];
    printf("Search by (1.Id / 2.Name): ");
    scanf("%d", &type);
    clear_input_buffer();

    if (type == 1) {
        printf("Enter ID: ");
        scanf("%s", target);
        sprintf(query, "id = %s", target);
    } else {
        printf("Enter Name: ");
        scanf("%s", target);
        sprintf(query, "name LIKE %s", target);
    }
    
    printf("\n--- Search Results ---\n");
    search((char*)DB_NAME, query);
}

void Mark_as_done_db() {
    int id;
    printf("Enter task ID to mark as done: ");
    scanf("%d", &id);
    clear_input_buffer();

    char query[50], new_vals[100];
    sprintf(query, "id = %d", id);
    
    // هنا نحتاج لإعادة كتابة السجل كاملاً (كمثال بسيط)
    // ملاحظة: مكتبتك الحالية تحتاج للقيم الجديدة كاملة
    // سنفترض أننا نحدث الحالة إلى 1 (Done)
    // في تحديثات مكتبتك القادمة يمكنك جعل التحديث لعمود واحد فقط
    printf("Enter task name again to confirm update: ");
    char name[50];
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = 0;

    sprintf(new_vals, "%d, %s, 1", id, name);
    update_data((char*)DB_NAME, query, new_vals);
}

void Delete_from_db() {
    int id;
    printf("Enter task ID to delete: ");
    scanf("%d", &id);
    clear_input_buffer();

    char query[50];
    sprintf(query, "id = %d", id);
    delete_data((char*)DB_NAME, query);
}

int main() {
    int choice;
    static int current_id = 1; 
    
    // تهيئة قاعدة البيانات عند تشغيل البرنامج
    // (يمكنك فحص إذا كان الملف موجوداً أولاً، أو استدعاؤها يدوياً مرة واحدة)
    printf("Initialize system? (1: Yes / 0: Skip): ");
    scanf("%d", &choice);
    if(choice == 1) init_system();

    while (1) {
        printf("\n--- TODO LIST (DB VERSION) ---\n");
        read_all_data((char*)DB_NAME);
        
        printf("\n1. Add Task\n2. Search Task\n3. Mark Done\n4. Delete Task\n5. Exit\nChoice: ");
        scanf("%d", &choice);
        clear_input_buffer();

        switch (choice) {
            case 1:
                Add_task_to_db(current_id++);
                break;
            case 2:
                Search_in_db();
                break;
            case 3:
                Mark_as_done_db();
                break;
            case 4:
                Delete_from_db();
                break;
            case 5:
                printf("Exiting... All data is already safe in %s\n", DB_NAME);
                return 0;
            default:
                printf("Invalid choice!\n");
        }
    }
    return 0;
}