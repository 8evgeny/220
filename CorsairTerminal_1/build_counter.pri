# ==============================================================================
# Автоматический счетчик билдов для qmake (Qt)
# ==============================================================================

# Путь к файлу-хранилищу текущего номера сборки
BUILD_COUNTER_FILE = $$PWD/build_number.txt

# Если файла еще нет, инициализируем его нулем
!exists($$BUILD_COUNTER_FILE) {
    INIT_BUILD_NUMBER = 0
    write_file($$BUILD_COUNTER_FILE, INIT_BUILD_NUMBER)
}

# Загружаем текущее число из файла, увеличиваем на 1 и перезаписываем обратно.
# В qmake это делается штатными средствами, не ломая сборку в разных ОС:
CURRENT_BUILD_NUM = $$cat($$BUILD_COUNTER_FILE, blob)
NEXT_BUILD_NUM = $$num_add($$CURRENT_BUILD_NUM, 1)
write_file($$BUILD_COUNTER_FILE, NEXT_BUILD_NUM)

# Формируем содержимое будущего заголовочного файла version.h
VERSION_HEADER_CONTENT = \
    "$${LITERAL_HASH}ifndef VERSION_H" \
    "$${LITERAL_HASH}define VERSION_H" \
    "" \
    "$${LITERAL_HASH}define BUILD_NUMBER \"$$NEXT_BUILD_NUM\"" \
    "" \
    "$${LITERAL_HASH}endif // VERSION_H"

# Записываем заголовок прямо сейчас (чтобы IDE не ругалась на отсутствие файла)
VERSION_HEADER_PATH = $$PWD/src/version.h
write_file($$VERSION_HEADER_PATH, VERSION_HEADER_CONTENT)

# Добавляем путь к сгенерированному файлу в проект,
# чтобы его можно было инклудить как #include "version.h" без указания путей
DEPENDPATH += $$PWD/src

# Кастомная цель для Make, чтобы файл обновлялся при КАЖДОЙ сборке,
# если в проекте изменился хоть один исходник.
update_build_header.target = $$VERSION_HEADER_PATH
update_build_header.commands = @echo "Incrementing build number to $$NEXT_BUILD_NUM..."
win32:update_build_header.commands = @echo Incrementing build number to $$NEXT_BUILD_NUM...

QMAKE_EXTRA_TARGETS += update_build_header
PRE_TARGETDEPS += $$VERSION_HEADER_PATH