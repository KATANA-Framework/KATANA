# Contributing

Спасибо за помощь в развитии KATANA! Сейчас репозиторий содержит только core (реактор, HTTP/1.1 парсер/ответы, арены, таймеры, TCP-хелперы). Добавляя фичи, держите документацию в актуальном состоянии и не обещайте то, чего нет в коде.

## Окружение

- Зависимости: CMake ≥ 3.20, Ninja, Clang ≥ 16 или GCC ≥ 12. Для io_uring-пресетов нужен `liburing-dev`.
- Конфигурация через `CMakePresets.json`:
  - Debug: `cmake --preset debug && cmake --build --preset debug`
  - Sanitizers: `--preset asan|tsan|ubsan`
  - io_uring: `--preset io_uring-debug` / `io_uring-release`
  - Примеры/бенчмарки: `--preset examples` / `bench`
- Тесты: `ctest --preset debug` (используется лёгкий gtest-совместимый харнес `test/gtest/gtest.h`).
- Fuzzing: `cmake --preset fuzz && cmake --build --preset fuzz && ./build/fuzz/test/fuzz/http_parser_fuzz -max_total_time=60`.

## Качество и стиль

- Форматирование: `clang-format` по `.clang-format`. Пользуйтесь `pre-commit install` (см. `.pre-commit-config.yaml`) перед коммитами.
- Статический анализ: `.clang-tidy` включён, после конфигурации можно запускать `clang-tidy -p build/debug <files>`.
- Код без необоснованных глобальных состояний и без сырого `new/delete`; RAII и `std::expected`/`monotonic_arena` предпочтительнее.

## Тестирование

- Unit/integration тесты находятся в `test/unit` и `test/integration`.
- Сейчас нет харнеса для изолированного тестирования HTTP/бизнес-хендлеров без запуска реактора и нет моков для async-кода — приветствуются фикстуры в `test/support` (stub сокеты, фейковые таймеры, event-loop drivers).
- При добавлении новых подсистем добавляйте fixture-based тесты, а для регрессий оставляйте минимальные воспроизведения.

## Документация

- README/ARCHITECTURE описывают целевую архитектуру. Если функциональность добавлена или, наоборот, ещё не реализована, фиксируйте это в соответствующих секциях.
- Описывайте новые CLI-флаги, параметры сборки и параметры тестов в README или отдельном файле в `docs/`.

## PR-Checklist

- [ ] `cmake --build --preset debug`
- [ ] `ctest --preset debug`
- [ ] `pre-commit run --all-files`
- [ ] Обновлены релевантные .md (особенно если меняется фактическая функциональность)
