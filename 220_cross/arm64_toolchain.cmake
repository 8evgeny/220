set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Компиляторы
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Пути
set(SYSROOT_PATH /home/user/sysroot_arm64_firefly)
set(QT_ARM_PATH /home/user/Qt/6.11.3_arm/gcc_arm64)
set(QT_HOST_PATH /home/user/Qt/6.11.3/gcc_64)

# 1. Основные настройки путей
set(CMAKE_SYSROOT ${SYSROOT_PATH})
set(CMAKE_FIND_ROOT_PATH ${SYSROOT_PATH} ${QT_ARM_PATH})
set(CMAKE_PREFIX_PATH ${QT_ARM_PATH})

# 2. Добавляем путь к модулям Qt, чтобы WrapOpenGL был найден
list(APPEND CMAKE_MODULE_PATH ${QT_ARM_PATH}/lib/cmake/Qt6)

# 3. Хост-путь для Qt6 (обязательно в кэш)
set(QT_HOST_PATH ${QT_HOST_PATH} CACHE PATH "Host Qt path")

# 4. Режимы поиска
# Программы ищем только на хосте (x86)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Библиотеки и заголовки ищем ТОЛЬКО в sysroot и Qt ARM
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# Пакеты ищем в обоих местах (чтобы Qt нашел свои зависимости)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
