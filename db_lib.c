
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define DB_LIB_IMPLEMENTATION
#include "db_lib.h"


int main() {
    char *db = "company.db";

    // 1. إنشاء قاعدة البيانات (CREATE)
    // الأعمدة: المعرف (INT)، الاسم (TXT)، هل هو مدير (BOOL)
    printf("1. Creating Database...\n");
    create_db(db, "id INT, name TXT, is_manager BOOL");

    // 2. إضافة بيانات موظفين (INSERT/WRITE)
    printf("2. Inserting Records...\n");
    write_data(db, "1, Farouk, 1");   // مدير
    write_data(db, "2, Ahmed, 0");    // ليس مدير
    write_data(db, "3, Sara, 0");     // ليس مدير
    write_data(db, "4, Omar, 1");     // مدير

    // 3. قراءة كل البيانات (READ ALL)
    printf("\n3. Current Inventory:");
    read_all_data(db);

    // 4. البحث عن موظف معين (SEARCH)
    printf("\n4. Searching for 'Sara'...");
    search(db, "name = Sara");

    // 5. تحديث بيانات موظف (UPDATE)
    // سنقوم بترقية "Ahmed" ليصبح مديراً (تغيير الـ BOOL من 0 إلى 1)
    printf("\n5. Updating Ahmed to be a Manager...");
    update_data(db, "name = Ahmed", "2, Ahmed, 1");

    // 6. حذف موظف (DELETE)
    // سنقوم بحذف الموظف رقم 4
    printf("\n6. Deleting employee with ID 4...");
    delete_data(db, "id = 4");

    // 7. عرض النتيجة النهائية بعد كل التعديلات
    printf("\n7. Final Database State:");
    read_all_data(db);

    // 8. اختبار البحث الصامت (SILENT SEARCH)
    // هذا مفيد برمجياً لمعرفة عدد الموظفين المديرين حالياً
    struct SearchResult res = search(db, "SILENT is_manager = 1");
    printf("\n8. Statistic: Number of managers found: %d\n", res.count);
    
    // تنظيف الذاكرة الناتجة عن البحث الصامت
    if (res.indexes != NULL) free(res.indexes);

    printf("\n--- Test Completed Successfully ---\n");
    return 0;
}