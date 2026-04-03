# 🚀 LightDB: A Minimalist Binary Database Engine in C

**LightDB** is a lightweight, single-header binary database engine designed for C developers who need a fast, reliable, and simple way to store structured data without the overhead of heavy SQL engines.

---

## 🌟 Key Features

* **Single-Header Magic:** Just one file (`db_lib.h`) to manage your entire data layer.
* **True Binary Storage:** Data is stored in a compact binary format, ensuring high performance and small disk footprint.
* **Dynamic Schemas:** Define your tables and columns (INT, TXT, BOOL) on the fly.
* **Full CRUD Support:** * **Create:** Initialize new database files with custom schemas.
    * **Read:** Selective column reading or full table scans.
    * **Update:** Precision record updates using a built-in search engine.
    * **Delete:** Smart file rebuilding to physically remove data.
* **Silent Search:** Programmatically find record indexes for advanced data processing.

---

## 🛠️ Installation & Setup

To use **LightDB** in your project, simply copy `db_lib.h` to your project folder. 

In **exactly one** C source file (e.g., `db_lib.c` or your `main.c`), define the implementation macro before including the header:

```c
#define DB_LIB_IMPLEMENTATION
#include "db_lib.h"
