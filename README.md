---
Title : Module MODULE_NAME
Author: al-martyn1
---

# Использование шаблона модуля

Просто клонируете данный проект, заменяете origin на адрес своего репозитория, и пушите его туда.

Можно настроить использование данного проекта в качестве шаблона для создания репозитория (на github, например).

В `CMakeLists.txt` обычно досточно просто поменять `MODULE_NAME` на имя вашего модуля.
                                                                
```
project(MODULE_NAME CXX)
```

Некоторые нюансы создания более сложных модулей описаны уже непосредственно в `CMakeLists.txt`.

Если модуль не является частью какого-то другого, удалите каталог `inc`.

Подключение модуля в `CMakeLists.txt` проекте:

```
# ...
add_subdirectory(${LIB_ROOT}/MODULE_NAME)
# ...
set(COMMON_LIBS [...] MODULE_NAME::MODULE_NAME)
# ...
target_link_libraries(YOUR_EXE PRIVATE [...] "${COMMON_LIBS}")

```
