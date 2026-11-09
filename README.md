# Multithreaded HTTP server (C++20)

## Вариант №3. Пул на `packaged_task`. 
Пул, принимающий произвольные задачи и возвращающий `std::future`. Обработка запроса разбита на этапы (парсинг, обработка, сериализация). Обязательные примитивы: `std::packaged_task`, `std::future`, `std::promise`.

## Команда
Антонов Александр Сергеевич: Настройка CMake, CI/CD, реализация пула потоков на packaged_task
Трифонов Александр Александрович: Реализация HTTP-парсера, синхронизация хранилища, написание тестов

## Сборка
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j