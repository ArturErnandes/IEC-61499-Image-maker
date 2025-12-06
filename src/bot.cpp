#include <tgbot/tgbot.h>

#include <cstdlib>      // getenv
#include <fstream>      // ofstream
#include <cctype>       // tolower
#include <iostream>
#include <filesystem>   // exists
#include <chrono>
#include <sstream>
#include <string_view>

#include "fb_parser.hpp"
#include "svg_renderer.hpp"
#include "png_renderer.hpp"

// ------------------ Вспомогательные функции ------------------

// Главное меню
void sendMainMenu(TgBot::Bot& bot, int64_t chatId) {
    std::string messageText =
        "Привет!\n"
        "Этот бот генерирует принципиальную схему функционального блока IEC-61499 "
        "на основе его XML файла";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
        btn->text = "Загрузить xml";
        btn->callbackData = "load_xml";
        row.push_back(btn);
        keyboard->inlineKeyboard.push_back(row);
    }

    {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
        btn->text = "Помощь";
        btn->callbackData = "help";
        row.push_back(btn);
        keyboard->inlineKeyboard.push_back(row);
    }

    {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
        btn->text = "О проекте";
        btn->callbackData = "project_about";
        row.push_back(btn);
        keyboard->inlineKeyboard.push_back(row);
    }

    bot.getApi().sendMessage(
        chatId,
        messageText,
        nullptr,              // linkPreviewOptions
        nullptr,              // replyParameters
        keyboard,
        "Markdown"
    );
}

void ensureImagesDir() {
    const std::filesystem::path dir = "images";
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
}

void sendResults(TgBot::Bot& bot, const std::string& pngPath, const std::string& svgPath, int64_t chatId) {
    if (std::filesystem::exists(pngPath)) {
        TgBot::InputFile::Ptr png = TgBot::InputFile::fromFile(pngPath, "image/png");
        bot.getApi().sendDocument(chatId, png);
    }

    if (std::filesystem::exists(svgPath)) {
        TgBot::InputFile::Ptr svg = TgBot::InputFile::fromFile(svgPath, "image/svg+xml");
        bot.getApi().sendDocument(chatId, svg);
    }

    bot.getApi().sendMessage(chatId,
        "Готово, вот финальные файлы принципиальной схемы функционального блока IEC-61499",
        nullptr, nullptr, nullptr, ""
    );

    sendMainMenu(bot, chatId);
}

// Генерация SVG/PNG по XML и отправка пользователю
void generateTrigger(TgBot::Bot& bot, const std::string& xmlPath, const std::string& baseName, int64_t chatId) {
    try {
        fbsvg::FBModel model = fbsvg::FBParser::parse_from_file(xmlPath);
        std::string svgContent = fbsvg::SVGRenderer::render(model);

        // Имена совпадают с исходным файлом (.fbt -> .svg/.png)
        ensureImagesDir();
        std::filesystem::path svgPath = std::filesystem::path("images") / (baseName + ".svg");
        std::filesystem::path pngPath = std::filesystem::path("images") / (baseName + ".png");

        {
            std::ofstream svgFile(svgPath, std::ios::binary);
            svgFile << svgContent;
        }

        fbsvg::PNGRenderer::render_from_svg(svgContent, pngPath);

        sendResults(bot, pngPath, svgPath, chatId);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка генерации: " << e.what() << std::endl;
        bot.getApi().sendMessage(chatId, std::string("Ошибка генерации: ") + e.what());
    }
}

// Сохранение XML, полученного от пользователя
void saveXml(TgBot::Bot& bot, TgBot::Message::Ptr message) {
    if (!message->document) {
        return;
    }

    int64_t chatId = message->chat->id;

    TgBot::File::Ptr fileInfo = bot.getApi().getFile(message->document->fileId);

    // Базовое имя без директорий и без расширения
    ensureImagesDir();

    std::filesystem::path originalName = std::filesystem::path(message->document->fileName).filename();
    std::string stem = originalName.stem().string();
    if (stem.empty()) {
        stem = "fb";
    }

    // Сохраняем входной файл в images/ с оригинальным именем
    std::filesystem::path localPath = std::filesystem::path("images") / (stem + ".fbt");

    const std::string content = bot.getApi().downloadFile(fileInfo->filePath);
    std::ofstream ofs(localPath, std::ios::binary);
    if (!ofs.is_open()) {
        bot.getApi().sendMessage(chatId, "Ошибка: не удалось сохранить файл.");
        return;
    }
    ofs.write(content.data(), static_cast<std::streamsize>(content.size()));
    ofs.close();

    generateTrigger(bot, localPath.string(), stem, chatId);
}

// ------------------ main ------------------

int main() {
    const char* tokenEnv = std::getenv("TOKEN");
    if (!tokenEnv) {
        std::cerr << "Ошибка: переменная окружения TOKEN не установлена.\n";
        return 1;
    }

    TgBot::Bot bot(tokenEnv);

    // /start
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        sendMainMenu(bot, message->chat->id);
    });

    // Кнопки
    bot.getEvents().onCallbackQuery([&bot](TgBot::CallbackQuery::Ptr query) {
        bot.getApi().answerCallbackQuery(query->id);

        int64_t chatId = query->message->chat->id;
        const std::string& data = query->data;

        if (data == "back_to_main") {
            sendMainMenu(bot, chatId);
        }
        else if (data == "load_xml") {
            TgBot::InlineKeyboardMarkup::Ptr kb(new TgBot::InlineKeyboardMarkup);
            TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
            btn->text = "Отмена";
            btn->callbackData = "back_to_main";
            kb->inlineKeyboard.push_back({btn});

            bot.getApi().sendMessage(
                chatId,
                "Пришли XML файл функционального блока:",
                nullptr, nullptr, kb
            );
        }
        else if (data == "help") {
            TgBot::InlineKeyboardMarkup::Ptr kb(new TgBot::InlineKeyboardMarkup);
            TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
            btn->text = "Назад";
            btn->callbackData = "back_to_main";
            kb->inlineKeyboard.push_back({btn});

            bot.getApi().sendMessage(chatId,
                "Инструкция:\n"
                "1. Нажмите \"Загрузить XML\"\n"
                "2. Отправьте XML файл (.fbt)\n"
                "3. Дождитесь генерации PNG и SVG\n\n"
                "По вопросам: @aToTheStars",
                nullptr, nullptr, kb
            );
        }
        else if (data == "project_about") {
            TgBot::InlineKeyboardMarkup::Ptr kb(new TgBot::InlineKeyboardMarkup);
            TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
            btn->text = "Назад";
            btn->callbackData = "back_to_main";
            kb->inlineKeyboard.push_back({btn});

            bot.getApi().sendMessage(chatId,
                "Бот создан в рамках курсовой работы.\n"
                "Разработчики:\n"
                "@aToTheStars\n"
                "@MangoWkusnoe",
                nullptr, nullptr, kb
            );
        }
    });

    // Приём XML файлов
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        if (!message->document) {
            return;
        }

        int64_t chatId = message->chat->id;
        std::string fileName = message->document->fileName;

        // Перевод в lowercase вручную
        std::string lower = fileName;
        for (char& c : lower) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        // Проверка расширения
        bool isFbt = false;
        if (lower.size() >= 4 && lower.substr(lower.size() - 4) == ".fbt") {
            isFbt = true;
        }

        if (isFbt) {
            bot.getApi().sendMessage(chatId, "Обработка файла...");
            saveXml(bot, message);
        } else {
            bot.getApi().sendMessage(chatId, "Неверный формат файла. Пришлите файл .fbt");
        }
    });

    // Запуск
    std::cout << "Бот запущен..." << std::endl;

    try {
        TgBot::TgLongPoll poll(bot);
        while (true) {
            poll.start();
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка long polling: " << e.what() << std::endl;
    }

    return 0;
}
