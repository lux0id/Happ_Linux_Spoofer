# Happ Linux Spoofer

Библиотека для подмены системных идентификаторов (HWID, hostname) через `LD_PRELOAD`. Предназначена для работы с программой **Happ на Linux**.

---

## Сборка

```bash
gcc -fPIC -shared -o libfakesysinfo.so fakesysinfo.c -ldl -O2
```
## Запуск
```bash
# 1. Создайте файл с фейковым HWID
echo "00000000-0000-0000-0000-000000000000" > fake_machine_id

# 2. Запустите Happ с библиотекой
LD_PRELOAD=./libfakesysinfo.so happ

# Или через абсолютный путь
LD_PRELOAD=/путь/к/libfakesysinfo.so /путь/к/happ
```
## Настройка
Измените параметры в начале fakesysinfo.c перед сборкой:
```bash
#define HWID_FILE "fake_machine_id"        // Файл с фейковым HWID
#define FAKE_HOSTNAME "host-name"       // Подменяемое имя хоста
#define FAKE_OS_FILE "/tmp/.fake_hostname" // Временный файл
```
## Что подменяется
| Вызов | Что меняется |
| :--- | :---: |
| uname() | nodename → FAKE_HOSTNAME |
| open() / fopen() | /etc/machine-id → fake_machine_id |
| open() / fopen() | sys/class/dmi/id/product_uuid → fake_machine_id |
| open() / fopen() | /etc/hostname → FAKE_OS_FILE |

## Важно

    Архитектура: Библиотека должна соответствовать архитектуре Happ (обычно x86_64)
    Права: Файл fake_machine_id должен существовать до запуска
    LD_PRELOAD: Не работает с setuid-бинарниками (требуются права root)
    Happ версии: Протестировано на актуальных Linux-версиях
## Проверка
```bash
# Проверка hostname
LD_PRELOAD=./libfakesysinfo.so uname -n

# Проверка machine-id
LD_PRELOAD=./libfakesysinfo.so cat /etc/machine-id
```
## Лицензия
MIT. Используйте на свой страх и риск. Только для законных целей.
